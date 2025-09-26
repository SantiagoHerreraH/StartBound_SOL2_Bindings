#pragma once

#include <random>
#include "../src/GameEngine.h"
#include "Rect.h"

namespace dae {

	class Random {

	public:
		static int GetRandomBetweenRange(int min, int max)
		{
			//if you don't add the + 1, the range can never reach max
			return (std::rand() % (max - min + 1)) + min;
		}

		static float GetRandomBetweenRange(float min, float max)
		{
			float scale = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
			return min + scale * (max - min);
		}

		static COLORREF GetRandomBetweenRange(COLORREF min, COLORREF max)
		{

			
			 int r = Random::GetRandomBetweenRange(GetRValue(min), GetRValue(max));
			 int g = Random::GetRandomBetweenRange(GetGValue(min), GetGValue(max));
			 int b = Random::GetRandomBetweenRange(GetBValue(min), GetBValue(max));
			 return RGB(r, g, b);
		}

		static glm::vec2 GetRandomBetweenRange(glm::vec2 min, glm::vec2 max)
		{
			//if you don't add the + 1, the range can never reach max
			return glm::vec2{ Random::GetRandomBetweenRange(min.x, max.x), Random::GetRandomBetweenRange(min.y, max.y) };
		}

		static glm::vec2 GetRandomInRectPerimeter(const Rect& rect) {

			glm::vec2 randomValue{ GetRandomBetweenRange(glm::vec2{rect.Left, rect.Top}, glm::vec2{rect.Right(), rect.Bottom()})};

			bool chooseHorizontalSide{ (bool)GetRandomBetweenRange(0,1) };
			bool chooseBiggerSideInValue{ (bool)GetRandomBetweenRange(0,1) };

			if (chooseHorizontalSide)
			{
				randomValue.x = chooseBiggerSideInValue? rect.Right() : rect.Left;
			}
			else
			{
				randomValue.y = chooseBiggerSideInValue ? rect.Bottom() : rect.Top;
			}

			return randomValue;
		}
	};
}
