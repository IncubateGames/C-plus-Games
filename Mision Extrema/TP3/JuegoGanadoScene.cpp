#include "JuegoGanadoScene.h"
#include "TextureManager.h"
#include "IntroScene.h"

JuegoGanadoScene::JuegoGanadoScene(void)
{
	contador = 0;
}

JuegoGanadoScene::JuegoGanadoScene(GameEngine *engine)
{
	contador = 0;	
	this->engine = engine;
}

JuegoGanadoScene::~JuegoGanadoScene(void)
{

}

void JuegoGanadoScene::Init()
{
	spriteIntro.SetImage(TextureManager::GetInstance().GetTexture("../data/images/GameWon.png"));		
}

void JuegoGanadoScene::ProcessEvent(const sf::Event& e)
{
	/*if(e.Type == e.Closed)
	{
		
	}
	*/
	if (e.Type == sf::Event::KeyPressed)
	{
		if(e.Key.Code == sf::Key::Escape || e.Key.Code == sf::Key::Return)
		{
			if(!isFinished)
			{
				isFinished = true;
			}
		}
	}
}

void JuegoGanadoScene::Update(const float& dt)
{		
	/*contador += dt;		
	if(contador > 20)
	{		
		if(!isFinished)
		{
			isFinished = true;
		}		
	}*/			
}

void JuegoGanadoScene::Render(sf::RenderWindow& w)
{
	w.Draw(spriteIntro);
}

void JuegoGanadoScene::Cleanup()
{	
	engine->PushScene(new IntroScene(engine));
}
