#pragma once

#include <string>
#include "Component.h"
#include "EventSystem.h"
#include "Rect.h"
#include "../3rdParty/glm/glm.hpp"
#include <Windows.h>


namespace dae {

	class CRigidbody2D;
	using CollisionTag = int;

	class CCollider final : public Component
	{
	public:
		CCollider(const Rect& rect, const CollisionTag& tag, bool isSolid);

		void Start() override;
		void FixedUpdate() override;
		void Render()const override;
		void SetActive(bool isActive) override;

		void SetRectOffset(const glm::vec2& offset);
		void CenterRect();

		void SetOriginalRect(const Rect& rect);
		const Rect& GetOriginalRect()const;
		const Rect& GetTransformedRect()const;

		CollisionTag GetCollisionTag();
		void AddCollisionTagToCollideWith(const CollisionTag& tag);
		
		void RenderCollider(bool render, bool fill, COLORREF renderColor);

		Event<GameObject&, GameObject&>& OnCollisionBeginEvent();
		Event<GameObject&, GameObject&>& OnCollisionStayEvent();
		Event<GameObject&, GameObject&>& OnCollisionEndEvent();

	private:


		Rect& GetTransformedRect();

		void ResolveNonSolid(GameObject* other, bool isStayOverlapping, int otherIdx);
		void ResolveSolid(GameObject* other, bool isStayOverlapping, int otherIdx);

		bool m_IsSolid{false};
		glm::vec2 m_Offset{0,0};
		Rect m_OriginalRect{};
		mutable Rect m_TransformedRect{};
		void TransformRect()const;

		CollisionTag m_CollisionTag{-1};//default collision tag
		std::vector<CollisionTag> m_CollisionTagsToCollideWith{};

		COLORREF m_RenderColor{RGB(0,255,0)};
		bool m_RenderCollider{ false };
		bool m_RenderFill{ false };

		std::vector<std::shared_ptr<CCollider>> m_CollidersToCollideWith;

		bool IsStayOverlappingWith(const GameObject& other);
		std::vector<GameObject*> m_CollidingWithEntities;

		Event<GameObject&, GameObject&> m_OnCollisionStayEvent{ }; //self, other
		Event<GameObject&, GameObject&> m_OnCollisionBeginEvent{ }; //self, other
		Event<GameObject&, GameObject&> m_OnCollisionEndEvent{ }; //self, other 


		std::shared_ptr<CRigidbody2D> m_RigidBody;
	};

}

