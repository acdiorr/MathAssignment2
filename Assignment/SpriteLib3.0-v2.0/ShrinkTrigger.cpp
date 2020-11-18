#include "ShrinkTrigger.h"
#include "ECS.h"

void ShrinkTrigger::OnTrigger()
{
	Trigger::OnTrigger();
}

void ShrinkTrigger::OnEnter()
{
	Trigger::OnEnter();
	for (int i = 0; i < m_targetEntities.size(); i++)
	{
		ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).SetPosition(movement, true);
	}
}

void ShrinkTrigger::OnExit()
{
	Trigger::OnExit();
}
