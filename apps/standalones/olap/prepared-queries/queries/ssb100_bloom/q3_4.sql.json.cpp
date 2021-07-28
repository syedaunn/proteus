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

// AUTOGENERATED FILE. DO NOT EDIT.

constexpr auto query = "ssb100_Q3_4";

#include "query.cpp.inc"

PreparedStatement Query::prepare34(bool memmv, size_t bloomSize) {
  auto rel44853 =
      getBuilder<Tplugin>()
          .scan(
              "inputs/ssbm100/date.csv", {"d_datekey", "d_year", "d_yearmonth"},
              getCatalog(),
              pg{Tplugin::
                     type})  // (table=[[SSB, ssbm_date]], fields=[[0, 4, 6]],
                             // traits=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .membrdcst(dop, true, true)
          .router(
              [&](const auto &arg) -> expression_t {
                return arg["__broadcastTarget"];
              },
              dop, 1, dev,
              aff_parallel())  // (trait=[Pelago.[].packed.X86_64.homBrdcst.hetSingle])
          .to_gpu()  // (trait=[Pelago.[].packed.NVPTX.homBrdcst.hetSingle])
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle])
          .filter([&](const auto &arg) -> expression_t {
            return eq(arg["d_yearmonth"], "Dec1997");
          })
          .project([&](const auto &arg) -> std::vector<expression_t> {
            return {arg["d_datekey"], arg["d_year"]};
          });
  auto rel44857 =
      getBuilder<Tplugin>()
          .scan(
              "inputs/ssbm100/customer.csv", {"c_custkey", "c_city"},
              getCatalog(),
              pg{Tplugin::
                     type})  // (table=[[SSB, ssbm_customer]], fields=[[0, 3]],
                             // traits=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .membrdcst(dop, true, true)
          .router(
              [&](const auto &arg) -> expression_t {
                return arg["__broadcastTarget"];
              },
              dop, 1, dev,
              aff_parallel())  // (trait=[Pelago.[].packed.X86_64.homBrdcst.hetSingle])
          .to_gpu()  // (trait=[Pelago.[].packed.NVPTX.homBrdcst.hetSingle])
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle])
          .filter([&](const auto &arg) -> expression_t {
            return (eq(arg["c_city"], "UNITED KI1") |
                    eq(arg["c_city"], "UNITED KI5"));
          })  // (condition=[OR(=($1, 'UNITED KI1'), =($1, 'UNITED KI5'))],
              // trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle],
              // isS=[false])
      ;
  auto rel44861 =
      getBuilder<Tplugin>()
          .scan(
              "inputs/ssbm100/supplier.csv", {"s_suppkey", "s_city"},
              getCatalog(),
              pg{Tplugin::
                     type})  // (table=[[SSB, ssbm_supplier]], fields=[[0, 3]],
                             // traits=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .membrdcst(dop, true, true)
          .router(
              [&](const auto &arg) -> expression_t {
                return arg["__broadcastTarget"];
              },
              dop, 128, dev,
              aff_parallel())  // (trait=[Pelago.[].packed.X86_64.homBrdcst.hetSingle])
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle])
          .filter([&](const auto &arg) -> expression_t {
            return (eq(arg["s_city"], "UNITED KI1") |
                    eq(arg["s_city"], "UNITED KI5"));
          })
          .bloomfilter_build(
              [&](const auto &arg) -> expression_t { return arg["s_suppkey"]; },
              bloomSize, 1)
          .pack()
          .to_gpu()
          .unpack();
  auto rel =
      getBuilder<Tplugin>()
          .scan("inputs/ssbm100/lineorder.csv",
                {"lo_custkey", "lo_suppkey", "lo_orderdate", "lo_revenue"},
                getCatalog(), pg{"block"})
          .router(8, RoutingPolicy::LOCAL, DeviceType::CPU)
          //          .unpack()
          .bloomfilter_repack(
              [&](const auto &arg) { return arg["lo_suppkey"]; }, bloomSize, 1)
          //          .pack()
          .router(8, RoutingPolicy::LOCAL, dev);

  if (memmv) rel = rel.memmove(8, dev);

  rel =
      rel.to_gpu()   // (trait=[Pelago.[].packed.NVPTX.homRandom.hetSingle])
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homRandom.hetSingle])
          .join(
              rel44853,
              [&](const auto &build_arg) -> expression_t {
                return build_arg["$0"];
              },
              [&](const auto &probe_arg) -> expression_t {
                return probe_arg["lo_orderdate"];
              },
              6, 32)
          .join(
              rel44857,
              [&](const auto &build_arg) -> expression_t {
                return build_arg["$0"];
              },
              [&](const auto &probe_arg) -> expression_t {
                return probe_arg["lo_custkey"];
              },
              16,
              32768)  // (condition=[=($2, $0)], joinType=[inner],
                      // rowcnt=[2.4576E7], maxrow=[3000000.0],
                      // maxEst=[3000000.0], h_bits=[27],
                      // build=[RecordType(INTEGER c_custkey, VARCHAR
                      // c_city)], lcount=[8.70502961749499E8],
                      // rcount=[2.45775624192E9], buildcountrow=[2.4576E7],
                      // probecountrow=[2.45775624192E9])
          .join(
              rel44861,
              [&](const auto &build_arg) -> expression_t {
                return build_arg["$0"];
              },
              [&](const auto &probe_arg) -> expression_t {
                return probe_arg["lo_suppkey"];
              },
              13,
              2048)  // (condition=[=($3, $0)], joinType=[inner],
                     // rowcnt=[1638400.0], maxrow=[200000.0],
                     // maxEst=[200000.0], h_bits=[23],
                     // build=[RecordType(INTEGER s_suppkey, VARCHAR s_city)],
                     // lcount=[4.915979188432821E7],
                     // rcount=[3.0721953024E11], buildcountrow=[1638400.0],
                     // probecountrow=[3.0721953024E11])
          .groupby(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {arg["c_city"].as("PelagoAggregate#44871", "$0"),
                        arg["s_city"].as("PelagoAggregate#44871", "$1"),
                        arg["d_year"].as("PelagoAggregate#44871", "$2")};
              },
              [&](const auto &arg) -> std::vector<GpuAggrMatExpr> {
                return {GpuAggrMatExpr{
                    (arg["lo_revenue"]).as("PelagoAggregate#44871", "$3"), 1, 0,
                    SUM}};
              },
              10,
              131072)  // (group=[{0, 1, 2}], lo_revenue=[SUM($3)],
                       // trait=[Pelago.[].unpckd.NVPTX.homRandom.hetSingle])
          .pack()      // (trait=[Pelago.[].packed.NVPTX.homRandom.hetSingle],
                       // intrait=[Pelago.[].unpckd.NVPTX.homRandom.hetSingle],
          // inputRows=[23407.202304], cost=[{1.6016 rows, 1.6 cpu, 0.0
          // io}])
          .to_cpu()  // (trait=[Pelago.[].packed.X86_64.homRandom.hetSingle])
          .router(
              DegreeOfParallelism{1}, 128, RoutingPolicy::RANDOM,
              DeviceType::CPU,
              aff_reduce())  // (trait=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .memmove(8, DeviceType::CPU)
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homSingle.hetSingle])
          .groupby(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {arg["$0"].as("PelagoAggregate#44877", "c_city"),
                        arg["$1"].as("PelagoAggregate#44877", "s_city"),
                        arg["$2"].as("PelagoAggregate#44877", "d_year")};
              },
              [&](const auto &arg) -> std::vector<GpuAggrMatExpr> {
                return {GpuAggrMatExpr{
                    (arg["$3"]).as("PelagoAggregate#44877", "lo_revenue"), 1, 0,
                    SUM}};
              },
              10,
              131072)  // (group=[{0, 1, 2}], lo_revenue=[SUM($3)],
                       // trait=[Pelago.[].unpckd.NVPTX.homSingle.hetSingle])
          .sort(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {arg["c_city"], arg["s_city"], arg["d_year"],
                        arg["lo_revenue"]};
              },
              {direction::NONE, direction::NONE, direction::ASC,
               direction::DESC})
          .print(pg{"pm-csv"});
  return rel.prepare();
}

PreparedStatement Query::prepare34_b(bool memmv, size_t bloomSize) {
  auto rel =
      getBuilder<Tplugin>()
          .scan("inputs/ssbm100/lineorder.csv",
                {"lo_custkey", "lo_suppkey", "lo_orderdate", "lo_revenue"},
                getCatalog(), pg{"block"})  // (table=[[SSB, ssbm_lineorder]],
                                            // fields=[[2, 4, 5, 12]],
          // traits=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .router(8, RoutingPolicy::LOCAL, DeviceType::CPU)
          .unpack()
          .bloomfilter_probe([&](const auto &arg) { return arg["lo_suppkey"]; },
                             bloomSize, 1)
          .reduce(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {expression_t{int64_t{1}}.as("tmp", "cnt")};
              },
              {SUM})
          .router(
              DegreeOfParallelism{1}, 128, RoutingPolicy::RANDOM,
              DeviceType::
                  CPU)  // (trait=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .reduce(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {arg["cnt"]};
              },
              {SUM})
          .print(pg("pm-csv"));

  return rel.prepare();
}
