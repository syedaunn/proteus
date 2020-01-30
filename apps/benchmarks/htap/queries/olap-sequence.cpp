/*
    Proteus -- High-performance query processing on heterogeneous hardware.

                            Copyright (c) 2019
        Data Intensive Applications and Systems Laboratory (DIAS)
                École Polytechnique Fédérale de Lausanne

                            All Rights Reserved.

    Permission to use, copy, modify and distribute this software and
    its documentation is hereby granted, provided that both the
    copyright notice and this permission notice appear in all copies of
    the software, derivative works or modified versions, and any
    portions thereof, and that both notices appear in supporting
    documentation.

    This code is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. THE AUTHORS
    DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER
    RESULTING FROM THE USE OF THIS SOFTWARE.
*/

#include "olap-sequence.hpp"

#include <aeolus-plugin.hpp>
#include <ch100w/query.hpp>
#include <numeric>
#include <routing/affinitizers.hpp>
#include <routing/degree-of-parallelism.hpp>
#include <topology/affinity_manager.hpp>
#include <topology/topology.hpp>
#include <util/timing.hpp>

#include "../htap-cli-flags.hpp"
#include "ch/ch-queries.hpp"

#define WARMUP_QUERIES 2
// FIXME: remove following
#define ETL_AFFINITY_SOCKET 0

constexpr auto num_ch_q = 4;
template <typename plugin_t, typename aff_t, typename red_t>
constexpr auto CH_queries = {Q<1>::prepare<plugin_t, aff_t, red_t>,
                             Q<4>::prepare<plugin_t, aff_t, red_t>,
                             Q<6>::prepare<plugin_t, aff_t, red_t>,
                             Q<19>::prepare<plugin_t, aff_t, red_t>};

// auto q1_rel = {std::make_pair(
//     "tpcc_orderline", std::vector<std::string>{"ol_delivery_d", "ol_number",
//                                                "ol_amount", "ol_quantity"})};
// auto q4_rel = {
//     std::make_pair("tpcc_order",
//                    std::vector<std::string>{"o_id", "o_d_id", "o_w_id",
//                                             "o_entry_d", "o_ol_cnt"}),
//     std::make_pair("tpcc_orderline",
//                    std::vector<std::string>{"ol_o_id", "ol_d_id", "ol_w_id",
//                                             "ol_delivery_d"})};
// auto q6_rel = {std::make_pair(
//     "tpcc_orderline",
//     std::vector<std::string>{"ol_delivery_d", "ol_quantity", "ol_amount"})};
// auto q19_rel = {
//     std::make_pair("tpcc_item", std::vector<std::string>{"i_id", "i_price"}),
//     std::make_pair("tpcc_orderline",
//                    std::vector<std::string>{"ol_w_id", "ol_i_id",
//                    "ol_quantity",
//                                             "ol_amount"})};

const auto q1_rel = std::vector<std::string>{"tpcc_orderline"};
const auto q4_rel = std::vector<std::string>{"tpcc_order", "tpcc_orderline"};
const auto q6_rel = std::vector<std::string>{"tpcc_orderline"};
const auto q19_rel = std::vector<std::string>{"tpcc_item", "tpcc_orderline"};

auto ch_relations =
    std::vector<std::vector<std::string>>{q1_rel, q4_rel, q6_rel, q19_rel};

auto OLAPSequence::getIsolatedOLAPResources() {
  std::vector<SpecificCpuCoreAffinitizer::coreid_t> coreids;
  for (auto &olap_n : conf.olap_nodes) {
    for (auto id :
         (dynamic_cast<topology::cpunumanode *>(olap_n))->local_cores) {
      coreids.emplace_back(id);
    }
  }
  return coreids;
}

auto OLAPSequence::getColocatedResources() {
  std::vector<SpecificCpuCoreAffinitizer::coreid_t> coreids;
  int j = 0;
  for (auto &olap_n : conf.olap_nodes) {
    for (auto id :
         (dynamic_cast<topology::cpunumanode *>(olap_n))->local_cores) {
      if (j < conf.collocated_worker_threshold) {
        j++;
        continue;
      }

      coreids.emplace_back(id);
    }
  }

  int k = 0;
  for (auto &oltp_n : conf.oltp_nodes) {
    for (auto id :
         (dynamic_cast<topology::cpunumanode *>(oltp_n))->local_cores) {
      if (k >= conf.collocated_worker_threshold) {
        break;
      }
      coreids.emplace_back(id);
      k++;
    }

    if (k >= conf.collocated_worker_threshold) {
      break;
    }
  }
  assert(k == conf.collocated_worker_threshold);
  return coreids;
}

