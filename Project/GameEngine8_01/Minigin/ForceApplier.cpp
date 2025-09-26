#include "ForceApplier.h"
#include "Collider.h"
#include "Rigidbody.h"
#include "GameObject.h"

namespace dae {
	CForceApplier::CForceApplier(const ForceApplierData& forceApplierData) : m_ForceApplierData(forceApplierData)
	{
	}
	void CForceApplier::Start()
	{
		CCollider* collider = Owner().GetComponent<CCollider>();

		assert(collider);


		float forceOnBegin{ m_ForceApplierData.PushForceOnStartCollision};
		float forceOnStay{ m_ForceApplierData.PushForceOnStayCollision };
		float forceOnEnd{ m_ForceApplierData.PushForceOnEndCollision };

		collider->OnCollisionBeginEvent().Subscribe([forceOnBegin](GameObject& self, GameObject& other)
			{
				CRigidbody2D* rigidbody = other.GetComponent<CRigidbody2D>();



				if (rigidbody)
				{
					glm::vec2 delta = other.Transform().GetWorldTransform().Position - self.Transform().GetWorldTransform().Position;

					delta = glm::normalize(delta) * forceOnBegin;

					rigidbody->SetVelocity(delta);
				}
			});

		collider->OnCollisionStayEvent().Subscribe([forceOnStay](GameObject& self, GameObject& other)
			{
				CRigidbody2D* rigidbody = other.GetComponent<CRigidbody2D>();



				if (rigidbody)
				{
					glm::vec2 delta = other.Transform().GetWorldTransform().Position - self.Transform().GetWorldTransform().Position;

					delta = glm::normalize(delta) * forceOnStay;

					rigidbody->SetVelocity(delta);
				}
			});

		collider->OnCollisionEndEvent().Subscribe([forceOnEnd](GameObject& self, GameObject& other)
			{
				CRigidbody2D* rigidbody = other.GetComponent<CRigidbody2D>();

				if (rigidbody)
				{
					glm::vec2 delta = other.Transform().GetWorldTransform().Position - self.Transform().GetWorldTransform().Position;

					delta = glm::normalize(delta) * forceOnEnd;

					rigidbody->SetVelocity(delta);
				}
			});

	}
}