#include "DestroyTrigger.h"
#include "ECS.h"
void DestroyTrigger::OnTrigger()
{
	if (!triggered)
	{
		PhysicsBody::m_bodiesToDelete.push_back(m_entity);
		triggered = true; 
	}
}