auto OLAPSequence::getElasticResources() {
  std::vector<SpecificCpuCoreAffinitizer::coreid_t> coreids;

  for (auto &olap_n : conf.olap_nodes) {
    for (auto id :
         (dynamic_cast<topology::cpunumanode *>(olap_n))->local_cores) {
      coreids.emplace_back(id);
    }
  }

  int k = 0;
  for (auto &oltp_n : conf.oltp_nodes) {
    for (auto id :
         (dynamic_cast<topology::cpunumanode *>(oltp_n))->local_cores) {
      if (k >= conf.oltp_scale_threshold) {
        break;
      }
      coreids.emplace_back(id);
      k++;
    }

    if (k >= conf.oltp_scale_threshold) {
      break;
    }
  }
  assert(k == conf.oltp_scale_threshold);

  return coreids;
}

void OLAPSequence::setupAdaptiveSequence() {
  auto resources_colocated = getColocatedResources();
  auto resources_isolated = getIsolatedOLAPResources();
  auto resources_elastic = getElasticResources();

  DegreeOfParallelism colocated_dop{resources_colocated.size()};
  DegreeOfParallelism isolated_dop{resources_isolated.size()};
  DegreeOfParallelism elastic_dop{resources_elastic.size()};

  // ---- Queries -----
  // S1_COLOCATED

  {
    auto aff_parallel = [&]() -> std::unique_ptr<Affinitizer> {
      return std::make_unique<SpecificCpuCoreAffinitizer>(resources_colocated);
    };

    auto aff_reduce = []() -> std::unique_ptr<Affinitizer> {
      return std::make_unique<CpuCoreAffinitizer>();
    };

    typedef decltype(aff_parallel) aff_t;
    typedef decltype(aff_reduce) red_t;

    for (const auto &q : CH_queries<AeolusRemotePlugin, aff_t, red_t>) {
      stmts.emplace_back(
          q(colocated_dop, aff_parallel, aff_reduce, DeviceType::CPU));
      total_queries++;
    }
  }

  // S2_ISOLATED

  {
    auto aff_parallel = [&]() -> std::unique_ptr<Affinitizer> {
      return std::make_unique<SpecificCpuCoreAffinitizer>(resources_isolated);
    };

    auto aff_reduce = []() -> std::unique_ptr<Affinitizer> {
      return std::make_unique<CpuCoreAffinitizer>();
    };

    typedef decltype(aff_parallel) aff_t;
    typedef decltype(aff_reduce) red_t;

    for (const auto &q : CH_queries<AeolusLocalPlugin, aff_t, red_t>) {
      stmts.emplace_back(
          q(isolated_dop, aff_parallel, aff_reduce, DeviceType::CPU));
    }
  }

  // S3_IS

  {
    auto aff_parallel = [&]() -> std::unique_ptr<Affinitizer> {
      return std::make_unique<SpecificCpuCoreAffinitizer>(resources_isolated);
    };

    auto aff_reduce = []() -> std::unique_ptr<Affinitizer> {
      return std::make_unique<CpuCoreAffinitizer>();
    };

    typedef decltype(aff_parallel) aff_t;
    typedef decltype(aff_reduce) red_t;

    for (const auto &q : CH_queries<AeolusElasticPlugin, aff_t, red_t>) {
      stmts.emplace_back(
          q(isolated_dop, aff_parallel, aff_reduce, DeviceType::CPU));
    }
  }

  // S3_NI

  {
    auto aff_parallel = [&]() -> std::unique_ptr<Affinitizer> {
      return std::make_unique<SpecificCpuCoreAffinitizer>(resources_elastic);
    };

    auto aff_reduce = []() -> std::unique_ptr<Affinitizer> {
      return std::make_unique<CpuCoreAffinitizer>();
    };

    typedef decltype(aff_parallel) aff_t;
    typedef decltype(aff_reduce) red_t;

    for (const auto &q : CH_queries<AeolusElasticNIPlugin, aff_t, red_t>) {
      stmts.emplace_back(
          q(isolated_dop, aff_parallel, aff_reduce, DeviceType::CPU));
    }
  }

  return;
}

