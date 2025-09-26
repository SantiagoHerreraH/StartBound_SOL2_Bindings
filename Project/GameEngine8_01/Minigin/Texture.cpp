#include "Texture.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "../src/GameEngine.h"
#include <stdexcept>


#pragma region CTextureHandle

dae::CTextureHandle::CTextureHandle(const std::string& path)
{
	SetTexture(path);
}

void dae::CTextureHandle::Render()const 
{
	TransformData transformData{ OwnerConst().TransformConst().GetWorldTransform()};
	transformData += GetTextureTransform();

	GAME_ENGINE->DrawBitmap(Data(), transformData.Position.x, transformData.Position.y);
	
}

bool dae::CTextureHandle::IsValid()
{
	return m_IsValid;
}

bool dae::CTextureHandle::SetTexture(const std::string& path, bool resetTextureTransform)
{

	m_TextureData = ResourceManager::GetInstance().GetTextureData(path);
	m_IsValid = m_TextureData->Exists();

	if (m_IsValid && resetTextureTransform)
	{
		m_TransformData = TransformData{};
	}

	return m_IsValid;
}

void dae::CTextureHandle::SetTextureView(const Rect& rect)
{
	m_TextureView = rect;
	m_SetTextureView = true;
}

const dae::Rect* dae::CTextureHandle::GetTextureView() const
{
	return m_SetTextureView ? &m_TextureView : nullptr;
}

const Bitmap* dae::CTextureHandle::Data() const
{
	return m_TextureData.get();
}

void dae::CTextureHandle::Center()
{
	glm::vec2 size = { m_TextureData.get()->GetWidth(), m_TextureData.get()->GetHeight() };
	m_TransformData.Position.x = (-size.x/2.f) * m_TransformData.Scale.x;
	m_TransformData.Position.y = (-size.y/2.f) * m_TransformData.Scale.y;
}

void dae::CTextureHandle::SetTextureTransform(TransformData& transformData)
{
	m_TransformData = transformData;
}

#pragma endregion

#pragma region Sprite Sheet

namespace dae {

	SpriteSheet::SpriteSheet(const CTextureHandle& texture, unsigned int numberOfRows, unsigned int numberOfColumns, unsigned int numberOfEmptyPixeBetweenColumns, unsigned int numberOfEmptyPixeBetweenRows) :
		m_Texture(texture),
		m_NumberOfRows(numberOfRows),
		m_NumberOfColumns(numberOfColumns),
		m_NumberOfEmptyPixelsBetweenColumns(numberOfEmptyPixeBetweenColumns),
		m_NumberOfEmptyPixelsBetweenRows(numberOfEmptyPixeBetweenRows)
	{

		m_CellWidth =  int(m_Texture.Data()->GetWidth() / (float)m_NumberOfColumns);
		m_CellHeight = int(m_Texture.Data()->GetHeight() / (float)m_NumberOfRows	);

		glm::vec2 size = { GetOriginalCellWidth() , GetOriginalCellHeight()};
		TransformData transformData{ m_Texture.GetTextureTransform() };
		transformData.Position.x = (-size.x / 2.f) * m_Texture.GetTextureTransform().Scale.x;
		transformData.Position.y = (-size.y / 2.f) * m_Texture.GetTextureTransform().Scale.y;
		m_Texture.SetTextureTransform(transformData);
	}

	const CTextureHandle& SpriteSheet::GetFrame(int rowIndex, int columnIndex)
	{
		FrameTexture(rowIndex, columnIndex);
		return m_Texture;
	}

	CTextureHandle& SpriteSheet::TextureHandle()
	{
		return m_Texture;
	}

	int SpriteSheet::GetOriginalCellWidth() const
	{
		return m_CellWidth - m_NumberOfEmptyPixelsBetweenColumns;
	}

	int SpriteSheet::GetOriginalCellHeight() const
	{
		return m_CellHeight - m_NumberOfEmptyPixelsBetweenRows;
	}

	float SpriteSheet::GetScaledCellWidth() const
	{
		return (m_CellWidth - m_NumberOfEmptyPixelsBetweenColumns) * m_Texture.GetTextureTransform().Scale.x;
	}

	float SpriteSheet::GetScaledCellHeight() const
	{
		return (m_CellHeight - m_NumberOfEmptyPixelsBetweenRows) * m_Texture.GetTextureTransform().Scale.y;
	}

	void SpriteSheet::FrameTexture(int rowIndex, int columnIndex)
	{
		m_Texture.SetTextureView(Rect{
			int((rowIndex		* m_CellHeight)	+ (m_NumberOfEmptyPixelsBetweenRows		/ 2.f)		),
			int((columnIndex	* m_CellWidth)	+ (m_NumberOfEmptyPixelsBetweenColumns	/ 2.f)		),
			m_CellWidth - m_NumberOfEmptyPixelsBetweenColumns,
			m_CellHeight - m_NumberOfEmptyPixelsBetweenRows });
	}
}



#pragma endregion