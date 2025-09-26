#pragma once

#include "Component.h"

namespace dae {

	struct ForceApplierData {
		float PushForceOnStartCollision{ 0 };
		float PushForceOnStayCollision{ 0 };
		float PushForceOnEndCollision{ 0 };
	};

	class CForceApplier : public Component {

	public:
		CForceApplier(const ForceApplierData& forceApplierData);
		void Start()override;

	private:
		ForceApplierData m_ForceApplierData;
	};
}