OLAPSequence::OLAPSequence(int client_id, HTAPSequenceConfig conf,
                           DeviceType dev)
    : conf(conf), total_queries(0), client_id(client_id) {
  if (dev == DeviceType::GPU &&
      conf.schedule_policy != SchedulingPolicy::S1_COLOCATED) {
    assert(false && "GPU OLAP supports isolated scheduling mode only");
  }

  if (conf.schedule_policy == SchedulingPolicy::ADAPTIVE) {
    setupAdaptiveSequence();
    return;
  }

  std::vector<SpecificCpuCoreAffinitizer::coreid_t> coreids;

  // Resource Scheduling
  if (dev == DeviceType::CPU) {
    if (conf.schedule_policy == SchedulingPolicy::S2_ISOLATED ||
        conf.schedule_policy == SchedulingPolicy::S3_IS ||
        conf.resource_policy == SchedulingPolicy::ISOLATED) {
      coreids = getIsolatedOLAPResources();
    } else if (conf.schedule_policy == SchedulingPolicy::S1_COLOCATED ||
               conf.resource_policy == SchedulingPolicy::COLOCATED) {
      coreids = getColocatedResources();

    } else if (conf.schedule_policy == SchedulingPolicy::S3_NI ||
               conf.resource_policy == SchedulingPolicy::ELASTIC) {
      assert(conf.oltp_scale_threshold > 0);
      coreids = getElasticResources();
    }
  }

  DegreeOfParallelism dop{(dev == DeviceType::CPU) ? coreids.size()
                                                   : conf.olap_nodes.size()};

  auto aff_parallel = [&]() -> std::unique_ptr<Affinitizer> {
    if (dev == DeviceType::CPU) {
      return std::make_unique<SpecificCpuCoreAffinitizer>(coreids);
    } else {
      return std::make_unique<GPUAffinitizer>();
    }
  };

  auto aff_reduce = []() -> std::unique_ptr<Affinitizer> {
    return std::make_unique<CpuCoreAffinitizer>();
  };

  typedef decltype(aff_parallel) aff_t;
  typedef decltype(aff_reduce) red_t;

  // Data Access Methods

  if (conf.data_access_policy == SchedulingPolicy::REMOTE_READ ||
      conf.schedule_policy == SchedulingPolicy::S1_COLOCATED) {
    for (const auto &q : CH_queries<AeolusRemotePlugin, aff_t, red_t>) {
      stmts.emplace_back(q(dop, aff_parallel, aff_reduce, dev));
      total_queries++;
    }

  } else if (conf.data_access_policy == SchedulingPolicy::LOCAL_READ ||
             conf.schedule_policy == SchedulingPolicy::S2_ISOLATED) {
    for (const auto &q : CH_queries<AeolusLocalPlugin, aff_t, red_t>) {
      stmts.emplace_back(q(dop, aff_parallel, aff_reduce, dev));
      total_queries++;
    }

  } else if (conf.data_access_policy == SchedulingPolicy::HYBRID_READ ||
             conf.schedule_policy == SchedulingPolicy::S3_IS ||
             conf.schedule_policy == SchedulingPolicy::S3_NI) {
    for (const auto &q : CH_queries<AeolusElasticPlugin, aff_t, red_t>) {
      stmts.emplace_back(q(dop, aff_parallel, aff_reduce, dev));
      total_queries++;
    }

  } else {
    assert(false && "Unsupported data access plugin");
  }
}

std::ostream &operator<<(std::ostream &out, const OLAPSequence &seq) {
  out << "OLAP Stats" << std::endl;
  for (const auto &r : seq.stats) {
    out << "Sequence Run # " << r->run_id << std::endl;

    // per query avg time
    // std::vector<double> q_avg_time(r->sts.size(), 0.0);
    uint i = 0;
    for (const auto &q : r->sts) {
      out << "\t\tQuery # " << (i + 1) << " -";
      for (const auto &q_ts : q) {
        // q_avg_time[i] += q_ts;

        out << "\t" << q_ts.second;
      }
      out << std::endl;
      // q_avg_time[i] /= q.size();
      // out << "\t\tQuery (Avg) # " << (i + 1) << " - " << q_avg_time[i] << "
      // ms"
      //     << std::endl;
      i++;
    }

    // average sequence time
    // double average = (double)(std::accumulate(r->sequence_time.begin(),
    //                                           r->sequence_time.end(), 0.0))
    //                                           /
    //                  ((double)r->sequence_time.size());

    // out << "\t\tQuery Sequence - " << average << " ms" << std::endl;

    out << "\tSequence Time "
        << " -";
    for (const auto &st : r->sequence_time) {
      out << "\t" << st.second;
    }
    out << std::endl;
  }
  return out;
}

