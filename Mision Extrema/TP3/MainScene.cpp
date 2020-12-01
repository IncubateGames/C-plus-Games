#include "MainScene.h"
#include "JuegoGanadoScene.h"
#include "JuegoPerdidoScene.h"

using namespace sf;

MainScene::MainScene(GameEngine *engine)
{
	cronometro = nullptr;
	prince = nullptr;	
	this->engine = engine;
}

MainScene::~MainScene(void)
{	
	if(cronometro != nullptr)
	{
		delete cronometro;
		cronometro = nullptr;
	}	

	if(prince != nullptr)
	{
		delete prince;
		prince = nullptr;
	}

	Nivel::Instance()->Release();
	ManejadorDisparos::Instance()->Release();
	ParticleSystemManager::Instance()->Release();
	EntityManager::Instance()->Release();
}

void MainScene::Init()
{
	//IsDebug(true);
	isQuit = false;
	isReload = false;
	isGameWon = false;
	isSlowTime = false;
	isPause = false;
	isContinue = false;
	isEndGame = false;
	slowTime = 3.0f;
	slowTimeCount = 0;
	FinishCount = 3.0f;			

	font.LoadFromFile("../data/fonts/arialbd.ttf");
	text.SetFont(font);
	text.SetColor(Color::White);
	text.SetSize(10);

	keysHUD.SetImage(TextureManager::GetInstance().GetTexture("../data/images/global_level.png"));
	keysHUD.SetScale(0.7f,0.7f);
	keysHUD.SetBlendMode(sf::Blend::Alpha);
	keysHUD.SetColor(Color::Color(255,255,255,150));
	IntRect recKeys(32,128,64,160);
	
	keysHUD.SetSubRect(recKeys);
	keysHUD.SetCenter(0,0);
		
	vidasHUD.SetImage(TextureManager::GetInstance().GetTexture("../data/images/personaje.png"));
	vidasHUD.SetScale(0.4f,0.4f);
	vidasHUD.SetBlendMode(sf::Blend::Alpha);
	vidasHUD.SetColor(Color::Color(255,255,255,150));
	IntRect recVidas(145,285,145 + 22,285 + 57);
	
	vidasHUD.SetSubRect(recVidas);
	vidasHUD.SetCenter(0,0);
	
	cronometro = new Cronometro(600,font);	
	cronometro->Init();	
	
	EntityManager::Instance()->Init(engine);	
	
	prince = new Personaje(engine);	
	prince->Inicializar();	
	
	EntityManager::Instance()->AiTracker(prince);

	//lastLevelFile = "../data/levels/level5.lev";// mainlevel-level1.lev";
	lastLevelFile = "../data/levels/mainlevel-level1.lev";
	
	ManejadorDisparos::Instance()->Init(engine);
	ParticleSystemManager::Instance()->Init();	
	ParticleSystemManager::Instance()->AddAffector(new Gravity(0,1000));
	ParticleSystemManager::Instance()->AddAffector(new Fade(0.5));

	//engine->PlayGameMusic();

	IntLevel();	
}


void MainScene::AgregarEnemigo(const float& x,const float& y,const int& tipo)
{
	Enemigo *e = new Enemigo(tipo,engine);		
	EntityManager::Instance()->Agregar(e);
	e->SetPosition(x,y);
}

void MainScene::GameFinish(bool isWon)
{
	if(isWon)
	{		
		engine->PushScene(new JuegoGanadoScene(engine));
	}
	else
	{
		engine->PushScene(new JuegoPerdidoScene(engine));
	}
}

void MainScene::ProcessEvent(const sf::Event& e)
{	
	//// actualizamos el estado del joystick segun los eventos
	if (!isEndGame && e.Type == sf::Event::KeyPressed){
		if (isQuit)
		{
			switch(e.Key.Code)
			{			
				case sf::Key::S:						
					isExit = true;
					break;
				case sf::Key::N:	
					isQuit = false;
					Pause(false);
					break; 				
			}
		}
		else if(isPause)
		{
			switch(e.Key.Code)
			{			
				case sf::Key::P:						
					if(isPause)
					{
						Pause(false);
					}
					else
					{
						Pause();					
					}
				break;
			}
		}
		else if(!isContinue)
		{
			switch(e.Key.Code)
			{	
				case sf::Key::Return:					
					if(!isContinue)
					{						
						isReload = true;
					}
					break; 
				case sf::Key::Escape: 
					isQuit = true;					
					Pause();
					break;				
			}
		}
		else
		{
			switch(e.Key.Code)
			{			
				case sf::Key::P:						
					if(isPause)
					{
						Pause(false);
					}
					else
					{
						Pause();					
					}
					break;
				case sf::Key::Return:					
					if(!isContinue)
					{						
						isReload = true;
					}
					break; 
				case sf::Key::Escape: 
					isQuit = true;					
					Pause();
					break;
				case sf::Key::Q:    			
					SlowTime(0,true);				
					break;
			}
		}
		
	}	
}

