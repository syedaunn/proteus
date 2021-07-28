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

constexpr auto query = "ssb100_Q2_3";

#include "query.cpp.inc"

PreparedStatement Query::prepare23(bool memmv, size_t bloomSize) {
  auto rel23990 =
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
              dop, 1024, dev,
              aff_parallel())  // (trait=[Pelago.[].packed.X86_64.homBrdcst.hetSingle])
          .to_gpu()  // (trait=[Pelago.[].packed.NVPTX.homBrdcst.hetSingle])
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle])
      ;
  auto rel23995 =
      getBuilder<Tplugin>()
          .scan("inputs/ssbm100/supplier.csv", {"s_suppkey", "s_region"},
                getCatalog(), pg{Tplugin::type})
          .membrdcst(dop, memmv || !(dev == DeviceType::CPU), !memmv)
          .router(
              [&](const auto &arg) -> expression_t {
                return arg["__broadcastTarget"];
              },
              dop, 1024, dev, aff_parallel())
          .to_gpu()
          .unpack()
          .filter([&](const auto &arg) -> expression_t {
            return eq(arg["s_region"], "EUROPE");
          })
          .project([&](const auto &arg) -> std::vector<expression_t> {
            return {arg["s_suppkey"]};
          });
  auto rel23999 =
      getBuilder<Tplugin>()
          .scan(
              "inputs/ssbm100/part.csv", {"p_partkey", "p_brand1"},
              getCatalog(),
              pg{Tplugin::
                     type})  // (table=[[SSB, ssbm_part]], fields=[[0, 4]],
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
            return eq(arg["p_brand1"], "MFGR#2239");
          })
          .bloomfilter_build(
              [&](const auto &arg) -> expression_t { return arg["p_partkey"]; },
              bloomSize, 6)
          .project([&](const auto &arg) -> std::vector<expression_t> {
            return {arg["p_partkey"], arg["p_brand1"]};
          })
          .pack()
          .to_gpu()
          .unpack();
  auto rel =
      getBuilder<Tplugin>()
          .scan("inputs/ssbm100/lineorder.csv",
                {"lo_partkey", "lo_suppkey", "lo_orderdate", "lo_revenue"},
                getCatalog(), pg{Tplugin::type})
          .router(8, RoutingPolicy::LOCAL, DeviceType::CPU)
          //          .unpack()
          .bloomfilter_repack(
              [&](const auto &arg) { return arg["lo_partkey"]; }, bloomSize, 6)
          //          .pack()
          .router(8, RoutingPolicy::LOCAL, dev);

  if (memmv) rel = rel.memmove(8, dev);

  rel =
      rel.to_gpu()   // (trait=[Pelago.[].packed.NVPTX.homRandom.hetSingle])
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homRandom.hetSingle])
          .join(
              rel23999,
              [&](const auto &build_arg) -> expression_t {
                return build_arg["p_partkey"];
              },
              [&](const auto &probe_arg) -> expression_t {
                return probe_arg["lo_partkey"];
              },
              12,
              2048)  // (condition=[=($2, $0)], joinType=[inner],
                     // rowcnt=[1433600.0], maxrow=[1400000.0],
                     // maxEst=[1400000.0], h_bits=[23],
                     // build=[RecordType(INTEGER p_partkey, VARCHAR
                     // p_brand1)], lcount=[4.263195668985415E7],
                     // rcount=[3.0721953024E11], buildcountrow=[1433600.0],
                     // probecountrow=[3.0721953024E11])
          .join(
              rel23995,
              [&](const auto &build_arg) -> expression_t {
                return build_arg["s_suppkey"];
              },
              [&](const auto &probe_arg) -> expression_t {
                return probe_arg["lo_suppkey"];
              },
              17,
              512 * 1024)  // (condition=[=($1, $0)], joinType=[inner],
                           // rowcnt=[4.096E7], maxrow=[200000.0],
                           // maxEst=[200000.0], h_bits=[28],
                           // build=[RecordType(INTEGER s_suppkey)],
                           // lcount=[7.179512438249687E8],
                           // rcount=[3.0721953024E8], buildcountrow=[4.096E7],
                           // probecountrow=[3.0721953024E8])
          .join(
              rel23990,
              [&](const auto &build_arg) -> expression_t {
                return build_arg["d_datekey"];
              },
              [&](const auto &probe_arg) -> expression_t {
                return probe_arg["lo_orderdate"];
              },
              14,
              2556)  // (condition=[=($2, $0)], joinType=[inner],
                     // rowcnt=[2617344.0], maxrow=[2556.0], maxEst=[2556.0],
                     // h_bits=[24], build=[RecordType(INTEGER d_datekey,
                     // INTEGER d_year)], lcount=[8.098490429651935E7],
                     // rcount=[6.144390604800001E7], buildcountrow=[2617344.0],
                     // probecountrow=[6.144390604800001E7])
          .groupby(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {arg["d_year"].as("PelagoProject#11438", "d_year"),
                        arg["p_brand1"].as("PelagoProject#11438", "p_brand1")};
              },
              [&](const auto &arg) -> std::vector<GpuAggrMatExpr> {
                return {GpuAggrMatExpr{
                    (arg["lo_revenue"]).as("PelagoProject#11438", "EXPR$0"), 1,
                    0, SUM}};
              },
              10,
              400686)  // (group=[{0, 1}], EXPR$0=[SUM($2)],
                       // trait=[Pelago.[].unpckd.NVPTX.homRandom.hetSingle])
          .pack()      // (trait=[Pelago.[].packed.NVPTX.homRandom.hetSingle],
                       // intrait=[Pelago.[].unpckd.NVPTX.homRandom.hetSingle],
                       // inputRows=[2.45775624192E8], cost=[{282.28200000000004
                       // rows, 282.0 cpu, 0.0 io}])
          .to_cpu()
          .router(
              DegreeOfParallelism{1}, 128, RoutingPolicy::RANDOM,
              DeviceType::CPU,
              aff_reduce())  // (trait=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .memmove(8, DeviceType::CPU)
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homSingle.hetSingle])
          .groupby(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {arg["d_year"].as("PelagoProject#11438", "d_year"),
                        arg["p_brand1"].as("PelagoProject#11438", "p_brand1")};
              },
              [&](const auto &arg) -> std::vector<GpuAggrMatExpr> {
                return {GpuAggrMatExpr{
                    (arg["EXPR$0"]).as("PelagoProject#11438", "EXPR$0"), 1, 0,
                    SUM}};
              },
              10,
              131072)  // (group=[{0, 1}], EXPR$0=[SUM($2)],
                       // trait=[Pelago.[].unpckd.NVPTX.homSingle.hetSingle])
          .sort(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {arg["EXPR$0"], arg["d_year"], arg["p_brand1"]};
              },
              {direction::NONE, direction::ASC,
               direction::ASC})  // (sort0=[$1], sort1=[$2], dir0=[ASC],
                                 // dir1=[ASC], trait=[Pelago.[1,
                                 // 2].unpckd.X86_64.homSingle.hetSingle])
          .print(pg("pm-csv"));
  return rel.prepare();
}

