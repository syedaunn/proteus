/*
    Harmonia -- High-performance elastic HTAP on heterogeneous hardware.

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
#include "q19.hpp"

static int q_instance = 30;

PreparedStatement Q_19_cpar(DegreeOfParallelism dop, const aff_t &aff_parallel,
                            const aff_t &aff_reduce, DeviceType dev,
                            const scan_t &scan) {
  auto query = "Q19";
  auto memmv = false;

  assert(dev == DeviceType::CPU);

  auto rel1618 =
      scan(
          "tpcc_item",
          {"i_id",
           "i_price"})  // (table=[[SSB, ch100w_item]], fields=[[0, 3]],
                        // traits=[Pelago.[].X86_64.packed.homSingle.hetSingle.none])
          .membrdcst(dop, true, true)
          .router(
              [&](const auto &arg) -> std::optional<expression_t> {
                return arg["__broadcastTarget"];
              },
              dop, 1, RoutingPolicy::HASH_BASED, dev,
              aff_parallel())  // (trait=[Pelago.[].X86_64.packed.homBrdcst.hetSingle.none])
          .unpack()  // (trait=[Pelago.[].X86_64.unpckd.homBrdcst.hetSingle.cX86_64])
          .project([&](const auto &arg) -> std::vector<expression_t> {
            return {(arg["$0"]).as("PelagoProject#1617", "i_id"),
                    (ge(arg["$1"], ((double)1))).as("PelagoProject#1617", ">="),
                    (le(arg["$1"], ((double)400000)))
                        .as("PelagoProject#1617", "<=")};
          })  // (i_id=[$0], >==[>=($1, 1)], <==[<=($1, 400000)],
              // trait=[Pelago.[].X86_64.unpckd.homBrdcst.hetSingle.cX86_64])
          .filter([&](const auto &arg) -> expression_t {
            return (arg["$1"] & arg["$2"]);
          })  // (condition=[AND($1, $2)],
              // trait=[Pelago.[].X86_64.unpckd.homBrdcst.hetSingle.cX86_64],
              // isS=[false])
      ;
  return scan(
             "tpcc_orderline",
             {"ol_w_id", "ol_i_id", "ol_quantity",
              "ol_amount"})  // (table=[[SSB, ch100w_orderline]], fields=[[2, 4,
                             // 7, 8]],
                             // traits=[Pelago.[].X86_64.packed.homSingle.hetSingle.none])
      .router(
          dop, 1, RoutingPolicy::LOCAL, dev,
          aff_parallel())  // (trait=[Pelago.[].X86_64.packed.homRandom.hetSingle.none])
      .unpack()  // (trait=[Pelago.[].X86_64.unpckd.homRandom.hetSingle.cX86_64])
      .project([&](const auto &arg) -> std::vector<expression_t> {
        return {(arg["$1"]).as("PelagoProject#1621", "ol_i_id"),
                (arg["$3"]).as("PelagoProject#1621", "ol_amount"),
                (ge(arg["$2"], 1)).as("PelagoProject#1621", ">="),
                (le(arg["$2"], 10)).as("PelagoProject#1621", "<="),
                ((eq(arg["$0"], 1) | eq(arg["$0"], 2) | eq(arg["$0"], 3)))
                    .as("PelagoProject#1621", "OR"),
                ((eq(arg["$0"], 1) | eq(arg["$0"], 2) | eq(arg["$0"], 4)))
                    .as("PelagoProject#1621", "OR5"),
                ((eq(arg["$0"], 1) | eq(arg["$0"], 5) | eq(arg["$0"], 3)))
                    .as("PelagoProject#1621", "OR6")};
      })  // (ol_i_id=[$1], ol_amount=[$3],
          // >==[>=($2, 1)], <==[<=($2, 10)],
          // OR=[OR(=($0, 1), =($0, 2), =($0, 3))],
          // OR5=[OR(=($0, 1), =($0, 2),
          // =($0, 4))], OR6=[OR(=($0, 1), =($0, 5),
          // =($0, 3))],
          // trait=[Pelago.[].X86_64.unpckd.homRandom.hetSingle.cX86_64])
      .filter([&](const auto &arg) -> expression_t {
        return (arg["$2"] & arg["$3"] & (arg["$4"] | arg["$5"] | arg["$6"]));
      })  // (condition=[AND($2, $3, OR($4, $5,
          // $6))],
          // trait=[Pelago.[].X86_64.unpckd.homRandom.hetSingle.cX86_64],
          // isS=[false])
      .join(
          rel1618,
          [&](const auto &build_arg) -> expression_t {
            return build_arg["$0"].as("PelagoJoin#1623", "bk_0");
          },
          [&](const auto &probe_arg) -> expression_t {
            return probe_arg["$0"].as("PelagoJoin#1623", "pk_0");
          },
          27,
          100000)  // (condition=[=($3, $0)],
                   // joinType=[inner],
                   // rowcnt=[2.56E7],
                   // maxrow=[100000.0],
                   // maxEst=[100000.0], h_bits=[27],
                   // build=[RecordType(INTEGER i_id,
                   // BOOLEAN >=, BOOLEAN <=)],
                   // lcount=[1.3944357272154548E9],
                   // rcount=[3.84E9],
                   // buildcountrow=[2.56E7],
                   // probecountrow=[3.84E9])
      .project([&](const auto &arg) -> std::vector<expression_t> {
        return {(arg["$4"]).as("PelagoProject#1624", "ol_amount")};
      })  // (ol_amount=[$4],
          // trait=[Pelago.[].X86_64.unpckd.homRandom.hetSingle.cX86_64])
      .reduce(
          [&](const auto &arg) -> std::vector<expression_t> {
            return {(arg["$0"]).as("PelagoAggregate#1625", "$0")};
          },
          {SUM})  // (group=[{}], revenue=[SUM($0)],
                  // trait=[Pelago.[].X86_64.unpckd.homRandom.hetSingle.cX86_64],
                  // global=[false])
      .router(
          DegreeOfParallelism{1}, 128, RoutingPolicy::RANDOM, DeviceType::CPU,
          aff_reduce())  // (trait=[Pelago.[].X86_64.unpckd.homSingle.hetSingle.cX86_64])
      .reduce(
          [&](const auto &arg) -> std::vector<expression_t> {
            return {(arg["$0"]).as("PelagoAggregate#1627", "$0")};
          },
          {SUM})  // (group=[{}], revenue=[SUM($0)],
                  // trait=[Pelago.[].X86_64.unpckd.homSingle.hetSingle.cX86_64],
                  // global=[true])
      .print(
          [&](const auto &arg,
              std::string outrel) -> std::vector<expression_t> {
            return {arg["$0"].as(outrel, "revenue")};
          },
          std::string{query} + (memmv ? "mv" : "nmv") +
              std::to_string(
                  q_instance++))  // (trait=[ENUMERABLE.[].X86_64.unpckd.homSingle.hetSingle.cX86_64])
      .prepare();
}
