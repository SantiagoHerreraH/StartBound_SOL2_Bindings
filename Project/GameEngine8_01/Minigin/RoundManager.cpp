
#include "RoundManager.h"
#include "SceneManager.h"
#include "Random.hpp"
#include "Settings.h"
#include "TimerSystem.h"
#include "Misc_CreationFunctions.h"
#include "Rigidbody.h"
#include "GameStats.h"

namespace dae {

	RoundManager::RoundManager(const RoundParams& roundParams, const std::string& sceneRoundName, GameObjectHandle player) :
		m_RoundData(std::make_shared<RoundData>()),
		m_SceneRoundName(sceneRoundName)
	{
		RoundParams params = roundParams;
		auto roundData = m_RoundData;

		assert(!roundParams.EnemyTypes.empty());

		auto sceneFunc = [params, roundData, player](Scene& scene) mutable
			{

				auto particlySystem = CreateParticleSystem();
				scene.AddGameObjectHandle(particlySystem);

				Rect rectPerimeter{};
				rectPerimeter.Top = 0;
				rectPerimeter.Left = 0;
				rectPerimeter.Width = g_WindowWidth;
				rectPerimeter.Height = g_WindowHeight;


				roundData->Enemies.clear();
				glm::vec2 startPos{};
				int currentChosenIdx{};
				for (size_t i = 0; i < params.InitialMaxEnemyNumber; i++)
				{
					currentChosenIdx = Random::GetRandomBetweenRange(0, params.EnemyTypes.size() - 1);
					startPos = Random::GetRandomInRectPerimeter(rectPerimeter);
					roundData->Enemies.push_back(Enemy{ params.EnemyTypes[currentChosenIdx], player, startPos});
					roundData->Enemies.back().AddScene(scene);
					roundData->Enemies.back().GetGameObject().SetActive(false);
				}


				TimerSystem* TIMERSYSTEM = &TimerSystem::GetFromScene(&scene);

				//---- SPAWN ENEMIES

				TimerKey spawnEnemyTimerKey =  TIMERSYSTEM->AddTimer(Timer{ params.SecondsBetweenSpawning, false, "SpawnEnemy"});

				TIMERSYSTEM->TimerAt(spawnEnemyTimerKey).GetOnRestartEvent().Subscribe([rectPerimeter, roundData]()
					{
						int currentIdx = Random::GetRandomBetweenRange(0, roundData->Enemies.size() - 1);
						int iterationNum = 0;

						while (iterationNum < roundData->Enemies.size())
						{
							if (!roundData->Enemies[currentIdx].GetGameObject().IsActive())
							{
								glm::vec2 pos = Random::GetRandomInRectPerimeter(rectPerimeter);
								roundData->Enemies[currentIdx].GetGameObject().Transform().SetLocalPositionX(pos.x);
								roundData->Enemies[currentIdx].GetGameObject().Transform().SetLocalPositionY(pos.y);
								roundData->Enemies[currentIdx].GetGameObject().GetComponent<CRigidbody2D>()->SetVelocity(glm::vec2{0,0}, SetVelocityMode::NullForces);
								roundData->Enemies[currentIdx].GetGameObject().SetActive(true);
								break;
							}

							currentIdx = (currentIdx + 1) % roundData->Enemies.size();
							++iterationNum;
	
						}
					
					});
				TIMERSYSTEM->TimerAt(spawnEnemyTimerKey).GetOnEndEvent().Subscribe([spawnEnemyTimerKey, TIMERSYSTEM]()
					{
						TIMERSYSTEM->RestartTimer(spawnEnemyTimerKey);
					
					});

				//----- Increase Enemy Num

				TimerKey increaseEnemyTimerKey = TIMERSYSTEM->AddTimer(Timer{ params.SecondsToIncreaseMaxEnemyNumByOne, false, "IncreaseEnemyNum" });

				TIMERSYSTEM->TimerAt(increaseEnemyTimerKey).GetOnRestartEvent().Subscribe([params, rectPerimeter, roundData, player]() mutable
					{
						int currentChosenIdx = Random::GetRandomBetweenRange(0, params.EnemyTypes.size() - 1);
						glm::vec2 startPos = Random::GetRandomInRectPerimeter(rectPerimeter);
						roundData->Enemies.push_back(Enemy{ params.EnemyTypes[currentChosenIdx], player, startPos });
						roundData->Enemies.back().GetGameObject().SetActive(true);

					});
				TIMERSYSTEM->TimerAt(increaseEnemyTimerKey).GetOnEndEvent().Subscribe([increaseEnemyTimerKey, TIMERSYSTEM]()
					{
						TIMERSYSTEM->RestartTimer(increaseEnemyTimerKey);

					});


				scene.AddGameObjectHandle(player);
				player->GetComponent<CGameStatController>()->ResetCurrentStat(StatType::Health);
				player->GetComponent<CGameStatController>()->SetStat(StatType::Points, 0);
				player->GetComponent<CGameStatController>()->SetStat(StatType::NumberOfHits, 0);
				player->GetComponent<CGameStatController>()->SetStat(StatType::ShotsFired, 0);
				
			};

		SceneManager::GetInstance().AddScene(m_SceneRoundName, sceneFunc);
	}

}
