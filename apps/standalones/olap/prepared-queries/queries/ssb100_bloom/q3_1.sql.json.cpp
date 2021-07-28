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

constexpr auto query = "ssb100_Q3_1";

#include "query.cpp.inc"

PreparedStatement Query::prepare31(bool memmv, size_t bloomSize) {
  auto rel29287 =
      getBuilder<Tplugin>()
          .scan(
              "inputs/ssbm100/date.csv", {"d_datekey", "d_year"}, getCatalog(),
              pg{Tplugin::
                     type})  // (table=[[SSB, ssbm_date]], fields=[[0, 4]],
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
            return (ge(arg["d_year"], 1992) & le(arg["d_year"], 1997));
          })  // (condition=[AND(>=($1, 1992), <=($1, 1997))],
              // trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle],
              // isS=[false])
      ;
  auto rel29292 =
      getBuilder<Tplugin>()
          .scan(
              "inputs/ssbm100/customer.csv",
              {"c_custkey", "c_nation", "c_region"}, getCatalog(),
              pg{Tplugin::
                     type})  // (table=[[SSB, ssbm_customer]], fields=[[0, 4,
                             // 5]],
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
            return eq(arg["c_region"], "ASIA");
          })
          .project([&](const auto &arg) -> std::vector<expression_t> {
            return {arg["c_custkey"], arg["c_nation"]};
          });
  auto rel29297 =
      getBuilder<Tplugin>()
          .scan("inputs/ssbm100/supplier.csv",
                {"s_suppkey", "s_nation", "s_region"}, getCatalog(),
                pg{"block"})
          .membrdcst(dop, true, true)
          .router(
              [&](const auto &arg) -> expression_t {
                return arg["__broadcastTarget"];
              },
              dop, 128, dev,
              aff_parallel())  // (trait=[Pelago.[].packed.X86_64.homBrdcst.hetSingle])
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle])
          .filter([&](const auto &arg) -> expression_t {
            return eq(arg["s_region"], "ASIA");
          })
          .bloomfilter_build(
              [&](const auto &arg) -> expression_t { return arg["s_suppkey"]; },
              bloomSize, 1)
          .project([&](const auto &arg) -> std::vector<expression_t> {
            return {arg["s_suppkey"], arg["s_nation"]};
          })
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
              rel29292,
              [&](const auto &build_arg) -> expression_t {
                return build_arg["c_custkey"];
              },
              [&](const auto &probe_arg) -> expression_t {
                return probe_arg["lo_custkey"];
              },
              20, 1048576)
          .join(
              rel29297,
              [&](const auto &build_arg) -> expression_t {
                return build_arg["s_suppkey"];
              },
              [&](const auto &probe_arg) -> expression_t {
                return probe_arg["lo_suppkey"];
              },
              16,
              65536)  // (condition=[=($3, $0)], joinType=[inner],
                      // rowcnt=[4.096E7], maxrow=[200000.0],
                      // maxEst=[200000.0], h_bits=[28],
                      // build=[RecordType(INTEGER s_suppkey, VARCHAR
                      // s_nation)], lcount=[1.4926851046814082E9],
                      // rcount=[3.0721953024E11], buildcountrow=[4.096E7],
                      // probecountrow=[3.0721953024E11])
          .join(
              rel29287,
              [&](const auto &build_arg) -> expression_t {
                return build_arg["d_datekey"];
              },
              [&](const auto &probe_arg) -> expression_t {
                return probe_arg["lo_orderdate"];
              },
              14,
              2556)  // (condition=[=($2, $0)], joinType=[inner],
                     // rowcnt=[654336.0], maxrow=[2556.0], maxEst=[2556.0],
                     // h_bits=[22], build=[RecordType(INTEGER d_datekey,
                     // INTEGER d_year)], lcount=[1.8432021459974352E7],
                     // rcount=[1.22887812096E10], buildcountrow=[654336.0],
                     // probecountrow=[1.22887812096E10])
          .groupby(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {arg["c_nation"].as("PelagoAggregate#29307", "c_nation"),
                        arg["s_nation"].as("PelagoAggregate#29307", "s_nation"),
                        arg["d_year"].as("PelagoAggregate#29307", "d_year")};
              },
              [&](const auto &arg) -> std::vector<GpuAggrMatExpr> {
                return {GpuAggrMatExpr{
                    (arg["lo_revenue"])
                        .as("PelagoAggregate#29307", "lo_revenue"),
                    1, 0, SUM}};
              },
              10,
              131072)  // (group=[{0, 1, 2}], lo_revenue=[SUM($3)],
                       // trait=[Pelago.[].unpckd.NVPTX.homRandom.hetSingle])
          .pack()      // (trait=[Pelago.[].packed.NVPTX.homRandom.hetSingle],
                       // intrait=[Pelago.[].unpckd.NVPTX.homRandom.hetSingle],
          // inputRows=[3.0721953024E8], cost=[{469.26880000000006 rows,
          // 468.8 cpu, 0.0 io}])
          .to_cpu()  // (trait=[Pelago.[].packed.X86_64.homRandom.hetSingle])
          .router(
              DegreeOfParallelism{1}, 128, RoutingPolicy::RANDOM,
              DeviceType::
                  CPU)  // (trait=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .memmove(8, DeviceType::CPU)
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homSingle.hetSingle])
          .groupby(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {arg["c_nation"].as("PelagoAggregate#29313", "c_nation"),
                        arg["s_nation"].as("PelagoAggregate#29313", "s_nation"),
                        arg["d_year"].as("PelagoAggregate#29313", "d_year")};
              },
              [&](const auto &arg) -> std::vector<GpuAggrMatExpr> {
                return {GpuAggrMatExpr{
                    (arg["lo_revenue"])
                        .as("PelagoAggregate#29313", "lo_revenue"),
                    1, 0, SUM}};
              },
              10,
              131072)  // (group=[{0, 1, 2}], lo_revenue=[SUM($3)],
                       // trait=[Pelago.[].unpckd.NVPTX.homSingle.hetSingle])
          .sort(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {arg["c_nation"], arg["s_nation"], arg["d_year"],
                        arg["lo_revenue"]};
              },
              {direction::NONE, direction::NONE, direction::ASC,
               direction::DESC})  // (sort0=[$2], sort1=[$3], dir0=[ASC],
                                  // dir1=[DESC], trait=[Pelago.[2, 3
                                  // DESC].unpckd.X86_64.homSingle.hetSingle])
          .print(pg("pm-csv"));
  return rel.prepare();
}

PreparedStatement Query::prepare31_b(bool memmv, size_t bloomSize) {
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
