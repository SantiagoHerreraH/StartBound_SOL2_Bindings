#pragma once

#include "StatSystem.h"
#include "GameStats.h"

namespace dae {
	
	struct StatModifierData {
		StatType StatToModify;
		StatModifierType StatModifierType;
		StatValue InitialStatValue;
	};

	class CCollider;


	class CStatModifier : public Component {

	public:
		CStatModifier(const StatModifierData& statModifierData);
		void Start()override;
		GameStat& Stat();


	private:
		StatModifierData m_StatModifierData;

		std::shared_ptr<GameStat> m_StatModifierStat;
		std::shared_ptr<CGameStatController> m_CGameStatController;
		std::shared_ptr<CCollider> m_Collider;
	};
}