#pragma once
#include "Trigger.h"
#include "Scene.h"

class CreationTrigger : public Trigger
{
public:
	void OnTrigger() override;

	void OnEnter() override;
	void OnExit() override;

protected:
	bool triggered = false;
};