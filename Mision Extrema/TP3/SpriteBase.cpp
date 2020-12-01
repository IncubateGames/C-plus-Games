#include <functional>
#include "SpriteBase.h"

#ifndef round
 #define round(r) r-int(r) >= 0.5 ? int(r) + 1 : int(r)
#endif

SpriteBase::SpriteBase(GameEngine *e,int cant_estados,const string &filename,float scale_x,float scale_y,bool NPC)
{
	ptrNextColl_Check = nullptr;
	engine = e;
	isNPC = NPC;
	velocidad.x = 0.0f;
	velocidad.y = 0.0f;
	posicion.x = 0.0f;
	posicion.y = GetPosition().y;
	posicion_inicial.x = 0.0f;
	posicion_inicial.y = 0.0f;
	currentState = 0;
	gravity = 512.0f;
	v0 = -256.0f;
	direccion = Direccion::RIGHT;
	velocidad.y=v0;
	delay = 0;
	ajustaColision_x = 0;
	ajustaColision_y = 0;
	scale.x = scale_x;
	scale.y = scale_y;
	isHitWall = false;
	vidas = 5;
	isDead = false;
	isVisible = true;
	pause = false;
	animaciones = new AnimatedBase[cant_estados];
	
	const sf::Image &tex = TextureManager::GetInstance().GetTexture(filename);	
	SetImage(tex);

	SetScale(scale_x,scale_y);

	this->joystick.a = false;
	this->joystick.b = false;
	this->joystick.down = false;
	this->joystick.left = false;
	this->joystick.right = false;	
	this->joystick.up = false;

	ResetCollMatrix();
}

SpriteBase::~SpriteBase(void)
{
	delete[] animaciones;	
}

void SpriteBase::Disparar(float shootTime,float vel_misiles)
{	
	Vector2f pos = GetPosition();
	IntRect rect = animaciones[currentState].GetCurrentFrameRect();
	Vector2f vect = animaciones[currentState].GetCurrentFrameOffset();

	if(direccion == Direccion::RIGHT)
	{
		Disparar(aabb.Right , aabb.Top + aabb.GetHeight()/4-2 ,shootTime,vel_misiles);
	}
	else if(direccion == Direccion::LEFT)
	{
		Disparar(aabb.Left - aabb.GetWidth()/2, aabb.Top + aabb.GetHeight()/4-2 ,shootTime,vel_misiles);		
	}
}

void SpriteBase::CambiarEstado(int nuevoEstado, bool continueFromCurrent)
{
	int viejoEstado=currentState;
	currentState=nuevoEstado;
	animaciones[nuevoEstado].Reset();	
	if(continueFromCurrent){
		// seteamos el frame de la nueva animacion
		animaciones[nuevoEstado].SetCurrentFrameNum(animaciones[viejoEstado].GetCurrentFrameNum());
		// seteamos el tiempo del frame de la nueva animacion al mismo tiempo que tenia la anterior
		animaciones[nuevoEstado].Animate(animaciones[viejoEstado].GetCurrentFrameElapsedTime());
	}
}

bool SpriteBase::IsAnimationEnded()
{
	return animaciones[currentState].Ended();
}

void SpriteBase::AnimationStop()
{
	animaciones[currentState].Stop();
}

int SpriteBase::AnimationCurrentFrame()
{
	return animaciones[currentState].GetCurrentFrameNum();
}

void SpriteBase::Mover_y_Animar(Joystick &j, float dt)
{	
	if(!pause)
	{
		if(isDead)
			return;
		//Actualiza Delta de tiempo
		this->dt =  dt;
	
		if(!isNPC)
		{
			//Actualiza el estado del control
			this->joystick.a = j.a;
			this->joystick.b = j.b;
			this->joystick.down = j.down;
			this->joystick.left = j.left;
			this->joystick.right = j.right;	
			this->joystick.up = j.up;
		}
		else
		{
			this->joystick.a = false;
			this->joystick.b = false;
			this->joystick.down = false;
			this->joystick.left = false;
			this->joystick.right = false;	
			this->joystick.up = false;

			AiNpc();
		}

		ptrNextColl_Check =  nullptr;

		CalculateAABB();		
		GenerateContacts();

		if(!isNPC)
		{
			engine->TrackingPlayer(this);
			engine->ResolverColision(ColisionMatrix);
		}

		Internal_Mover_y_Animar();

		if(direccion == Direccion::RIGHT)
		{
			FlipX(false);				
		}
		else if(direccion == Direccion::LEFT)
		{	
			FlipX(true);				
		}
		
		//Anima el objeto
		(animaciones+currentState)->Animate(dt);
		IntRect rect = animaciones[currentState].GetCurrentFrameRect();
		SetSubRect(rect);
		
		if(direccion == Direccion::RIGHT)
		{	
			SetCenter(animaciones[currentState].GetCurrentFrameOffset());
			SetOffsetAABB(animaciones[currentState].GetCurrentFrameOffset());
		}
		else if(direccion == Direccion::LEFT)
		{
			SetCenter(animaciones[currentState].GetCurrentFrameFlipOffset());
			SetOffsetAABB(animaciones[currentState].GetCurrentFrameFlipOffset());
		}

		if(!SecuenciaDisparoFinalizada())
		{
			shootTime-=dt;
		}
		/*CalculateAABB();		
		GenerateContacts();
		*/
		/*if(!isNPC)
		{
			engine->TrackingPlayer(this);
			engine->ResolverColision(ColisionMatrix);
		}*/
	}
}

