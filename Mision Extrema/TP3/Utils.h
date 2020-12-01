#ifndef _Utils_
#define _Utils_

#include "SFML\Graphics\Rect.hpp"
#include <assert.h>
#include "Nivel.h"

namespace Utils
{
	namespace Tiletype
	{
		typedef enum Tiletype
		{
			NOTHING = 0,
			SOLID = 1
		};	
	};

	typedef enum TCollDirection
	{
		Up,
		Down,
		Left,
		Right
	};

	typedef enum TCollType
	{
		TR = 0,
		TL,
		BL,
		BR,
		NA
	};

	class Tile// : public Sprite
	{	
		public:			
			int iImage;		// el numero de imagen
			bool solid;		// si se puede chocar con el tile
			bool wall;		//si es pared
			bool floor;		//si es piso
			bool ceiling;	//Si es techo
			int  iPortal;	//indice si es una puerta a otro nivel
			bool isBomb; // indica si es una bomba
			int iOverLayer; //indica si debe pintarse sobre
			bool isAnim; //indica si esta animado
			int iEnemigo; //indica si es un enemigo
			bool isEntryPoint; //punto en el cual aparece el personaje cuando empieza el nivel
			int iType; //
			sf::FloatRect rect; // para facilitar la deteccion de colisiones
			bool isDead; //indica si ya se uso. No hay que mostrarlo ni detectar colisiones
			bool isKey; //indica si es una clave para desactivar la bomba final
			int iKey; //indice que indica cual es la clave, hay 5 en total
			bool isDynamic; //indica que no es un elemento fijo del nivel
			bool isKeyBomb; // es la bomba que hay que desactivar		
			bool isSolidAlpha; //indica que es un tile de colision pero transparente
			void *iLookupCollision;			
			int iCountLookupCollisionItem;
			int iGlobalIndex;
			int index_x;
			int index_y;			
			bool isTrackingPlayer;
			mutable bool isEnable;
			mutable void *ptrDynObj;
			
			void SetOffTrackingPlayer()
			{
				isTrackingPlayer = false;
			};

			void SetptrDynObj(void *ptrDynObj) const
			{
				this->ptrDynObj = ptrDynObj;
			};

			void SetEnable(bool state) const
			{
				isEnable = state;
			};

			Tile()
			{
				isEnable = true;
				iImage=-1;
				solid = false;
				wall = false;
				floor = false;
				ceiling = false;
				iPortal=-1;
				isBomb=false;
				iOverLayer=-1;
				isAnim=false;
				iEnemigo = -1;
				isEntryPoint = false;							
				iType = -1;
				rect = FloatRect();
				isDead = false;
				isKey = false;
				iKey = -1;
				isDynamic = false;
				isKeyBomb = false;
				iLookupCollision = nullptr;
				iCountLookupCollisionItem = 0;
				iGlobalIndex = -1;
				index_x = -1;
				index_y = -1;	
				isTrackingPlayer = false;
				ptrDynObj = nullptr;
				isSolidAlpha = false;
			};
	};
	
	class CollItem
	{
	private:
		CollItem(const CollItem& other){;};
		CollItem& operator==(const CollItem& other){;};
	public:
		int globalIndex;
		int iType;
		float adjX;
		float adjY;
		int iPortal;
		bool isBomb;
		int iKey;
		bool isKeyBomb;
		bool solid;
		TCollType colltype;

		CollItem(){;};

		void ResetValues()
		{
			adjX = 0;
			adjY = 0;
			globalIndex = -1;
			iType = -1;
			iKey = -1;
			iPortal = -1;
			isBomb = false;
			isKeyBomb = false;
			solid = false;
			colltype = Utils::NA;
		};

		bool LoadValues(const sf::FloatRect& s,const sf::FloatRect& r,const Tile *const tile,TCollType collType)
		{	

			if(tile != nullptr && tile->isEnable)
			{
				this->globalIndex = tile->iGlobalIndex;
				this->iType = tile->iType;
				this->iKey = tile->iKey;
				this->isBomb = tile->isBomb;
				this->iPortal = tile->iPortal;
				this->isKeyBomb = tile->isKeyBomb;
				this->solid = tile->solid;
				this->colltype = collType;

				if(tile->isDynamic && tile->isDead)
				{
					return false;
				}
				else if(iPortal != -1 || isBomb || iKey != -1 || isKeyBomb || solid)
				{
					if(solid)
					{					
						//obtener el sentido del desplazamiento
						int x = static_cast<int>((s.Left - r.Left));
						int y = static_cast<int>((s.Top - r.Top));
						//y<0 bajando
						//x<0 a la derecha
						switch(collType)
						{
						case Utils::TL:
							//Ajustar X
							if( x > 0)
							{
								this->adjX = tile->rect.Right - s.Left;
							}
							//Ajustar Y
							if(y > 0)
							{
								this->adjY = tile->rect.Bottom - s.Top;
							}							
							break;
						case Utils::TR:
							//Ajustar X
							if(x < 0)
							{
								this->adjX = tile->rect.Left - s.Right;
							}							
							//Ajustar Y
							if(y > 0)
							{
								this->adjY = tile->rect.Bottom - s.Top;
							}
							break;
						case Utils::BL:
							//Ajustar X
							if( x > 0)
							{
								this->adjX = tile->rect.Right - s.Left;
							}
							//Ajustar Y
							if(y < 0)
							{								
								this->adjY = tile->rect.Top - s.Bottom;
							}
							break;
						case Utils::BR:
							//Ajustar X
							if(x < 0)
							{
								this->adjX = tile->rect.Left - s.Right;
							}								
							//Ajustar Y
							if(y < 0)
							{								
								this->adjY = tile->rect.Top - s.Bottom;
							}
							break;
						}
					}
					return true;
				}
			}
			return false;		
		};

		~CollItem(){};
	};
};

#endif