#include "MainEntities.h"

int MainEntities::m_mainCamera = 0;
int MainEntities::m_mainPlayer = 0;
int MainEntities::m_mainPlatform1 = 0;
int MainEntities::m_mainPlatform2 = 0;

int MainEntities::MainCamera()
{
	return m_mainCamera;
}

int MainEntities::MainPlayer()
{
	return m_mainPlayer;
}

int MainEntities::MainPlatform1()
{
	return m_mainPlatform1;
}

int MainEntities::MainPlatform2()
{
	return m_mainPlatform2;
}

void MainEntities::MainCamera(int main)
{
	m_mainCamera = main;
}

void MainEntities::MainPlayer(int main)
{
	m_mainPlayer = main;
}

void MainEntities::MainPlatform1(int main)
{
	m_mainPlatform1 = main;
}

void MainEntities::MainPlatform2(int main)
{
	m_mainPlatform2 = main;
}