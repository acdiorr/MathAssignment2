#include "PhysicsPlayground.h"
#include "Utilities.h"

#include <random>

PhysicsPlayground::PhysicsPlayground(std::string name)
	: Scene(name)
{
	//No gravity this is a top down scene
	m_gravity = b2Vec2(0.f, -95.f);
	m_physicsWorld->SetGravity(m_gravity);

	m_physicsWorld->SetContactListener(&listener);
}

void PhysicsPlayground::InitScene(float windowWidth, float windowHeight)
{
	//Dynamically allocates the register
	m_sceneReg = new entt::registry;

	//Music time
	SoundFunctions::LoadSound("MirageSaloon.mp3");
	SoundFunctions::AdjustVolume("MirageSaloon.mp3", 100);
	SoundFunctions::LoopSound("MirageSaloon.mp3");
	SoundFunctions::LoadSound("oof.mp4.mp3");
	SoundFunctions::AdjustVolume("oof.mp4.mp3", 100);

	//Attach the register
	ECS::AttachRegister(m_sceneReg);

	//Sets up aspect ratio for the camera
	float aspectRatio = windowWidth / windowHeight;

	EffectManager::CreateEffect(EffectType::Grain, windowWidth, windowHeight);
	GrainEffect* grainTemp = (GrainEffect*)EffectManager::GetEffect(EffectManager::GetGrainHandle());
	grainTemp->SetStrength(100.f);

	EffectManager::CreateEffect(EffectType::Sepia, windowWidth, windowHeight);
	SepiaEffect* sepiaTemp = (SepiaEffect*)EffectManager::GetEffect(EffectManager::GetSepiaHandle());
	sepiaTemp->SetIntensity(0.7f);

	EffectManager::CreateEffect(EffectType::Vignette, windowWidth, windowHeight);
	VignetteEffect* vignetteTemp = (VignetteEffect*)EffectManager::GetEffect(EffectManager::GetVignetteHandle());
	vignetteTemp->SetInnerRadius(0.3f);

	//Setup MainCamera Entity
	{
		/*Scene::CreateCamera(m_sceneReg, vec4(-75.f, 75.f, -75.f, 75.f), -100.f, 100.f, windowWidth, windowHeight, true, true);*/

		//Creates Camera entity
		auto entity = ECS::CreateEntity();
		player = entity;
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
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 2000, 1600);
		ECS::GetComponent<Sprite>(entity).SetTransparency(0.8f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, -40.f));
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
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 30.f, 10.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 8.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(-5.f), float32(5.f));
		tempBody = m_physicsWorld->CreateBody(&tempDef);

		//tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);
		tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetHeight() - shrinkY)/2.f), vec2(0.f, 0.f), false, PLAYER, GROUND | ENEMY | OBJECTS | ENVIRONMENT | PICKUP | TRIGGER, 0.5f, 3.f);
		//std::vector<b2Vec2> points = { b2Vec2(-tempSpr.GetWidth() / 2.f, -tempSpr.GetHeight() / 2.f), b2Vec2(tempSpr.GetWidth() / 2.f, -tempSpr.GetHeight() / 2.f), b2Vec2(0, tempSpr.GetHeight() / 2.f) };
		//tempPhsBody = PhysicsBody(entity, BodyType::TRIANGLE, tempBody, points, vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);

		tempPhsBody.SetRotationAngleDeg(0.f);
		tempPhsBody.SetFixedRotation(true);
		tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));
		tempPhsBody.SetGravityScale(1.f);
	}


	//----------------------STARTING LEVEL SECTION-------------------------------\\

	//Setup Initial Static Box
	CreateBoxEntity("floor.png", 120, 20, -7.5f, -34.f);
	//Setup Static Ramp BOX
	CreateBoxEntity("trianglefloor.png", 150, 76, 127.f, -62.f, 0, false);
	//Another floor
	CreateBoxEntity("floor.png", 178, 15, 248.f, -107.5f);
	//Setup Static WALL TOP
	CreateBoxEntity("plank.png", 55, 15, 332.f, -28.f, 90.f, true, -30.f);
	//Setup TRIGGER WALL BOTTOM
	CreateSpriteEntity(1, 2, true, &wall, "plank.png", 45, 15, 45.f, -20.f, 2.f, 332.f, -65.f, 0, 0, 90.f, OBJECTS, PLAYER | ENEMY | OBJECTS, 1.f, 1.f);

	//Create platform 
	CreateBoxEntity("platform.png", 50, 10, 127.f, 0.f);
	//Create platform 2 
	CreateBoxEntity("platform.png", 50, 10, 227.f, 0.f);

	//Create floor for trigger room 
	CreateBoxEntity("floor.png", 120, 20, 368.f, -5.f);

	//Create Elevator
	//CreateSpriteEntity(3, false, true, &elevator, "boxSprite.jpg", 45, 5, 45.f, -20.f, 2.f, 460.f, -105.f, 0, 22.f, 0, ENVIRONMENT, PLAYER | ENEMY, 0.3f, 1.f);
	{
		//Create Entity
		auto entity = ECS::CreateEntity();
		ECS::SetIsMainPlatform1(entity, true);
		elevator = entity;

		//Adds Components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		//Setup the Components
		std::string fileName =	"platform.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 45, 9);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(45.f, -20.f, 2.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;
		b2Body* tempBody;
		b2BodyDef tempDef;

		tempDef.type = b2_kinematicBody;

		tempDef.position.Set(float32(460.f), float32(-105.f)); //set position

		tempBody = m_physicsWorld->CreateBody(&tempDef);
		//tempBody->SetLinearVelocity(b2Vec2(0, 22.f));

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, OBJECTS, PLAYER | ENEMY, 10.f, 1.f);
		tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
		tempPhsBody.SetRotationAngleDeg(0);
	}

	//Create wall for trigger room/elevator shaft 
	CreateBoxEntity("plank.png", 410, 12, 422.f, 195.f, 90.f, true, -12.f);
	//DECORATION
	CreateDecoration("plankVertical.png", 12, 170, 422.f, -20.f, -15.f);
	//Setup Static Ramp 2
	CreateBoxEntity("trianglefloor.png", 78, 44.5f, 376.f, -122.f, 0, false);

	//Setup wall bottom
	CreateBoxEntity("plank.png", 40, 10, 410.f, -125.f, 90.f, true, -30.f);
	//Setup bottom
	CreateBoxEntity("floor.png", 30, 10, 400.f, -140.f, 0, true, -35.f);
	//Setup platorm 2 for room 2 
	CreateBoxEntity("floor.png", 112, 10, 445.f, -110.f);
	//Setup right wall for elevator shaft
	CreateBoxEntity("plank.png", 300, 12, 495.f, 40.f, 90, true, -20.f);
	//DECORATION
	CreateDecoration("plankVertical.png", 12, 110, 495.f, 240.f, -15.f);
	//DECORATIONd
	CreateDecoration("plankVertical.png", 12, 105, 495.f, 350.f, -15.f);
	//DECORATION
	CreateDecoration("plankVertical.png", 12, 110, 593.f, 240.f, -15.f);
	//DECORATION
	CreateDecoration("plankVertical.png", 12, 105, 593.f, 350.f, -15.f);
	//ROOF OF START
	CreateBoxEntity("floor.png", 391, 10, 235.f, 190.f);
	//LEFT ROOF WALL
	CreateBoxEntity("plank.png", 180, 10, 46.f, 160.f, 90, true, -20.f);
	//DECORATION
	CreateDecoration("plankVertical.png", 10, 100, 46.f, 20.f, -15.f);
	//ROOF OF SPAWN
	CreateBoxEntity("floor.png", 120, 10, -7.5f, 70.f);
	//WALL OF SPAWN
	CreateBoxEntity("plank.png", 100, 10, -60.f, 20.f, 90, true, -20.f);
	//DECORATION
	CreateDecoration("floorVertical.png", 5, 100, -64.5f, 20.f, -15.f);
	//DOOR DECORATION
	CreateDecoration("door.png", 40, 83, -5, 0, -10.f);

	//BACKGROUND WOOD
	CreateDecoration("floor3.png", 500, 160, -191.f, -108.f, -39.f);
	CreateDecoration("floor3.png", 500, 260, -203.f, 102.f, -39.f);
	CreateDecoration("floor3.png", 500, 160, 172.f, 270.f, -39.f);
	CreateDecoration("floor3.png", 500, 160, 88.f, -180.f, -39.f);
	CreateDecoration("floor3.png", 500, 170, 348.f, -195.f, -39.f);
	CreateDecoration("floor3.png", 500, 160, 345.f, 380.f, -39.f);
	CreateDecoration("floor3.png", 500, 160, 745.f, 480.f, -39.f);
	CreateDecoration("floor3.png", 500, 160, 965.f, 480.f, -39.f);

	//----------------------POST ELEVATOR SECTION-------------------------------\\

	//ROOF ELEVATOR SHAFT
	CreateBoxEntity("floor.png", 434, 10, 630.f, 300.f, 0, true, -10.f);
	//ROOF WIN AREA
	CreateBoxEntity("floor.png", 934, 10, 880.f, 400.f, 0, true, -10.f);
	//WALL WIN AREA
	CreateDecoration("floorVertical.png", 5, 100, 417.5f, 345.f, -15.f);
	//Initial platform that you get off on
	CreateBoxEntity("floor.png", 112, 10, 544.f, 190.f, 0, true, -10.f);

	//MOVING PLATFORM
	{
		//Create Entity
		auto entity = ECS::CreateEntity();
		ECS::SetIsMainPlatform2(entity, true);

		//Adds Components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		//Setup the Components
		std::string fileName = "platform.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 45, 9);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(45.f, -20.f, 2.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;
		b2Body* tempBody;
		b2BodyDef tempDef;

		tempDef.type = b2_kinematicBody;

		tempDef.position.Set(float32(630.f), float32(190.f)); //set position

		tempBody = m_physicsWorld->CreateBody(&tempDef);
		//tempBody->SetLinearVelocity(b2Vec2(0, 22.f));

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, OBJECTS, PLAYER | ENEMY, 1000.f, 1.f);
		tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
		tempPhsBody.SetRotationAngleDeg(0);
	}

	//Platform for the Triangle Puzzle
	CreateBoxEntity("floor.png", 150, 10, 940.f, 210.f);
	//Triangle slot left
	CreateBoxEntity("floor.png", 24, 10, 1020.f, 203.f, -45);
	//Triangle slot right
	CreateBoxEntity("floor.png", 24, 10, 1030.f, 203.f, 45);
	//Platform that will continue until the next lil' hole
	CreateBoxEntity("floor.png", 150, 10, 1110.f, 210.f);
	//Square slot left
	CreateBoxEntity("floor.png", 20, 10, 1180.f, 205.f, 90);
	//Square slot bottom
	CreateBoxEntity("floor.png", 20, 10, 1190.f, 200.f);
	//Square slot right
	CreateBoxEntity("floor.png", 20, 10, 1200.f, 205.f, 90);
	//Platform that will continue until the end lul
	CreateBoxEntity("floor.png", 150, 10, 1270.f, 210.f);
	//Wall off the player
	CreateBoxEntity("floor.png", 300, 10, 1340.f, 360.f, 90);

	//OBJECTS FOR FIRST PUZZLE\\

	//BALL
	CreateSpriteEntity(2, 1, true, &ball, "happyBall.png", 20, 20, 45.f, -8.f, 3.f, 45.f, -8.f, 0, 0, 0, OBJECTS, OBJECTS | GROUND | ENVIRONMENT | PLAYER | TRIGGER, 0.3f, 1.f);
	//Setup Ball Scale
	CreateScaleTrigger(ball, false, "booze.png", 15, 20, 30.f, -20.f, 80.f, 405.f, 7.f);
	//Setup Wall Destroy
	CreateDestroyTrigger(wall, true, "boxSprite.jpg", 5, 5, 30.f, -20.f, 80.f, 400.f, -130.f);

	//OBJECTS FOR SECOND PUZZLE\\
	
	//Wall to last puzzle
	CreateSpriteEntity(1, 2, true, &wall3, "floor.png", 300, 10, 1080.f, 360.f, 20.f, 1080, 360.f, 0, 0, 90.f, GROUND, OBJECTS | ENVIRONMENT | PLAYER, 1.f, 1.f);
	//Stairs
	CreateSpriteEntity(1, 3, true, &stairs, "trianglefloor.png", 160, 90, 127.f, -62.f, -20.f, 0.f, 300.f, 0, 0, 0, OBJECTS, PLAYER | ENEMY | OBJECTS, 1.f, 1.f);

	//Open Blocked Win area
	CreateTranslateTrigger(stairs, true, "boxSprite.jpg", 5, 5, 30.f, -20.f, 80.f, 927.f, 260.f, 1025.f, 204.f);
	//Open Next Puzzle
	CreateDestroyTrigger(wall3, true, "boxSprite.jpg", 5, 5, 30.f, -20.f, 80.f, 1025.f, 204.f);
	//Triangle
	CreateTriangleEntity("triangleObj.png", 40, 20, 970.f, 230.f);

	//OBJECTS FOR THE THIRD PUZZLE\\

	//Win Wall
	CreateSpriteEntity(1, 2, true, &wall2, "floor.png", 100, 15, 45.f, -20.f, 2.f, 750.f, 345.f, 0, 0, 90.f, GROUND, PLAYER | ENEMY, 1.f, 1.f);
	//Destroy Win Wall
	CreateDestroyTrigger(wall2, true, "boxSprite.jpg", 5, 5, 30.f, -20.f, 80.f, 1190.f, 204.f);

	//The correct piece
	CreateBoxEntity("boxSprite.jpg", 8, 8, 1300.f, 360.f, 0, true, 2.f, true);
	//wrong stupid ones lul
	CreateSpriteEntity(2, 1, true, &noTouch, "happyBall.png", 30, 20, 1240.f, 370.f, 3.f, 1240.f, 300.f, 0, 0, 0, OBJECTS, GROUND | ENVIRONMENT | PLAYER | OBJECTS, 0.3f, 1.f);
	CreateSpriteEntity(2, 1, true, &noTouch, "happyBall.png", 20, 14, 1290.f, 380.f, 3.f, 1290.f, 300.f, 0, 0, 0, OBJECTS, GROUND | ENVIRONMENT | PLAYER | OBJECTS, 0.3f, 1.f);
	CreateSpriteEntity(2, 1, true, &noTouch, "happyBall.png", 20, 20, 1270.f, 390.f, 3.f, 1270.f, 300.f, 0, 0, 0, OBJECTS, GROUND | ENVIRONMENT | PLAYER | OBJECTS, 0.3f, 1.f);
	CreateBoxEntity("boxSprite.jpg", 40, 15, 1290.f, 300.f, 0, true, 2.f, true);
	CreateBoxEntity("boxSprite.jpg", 50, 39, 1280.f, 300.f, 0, true, 2.f, true);
	CreateBoxEntity("boxSprite.jpg", 20, 50, 1280.f, 300.f, 0, true, 2.f, true);


	//OBJECT FOR WINNING\\

	//Win image
	CreateSpriteEntity(1, 1, true, &image, "win.png", 150, 150, 1000.f, -600.f, 100.f, 1000.f, -600.f, 0, 0, 0, OBJECTS, 0, 1.f, 1.f);

	//win Trigger
	CreateTranslateTrigger(image, false, "borgar.png", 10, 10, 30.f, -20.f, 80.f, 460.f, 317.f, 460.f, 317.f);
	endTriggerEntity = CreateEndTrigger(false, "borgar.png", 10, 10, 30.f, -20.f, 80.f, 460.f, 317.f);



	ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
}

