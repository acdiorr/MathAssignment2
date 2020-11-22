#include "EndTrigger.h"
#include "ECS.h"

void EndTrigger::OnTrigger()
{
	Trigger::OnTrigger();

}

void EndTrigger::OnEnter()
{
	Trigger::OnEnter();

	if (!triggered)
	{
		timer = 6;

	}

	triggered = false;
}

void EndTrigger::OnExit()
{
	Trigger::OnExit();
}

void EndTrigger::OnUpdate()
{
	if (timer > 0)
	{
		timer -= Timer::deltaTime;
		if (timer <= 0)
		{
			timer = 0;
			exit(0);
		}
	}
}
