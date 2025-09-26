#include "Collider.h"
#include <algorithm>
#include "SceneManager.h"
#include "Scene.h"
#include "Rigidbody.h"
#include "../src/GameEngine.h"

namespace dae {
	CCollider::CCollider(const Rect& rect, const CollisionTag& tag, bool isSolid) :
		m_OriginalRect(rect),
		m_CollisionTag(tag),
		m_TransformedRect(rect),
		m_IsSolid(isSolid)
	{
	}


	void CCollider::CenterRect()
	{
		m_Offset.x = -m_OriginalRect.Width / 2.f;
		m_Offset.y = -m_OriginalRect.Height / 2.f;
	}

	void CCollider::SetRectOffset(const glm::vec2& offset)
	{
		m_Offset = offset;
	}

	void CCollider::SetOriginalRect(const Rect& rect)
	{
		m_OriginalRect = rect;
	}
	
	const Rect& CCollider::GetOriginalRect() const
	{
		return m_OriginalRect;
	}
	const Rect& CCollider::GetTransformedRect() const
	{
		TransformRect();
		return m_TransformedRect;
	}
	CollisionTag CCollider::GetCollisionTag()
	{
		return m_CollisionTag;
	}
	
	void CCollider::AddCollisionTagToCollideWith(const CollisionTag& tag)
	{
		//probably not a lot, so it's okay to do this
		for (size_t i = 0; i < m_CollisionTagsToCollideWith.size(); i++)
		{
			if (m_CollisionTagsToCollideWith[i] == tag)
			{
				return;
			}
		}

		m_CollisionTagsToCollideWith.push_back(tag);
	}
	void CCollider::RenderCollider(bool render, bool fill, COLORREF renderColor)
	{
		m_RenderCollider = render;
		m_RenderFill = fill;
		m_RenderColor = renderColor;
	}
	Event<GameObject&, GameObject&>& CCollider::OnCollisionBeginEvent()
	{
		return m_OnCollisionBeginEvent;
	}
	Event<GameObject&, GameObject&>& CCollider::OnCollisionStayEvent()
	{
		return m_OnCollisionStayEvent;
	}
	Event<GameObject&, GameObject&>& CCollider::OnCollisionEndEvent()
	{
		return m_OnCollisionEndEvent;
	}

	Rect& CCollider::GetTransformedRect()
	{
		TransformRect();
		return m_TransformedRect;
	}

	void CCollider::ResolveNonSolid(GameObject* other, bool isStayOverlapping, int otherIdx)
	{
		if (IsColliding(GetTransformedRect(), m_CollidersToCollideWith[otherIdx]->GetTransformedRect()))
		{
			if (!isStayOverlapping)
			{
				m_CollidingWithEntities.push_back(other);
				m_CollidersToCollideWith[otherIdx]->m_CollidingWithEntities.push_back(&Owner());

				m_OnCollisionBeginEvent.Invoke(Owner(), *other);
			}

			m_OnCollisionStayEvent.Invoke(Owner(), *other);
		}
		else if (isStayOverlapping)
		{
			m_OnCollisionEndEvent.Invoke(Owner(), *other);

			std::erase(m_CollidingWithEntities, other);
			std::erase(m_CollidersToCollideWith[otherIdx]->m_CollidingWithEntities, &Owner());

		}
	}

	void CCollider::ResolveSolid(GameObject* other, bool isStayOverlapping, int otherIdx)
	{
		glm::vec2 vel{ m_RigidBody ? m_RigidBody->GetVelocity() : glm::vec2{} };
		glm::vec2 otherVel{ 
			m_CollidersToCollideWith[otherIdx]->m_RigidBody ? 
			m_CollidersToCollideWith[otherIdx]->m_RigidBody->GetVelocity() : glm::vec2{} };

		if (ResolveCollision(GetTransformedRect(), vel,  m_CollidersToCollideWith[otherIdx]->GetTransformedRect(), otherVel))
		{
			if (!isStayOverlapping)
			{
				m_CollidingWithEntities.push_back(other);
				m_CollidersToCollideWith[otherIdx]->m_CollidingWithEntities.push_back(&Owner());

				m_OnCollisionBeginEvent.Invoke(Owner(), *other);
			}

			m_OnCollisionStayEvent.Invoke(Owner(), *other);
		}
		else if (isStayOverlapping)
		{
			m_OnCollisionEndEvent.Invoke(Owner(), *other);

			std::erase(m_CollidingWithEntities, other);
			std::erase(m_CollidersToCollideWith[otherIdx]->m_CollidingWithEntities, &Owner());

		}

		if (m_RigidBody)
		{
			m_RigidBody->SetVelocity(vel);
		}
		if (m_CollidersToCollideWith[otherIdx]->m_RigidBody)
		{
			m_CollidersToCollideWith[otherIdx]->m_RigidBody->SetVelocity(otherVel);
		}
	}
	
