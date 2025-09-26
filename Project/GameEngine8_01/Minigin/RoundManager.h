#pragma once

#include "Player.h"
#include "Enemy.h"


namespace dae {

	struct RoundParams
	{
		float SecondsBetweenSpawning;
		int InitialMaxEnemyNumber;
		float SecondsToIncreaseMaxEnemyNumByOne;
		std::vector<EnemyType> EnemyTypes;
		void AddEnemyType(const EnemyType& enemyType) {
			EnemyTypes.push_back(enemyType);
		}
	};

	class RoundManager final {

	public:
		RoundManager(const RoundParams& roundParams, const std::string& sceneRoundName, GameObjectHandle player);
		const std::string& GetName() const{ return m_SceneRoundName; }

	private:
		RoundParams m_RoundParams;
		std::string m_SceneRoundName;

		struct RoundData {

			std::vector<Enemy> Enemies;
		};

		std::shared_ptr<RoundData> m_RoundData;

	};
}