void SpriteBase::AiNpc()
{
	
}

void SpriteBase::SetOffsetAABB(Vector2f &offset)
{
	//aabb.Offset(-offset.x * scale.x ,-offset.y * scale.y);
}

void SpriteBase::CalculateAABB()
{	
	const Vector2f &pos = GetPosition();
	const FloatRect &rec = animaciones[currentState].GetAABB();
	aabb.Top =  pos.y + rec.Top;
	aabb.Left = pos.x + rec.Left;
	aabb.Bottom = pos.y + rec.Bottom;
	aabb.Right = pos.x + rec.Right;
}

FloatRect &SpriteBase::GetAABB()
{
	return aabb;
}

bool SpriteBase::DelayTransition(bool reset)
{
	if(reset)
	{
		delay = 0.15f;
	}
	
	if(delay > 0)
	{
		delay -= dt;
		return true;
	}else
	{
		return false;
	}
}

void SpriteBase::AnimationReverse(bool reverse)
{
	animaciones[currentState].reverse = reverse;
}

void SpriteBase::Inicializar()
{
	InicializarAnimaciones();
}

// para saber si ya expiro el tiempo que dura la secuencia de disparo
bool SpriteBase::SecuenciaDisparoFinalizada(){
	return shootTime<0;
}

void SpriteBase::Disparar(float x, float y,float shoot_time,float velmisiles){
	shootTime=shoot_time;	
	engine->AgregarDisparo(x, y, velmisiles*GetDireccionX(),isNPC);	
}

//saber si choca con alguna pared por derecha o por izquierda
bool SpriteBase::ColisionaPared()
{	
	if(IsCollition)
	{	
		if((ColisionMatrix[Utils::TL].solid && ColisionMatrix[Utils::BL].solid) || (ColisionMatrix[Utils::TR].solid && ColisionMatrix[Utils::BR].solid))
		{

			float result = ColisionMatrix[Utils::TL].adjX + ColisionMatrix[Utils::TR].adjX;
			if(result != 0)
			{
				Move(result,0);
			}								
			return true;
		}					
	}
	return false;	
}

bool SpriteBase::ColisionaParedCuandoJump()
{	
	if(IsCollition)
	{	
		if(ColisionMatrix[Utils::TL].solid || ColisionMatrix[Utils::TR].solid)
		{

			float result = ColisionMatrix[Utils::TL].adjX + ColisionMatrix[Utils::TR].adjX;
			if(result != 0)
			{
				Move(result,0);
			}								
			return true;
		}					
	}
	return false;	
}

bool SpriteBase::CanJump()
{
	return engine->FastColision(aabb,Utils::Up);
};

bool SpriteBase::CanDown()
{
	return engine->FastColision(aabb,Utils::Down);
};

bool SpriteBase::ColisionaTechoClim()
{
	if(IsCollition)
	{
		if(ColisionMatrix[Utils::TL].solid && ColisionMatrix[Utils::TR].solid)
		{
			float resulty = (ColisionMatrix[Utils::TL].adjY + ColisionMatrix[Utils::TR].adjY) * 0.5f;			
			if(resulty != 0)
			{
				Move(0,resulty);		
			}
			velocidad.y = -v0;			
			return true;
		}		
	}
	return false;
};

// saber si choca con el techo
bool SpriteBase::ColisionaTecho(){

	if(IsCollition)
	{		
		if(ColisionMatrix[Utils::TL].solid && ColisionMatrix[Utils::TR].solid)
		{
			float resulty = (ColisionMatrix[Utils::TL].adjY + ColisionMatrix[Utils::TR].adjY)*0.5f;
			
			if(resulty != 0)
			{
				Move(0,resulty);		
			}
			velocidad.y = -v0;			
			return true;
		}	
	}
	return false;
}

void SpriteBase::Mover_y_Animar_NPC(float dt)
{
	if(isDead)
		return;

	isVisible = CheckVisibility();

	//Actualiza Delta de tiempo
	this->dt =  dt;

	this->joystick.a =false;
	this->joystick.b =false;
	this->joystick.down =false;
	this->joystick.left =false;
	this->joystick.right =false;	
	this->joystick.up =false;

	CalculateAABB();
	Internal_Mover_y_Animar();

	if(direccion == Direccion::RIGHT)
	{
		FlipX(false);				
	}
	else if(direccion == Direccion::LEFT)
	{	
		FlipX(true);				
	}

	if(!DelayTransition())
	{
		//Anima el objeto
		(animaciones+currentState)->Animate(dt);
		IntRect rect = animaciones[currentState].GetCurrentFrameRect();
		SetSubRect(rect);
		
		if(direccion == Direccion::RIGHT)
		{	
			SetCenter(animaciones[currentState].GetCurrentFrameOffset());
			SetOffsetAABB(animaciones[currentState].GetCurrentFrameOffset());
		}
		else if(direccion == Direccion::LEFT)
		{
			SetCenter(animaciones[currentState].GetCurrentFrameFlipOffset());
			SetOffsetAABB(animaciones[currentState].GetCurrentFrameFlipOffset());
		}

		//CalculateAABB();
	}

	if(!SecuenciaDisparoFinalizada()) shootTime-=dt;
}

