#ifndef CBF_KDL_CHAIN_VIZ_TRANSFORMS_HH
#define CBF_KDL_CHAIN_VIZ_TRANSFORMS_HH

#include <cbf/sensor_transform.h>
#include <cbf/namespace.h>
#include <cbf/kdl_transforms.h>

#include <qkdlchainview.h>
#include <qkdltreeview.h>

namespace CBFSchema {
	class KDLChainViewSensorTransform;
	class KDLTreeViewSensorTransform;
}

namespace CBF {

struct KDLChainViewSensorTransform : public SensorTransform {
	KDLChainViewSensorTransform(const CBFSchema::KDLChainViewSensorTransform &xml_instance, ObjectNamespacePtr object_namespace);

	KDLChainViewSensorTransform(KDLChainPositionSensorTransformPtr operand) {
		init(operand);
	}

	void init(KDLChainPositionSensorTransformPtr operand) {
		m_Operand = operand;
		m_ChainView = boost::shared_ptr<KDLCV::QKDLChainView>(new KDLCV::QKDLChainView(operand->chain().get()));
		m_ChainView->show();
	}

	virtual void update(const FloatVector &res) {
		CBF_DEBUG("pose size: " << m_ChainView->pose().size());
		CBF_DEBUG("res size: " << res.size());
		m_Operand->update(res);
		std::copy(res.begin(), res.end(), m_ChainView->pose().begin());
		m_ChainView->update();
	}


	virtual unsigned int task_dim() const { return m_Operand->task_dim(); }
	virtual unsigned int resource_dim() const { return m_Operand->resource_dim(); }

	virtual const FloatVector& result() const { return m_Operand->result(); }
	virtual const FloatMatrix& task_jacobian() const { return m_Operand->task_jacobian(); }

	KDLChainPositionSensorTransformPtr m_Operand;

	boost::shared_ptr<KDLCV::QKDLChainView> m_ChainView;
};





struct KDLTreeViewSensorTransform : public SensorTransform {
	KDLTreeViewSensorTransform(const CBFSchema::KDLTreeViewSensorTransform &xml_instance, ObjectNamespacePtr object_namespace);

	KDLTreeViewSensorTransform(KDLTreePositionSensorTransformPtr operand) {
		init(operand);
	}

	void init(KDLTreePositionSensorTransformPtr operand) {
		m_Operand = operand;
		m_TreeView = boost::shared_ptr<KDLCV::QKDLTreeView>(new KDLCV::QKDLTreeView(operand->tree().get()));
		m_TreeView->show();
	}

	virtual void update(const FloatVector &res) {
		CBF_DEBUG("pose size: " << m_TreeView->pose().size());
		CBF_DEBUG("res size: " << res.size());
		m_Operand->update(res);
		std::copy(res.begin(), res.end(), m_TreeView->pose().begin());
		m_TreeView->update();
	}


	virtual unsigned int task_dim() const { return m_Operand->task_dim(); }
	virtual unsigned int resource_dim() const { return m_Operand->resource_dim(); }

	virtual const FloatVector& result() const { return m_Operand->result(); }
	virtual const FloatMatrix& task_jacobian() const { return m_Operand->task_jacobian(); }

	KDLTreePositionSensorTransformPtr m_Operand;

	boost::shared_ptr<KDLCV::QKDLTreeView> m_TreeView;
};


} // namespace

#endif
