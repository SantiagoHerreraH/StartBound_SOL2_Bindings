#include "Movement.h"
#include "GameObject.h"
#include "GameTime.h"

void dae::CMovement2D::Start()
{
	m_Rigidbody = Owner().GetComponentHandle<CRigidbody2D>();

	if (!m_Rigidbody)
	{
		m_Rigidbody = Owner().AddComponent<CRigidbody2D>(CRigidbody2D{});
	}
}

void dae::CMovement2D::FixedUpdate()
{
	if (glm::length(m_MovementInput) > FLT_EPSILON)
	{
		m_MovementInput = glm::normalize(m_MovementInput);

		glm::vec2 desiredForce = (m_MovementInput * m_MaxForce);
		glm::vec2 desiredVel = m_Rigidbody->GetVelocity() + (desiredForce * Time::GetInstance().GetFixedTimeStep());

		if (glm::length(desiredVel) > m_MaxSpeed)
		{
			desiredVel = glm::normalize(desiredVel) * m_MaxSpeed;
			desiredForce = desiredVel - m_Rigidbody->GetVelocity();
		}

		m_Rigidbody->SetNetForce(desiredForce);
		m_MovementInput = glm::vec2{ 0,0 };
	}

	if (glm::length(m_Rigidbody->GetVelocity()) > m_MaxSpeed)
	{
		m_Rigidbody->SetVelocity(glm::normalize(m_Rigidbody->GetVelocity()) * m_MaxSpeed);
	}
}

void dae::CMovement2D::SetMaxSpeed(float maxSpeed)
{
	m_MaxSpeed = maxSpeed < 0 ? 0 : maxSpeed;
}

void dae::CMovement2D::SetMaxAccelForce(float maxForce)
{
	m_MaxForce = maxForce < 0 ? 0 : maxForce;
}

void dae::CMovement2D::AddSingleFrameMovementInput(const glm::vec2& input)
{
	m_MovementInput += input;
}

