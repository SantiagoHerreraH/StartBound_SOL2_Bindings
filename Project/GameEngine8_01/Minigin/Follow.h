#pragma once

#include "../3rdParty/glm/glm.hpp"
#include "Component.h"
#include <vector>


namespace dae {

	class CMovement2D;

	struct FollowData {
		std::vector<glm::vec2> PossibleMovementDirections;// if empty, all are possible
		float Speed;
		float TimeToChangeDirection;
		float OptimalDistanceFromTarget;
	};

	class CFollow : public Component {

	public:
		CFollow(GameObjectHandle target, const FollowData& followData);
		void Start()override;
		void Update()override;

	private:
		float m_TimeSinceLastChangedDirection;
		GameObjectHandle m_Target;
		FollowData m_FollowData;
		std::shared_ptr<CMovement2D> m_Movement;
		glm::vec2 m_ChosenDelta{};

	};
}