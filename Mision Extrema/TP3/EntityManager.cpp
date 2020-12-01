#include "EntityManager.h"

EntityManager* EntityManager::_instance = nullptr;

EntityManager::~EntityManager(void)
{
	Clear();
}

void EntityManager::Clear()
{
	{
		vector<Enemigo *>::iterator e = enemigos.begin();
		while(e != enemigos.end())
		{
			delete *e;
			e = enemigos.erase(e);		
		}
	}
	{
		vector<DynamicObject *>::iterator e = dynObj.begin();
		while(e != dynObj.end())
		{
			delete *e;
			e = dynObj.erase(e);		
		}
	}
};

void EntityManager::AiTracker(Personaje *p)
{
	target = p;
}

void EntityManager::Mover(Joystick &j,const float& dt)
{
	{
		vector<Enemigo *>::iterator e=enemigos.begin();
		while(e!=enemigos.end())
		{
			if((*e)->isDead)
			{
				delete *e;
				e = enemigos.erase(e);
			}
			else
			{
				(*e)->Mover_y_Animar(j,dt);
				engine->TrackingEntity((*e));
				e++;
			}
		}
	}	
}

void EntityManager::Agregar(Enemigo *entity)
{
	int index = -1;
	if(enemigos.empty())
	{
		index = 0;
	}
	else 
	{
		vector<Enemigo *>::iterator it = enemigos.end();
		it--;
		index = (*it)->index + 1;
	}
	entity->Inicializar(index);		
	entity->AiTracker(target);
	enemigos.push_back(entity);
}

void EntityManager::Agregar(DynamicObject *entity)
{
	int index = -1;
	if(dynObj.empty())
	{
		index = 0;
	}
	else 
	{
		vector<DynamicObject *>::iterator it = dynObj.end();
		it--;
		index = (*it)->GetIndex() + 1;
	}

	entity->Inicializar(index);
	dynObj.push_back(entity);
};

void EntityManager::Disable(DynamicObject* entity)
{
	entity->Disable();
}

void EntityManager::Dibujar(sf::RenderWindow &w)
{	
	vector<Enemigo *>::iterator e= enemigos.begin();
	while(e!=enemigos.end())
	{
		(*e)->Draw(w);	
		e++;
	}	
}

void EntityManager::DibujarDynamic(sf::RenderWindow &w)
{
	int index = -1;
	vector<DynamicObject *>::iterator e= dynObj.begin();
	while(e!=dynObj.end())
	{
		if((*e)->Enable())
		{
			index = (*e)->GetImageIndex();
			if(index != -1)
			{
				Nivel::Instance()->RenderTile(index,(*e)->GetRect(),w);						
			}
		}
		e++;
	}
}



