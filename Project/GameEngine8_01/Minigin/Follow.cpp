
#include "Follow.h"
#include "GameObject.h"
#include "Movement.h"
#include "GameTime.h"

namespace dae {
	CFollow::CFollow(GameObjectHandle target, const FollowData& followData) : 
		m_Target(target),
		m_FollowData(followData)
	{
		m_TimeSinceLastChangedDirection = m_FollowData.TimeToChangeDirection;
	}
	void CFollow::Start()
	{
		m_Movement = Owner().GetComponentHandle<CMovement2D>();

		if (!m_Movement)
		{
			m_Movement = Owner().AddComponent<CMovement2D>(CMovement2D{});
		}
	}
	void CFollow::Update()
	{
		m_TimeSinceLastChangedDirection += Time::GetInstance().GetElapsedSeconds();

		if (m_TimeSinceLastChangedDirection >= m_FollowData.TimeToChangeDirection)
		{
			m_TimeSinceLastChangedDirection = 0;

			glm::vec3 targetPos = m_Target->Transform().GetWorldTransform().Position;
			glm::vec3 ownerPos = Owner().Transform().GetWorldTransform().Position;

			glm::vec3 delta = targetPos - ownerPos;
			glm::vec3 optimalPoint = targetPos - (glm::normalize(delta) * m_FollowData.OptimalDistanceFromTarget);
			delta = optimalPoint - ownerPos;

			m_ChosenDelta = delta ;

			glm::vec2 deltaNorm = glm::normalize(delta);
			float highestDOT = -INFINITY;
			float currentDot{};
			for (size_t i = 0; i < m_FollowData.PossibleMovementDirections.size(); i++)
			{
				currentDot = glm::dot(deltaNorm, glm::normalize(m_FollowData.PossibleMovementDirections[i]));
				if (currentDot > highestDOT)
				{
					highestDOT = currentDot;
					m_ChosenDelta = m_FollowData.PossibleMovementDirections[i];
				}
			}

			Owner().GetComponent<CRigidbody2D>()->SetVelocity({ 0,0 });

		}


		m_Movement->SetMaxSpeed(m_FollowData.Speed);
		m_Movement->AddSingleFrameMovementInput(m_ChosenDelta);
	}
}
