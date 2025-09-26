#pragma once

#include "Singleton.h"
#include "../src/GameEngine.h"

namespace dae {

	class Time : public Singleton<Time>
	{
		friend class Minigin;

	public: //this engine only has steady framerate
		int GetFramesPerSecond()  { return GAME_ENGINE->GetFrameRate(); }
		float GetElapsedSeconds() { return 1.f / float(GAME_ENGINE->GetFrameRate()); }
		float GetFixedTimeStep()  { return 1.f / float(GAME_ENGINE->GetFrameRate()); }

		
	private:
		float m_ElapsedSeconds{0.f};
		float m_FixedTimeStep{ 0.f };

	};
}