void MainScene::Pause(bool enable)
{
	if(enable)
	{
		isPause = true;										
		prince->pause = true;										 				
		isContinue = false;
	}
	else
	{
		isPause = false;										
		prince->pause = false;					
		cronometro->Reset();
		isContinue = true;
	}
}

void MainScene::Update(const float& dt)
{	
	if(isReload)
	{
		IntLevel();
	}
	else if(Nivel::Instance()->isNeedNextLoadLevel)
	{	
		lastLevelFile = Nivel::Instance()->fileNextLevel;			
		isReload = true;		
	}
	else
	{
		if(isEndGame)
		{			
			FinishCount -= dt;

			if( FinishCount <= 0)
			{
				if(!isFinished)
				{	
					isFinished = true;
					engine->PlayWonMusic();
				}
			}
		}

		if(!isEndGame)
		{
			if(Nivel::Instance()->GetGameWon())
			{				
				GameFinish(true);
				isGameWon = true;
				isEndGame = true;
				engine->StopMusic();
			}
			else if(prince->isDead)
			{
				if(isContinue)
				{
					SlowTime(0,true,true);
					Pause(false);
					isContinue = false;		
				}		
				if(prince->vidas <= 0)
				{			
					engine->StopMusic();
					GameFinish(false);
					isEndGame = true;
				}
			}
			else if(isContinue)
			{
				ActualizarContador();
				Nivel::Instance()->CleanUpLookupCollisionTable();
				engine->SetViewCenterSmooth(prince->GetPosition(),1.0f,dt);				
				prince->Mover_y_Animar(engine->j,dt);		
				EntityManager::Instance()->Mover(engine->j,dt);	
			}

			SlowTime(dt);
		}

		ManejadorDisparos::Instance()->MoverDisparos(dt);
		ParticleSystemManager::Instance()->Simulate(dt);
	}
}

void MainScene::SlowTime(const float& dt,bool restart,bool force)
{
	if(restart)
	{
		if(force)
		{
			isSlowTime = false;
			engine->fpsScale = 1.0f;
		}
		else
		{
			if(!isSlowTime)
			{
				engine->fpsScale = 0.5f;
				isSlowTime = true;
			}
			else
			{
				isSlowTime = false;
				engine->fpsScale = 1.0f;
			}
		}
	}

	if(isSlowTime)
	{
		slowTimeCount += dt; 

		if(slowTimeCount > slowTime)
		{
			isSlowTime = false;
			engine->fpsScale = 1.0f;
			slowTimeCount = 0.f;
		}
	}		
}

void MainScene::IntLevel()
{
	EntityManager::Instance()->Clear();
	ManejadorDisparos::Instance()->Clear();
	ParticleSystemManager::Instance()->Clear();

	//Sincronizamos las Keys
	if(Nivel::Instance() != nullptr)
	{	
		for(int i=0;i<5;i++)
		{
			irKeys[i] = Nivel::Instance()->irKeys[i];			
		}
	}

	Nivel::Instance()->Release();

	Nivel::Instance()->Init(lastLevelFile);

	//Sincronizamos las Keys
	for(int i=0;i<5;i++)
	{		
		//irKeys[i] = 1;
		Nivel::Instance()->irKeys[i] = irKeys[i];
	}

	engine->InitLevelView(Nivel::Instance()->levelSize.x,Nivel::Instance()->levelSize.y,Nivel::Instance()->tileIndividualSize.x,Nivel::Instance()->tileIndividualSize.y);
		
	const Tile* tile = nullptr;
	int mgi = Nivel::Instance()->GetMaxGlobalIndex();
	for(int i=0;i<=mgi;i++)
	{			
		tile = Nivel::Instance()->GetTile(i);
		
		if(tile == nullptr)
		{
			continue;
		}

		if(!tile->isEnable)
		{
			continue;
		}

		if(tile->iEnemigo != -1)
		{	
			if(!isDebug)
			{
				AgregarEnemigo(tile->rect.Left,tile->rect.Top,tile->iEnemigo);
			}
		}
		else if(tile->isDynamic && tile->isEnable)// (tile->iPortal != -1 || tile->isBomb || tile->isKey || tile->isKeyBomb)
		{
			AgregarDynamicObject(tile);
		}
	}
	
	prince->SetPosition(Nivel::Instance()->vEntryPoint);	
	prince->ResetState();	

	cronometro->Reset();
	prince->isDead = false;
	prince->pause = false;
	isPause = false;
	isContinue = true;
	Nivel::Instance()->isNeedNextLoadLevel = false;		
	isReload = false;
	SlowTime(0,true,true);	
}

