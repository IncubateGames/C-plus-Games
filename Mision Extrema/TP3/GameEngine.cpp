#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <stdio.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <climits>
#include <functional>
#include "GameEngine.h"
#include "SpriteBase.h"
#include "EntityManager.h"
#include "DynamicObject.h"

GameEngine::GameEngine(int ancho,int alto,std::string titulo,float fps)
{	
	tiles_x = 10;
	tiles_y = 8;

	fps_dt = 1 / fps;	
	last_diff_dt = 0.0f;
	m_currentScene = NULL;
	m_sceneToDelete = NULL;
	exitEngine = false;
	isPause = false;
	fpsScale = 1.0f;
	j.up=j.down=j.left=j.right=j.a=j.b=0;

	srand(static_cast<unsigned int>(time(nullptr)));
	
	wnd = new RenderWindow(VideoMode(ancho,alto),titulo);	
	wnd->SetFramerateLimit(static_cast<int>(fps*2.0f));

	wnd->SetView(wnd->GetDefaultView());
	
	wnd->Show(true);		
}

void GameEngine::TrackingPlayer(SpriteBase *const player)
{
	const Nivel *const nivel =  Nivel::Instance();
	Tile *tile = nullptr;
	if(player->GlobalIndex >= 0)
	{
		tile = nivel->GetTile(player->GlobalIndex);
		if(tile != nullptr)
		{
			tile->isTrackingPlayer = false;
		}
	}
	sf::Vector2f pos = player->GetPosition();
	player->GlobalIndex = nivel->ConvertPointToGlobalIndex(pos.x,pos.y);

	if(player->GlobalIndex >= 0)
	{
		tile = nivel->GetTile(player->GlobalIndex);
		if(tile != nullptr)
		{
			tile->isTrackingPlayer = true;
		}
	}
};

void GameEngine::TrackingEntity(SpriteBase *const entity)
{
	const Nivel *const nivel =  Nivel::Instance();
	Tile *tile = nullptr;
	
	sf::Vector2f pos = entity->GetPosition();
	entity->GlobalIndex = nivel->ConvertPointToGlobalIndex(pos.x,pos.y);

	if(entity->GlobalIndex >= 0)
	{
		tile = nivel->GetTile(entity->GlobalIndex);
		if(tile != nullptr)
		{
			if(tile->iCountLookupCollisionItem == 0)
			{
				tile->iCountLookupCollisionItem = 1;
				tile->iLookupCollision = entity;
				Nivel::Instance()->CollisionTable.push_back(entity->GlobalIndex);			
			}
			else
			{
				tile->iCountLookupCollisionItem++;
				entity->ptrNextColl_Check = tile->iLookupCollision;
				tile->iLookupCollision = entity;
			}
		}
	}
};

void GameEngine::AgregarDisparo(const float& x,const float& y,const float& vel,bool isNPC)
{
    ManejadorDisparos::Instance()->AgregarDisparo(x, y, vel,isNPC);
	PlayShootFireSound();
};

bool GameEngine::FastColision(const float& x, const float& y,int& index) const
{
	const Nivel *const nivel =  Nivel::Instance();
	int globalindex = nivel->ConvertPointToGlobalIndex(x,y);
	if(globalindex >= 0)
	{
		const Tile *tile = nivel->GetTile(globalindex);
		if(tile != nullptr)
		{
			if(!tile->isDynamic && tile->isDead || !tile->isEnable)
			{
				index = -1;
				return false;
			}
			else if(tile->solid || tile->iCountLookupCollisionItem > 0 || tile->isTrackingPlayer)
			{	
				index = globalindex;
				return true;
			}
		}
	}	
	index = -1;
	return false;
};

