#include "HealthBar.h"
#include "GameStats.h"
#include "GameObject.h"
#include "../src/GameEngine.h"

namespace dae {
	CHealthBar::CHealthBar(const HealthBarData& healthBarData) : m_HealthBarData(healthBarData),
		m_Percentage(std::make_shared<float>())
	{
		*m_Percentage = 1;
	}
	void CHealthBar::Start()
	{
		std::shared_ptr<CGameStatController> gameStatController = Owner().GetComponentHandle<CGameStatController>();
		
		auto percentage = m_Percentage;
		assert(gameStatController);
		assert(gameStatController->HasStat(StatType::Health));

		gameStatController->OnCurrentStatChange(StatType::Health).Subscribe([gameStatController, percentage](StatValue currentValue)
			{

				*percentage = currentValue/(float)gameStatController->GetStat(StatType::Health, StatCategory::MaxStat);
			});


		*percentage = 
			gameStatController->GetStat(StatType::Health) /
			(float)gameStatController->GetStat(StatType::Health, StatCategory::MaxStat);
	}
	void CHealthBar::Render() const
	{
		Rect worldRect = m_HealthBarData.Rect;

		worldRect.Left += OwnerConst().TransformConst().GetWorldTransform().Position.x;
		worldRect.Top += OwnerConst().TransformConst().GetWorldTransform().Position.y;

		GAME_ENGINE->SetColor(m_HealthBarData.Color);
		GAME_ENGINE->DrawRect(worldRect.Left, worldRect.Top, worldRect.Right(), worldRect.Bottom());

		worldRect.Width *= *m_Percentage;
		GAME_ENGINE->FillRect(worldRect.Left, worldRect.Top, worldRect.Right(), worldRect.Bottom());
	}
}