#pragma once

#include "Follow.h"
#include "Gun.h"
#include "Rect.h"
#include "Scene.h"

namespace dae {


	struct EnemyType {
		std::string ImageFile{};
		FollowData FollowData{};
		GunType GunType{};
		float DistanceFromTargetToShoot{};

		float BodyDamage{};
		float BodyPushForce{};

		int MaxHealth{};
		int Points{};
	};

	class Enemy {

	public:
		Enemy(const EnemyType& enemyType, GameObjectHandle targetPlayer, const glm::vec2& startPos);

		GameObject& GetGameObject() { return *m_EnemyHandle; }
		GameObjectHandle GetGameObjectHandle() { return m_EnemyHandle; }

		void AddScene(Scene& scene);

	private:
		GameObjectHandle m_EnemyHandle;
	};
}