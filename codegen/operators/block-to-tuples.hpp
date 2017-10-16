/*
    RAW -- High-performance querying over raw, never-seen-before data.

                            Copyright (c) 2017
        Data Intensive Applications and Systems Labaratory (DIAS)
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
#ifndef BLOCK_TO_TUPLES_HPP_
#define BLOCK_TO_TUPLES_HPP_

#include "operators/operators.hpp"
#include "util/gpu/gpu-raw-context.hpp"

class BlockToTuples : public UnaryRawOperator {
public:
    BlockToTuples(RawOperator * const             child,
                GpuRawContext * const           context,
                const vector<RecordAttribute*> &wantedFields) :
                    UnaryRawOperator(child), 
                    context(context), 
                    wantedFields(wantedFields){}

    virtual ~BlockToTuples()                                             { LOG(INFO)<<"Collapsing BlockToTuples operator";}

    virtual void produce();
    virtual void consume(RawContext    * const context, const OperatorState& childState);
    virtual void consume(GpuRawContext * const context, const OperatorState& childState);
    virtual bool isFiltering() const {return false;}

private:
    void nextEntry();
    virtual void open (RawPipeline * pip);
    virtual void close(RawPipeline * pip);

    const vector<RecordAttribute *> wantedFields;
    std::vector<size_t>             old_buffs   ;
    GpuRawContext * const           context     ;
    AllocaInst                    * mem_itemCtr ;
};

#endif /* BLOCK_TO_TUPLES_HPP_ */