#ifndef __MAINENTITIES_H__
#define __MAINENTITIES_H__

class MainEntities
{
public:
	//Getters
	static int MainCamera();
	static int MainPlayer();
	static int MainPlatform1();
	static int MainPlatform2();

	//Setters
	static void MainCamera(int main);
	static void MainPlayer(int main);
	static void MainPlatform1(int main);
	static void MainPlatform2(int main);

private:
	static int m_mainCamera;
	static int m_mainPlayer;
	static int m_mainPlatform1;
	static int m_mainPlatform2;
};

#endif // !__MAINENTITIES_H__
