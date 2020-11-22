#include "Scene.h"
#include "Utilities.h"
#include "PhysicsPlayground.h"

Scene::Scene(std::string name)
{
	m_physicsWorld = new b2World(m_gravity);
	m_name = name;
}

void Scene::Unload()
{
	if (m_sceneReg != nullptr)
	{
		delete m_sceneReg;
		m_sceneReg = nullptr;
	}

	if (m_physicsWorld != nullptr)
	{
		delete m_physicsWorld;
		m_physicsWorld = nullptr;
	}
}

void Scene::InitScene(float windowWidth, float windowHeight)
{
	//Default scene class init just includes a camera so stuff doesn't immediately crash
	printf("You shouldn't be running this, while initializing this Scene Type.\n");

	//Dynamically allocates the register
	if (m_sceneReg == nullptr)
	{
		m_sceneReg = new entt::registry;
	}

	//Attach the register
	ECS::AttachRegister(m_sceneReg);

	//Sets up aspect ratio for the camera
	float aspectRatio = windowWidth / windowHeight;

	//Setup Main Camera entity
	{
		//Creates Camera entity
		auto entity = ECS::CreateEntity();
		ECS::SetIsMainCamera(entity, true);

		//Creates new orthographic camera
		ECS::AttachComponent<Camera>(entity);
		//ECS::AttachComponent<HorizontalScroll>(entity);
		vec4 temp = vec4(-75.f, 75.f, -75.f, 75.f);
		ECS::GetComponent<Camera>(entity).SetOrthoSize(temp);
		ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
		ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100.f, 100.f);
	}

	//Setup new Entity
	{
		//Creates entity
		auto entity = ECS::CreateEntity();
		m_helloWorldSign = entity;

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//Sets up components
		std::string fileName = "HelloWorld.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 100, 60);
		ECS::GetComponent<Sprite>(entity).SetTransparency(0.5f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 0.f));
	}
}

void Scene::Update()
{
	auto& tempSpr = m_sceneReg->get<Sprite>(m_helloWorldSign);
	
	tempSpr.SetTransparency((0.5 * sin(Timer::time * 3.f)) + 0.5f);
}

void Scene::AdjustScrollOffset()
{
	float maxSizeX = ECS::GetComponent<Camera>(MainEntities::MainCamera()).GetOrthoSize().y;
	float maxSizeY = ECS::GetComponent<Camera>(MainEntities::MainCamera()).GetOrthoSize().w;

	float playerHalfSize = ECS::GetComponent<Sprite>(MainEntities::MainPlayer()).GetWidth() / 2.f;

	ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetOffset((maxSizeX * BackEnd::GetAspectRatio()) - playerHalfSize);
	ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetOffset(maxSizeY - playerHalfSize);
}

void Scene::CreateDecoration(std::string name, int sprX, int sprY, int posX, int posY, int posZ)
{
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);

	//Set up the components
	std::string fileName = name;
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, sprX, sprY);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(posX, posY, posZ));
}

void Scene::CreateCameraEntity(bool mainCamera, float windowWidth, float windowHeight, float left, float right, float bottom, float top, 
									float zNear, float zFar, float aspectRatio, bool vertScroll, bool horizScroll)
{
	//Setup main camera
	{
		//Creates Camera Entity
		auto entity = ECS::CreateEntity();
		ECS::SetIsMainCamera(entity, false);

		ECS::AttachComponent<Camera>(entity);
		if (horizScroll)
		{
			ECS::AttachComponent<HorizontalScroll>(entity);
		}
		if (vertScroll)
		{
			ECS::AttachComponent<VerticalScroll>(entity);
		}

		vec4 temp = vec4(left, right, bottom, top);
		ECS::GetComponent<Camera>(entity).SetOrthoSize(temp);
		ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
		ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, zNear, zFar);

		if (horizScroll)
		{
			ECS::GetComponent<HorizontalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		}
		if (vertScroll)
		{
			ECS::GetComponent<VerticalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		}
	}
}
void Scene::CreateBoxEntity(std::string fileName, int spriteX, int spriteY, int vecX, int vecY, int rotDeg, bool square, int vecZ, bool isDynamic, float shrinkXValue, float shrinkYValue)
{
	//Creates entity
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	//Sets up components
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, spriteX, spriteY);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(vecX, vecY, vecZ));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;

	if (isDynamic)
	{
		tempDef.type = b2_dynamicBody;
	}
	else
	{
		tempDef.type = b2_staticBody;
	}
	tempDef.position.Set(float32(vecX), float32(vecY));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	if (square)
	{
		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkXValue), float(tempSpr.GetHeight() - shrinkYValue), vec2(0.f, 0.f), false, OBJECTS, PLAYER | ENEMY | GROUND | OBJECTS | TRIGGER);
	}
	else
	{
		std::vector<b2Vec2> points = { b2Vec2(-tempSpr.GetWidth() / 2.f, -tempSpr.GetHeight() / 2.f), b2Vec2(tempSpr.GetWidth() / 2.f, -tempSpr.GetHeight() / 2.f), b2Vec2(-tempSpr.GetWidth() / 2.f, tempSpr.GetHeight() / 2.f) };
		tempPhsBody = PhysicsBody(entity, BodyType::RIGHTTRIANGLE, tempBody, points, vec2(0.f, 0.f), false, OBJECTS, PLAYER | ENEMY | GROUND | OBJECTS, 0.5f, 3.f); //right triangle
	}

	tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
	tempPhsBody.SetRotationAngleDeg(rotDeg);
}

