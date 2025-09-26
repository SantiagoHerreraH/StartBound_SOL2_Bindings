#pragma once

#include "Component.h"
#include "TimerSystem.h"
#include "CollisionLayers.h"
#include "Audio.h"
#include "Command.h"

namespace dae {

	struct GunType {

		std::string BulletImageFile{};
		int BulletAmount{ 10 };
		float BulletLifeTime{};
		float BulletSpeed{ 500 };
		float TimeBetweenShots{ 0.3f };
		float BulletPushForce{ 300.f };
		int BulletDamage{};

	};

	struct GunData {

		GunType GunType;
		CollisionLayers BulletCollisionLayer;
		CollisionLayers BulletCollisionLayerToCollideAgainst;
		
		GameObjectHandle Shooter;
	};

	class CGun : public Component
	{
	public:
		CGun(const GunData& gunData);
		void Start()override;
		void Shoot(const glm::vec2& direction);

	private:
		GunData m_GunData;
		std::shared_ptr<CAudio> m_ShootAudio;

		size_t m_CurrentBulletIndex{0};
		TimerSystem* m_CurrentTimerSystem;
		TimerKey m_TimeBetweenShotsTimerKey;

		std::vector<GameObjectHandle> m_Bullets;
	};

}


