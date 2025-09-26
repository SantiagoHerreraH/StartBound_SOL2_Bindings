#include "StatModifier.h"
#include "GameObject.h"
#include "Collider.h"
#include <assert.h>

namespace dae {
	
	CStatModifier::CStatModifier(const StatModifierData& statModifierData) :
		m_StatModifierData(statModifierData),
		m_StatModifierStat(std::make_shared<GameStat>())
	{
	}
	void CStatModifier::Start()
	{
		m_CGameStatController = Owner().GetComponentHandle<CGameStatController>();

		if (!m_CGameStatController)
		{
			m_CGameStatController = Owner().AddComponent<CGameStatController>(CGameStatController{});
		}


		auto stat = m_StatModifierStat;
		stat->CurrentStat = m_StatModifierData.InitialStatValue;
		stat->MaxLimitStat = m_StatModifierData.InitialStatValue;
		stat->MaxStat = m_StatModifierData.InitialStatValue;


		m_Collider = Owner().GetComponentHandle<CCollider>();

		assert(m_Collider);

		auto statModData = m_StatModifierData;

		m_Collider->OnCollisionBeginEvent().Subscribe([stat, statModData](GameObject& self, GameObject& other)
			{
				CGameStatController* otherStatController = other.GetComponent<CGameStatController>();
				CGameStatController* selfStatController = self.GetComponent<CGameStatController>();


				if (otherStatController && otherStatController->HasStat(statModData.StatToModify))
				{
					switch (statModData.StatModifierType)
					{
						case StatModifierType::Offset:

							otherStatController->OffsetStat(statModData.StatToModify, stat->CurrentStat);
							break;

						case StatModifierType::Scale:

							otherStatController->ScaleStat(statModData.StatToModify, stat->CurrentStat);
							break;
					default:
						break;
					}
				}
			
			});
	}
	GameStat& CStatModifier::Stat()
	{
		return *m_StatModifierStat;
	}
}