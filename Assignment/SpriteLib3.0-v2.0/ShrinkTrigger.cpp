#include "ShrinkTrigger.h"
#include "ECS.h"

void ShrinkTrigger::OnTrigger()
{
	Trigger::OnTrigger();

	if (!triggered)
	{
		for (int i = 0; i < m_targetEntities.size(); i++)
		{
			PhysicsBody::m_bodiesToShrink.push_back(m_targetEntities[i]);
		}

		triggered = true;
	}
}

void ShrinkTrigger::OnEnter()
{
	Trigger::OnEnter();

	if (!triggered)
	{
		for (int i = 0; i < m_targetEntities.size(); i++)
		{
			PhysicsBody::m_bodiesToShrink.push_back(m_targetEntities[i]);
		}

		triggered = true;
	}
}

void ShrinkTrigger::OnExit()
{
	Trigger::OnExit();
}
