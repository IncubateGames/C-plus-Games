#include "Enemigo.h"

#include "SFML\Graphics.hpp"

using namespace sf;

Enemigo::Enemigo(int modo,GameEngine *e):Personaje(e)
{
	engine = e;
	isNPC = true;	
	contadorPasos = 0;
	contadorDisparos = 0;
	direccion = Direccion::LEFT;
	currentState = EstadosFSM::DETENIDO;

	switch(modo)
	{
	case GUARDIA:
		currentModo = GUARDIA;
		break;
	case PATRULLA:
		currentModo = PATRULLA;
		break;
	case MERCENARIO:
		currentModo = MERCENARIO;
		break;
	}
}

Enemigo::~Enemigo(void)
{

}

void Enemigo::AiTracker(Personaje *p)
{
	target = p;	
}

void Enemigo::Inicializar(int index)
{
	this->index  = index;
	Personaje::Inicializar();	
	SetupComportamiento();
}


void Enemigo::SetupComportamiento()
{
	switch(currentModo)
	{
	case GUARDIA:
		SetColor(sf::Color::Red);
		break;
	case PATRULLA:
		SetColor(Color::Green);
		break;
	case MERCENARIO:
		SetColor(Color::Blue);
		break;
	}
	
}

void Enemigo::AiNpc()
{
	if(SpriteBase::currentState != PlayerState::Estado::JUMP_DOWN)
	{
		switch(currentModo)
		{
		case GUARDIA:
			ModoGuardia();
			break;
		case PATRULLA:
			ModoPatrulla();
			break;
		case MERCENARIO:
			ModoMercenario();
			break;
		}
	}
}

void Enemigo::ModoGuardia()
{
	if(currentState == EstadosFSM::DETENIDO)
	{
		int opcion = rand()%3;
						
		switch(opcion)
		{
		case 0:
			currentState = EstadosFSM::GIRAR;
			break;		
		case 2:
			currentState = EstadosFSM::BUSCAR;
			break;
		}
		contadorPasos = rand()%100+100;
	}
	else if(currentState == EstadosFSM::BUSCAR)
	{
		bool isTargetInRange =  CheckTargetVisibility();

		if(isTargetInRange)
		{
			currentState = EstadosFSM::DISPARAR;	
			contadorDisparos = rand()%50+50;
			contadorPasos += rand()%50+50;
		}
	}
	else if(currentState == EstadosFSM::GIRAR)
	{
		if(direccion == Direccion::LEFT)
		{
			joystick.right  = true;
		}
		else
		{
			joystick.left  = true;
		}
		currentState = EstadosFSM::NOP;
	}	
	else if(currentState == EstadosFSM::DISPARAR)
	{
		if(contadorDisparos > 0)
		{
			joystick.a = true;
			joystick.down = true;
			contadorDisparos--;
		}
		else
		{
			currentState = EstadosFSM::NOP;
		}

	}

	if(contadorPasos <= 0)
	{
		currentState = EstadosFSM::DETENIDO;
	}else
	{
		contadorPasos--;
	}
}

void Enemigo::ModoPatrulla()
{
	if(currentState == EstadosFSM::DETENIDO)
	{
		int opcion = rand()%3;
		
		switch(opcion)
		{
		case 0:
			currentState = EstadosFSM::GIRAR;
			break;
		case 1:
			currentState = EstadosFSM::CORRER;		
			break;
		case 2:
			currentState = EstadosFSM::BUSCAR;
			break;
		}
		contadorPasos = rand()%20+20;
	}
	else if(currentState == EstadosFSM::BUSCAR)
	{
		bool isTargetInRange =  CheckTargetVisibility();

		if(isTargetInRange)
		{
			currentState = EstadosFSM::DISPARAR;	
			contadorDisparos = rand()%20+20;
			contadorPasos = rand()%50+50;
		}
	}
	else if(currentState == EstadosFSM::GIRAR)
	{
		if(direccion == Direccion::LEFT)
		{
			joystick.right  = true;
		}
		else
		{
			joystick.left  = true;
		}
		currentState = EstadosFSM::NOP;
	}
	else if(currentState == EstadosFSM::CORRER)
	{
		bool isTargetInRange =  CheckTargetVisibility();
			
		if(isTargetInRange)
		{
			currentState = EstadosFSM::DETENIDO;
		}

		if(isHitWall)
		{
			currentState = EstadosFSM::GIRAR;
		}
		if(direccion == Direccion::LEFT)
		{
			joystick.left = true;
		}
		else
		{
			joystick.right = true;
		}
	}
	else if(currentState == EstadosFSM::DISPARAR)
	{
		if(contadorDisparos > 0)
		{
			joystick.a = true;
			contadorDisparos--;
		}
	}

	if(contadorPasos <= 0)
	{
		currentState = EstadosFSM::DETENIDO;
	}else
	{
		contadorPasos--;
	}
}

void Enemigo::ModoMercenario()
{
	if(currentState == EstadosFSM::DETENIDO)
	{
		int opcion = rand()%2;
		
		switch(opcion)
		{
		case 0:
			currentState = EstadosFSM::BUSCAR;
			break;
		case 1:
			currentState = EstadosFSM::GIRAR;
			break;
		}
		contadorPasos = rand()%20+20;
	}
	else if(currentState == EstadosFSM::BUSCAR)
	{
		bool isTargetInRange =  CheckTargetVisibility();

		if(isTargetInRange)
		{
			int opcion = rand()%2;

			switch(opcion)
			{			
			case 0:
				currentState = EstadosFSM::DISPARAR;
				break;
			case 1:
				currentState = EstadosFSM::CORRER_Y_DISPARAR;
				break;
			}				
			contadorDisparos = rand()%20+20;
			contadorPasos += rand()%10+10;
		}
	}
	else if(currentState == EstadosFSM::GIRAR)
	{
		if(direccion == Direccion::LEFT)
		{
			joystick.right  = true;
		}
		else
		{
			joystick.left  = true;
		}
		currentState = EstadosFSM::DETENIDO;
	}
	else if(currentState == EstadosFSM::CORRER)
	{
		if(isHitWall)
		{
			currentState = EstadosFSM::GIRAR;
		}
		if(direccion == Direccion::LEFT)
		{
			joystick.left = true;
		}
		else
		{
			joystick.right = true;
		}
	}	
	else if(currentState == EstadosFSM::DISPARAR)
	{	
		if(contadorDisparos > 0)
		{
			joystick.a = true;
			contadorDisparos--;
		}		
	}
	else if(currentState == EstadosFSM::CORRER_Y_DISPARAR)
	{
		if(direccion == Direccion::LEFT)
		{
			joystick.left = true;
		}
		else
		{
			joystick.right = true;
		}
		if(contadorDisparos > 0)
		{
			joystick.a = true;
			contadorDisparos--;
		}
	}

	if(contadorPasos < 0)
	{
		currentState = EstadosFSM::DETENIDO;
	}
	else
	{
		contadorPasos--;
	}
}

bool Enemigo::CheckTargetVisibility()
{	
	Vector2f pos = GetPosition();
	Vector2f targetpos = target->GetPosition();
	
	int dir  = GetDireccionX();
	
	if(dir < 0 && pos.x > targetpos.x || dir > 0 && pos.x < targetpos.x)
	{
		if(abs(pos.y - targetpos.y) <= 20)
		{
			//6 tiles de separacion
			if(abs(pos.x - targetpos.x) <= 192.0f)
			{
				return true;
			}
		}
	}	
	
	return false;
}

