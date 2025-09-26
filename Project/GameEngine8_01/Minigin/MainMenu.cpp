#include "MainMenu.h"
#include "SceneManager.h"
#include "TextCreator.h"
#include "Settings.h"
#include "CButtonGrid.h"
#include "Misc_CreationFunctions.h"

dae::MainMenu::MainMenu(MainMenuData mainMenuData)
{

	auto sceneFunc = [mainMenuData](Scene& scene)
		{
			auto particlySystem = CreateParticleSystem();
			scene.AddGameObjectHandle(particlySystem);

			//------ GRID CREATION

			auto buttonGridGO = scene.CreateGameObject();

			ButtonGridData buttonGridData{ };
			buttonGridData.ColumnNumber = 1;
			buttonGridData.OffsetBetweenCols = 0;
			buttonGridData.OffsetBetweenRows = 50;
			buttonGridData.StartPos = { int(g_WindowWidth / 2.f), int(g_WindowHeight / 2.f) + 60 };
			CButtonGrid buttonGrid{ buttonGridData };

			auto gridRef = buttonGridGO->AddComponent(buttonGrid);

			//------ BUTTON CREATION

			std::string name = "PLAY";
			ButtonData data{};
			data.FontData.FontFile = "Emulogic-zrEw.ttf";
			data.FontData.FontSize = 25;
			data.Name = name;
			data.OnPress.Subscribe([mainMenuData]()
				{
					SceneManager::GetInstance().ChangeCurrentScene(mainMenuData.NextSceneName);
				});
			data.SelectedColor = RGB(255, 0, 0);
			data.UnselectedColor = RGB(255, 255, 255);

			CButton button{ data };
			auto gameObj = std::make_shared<GameObject>();
			gameObj->AddComponent(button);

			gridRef->AddButton(gameObj);


			TextCreator textCreatorBack("STAR BOUND", { int(g_WindowWidth / 2.f),   int(g_WindowHeight / 2.f) + 2}, 31, RGB(255, 0, 0));
			scene.AddGameObjectHandle(textCreatorBack.GetGameObjectHandle());

			TextCreator textCreatorOne("STAR BOUND", { int(g_WindowWidth / 2.f),   int(g_WindowHeight/ 2.f)}, 30, RGB( 255, 255 , 255 ));
			scene.AddGameObjectHandle(textCreatorOne.GetGameObjectHandle());


			TextCreator highscoreText("PRESS SPACE", { g_WindowWidth / 2.f,      (g_WindowHeight / 2.f)  + 90}, 13, RGB(255, 255, 255));
			scene.AddGameObjectHandle(highscoreText.GetGameObjectHandle());
		
		
		};

	SceneManager::GetInstance().AddScene(mainMenuData.SceneName, sceneFunc);
}
