#pragma once
#include <Box2D/Box2D.h>
class PhysicsPlaygroundListener : public b2ContactListener
{
public:
	PhysicsPlaygroundListener();

	void BeginContact(b2Contact* contact) override;
	void EndContact(b2Contact* contact) override;
private:
	void TriggerObject(b2Fixture* sensor);
};