std::ostream &operator<<(std::ostream &out, const OLAPSequenceStats &r) {
  out << "Sequence Run # " << r.run_id << std::endl;

  uint ts = 0;
  for (const auto &q : r.sts) {
    out << "\t\t\tTime -";
    for (const auto &q_ts : q) {
      out << "\t" << q_ts.first;
    }
    out << std::endl;
    ts++;
  }

  uint t = 0;
  for (const auto &q : r.oltp_stats) {
    out << "\t\t\tOLTP -";
    for (const auto &q_ts : q) {
      out << "\t" << q_ts.second;
    }
    out << std::endl;
    t++;
  }

  uint i = 0;
  for (const auto &q : r.sts) {
    out << "\t\tQuery # " << (i + 1) << " -";
    for (const auto &q_ts : q) {
      out << "\t" << q_ts.second;
    }
    out << std::endl;
    i++;
  }

  uint k = 0;
  for (const auto &q : r.sts_state) {
    out << "\t\tQState # " << (k + 1) << " -";
    for (const auto &q_ts : q) {
      out << "\t" << q_ts;
    }
    out << std::endl;
    k++;
  }

  out << "\tSequence Time "
      << " -";
  for (const auto &st : r.sequence_time) {
    out << "\t" << st.first;
  }
  out << std::endl;

  out << "\tSequence Query Time "
      << " -";
  for (const auto &st : r.sequence_time) {
    out << "\t" << st.second;
  }
  out << std::endl;

  return out;
}

HTAPSequenceConfig::HTAPSequenceConfig(
    exec_nodes olap_nodes, exec_nodes oltp_nodes, uint oltp_scale_threshold,
    uint collocated_worker_threshold,
    SchedulingPolicy::ScheduleMode schedule_policy)
    : olap_nodes(olap_nodes),
      oltp_nodes(oltp_nodes),
      oltp_scale_threshold(oltp_scale_threshold),
      collocated_worker_threshold(collocated_worker_threshold),
      schedule_policy(schedule_policy) {
  switch (schedule_policy) {
    case SchedulingPolicy::S1_COLOCATED:
      resource_policy = SchedulingPolicy::COLOCATED;
      data_access_policy = SchedulingPolicy::REMOTE_READ;
      break;
    case SchedulingPolicy::S2_ISOLATED:
      resource_policy = SchedulingPolicy::ISOLATED;
      data_access_policy = SchedulingPolicy::LOCAL_READ;
      break;
    case SchedulingPolicy::S3_IS:
      resource_policy = SchedulingPolicy::ISOLATED;
      data_access_policy = SchedulingPolicy::HYBRID_READ;
      break;
    case SchedulingPolicy::S3_NI:
      resource_policy = SchedulingPolicy::ELASTIC;
      data_access_policy = SchedulingPolicy::HYBRID_READ;
      break;
    case SchedulingPolicy::ADAPTIVE:
    default:
      break;
  }
}
HTAPSequenceConfig::HTAPSequenceConfig(
    exec_nodes olap_nodes, exec_nodes oltp_nodes, uint oltp_scale_threshold,
    uint collocated_worker_threshold,
    SchedulingPolicy::ResourceSchedule resource_policy,
    SchedulingPolicy::DataAccess data_access_policy)
    : olap_nodes(olap_nodes),
      oltp_nodes(oltp_nodes),
      oltp_scale_threshold(oltp_scale_threshold),
      collocated_worker_threshold(collocated_worker_threshold),
      resource_policy(resource_policy),
      data_access_policy(data_access_policy) {
  // set the schedule policy accordingly.
  switch (resource_policy) {
    case SchedulingPolicy::COLOCATED: {
      switch (data_access_policy) {
        case SchedulingPolicy::REMOTE_READ:
          schedule_policy = SchedulingPolicy::S1_COLOCATED;
          break;
        case SchedulingPolicy::HYBRID_READ:
        case SchedulingPolicy::LOCAL_READ:
        default:
          schedule_policy = SchedulingPolicy::CUSTOM;
          break;
      }
      break;
    }
    case SchedulingPolicy::ELASTIC: {
      assert(oltp_scale_threshold != 0 &&
             "Cannot have elastic OLAP with non-elastic OLTP!");
      switch (data_access_policy) {
        case SchedulingPolicy::HYBRID_READ:
          schedule_policy = SchedulingPolicy::S3_NI;
          break;
        case SchedulingPolicy::REMOTE_READ:
        case SchedulingPolicy::LOCAL_READ:
        default:
          schedule_policy = SchedulingPolicy::CUSTOM;
          break;
      }
      break;
    }
    case SchedulingPolicy::ISOLATED: {
      switch (data_access_policy) {
        case SchedulingPolicy::LOCAL_READ:
          schedule_policy = SchedulingPolicy::S2_ISOLATED;
          break;
        case SchedulingPolicy::HYBRID_READ:
          schedule_policy = SchedulingPolicy::S3_IS;
          break;
        case SchedulingPolicy::REMOTE_READ:
        default:
          schedule_policy = SchedulingPolicy::CUSTOM;
          break;
      }
      break;
    }
    default:
      schedule_policy = SchedulingPolicy::CUSTOM;
      break;
  }
}

