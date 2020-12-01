#ifndef _Enemigo_
#define _Enemigo_

#include "Personaje.h"

class GameEngine;

namespace EstadosFSM
{
	enum EstadosFSM
	{
		DETENIDO,
		GIRAR,
		CORRER,
		DISPARAR,
		BUSCAR,
		CORRER_Y_DISPARAR,
		NOP
	};
}

class Enemigo :	public Personaje
{
protected:
	enum ModoComportamiento
	{
		GUARDIA=0,
		PATRULLA=1,
		MERCENARIO=2
	};

	static const int MAX_DISPAROS = 10;

	EstadosFSM::EstadosFSM currentState;
	ModoComportamiento currentModo;
	int contadorPasos;
	int contadorDisparos;
	
	GameEngine *engine;

	Personaje *target;
	void AiNpc();

	void ModoGuardia();
	void ModoPatrulla();
	void ModoMercenario();
	void SetupComportamiento();
	bool CheckTargetVisibility();

public:
	int index;
	void AiTracker(Personaje *p);	
	void Inicializar(int index);
	Enemigo(int modo,GameEngine *engine);
	~Enemigo(void);
};


#endif
