package ch.epfl.dias.calcite.adapter.pelago

import java.util

import ch.epfl.dias.calcite.adapter.pelago.metadata.PelagoRelMetadataQuery
import ch.epfl.dias.emitter.Binding
import ch.epfl.dias.emitter.PlanToJSON._
import org.apache.calcite.plan.{RelOptCluster, RelOptCost, RelOptPlanner, RelTraitSet}
import org.apache.calcite.rel.`type`.RelDataType
import org.apache.calcite.rel.{RelNode, RelWriter, SingleRel}
import org.apache.calcite.rel.core._
import org.apache.calcite.rel.metadata.RelMetadataQuery
import org.apache.calcite.rex.{RexCorrelVariable, RexFieldAccess, RexInputRef, RexNode}
import org.apache.calcite.util.Pair
import org.json4s._
import org.json4s.JsonAST.JValue
import org.json4s.JsonDSL._

import scala.collection.JavaConverters._

class PelagoUnnest protected (cluster: RelOptCluster, traitSet: RelTraitSet, input: RelNode,
                              val correlationId: CorrelationId,
//                              val requiredColumns: ImmutableBitSet,
//                              val joinType: SemiJoinType,
                              val outType: RelDataType,
                              val namedProjections: List[Pair[RexNode, String]])
  extends SingleRel(cluster, traitSet, input) with PelagoRel{
//
//  override def copy(traitSet: RelTraitSet, input: RelNode, right: RelNode, correlationId: CorrelationId, requiredColumns: ImmutableBitSet, joinType: SemiJoinType): Correlate = {
//    PelagoUnnest.create(left, right, correlationId, requiredColumns, joinType)
//  }

  override def copy(traitSet: RelTraitSet, inputs: util.List[RelNode]): PelagoUnnest = copy(traitSet, inputs.get(0))

  def copy(traitSet: RelTraitSet, input: RelNode) = PelagoUnnest.create(input, correlationId, namedProjections, outType)

  override def deriveRowType = {
    outType
//    println(namedProjections.map { p => p.left.asInstanceOf[RexFieldAccess].getType })
//    println(input.getRowType.getFieldList.asScala.map { p => p.getType }.toList ::: namedProjections.map { p => p.left.asInstanceOf[RexFieldAccess].getField })
//    cluster.getTypeFactory.createStructType(
//      (
//        input.getRowType.getFieldList.asScala.map { p => p.getType } ++
//        namedProjections.map { p => p.left.asInstanceOf[RexFieldAccess].getField.getType }
//      ).asJava,
//      (
//        input.getRowType.getFieldList.asScala.map { p => p.getName } ++
//        namedProjections.map{ p => p.right }
//      ).asJava
//    )
  }

  override def estimateRowCount(mq: RelMetadataQuery): Double = input.estimateRowCount(mq) * 8


  override def computeSelfCost(planner: RelOptPlanner, mq: RelMetadataQuery): RelOptCost = {
    mq.getNonCumulativeCost(this)
  }

  override def computeBaseSelfCost(planner: RelOptPlanner, mq: RelMetadataQuery): RelOptCost = {
    if (traitSet.containsIfApplicable(RelDeviceType.NVPTX)) {
      super.computeSelfCost(planner, mq).multiplyBy(100)
    } else {
      super.computeSelfCost(planner, mq).multiplyBy(1000)
    }
  }

  override def explainTerms(pw: RelWriter): RelWriter = super.explainTerms(pw).item("trait", getTraitSet.toString)

  override def implement(target: RelDeviceType, alias2: String): (Binding, JValue) = {
    val alias = PelagoTable.create(alias2, getRowType)
    val op    = ("operator" , "project")

    val unnest = implementUnnest
    val inputBinding: Binding = unnest._1
    val unnestdOp = unnest._2

    val proj_exprs = input.getRowType.getFieldList.asScala.map {
      f => {
        emitExpression(RexInputRef.of(f.getIndex, input.getRowType), List(inputBinding), true, this).asInstanceOf[JObject] ~
          ("register_as", ("attrName", f.getName) ~ ("relName", alias.getPelagoRelName))
      }
    }

    val unnestRowTypeSlice = getRowType.getFieldList.asScala.slice(input.getRowType.getFieldCount, getRowType.getFieldCount)

    val unnestedRowType = getCluster.getTypeFactory.createStructType(
                                                                      unnestRowTypeSlice.map {p => p.getType}.asJava,
                                                                      unnestRowTypeSlice.map {p => p.getName}.asJava
                                                                    )

    assert(namedProjections.size == 1)
    val unnestBinding = Binding(PelagoTable.create(inputBinding.rel + "." + namedProjections.head.right, unnestedRowType), getFields(unnestedRowType))

    val nested_exprs = unnestRowTypeSlice.zipWithIndex.map {
      p => {//this binding is actually the input binding
        emitExpression(RexInputRef.of(p._2, unnestedRowType), List(unnestBinding), true, this).asInstanceOf[JObject] ~
          ("register_as", ("attrName", p._1.getName) ~ ("relName", alias.getPelagoRelName))
      }
    }

    val json = op ~
      ("gpu"    , getTraitSet.containsIfApplicable(RelDeviceType.NVPTX) ) ~
      ("e"      , proj_exprs ++ nested_exprs                            ) ~
      ("relName", alias.getPelagoRelName                                ) ~
      ("input"  , unnestdOp                                             )

    val binding: Binding = Binding(alias, getFields(getRowType))
    val ret: (Binding, JValue) = (binding, json)
    ret
  }


  def implementUnnest(): (Binding, JValue) = {
    val op    = ("operator" , "unnest")
    val alias = "__unnest" + getId
    val child = input.asInstanceOf[PelagoRel].implement(getTraitSet.getTrait(RelDeviceTypeTraitDef.INSTANCE))
    val childBinding: Binding = child._1
    val childOp = child._2
//    val rowType = emitSchema(childBinding.rel, getRowType)
//    val cond = emitExpression(getCondition, List(childBinding))

    assert(namedProjections.size == 1)

    val p = namedProjections.head

    val expr = p.left.asInstanceOf[RexFieldAccess]
    assert(expr.getReferenceExpr().asInstanceOf[RexCorrelVariable].id == correlationId)
    val f = emitExpression(RexInputRef.of(p.left.asInstanceOf[RexFieldAccess].getField.getIndex, input.getRowType), List(childBinding), true, this)

    val unnest_exprs = ("e", f) ~ ("name", "__" + alias + "_" + f.\("attribute").\("attrName").extract[String])

    val json : JValue = op ~
      ("gpu"      , getTraitSet.containsIfApplicable(RelDeviceType.NVPTX) ) ~
      ("p"        , ("expression", "bool") ~ ("v", true)                  ) ~
      ("path"     , unnest_exprs                                          ) ~
      ("argNo"    , 0                                                     ) ~
      ("input"    , childOp                                               )

    val ret: (Binding, JValue) = (childBinding, json)
    ret
  }
}


