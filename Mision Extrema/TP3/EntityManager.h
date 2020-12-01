#ifndef _EntityManager_
#define _EntityManager_

#include <SFML/Graphics/RenderWindow.hpp>
#include <list>
#include <vector>
#include "Personaje.h"
#include "Enemigo.h"
#include "DynamicObject.h"

class Enemigo;
class Personaje;

class EntityManager
{
private:
	vector<Enemigo *> enemigos;
	
	vector<DynamicObject *> dynObj;

	Personaje *target;	
	GameEngine *engine;
	static EntityManager* _instance;
	EntityManager(GameEngine* e){
		this->engine = engine;
	};
	EntityManager(EntityManager const& other){;};
	EntityManager& operator=(EntityManager const& other){;};
public:	
	void SetEnvironment(ManejadorDisparos* d,Nivel* n);
	void AiTracker(Personaje* p);		
	void Mover(Joystick& j,const float& dt);		
	void Agregar(Enemigo* entity);
	void Agregar(DynamicObject* entity);
	void Disable(DynamicObject* entity);
	void Dibujar(sf::RenderWindow& w);	
	void DibujarDynamic(sf::RenderWindow& w);	
	
	void Clear();

	Personaje& GetPlayer() const
	{
		return *target;
	};
	
	~EntityManager(void);

	static EntityManager* Instance()
	{
		return _instance;
	};

	void Init(GameEngine* engine)
	{
		if(_instance == nullptr)
		{
			_instance = new EntityManager(engine);			
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