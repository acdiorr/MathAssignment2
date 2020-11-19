#pragma once
#include <Windows.h>
#include <string>
//these two headers are already included in the <Windows.h> header
#pragma comment(lib, "Winmm.lib")

class SoundFunctions abstract
{
public:
	static void LoadSound(std::string fileName);
	static void PlaySound(std::string fileName);
	static void LoopSound(std::string fileName);
	static void PauseSound(std::string fileName);
	static void StopSound(std::string fileName);

private:
};