#include "Audio.h"
#include "ServiceLocator.h"
#include "GameObject.h"

dae::CAudio::CAudio(const AudioData& audioData) :
	m_AudioData(audioData),
	m_Audio(audioData.File){}

void dae::CAudio::Update()
{
	m_Audio.Tick();
}

void dae::CAudio::Play() 
{
	m_Audio.Play();

}

void dae::CAudio::Mute()
{
	m_Audio.SetVolume(0);
}

void dae::CAudio::Unmute()
{
	m_Audio.SetVolume(100);
}
