#pragma once
#include "Trigger.h"
#include <Box2D/Box2D.h>
#include "timer.h"

class EndTrigger : public Trigger
{
public:
	void OnTrigger() override;

	void OnEnter() override;
	void OnExit() override;
	void OnUpdate();

protected:
	bool triggered = false;
	float timer = 0;
};

