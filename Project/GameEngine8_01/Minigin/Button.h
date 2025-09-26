#pragma once

#include <string>
#include <vector>
#include "../3rdParty/glm/glm.hpp"
#include "../src/GameEngine.h"
#include "EventSystem.h"
#include "Scene.h"
#include "InputManager.h"
#include "Text.h"

namespace dae {

	struct ButtonData {
		std::string Name;
		FontData FontData;
		COLORREF UnselectedColor{ RGB(255, 255 , 255) };
		COLORREF SelectedColor{ RGB(255, 0 , 0) };
		dae::Event<> OnPress;
	};

	class CButton : public Component{
		
	public:
		CButton(const ButtonData& buttonData);
		void Start()override;
		bool IsSelected()const { return m_IsSelected; }
		void Select();
		void Unselect();
		void Press();
		void SubscribeOnPress(const std::function<void()>& func);
	private:

		bool m_IsSelected{ false };
		std::shared_ptr<CText> m_CText;
		ButtonData m_ButtonData;
	};

	
}