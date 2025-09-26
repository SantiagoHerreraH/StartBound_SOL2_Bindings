#pragma once

#include "Rigidbody.h"
#include "Component.h"
#include "../3rdParty/glm/glm.hpp"


namespace dae {



	

	struct CMovement2D : public Component
	{
	public:
		void Start()override;
		void FixedUpdate()override;

		void SetMaxSpeed(float maxSpeed);
		void SetMaxAccelForce(float maxForce);
		void AddSingleFrameMovementInput(const glm::vec2& input);

	private:
		std::shared_ptr<CRigidbody2D> m_Rigidbody{nullptr};
		glm::vec2 m_MovementInput;
		float m_MaxSpeed = 1;
		float m_MaxForce = 1;
		float m_CurrentSpeed = 1;
	};

}