PreparedStatement Query::prepare23_b(bool memmv, size_t bloomSize) {
  auto rel =
      getBuilder<Tplugin>()
          .scan("inputs/ssbm100/lineorder.csv", {"lo_partkey"}, getCatalog(),
                pg{Tplugin::type})
          .router(8, RoutingPolicy::LOCAL, DeviceType::CPU)
          .unpack()
          .bloomfilter_probe([&](const auto &arg) { return arg["lo_partkey"]; },
                             bloomSize, 6)
          .reduce(
              [&](const auto &arg) -> std::vector<expression_t> {
                //                auto h =
                //                expressions::HashExpression{arg["lo_partkey"]};
                //                return {(h & ((int64_t)bloomSize -
                //                1)).as("tmp", "cnt")};
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

PreparedStatement Query::prepare23_b2(bool memmv, size_t bloomSize) {
  auto rel =
      getBuilder<Tplugin>()
          .scan("inputs/ssbm100/lineorder.csv", {"lo_partkey"}, getCatalog(),
                pg{Tplugin::type})
          .router(8, RoutingPolicy::LOCAL, DeviceType::CPU)
          .unpack()
          .bloomfilter_probe([&](const auto &arg) { return arg["lo_partkey"]; },
                             bloomSize, 6)
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

PreparedStatement Query::prepare23_c(bool memmv, size_t bloomSize) {
  auto rel23990 =
      getBuilder<Tplugin>()
          .scan(
              "inputs/ssbm100/date.csv", {"d_datekey", "d_year"}, getCatalog(),
              pg{Tplugin::type})  // (table=[[SSB, ssbm_date]], fields=[[0, 4]],
          // traits=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .membrdcst(dop, true, true)
          .router(
              [&](const auto &arg) -> expression_t {
                return arg["__broadcastTarget"];
              },
              dop, 1024, dev,
              aff_parallel())  // (trait=[Pelago.[].packed.X86_64.homBrdcst.hetSingle])
          .to_gpu()  // (trait=[Pelago.[].packed.NVPTX.homBrdcst.hetSingle])
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle])
      ;
  auto rel23995 =
      getBuilder<Tplugin>()
          .scan("inputs/ssbm100/supplier.csv", {"s_suppkey", "s_region"},
                getCatalog(), pg{Tplugin::type})
          .membrdcst(dop, memmv || !(dev == DeviceType::CPU), !memmv)
          .router(
              [&](const auto &arg) -> expression_t {
                return arg["__broadcastTarget"];
              },
              dop, 1024, dev, aff_parallel())
          .to_gpu()
          .unpack()
          .filter([&](const auto &arg) -> expression_t {
            return eq(arg["s_region"], "EUROPE");
          })
          .project([&](const auto &arg) -> std::vector<expression_t> {
            return {arg["s_suppkey"]};
          });
  auto rel23999 =
      getBuilder<Tplugin>()
          .scan(
              "inputs/ssbm100/part.csv", {"p_partkey", "p_brand1"},
              getCatalog(),
              pg{Tplugin::type})  // (table=[[SSB, ssbm_part]], fields=[[0, 4]],
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
            return eq(arg["p_brand1"], "MFGR#2239");
          })
          .bloomfilter_build(
              [&](const auto &arg) -> expression_t { return arg["p_partkey"]; },
              bloomSize, 6)
          .project([&](const auto &arg) -> std::vector<expression_t> {
            return {arg["p_partkey"], arg["p_brand1"]};
          })
          .pack()
          .to_gpu()
          .unpack();
  auto rel =
      getBuilder<Tplugin>()
          .scan("inputs/ssbm100/lineorder.csv",
                {"lo_partkey", "lo_suppkey", "lo_orderdate", "lo_revenue"},
                getCatalog(), pg{Tplugin::type})
          .router(8, RoutingPolicy::LOCAL, DeviceType::CPU)
          .unpack()
          .bloomfilter_probe([&](const auto &arg) { return arg["lo_partkey"]; },
                             bloomSize, 6)
          .pack()
          .router(
              dop, 8, RoutingPolicy::LOCAL, dev,
              aff_parallel())  // (trait=[Pelago.[].packed.X86_64.homRandom.hetSingle])
      ;

  if (memmv) rel = rel.memmove(8, dev);

  rel = rel.reduce(
               [&](const auto &arg) -> std::vector<expression_t> {
                 return {expression_t{1}.as("tmp", "cnt")};
               },
               {SUM})
            .router(DegreeOfParallelism{1}, 8, RoutingPolicy::RANDOM,
                    DeviceType::CPU)
            .reduce(
                [&](const auto &arg) -> std::vector<expression_t> {
                  return {arg["cnt"]};
                },
                {SUM})
            .print(pg("pm-csv"));
  return rel.prepare();
}

