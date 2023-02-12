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

constexpr auto query = "ch100w_Q19";

#include "query.cpp.inc"

PreparedStatement Query::prepare19(bool memmv) {
  auto rel2305 =
      getBuilder<Tplugin>()
          .scan(
              "tpcc_item", {"i_id", "i_price"}, getCatalog(),
              pg{Tplugin::
                     type})  // (table=[[SSB, tpcc_item]], fields=[[0, 3]],
                             // traits=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .membrdcst(dop, true, true)
          .router(
              [&](const auto &arg) -> expression_t {
                return arg["__broadcastTarget"];
              },
              dop, 1, dev,
              aff_parallel())  // (trait=[Pelago.[].packed.X86_64.homBrdcst.hetSingle])
      ;

  if (memmv) rel2305 = rel2305.memmove(8, dev);

  rel2305 =
      rel2305
          .to_gpu()  // (trait=[Pelago.[].packed.NVPTX.homBrdcst.hetSingle])
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle])
          .filter([&](const auto &arg) -> expression_t {
            return (ge(arg["i_price"], 1.0) &
                    le(arg["i_price"], ((double)400000.0)));
          })  // (condition=[AND($1, $2)],
              // trait=[Pelago.[].unpckd.NVPTX.homBrdcst.hetSingle],
              // isS=[false])
      ;
  auto rel =
      getBuilder<Tplugin>()
          .scan(
              "tpcc_orderline",
              {"ol_w_id", "ol_i_id", "ol_quantity", "ol_amount"}, getCatalog(),
              pg{Tplugin::
                     type})  // (table=[[SSB, tpcc_orderline]], fields=[[2, 4,
                             // 7, 8]],
                             // traits=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .router(dop, 32, RoutingPolicy::LOCAL, dev, aff_parallel())  //
      // (trait=[Pelago.[].packed.X86_64.homRandom.hetSingle])
      ;

  if (memmv) rel = rel.memmove(8, dev);

  rel =
      rel.to_gpu()   // (trait=[Pelago.[].packed.NVPTX.homRandom.hetSingle])
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homRandom.hetSingle])
          .filter([&](const auto &arg) -> expression_t {
            return (ge(arg["ol_quantity"], 1) & le(arg["ol_quantity"], 10) &
                    (eq(arg["ol_w_id"], 1) | eq(arg["ol_w_id"], 2) |
                     eq(arg["ol_w_id"], 3) | eq(arg["ol_w_id"], 4) |
                     eq(arg["ol_w_id"], 5)));
          })  // (condition=[AND($2, $3, OR($4, $5, $6))],
              // trait=[Pelago.[].unpckd.NVPTX.homRandom.hetSingle],
              // isS=[false])
          .join(
              rel2305,
              [&](const auto &build_arg) -> expression_t {
                return build_arg["$0"];
              },
              [&](const auto &probe_arg) -> expression_t {
                return probe_arg["ol_i_id"];
              },
              19,
              100000)  // (condition=[=($3, $0)], joinType=[inner],
                       // rowcnt=[2.56E7], maxrow=[100000.0], maxEst=[100000.0],
                       // h_bits=[27], build=[RecordType(INTEGER i_id, BOOLEAN
                       // >=, BOOLEAN <=)], lcount=[1.3944357272154548E9],
                       // rcount=[3.84E9], buildcountrow=[2.56E7],
                       // probecountrow=[3.84E9])
          .reduce(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {(arg["ol_amount"]).as("PelagoAggregate#2313", "$0")};
              },
              {SUM})  // (group=[{}], revenue=[SUM($0)],
                      // trait=[Pelago.[].unpckd.NVPTX.homRandom.hetSingle])
          .to_cpu()   // (trait=[Pelago.[].packed.X86_64.homRandom.hetSingle])
          .router(
              DegreeOfParallelism{1}, 128, RoutingPolicy::RANDOM,
              DeviceType::CPU,
              aff_reduce())  // (trait=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .reduce(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {(arg["$0"]).as("PelagoAggregate#2319", "revenue")};
              },
              {SUM})  // (group=[{}], revenue=[SUM($0)],
                      // trait=[Pelago.[].unpckd.NVPTX.homSingle.hetSingle])
          .print(
              pg{"pm-csv"},
              std::string{query} +
                  (memmv
                       ? "mv"
                       : "nmv"))  // (trait=[ENUMERABLE.[].unpckd.X86_64.homSingle.hetSingle])
      ;

  return rel.prepare();
}