void MainScene::AgregarDynamicObject(const Tile *tile)
{	
	if(tile->isEnable)
	{
		//verifica si es una llave
		if(tile->iKey != -1)
		{	
			if(Nivel::Instance()->irKeys[tile->iKey] == 1)
			{				
				tile->SetEnable(false);
			}			
		}

		if(tile->isEnable)
		{
			DynamicObject *e = new DynamicObject(tile->iOverLayer,tile->rect);		
			EntityManager::Instance()->Agregar(e);
			tile->SetptrDynObj(e);	
		}
	}
}

void MainScene::ActualizarContador()
{
	cronometro->Update();
	isEndGame = ((Cronometro*)cronometro)->isEndTime();
}

void MainScene::ShowHUD(RenderWindow &w)
{
	FloatRect rect = w.GetView().GetRect();// nivel->levelView.GetRect();	
	Vector2f center;
	center.y = rect.Top + rect.GetHeight() / 2;
	center.x = rect.Left + rect.GetWidth() /2;
	
	cronometro->SetPosition(rect.Right - 50.0f,rect.Top + 5);
	w.Draw(cronometro->DrawObject());
		
	for(int i = 0;i<prince->vidas;i++)
	{
		vidasHUD.SetPosition(rect.Left + 5 + i * 10 ,rect.Top + 5);		
		w.Draw(vidasHUD);
	}
	
	for(int i = 0;i<5;i++)
	{
		if(Nivel::Instance()->irKeys[i] != 1)
		{
			keysHUD.SetColor(Color::Color(255,255,255,150));
		}
		else
		{
			keysHUD.SetColor(Color::Color(255,255,255,255));
		}

		keysHUD.SetPosition(rect.Left + 135 + i * 15 ,rect.Top + 5);
		w.Draw(keysHUD);
	}

	text.SetText(Nivel::Instance()->name);
	text.SetPosition(center.x - 10,rect.Top);
	text.SetColor(Color::Color(255,255,255,250));
	w.Draw(text);

	if(isSlowTime)
	{
		text.SetText("Slow Time");
		text.SetPosition(center.x  ,rect.Top + 30);
		text.SetColor(Color::Color(255,255,255,210));
		w.Draw(text);
	}
	
	if(isGameWon)
	{
		text.SetText("El jugador gana el juego");
		text.SetPosition(center.x - 40 ,center.y - 30);
		text.SetColor(Color::Color(255,255,255,255));
		w.Draw(text);
	}
	else if(isEndGame)
	{
		if(prince->vidas == 0)
		{
			text.SetText("Ya no hay mas vidas");
		}
		else
		{
			text.SetText("El tiempo ha finalizado");
		}
		text.SetPosition(center.x - 40,center.y - 30);
		text.SetColor(Color::Color(255,255,255,255));
		w.Draw(text);
	}
	else if(isQuit)
	{
		text.SetText("Salir del Juego?");		
		text.SetPosition(center.x - 40 ,center.y - 30);
		text.SetColor(Color::Color(255,255,255,255));
		w.Draw(text);

		text.SetText("   Si[s], No[n]");
		text.SetPosition(center.x - 40 ,center.y - 20);
		text.SetColor(Color::Color(255,255,255,255));
		w.Draw(text);
	}
	else if(isPause)
	{
		text.SetText("Pausa");
		text.SetPosition(center.x,center.y - 30);
		text.SetColor(Color::Color(255,255,255,255));
		w.Draw(text);
	}
	else if(!isContinue)
	{
		text.SetText("Presione Enter");		
		text.SetPosition(center.x - 40 ,center.y - 30);
		text.SetColor(Color::Color(255,255,255,255));
		w.Draw(text);

		text.SetText("para continuar");
		text.SetPosition(center.x - 40 ,center.y - 20);
		text.SetColor(Color::Color(255,255,255,255));
		w.Draw(text);
	}
	
}

void MainScene::Render(sf::RenderWindow &w)
{
	if(!isReload)
	{
		Nivel::Instance()->Draw(w);	
		prince->Draw(w);
	//	prince->DrawAABB(w);
		EntityManager::Instance()->Dibujar(w);								
		EntityManager::Instance()->DibujarDynamic(w);
		ManejadorDisparos::Instance()->DibujarDisparos(w);
		Nivel::Instance()->DrawOverLayer(w);
		ParticleSystemManager::Instance()->Render(w);									
		ShowHUD(w);
	//	Nivel::Instance()->DrawGrid(w);	
	}
}

void MainScene::Cleanup()
{
	
}
