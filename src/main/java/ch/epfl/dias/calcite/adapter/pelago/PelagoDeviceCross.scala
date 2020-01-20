package ch.epfl.dias.calcite.adapter.pelago

import java.util
import java.util.List

import ch.epfl.dias.calcite.adapter.pelago.metadata.{PelagoRelMdDeviceType, PelagoRelMdDistribution, PelagoRelMetadataQuery}
import org.apache.calcite.rel.metadata.RelMdDistribution
import org.apache.calcite.rel.{RelDistribution, RelDistributionTraitDef, RelDistributions}

//import ch.epfl.dias.calcite.adapter.pelago.`trait`.RelDeviceType
//import ch.epfl.dias.calcite.adapter.pelago.`trait`.RelDeviceTypeTraitDef
import ch.epfl.dias.emitter.Binding
import ch.epfl.dias.emitter.PlanToJSON.{emitExpression, emitSchema}
import com.google.common.base.Supplier
import org.apache.calcite.plan._
import org.apache.calcite.rel.RelNode
import org.apache.calcite.rel.RelWriter
import org.apache.calcite.rel.SingleRel
import org.json4s.{JValue, JsonAST}
import org.json4s.JsonDSL._
import org.json4s._
import org.json4s.jackson.JsonMethods._
import org.json4s.jackson.Serialization
import ch.epfl.dias.emitter.PlanToJSON.{emitAggExpression, emitArg, emitExpression, emitSchema, getFields}
import ch.epfl.dias.emitter.Binding
import org.apache.calcite.rel.convert.Converter
import org.apache.calcite.rel.core.Exchange
import org.apache.calcite.rel.metadata.RelMetadataQuery
import org.apache.calcite.util.Util
import scala.collection.JavaConverters._

class PelagoDeviceCross protected(cluster: RelOptCluster, traits: RelTraitSet, input: RelNode, val deviceType: RelDeviceType)
      extends SingleRel(cluster, traits, input) with PelagoRel with Converter {
  protected var inTraits: RelTraitSet = input.getTraitSet

  override def explainTerms(pw: RelWriter): RelWriter = {
    super.explainTerms(pw)
      .item("trait", getTraitSet.toString)
  }

  def getDeviceType() = deviceType;

  override def copy(traitSet: RelTraitSet, inputs: List[RelNode]): PelagoDeviceCross = copy(traitSet, inputs.get(0), deviceType)

  def copy(traitSet: RelTraitSet, input: RelNode, deviceType: RelDeviceType) = PelagoDeviceCross.create(input, deviceType)

  override def computeSelfCost(planner: RelOptPlanner, mq: RelMetadataQuery): RelOptCost = {
    mq.getNonCumulativeCost(this)
  }

  override def computeBaseSelfCost(planner: RelOptPlanner, mq: RelMetadataQuery): RelOptCost = { // Higher cost if rows are wider discourages pushing a project through an
//    if (traitSet.containsIfApplicable(RelPacking.UnPckd) && (getDeviceType eq RelDeviceType.NVPTX)) return planner.getCostFactory.makeInfiniteCost()
    // exchange.
    val rowCount = mq.getRowCount(this)
    val bytesPerRow = getRowType.getFieldCount * 4
    planner.getCostFactory.makeCost(rowCount, rowCount * bytesPerRow * 1e6
      * (
      if (traitSet.containsIfApplicable(RelPacking.UnPckd) && (getDeviceType eq RelDeviceType.NVPTX)) return planner.getCostFactory.makeHugeCost()
      else if (traitSet.containsIfApplicable(RelPacking.UnPckd)) Math.min(1e12, rowCount)
      else 1), rowCount * bytesPerRow * 1e5)
  }

  override def estimateRowCount(mq: RelMetadataQuery): Double = input.estimateRowCount(mq)

  override def implement(target: RelDeviceType, alias: String): (Binding, JValue) = {
    val op = ("operator" ,
      if (getDeviceType eq RelDeviceType.NVPTX) "cpu-to-gpu"
      else "gpu-to-cpu"
    )
    val child = getInput.asInstanceOf[PelagoRel].implement(getDeviceType, alias)
    val childBinding = child._1
    var childOp = child._2
//    assert(childBinding.rel == alias)

    val rowType = emitSchema(childBinding.rel, getRowType, false, getTraitSet.containsIfApplicable(RelPacking.Packed))
//
//    if (target != null && getDeviceType() == RelDeviceType.NVPTX && getTraitSet.containsIfApplicable(RelPacking.Packed) && !getTraitSet.containsIfApplicable(RelDistributions.SINGLETON)) {
//      childOp = ("operator", "mem-move-device") ~
//        ("projections", emitSchema(childBinding.rel, getRowType, false, true)) ~
//        ("input", childOp) ~
//        ("to_cpu", target != RelDeviceType.NVPTX)
//    }

    var json = op ~
      ("projections", rowType) ~
      ("queueSize", 1024*1024/4) ~ //FIXME: make adaptive
      ("granularity", "thread") ~ //FIXME: not always
      ("input", childOp)


    if (target != null && getDeviceType() == RelDeviceType.X86_64 && getTraitSet.containsIfApplicable(RelPacking.Packed)) {
      json = ("operator", "mem-move-device") ~
        ("projections", emitSchema(childBinding.rel, getRowType, false, true)) ~
        ("input", json) ~
        ("to_cpu", target != RelDeviceType.NVPTX) ~
        ("do_transfer", getRowType.getFieldList.asScala.map(_ => true))
    }

    val ret = (childBinding, json)
    ret
  }

  override def getInputTraits: RelTraitSet = inTraits

  override def getTraitDef: RelTraitDef[_ <: RelTrait] = RelDeviceTypeTraitDef.INSTANCE
}

object PelagoDeviceCross {
  def create(input: RelNode, toDevice: RelDeviceType): PelagoDeviceCross = {
    val cluster = input.getCluster
    val mq = cluster.getMetadataQuery
    val traitSet = input.getTraitSet.replace(PelagoRel.CONVENTION).replace(toDevice)
      .replaceIf(RelComputeDeviceTraitDef.INSTANCE, () => RelComputeDevice.from(input, false))
      .replaceIf(RelHetDistributionTraitDef.INSTANCE, () => mq.asInstanceOf[PelagoRelMetadataQuery].hetDistribution(input))
    new PelagoDeviceCross(input.getCluster, traitSet, input, toDevice)
  }
}