bool GameEngine::FastColision(const sf::FloatRect& s, Utils::TCollDirection dir) const
{
	const Nivel *const nivel =  Nivel::Instance();
	float dx = (s.Right + s.Left) * 0.5f;
	float dy = (s.Top + s.Bottom) * 0.5f;

	int index = -1;

	if(dir == Utils::Up)
	{
		index = nivel->ConvertPointToGlobalIndex(dx,s.Top);
	}
	else if(dir == Utils::Down)
	{
		index = nivel->ConvertPointToGlobalIndex( dx,s.Bottom);
	}
	else if(dir == Utils::Left)
	{
		index = nivel->ConvertPointToGlobalIndex(s.Left,dy);
	}
	else if(dir == Utils::Right)
	{
		index = nivel->ConvertPointToGlobalIndex(s.Right,dy);
	}

	index = nivel->GetDirPoint(index,dir);	
	
	if(index >= 0)
	{
		const Tile *const tile = nivel->GetTile(index);
		if(tile != nullptr && tile->isEnable)
		{
			return !tile->solid;
		}

		return false;
	}
	else
	{
		return true;
	}
}

bool GameEngine::HayColision(const sf::FloatRect& s,const sf::FloatRect& r,Utils::CollItem (&collisionMatrix)[4]) const
{ 	
	const Nivel *const nivel =  Nivel::Instance();
	
	bool result = false;
	
	const Tile *tile = nullptr;

	int index = nivel->ConvertPointToGlobalIndex(r.Right,r.Top);

	if(index >= 0)
	{
		tile = nivel->GetTile(index);
	
		if(tile != nullptr)
		{
			result |= collisionMatrix[Utils::TR].LoadValues(s,r,tile,Utils::TR);

			tile = nivel->GetTile(nivel->ConvertPointToGlobalIndex(r.Left,r.Top));
			if(tile != nullptr)
			{
				result |= collisionMatrix[Utils::TL].LoadValues(s,r,tile,Utils::TL);
			}
			
			tile = nivel->GetTile(nivel->ConvertPointToGlobalIndex(r.Left,r.Bottom));
			if(tile != nullptr)
			{
				result |= collisionMatrix[Utils::BL].LoadValues(s,r,tile,Utils::BL);
			}
	
			tile = nivel->GetTile(nivel->ConvertPointToGlobalIndex(r.Right,r.Bottom));
			if(tile != nullptr)
			{
				result |= collisionMatrix[Utils::BR].LoadValues(s,r,tile,Utils::BR);
			}
		}		
	}
	else
	{
		result = true;
	}
	return result;
}

void GameEngine::ResolverColision(Utils::CollItem (&collisionMatrix)[4])
{
	for(int i=0;i<4;i++)
	{	
		if(collisionMatrix[i].colltype == Utils::NA)
		{
			continue;
		}
		//tipo = 1 -> Key
		//tipo = 2 -> Key Bomb
		//tipo = 3 -> Bomb
		if(collisionMatrix[i].iKey != -1)
		{
			PlayCatchItemSound();
			//Activar llave
			Nivel::Instance()->irKeys[collisionMatrix[i].iKey]  = 1;			
			//deshabilitar el objeto
			const Tile* tile = Nivel::Instance()->GetTile(collisionMatrix[i].globalIndex);		
			if(tile != nullptr)
			{
				EntityManager::Instance()->Disable(static_cast<DynamicObject*>(tile->ptrDynObj));
				tile->SetEnable(false);
			}
			break;
		}
		else if(collisionMatrix[i].isKeyBomb)
		{			
			//Verificar estan activas todas las llaves.
			//Si es asi, el juego termina y el jugador gana.
			Nivel::Instance()->CheckGameFinish();			
			PlayCatchItemSound();
			break;
		}
		else if(collisionMatrix[i].isBomb)
		{
			const Tile* tile = Nivel::Instance()->GetTile(collisionMatrix[i].globalIndex);		
			if(tile != nullptr)
			{
				//deshabilitar el objeto
				EntityManager::Instance()->Disable(static_cast<DynamicObject*>(tile->ptrDynObj));			 
				ParticleSystemManager::Instance()->CreateEmiterOneExplosion(tile->rect.Left + tile->rect.GetWidth()/2, tile->rect.Top + tile->rect.GetHeight()/2, Color::White);
				//Restar Vidas del player
				EntityManager::Instance()->GetPlayer().RecibirImpacto();
			}
			break;

		}
		else if(collisionMatrix[i].iPortal != -1)
		{	
			//Pasar de nivel
			PlayPortalSound();
			Nivel::Instance()->SetNextLevel(collisionMatrix[i].iPortal);	
			cout<<"Next Level: "<< Nivel::Instance()->fileNextLevel <<"\n";
			break;
		}
	}
}

