#pragma once

#include "Scene.h"
#include "PhysicsPlaygroundListener.h"
#include "SoundFunctions.h"
class PhysicsPlayground : public Scene
{
public:
	PhysicsPlayground(std::string name);

	void InitScene(float windowWidth, float windowHeight) override;

	void Update() override;

	//Input overrides
	void KeyboardHold() override;
	void KeyboardDown() override;
	void KeyboardUp() override;

protected:
	PhysicsPlaygroundListener listener;
	unsigned endTriggerEntity;

	int player = 0;
	int wall = 1;
	int ball = 2;
	int elevator = 3;
	int wall2 = 4;
	int stairs = 5;
	int wall3 = 6;
	int noTouch = 7;
	int image = 8;
};