void Scene::CreateSpriteEntity(int type, int body, bool triggerable, int* name, std::string file, int height, int width, int posx, int posy, int posz, int posX, int posY, float movingX, float movingY, 
								int rotDeg, EntityCategories category, int hitting, float friction, float density)
{
	//Create Entity
	auto entity = ECS::CreateEntity();
	if (triggerable) //set true
	{
		*name = entity;
	}

	//Adds Components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	//Setup the Components
	std::string fileName = file;
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, height, width);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(posx, posy, posz));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	float shrinkX = 0.f;
	float shrinkY = 0.f;
	b2Body* tempBody;
	b2BodyDef tempDef;

	if (type == 1)
	{
		tempDef.type = b2_staticBody;
	}
	if (type == 2)
	{
		tempDef.type = b2_dynamicBody;
	}
	if (type == 3)
	{
		tempDef.type = b2_kinematicBody;
	}

	tempDef.position.Set(float32(posX), float32(posY)); //set position

	tempBody = m_physicsWorld->CreateBody(&tempDef);
	tempBody->SetLinearVelocity(b2Vec2(movingX, movingY));
	if (body == 1)
	{
		tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetWidth() - shrinkY) / 2.f), vec2(0.f, 0.f), false, category, hitting, friction, density);
	}
	if (body == 2)
	{
		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, category, hitting, friction, density);
	}
	if (body == 3)
	{
		std::vector<b2Vec2> points = { b2Vec2(-tempSpr.GetWidth() / 2.f, -tempSpr.GetHeight() / 2.f), b2Vec2(tempSpr.GetWidth() / 2.f, -tempSpr.GetHeight() / 2.f), b2Vec2(-tempSpr.GetWidth() / 2.f, tempSpr.GetHeight() / 2.f) };
		tempPhsBody = PhysicsBody(entity, BodyType::RIGHTTRIANGLE, tempBody, points, vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY | OBJECTS, 0.5f, 3.f); //right triangle
	}
	tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
	tempPhsBody.SetRotationAngleDeg(rotDeg);
}

void Scene::CreateTriangleEntity(std::string fileName, int spriteX, int spriteY, int vecX, int vecY, int rotDeg, int vecZ, float shrinkXValue, float shrinkYValue)
{
	//Creates entity
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	//Sets up components
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, spriteX, spriteY);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(vecX, vecY, vecZ));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_dynamicBody;
	tempDef.position.Set(float32(vecX), float32(vecY));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	std::vector<b2Vec2> points = { b2Vec2(-tempSpr.GetWidth() / 2.f, -tempSpr.GetHeight() / 2.f), b2Vec2(tempSpr.GetWidth() / 2.f, -tempSpr.GetHeight() / 2.f), b2Vec2(0, tempSpr.GetHeight() / 2.f) };
	tempPhsBody = PhysicsBody(entity, BodyType::TRIANGLE, tempBody, points, vec2(0.f, 0.f), false, OBJECTS, PLAYER | ENEMY | OBJECTS | TRIGGER, 0.5f, 3.f);

	tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
	tempPhsBody.SetRotationAngleDeg(rotDeg);
}

