#pragma once

#include <string>
#include "Component.h"
#include "../src/GameEngine.h"

namespace dae {

	struct AudioData{

		std::string File;
	};

	class CAudio final : public Component
	{
	public:

		CAudio(const AudioData& audioData);
		void Update()override;
		void Play();
		void Mute();
		void Unmute();

	private:
		bool m_PlaySoundOnStart{ false };
		AudioData m_AudioData{};
		Audio m_Audio;
	};

}