PreparedStatement Query::prepare23_d(bool memmv, size_t bloomSize) {
  auto rel23990 =
      getBuilder<Tplugin>()
          .scan(
              "inputs/ssbm100/date.csv", {"d_datekey", "d_year"}, getCatalog(),
              pg{Tplugin::type})  // (table=[[SSB, ssbm_date]], fields=[[0, 4]],
          // traits=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .membrdcst(dop, true, true)
          .router(
              [&](const auto &arg) -> expression_t {
                return arg["__broadcastTarget"];
              },
              dop, 1024, dev,
              aff_parallel())  // (trait=[Pelago.[].packed.X86_64.homBrdcst.hetSingle])
          .to_gpu()  // (trait=[Pelago.[].packed.NVPTX.homBrdcst.hetSingle])
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle])
      ;
  auto rel23995 =
      getBuilder<Tplugin>()
          .scan("inputs/ssbm100/supplier.csv", {"s_suppkey", "s_region"},
                getCatalog(), pg{Tplugin::type})
          .membrdcst(dop, memmv || !(dev == DeviceType::CPU), !memmv)
          .router(
              [&](const auto &arg) -> expression_t {
                return arg["__broadcastTarget"];
              },
              dop, 1024, dev, aff_parallel())
          .to_gpu()
          .unpack()
          .filter([&](const auto &arg) -> expression_t {
            return eq(arg["s_region"], "EUROPE");
          })
          .project([&](const auto &arg) -> std::vector<expression_t> {
            return {arg["s_suppkey"]};
          });
  auto rel23999 =
      getBuilder<Tplugin>()
          .scan(
              "inputs/ssbm100/part.csv", {"p_partkey", "p_brand1"},
              getCatalog(),
              pg{Tplugin::type})  // (table=[[SSB, ssbm_part]], fields=[[0, 4]],
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
            return eq(arg["p_brand1"], "MFGR#2239");
          })
          .bloomfilter_build(
              [&](const auto &arg) -> expression_t { return arg["p_partkey"]; },
              bloomSize, 6)
          .project([&](const auto &arg) -> std::vector<expression_t> {
            return {arg["p_partkey"], arg["p_brand1"]};
          })
          .pack()
          .to_gpu()
          .unpack();
  auto rel =
      getBuilder<Tplugin>()
          .scan("inputs/ssbm100/lineorder.csv", {"lo_partkey"}, getCatalog(),
                pg{Tplugin::type})
          .router(8, RoutingPolicy::LOCAL, DeviceType::CPU)
          .unpack()
          //          .bloomfilter_probe([&](const auto &arg) { return
          //          arg["lo_partkey"]; },
          //                             bloomSize, 6)
          .pack()
          .router(
              dop, 8, RoutingPolicy::LOCAL, dev,
              aff_parallel())  // (trait=[Pelago.[].packed.X86_64.homRandom.hetSingle])
      ;

  if (memmv) rel = rel.memmove(8, dev);

  rel = rel.reduce(
               [&](const auto &arg) -> std::vector<expression_t> {
                 return {expression_t{1}.as("tmp", "cnt")};
               },
               {SUM})
            .router(DegreeOfParallelism{1}, 8, RoutingPolicy::RANDOM,
                    DeviceType::CPU)
            .reduce(
                [&](const auto &arg) -> std::vector<expression_t> {
                  return {arg["cnt"]};
                },
                {SUM})
            .print(pg("pm-csv"));
  return rel.prepare();
}

