#pragma once

#include "Scene.h"
#include "Collider.h"
#include "Rigidbody.h"
#include "Texture.h"
#include "TimerSystem.h"

#include "GameStats.h"
#include "CollisionLayers.h"
#include "Lifetime.h"
#include "Animation.h"

namespace dae {

	inline dae::GameObjectHandle CreateBullet(GameObjectHandle owner, CollisionLayers collisionLayer, CollisionLayers collisionLayerToCollideAgainst, const GunType& gunType) {

		//----- COMPONENTS


		// -- Animation

		SpriteSheet spriteSheet{ CTextureHandle(gunType.BulletImageFile), 1, 3 };

		CAnimation animation{};
		animation.SetFramesPerSecond(6);
		animation.AddFrame(spriteSheet.GetFrame(0, 0), 1);
		animation.AddFrame(spriteSheet.GetFrame(0, 1), 1);
		animation.AddFrame(spriteSheet.GetFrame(0, 2), 1);

		// -- Collider

		Rect rect{};
		rect.Height = spriteSheet.GetScaledCellHeight();
		rect.Width = spriteSheet.GetScaledCellWidth();

		CCollider collider{ rect, int(collisionLayer), false};
		collider.CenterRect();
		collider.AddCollisionTagToCollideWith((int)collisionLayerToCollideAgainst);

		float pushForce = gunType.BulletPushForce;
		int damage = -gunType.BulletDamage;
		auto damageCollided = [damage, owner, pushForce](GameObject& self, GameObject& other) mutable
			{
				if (owner.get() != &other)
				{
					CGameStatController* otherStatController = other.GetComponent<CGameStatController>();
					CGameStatController* ownerStatController = owner->GetComponent<CGameStatController>();

					if (otherStatController)
					{

						otherStatController->OffsetStat(StatType::Health, damage);
						self.SetActive(false);

						CRigidbody2D* otherRigidBody = other.GetComponent<CRigidbody2D>();

						if (otherRigidBody)
						{
							glm::vec2 delta = other.Transform().GetWorldTransform().Position - self.Transform().GetWorldTransform().Position;

							otherRigidBody->SetVelocity(glm::normalize(delta) * pushForce);
						}

						if (ownerStatController && otherStatController->HasStat(StatType::Points))
						{
							ownerStatController->OffsetStat(StatType::Points, otherStatController->GetStat(StatType::Points, StatCategory::CurrentStat));
							ownerStatController->OffsetStat(StatType::NumberOfHits, 1);
						}
					}

				}
			};

		
		collider.OnCollisionBeginEvent().Subscribe(damageCollided);

		// -- Rigidbody

		dae::CRigidbody2D rigidBody{};


		// -- Lifetime

		dae::CLifeTime lifeTime{ gunType.BulletLifeTime, true };


		//------ Create GameObject

		GameObjectHandle currentBullet{ std::make_shared<GameObject>()};
		currentBullet->AddComponent(rigidBody);
		currentBullet->AddComponent(collider);
		currentBullet->AddComponent(lifeTime);
		currentBullet->AddComponent(animation);

		currentBullet->SetActive(false);

		return currentBullet;
	}

}