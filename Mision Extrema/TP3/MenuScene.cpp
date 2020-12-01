#include "MenuScene.h"
#include "TextureManager.h"
#include "MisionScene.h"

MenuScene::MenuScene(void)
{
	contador = 0;
}

MenuScene::MenuScene(GameEngine *engine)
{
	contador = 0;	
	this->engine = engine;
}

MenuScene::~MenuScene(void)
{
}

void MenuScene::Init()
{
	spriteIntro.SetImage(TextureManager::GetInstance().GetTexture("../data/images/Menu.png"));		
}

void MenuScene::ProcessEvent(const sf::Event &e)
{
	if(e.Type == e.Closed)
	{	
		if(!isExit)
		{
			isExit = true;					
		}
	}
	if (e.Type == sf::Event::KeyPressed)
	{
		if(contador == 0)
		{			
			if(e.Key.Code == sf::Key::Space)
			{
				if(!isFinished)
				{
					isFinished = true;	
					engine->PushScene(new MisionScene(engine));
				}
			}

			if(e.Key.Code == sf::Key::Escape)
			{	
				if(!isExit)
				{
					isExit = true;
				}
			}

			if(e.Key.Code == sf::Key::A)
			{				
				spriteIntro.SetImage(TextureManager::GetInstance().GetTexture("../data/images/Elementos.png"));
				contador = 1;
			}
		}
		else
		{		
			if(contador == 1)
			{
				spriteIntro.SetImage(TextureManager::GetInstance().GetTexture("../data/images/HUD.png"));	
				contador = 2;
			}
			else if(contador == 2)
			{
				spriteIntro.SetImage(TextureManager::GetInstance().GetTexture("../data/images/Mandos.png"));
				contador = 3;
			}
			else if(contador == 3)
			{
				spriteIntro.SetImage(TextureManager::GetInstance().GetTexture("../data/images/Menu.png"));
				contador = 0;
			}			
		}
	}
}

void MenuScene::Update(const float &dt)
{			
}

void MenuScene::Render(sf::RenderWindow &w)
{
	w.Draw(spriteIntro);
}

void MenuScene::Cleanup()
{		
}
