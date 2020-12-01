#include "Disparos.h"
#include "EntityManager.h"

bool Disparo::init=false;
ManejadorDisparos* ManejadorDisparos::_instance = nullptr;

Disparo::Disparo(float x0, float y0, float vel,bool isNPC){	
	// inicializamos posicion, vel e imagen del disparo
	const sf::Image &imgDisparo = TextureManager::GetInstance().GetTexture("../data/images/disparo.png");
	SetImage(imgDisparo);
	SetScale(0.9f,0.9f);
	SetPosition(x0, y0);	
	velx=vel;
	this->isNPC = isNPC;
	maxLifeTime = 1.5f;
};

Disparo::~Disparo(void)
{

}

ManejadorDisparos::~ManejadorDisparos()
{	
	Clear();
}

void ManejadorDisparos::Clear()
{
	vector<Disparo *>::iterator p=disparos.begin();
	while(p!=disparos.end()){
		delete *p;
		p=disparos.erase(p);		
	}
};

// recorre la lista de disparos, si el disparo se encuentra fuera de la region
// dada por r, el disparo se elimina de la lista, sino se mueve
void ManejadorDisparos::MoverDisparos(float dt){	
	float misilx;
	float misily;
	bool isNPC;
	int tipo  = 0;
	Color color = Color::White;
	
	Disparo *disparo = nullptr;
	const Tile *tile = nullptr;
	int indexrand;
	bool isRealImpact = false;
	bool isSolidImpact = false;
	vector<Disparo *>::iterator it = disparos.begin();
	while(it != disparos.end())
	{		
		isRealImpact = false;
		isSolidImpact = false;
		disparo = (*it);
		
		disparo->Move(disparo->velx*dt, 0);
		
		disparo->maxLifeTime -= dt;
		misilx = disparo->GetPosition().x;
		misily = disparo->GetPosition().y;
		isNPC = disparo->isNPC;

		int index;

		if(disparo->maxLifeTime < 0)
		{
			delete disparo;
			disparo = nullptr;
			it = disparos.erase(it);
		}
		else
		{
			if(engine->FastColision(misilx,misily,index))
			{	
				if(index >= 0)
				{
					tile = Nivel::Instance()->GetTile(index);
					if(tile != nullptr && !tile->solid)
					{	
						if(isNPC)
						{
							//verifica contra el player
							if(tile->isTrackingPlayer)
							{
								isRealImpact = EntityManager::Instance()->GetPlayer().RecibirImpacto(misilx,misily);									
								color = EntityManager::Instance()->GetPlayer().GetColor();
							}
						}
						else
						{
							//verificar si hay una colision contra los enemigos.
							if(tile->iCountLookupCollisionItem > 0)
							{	
								indexrand = std::rand() % tile->iCountLookupCollisionItem;
								//-->
								SpriteBase *enemigo = static_cast<SpriteBase*>(tile->iLookupCollision);
								if(indexrand > 0)
								{
									void *indePtr = enemigo->ptrNextColl_Check;
									for(int i=0;i < indexrand; i++)
									{
										enemigo = static_cast<SpriteBase*>(indePtr);
									}
								}
								isRealImpact = enemigo->RecibirImpacto(misilx,misily);	
								color = enemigo->GetColor();
							}
						}
					}
					else
					{
						isRealImpact = true;
						isSolidImpact = true;
					}				
				}
			}	
			
			if(isRealImpact)
			{						
				if(isSolidImpact)
				{
					ParticleSystemManager::Instance()->CreateEmiterOneShoot(misilx,misily);
				}
				else
				{
					ParticleSystemManager::Instance()->CreateEmiterOneExplosion(misilx,misily,color);
				}
				delete disparo;
				disparo = nullptr;
				it = disparos.erase(it);						
			}
			else
			{
				it++;
			}
		}
	}
}

// agrega un nuevo disparo a la lista con la posicion y velocidad dadas
void ManejadorDisparos::AgregarDisparo(float x, float y, float vel,bool isNPC)
{
	disparos.push_back(new Disparo(x, y, vel,isNPC));
}

// recorre la lista de disparos y los dibuja
void ManejadorDisparos::DibujarDisparos(sf::RenderWindow &w){	
	vector<Disparo *>::iterator p=disparos.begin();
	while(p!=disparos.end()){		
		Disparo *d = (*p);
		w.Draw(*d);
		p++;
	}
}
