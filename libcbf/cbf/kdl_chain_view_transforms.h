#ifndef CBF_KDL_CHAIN_VIZ_TRANSFORMS_HH
#define CBF_KDL_CHAIN_VIZ_TRANSFORMS_HH

#include <cbf/sensor_transform.h>
#include <cbf/namespace.h>

#include <kdlchainview.h>

namespace CBFSchema {
	class KDLChainViewSensorTransform;
	class KDLTreeViewSensorTransform;
}

namespace CBF {

struct KDLChainViewSensorTransform : public SensorTransform {
	KDLChainViewSensorTransform(const CBFSchema::KDLChainViewSensorTransform &xml_instance, ObjectNamespacePtr object_namespace);

	KDLChainViewSensorTransform(SensorTransformPtr operand) {

	}

	void init(SensorTransformPtr operand) {

	}

	virtual void update(const FloatVector &res) {
		m_Operand->update(res);
	}

	SensorTransformPtr m_Operand;
};


struct KDLTreeViewSensorTransform : public SensorTransform {
	KDLTreeViewSensorTransform(const CBFSchema::KDLTreeViewSensorTransform &xml_instance, ObjectNamespacePtr object_namespace);

	KDLTreeViewSensorTransform(SensorTransformPtr operand) {

	}

	void init(SensorTransformPtr operand) {

	}

	virtual void update(const FloatVector &res) {
		m_Operand->update(res);
	}

	virtual const FloatVector& result() { return m_Operand->result(); }
	virtual const FloatMatrix& task_jacobian() { return m_Operand->task_jacobian(); }

	SensorTransformPtr m_Operand;
};

} // namespace

#endif