	void CCollider::TransformRect() const
	{
		glm::vec3 pos = OwnerConst().TransformConst().GetWorldTransform().Position;

		m_TransformedRect.Left = int(m_OriginalRect.Left + m_Offset.x + pos.x); 
		m_TransformedRect.Top = int(m_OriginalRect.Top + m_Offset.y + pos.y);
	}
	bool CCollider::IsStayOverlappingWith(const GameObject& other)
	{
		for (size_t i = 0; i < m_CollidingWithEntities.size(); i++)
		{
			if (m_CollidingWithEntities[i] == &other)
			{
				return true;
			}
		}
		return false;
	}
	void CCollider::Start()
	{
		std::vector<std::shared_ptr<CCollider>> sceneColliders = SceneManager::GetInstance().GetCurrentScene().GetAllComponentsOfType<CCollider>();

		m_CollidersToCollideWith.clear();

		for (size_t colliderIdx = 0; colliderIdx < sceneColliders.size(); colliderIdx++)
		{
			for (size_t tagIdx = 0; tagIdx < m_CollisionTagsToCollideWith.size(); tagIdx++)
			{
				if (sceneColliders[colliderIdx].get() == this)
				{
					continue;
				}

				if (sceneColliders[colliderIdx]->GetCollisionTag() == m_CollisionTagsToCollideWith[tagIdx])
				{
					m_CollidersToCollideWith.push_back(sceneColliders[colliderIdx]);
					break;
				}
			}
		}

		m_RigidBody = Owner().GetComponentHandle<CRigidbody2D>();
	}
	void CCollider::FixedUpdate()
	{
		bool isStayOverlapping{};
		for (size_t i = 0; i < m_CollidersToCollideWith.size(); i++)
		{
			if (m_CollidersToCollideWith[i]->IsActive())
			{
				GameObject* otherOwner = &m_CollidersToCollideWith[i]->Owner();
				isStayOverlapping = IsStayOverlappingWith(m_CollidersToCollideWith[i]->Owner());

				if (m_IsSolid && m_CollidersToCollideWith[i]->m_IsSolid)
				{
					ResolveSolid(otherOwner, isStayOverlapping, i);
				}
				else
				{
					ResolveNonSolid(otherOwner, isStayOverlapping, i);
				}
				
			}
		}

	}
	void CCollider::Render() const
	{
		if (m_RenderCollider)
		{
			GAME_ENGINE->SetColor(m_RenderColor);

			Rect rect = GetTransformedRect();

			if (m_RenderFill)
			{
				GAME_ENGINE->FillRect(rect.Left, rect.Top, rect.Right(), rect.Bottom());
			}
			else
			{
				GAME_ENGINE->DrawRect(rect.Left, rect.Top, rect.Right(), rect.Bottom());
			}
		}
	}
	void CCollider::SetActive(bool isActive)
	{
		Component::SetActive(isActive);

		if (!isActive)
		{
			for (size_t i = 0; i < m_CollidersToCollideWith.size(); i++)
			{

				GameObject* otherOwner = &m_CollidersToCollideWith[i]->Owner();
				m_OnCollisionEndEvent.Invoke(Owner(), *otherOwner);

				std::erase(m_CollidingWithEntities, otherOwner);
				std::erase(m_CollidersToCollideWith[i]->m_CollidingWithEntities, &Owner());
			}
		}
	}
}


