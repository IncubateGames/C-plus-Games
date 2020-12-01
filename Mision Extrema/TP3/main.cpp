#include "GameEngine.h"
#include "MainScene.h"
#include "IntroScene.h"
#include "MenuScene.h"
#include "MisionScene.h"

using namespace std;

//int const resx=800, resy=600;
int const resx=1024, resy=768;

int main(int argc, char *argv[]) 
{
	GameEngine *engine = new GameEngine(resx,resy,"Extreme Mission",30.0f);	
	engine->PushScene(new IntroScene(engine));			
	//engine->PushScene(new MainScene(engine));	
	engine->Loop();
	delete engine;

	return 0;
}



