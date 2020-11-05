#pragma once


class Trigger
{
public:
	//Override this in own triggers, make it do something
	virtual void OnTrigger();

	void SetEntity(int entity);
	int GetEntity();
protected:
	int m_entity;
};

