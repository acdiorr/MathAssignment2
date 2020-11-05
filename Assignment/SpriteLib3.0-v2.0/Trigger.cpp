#include "Trigger.h"

#include "ECS.h"

void Trigger::OnTrigger()
{
	//make the object linked transparent
	ECS::GetComponent<Sprite>(m_entity).SetTransparency(0.2f);
}

void Trigger::SetEntity(int entity)
{
	m_entity = entity;
}

int Trigger::GetEntity()
{
	return m_entity;
}