void OLAPSequence::migrateState(SchedulingPolicy::ScheduleMode &curr,
                                SchedulingPolicy::ScheduleMode to,
                                OLTP &txn_engine) {
  // S1_COLOCATED, S2_ISOLATED, S3_IS, S3_NI
  if (curr == to) return;

  LOG(INFO) << "Migrating from state-" << curr << " to state-" << to;
  if (curr == SchedulingPolicy::S2_ISOLATED && to == SchedulingPolicy::S3_IS) {
    curr = to;
    return;
  }

  time_block t("T_HTAPstateMigration: ");

  // from isolated
  if ((curr == SchedulingPolicy::S2_ISOLATED ||
       curr == SchedulingPolicy::S3_IS) &&
      (to == SchedulingPolicy::S3_NI || to == SchedulingPolicy::S1_COLOCATED)) {
    if (to == SchedulingPolicy::S1_COLOCATED) {
      // migrate oltp worker
      txn_engine.migrate_worker(conf.collocated_worker_threshold);

    } else {
      // scale down oltp
      txn_engine.scale_down(conf.oltp_scale_threshold);
    }
  }
  // to isolated
  else if ((curr == SchedulingPolicy::S3_NI ||
            curr == SchedulingPolicy::S1_COLOCATED) &&
           (to == SchedulingPolicy::S2_ISOLATED ||
            to == SchedulingPolicy::S3_IS)) {
    if (curr == SchedulingPolicy::S1_COLOCATED) {
      // migrate back
      txn_engine.migrate_worker(conf.collocated_worker_threshold);
    } else {
      // scale-up oltp
      txn_engine.scale_up(conf.oltp_scale_threshold);
    }
  } else {
    // missing NI <-> COLOCATED
    LOG(INFO) << "Current State: " << curr;
    LOG(INFO) << "Next State: " << to;
    assert(false && "How come?");
  }

  curr = to;
}

SchedulingPolicy::ScheduleMode OLAPSequence::getNextState(
    SchedulingPolicy::ScheduleMode current_state, OLTP &txn_engine,
    size_t &query_idx) {
  // txn_engine.getFreshnessRatio();
  // getFreshnessRatioRelation(std::string table_name)

  // Need to capture information about the query,
  // somehow pass the metadata of query in this function, so that we know
  // exactly what this query will touch and what are the exact requirements.

  /*
  The ratio of fresh data to the overall amount of data that the query will
  access is given by Rf q , whereas the ratio of fresh data that the query will
  access to the overall number of fresh data is given by Rf t .
  */

  // BUG: account for number of columns in query and the datatype.
  // For the fresh tuples, account for the updates else inserts will be same no
  // matter what.

  auto db_f = txn_engine.getFreshness();
  auto query_f =
      txn_engine.getFreshnessRelation(ch_relations[(query_idx % num_ch_q)]);

  auto query_fresh_data = query_f.second - query_f.first;
  auto total_fresh_data = db_f.second - db_f.first;

  // R_fq
  double r_fq = ((double)query_fresh_data) / ((double)query_f.second);
  double r_ft = ((double)query_fresh_data) / ((double)total_fresh_data);

  if (r_fq < r_ft) {
    if (conf.oltp_scale_threshold <= 0) {
      return SchedulingPolicy::S3_IS;
    } else {
      return SchedulingPolicy::S3_NI;
    }

  } else {
    return SchedulingPolicy::S2_ISOLATED;
  }
}

static inline size_t time_diff(timepoint_t end, timepoint_t start) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
      .count();
}

