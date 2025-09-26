//-----------------------------------------------------------------
// Main Game File
// C++ Source - Game.cpp - version v8_01
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Game.h"
#include "../Minigin/SceneManager.h"
#include "../Minigin/InputManager.h"
#include "../Minigin/ResourceManager.h"
#include "../Minigin/TimerSystem.h"
#include "../Minigin/TimerSystem.h"


//-----------------------------------------------------------------
// Game Member Functions																				
//-----------------------------------------------------------------

Game::Game() 																	
{
	
}

Game::~Game()																						
{
	
}


void Game::Initialize()			
{
	AbstractGame::Initialize();

}

void Game::Awake()
{
	auto currentPath = std::filesystem::current_path();
	for (std::filesystem::path dir = currentPath; !dir.empty(); dir = dir.parent_path()) {
		
		auto dataPath = dir / "Data";
		if (std::filesystem::exists(dataPath) && std::filesystem::is_directory(dataPath))
		{
			currentPath = dir / "Data\\";
			break;
		}
	}
	dae::ResourceManager::GetInstance().Init(currentPath.string());
	dae::LUA_CONTROLLER->CreateGame();
	dae::SceneManager::GetInstance().Start();
	m_AwakeCalled =  true;

}

void Game::Start()
{
}

void Game::End()
{
	if (m_AwakeCalled)
	{
		dae::SceneManager::GetInstance().End();
	}
}

void Game::Paint(RECT rect) const
{
	if (m_AwakeCalled)
	{
		GAME_ENGINE->FillWindowRect(RGB(0, 0, 0));
		dae::SceneManager::GetInstance().Render();
		dae::InputManager::GetCurrent().Render();
	}
}

void Game::Tick()
{
	if (m_AwakeCalled)
	{
		dae::SceneManager::GetInstance().FixedUpdate(); //This engine only has a steady framerate, so both are fixed updates, but for now, I will just keep it like so
		dae::SceneManager::GetInstance().Update();
		dae::TimerSystem::GetCurrent().Update();
	}
}

void Game::MouseButtonAction(bool isLeft, bool isDown, int x, int y, WPARAM wParam)
{	
	if (m_AwakeCalled)
	{
		dae::InputManager::GetCurrent().UpdateMouseEvents(isDown, isLeft);
	}
}

void Game::MouseWheelAction(int x, int y, int distance, WPARAM wParam)
{	
	
}

void Game::MouseMove(int x, int y, WPARAM wParam)
{	
	dae::g_MousePosX = x;
	dae::g_MousePosY = y;

	if (m_AwakeCalled)
	{
		dae::InputManager::GetCurrent().RegisterMousePos(glm::ivec2{ x, y });
	}
}

void Game::CheckKeyboard()
{	
	// Here you can check if a key is pressed down
	// Is executed once per frame 

	/* Example:
	*/

	dae::InputManager::GetCurrent().Update();
}

void Game::KeyPressed(TCHAR key)
{	
	/* Example:
	switch (key)
	{
	case _T('K'): case VK_LEFT:
		GAME_ENGINE->MessageBox("Moving left.");
		break;
	case _T('L'): case VK_DOWN:
		GAME_ENGINE->MessageBox("Moving down.");
		break;
	case _T('M'): case VK_RIGHT:
		GAME_ENGINE->MessageBox("Moving right.");
		break;
	case _T('O'): case VK_UP:
		GAME_ENGINE->MessageBox("Moving up.");
		break;
	case VK_ESCAPE:
		GAME_ENGINE->MessageBox("Escape menu.");
	}
	*/
}

void Game::CallAction(Caller* callerPtr)
{
	// Insert the code that needs to execute when a Caller (= Button, TextBox, Timer, Audio) executes an action
}




