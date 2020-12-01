#include "IntroScene.h"
#include "TextureManager.h"
#include "MenuScene.h"

IntroScene::IntroScene(GameEngine *engine)
{
	contador = 0;
	isIntro = false;	
	this->engine = engine;
}

IntroScene::IntroScene(void)
{
	contador = 0;
	isIntro = false;
}

IntroScene::~IntroScene(void)
{

}

void IntroScene::Init()
{
	spriteIntro.SetImage(TextureManager::GetInstance().GetTexture("../data/images/Intro2.png"));	
	engine->PlayMenuMusic();
	
}

void IntroScene::ProcessEvent(const sf::Event& e)
{
	/*if(e.Type == e.Closed)
	{
		if(!isFinished)
		{
			isFinished = true;
			
		}
	}
	if (e.Type == sf::Event::KeyPressed)
	{
		if(e.Key.Code == sf::Key::Escape || e.Key.Code == sf::Key::Return)
		{
			if(!isFinished)
			{
				isFinished = true;				
			}
		}
	}*/
}

void IntroScene::Update(const float& dt)
{	
	contador += dt;	
	/*if(contador > 3 && !isIntro)
	{
		spriteIntro.SetImage(TextureManager::GetInstance().GetTexture("../data/images/Intro2.png"));		
		isIntro = true;
	}	
	else */
	if(contador > 3)
	{
		if(!isFinished)
		{
			isFinished = true;
			engine->PushScene(new MenuScene(engine));									
		}
	}
}

void IntroScene::Render(sf::RenderWindow& w)
{
	w.Draw(spriteIntro);
}

void IntroScene::Cleanup()
{	
	
}
