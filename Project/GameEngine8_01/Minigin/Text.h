#pragma once
#include <string>
#include "Texture.h"
#include "../src/GameEngine.h"
#include "Component.h"


namespace dae {

	struct FontData {
		std::string FontFile;
		uint8_t FontSize;
		bool Bold;
		bool Itallic;
		bool Underlined;
	};

	struct TextData {
		FontData FontData;
		COLORREF Color;
		std::string Text;
	};

	struct CText final : public Component{
	public:
		CText() {}
		~CText();
		CText(const std::string& fontFullPath, uint8_t fontSize);
		CText(const FontData& fontData);
		CText(const TextData& textData);

		virtual void Render()const override;

		void SetText(std::string text); 
		void AddText(const std::string& text,bool center = true);
		void SetColor(const COLORREF& color);
		bool IsFontValid();
		bool SetFont(const std::string& fullPath, uint8_t size);
		const Font* Data()const;
		glm::ivec2 GetSize()const;
		void SetTextTransform(const TransformData& transform) { m_TransformData = transform; }
		const TransformData& GetTextureTransform() const { return m_TransformData; }
		glm::vec2 GetScaledSize() const;
		void Center();

	private:
		std::string m_Text;
		COLORREF m_Color{};
		std::string m_FontPath;
		std::shared_ptr<Font> m_Font;
		TransformData m_TransformData;

		bool m_IsFontValid{ false };
		mutable bool m_BoundsNeedsUpdate{ true }; // needs to be muttable for dirty pattern on Data() call
		mutable RECT m_Bounds{};// needs to be muttable for dirty pattern on Data() call
	};
}