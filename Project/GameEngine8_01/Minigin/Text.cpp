#include "Text.h"
#include "ResourceManager.h"
#include <iostream>
#include "GameObject.h"
#include "../src/GameEngine.h"

dae::CText::~CText()
{
}

dae::CText::CText(const std::string& fullPath, uint8_t size) : m_Font(std::make_shared<Font>(fullPath, false, false, false, size))
{
	SetFont(fullPath, size);
}

dae::CText::CText(const FontData& fontData) : m_Font(std::make_shared<Font>(fontData.FontFile, fontData.Bold, fontData.Itallic, fontData.Underlined, fontData.FontSize)){

}


dae::CText::CText(const TextData& textData) : CText(textData.FontData) 
{
	SetColor(textData.Color);
	SetText(textData.Text);
}

void dae::CText::Render()const
{
	if (m_Text.empty())
	{
		return;
	}

	TransformData transformData{ OwnerConst().TransformConst().GetWorldTransform() };
	transformData += GetTextureTransform();

	Font* fontPtr = m_Font.get();

	GAME_ENGINE->SetColor(m_Color);
	GAME_ENGINE->SetFont(fontPtr);
	GAME_ENGINE->DrawString(m_Text, transformData.Position.x, transformData.Position.y);
}

void dae::CText::SetText(std::string text)
{
	if (m_Text != text)
	{
		m_BoundsNeedsUpdate = true;
		m_Text = std::move(text);
	}
}

void dae::CText::AddText(const std::string& text, bool center)
{
	if (m_Text != text)
	{
		m_BoundsNeedsUpdate = true;
		m_Text += text;

		if (center)
		{
			Center();
		}
	}
}

void dae::CText::SetColor(const COLORREF& color)
{
	m_BoundsNeedsUpdate = true;
	m_Color = color;
}

bool dae::CText::IsFontValid()
{
	return m_IsFontValid;
}

bool dae::CText::SetFont(const std::string& fullPath, uint8_t size)
{
	m_Font = ResourceManager::GetInstance().GetFontData(fullPath, size);
	m_IsFontValid = m_Font != nullptr;
	m_BoundsNeedsUpdate = true;
	return m_IsFontValid;
}

const Font* dae::CText::Data() const
{
	if (m_Text.empty())
	{
		return nullptr;
	}
	
	return m_Font.get();
}

glm::ivec2 dae::CText::GetSize() const
{
	if (m_Text.empty())
	{
		return glm::ivec2(0,0);
	}

	if (m_BoundsNeedsUpdate)
	{
		m_BoundsNeedsUpdate = false;

		TransformData transformData = GetTextureTransform();

		GAME_ENGINE->SetFont(m_Font.get());
		m_Bounds = GAME_ENGINE->GetStringBounds(m_Text, transformData.Position.x, transformData.Position.y);
	}

	return glm::ivec2(m_Bounds.right - m_Bounds.left, m_Bounds.bottom - m_Bounds.top);
}


glm::vec2 dae::CText::GetScaledSize() const {

	glm::vec2 size = GetSize();

	size.x *= m_TransformData.Scale.x;
	size.y *= m_TransformData.Scale.y;

	return size;
}

void dae::CText::Center()
{
	if (m_Text.empty())
	{
		return;
	}
	glm::vec2 size = GetSize();

	m_TransformData.Position.x = (-size.x / 2.f) * m_TransformData.Scale.x;
	m_TransformData.Position.y = (-size.y / 2.f) * m_TransformData.Scale.y;
}
