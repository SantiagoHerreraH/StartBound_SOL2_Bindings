#include "PlayerController.h"

dae::CPlayerController::CPlayerController()
{
}

void dae::CPlayerController::Start()
{
	m_PlayerId = InputManager::GetCurrent().CreateNewPlayerId();

	KeyboardKeyData keyboardKeyData{};
	keyboardKeyData.PlayerGameObject = &Owner();

	for (size_t i = 0; i < m_PlayerKeyboardKeyData.size(); i++)
	{
		keyboardKeyData.ButtonState = m_PlayerKeyboardKeyData[i].ButtonState;
		keyboardKeyData.Key = m_PlayerKeyboardKeyData[i].Key;
		keyboardKeyData.OnTriggered = m_PlayerKeyboardKeyData[i].OnTriggered;

		InputManager::GetCurrent().BindKey(keyboardKeyData);
	}

	MouseEventData mouseEventData{};
	mouseEventData.PlayerGameObject = &Owner();

	for (size_t i = 0; i < m_PlayerMouseEventData.size(); i++)
	{
		auto& playerMouseEventData = m_PlayerMouseEventData[i];

		mouseEventData.ButtonState = playerMouseEventData.ButtonState;
		mouseEventData.IsLeft = playerMouseEventData.IsLeft;
		mouseEventData.OnTriggered = playerMouseEventData.OnTriggered;
		mouseEventData.PlayerId = m_PlayerId;
		InputManager::GetCurrent().BindMouseEvent(mouseEventData);
	}
}

void dae::CPlayerController::SetActive(bool isActive)
{
	Component::SetActive(isActive);

	if (m_PlayerId < 0)
	{
		return;
	}
	if (isActive)
	{
		InputManager::GetCurrent().EnableInput(m_PlayerId);
	}
	else
	{
		InputManager::GetCurrent().DisableInput(m_PlayerId);
	}
}

void dae::CPlayerController::BindKey(const PlayerKeyboardKeyData& playerKeyboardKeyData)
{
	if (HasOwner() && m_PlayerId >= 0)
	{
		KeyboardKeyData keyboardData{};
		keyboardData.PlayerGameObject = &Owner();
		keyboardData.ButtonState = playerKeyboardKeyData.ButtonState;
		keyboardData.Key = playerKeyboardKeyData.Key;
		keyboardData.OnTriggered = playerKeyboardKeyData.OnTriggered;
		keyboardData.PlayerId = m_PlayerId;
		InputManager::GetCurrent().BindKey(keyboardData);
	}
	else
	{
		m_PlayerKeyboardKeyData.push_back(playerKeyboardKeyData);
	}
}

void dae::CPlayerController::BindMouseEvent(const PlayerMouseEventData& playerMouseEventData)
{
	if (HasOwner() && m_PlayerId >= 0)
	{
		MouseEventData mouseEventData{};
		mouseEventData.PlayerGameObject = &Owner();
		mouseEventData.ButtonState = playerMouseEventData.ButtonState;
		mouseEventData.IsLeft = playerMouseEventData.IsLeft;
		mouseEventData.OnTriggered = playerMouseEventData.OnTriggered;
		mouseEventData.PlayerId = m_PlayerId;
		InputManager::GetCurrent().BindMouseEvent(mouseEventData);
	}
	else
	{
		m_PlayerMouseEventData.push_back(playerMouseEventData);
	}
}
