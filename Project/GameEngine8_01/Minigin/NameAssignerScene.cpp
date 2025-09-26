#include "NameAssignerScene.h"
#include "Scene.h"
#include "CButtonGrid.h"
#include "Settings.h"
#include "TextCreator.h"
#include "EventTriggerCommand.h"

namespace dae {

	NameAssignerScene::NameAssignerScene(const NameAssignerSceneData& data)
	{
		m_NameAssignerSceneInternalData = std::make_shared<NameAssignerSceneInternalData>();
		auto internalData = m_NameAssignerSceneInternalData;
		internalData->NameAssignerSceneData = data;

		auto sceneCreationFunction = [internalData](Scene& scene)
			{

				internalData->NameAssignerSceneData.Player->SetName("");
				TextCreator currentText{ "", {100, 50}, 25, RGB(255, 255, 255) };
				scene.AddGameObjectHandle(currentText.GetGameObjectHandle());
				auto updateText = [internalData, currentText]() mutable
					{
						auto& currentObj = *internalData->NameAssignerSceneData.Player;
						const std::string& name = currentObj.GetName();
						currentText.SetText(name);
					};

				ButtonGridData buttonGridData{};
				buttonGridData.ColumnNumber = 8;
				buttonGridData.OffsetBetweenCols = 50;
				buttonGridData.OffsetBetweenRows = 50;
				buttonGridData.StartPos = {100, 150};

				auto buttonGridGO = scene.CreateGameObject();
				auto buttonGrid = buttonGridGO->AddComponent(CButtonGrid{ buttonGridData });
				auto playerController = buttonGridGO->AddComponent(CPlayerController{});
			

				ButtonData buttonData{};
				buttonData.FontData.FontFile = "Emulogic-zrEw.ttf";
				buttonData.FontData.FontSize = 40;

				const int maxCharsInName = 5;

				for (char currentLetter = 'A'; currentLetter <= 'Z'; currentLetter++)
				{
					buttonData.OnPress.UnsubscribeAll();
					buttonData.Name = currentLetter;
					buttonData.OnPress.Subscribe(
						[internalData, currentLetter, updateText, maxCharsInName]() mutable
						{
							auto currentObj = internalData->NameAssignerSceneData.Player;
							const std::string& name = currentObj->GetName();
							if (name.size() < maxCharsInName)
							{
								currentObj->SetName(name + currentLetter);
								updateText();
							}
						
						}); //add letter to name
					buttonData.SelectedColor = RGB( 255,0,0 );
					buttonData.UnselectedColor = RGB( 255, 255 , 255 );


					auto buttonGO = scene.CreateGameObject();
					buttonGO->AddComponent(CButton{ buttonData });
					buttonGrid->AddButton(buttonGO);
				}

				auto nextScene = [internalData, updateText, currentText](GameObject&) mutable
					{
						SceneManager::GetInstance().ChangeCurrentScene(internalData->NameAssignerSceneData.SceneNameUponCompletion);
					};

				auto deleteLetter = [internalData, updateText](GameObject&) mutable
					{
						auto& currentObj = *internalData->NameAssignerSceneData.Player;
						std::string name = currentObj.GetName();
						if (!name.empty())
						{
							name.pop_back();
						}
						currentObj.SetName(name);
						updateText();
					};

				Event<GameObject&> nextGameObjectEvent{};
				nextGameObjectEvent.Subscribe(nextScene);

				Event<GameObject&> deleteLetterEvent{};
				deleteLetterEvent.Subscribe(deleteLetter);


				playerController->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_DOWN, VK_RETURN,			 std::make_shared<EventTriggerCommand>(nextGameObjectEvent) });

				playerController->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_DOWN, VK_BACK,			std::make_shared<EventTriggerCommand>(deleteLetterEvent) });

				//----STRINGS

				std::string maxChars = "MAX " + std::to_string(maxCharsInName) + " CHARACTERS IN NAME";

				TextCreator keyGuide{ "KEYBOARD GUIDE",					{g_WindowWidth / 3.f,  g_WindowHeight - 150}, 14, RGB(255, 0, 0) };
				TextCreator charsIndication{ maxChars,					{g_WindowWidth / 3.f,  g_WindowHeight - 120}, 12, RGB(255, 255, 255)};
				TextCreator enterText{ "PRESS SPACE TO INPUT LETTER",	{g_WindowWidth / 3.f,  g_WindowHeight - 90}, 12, RGB(255, 255, 255) };
				TextCreator deleteText{ "PRESS BACKSPACE TO DELETE",	{g_WindowWidth / 3.f,  g_WindowHeight - 60}, 12, RGB(255, 255, 255) };
				TextCreator acceptText{ "PRESS RETURN TO ACCEPT",		{g_WindowWidth / 3.f,  g_WindowHeight - 30}, 12, RGB(255, 255, 255) };



				scene.AddGameObjectHandle(keyGuide.GetGameObjectHandle());
				scene.AddGameObjectHandle(charsIndication.GetGameObjectHandle());
				scene.AddGameObjectHandle(enterText.GetGameObjectHandle());
				scene.AddGameObjectHandle(deleteText.GetGameObjectHandle());
				scene.AddGameObjectHandle(acceptText.GetGameObjectHandle());

				
				
				//----

				TextCreator gameRules{ "GAME RULES",						{g_WindowWidth * (2/ 3.f),  g_WindowHeight - 120}, 14, RGB(255, 0, 0) };
				TextCreator moveIndications{ "WASD OR ARROWS TO MOVE",		{g_WindowWidth * (2/ 3.f),  g_WindowHeight - 90}, 12, RGB(255, 255, 255) };
				TextCreator aimIndications{ "MOUSE TO AIM",					{g_WindowWidth * (2/ 3.f),  g_WindowHeight - 60}, 12, RGB(255, 255, 255) };
				TextCreator shootIndications{ "LEFT CLICK TO SHOOT",		{g_WindowWidth * (2/ 3.f),  g_WindowHeight - 30}, 12, RGB(255, 255, 255) };

				scene.AddGameObjectHandle(gameRules.GetGameObjectHandle());
				scene.AddGameObjectHandle(moveIndications.GetGameObjectHandle());
				scene.AddGameObjectHandle(aimIndications.GetGameObjectHandle());
				scene.AddGameObjectHandle(shootIndications.GetGameObjectHandle());

			};

		SceneManager::GetInstance().AddScene(data.SceneName, sceneCreationFunction);

	}

}