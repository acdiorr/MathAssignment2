#include "TranslateTrigger.h"
#include "ECS.h"

void TranslateTrigger::OnTrigger()
{
	Trigger::OnTrigger();
}

void TranslateTrigger::OnEnter()
{
	Trigger::OnEnter();

	if (!triggered)
	{
		for (int i = 0; i < m_targetEntities.size(); i++)
		{
			//ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).SetPosition(ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).GetPosition() + movement, true);
			//ECS::GetComponent<PhysicsBody>(m_targetEntities[i])->ECS::AttachComponent<b2Body>.SetLinearVelocity(movement));
			ECS::GetComponent<PhysicsBody>(m_targetEntities[i]).SetPosition(movement, true);
		}
	}
	
	triggered = false;
}

void TranslateTrigger::OnExit()
{
	Trigger::OnExit();
}
