#include "PhysicsPlayground.h"
#include "Utilities.h"

#include <random>

PhysicsPlayground::PhysicsPlayground(std::string name)
	: Scene(name)
{
	//No gravity this is a top down scene
	m_gravity = b2Vec2(0.f, -65.f);
	m_physicsWorld->SetGravity(m_gravity);

	m_physicsWorld->SetContactListener(&listener);
}

void PhysicsPlayground::InitScene(float windowWidth, float windowHeight)
{
	//Dynamically allocates the register
	m_sceneReg = new entt::registry;

	//Music time
	SoundFunctions::LoadSound("WildWestMusic.mp3");
	SoundFunctions::LoopSound("WildWestMusic.mp3");

	//Attach the register
	ECS::AttachRegister(m_sceneReg);

	//Sets up aspect ratio for the camera
	float aspectRatio = windowWidth / windowHeight;

	EffectManager::CreateEffect(EffectType::Grain, windowWidth, windowHeight);
	EffectManager::CreateEffect(EffectType::Sepia, windowWidth, windowHeight);
	SepiaEffect* sepiaTemp = (SepiaEffect*)EffectManager::GetEffect(EffectManager::GetSepiaHandle());
	sepiaTemp->SetIntensity(0.7f);
	EffectManager::CreateEffect(EffectType::Vignette, windowWidth, windowHeight);
	VignetteEffect* vignetteTemp = (VignetteEffect*)EffectManager::GetEffect(EffectManager::GetVignetteHandle());
	vignetteTemp->SetInnerRadius(0.3f);

	//music lul
	//PlaySound("WestMusic.wav", NULL, SND_FILENAME);
	//Setup MainCamera Entity
	{
		/*Scene::CreateCamera(m_sceneReg, vec4(-75.f, 75.f, -75.f, 75.f), -100.f, 100.f, windowWidth, windowHeight, true, true);*/

		//Creates Camera entity
		auto entity = ECS::CreateEntity();
		ECS::SetIsMainCamera(entity, true);

		//Creates new orthographic camera
		ECS::AttachComponent<Camera>(entity);
		ECS::AttachComponent<HorizontalScroll>(entity);
		ECS::AttachComponent<VerticalScroll>(entity);

		//set to 75 ok
		float num = 75.f;

		vec4 temp = vec4(-num, num, -num, num);
		ECS::GetComponent<Camera>(entity).SetOrthoSize(temp);
		ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
		ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100.f, 100.f);

		//Attaches the camera to vert and horiz scrolls
		ECS::GetComponent<HorizontalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		ECS::GetComponent<VerticalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
	}

	//Setup new Entity
	{
		/*Scene::CreateSprite(m_sceneReg, "HelloWorld.png", 100, 60, 0.5f, vec3(0.f, 0.f, 0.f));*/

		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//Set up the components
		std::string fileName = "floppa.gif";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 200, 160);
		ECS::GetComponent<Sprite>(entity).SetTransparency(0.8f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 0.f));
	}
	
	//McCree :DDDD
	{
		/*Scene::CreatePhysicsSprite(m_sceneReg, "LinkStandby", 80, 60, 1.f, vec3(0.f, 30.f, 2.f), b2_dynamicBody, 0.f, 0.f, true, true)*/

		auto entity = ECS::CreateEntity();
		ECS::SetIsMainPlayer(entity, true);

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<CanJump>(entity);

		//Sets up the components
		std::string fileName = "McCree.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 32, 32);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 30.f, 2.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(0.f), float32(30.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		//tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);
		tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetHeight() - shrinkY)/2.f), vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);
		//std::vector<b2Vec2> points = { b2Vec2(-tempSpr.GetWidth() / 2.f, -tempSpr.GetHeight() / 2.f), b2Vec2(tempSpr.GetWidth() / 2.f, -tempSpr.GetHeight() / 2.f), b2Vec2(0, tempSpr.GetHeight() / 2.f) };
		//tempPhsBody = PhysicsBody(entity, BodyType::TRIANGLE, tempBody, points, vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);

		tempPhsBody.SetRotationAngleDeg(0.f);
		tempPhsBody.SetFixedRotation(true);
		tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));
		tempPhsBody.SetGravityScale(1.f);
	}


	//----------------------STARTING LEVEL SECTION-------------------------------\\


	//Setup Initial Static Box
	CreateBoxEntity("boxSprite.jpg", 150, 10, -10.f, -30.f);
	//Setup Static Ramp BOX
	CreateBoxEntity("boxSprite.jpg", 150, 10, 127.5f, -67.f, -30.f);
	//Another floor
	CreateBoxEntity("boxSprite.jpg", 150, 10, 265.f, -105.f);
	//Setup Static WALL TOP
	CreateBoxEntity("boxSprite.jpg", 55, 15, 332.f, -28.f, 90.f);
	//Setup Static WALL BOTTOM
	CreateSpriteEntity(true, false, true, &wall, "boxSprite.jpg", 45, 15, 45.f, -20.f, 2.f, 332.f, -65.f, 90.f, GROUND, PLAYER | ENEMY, 1.f, 1.f);


	//Create platform 
	CreateBoxEntity("boxSprite.jpg", 50, 10, 127.f, 0.f);
	//Create platform 2 
	CreateBoxEntity("boxSprite.jpg", 50, 10, 227.f, 0.f);

	//Create floor for trigger room 
	CreateBoxEntity("boxSprite.jpg", 100, 10, 377.f, -5.f);
	//Create Elevator
	CreateSpriteEntity(true, false, true, &elevator, "boxSprite.jpg", 45, 5, 45.f, -20.f, 2.f, 465.f, -105.f, 0, ENVIRONMENT, PLAYER | ENEMY, 0.3f, 1.f);
	//Create wall for trigger room/elevator shaft 
	CreateBoxEntity("boxSprite.jpg", 250, 10, 422.f, 125.f, 90.f);


	//Setup Static Ramp 2
	CreateBoxEntity("boxSprite.jpg", 70, 10, 367.f, -122.f, -30.f);
	//Setup platform room 2
	CreateBoxEntity("boxSprite.jpg", 50, 10, 420.f, -110.f);
	//Setup wall bottom
	CreateBoxEntity("boxSprite.jpg", 40, 10, 410.f, -125.f, 90.f);
	//Setup bottom
	CreateBoxEntity("boxSprite.jpg", 30, 10, 400.f, -140.f);
	//Setup platorm 2 for room 2 
	CreateBoxEntity("boxSprite.jpg", 100, 10, 450.f, -110.f);

	//Setup right wall for elevator shaft
	CreateBoxEntity("boxSprite.jpg", 300, 10, 495.f, 40.f, 90);


	//----------------------POST ELEVATOR SECTION-------------------------------\\

	//Initial platform that you get off on
	CreateBoxEntity("boxSprite.jpg", 100, 10, 540.f, 190.f);
	//This is where the moving platform will be lul, for now just a basic platform 
	CreateBoxEntity("boxSprite.jpg", 50, 10, 700.f, 190.f);

	//Platform for the Triangle Puzzle
	CreateBoxEntity("boxSprite.jpg", 150, 10, 940.f, 210.f);
	//Triangle slot left
	CreateBoxEntity("boxSprite.jpg", 24, 10, 1020.f, 203.f, -45);
	//Triangle slot right
	CreateBoxEntity("boxSprite.jpg", 24, 10, 1030.f, 203.f, 45);

	//Ball
	{
		auto entity = ECS::CreateEntity();
		ball = entity;
		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		//Sets up the components
		std::string fileName = "happyBall.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 20, 20);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(45.f, -8.f, 3.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(45.f), float32(-8.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		//tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false);
		tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetWidth() - shrinkY) / 2.f), vec2(0.f, 0.f), false, OBJECTS, GROUND | ENVIRONMENT | PLAYER | TRIGGER, 0.3f);

		tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));
	}

	//Setup trigger
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<Trigger*>(entity);
		
		//Sets up components
		std::string fileName = "boxSprite.jpg";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 15, 15);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -20.f, 80.f));
		ECS::GetComponent<Trigger*>(entity) = new ShrinkTrigger();
		ECS::GetComponent<Trigger*>(entity)->SetTriggerEntity(entity);
		ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(ball);
		ShrinkTrigger* temp = (ShrinkTrigger*) ECS::GetComponent<Trigger*>(entity);
		temp->scale = -1.f;


		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;
		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(408.f), float32(7.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), true, TRIGGER, PLAYER | OBJECTS);
		tempPhsBody.SetColor(vec4(1.f, 0.f, 0.f, 0.3f));
	}

	//Setup elevator trigger
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<Trigger*>(entity);

		//Sets up components
		std::string fileName = "boxSprite.jpg";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 15, 15);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -20.f, 80.f));
		ECS::GetComponent<Trigger*>(entity) = new TranslateTrigger();
		ECS::GetComponent<Trigger*>(entity)->SetTriggerEntity(entity);
		ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(elevator);
		TranslateTrigger* temp = (TranslateTrigger*)ECS::GetComponent<Trigger*>(entity);
		temp->movement = b2Vec2(0, 15.f);

		for (int i = 0; i < 50; i++)
		{
			temp->movement.Set(465.f, -104.f + i);
		}

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;
		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(435.f), float32(-105.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), true, TRIGGER, PLAYER | OBJECTS);
		tempPhsBody.SetColor(vec4(1.f, 0.f, 0.f, 0.3f));
	}
	CreateDestroyTrigger(wall, "boxSprite.jpg", 5, 5, 30.f, -20.f, 80.f, 400.f, -130.f);

	ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
}

