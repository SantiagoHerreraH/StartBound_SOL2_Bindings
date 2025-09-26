#include "EnemyAI.h"

namespace dae {
	void CEnemyAI::Start()
	{
		m_Gun = Owner().GetComponentHandle<CGun>();
		assert(m_Gun);
		assert(m_EnemyAIData.Target);
	}
	void CEnemyAI::Update()
	{
		glm::vec2 delta = m_EnemyAIData.Target->Transform().GetWorldTransform().Position - Owner().Transform().GetWorldTransform().Position;

		if (glm::length(delta) <= m_EnemyAIData.DistanceFromTargetToShoot)
		{
			m_Gun->Shoot(delta);
		}
	}
	void CEnemyAI::Render() const
	{
		glm::vec2 pos = OwnerConst().TransformConst().GetWorldTransform().Position;
		float radius = m_EnemyAIData.DistanceFromTargetToShoot;
		GAME_ENGINE->SetColor(RGB(100, 0, 0));
		GAME_ENGINE->DrawOval(pos.x - radius, pos.y - radius, pos.x + radius, pos.y + radius);
	}
}