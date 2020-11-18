#pragma once
#include "Trigger.h"
#include <Box2D/Box2D.h>

class ShrinkTrigger : public Trigger
{
public:
	void OnTrigger() override;

	void OnEnter() override;
	void OnExit() override;

	float scale;
protected:
	bool triggered = false;
};

