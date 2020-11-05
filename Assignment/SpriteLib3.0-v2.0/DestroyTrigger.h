#pragma once

#include "Trigger.h"
class DestroyTrigger : public Trigger
{
public:
	void OnTrigger() override;
protected:

	bool triggered = false;
};

