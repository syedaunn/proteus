package ch.epfl.dias.calcite.adapter.pelago

import ch.epfl.dias.emitter.Binding
import ch.epfl.dias.emitter.PlanToJSON
import org.apache.calcite.plan._
import org.apache.calcite.rel._
import org.apache.calcite.rel.convert.ConverterImpl
import org.apache.calcite.rel.core.Aggregate
import org.apache.calcite.rel.core.AggregateCall
import org.apache.calcite.rel.core.Exchange
import org.apache.calcite.rel.metadata.RelMetadataQuery
import org.json4s.JsonAST
import java.util

import ch.epfl.dias.emitter.PlanToJSON.{emitAggExpression, emitArg, emitExpression, emitSchema, emit_, getFields}
import ch.epfl.dias.emitter.Binding
import org.apache.calcite.plan.RelOptCluster
import org.apache.calcite.plan.RelOptCost
import org.apache.calcite.plan.RelOptPlanner
import org.apache.calcite.plan.RelTraitSet
import org.apache.calcite.rel.RelNode
import org.apache.calcite.rel.core.{AggregateCall, Filter}
import org.apache.calcite.rel.metadata.RelMetadataQuery
import org.apache.calcite.rex.{RexInputRef, RexNode}
import org.json4s.JsonDSL._
import org.json4s._
import org.json4s.jackson.JsonMethods._
import org.json4s.jackson.Serialization
import org.apache.calcite.rex.RexNode
import org.json4s.JsonAST

import scala.collection.JavaConverters._
import java.util

import ch.epfl.dias.calcite.adapter.pelago.metadata.RelMdDeviceType

//import ch.epfl.dias.calcite.adapter.pelago.`trait`.{RelDeviceType, RelDeviceTypeTraitDef}
import com.google.common.base.Supplier
import org.apache.calcite.rel.convert.Converter

