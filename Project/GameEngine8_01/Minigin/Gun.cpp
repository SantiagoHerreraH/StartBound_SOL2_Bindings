#include "Gun.h"
#include "Scene.h"
#include "TimerSystem.h"
#include "Bullet.h"
#include "Lifetime.h"
#include "GameStats.h"


dae::CGun::CGun(const GunData& gunData) : m_GunData(gunData)
{
	for (size_t i = 0; i < m_GunData.GunType.BulletAmount; i++)
	{
		m_Bullets.push_back(CreateBullet(m_GunData.Shooter, m_GunData.BulletCollisionLayer, m_GunData.BulletCollisionLayerToCollideAgainst, gunData.GunType));
	}
}

void dae::CGun::Start()
{

	for (size_t i = 0; i < m_Bullets.size(); i++)
	{
		SceneManager::GetInstance().GetCurrentScene().AddGameObjectHandle(m_Bullets[i]);
	}

	m_CurrentTimerSystem = &TimerSystem::GetCurrent();

	Timer timeBetweenShots{ m_GunData.GunType.TimeBetweenShots, false };
	m_TimeBetweenShotsTimerKey = m_CurrentTimerSystem->AddTimer(timeBetweenShots);
	
}

void dae::CGun::Shoot(const glm::vec2& direction)
{
	if (m_CurrentTimerSystem &&
		m_CurrentTimerSystem->HasTimer(m_TimeBetweenShotsTimerKey) &&
		m_GunData.Shooter->IsActive() && 
		m_CurrentTimerSystem->TimerAt(m_TimeBetweenShotsTimerKey).IsFinished())
	{
		m_GunData.Shooter->GetComponent<CGameStatController>()->OffsetStat(StatType::ShotsFired, 1);
		//m_ShootAudio->Play();

		glm::vec2 where = Owner().Transform().GetWorldTransform().Position;
		m_Bullets[m_CurrentBulletIndex]->GetComponent<CLifeTime>()->Respawn(where);
		m_Bullets[m_CurrentBulletIndex]->GetComponent<CRigidbody2D>()->SetVelocity(glm::normalize(direction) * m_GunData.GunType.BulletSpeed);

		m_CurrentTimerSystem->RestartTimer(m_TimeBetweenShotsTimerKey);//time between shots

		m_CurrentBulletIndex = (m_CurrentBulletIndex + 1) % m_Bullets.size();
	}
}