FloatRect GameEngine::GetLevelViewRect()
{
	return FloatRect(); //nivel->levelView.GetRect();
}

void GameEngine::Init()
{
	m_currentScene = nullptr;
	snd_shoot_buffer.LoadFromFile("../data/sound/shoot.ogg");
	snd_explosion_buffer.LoadFromFile("../data/sound/explosion.ogg");
	snd_ImpactEntity_buffer.LoadFromFile("../data/sound/shootImpact.ogg");
	snd_ImpactSolid_buffer.LoadFromFile("../data/sound/ShootSolid.ogg");
	snd_catchItem_buffer.LoadFromFile("../data/sound/catchItem.ogg");
	snd_portal_buffer.LoadFromFile("../data/sound/teleport.ogg");
}

void GameEngine::StopMusic()
{
	gameMusic.Stop();
}

void GameEngine::PlayShootFireSound()
{	
	snd_manager_shoot.Stop();
	snd_manager_shoot.SetBuffer(snd_shoot_buffer);
	snd_manager_shoot.Play();
}

void GameEngine::PlayExplosionSound()
{	
	snd_manager_explosion.Stop();
	snd_manager_explosion.SetBuffer(snd_explosion_buffer);
	snd_manager_explosion.Play();
}

void GameEngine::PlayCatchItemSound()
{
	snd_manager_shoot.Stop();
	snd_manager_shoot.SetBuffer(snd_catchItem_buffer);
	snd_manager_shoot.Play();
}

void GameEngine::PlayShootImpactSound()
{
	snd_manager_explosion.Stop();
	snd_manager_explosion.SetBuffer(snd_ImpactEntity_buffer);
	snd_manager_explosion.Play();
}

void GameEngine::PlayShootImpactSolidSound()
{
	snd_manager_explosion.Stop();
	snd_manager_explosion.SetBuffer(snd_ImpactSolid_buffer);
	snd_manager_explosion.Play();
}

void GameEngine::PlayPortalSound()
{
	snd_manager_explosion.Stop();
	snd_manager_explosion.SetBuffer(snd_portal_buffer);
	snd_manager_explosion.Play();
}

void GameEngine::PlayMenuMusic()
{
	gameMusic.Stop();
	gameMusic.OpenFromFile("../data/sound/MenuMusic.ogg");	
	gameMusic.SetLoop(true);
	gameMusic.Play();
}

void GameEngine::PlayGameMusic()
{
	gameMusic.Stop();
	gameMusic.OpenFromFile("../data/sound/MenuMusic.ogg");
	gameMusic.SetLoop(true);
	gameMusic.Play();
}

void GameEngine::PlayWonMusic()
{
	gameMusic.Stop();
	gameMusic.OpenFromFile("../data/sound/GameWonMusic.ogg");
	gameMusic.SetLoop(true);
	gameMusic.Play();
}

void GameEngine::PlayLostMusic()
{
	gameMusic.Stop();
	gameMusic.OpenFromFile("../data/sound/GameLostMusic.ogg");
	gameMusic.SetLoop(true);
	gameMusic.Play();
}

GameEngine::~GameEngine(void)
{
	
}

void GameEngine::PushScene(SceneBase *scene)
{
	m_sceneQueue.push_back(scene);
}

