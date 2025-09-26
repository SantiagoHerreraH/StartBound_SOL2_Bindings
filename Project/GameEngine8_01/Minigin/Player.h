#pragma once

#include "GameObject.h"
#include "Gun.h"
#include "EventSystem.h"
#include "Rect.h"
#include "../src/GameEngine.h"

namespace dae {

	struct PlayerType {

		int MaxHealth{ 1 };
		GunType GunType{};
	};

	class Player
	{
	public:
		Player() {}
		Player(std::string sceneNameOnPlayerDie, const PlayerType& PlayerType);

		GameObject& GetGameObject() { return *m_CurrentPlayer; }
		GameObjectHandle GetGameObjectHandle();

		void SubscribeOnDie(const std::function<void()>& func);

		void AddScene(Scene& scene);

	private:
		GameObjectHandle m_CurrentPlayer;

	};

	void BindPlayerShoot(GameObjectHandle player);
}