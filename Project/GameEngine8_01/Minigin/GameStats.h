#pragma once

#include "StatSystem.h"

namespace dae {

	enum class StatType {
		Health = 0,
		Points,
		ShotsFired,
		NumberOfHits
	};

	enum class StatModifierType {
		Offset,
		Scale

	};


	using StatValue = int;
	using StatKey = StatType;

	

	using GameStat = Stat<StatValue>;
	using CGameStatController = StatController<StatKey, StatValue>;
}