void PhysicsPlayground::Update()
{
	auto& elevator = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlatform1());
	auto& platform = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlatform2());
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());

	if (elevator.GetPosition().y <= -105) //go up
	{
		elevator.GetBody()->SetLinearVelocity(b2Vec2(0.f, 20.f));
	}
	if (elevator.GetPosition().y >= 200) //go down
	{
		elevator.GetBody()->SetLinearVelocity(b2Vec2(0.f, -20.f));
	}

	if (platform.GetPosition().x <= 630)
	{
		platform.GetBody()->SetLinearVelocity(b2Vec2(20.f, 0.f));
	}
	if (platform.GetPosition().x >= 820)
	{
		platform.GetBody()->SetLinearVelocity(b2Vec2(-20.f, 0.f));
	}

	if (player.GetPosition().y <= -200) //death barrier
	{
		player.SetPosition(b2Vec2(544, 210), true);
	}
	((EndTrigger*)(ECS::GetComponent<Trigger*>(endTriggerEntity)))->OnUpdate();
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
		speed *= 1.8f;
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
			SoundFunctions::ReplaySound("oof.mp4.mp3");
			vel += b2Vec2(0.f, 80.f);
			canJump.m_canJump = false;
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

	else if (Input::GetKey(Key::R))
	{
		std::cout << "X: " << player.GetPosition().x << "\n" << "Y: " << player.GetPosition().y << "\n";
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
