#ifndef DISPAROS_H
#define DISPAROS_H

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <list>
#include <vector>
#include "GameEngine.h"
#include "Nivel.h"
#include "ParticleSystemManager.h"
#include "TextureManager.h"
#include "SceneBase.h"

using namespace std;

class GameEngine;


class Disparo: public sf::Sprite{
	private:
	
	static bool init; 				// para saber si ya cargamos o no la imagen
	float velx;						// la velocidad del disparo
	bool isNPC;

	public:
	float maxLifeTime;
	Disparo(float x0, float y0, float vel,bool isNPC);	// constructor
	~Disparo(void);
	friend class ManejadorDisparos;	// para que ManejadorDisparos pueda acceder al miembro velx
};

// la clase se encarga de manejar todos los disparos
class ManejadorDisparos{
private:
	vector<Disparo *> disparos;								// guardamos una lista de disparos	
	GameEngine *engine;	
	static ManejadorDisparos* _instance;
	
	ManejadorDisparos(GameEngine *engine){
		this->engine = engine;
	};
	ManejadorDisparos(){;};
	ManejadorDisparos(string level_file);
	ManejadorDisparos(ManejadorDisparos const& other){;};
	ManejadorDisparos& operator=(ManejadorDisparos const& other){;};

public:
	
	~ManejadorDisparos();	
	void MoverDisparos(float dt);			// mueve los disparos y revisa que esten dentro de la pantalla
	void AgregarDisparo(float x, float y, float vel,bool isNPC);	// agrega un nuevo disparo
	void DibujarDisparos(sf::RenderWindow& w);				// Dibuja todos los disparos en la ventana w
	void Clear();
	static ManejadorDisparos* Instance()
	{
		return _instance;
	};

	void Init(GameEngine *engine)
	{
		if(_instance == nullptr)
		{
			_instance = new ManejadorDisparos(engine);			
		};		
	};

	void Release()
	{
		if(_instance != nullptr)
		{
			delete _instance;
			_instance = nullptr;
		}
	};
};


#endif