class PelagoRouter protected(cluster: RelOptCluster, traitSet: RelTraitSet, input: RelNode, distribution: RelDistribution)
    extends Exchange(cluster, traitSet, input, distribution) with PelagoRel with Converter {
  assert(getConvention eq PelagoRel.CONVENTION)
  assert(getConvention eq input.getConvention)
  protected var inTraits: RelTraitSet = input.getTraitSet

  override def copy(traitSet: RelTraitSet, input: RelNode, distribution: RelDistribution) = PelagoRouter.create(input, distribution)

  override def estimateRowCount(mq: RelMetadataQuery): Double = {
    var rc = super.estimateRowCount(mq)
//    if      (getDistribution eq RelDistributions.BROADCAST_DISTRIBUTED) rc = rc * 4.0
//    else if (getDistribution eq RelDistributions.RANDOM_DISTRIBUTED   ) rc = rc / 4.0 //TODO: Does this hold even when input is already distributed ?
    rc
  }

  override def computeSelfCost(planner: RelOptPlanner, mq: RelMetadataQuery): RelOptCost = {
//    if (traitSet.containsIfApplicable(RelPacking.UnPckd)) return planner.getCostFactory.makeHugeCost()
    var base = super.computeSelfCost(planner, mq).multiplyBy(1)
//    if (getDistribution.getType eq RelDistribution.Type.HASH_DISTRIBUTED) base = base.multiplyBy(80)
    base

//    planner.getCostFactory.makeZeroCost()
  }

  override def explainTerms(pw: RelWriter): RelWriter = super.explainTerms(pw).item("trait", getTraitSet.toString)

  override def implement(target: RelDeviceType): (Binding, JValue) = {
    assert(getTraitSet.containsIfApplicable(RelPacking.UnPckd) || (target != null))
    val child = getInput.asInstanceOf[PelagoRel].implement(null)
    val childBinding: Binding = child._1
    var childOp = child._2
    val rowType = emitSchema(childBinding.rel, getRowType, false, getTraitSet.containsIfApplicable(RelPacking.Packed))

    var out_dop = 2 //if (target == RelDeviceType.NVPTX) 2 else 24
    if (getDistribution.getType eq RelDistribution.Type.SINGLETON) {
      out_dop = 1
    }

    var in_dop = 2 //if (target == RelDeviceType.NVPTX) 2 else 24
    if (input.getTraitSet.containsIfApplicable(RelDistributions.SINGLETON)) {
      in_dop = 1
    }

    val op = {
      if (getDistribution.getType eq RelDistribution.Type.BROADCAST_DISTRIBUTED) {
        ("operator", "broadcast")
      } else if (getDistribution.getType eq RelDistribution.Type.SINGLETON) {
        ("operator", "unionall")
      } else if (getDistribution.getType eq RelDistribution.Type.RANDOM_DISTRIBUTED) {
        ("operator", "split")
      } else {
        ("operator", "shuffle")
//        assert(false, "translation not implemented")
      }
    }

    val projs = getRowType.getFieldList.asScala.zipWithIndex.map{
      f => {
        emitExpression(RexInputRef.of(f._2, getInput.getRowType), List(childBinding)).asInstanceOf[JObject]
      }
    }

    val policy: JObject = {
      if (getDistribution.getType eq RelDistribution.Type.BROADCAST_DISTRIBUTED) {
        if (getTraitSet.containsIfApplicable(RelPacking.Packed)) {
          childOp = ("operator", "mem-broadcast-device") ~
            ("num_of_targets", out_dop) ~
            ("projections", emitSchema(childBinding.rel, getRowType, false, true)) ~
            ("input", child._2) ~
            ("to_cpu", target != RelDeviceType.NVPTX)
        }

        ("target",
          ("expression", "recordProjection") ~
          ("e",
            ("expression", "argument") ~
            ("argNo", -1) ~
            ("type",
              ("type", "record") ~
              ("relName", childBinding.rel)
            ) ~
            ("attributes", List(
              ("relName", childBinding.rel) ~
              ("attrName", "__broadcastTarget")
            ))
          ) ~
          ("attribute",
            ("relName", childBinding.rel) ~
            ("attrName", "__broadcastTarget")
          )
        )
      } else if (getDistribution.getType eq RelDistribution.Type.SINGLETON) {
        if (getTraitSet.containsIfApplicable(RelPacking.Packed)) {
          ("operator", "exchange") //just leave the default policy
        } else {
          ("numa_local", false)
        }
      } else if (getDistribution.getType eq RelDistribution.Type.RANDOM_DISTRIBUTED) {
        if (getTraitSet.containsIfApplicable(RelPacking.Packed)) {
          if (target == RelDeviceType.NVPTX) {
            ("numa_local", true)
          } else {
            ("rand_local_cpu", true)
          }
        } else {
          ("numa_local", false)
        }
      } else {
        // else if (getDistribution.getType eq RelDistribution.Type.SINGLETON) {
        assert(false, "translation not implemented")
        ("operator", "shuffle")
      }
    }

    var json = ("operator", "exchange") ~
      ("gpu"           , getTraitSet.containsIfApplicable(RelDeviceType.NVPTX) ) ~
      ("projections", rowType) ~
      ("numOfParents", out_dop) ~
      ("producers", in_dop) ~
      ("slack", 8) ~
      policy ~
      ("input", childOp)

    if (getDistribution.getType != RelDistribution.Type.BROADCAST_DISTRIBUTED && getTraitSet.containsIfApplicable(RelPacking.Packed)) {
      json = ("operator", "mem-move-device") ~
        ("projections", emitSchema(childBinding.rel, getRowType, false, true)) ~
        ("input", json) ~
        ("to_cpu", target != RelDeviceType.NVPTX)
    }

    val ret: (Binding, JValue) = (childBinding, json)
    ret
  }

  override def getInputTraits: RelTraitSet = inTraits

  override def getTraitDef: RelTraitDef[_ <: RelTrait] = RelDistributionTraitDef.INSTANCE
}

object PelagoRouter{
  def create(input: RelNode, distribution: RelDistribution): PelagoRouter = {
    val cluster  = input.getCluster
    val traitSet = input.getTraitSet.replace(PelagoRel.CONVENTION).replace(distribution)
      .replaceIf(RelDeviceTypeTraitDef.INSTANCE, new Supplier[RelDeviceType]() {
        override def get: RelDeviceType = {
//          System.out.println(RelMdDeviceType.exchange(cluster.getMetadataQuery, input) + " " + input.getTraitSet + " " + input)
//          return RelMdDeviceType.exchange(cluster.getMetadataQuery, input)
          return RelDeviceType.X86_64;
        }
      });
    new PelagoRouter(input.getCluster, traitSet, input, distribution)
  }
}