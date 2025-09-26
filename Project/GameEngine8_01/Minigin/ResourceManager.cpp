#include <stdexcept>
#include "../src/GameEngine.h"
#include "ResourceManager.h"
#include "Texture.h"

namespace fs = std::filesystem;

void dae::ResourceManager::Init(const std::filesystem::path& dataPath)
{
	m_dataPath = dataPath;

	
}

std::shared_ptr<Bitmap> dae::ResourceManager::GetTextureData(const std::string& file)
{
	const auto fullPath = m_dataPath/file;
	const auto filename = fs::path(fullPath).filename().string();
	if(m_TextureName_To_Texture.find(filename) == m_TextureName_To_Texture.end())
		m_TextureName_To_Texture.insert(std::pair(filename,std::make_shared<Bitmap>(fullPath.string())));
	return m_TextureName_To_Texture.at(filename);
}

std::shared_ptr<Font> dae::ResourceManager::GetFontData(const std::string& file, uint8_t size)
{
	const auto fullPath = m_dataPath/file;
	const auto filename = fs::path(fullPath).filename().string();
	const auto key = std::pair<std::string, uint8_t>(filename, size);
	if(m_FontData_To_Font.find(key) == m_FontData_To_Font.end())
		m_FontData_To_Font.insert(std::pair(key,std::make_shared<Font>(fullPath.string(), false, false, false, size)));
	return m_FontData_To_Font.at(key);
}

void dae::ResourceManager::UnloadUnusedResources()
{
	for (auto it = m_TextureName_To_Texture.begin(); it != m_TextureName_To_Texture.end();)
	{
		if (it->second.use_count() == 1)
			it = m_TextureName_To_Texture.erase(it);
		else
			++it;
	}

	for (auto it = m_FontData_To_Font.begin(); it != m_FontData_To_Font.end();)
	{
		if (it->second.use_count() == 1)
			it = m_FontData_To_Font.erase(it);
		else
			++it;
	}
}
