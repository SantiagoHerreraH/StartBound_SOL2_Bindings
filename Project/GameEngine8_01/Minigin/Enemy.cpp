#include "Enemy.h"
#include "GameObject.h"
#include "Gun.h"
#include "Boundary.h"
#include "Collider.h"
#include "Rigidbody.h"
#include "Movement.h"
#include "GameStats.h"
#include "Settings.h"
#include "Follow.h"
#include "EnemyAI.h"
#include "StatModifier.h"
#include "ForceApplier.h"
#include "Animation.h"
#include "HealthBar.h"

#undef max
#undef min
#include <limits>

namespace dae {

	Enemy::Enemy(const EnemyType& enemyType, GameObjectHandle targetPlayer, const glm::vec2& startPos)
	{	
		//------- PLAYER

		dae::GameObjectHandle currentEnemy = std::make_shared<GameObject>();

		//----- COMPONENTS -----

		CTextureHandle texture { enemyType.ImageFile };

		SpriteSheet spriteSheet{ texture, 1, 3 };

		CAnimation animation{};
		animation.SetFramesPerSecond(6);
		animation.CanLoop(true);
		animation.AddFrame(spriteSheet.GetFrame(0, 0), 1);
		animation.AddFrame(spriteSheet.GetFrame(0, 1), 1);
		animation.AddFrame(spriteSheet.GetFrame(0, 2), 1);

		currentEnemy->AddComponent(animation);


		//------
		HealthBarData healthBarData{};
		healthBarData.Color = RGB(255, 0, 0);
		healthBarData.Rect.Top = -26;
		healthBarData.Rect.Left = -15;
		healthBarData.Rect.Width = 30;
		healthBarData.Rect.Height = 5;

		currentEnemy->AddComponent(CHealthBar{ healthBarData });


		//------
		
		ForceApplierData forceApplierData{};
		forceApplierData.PushForceOnStartCollision = enemyType.BodyPushForce;

		 currentEnemy->AddComponent(CForceApplier{ forceApplierData });

		//------

		GunData gunData{};
		gunData.GunType = enemyType.GunType;
		gunData.BulletCollisionLayer = CollisionLayers::EnemyBullet;
		gunData.BulletCollisionLayerToCollideAgainst = CollisionLayers::Player;
		gunData.Shooter = currentEnemy;

		CGun gun{ gunData };

		currentEnemy->AddComponent(gun);

		//------

		StatModifierData statModifierData{};
		statModifierData.InitialStatValue = -enemyType.BodyDamage;
		statModifierData.StatModifierType = StatModifierType::Offset;
		statModifierData.StatToModify = StatType::Health;

		currentEnemy->AddComponent(CStatModifier{ statModifierData });

		//------

		Rect rect{};
		rect.Height = spriteSheet.GetScaledCellHeight();
		rect.Width = spriteSheet.GetScaledCellWidth();

		dae::CCollider collider{ rect, (int)CollisionLayers::Enemy, true };
		collider.CenterRect();

		currentEnemy->AddComponent(collider);

		//-----

		dae::CRigidbody2D rigidBody{};
		rigidBody.SetMaxSpeed(500);
		currentEnemy->AddComponent(rigidBody);

		//-----

		currentEnemy->AddComponent(dae::CFollow{targetPlayer, enemyType.FollowData });


		//-----

		currentEnemy->AddComponent(dae::CEnemyAI{ EnemyAIData{targetPlayer, enemyType.DistanceFromTargetToShoot } });

		//-----

		dae::CMovement2D movement{};
		movement.SetMaxSpeed(30000);
		movement.SetMaxAccelForce(100000);
		currentEnemy->AddComponent(movement);

		//-----

		CGameStatController statController{};

		GameStat initialHealth{ };
		initialHealth.CurrentStat = enemyType.MaxHealth;
		initialHealth.MaxLimitStat =  enemyType.MaxHealth;
		initialHealth.MaxStat = enemyType.MaxHealth;


		constexpr int maxInt = std::numeric_limits<int>::max();

		GameStat initialPoints{ enemyType.Points, maxInt, maxInt };
		GameStat initialShotsFired{ 0, maxInt, maxInt };
		GameStat initialNumberOfHits{ 0, maxInt, maxInt };

		statController.CreateStat(StatType::Health, initialHealth);

		statController.CreateStat(StatType::Points, initialPoints);
		statController.CreateStat(StatType::ShotsFired, initialShotsFired);
		statController.CreateStat(StatType::NumberOfHits, initialNumberOfHits);

		auto statControllerRef = currentEnemy->AddComponent(statController);


		//-----

		currentEnemy->Transform().SetLocalPositionX(startPos.x);
		currentEnemy->Transform().SetLocalPositionY(startPos.y);

		//----- ACTIONS -----

		m_EnemyHandle = currentEnemy;

		statControllerRef->OnCurrentStatZeroOrLess(StatType::Health).Subscribe([currentEnemy]()mutable
			{
				currentEnemy->SetActive(false);
				currentEnemy->GetComponent<CGameStatController>()->ResetCurrentStat(StatType::Health);
				currentEnemy->GetComponent<CRigidbody2D>()->SetVelocity(glm::vec2{ 0,0 }, SetVelocityMode::NullForces);
			});
	}


	void Enemy::AddScene(Scene& scene) {

		scene.AddGameObjectHandle(m_EnemyHandle);
	}
}