#include "ShrinkTrigger.h"
#include "ECS.h"

void ShrinkTrigger::OnTrigger()
{
	Trigger::OnTrigger();
}

void ShrinkTrigger::OnEnter()
{
	Trigger::OnEnter();

	if (!triggered)
	{
		for (int i = 0; i < m_targetEntities.size(); i++)
		{
			ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).ScaleBody(scale * 1.f, 0);
		}

		triggered = true;
	}
}

void ShrinkTrigger::OnExit()
{
	Trigger::OnExit();
	
}