void PhysicsPlayground::Update()
{

}


void PhysicsPlayground::KeyboardHold()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
	auto& canJump = ECS::GetComponent<CanJump>(MainEntities::MainPlayer());
	//set to 15, we just building rn
	float speed = 30.f;
	b2Vec2 vel = b2Vec2(0.f, 0.f);

	if (Input::GetKey(Key::Shift))
	{
		speed *= 2.8f;
	}

	if (Input::GetKey(Key::A))
	{
		vel += b2Vec2(-1.f, 0.f);
	}
	if (Input::GetKey(Key::D))
	{
		vel += b2Vec2(1.f, 0.f);
	}
	// I know this function is only for getKey, but why overcomplicate things?
	if (canJump.m_canJump)
	{
		if (Input::GetKeyDown(Key::Space))
		{
			vel += b2Vec2(0.f, 80.f);
			//canJump.m_canJump = false;
		}
	}

	//Change physics body size for circle
	if (Input::GetKey(Key::O))
	{
		player.ScaleBody(1.3 * Timer::deltaTime, 0);
	}
	else if (Input::GetKey(Key::I))
	{
		player.ScaleBody(-1.3 * Timer::deltaTime, 0);
	}

	b2Vec2 NewLinearVelocity = b2Vec2(vel.x * speed, player.GetVelocity().y + vel.y);

	player.GetBody()->SetLinearVelocity(NewLinearVelocity);
}

void PhysicsPlayground::KeyboardDown()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());

	if (Input::GetKeyDown(Key::T))
	{
		PhysicsBody::SetDraw(!PhysicsBody::GetDraw());
	}

}

void PhysicsPlayground::KeyboardUp()
{
	

}
