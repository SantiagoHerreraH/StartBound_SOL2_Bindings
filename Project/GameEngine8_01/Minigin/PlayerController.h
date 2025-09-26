#pragma once

#include "Component.h"
#include "InputManager.h"


namespace dae {

	
	struct PlayerKeyboardKeyData {
		ButtonState ButtonState;
		KeyboardKey Key;
		std::shared_ptr<ICommand> OnTriggered;
	};

	struct PlayerMouseEventData {

		ButtonState ButtonState;
		bool IsLeft;
		std::shared_ptr<ICommand> OnTriggered;
	};

	class CPlayerController final : public Component
	{
	public:
		CPlayerController();
		
		void Start()override;
		void SetActive(bool isActive)override;

		void BindKey(const PlayerKeyboardKeyData& playerKeyboardKeyData);
		void BindMouseEvent(const PlayerMouseEventData& playerKeyboardKeyData);

	private:

		PlayerId m_PlayerId{ -1 };
		std::vector<PlayerKeyboardKeyData> m_PlayerKeyboardKeyData;
		std::vector<PlayerMouseEventData>  m_PlayerMouseEventData;
	};
}


