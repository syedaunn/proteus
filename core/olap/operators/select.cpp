/*
    Proteus -- High-performance query processing on heterogeneous hardware.

                            Copyright (c) 2014
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

#include "operators/select.hpp"

void Select::produce_(ParallelContext *context) {
  getChild()->produce(context);
}

void Select::consume(Context *const context, const OperatorState &childState) {
  generate(context, childState);
}

void Select::generate(Context *const context,
                      const OperatorState &childState) const {
  context->gen_if(expr, childState)([&] {
    // Triggering parent
    OperatorState newState{*this, childState.getBindings()};
    getParent()->consume(context, newState);
  });
}
