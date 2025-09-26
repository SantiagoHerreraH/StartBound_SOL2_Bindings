#pragma once

#include <string>

namespace dae {

	struct MainMenuData {
		std::string SceneName;
		std::string NextSceneName;
	};

	class MainMenu {

	public:
		MainMenu(MainMenuData mainMenuData);
		const std::string& GetName()const { return m_MainMenuData.SceneName; }

	private:
		MainMenuData m_MainMenuData;

	};
}