// saber si chocara con el suelo cuando esta cayendo,
// o si hay suelo debajo
bool SpriteBase::ColisionaSuelo()
{
	if(IsCollition)
	{
		bool ajustar = false;
		//Ajustar de acuerdo a la direccion del personaje.
		if(ColisionMatrix[Utils::BL].solid && ColisionMatrix[Utils::BR].solid)
		{
			ajustar = true;
		}
		else if(ColisionMatrix[Utils::BL].solid || ColisionMatrix[Utils::BR].solid)
		{	
			if(!ColisionMatrix[Utils::TL].solid && !ColisionMatrix[Utils::TR].solid)
			{						
				ajustar = true;						
			}
		}

		if(ajustar)
		{
			float resultY = max(ColisionMatrix[Utils::BL].adjY,ColisionMatrix[Utils::BR].adjY);
						
			if(resultY != 0)
			{
				Move(0,resultY);				
				velocidad.y = 0;			
			}						
			return true;
		}
	}
	return false;
}

void SpriteBase::GenerateContacts()
{
	IsCollition = false;
	ResetCollMatrix();

	float vel_y = velocidad.y + gravity * dt;
	float despl_y = vel_y * dt;
	float despl_x = GetDireccionX() * velocidad.x * dt;	

	aabb_tmp.Top = aabb.Top + despl_y;
	aabb_tmp.Bottom =  aabb.Bottom + despl_y;
	aabb_tmp.Left = aabb.Left + despl_x;
	aabb_tmp.Right = aabb.Right + despl_x;
		
	IsCollition  = engine->HayColision(aabb,aabb_tmp,ColisionMatrix);
	
};

void SpriteBase::ResetCollMatrix()
{		
	ColisionMatrix[Utils::TR].ResetValues();
	ColisionMatrix[Utils::TL].ResetValues();	
	ColisionMatrix[Utils::BL].ResetValues();	
	ColisionMatrix[Utils::BR].ResetValues();
};

void SpriteBase::ResolverColision(int tipo,FloatRect aabb_tmp)
{
	//tipo = 1 -> Key
	//tipo = 2 -> Key Bomb
	//tipo = 3 -> Bomb
	if(tipo == 1)
	{
		
	}
	else if(tipo == 2)
	{

	}
	else if(tipo == 3)
	{
		ParticleSystemManager::Instance()->CreateEmiterOneExplosion(aabb_tmp.Left+aabb_tmp.GetWidth()/2,aabb_tmp.Top+aabb_tmp.GetHeight()/2,Color::White);
		isDead = true;
		vidas--;
	}
	else if(tipo == 4)
	{

	}
	else if(tipo >= 10 && tipo <= 19)
	{
		cout<<"Cargar Nivel"<<"\n";
	}
}

void SpriteBase::AjustaColisionX()
{	
	Move(ajustaColision_x, 0);					
}

void SpriteBase::AjustaColisionY()
{
	Move(0, ajustaColision_y);
}

int SpriteBase::GetDireccionX()
{
	if(direccion == Direccion::LEFT)
			return -1;
	else if(direccion == Direccion::RIGHT)
			return 1;
	return 0;
}

void SpriteBase::Draw(sf::RenderWindow &w)
{
	if(isVisible && !isDead)
	{
		w.Draw(*this);
	}
}

void SpriteBase::DrawAABB(sf::RenderWindow& w)
{
	sf::Shape rectangle = sf::Shape::Rectangle(aabb.Left,aabb.Top,aabb.Right,aabb.Bottom,sf::Color(0,0,0,0),1,sf::Color::Blue);
	w.Draw(rectangle);
}

bool SpriteBase::CheckVisibility()
{
	Vector2f pos = GetPosition();

	if(pos.x < 0)
		pos.x = 0;
	if(pos.y < 0)
		pos.y = 0;

	/*FloatRect view = nivel->levelView.GetRect();
	return pos.x > view.Left && pos.x < view.Right && pos.y > view.Top && pos.y < view.Bottom;*/
	return false;
}

bool SpriteBase::RecibirImpacto(float x,float y)
{
	if(isVisible && !isDead)
	{
		if(aabb.Contains(x,y))
		{
			isDead = true;
			vidas--;
			engine->PlayShootImpactSound();
			return true;
		}
	}
	return false;
}

void SpriteBase::RecibirImpacto()
{
	if(isVisible && !isDead)
	{
		isDead = true;
		vidas--;
		engine->PlayExplosionSound();
	}
}
