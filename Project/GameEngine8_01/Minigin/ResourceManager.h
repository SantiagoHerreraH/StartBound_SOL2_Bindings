#pragma once
#include <filesystem>
#include <string>
#include <memory>
#include <map>
#include "Singleton.h"

class Font;
class Bitmap;
namespace dae
{
	class ResourceManager final : public Singleton<ResourceManager>
	{
	public:
		void Init(const std::filesystem::path& data);
		const std::filesystem::path& GetDataPath()const { return m_dataPath; }

		//loads first time if not loaded yet
		std::shared_ptr<Bitmap> GetTextureData(const std::string& file);
		//loads first time if not loaded yet
		std::shared_ptr<Font> GetFontData(const std::string& file, uint8_t size);
	private:
		friend class Singleton<ResourceManager>;
		ResourceManager() = default;
		std::filesystem::path m_dataPath;

		void UnloadUnusedResources();

		std::map<std::string, std::shared_ptr<Bitmap>> m_TextureName_To_Texture;
		std::map<std::pair<std::string, uint8_t>, std::shared_ptr<Font>> m_FontData_To_Font;

	};
}
