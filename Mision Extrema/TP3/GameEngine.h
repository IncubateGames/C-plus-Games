#ifndef _GameEngine_
#define _GameEngine_

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <stdio.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <climits> 
#include <deque>
#include "SpriteSheetManager.h"
#include "Animation.h"
#include "ParticleSystemManager.h"
#include "Disparos.h"
#include "Nivel.h"
#include "Cronometro.h"
#include "Joystick.h"
#include "Utils.h"
#include "SpriteBase.h"

class SceneBase;
class ManejadorDisparos;
class Nivel;
class SpriteBase;

using namespace std;
using namespace sf;
using namespace Utils;

class GameEngine
{
private:
	View m_currentView;
	std::deque<SceneBase *> m_sceneQueue;
	SceneBase *m_currentScene;
	SceneBase *m_sceneToDelete;	
	bool exitEngine;
	//bool isPause;	
	float fps_dt;
	float last_diff_dt;

	int tiles_x;
	int tiles_y;
	int tile_size_x;
	int tile_size_y;
	int realtiles_y;
	int realtiles_x;
	int levelSize_x;
	int levelSize_y;
	
	sf::Clock clk;
	sf::Clock clkPerf;

	sf::Music gameMusic;
	
	sf::SoundBuffer snd_catchItem_buffer;
	sf::SoundBuffer snd_shoot_buffer;
	sf::SoundBuffer snd_ImpactEntity_buffer;
	sf::SoundBuffer snd_explosion_buffer;
	sf::SoundBuffer snd_ImpactSolid_buffer;
	sf::SoundBuffer snd_portal_buffer;
	sf::Sound snd_manager_shoot;
	sf::Sound snd_manager_explosion;

	void DrawGame();
	void UpdatePoolEvents(const Event& evt); 
	void UpdateEvents(const float& dt);
	void DoEvents();

	void JoystickUpdate(const Event& evt);
	void Init();

	void SetViewCenter(sf::Vector2f newCenter);
	
public:
	bool isPause;
	Joystick j;
	float fpsScale;
	RenderWindow *wnd;
	View levelView;		
	Cronometro *cronometro;
	
	void PlayMenuMusic();
	void PlayGameMusic();
	void PlayWonMusic();
	void PlayLostMusic();
	void StopMusic();

	void PlayCatchItemSound();
	void PlayShootFireSound();
	void PlayShootImpactSound();
	void PlayExplosionSound();
	void PlayPortalSound();
	void PlayShootImpactSolidSound();

	void InitLevelView(int plevelSize_x, int plevelSize_y,int tile_size_x,int tile_size_y);
	bool HayColision(const sf::FloatRect& s,const sf::FloatRect& r,Utils::CollItem (&collisionMatrix)[4]) const;	
	bool FastColision(const sf::FloatRect& s, Utils::TCollDirection dir) const;
	bool FastColision(const float& x, const float& y,int& index) const;
	void ResolverColision(Utils::CollItem (&collisionMatrix)[4]);
	void TrackingPlayer(SpriteBase *const player);
	void TrackingEntity(SpriteBase *const entity);
	void AgregarDisparo(const float& x,const float& y,const float& vel,bool isNPC);
	void SetViewCenterSmooth(sf::Vector2f newCenter, float factor, float dt);
	FloatRect GetLevelViewRect();
	GameEngine(int ancho,int alto,std::string titulo,float fps);
	~GameEngine(void);
	void Loop();	
	void PushScene(SceneBase *scene);			
};

#endif