PreparedStatement Query::prepare23_e(bool memmv, size_t bloomSize) {
  auto rel =
      getBuilder<Tplugin>()
          .scan("inputs/ssbm100/lineorder.csv",
                {"lo_partkey", "lo_suppkey", "lo_orderdate", "lo_revenue"},
                getCatalog(), pg{Tplugin::type})
          .router(8, RoutingPolicy::LOCAL, DeviceType::CPU)
          .unpack()
          .reduce(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {(arg["lo_partkey"] + arg["lo_suppkey"] +
                         arg["lo_orderdate"] + arg["lo_revenue"])
                            .as("tmp", "cnt")};
              },
              {SUM})
          .router(DegreeOfParallelism{1}, 8, RoutingPolicy::RANDOM,
                  DeviceType::CPU)
          .reduce(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {arg["cnt"]};
              },
              {SUM})
          .print(pg("pm-csv"));
  return rel.prepare();
}

PreparedStatement Query::prepare23_mat(bool memmv, size_t bloomSize) {
  auto rel =
      getBuilder<Tplugin>()
          .scan("inputs/ssbm100/lineorder.csv",
                {"lo_partkey", "lo_suppkey", "lo_orderdate", "lo_revenue"},
                getCatalog(), pg{Tplugin::type})
          .router(8, RoutingPolicy::LOCAL, DeviceType::CPU)
          .unpack()
          .bloomfilter_probe([&](const auto &arg) { return arg["lo_partkey"]; },
                             bloomSize, 6)
          .pack()
          .router(DegreeOfParallelism{1}, 8, RoutingPolicy::LOCAL,
                  DeviceType::CPU);

  if (memmv) rel = rel.memmove(8, DeviceType::CPU);

  rel = rel.unpack().print(pg("block"));
  return rel.prepare();
}