object PelagoUnnest{
  def create(input: RelNode, correlationId: CorrelationId, namedProjections: util.List[Pair[RexNode, String]], uncollectionType: RelDataType): PelagoUnnest = {
    PelagoUnnest.create(input, correlationId, namedProjections.asScala.toList,
      input.getCluster.getTypeFactory.createStructType(
        (
          input.getRowType.getFieldList.asScala.map { p => p.getType } ++
            uncollectionType.getFieldList.asScala.map { p => p.getType }//.left.asInstanceOf[RexFieldAccess].getField.getType }
        ).asJava,
        (
          input.getRowType.getFieldList.asScala.map { p => p.getName } ++
            uncollectionType.getFieldList.asScala.map { p => p.getName }
        ).asJava
      )
    )
  }

  def create(input: RelNode, correlationId: CorrelationId, namedProjections: List[Pair[RexNode, String]], rowType: RelDataType): PelagoUnnest = {
    val traitSet = input.getTraitSet.replace(PelagoRel.CONVENTION)
      .replaceIf(RelDeviceTypeTraitDef.INSTANCE, () => RelDeviceType.X86_64)
      .replaceIf(RelComputeDeviceTraitDef.INSTANCE, () => RelComputeDevice.from(input))

    new PelagoUnnest(input.getCluster, traitSet, input, correlationId, rowType, namedProjections)
  }
}