void GameEngine::Loop()
{	
	Init();

	float dt = 0.0f;
	
	while(wnd->IsOpened() && !exitEngine)
	{
		if (m_currentScene == nullptr)
		{
			if(m_sceneQueue.size() > 0)
			{
				m_currentScene = m_sceneQueue.front();
				m_sceneQueue.pop_front();
				wnd->SetView(wnd->GetDefaultView());
				m_currentScene->Init();
			}
			else
			{
				m_currentScene = nullptr;	
				exitEngine = true;
			}
		}
		else if(m_currentScene->IsFinished())
		{
			m_currentScene->Cleanup();			
			delete m_currentScene;
			m_currentScene = nullptr;
		}
		else if(m_currentScene->IsExit())
		{
			if(m_currentScene)
			{
				m_currentScene->Cleanup();				
				delete m_currentScene;
				m_currentScene = nullptr;
			}

			while(m_sceneQueue.size() > 0)
			{
				m_currentScene = m_sceneQueue.front();
				m_sceneQueue.pop_front();
				m_currentScene->Cleanup();				
				delete m_currentScene;
				m_currentScene = nullptr;
			}
		}
		else
		{	
			dt = clk.GetElapsedTime();
			
			clk.Reset();

			if(dt > fps_dt)
			{
				cout<<"Adjust frame --> " << fps_dt << ":" << dt << "\n";				
				dt = fps_dt;		
			}

			dt = dt * fpsScale;

			if(!isPause)
			{	
				DoEvents();				
				UpdateEvents(dt);
			}
						
			DrawGame();
		}		
	}

	if(m_currentScene != nullptr)
	{
		m_currentScene->Cleanup();		
		delete m_currentScene;
		m_currentScene = nullptr;
	}

	while(m_sceneQueue.size() > 0)
	{
		m_currentScene = m_sceneQueue.front();
		m_sceneQueue.pop_front();
		m_currentScene->Cleanup();		
		delete m_currentScene;
		m_currentScene = nullptr;
	}

	wnd->Close();
}

void GameEngine::DoEvents(){
	Event evt;
	while(wnd->GetEvent(evt)){	
		if(evt.Type == evt.Closed)
		{				
			exitEngine = true;
		}
		else
		{
			JoystickUpdate(evt);
			UpdatePoolEvents(evt);
		}		
	}
	
}

void GameEngine::DrawGame()
{
	wnd->Clear();
	m_currentScene->Render(*wnd);
	wnd->Display();
}

void GameEngine::UpdatePoolEvents(const Event &evt)
{
	m_currentScene->ProcessEvent(evt);
}

void GameEngine::UpdateEvents(const float &dt)
{
	m_currentScene->Update(dt);		
}

void GameEngine::JoystickUpdate(const Event &evt)
{
	if (evt.Type == sf::Event::KeyPressed){
		switch(evt.Key.Code){
			case sf::Key::Up:		j.up=true; break; 
			case sf::Key::Down: 	j.down=true; break; 
			case sf::Key::Left: 	j.left=true; break; 
			case sf::Key::Right: 	j.right=true; break; 
			case sf::Key::A: 		j.a=true; break; 
			case sf::Key::S: 		j.b=true; break;
			/*
			case sf::Key::P:		
				if(!isContinue)
				{
					isPause = !isPause; 					
					//cronometro->Reset();
				}
				break;			
			case sf::Key::Return:	
				if(isPause)
				{
					toReload = true;
				}
				break; 
			case sf::Key::Escape: 
				isGameFinish = true; 
				isExit = true; 
				break;
			case sf::Key::Q:    						
				isSlowTime = !isSlowTime;						
				if(isSlowTime)
				{
					fpsScale = 0.5f;							
					slowTimeCount = 0;
				}
				else
				{
					fpsScale = 1.0f;
				}
				break;*/
		}
	}
			
	if (evt.Type == sf::Event::KeyReleased){
		switch(evt.Key.Code){
			case sf::Key::Up:		j.up=false; break; 
			case sf::Key::Down: 	j.down=false; break; 
			case sf::Key::Left: 	j.left=false; break; 
			case sf::Key::Right: 	j.right=false; break; 
			case sf::Key::A: 		j.a=false; break; 
			case sf::Key::S: 		j.b=false; break;
		}
	}	
}

