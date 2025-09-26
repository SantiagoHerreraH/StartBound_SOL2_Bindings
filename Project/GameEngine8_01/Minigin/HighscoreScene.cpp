#include "HighscoreScene.h"
#include "SceneManager.h"
#include "TextCreator.h"
#include "ScoreSaver.h"
#include "GameStats.h"
#include "StatDisplayer.h"
#include "PlayerController.h"
#include "../3rdParty/glm/glm.hpp"
#include "EventTriggerCommand.h"
#include "Misc_CreationFunctions.h"

namespace dae {
	HighscoreScene::HighscoreScene(const std::string& sceneName, std::string nextSceneName, GameObjectHandle player) :
		m_SceneName(sceneName)
	{

		auto sceneFunction = [player, nextSceneName](Scene& scene) mutable
			{

                auto particlySystem = CreateParticleSystem();
                scene.AddGameObjectHandle(particlySystem);

                ScoreSaver scoreSaver{};

                ScoreData scoreData{};
                scoreData.PlayerName = player->GetName();
                scoreData.Score = player->GetComponent<CGameStatController>()->GetStat(StatType::Points, StatCategory::CurrentStat);

                float vertOffsetBetweenDisplayers{ 60.f };
                float xWhere{ g_WindowWidth/3.f};

                TextCreator currentText{ scoreData.PlayerName, {xWhere, (g_WindowHeight / 2.f) - (vertOffsetBetweenDisplayers * 2)}, 15, RGB(255, 255, 255) };
                scene.AddGameObjectHandle(currentText.GetGameObjectHandle());

                GameObjectHandle statDisplayerGameObj = scene.CreateGameObject();

                StatDisplayData pointsDisplayData{};
                pointsDisplayData.StatNameTextData.FontData.FontFile = "Emulogic-zrEw.ttf";
                pointsDisplayData.StatNameTextData.FontData.FontSize = 13;
                pointsDisplayData.StatNameTextData.Color = RGB( 255, 0, 0 );
                pointsDisplayData.StatNameTextData.Text = "POINTS :";
                pointsDisplayData.StatTypeToDisplay = StatType::Points;
                pointsDisplayData.StatValueColor = RGB( 255, 255, 255 );
                pointsDisplayData.StatValueOffsetFromStatName = { 0, 5 };
                pointsDisplayData.StatValueBaseOffsetMultiplierX = 0;
                pointsDisplayData.StatValueBaseOffsetMultiplierY = 1;
                pointsDisplayData.UpdateAutomatically = false;
                pointsDisplayData.Where = { xWhere, (g_WindowHeight / 2.f) - vertOffsetBetweenDisplayers };
                pointsDisplayData.FromWho = player;

                CStatDisplayer pointsDisplayer{ pointsDisplayData };

                statDisplayerGameObj->AddComponent(pointsDisplayer);


                //------------

                StatDisplayData shotsFiredDisplayData{};
                shotsFiredDisplayData.StatNameTextData.FontData.FontFile = "Emulogic-zrEw.ttf";
                shotsFiredDisplayData.StatNameTextData.FontData.FontSize = 13;
                shotsFiredDisplayData.StatNameTextData.Color = RGB(255, 0, 0);
                shotsFiredDisplayData.StatNameTextData.Text = "SHOTS FIRED :";
                shotsFiredDisplayData.StatTypeToDisplay = StatType::ShotsFired;
                shotsFiredDisplayData.StatValueColor = RGB(255, 255, 255);
                shotsFiredDisplayData.StatValueOffsetFromStatName = { 0,5 };
                shotsFiredDisplayData.StatValueBaseOffsetMultiplierX = 0;
                shotsFiredDisplayData.StatValueBaseOffsetMultiplierY = 1;
                shotsFiredDisplayData.Where = { xWhere , (g_WindowHeight / 2.f) };
                shotsFiredDisplayData.FromWho = player;
                shotsFiredDisplayData.UpdateAutomatically = false;

                CStatDisplayer shotsFiredDisplayer{ shotsFiredDisplayData };

                statDisplayerGameObj->AddComponent(shotsFiredDisplayer);

                //------------

                StatDisplayData hitNumDisplayData{};
                hitNumDisplayData.StatNameTextData.FontData.FontFile = "Emulogic-zrEw.ttf";
                hitNumDisplayData.StatNameTextData.FontData.FontSize = 13;
                hitNumDisplayData.StatNameTextData.Color = RGB(255, 0, 0);
                hitNumDisplayData.StatNameTextData.Text = "HIT NUM :";
                hitNumDisplayData.StatTypeToDisplay = StatType::NumberOfHits;
                hitNumDisplayData.StatValueColor = RGB(255, 255, 255);
                hitNumDisplayData.StatValueOffsetFromStatName = { 0,5 };
                hitNumDisplayData.StatValueBaseOffsetMultiplierX = 0;
                hitNumDisplayData.StatValueBaseOffsetMultiplierY = 1;
                hitNumDisplayData.Where = { xWhere, (g_WindowHeight / 2.f) + vertOffsetBetweenDisplayers };
                hitNumDisplayData.FromWho = player;
                hitNumDisplayData.UpdateAutomatically = false;


                CStatDisplayer hitNumDisplayer{ hitNumDisplayData };

                statDisplayerGameObj->AddComponent(hitNumDisplayer);

                //--------- HIT MISS RATIO

                float shotsFired = player->GetComponent<CGameStatController>()->GetStat(StatType::ShotsFired);
                float numberOfHits = player->GetComponent<CGameStatController>()->GetStat(StatType::NumberOfHits);
                float hitMissRatioVal = shotsFired != 0 ? (numberOfHits * 100) / shotsFired : 0;

                TextCreator hitMissRatioText("HIT MISS RATIO: ", { xWhere,   (g_WindowHeight / 2.f) + (vertOffsetBetweenDisplayers * 2) }, 13, RGB( 255, 0 , 0 ));
                TextCreator hitMissRatio("% " + std::to_string(hitMissRatioVal), { xWhere,   (g_WindowHeight / 2.f) + (vertOffsetBetweenDisplayers * 2) + 18 }, 13, RGB( 255, 255 , 255 ));

                scene.AddGameObjectHandle(hitMissRatioText.GetGameObjectHandle());
                scene.AddGameObjectHandle(hitMissRatio.GetGameObjectHandle());

                //--------- HIGHSCORE 

                scoreSaver.AddScore(scoreData);
                ScoreData highScoreData = scoreSaver.GetHighscore();

                TextCreator highscoreText("HIGHSCORE: ",                            { g_WindowWidth * (2.f / 3.f),      (g_WindowHeight / 2.f)},        13, RGB(255, 0, 0));
                TextCreator highscoreName( std::to_string(highScoreData.Score),     { g_WindowWidth * (2.f/3.f),        (g_WindowHeight / 2.f)  + 22 }, 13, RGB(255, 255, 255));
                TextCreator highscoreValue(highScoreData.PlayerName,                { g_WindowWidth * (2.f/3.f),        (g_WindowHeight / 2.f)  + 44 }, 13, RGB(255, 255, 255));
                TextCreator menuInstructions("PRESS RETURN TO GO TO MAIN MENU",       { g_WindowWidth * (2.f / 3.f),      (g_WindowHeight / 2.f)  + 66},  13, RGB(255, 255, 255));

                scene.AddGameObjectHandle(highscoreText.GetGameObjectHandle());
                scene.AddGameObjectHandle(highscoreName.GetGameObjectHandle());
                scene.AddGameObjectHandle(highscoreValue.GetGameObjectHandle());
                scene.AddGameObjectHandle(menuInstructions.GetGameObjectHandle());



                //--------- ACTIVATE PLAYER

                player->SetActive(true);
                scene.AddGameObjectHandle(player);

                //--------- BACK TO MAIN MENU BINDINGS

                auto goToMainMenu = [nextSceneName](GameObject&) {SceneManager::GetInstance().ChangeCurrentScene(nextSceneName); };
                Event<GameObject&> backToMainMenuEvent{};
                backToMainMenuEvent.Subscribe(goToMainMenu);

                CPlayerController& playerController = *menuInstructions.GetGameObject().AddComponent<CPlayerController>(CPlayerController{});
                playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_DOWN, VK_RETURN,	     std::make_shared<EventTriggerCommand>(backToMainMenuEvent) });

                //---------

                

			};

		SceneManager::GetInstance().AddScene(sceneName, sceneFunction);
	}
}