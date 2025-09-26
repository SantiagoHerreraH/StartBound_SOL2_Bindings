
#include "InputManager.h"
#include <iostream>
#include "../src/GameEngine.h"

glm::ivec2 dae::InputManager::MousePos()
{
	return m_MousePos;
}

void dae::InputManager::ClearKeys()
{
	m_KeyboardEventsPerPlayerId.clear();
}

void dae::InputManager::BindKey(const KeyboardKeyData& keyboardKeyData)
{
	if (IsValidId(keyboardKeyData.PlayerId))
	{
		m_KeyboardEventsPerPlayerId[keyboardKeyData.PlayerId].push_back(keyboardKeyData);
	}
}

void dae::InputManager::BindMouseEvent(const MouseEventData& mouseDownData)
{
	if (IsValidId(mouseDownData.PlayerId))
	{
		m_MouseEventsPerPlayerId[mouseDownData.PlayerId].push_back(mouseDownData);
	}
}

dae::InputManager::InputManager()
{
	m_KeyboardKeyStates.resize(256);
}

void dae::InputManager::UpdateMouseEvents(bool isDown, bool isLeft)
{
	if (!isDown && !m_IsMouseDown)
	{
		return;
	}
	ButtonState currentButtonState{};

	if (isDown && m_IsMouseDown)
	{
		currentButtonState = ButtonState::BUTTON_PRESSED;
	}
	else if (!isDown && m_IsMouseDown)
	{
		currentButtonState = ButtonState::BUTTON_UP;
	}
	else if (isDown && !m_IsMouseDown)
	{
		currentButtonState = ButtonState::BUTTON_DOWN;
	}

	m_IsMouseDown = isDown;

	for (size_t activatedIdx = 0; activatedIdx < m_MouseEventsPerPlayerId.size(); activatedIdx++)
	{
		PlayerId currentPlayerId = m_ActivatedPlayerIds[activatedIdx];

		for (size_t currentKeyIdx = 0; currentKeyIdx < m_MouseEventsPerPlayerId[currentPlayerId].size(); currentKeyIdx++)
		{
			auto& mouseEvent = m_MouseEventsPerPlayerId[currentPlayerId][currentKeyIdx];
			bool triggered = (isLeft == mouseEvent.IsLeft) && (mouseEvent.ButtonState == currentButtonState);

			if (triggered)
			{
				mouseEvent.OnTriggered->Execute(*mouseEvent.PlayerGameObject);
			}
		}
	}
}

void dae::InputManager::Render()const {

	int radio = 5;
	GAME_ENGINE->SetColor(RGB(0,255,0));
	GAME_ENGINE->FillOval(m_MousePos.x - radio, m_MousePos.y - radio, m_MousePos.x + radio, m_MousePos.y + radio);
}

void dae::InputManager::Update()
{
	for (size_t activatedIdx = 0; activatedIdx < m_ActivatedPlayerIds.size(); activatedIdx++)
	{
		PlayerId currentPlayerId = m_ActivatedPlayerIds[activatedIdx];

		for (size_t currentKeyIdx = 0; currentKeyIdx < m_KeyboardEventsPerPlayerId[currentPlayerId].size(); currentKeyIdx++)
		{
			auto& keyboardEvent = m_KeyboardEventsPerPlayerId[currentPlayerId][currentKeyIdx];
			bool triggered = false;
			bool isKeyDown = GAME_ENGINE->IsKeyDown(keyboardEvent.Key);

			if (!m_KeyboardKeyStates[keyboardEvent.Key] && isKeyDown)
			{
				m_KeyboardKeyStates[keyboardEvent.Key] = true;
				triggered = keyboardEvent.ButtonState == ButtonState::BUTTON_DOWN;
			}
			else if (m_KeyboardKeyStates[keyboardEvent.Key] && !isKeyDown)
			{
				m_KeyboardKeyStates[keyboardEvent.Key] = false;
				triggered = keyboardEvent.ButtonState == ButtonState::BUTTON_UP;
			}
			else if (isKeyDown)
			{
				triggered = keyboardEvent.ButtonState == ButtonState::BUTTON_PRESSED;
			}

			if (triggered)
			{
 				keyboardEvent.OnTriggered->Execute(*keyboardEvent.PlayerGameObject);
			}
		}
	}
	
}

void dae::InputManager::Reset()
{
	m_KeyboardEventsPerPlayerId.clear();
	m_CurrentPlayerId = 0;
}

bool dae::InputManager::IsValidId(PlayerId playerId) const
{
	return playerId >= 0 && playerId < m_KeyboardEventsPerPlayerId.size();
}

bool dae::InputManager::IsPlayerIdEnabled(PlayerId playerId) const
{
	for (size_t i = 0; i < m_ActivatedPlayerIds.size(); i++)
	{
		if (m_ActivatedPlayerIds[i] == playerId)
		{
			return true;
		}
	}

	return false;
}

void dae::InputManager::EnableInput(PlayerId playerId)
{
	if (!IsPlayerIdEnabled(playerId))
	{
		m_ActivatedPlayerIds.push_back(playerId);
	}

}

void dae::InputManager::DisableInput(PlayerId playerId)
{
	for (size_t i = 0; i < m_ActivatedPlayerIds.size(); i++)
	{
		if (m_ActivatedPlayerIds[i] == playerId)
		{
			m_ActivatedPlayerIds.erase(m_ActivatedPlayerIds.begin() + i);
			break;
		}
	}

}

dae::PlayerId dae::InputManager::CreateNewPlayerId()
{
	m_KeyboardEventsPerPlayerId.emplace_back(KeyboardEvents{});
	m_MouseEventsPerPlayerId.emplace_back(MouseEvents{});

	PlayerId newPlayerId = m_CurrentPlayerId;
	++m_CurrentPlayerId;

	m_ActivatedPlayerIds.push_back(newPlayerId);

	return newPlayerId;
}


