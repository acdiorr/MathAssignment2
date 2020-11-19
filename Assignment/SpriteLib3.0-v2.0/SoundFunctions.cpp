#include "SoundFunctions.h"

void SoundFunctions::LoadSound(std::string fileName)
{
	std::string soundString = "open assets/sounds/" + fileName + " type mpegvideo alias " + fileName;
	mciSendString(soundString.c_str(), NULL, 0, NULL);
}

void SoundFunctions::PlaySound(std::string fileName)
{
	std::string soundString = "play " + fileName;
	mciSendString(soundString.c_str(), NULL, 0, NULL);
}

void SoundFunctions::LoopSound(std::string fileName)
{
	std::string soundString = "play " + fileName + " repeat";
	mciSendString(soundString.c_str(), NULL, 0, NULL);
}

void SoundFunctions::PauseSound(std::string fileName)
{
	std::string soundString = "pause " + fileName;
	mciSendString(soundString.c_str(), NULL, 0, NULL);
}

void SoundFunctions::StopSound(std::string fileName)
{
	std::string soundString = "close " + fileName;
	mciSendString(soundString.c_str(), NULL, 0, NULL);
}
