#pragma once
#include "scenebase.h"
#include "EntityManager.h"
#include "Personaje.h"
#include "Cronometro.h"
#include "Disparos.h"
#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>

class MainScene : public SceneBase
{
private:
	sf::Font font;
	sf::String text;
	Sprite keysHUD;
	Sprite vidasHUD;
	sf::Clock clkPerf;
	int irKeys[5];
	float FinishCount;
		
	Personaje *prince;
			
	Cronometro *cronometro;	
	string lastLevelFile;
	
	
	bool isSlowTime;
	bool isGameWon;
	bool isPause;
	bool isEndGame;
	bool isContinue;
	bool isReload;
	bool isQuit;
	float slowTime;
	float slowTimeCount;
	void Pause(bool enable=true);
	void GameFinish(bool isWon);
	
	void IntLevel();
	void ShowHUD(RenderWindow& w);
	void ActualizarContador();
	void SlowTime(const float& dt,bool restart=false,bool force=false);
	

public:	
	void AgregarEnemigo(const float& x,const float& y,const int& tipo);	
	void AgregarDynamicObject(const Tile* tile);	
	MainScene(GameEngine* engine);
	virtual ~MainScene(void);
	virtual void Init();
	virtual void ProcessEvent(const sf::Event& e);
	virtual void Update(const float& dt);
	virtual void Render(sf::RenderWindow& w);
	virtual void Cleanup();
};

