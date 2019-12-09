/*
    Proteus -- High-performance query processing on heterogeneous hardware.

                            Copyright (c) 2017
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

//#define NUM_TPCH_QUERIES 1
#define NUM_OLAP_REPEAT 16
//#define HTAP true

#include <gflags/gflags.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <thread>

// HTAP
#include "htap-cli-flags.hpp"
#include "queries/ch/ch-queries.hpp"

// OLAP includes
#include <common/olap-common.hpp>

#include "memory/memory-manager.hpp"
#include "plan/catalog-parser.hpp"
#include "plan/prepared-statement.hpp"
#include "storage/storage-manager.hpp"
#include "util/jit/pipeline.hpp"
#include "util/parallel-context.hpp"
#include "util/profiling.hpp"
#include "util/timing.hpp"

// OLTP
//#include "cli-flags.hpp"
#include "glo.hpp"
#include "interfaces/bench.hpp"
#include "queries/olap-sequence.hpp"
#include "scheduler/affinity_manager.hpp"
#include "scheduler/comm_manager.hpp"
#include "scheduler/topology.hpp"
#include "scheduler/worker.hpp"
#include "storage/column_store.hpp"
#include "storage/memory_manager.hpp"
#include "storage/table.hpp"
#include "topology/affinity_manager.hpp"
#include "tpcc_64.hpp"
#include "ycsb.hpp"

void init_olap_warmup() { proteus::olap::init(); }

void shutdown_olap() {
  StorageManager::unloadAll();
  MemoryManager::destroy();
  LOG(INFO) << "OLAP Shutdown complete";
}

void init_oltp(uint num_workers, std::string csv_path) {
  // TODO: # warehouse for csv should will be variable.

  bench::Benchmark *bench = nullptr;

  if (csv_path.length() < 2) {
    bench = new bench::TPCC("TPCC", num_workers, num_workers,
                            storage::COLUMN_STORE, FLAGS_ch_scale_factor);

  } else {
    bench =
        new bench::TPCC("TPCC", num_workers, num_workers, FLAGS_ch_scale_factor,
                        storage::COLUMN_STORE, 0, csv_path);
  }

  scheduler::WorkerPool::getInstance().init(bench, num_workers, 1, 3);
}

void run_oltp(const scheduler::cpunumanode &numa_node) {
  scheduler::WorkerPool::getInstance().start_workers();
}

void shutdown_oltp(bool print_stat = true) {
  scheduler::WorkerPool::getInstance().shutdown(print_stat);
  storage::Schema::getInstance().teardown();
  LOG(INFO) << "OLTP Shutdown complete";
}

void snapshot_oltp() { txn::TransactionManager::getInstance().snapshot(); }

void fly_olap(int i, OLAPSequence &olap_queries,
              const topology::cpunumanode &node) {
  LOG(INFO) << "[SERVER-COW] OLAP Client #" << i << ": Running OLAP Sequence";
  olap_queries.run(i, node, FLAGS_num_olap_repeat);
}

int main(int argc, char *argv[]) {
  // assert(HTAP_DOUBLE_MASTER && !HTAP_COW && "wrong snapshot mode in oltp");
  if (FLAGS_etl) {
    assert(HTAP_ETL && "ETL MODE NOT ON");
  }

  gflags::SetUsageMessage("Simple command line interface for proteus");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = 1;  // FIXME: the command line flags/defs seem to fail...
  g_num_partitions = 1;
  google::InstallFailureSignalHandler();

  FLAGS_num_olap_repeat =
      (NUM_OLAP_REPEAT / FLAGS_num_olap_clients);  // warmmup

  init_olap_warmup();

  if (FLAGS_num_oltp_clients == 0) {
    FLAGS_num_oltp_clients =
        topology::getInstance().getCpuNumaNodes()[0].local_cores.size();
  }

  std::cout << "QUERIES_PER_SESSION: " << (FLAGS_num_olap_repeat) << std::endl;
  std::cout << "OLAP Clients: " << FLAGS_num_olap_clients << std::endl;

  // google::InstallFailureSignalHandler();

  // set_trace_allocations(FLAGS_trace_allocations);

  const auto &txn_topo = scheduler::Topology::getInstance();
  const auto &txn_nodes = txn_topo.getCpuNumaNodes();
  // init_oltp(txn_nodes[0].local_cores.size(), "");
  init_oltp(FLAGS_num_oltp_clients, "");
  storage::Schema::getInstance().report();

  // OLAP INIT

  const auto &topo = topology::getInstance();
  const auto &nodes = topo.getCpuNumaNodes();

  auto OLAP_SOCKET = 0;
  auto OLTP_SOCKET = 1;

  exec_location{nodes[OLAP_SOCKET]}.activate();

  {
    time_block t("T_FIRST_SNAPSHOT_ETL_: ");
    snapshot_oltp();
    if (FLAGS_etl) storage::Schema::getInstance().ETL(OLAP_SOCKET);
  }

  int client_id = 1;
  OLAPSequence olap_queries(OLAPSequence::wrapper_t<AeolusLocalPlugin>{},
                            client_id, nodes[OLAP_SOCKET], nodes[OLTP_SOCKET]);

  profiling::resume();
  if (FLAGS_run_oltp && FLAGS_num_oltp_clients > 0)
    run_oltp(txn_nodes[OLTP_SOCKET]);

  usleep(2000000);

  scheduler::WorkerPool::getInstance().print_worker_stats_diff();
  if (FLAGS_elastic > 0) {
    if (!FLAGS_trade_core) {
      std::cout << "Scale-down OLTP" << std::endl;
      scheduler::WorkerPool::getInstance().scale_down(FLAGS_elastic);
    } else {
      for (uint i = 0; i < FLAGS_elastic; i++) {
        scheduler::WorkerPool::getInstance().migrate_worker();
      }
    }
  }
  usleep(2000000);

  for (int i = 0; i < FLAGS_num_olap_clients; i++) {
    scheduler::WorkerPool::getInstance().print_worker_stats_diff();

    std::cout << "Snapshot Request" << std::endl;
    snapshot_oltp();
    std::cout << "Snapshot Done" << std::endl;
    if (FLAGS_etl) {
      time_block t("T_ETL_: ");
      storage::Schema::getInstance().ETL(OLAP_SOCKET);
    }
    scheduler::WorkerPool::getInstance().print_worker_stats_diff();

    {
      time_block t("T_fly_olap_: ");
      fly_olap(i, olap_queries, nodes[OLAP_SOCKET]);
    }
    std::cout << "exited: " << i << std::endl;
  }

  scheduler::WorkerPool::getInstance().print_worker_stats_diff();
  scheduler::WorkerPool::getInstance().print_worker_stats();
  std::cout << "[Master] Shutting down everything" << std::endl;

  if (!FLAGS_run_oltp && FLAGS_num_oltp_clients > 0) {
    // FIXME: hack because it needs to run before it can be stopped
    run_oltp(txn_nodes[OLTP_SOCKET]);
  }

  exit(0);

  shutdown_oltp(true);
  shutdown_olap();

  return 0;
}
