#pragma once

#include "Player.h"

namespace dae {

	class HighscoreScene final{

	public:
		HighscoreScene(const std::string& sceneName, std::string nextSceneName, GameObjectHandle player);

	private:
		std::string m_SceneName;
	};
}