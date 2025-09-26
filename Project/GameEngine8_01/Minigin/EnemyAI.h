#pragma once

#include "Component.h"
#include "GameObject.h"
#include "Gun.h"

namespace dae {

	struct EnemyAIData {
		GameObjectHandle Target;
		float DistanceFromTargetToShoot;
	};
	
	class CEnemyAI : public Component {

	public:
		CEnemyAI(const EnemyAIData& enemyAIData) : m_EnemyAIData(enemyAIData) {}
		void Start()override;
		void Update()override;
		void Render()const override;

	private:
		EnemyAIData m_EnemyAIData;
		std::shared_ptr<CGun> m_Gun;
	};
}