void GameEngine::InitLevelView(int plevelSize_x, int plevelSize_y,int ptile_size_x,int ptile_size_y){
	
	tile_size_x = ptile_size_x;
	tile_size_y = ptile_size_y;
	
	levelSize_x = plevelSize_x;
	levelSize_y = plevelSize_y;

	int res_x = wnd->GetWidth();
	int res_y = wnd->GetHeight();
	// si no nos especifican el tamano en tiles de la vista
	// buscamos una vista que abarque todo el nivel
	if(tiles_x<0) tiles_x=levelSize_x;
	if(tiles_y<0) tiles_y=levelSize_y;
		
	// la logica consiste en hacer coincidir la menor magnitud del Nivel
	// (ancho o alto) con la menor magnitud de la ventana
	
	// si la ventana es mas ancha que alta
	if(res_x>res_y){
		// buscamos la menor magnitud del nivel y ese tamano se
		// correspondera con el alto (menor tamano)
		// la magnitud restante la calculamos con regla de 3
		if(tiles_x>tiles_y){
			// el alto del nivel abarcara el alto de la ventana
			realtiles_y=tiles_y;
			realtiles_x=(res_x*tiles_y)/res_y;
		}else{
			// el ancho del nivel abarca el ancho de la ventana
			realtiles_x=tiles_x;
			realtiles_y=(res_y*tiles_x)/res_x;
		}
	}else{
		// si la ventana es mas alta que ancha
		// hacemos lo mismo que antes, el menor lado de la ventana
		// le asignamos el menor lado del nivel
		if(tiles_x>tiles_y){
			// el alto del nivel abarcara el alto de la ventana 
			realtiles_y=tiles_y;
			realtiles_x=(res_y*tiles_x)/res_x;
		}else{
			// el ancho del nivel abarca el ancho de la ventana
			realtiles_x=tiles_x;
			realtiles_y=(res_y*tiles_x)/res_x;
		}
	}

	// seteamos la vista del nivel y hacemos un scroll para inicializarla/acomodarla
	levelView.SetHalfSize(sf::Vector2f(realtiles_x*tile_size_x*0.5f, realtiles_y*tile_size_y*0.5f));
	SetViewCenter(sf::Vector2f(0,0));
	
	wnd->SetView(levelView);
}

void GameEngine::SetViewCenterSmooth(sf::Vector2f newCenter, float factor, float dt){
	// conseguimos el centro actual de la vista
	sf::Vector2f oldCenter=levelView.GetCenter();
	
	// calculamos el desplazamiento como un proporcional a la diferencia
	// entre el viejo centro y el nuevo (y factor y el paso de tiempo)
	float scrollDeltax, scrollDeltay;
	scrollDeltax=(newCenter.x-oldCenter.x)*(factor*dt);
	scrollDeltay=(newCenter.y-oldCenter.y)*(factor*dt);
	
	// aplicamos el desplazamiento
	newCenter.x=oldCenter.x+scrollDeltax;
	newCenter.y=oldCenter.y+scrollDeltay;
	
	// seteamos el nuevo centro corrigiendolo si se sale fuera del nivel
	SetViewCenter(newCenter);
}

void GameEngine::SetViewCenter(sf::Vector2f newCenter){
	// el tamano de la mitad de la vista
	sf::Vector2f halfSize=levelView.GetHalfSize();
	static sf::Vector2f oldCenter;
	
	// el tamano del nivel pero en coordenadas
	sf::Vector2f levelSizeCoords;
	levelSizeCoords.x=levelSize_x*tile_size_x*1.0f;
	levelSizeCoords.y=levelSize_y*tile_size_y*1.0f;
	
	// si el nuevo centro se sale fuera del nivel, lo corregimos
	if(newCenter.x-halfSize.x<0) 
		newCenter.x=halfSize.x;
	if(newCenter.x+halfSize.x>levelSizeCoords.x) 
		newCenter.x=levelSizeCoords.x-halfSize.x; 
	if(newCenter.y-halfSize.y<0) 
		newCenter.y=halfSize.y;
	if(newCenter.y+halfSize.y > levelSizeCoords.y) 
		newCenter.y=levelSizeCoords.y-halfSize.y;
	
	// actualiza las capas de parallax scrolling
	Nivel::Instance()->UpdateParallax(oldCenter,newCenter,levelView);

	oldCenter=newCenter;
	
	// seteamos el nuevo centro de la vista
	levelView.SetCenter(newCenter);	
}