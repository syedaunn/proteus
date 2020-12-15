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

constexpr auto query = "ch100w_Q06";

#include "query.cpp.inc"

PreparedStatement Query::prepare06(bool memmv) {
  auto rel =
      getBuilder<Tplugin>()
          .scan(
              "tpcc_orderline", {"ol_delivery_d", "ol_quantity", "ol_amount"},
              getCatalog(),
              pg{Tplugin::
                     type})  // (table=[[SSB, ch100w_orderline]], fields=[[6, 7,
                             // 8]],
                             // traits=[Pelago.[].packed.X86_64.homSingle.hetSingle])
          .router(
              dop, 32, RoutingPolicy::LOCAL, dev,
              aff_parallel())  // (trait=[Pelago.[].packed.X86_64.homRandom.hetSingle])
      ;

  if (memmv) rel = rel.memmove(8, dev);

  rel =
      rel.to_gpu()   // (trait=[Pelago.[].packed.NVPTX.homRandom.hetSingle])
          .unpack()  // (trait=[Pelago.[].unpckd.NVPTX.homRandom.hetSingle])
          .filter([&](const auto &arg) -> expression_t {
            return (ge(arg["ol_delivery_d"],
                       expressions::DateConstant(915148800000)) &
                    lt(arg["ol_delivery_d"],
                       expressions::DateConstant(1577836800000)) &
                    ge(arg["ol_quantity"], 1) & le(arg["ol_quantity"], 100000));
          })  // (condition=[AND(>=($0, 1999-01-01 00:00:00:TIMESTAMP(3)), <($0,
              // 2020-01-01 00:00:00:TIMESTAMP(3)), >=($1, 1), <=($1, 100000))],
              // trait=[Pelago.[].unpckd.NVPTX.homRandom.hetSingle],
              // isS=[false])
          .reduce(
              [&](const auto &arg) -> std::vector<expression_t> {
                return {(arg["ol_amount"]).as("PelagoAggregate#1241", "$0")};
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
                return {(arg["$0"]).as("PelagoAggregate#1247", "revenue")};
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
