#pragma once
#include "Scene.h"
#include "../src/GameEngine.h"

namespace dae {

	class TextCreator {

	public:
		TextCreator(){}
		TextCreator(const std::string& text, const glm::vec2& pos, uint8_t fontSize, const COLORREF& color);
		void SetText(const std::string& text);
		GameObject& GetGameObject() { return *m_TextGameObject; }
		GameObjectHandle GetGameObjectHandle() const { return m_TextGameObject; }


	private:
		GameObjectHandle m_TextGameObject;
	};
}