void OLAPSequence::execute(OLTP &txn_engine, int repeat,
                           bool per_query_snapshot, size_t etl_interval_ms) {
  assert(conf.schedule_policy != SchedulingPolicy::CUSTOM &&
         "Not supported currently");

  SchedulingPolicy::ScheduleMode current_state = SchedulingPolicy::S2_ISOLATED;
  static uint run = 0;
  OLAPSequenceStats *stats_local =
      new OLAPSequenceStats(++run, stmts.size(), repeat);

  timepoint_t last_etl = std::chrono::system_clock::now() -
                         std::chrono::milliseconds(etl_interval_ms);

  // txn_engine.snapshot();
  // if (current_state != SchedulingPolicy::S1_COLOCATED) {
  //   txn_engine.etl(ETL_AFFINITY_SOCKET);
  //   last_etl = std::chrono::system_clock::now();
  // }

  // warm-up
  LOG(INFO) << "Warm-up execution - BEGIN";
  for (uint i = 0; i < WARMUP_QUERIES; i++) {
    stmts[i % stmts.size()].execute();
  }
  LOG(INFO) << "Warm-up execution - END";

  // sequence
  LOG(INFO) << "Exeucting OLAP Sequence[Client # " << this->client_id << "]";

  if (conf.schedule_policy != SchedulingPolicy::ADAPTIVE)
    migrateState(current_state, conf.schedule_policy, txn_engine);

  timepoint_t global_start = std::chrono::system_clock::now();

  for (int i = 0; i < repeat; i++) {
    LOG(INFO) << "Sequence # " << i;
    timepoint_t start_seq = std::chrono::system_clock::now();
    // add time based ETL here.

    if (!per_query_snapshot) {
      txn_engine.snapshot();
      if (current_state == SchedulingPolicy::S2_ISOLATED ||
          (conf.schedule_policy != SchedulingPolicy::S1_COLOCATED &&
           time_diff(std::chrono::system_clock::now(), last_etl) >=
               etl_interval_ms)) {
        txn_engine.etl(ETL_AFFINITY_SOCKET);
        last_etl = std::chrono::system_clock::now();
      }
    }
    for (size_t j = 0; j < total_queries; j++) {
      // update the snapshot
      if (per_query_snapshot) {
        txn_engine.snapshot();

        if (conf.schedule_policy != SchedulingPolicy::S2_ISOLATED ||
            conf.schedule_policy != SchedulingPolicy::S1_COLOCATED) {
          if (time_diff(std::chrono::system_clock::now(), last_etl) >=
              etl_interval_ms) {
            LOG(INFO) << "ETL_TIME:";
            txn_engine.etl(ETL_AFFINITY_SOCKET);
            last_etl = std::chrono::system_clock::now();
          }
        }
      }

      // migrate state according to freshness depending on the freshness of each
      // snapshots (olap and otlp)
      if (conf.schedule_policy == SchedulingPolicy::ADAPTIVE)
        migrateState(current_state, getNextState(current_state, txn_engine, j),
                     txn_engine);

      if (current_state == SchedulingPolicy::S2_ISOLATED) {
        // have to do it anyway
        LOG(INFO) << "ETL_STATE:";
        txn_engine.etl(ETL_AFFINITY_SOCKET);
        last_etl = std::chrono::system_clock::now();
      }

      // Query Execution

      timepoint_t start = std::chrono::system_clock::now();

      if (conf.schedule_policy == SchedulingPolicy::ADAPTIVE)
        // execute apprpriate query
        LOG(INFO) << stmts[j * conf.schedule_policy].execute();
      else
        LOG(INFO) << stmts[j].execute();
      timepoint_t end = std::chrono::system_clock::now();

      stats_local->oltp_stats[j].push_back(
          txn_engine.get_differential_stats(true));

      stats_local->sts[j].push_back(
          std::make_pair(time_diff(end, global_start), time_diff(end, start)));
      stats_local->sts_state[j].push_back(current_state);
    }

    // End sequence
    timepoint_t end_seq = std::chrono::system_clock::now();
    stats_local->sequence_time.push_back(std::make_pair(
        time_diff(end_seq, global_start), time_diff(end_seq, start_seq)));
  }

  LOG(INFO) << "Stats-Local:";
  LOG(INFO) << *stats_local;
  LOG(INFO) << "Stats-Local-END";
  stats.push_back(stats_local);

  LOG(INFO) << "Exeucting  Sequence - END";
  txn_engine.print_storage_stats();
}