void Scene::CreateScaleTrigger(int targetName, bool invisible, std::string fileName, int spriteX, int spriteY, int vecX, int vecY, int vecZ, int posX, int posY)
{
	///Creates entity
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<Trigger*>(entity);

	//Sets up components
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, spriteX, spriteY);
	if (invisible)
	{
		ECS::GetComponent<Sprite>(entity).SetTransparency(0.f);
	}
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(vecX, vecY, vecZ));
	ECS::GetComponent<Trigger*>(entity) = new ShrinkTrigger();
	ECS::GetComponent<Trigger*>(entity)->SetTriggerEntity(entity);
	ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(targetName);
	ShrinkTrigger* temp = (ShrinkTrigger*)ECS::GetComponent<Trigger*>(entity);
	temp->scale = -1.f; //can change to variable if needed


	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	float shrinkX = 0.f;
	float shrinkY = 0.f;
	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(posX), float32(posY));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), true, TRIGGER, PLAYER | OBJECTS);
	tempPhsBody.SetColor(vec4(1.f, 0.f, 0.f, 0.3f));
}
void Scene::CreateDestroyTrigger(int targetName, bool invisible, std::string fileName, int spriteX, int spriteY, int vecX, int vecY, int vecZ, int posX, int posY)
{
	//Creates entity
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<Trigger*>(entity);

	//Sets up components
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, spriteX, spriteY);
	if (invisible)
	{
		ECS::GetComponent<Sprite>(entity).SetTransparency(0.f);
	}
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(vecX, vecY, vecZ));
	ECS::GetComponent<Trigger*>(entity) = new DestroyTrigger();
	ECS::GetComponent<Trigger*>(entity)->SetTriggerEntity(entity);
	ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(targetName);

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	float shrinkX = 0.f;
	float shrinkY = 0.f;
	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(posX), float32(posY));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), true, TRIGGER, PLAYER | OBJECTS);
	tempPhsBody.SetColor(vec4(1.f, 0.f, 0.f, 0.3f));
}

void Scene::CreateTranslateTrigger(int targetName, bool invisible, std::string fileName, int spriteX, int spriteY, int vecX, int vecY, int vecZ, int moveX, int moveY, int posX, int posY)
{
	///Creates entity
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<Trigger*>(entity);

	//Sets up components
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, spriteX, spriteY);
	if (invisible)
	{
		ECS::GetComponent<Sprite>(entity).SetTransparency(0.f);
	}

	ECS::GetComponent<Transform>(entity).SetPosition(vec3(vecX, vecY, vecZ));
	ECS::GetComponent<Trigger*>(entity) = new TranslateTrigger();
	ECS::GetComponent<Trigger*>(entity)->SetTriggerEntity(entity);
	ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(targetName);
	TranslateTrigger* temp = (TranslateTrigger*)ECS::GetComponent<Trigger*>(entity);
	temp->movement = b2Vec2(moveX, moveY);


	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	float shrinkX = 0.f;
	float shrinkY = 0.f;
	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(posX), float32(posY));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), true, TRIGGER, PLAYER | OBJECTS);
	tempPhsBody.SetColor(vec4(1.f, 0.f, 0.f, 0.3f));

}
unsigned Scene::CreateEndTrigger(bool invisible, std::string fileName, int spriteX, int spriteY, int vecX, int vecY, int vecZ, int posX, int posY)
{
	///Creates entity
	auto entity = ECS::CreateEntity();

	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<Trigger*>(entity);

	//Sets up components
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, spriteX, spriteY);
	if (invisible)
	{
		ECS::GetComponent<Sprite>(entity).SetTransparency(0.f);
	}

	ECS::GetComponent<Transform>(entity).SetPosition(vec3(vecX, vecY, vecZ));
	ECS::GetComponent<Trigger*>(entity) = new EndTrigger();
	ECS::GetComponent<Trigger*>(entity)->SetTriggerEntity(entity);

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	float shrinkX = 0.f;
	float shrinkY = 0.f;
	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(posX), float32(posY));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), true, TRIGGER, PLAYER | OBJECTS);
	tempPhsBody.SetColor(vec4(1.f, 0.f, 0.f, 0.3f));
	return entity;
}

entt::registry* Scene::GetScene() const
{
	return m_sceneReg;
}

void Scene::SetScene(entt::registry& scene)
{
	m_sceneReg = &scene;
}

vec4 Scene::GetClearColor() const
{
	return m_clearColor;
}

void Scene::SetClearColor(vec4 clearColor)
{
	m_clearColor = clearColor;
}

std::string Scene::GetName() const
{
	return m_name;
}

void Scene::SetName(std::string name)
{
	m_name = name;
}

b2Vec2 Scene::GetGravity() const
{
	return m_gravity;
}

void Scene::SetGravity(b2Vec2 grav)
{
	m_gravity = grav;
}

b2World & Scene::GetPhysicsWorld()
{
	return *m_physicsWorld;
}

void Scene::SetWindowSize(float windowWidth, float windowHeight)
{
	//TODO: Find new way to get the main camera
	auto& tempCam = m_sceneReg->get<Camera>(MainEntities::MainCamera());
	
	tempCam.SetWindowSize(vec2(windowWidth, windowHeight));
	tempCam.Orthographic(float(windowWidth / windowHeight), tempCam.GetOrthoSize().x, tempCam.GetOrthoSize().y,
															tempCam.GetOrthoSize().z, tempCam.GetOrthoSize().w,
															tempCam.GetNear(), tempCam.GetFar());
}
