#ifndef _SceneBase_
#define _SceneBase_

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "TextureManager.h"
#include "GameEngine.h"

using namespace std;
using namespace sf;

class GameEngine;

class SceneBase
{
private:	
protected:
	bool isFinished;
	bool isExit;	
	bool isDebug;
	GameEngine *engine;

public:
	virtual bool HayColision(float x, float y,sf::Color& color,bool isNPC);
	virtual void AgregarEnemigo(float x, float y,int tipo);
	void IsDebug(bool debug);
	bool IsFinished();
	bool IsExit();
	SceneBase(void);
	virtual ~SceneBase(void);
	virtual void Init() = 0;
	virtual void ProcessEvent(const sf::Event& e) = 0;
	virtual void Update(const float& dt) = 0;
	virtual void Render(sf::RenderWindow& w) = 0;
	virtual void Cleanup() = 0;	
};

#endif
