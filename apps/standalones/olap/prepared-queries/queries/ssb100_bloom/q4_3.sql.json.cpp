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

constexpr auto query = "ssb100_Q4_3";

#include "query.cpp.inc"

PreparedStatement Query::prepare43(bool memmv, size_t bloomSize) {
  auto rel64283 =
      getBuilder<Tplugin>()
          .scan<Tplugin>(
              "inputs/ssbm100/date.csv", {"d_datekey", "d_year"},
              getCatalog())  // (table=[[SSB, ssbm_date]], fields=[[0, 4]],
                             // traits=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .membrdcst(dop, true, true)
          .router(
              [&](const auto &arg) -> std::optional<expression_t> {
                return arg["__broadcastTarget"];
              },
              dop, 1, RoutingPolicy::HASH_BASED, dev,
              aff_parallel())  // (trait=[Pelago.[].packed.X86_64.homBrdcst.hetSingle])
          .to_gpu()  // (trait=[Pelago.[].packed.NVPTX.homBrdcst.hetSingle])
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle])
          .filter([&](const auto &arg) -> expression_t {
            return (eq(arg["d_year"], 1997) | eq(arg["d_year"], 1998));
          })  // (condition=[OR(=($1, 1997), =($1, 1998))],
              // trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle],
              // isS=[false])
      ;
  auto rel64288 =
      getBuilder<Tplugin>()
          .scan<Tplugin>(
              "inputs/ssbm100/customer.csv", {"c_custkey", "c_region"},
              getCatalog())  // (table=[[SSB, ssbm_customer]], fields=[[0, 5]],
                             // traits=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .membrdcst(dop, true, true)
          .router(
              [&](const auto &arg) -> std::optional<expression_t> {
                return arg["__broadcastTarget"];
              },
              dop, 1, RoutingPolicy::HASH_BASED, dev,
              aff_parallel())  // (trait=[Pelago.[].packed.X86_64.homBrdcst.hetSingle])
          .to_gpu()  // (trait=[Pelago.[].packed.NVPTX.homBrdcst.hetSingle])
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle])
          .filter([&](const auto &arg) -> expression_t {
            return eq(arg["c_region"], "AMERICA");
          })  // (condition=[=($1, 'AMERICA')],
              // trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle],
              // isS=[false])
          .project([&](const auto &arg) -> std::vector<expression_t> {
            return {arg["c_custkey"]};
          })  // (c_custkey=[$0],
              // trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle])
      ;
  auto rel64293 =
      getBuilder<Tplugin>()
          .scan<Tplugin>(
              "inputs/ssbm100/part.csv",
              {"p_partkey", "p_category", "p_brand1"},
              getCatalog())  // (table=[[SSB, ssbm_part]], fields=[[0, 3, 4]],
                             // traits=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .membrdcst(dop, true, true)
          .router(
              [&](const auto &arg) -> std::optional<expression_t> {
                return arg["__broadcastTarget"];
              },
              dop, 1, RoutingPolicy::HASH_BASED, dev,
              aff_parallel())  // (trait=[Pelago.[].packed.X86_64.homBrdcst.hetSingle])
          .to_gpu()  // (trait=[Pelago.[].packed.NVPTX.homBrdcst.hetSingle])
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle])
          .filter([&](const auto &arg) -> expression_t {
            return eq(arg["p_category"], "MFGR#14");
          })  // (condition=[=($1, 'MFGR#14')],
              // trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle],
              // isS=[false])
          .project([&](const auto &arg) -> std::vector<expression_t> {
            return {arg["p_partkey"], arg["p_brand1"]};
          })  // (p_partkey=[$0], p_brand1=[$2],
              // trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle])
      ;
  auto rel64298 =
      getBuilder<Tplugin>()
          .scan<Tplugin>(
              "inputs/ssbm100/supplier.csv",
              {"s_suppkey", "s_city", "s_nation"},
              getCatalog())  // (table=[[SSB, ssbm_supplier]], fields=[[0, 3,
                             // 4]],
                             // traits=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .membrdcst(dop, true, true)
          .router(
              [&](const auto &arg) -> std::optional<expression_t> {
                return arg["__broadcastTarget"];
              },
              dop, 128, RoutingPolicy::HASH_BASED, dev,
              aff_parallel())  // (trait=[Pelago.[].packed.X86_64.homBrdcst.hetSingle])
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle])
          .filter([&](const auto &arg) -> expression_t {
            return eq(arg["s_nation"], "UNITED STATES");
          })
          .bloomfilter_build(
              [&](const auto &arg) -> expression_t { return arg["s_suppkey"]; },
              bloomSize, 1)
          .project([&](const auto &arg) -> std::vector<expression_t> {
            return {arg["s_suppkey"], arg["s_city"]};
          })
          .pack()
          .to_gpu()
          .unpack();

  auto rel =
      getBuilder<Tplugin>()
          .scan<Tplugin>(
              "inputs/ssbm100/lineorder.csv",
              {"lo_custkey", "lo_partkey", "lo_suppkey", "lo_orderdate",
               "lo_revenue", "lo_supplycost"},
              getCatalog())  // (table=[[SSB, ssbm_lineorder]], fields=[[2, 3,
                             // 4, 5, 12, 13]],
                             // traits=[Pelago.[].packed.X86_64.homSingle.hetSingle])
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
              rel64293,
              [&](const auto &build_arg) -> expression_t {
                return build_arg["p_partkey"];
              },
              [&](const auto &probe_arg) -> expression_t {
                return probe_arg["lo_partkey"];
              },
              17,
              65536)  // (condition=[=($3, $0)], joinType=[inner],
                      // rowcnt=[5.7344E7], maxrow=[1400000.0],
                      // maxEst=[1400000.0], h_bits=[28],
                      // build=[RecordType(INTEGER p_partkey, VARCHAR
                      // p_brand1)], lcount=[2.1283484744275851E9],
                      // rcount=[1.22887812096E10], buildcountrow=[5.7344E7],
                      // probecountrow=[1.22887812096E10])
          .join(
              rel64298,
              [&](const auto &build_arg) -> expression_t {
                return build_arg["s_suppkey"];
              },
              [&](const auto &probe_arg) -> expression_t {
                return probe_arg["lo_suppkey"];
              },
              14,
              8192)  // (condition=[=($4, $0)], joinType=[inner],
                     // rowcnt=[8192000.0], maxrow=[200000.0],
                     // maxEst=[200000.0], h_bits=[25],
                     // build=[RecordType(INTEGER s_suppkey, VARCHAR s_city)],
                     // lcount=[2.7216799017896134E8],
                     // rcount=[3.0721953024E11], buildcountrow=[8192000.0],
                     // probecountrow=[3.0721953024E11])
          .join(
              rel64288,
              [&](const auto &build_arg) -> expression_t {
                return build_arg["c_custkey"];
              },
              [&](const auto &probe_arg) -> expression_t {
                return probe_arg["lo_custkey"];
              },
              21,
              1048576)  // (condition=[=($1, $0)], joinType=[inner],
                        // rowcnt=[6.144E8], maxrow=[3000000.0],
                        // maxEst=[3000000.0], h_bits=[28],
                        // build=[RecordType(INTEGER c_custkey)],
                        // lcount=[1.2433094700931728E10],
                        // rcount=[4.91551248384E8], buildcountrow=[6.144E8],
                        // probecountrow=[4.91551248384E8])
          .join(
              rel64283,
              [&](const auto &build_arg) -> expression_t {
                return build_arg["d_datekey"];
              },
              [&](const auto &probe_arg) -> expression_t {
                return probe_arg["lo_orderdate"];
              },
              11,
              1024)  // (condition=[=($2, $0)], joinType=[inner],
                     // rowcnt=[785203.2], maxrow=[2556.0], maxEst=[2556.0],
                     // h_bits=[22], build=[RecordType(INTEGER d_datekey,
                     // INTEGER d_year)], lcount=[2.2404744691616405E7],
                     // rcount=[983040.0], buildcountrow=[785203.2],
                     // probecountrow=[983040.0])
          .groupby(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {
                    arg["d_year"].as("PelagoAggregate#64311", "d_year"),
                    arg["s_city"].as("PelagoAggregate#64311", "s_city"),
                    arg["p_brand1"].as("PelagoAggregate#64311", "p_brand1")};
              },
              [&](const auto &arg) -> std::vector<GpuAggrMatExpr> {
                return {
                    GpuAggrMatExpr{(arg["lo_revenue"] - arg["lo_supplycost"])
                                       .as("PelagoAggregate#64311", "profit"),
                                   1, 0, SUM}};
              },
              10,
              131072)  // (group=[{0, 1, 2}], profit=[SUM($3)],
                       // trait=[Pelago.[].unpckd.NVPTX.homRandom.hetSingle])
          .pack()      // (trait=[Pelago.[].packed.NVPTX.homRandom.hetSingle],
                       // intrait=[Pelago.[].unpckd.NVPTX.homRandom.hetSingle],
          // inputRows=[29491.2], cost=[{1.6016 rows, 1.6 cpu, 0.0 io}])
          .to_cpu()  // (trait=[Pelago.[].packed.X86_64.homRandom.hetSingle])
          .router(
              DegreeOfParallelism{1}, 128, RoutingPolicy::RANDOM,
              DeviceType::CPU,
              aff_reduce())  // (trait=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .memmove(8, DeviceType::CPU)
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homSingle.hetSingle])
          .groupby(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {
                    arg["d_year"].as("PelagoAggregate#64317", "d_year"),
                    arg["s_city"].as("PelagoAggregate#64317", "s_city"),
                    arg["p_brand1"].as("PelagoAggregate#64317", "p_brand1")};
              },
              [&](const auto &arg) -> std::vector<GpuAggrMatExpr> {
                return {GpuAggrMatExpr{
                    (arg["profit"]).as("PelagoAggregate#64317", "profit"), 1, 0,
                    SUM}};
              },
              10,
              131072)  // (group=[{0, 1, 2}], profit=[SUM($3)],
                       // trait=[Pelago.[].unpckd.NVPTX.homSingle.hetSingle])
          .sort(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {arg["d_year"], arg["s_city"], arg["p_brand1"],
                        arg["profit"]};
              },
              {direction::NONE, direction::ASC, direction::ASC, direction::ASC})
          // (sort0=[$0], sort1=[$1], sort2=[$2],
          // dir0=[ASC], dir1=[ASC], dir2=[ASC],
          // trait=[Pelago.[0, 1,
          // 2].unpckd.X86_64.homSingle.hetSingle])
          .print(pg{"pm-csv"})  // (trait=[ENUMERABLE.[2, 3
      // DESC].unpckd.X86_64.homSingle.hetSingle])
      ;
  return rel.prepare();
}

PreparedStatement Query::prepare43_b(bool memmv, size_t bloomSize) {
  auto rel =
      getBuilder<Tplugin>()
          .scan<Tplugin>("inputs/ssbm100/lineorder.csv",
                         {"lo_custkey", "lo_partkey", "lo_suppkey",
                          "lo_orderdate", "lo_revenue", "lo_supplycost"},
                         getCatalog())  // (table=[[SSB, ssbm_lineorder]],
                                        // fields=[[2, 3, 4, 5, 12, 13]],
          // traits=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .router(128, RoutingPolicy::LOCAL, DeviceType::CPU)
          .unpack()
          .bloomfilter_probe([&](const auto &arg) { return arg["lo_suppkey"]; },
                             bloomSize, 1)
          .reduce(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {expression_t{1}.as("tmp", "cnt")};
              },
              {SUM})
          .router(
              DegreeOfParallelism{1}, 128, RoutingPolicy::RANDOM,
              DeviceType::CPU,
              aff_reduce())  // (trait=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .reduce(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {arg["cnt"]};
              },
              {SUM})
          .print([&](const auto &arg) -> std::vector<expression_t> {
            return {arg["cnt"]};
          });

  return rel.prepare();
}
