#pragma once
#include "../3rdParty/glm/glm.hpp"
#include "SceneSingleton.h"
#include <tuple>
#include <map>
#include <memory>

#include "Command.h"
#include "EventSystem.h"

#include <unordered_set>

namespace dae
{	

	enum class ButtonState
	{
		BUTTON_DOWN,
		BUTTON_PRESSED,
		BUTTON_UP
	};

	using KeyboardKey = int;
	using PlayerId = int;
	
	struct KeyboardKeyData {
		ButtonState ButtonState;
		KeyboardKey Key;
		PlayerId PlayerId;
		GameObject* PlayerGameObject;
		std::shared_ptr<ICommand> OnTriggered;
	};
	using KeyboardEvents = std::vector<KeyboardKeyData>;

	struct MouseEventData {

		ButtonState ButtonState;
		bool IsLeft;
		PlayerId PlayerId;
		GameObject* PlayerGameObject;
		std::shared_ptr<ICommand> OnTriggered;
	};


	using MouseEvents = std::vector<MouseEventData>;

	inline int g_MousePosX = 0;
	inline int g_MousePosY = 0;

	class InputManager final : public SceneSingleton<InputManager>
	{
	public:
		InputManager();
		virtual ~InputManager() noexcept{}
		InputManager(const InputManager& other) = delete;
		InputManager(InputManager&& other) = delete;
		InputManager& operator=(const InputManager& other) = delete;
		InputManager& operator=(InputManager&& other) = delete;

		void UpdateMouseEvents(bool isDown, bool isLeft);
		void Render()const;
		void Update()override;
		void Reset()override;


		PlayerId CreateNewPlayerId();
		bool IsValidId(PlayerId playerId)const;

		bool IsPlayerIdEnabled(PlayerId playerId)const;
		void EnableInput(PlayerId playerId);
		void DisableInput(PlayerId playerId);

		void RegisterMousePos(const glm::ivec2& pos) { m_MousePos = pos; }
		glm::ivec2 MousePos();

		void ClearKeys();
		void BindKey(const KeyboardKeyData& keyboardKeyData);
		void BindMouseEvent(const MouseEventData& mouseDownData);

	private:
		bool m_IsMouseDown{ false };
		glm::ivec2 m_MousePos{};
		std::vector<KeyboardEvents> m_KeyboardEventsPerPlayerId{};
		std::vector<MouseEvents> m_MouseEventsPerPlayerId{};

		std::vector<bool> m_KeyboardKeyStates;
		PlayerId m_CurrentPlayerId{0};
		std::vector<PlayerId> m_ActivatedPlayerIds;
	};

	
}