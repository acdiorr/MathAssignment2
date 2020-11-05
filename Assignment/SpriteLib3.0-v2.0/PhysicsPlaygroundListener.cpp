#include "PhysicsPlaygroundListener.h"

#include "ECS.h"

PhysicsPlaygroundListener::PhysicsPlaygroundListener()
	: b2ContactListener()
{
}

void PhysicsPlaygroundListener::BeginContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	bool sensorA = fixtureA->IsSensor();
	bool sensorB = fixtureB->IsSensor();

	//If neither or both are sensors, will be false
	if (sensorA ^ sensorB)
	{
		if (sensorA)
		{
			TriggerObject(fixtureA);
		}
		else if (sensorB)
		{
			TriggerObject(fixtureB);
		}
	}
}

void PhysicsPlaygroundListener::EndContact(b2Contact* contact)
{

}

void PhysicsPlaygroundListener::TriggerObject(b2Fixture* sensor)
{
	int entity = (int)sensor->GetBody()->GetUserData();

	ECS::GetComponent<Trigger*>(entity)->OnTrigger();
}
