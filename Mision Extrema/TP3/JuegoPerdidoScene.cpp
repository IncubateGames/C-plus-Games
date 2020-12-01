#include "JuegoPerdidoScene.h"
#include "TextureManager.h"
#include "IntroScene.h"

JuegoPerdidoScene::JuegoPerdidoScene(void)
{
	contador = 0;
}

JuegoPerdidoScene::JuegoPerdidoScene(GameEngine *engine)
{	
	contador = 0;
	this->engine = engine;
}

JuegoPerdidoScene::~JuegoPerdidoScene(void)
{

}

void JuegoPerdidoScene::Init()
{
	spriteIntro.SetImage(TextureManager::GetInstance().GetTexture("../data/images/GameLost.png"));		
	engine->PlayLostMusic();
}

void JuegoPerdidoScene::ProcessEvent(const sf::Event& e)
{
	/*if(e.Type == e.Closed)
	{		
	}
	if (e.Type == sf::Event::KeyPressed)
	{
		if(e.Key.Code == sf::Key::Escape || e.Key.Code == sf::Key::Return)
		{			
		}
	}*/
}

void JuegoPerdidoScene::Update(const float& dt)
{	
	contador += dt;	
	if(contador > 6)
	{	
		if(!isFinished)
		{
			isFinished = true;			
		}		
	}
}

void JuegoPerdidoScene::Render(sf::RenderWindow& w)
{
	w.Draw(spriteIntro);
}

void JuegoPerdidoScene::Cleanup()
{
	engine->PushScene(new IntroScene(engine));
}
