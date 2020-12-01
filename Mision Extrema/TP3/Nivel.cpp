#include "Nivel.h"
#include <fstream>
#include <iomanip>
#include <assert.h>
#include "SFML\Graphics\Image.hpp"

Nivel* Nivel::_instance = nullptr;

// Constructor: cargar el nivel desde el archivo level_file
Nivel::Nivel(string level_file)
{	
	isNeedNextLoadLevel = false;
	this->level_file = level_file;	
	tiles = nullptr;
	backbuff = nullptr;
	imgBackbuff = nullptr;
	backbuff_over = nullptr;
	imgBackbuff_over = nullptr;
	maxGlobalIndex = -1;
	iEnemigos = 0;
	name = "";
	isDebug = false;
	GameWon = false;
}

Nivel::~Nivel()
{	
	Unload();
}

void Nivel::Unload()
{
	{
		map<int,Level*>::iterator it = nextLevels.begin();	
		while(it != nextLevels.end())
		{	
			delete it->second;
			it->second = nullptr;
			it++;
		}
		nextLevels.clear();
	}

	{
		vector<ParallaxLayer*>::iterator it = capasParallax.begin();
		while(it != capasParallax.end())
		{
			delete *it;
			*it = nullptr;
			it++;
		}
		capasParallax.clear();
	}

	CollisionTable.clear();
	
	if(tiles != nullptr)
	{
		delete[] tiles;
		tiles = nullptr;
	}

	if(backbuff != nullptr)
	{
		delete backbuff;
		backbuff = nullptr;
	}

	if(imgBackbuff != nullptr)
	{
		delete imgBackbuff;
		imgBackbuff = nullptr;
	}

	if(backbuff_over  != nullptr)
	{
		delete backbuff_over;
		backbuff_over  = nullptr;
	}

	if(imgBackbuff_over != nullptr)
	{
		delete imgBackbuff_over;
		imgBackbuff_over = nullptr;
	}

	maxGlobalIndex  = 0;
}

// carga un nivel desde un archivo de nivel
void Nivel::Load()
{	
	// abrimos el archivo
	ifstream entrada(level_file);

	assert(entrada.is_open()==true);
	
	//Nombre del Nivel
	std::getline(entrada,name);

	// leemos el nombre del archivo de tilesets
	entrada >> tileset_filename;

	//Cargamos los files para paralax
	entrada >> capasparalaxFiles[0];
	entrada >> capasparalaxFiles[1];
	
	//cargamos el tamaño en tiles del archivo de tiles
	entrada >> tileSize.x;
	entrada	>> tileSize.y;

	//cargamos el tamaño en tiles del nivel
	entrada >> levelSize.x;
	entrada	>> levelSize.y;
	
	// cargamos el tileset
	sm.Load(tileset_filename, tileSize.x, tileSize.y,levelSize.x,levelSize.y);
	
	sm.GetImage(0,FloatRect());
	Vector2f size = sm.GetSize();
	tileIndividualSize.x = (int) size.x;
	tileIndividualSize.y = (int) size.y;
	
	// inicializamos la matriz de tiles
	int aux = 0;

	tiles = new Tile[levelSize.x * levelSize.y];
	
	//leemos la matriz de numeros de imagenes
	for(int y=0; y<levelSize.y; y++){
		for(int x=0; x<levelSize.x; x++){
			entrada >> aux;
			tiles[levelSize.x * y + x ].iImage = aux-1;			
		}
	}
	
	//01 solid
	//02 solido transparente
	//03 bomb		
	//10-19 portal
	//20-29 recolectable -> 20,21,22,23,24 -> son las claves
	//60-69 Enemigo
	//88 Key Bomb
	//99 Enter Point - es donde aparece el player
	//100 solid - floor
	//101 solid - wall
	//102 solid - ceiling
	// leemos la matriz que nos indica cuales
	// tiles son solidos

	Tile *tiletmp = nullptr;

	maxGlobalIndex = CalculateGlobalIndex(levelSize.x - 1,levelSize.y-1);
	int gi = -1;
	aux = 0;
	for(int y=0; y<levelSize.y; y++){
		for(int x=0; x<levelSize.x; x++){
			entrada>>aux;

			gi =  x + levelSize.x * y;

			tiletmp = &tiles[gi];
			tiletmp->iGlobalIndex = gi;
			tiletmp->index_x = x;
			tiletmp->index_y = y;
			
			tiletmp->rect.Top = y * tileIndividualSize.y * 1.0f;
			tiletmp->rect.Bottom = tiletmp->rect.Top + tileIndividualSize.y;
			tiletmp->rect.Left = x * tileIndividualSize.x * 1.0f;
			tiletmp->rect.Right = tiletmp->rect.Left + tileIndividualSize.x;

			tiletmp->iType = aux;

			if(aux == 1)
			{
				tiletmp->solid = true;
			}	
			else if(aux == 2)
			{
				tiletmp->isSolidAlpha = true;	
				tiletmp->solid = true;
			}
			else if(aux == 3)
			{
				tiletmp->isBomb = true;	
				tiletmp->isDynamic = true;
			}
			else if(aux >= 10 && aux <= 19)
			{				
				tiletmp->iPortal = aux;					
			}
			else if(aux >= 20 && aux <= 29)
			{					
				tiletmp->isKey = true;
				tiletmp->iKey = aux - 20; 
				tiletmp->isDynamic = true;
				//-->??
				/*if(irKeys[tiles[y][x].iKey] != 0)
				{
					tiles[y][x].isDead = true;
				}*/
			}
			else if(aux >= 60 && aux <= 69)
			{
				iEnemigos++;
				tiletmp->iEnemigo = aux - 60;				
			}
			else if(aux == 88)
			{				
				tiletmp->isDynamic = true;
				tiletmp->isKeyBomb = true;
			}
			else if(aux == 99)
			{	
				tiletmp->isEntryPoint = true;				
				vEntryPoint.x = x * tileIndividualSize.x * 1.0f;
				vEntryPoint.y = y * tileIndividualSize.y * 1.0f;				
			}
			else if(aux == 100)
			{
				tiletmp->solid = true;
				tiletmp->floor = true;
			}
			else if(aux == 101)
			{
				tiletmp->solid = true;
				tiletmp->wall = true;
			}
			else if(aux == 102)
			{
				tiletmp->solid = true;
				tiletmp->ceiling = true;
			}
		}
	}

	// leemos la matriz que nos indica cuales
	// tiles son overlay
	aux = 0;
	for(int y=0; y<levelSize.y; y++)
	{
		for(int x=0; x<levelSize.x; x++){
			entrada >> aux;	
			tiletmp = &tiles[x + levelSize.x * y];				
			tiletmp->iOverLayer = aux-1;
		}
	}
			
	Level *templevel = NULL;
	
	int portals = 0;
	
	entrada >> portals;
	
	//Cargamos los link a los niveles
	for(int i=0; i<portals; i++){		
		templevel = new	Level();
		entrada >> templevel->index;
		entrada >> templevel->file;
		nextLevels.insert(pair<int,Level*>(templevel->index,templevel));
	}
	

	float offsetYCapas[]={0,20,110};
	float offsetXCapas[]={0,0,0};	
	float velCapas[]={0.0010f, 0.0015f, 0.0015f};
		
	for(unsigned i=0; i<2; i++)
	{	 
		 capasParallax.push_back(new ParallaxLayer(TextureManager::GetInstance().GetTexture(capasparalaxFiles[i]),
											   velCapas[i], true, offsetXCapas[i], 0, false, offsetYCapas[i]));
	}

	// cerramos el archivo
	entrada.close();

	PrepareBackBuff();
	PrepareLookupCollisionTable();

#ifdef _DEBUG
	{
		int x = 3;
		int y = 3;
		int gi = CalculateGlobalIndex(x,y);
		int x1 = -1,y1 = -1;
		ConverGlobalIndexToIndex(gi,x1,y1);
		
		assert(tiles[gi].index_x == x);
		assert(tiles[gi].index_y == y);

		assert(x1 == x);
		assert(y1 == y);
		
		ConverGlobalIndexToIndex(tiles[maxGlobalIndex].iGlobalIndex,x1,y1);

		assert(x1 == tiles[maxGlobalIndex].index_x);
		assert(y1 == tiles[maxGlobalIndex].index_y);
	}
#endif

}

void Nivel::PrepareBackBuff()
{
	// crea una imagen
	imgBackbuff = new sf::Image(levelSize.x*tileIndividualSize.x, levelSize.y*tileIndividualSize.y,Color::Color(0,0,0,0));
	imgBackbuff_over = new sf::Image(levelSize.x*tileIndividualSize.x, levelSize.y*tileIndividualSize.y,Color::Color(0,0,0,0));

	Tile *tiletmp = nullptr;
	const IntRect* rect =  nullptr;

	const Image& img = sm.InternalImage();

	// dibuja los tiles en la imagen
	for(int y=0; y<levelSize.y; y++)
	{
		for(int x=0; x<levelSize.x; x++)
		{
			tiletmp = &tiles[levelSize.x * y + x ];

			if(tiletmp->iOverLayer != -1 && !tiletmp->isDynamic)
			{
				rect = &sm.GetRect(tiletmp->iOverLayer);
				imgBackbuff_over->Copy(img, x*tileIndividualSize.x, y*tileIndividualSize.y,*rect);
			}				
			if(tiletmp->iImage != -1)
			{		
				rect = &sm.GetRect(tiletmp->iImage);
				imgBackbuff->Copy(img, x*tileIndividualSize.x, y*tileIndividualSize.y,*rect);
			}						
		}
	}		
	
	backbuff = new Sprite(*imgBackbuff);
	backbuff_over = new Sprite(*imgBackbuff_over);
}

void Nivel::PrepareLookupCollisionTable()
{
	CollisionTable.reserve(iEnemigos);
	CollisionTable.clear();
}

void Nivel::CleanUpLookupCollisionTable()
{
	vector<int>::iterator it;
	for(it=CollisionTable.begin();it!=CollisionTable.end();it++)
	{
		if(*it != -1)
		{	
			tiles[*it].iLookupCollision = nullptr;
			tiles[*it].iCountLookupCollisionItem = 0;
			*it = -1;
		}
	}
	CollisionTable.clear();
}

int Nivel::CalculateGlobalIndex(const int& x,const int& y) const
{
	return levelSize.x * y + x;
};

int Nivel::ConvertPointToGlobalIndex(const float& pos_x,const float& pos_y) const 
{
	int rel_x = static_cast<int>(pos_x / tileIndividualSize.x);
	int rel_y = static_cast<int>(pos_y /  tileIndividualSize.y);

	int g_pos = CalculateGlobalIndex(rel_x,rel_y);

	return g_pos;
};

void Nivel::ConverGlobalIndexToIndex(const int& globalindex,int& pos_x,int& pos_y) const
{
	pos_y = globalindex / levelSize.x;
	pos_x = globalindex - pos_y * levelSize.x;
};

int Nivel::GetDirPoint(const int& gloabindex,const Utils::TCollDirection& dir) const
{
	int x = 0;
	int y = 0;
	
	ConverGlobalIndexToIndex(gloabindex,x,y);
	switch(dir)
	{
	case Utils::Up:
		y--;
		break;
	case Utils::Down:
		y++;
		break;
	case Utils::Left:
		x--;
		break;
	case Utils::Right:
		x++;
		break;
	};	
	
	return CalculateGlobalIndex(x,y);
};

// guardamos el nivel en un archivo
void Nivel::Save(string& filename){
	
	//// abrimos el archivo de salida
	//ofstream salida(filename.c_str());
	//// guardamos el nombre del archivo con el tileset
	//salida<<tileset_filename<<endl;
	//// guardamos los tamanos del tileset y del nivel
	//salida<<tileSetSize.x<<" "<<tileSetSize.y<<endl;
	//salida<<levelSize.x<<" "<<levelSize.y<<endl;
	//salida<<endl;
	//
	//// guardamos los numeros de imagen de cada tile
	//for(unsigned i=0; i<levelSize.y; i++){
	//	for(unsigned j=0; j<levelSize.x; j++){
	//		salida<<setw(3)<<tiles[i][j].iImage<<" ";
	//	}
	//	salida<<endl;
	//}
	//salida<<endl;
	//
	//// para cada tile guardamos si es o no solido
	//for(unsigned i=0; i<levelSize.y; i++){
	//	for(unsigned j=0; j<levelSize.x; j++){
	//		salida<<setw(3)<<tiles[i][j].solid<<" ";
	//	}
	//	salida<<endl;
	//}
	//// cerramos el archivo
	//salida.close();
}

// dibuja el tilemap del nivel en la ventana w, para ello utiliza
// la funcion GetOverlappingTiles() para saber que tiles aparecen
// en la vista actual del nivel
void Nivel::Draw(sf::RenderWindow& w)
{	
	vector<ParallaxLayer*>::iterator it;
	for(it=capasParallax.begin();it != capasParallax.end();it++)
	{
		(*it)->Draw(w);
	}
	w.Draw(*backbuff);
}

void Nivel::DrawOverLayer(sf::RenderWindow& w)
{		
	w.Draw(*backbuff_over);
}

// dibuja una grilla sobre los tiles
void Nivel::DrawGrid(sf::RenderWindow& w){
	// coordenadas de la vista actual en tiles
	float x1, y1, x2, y2;
	sf::FloatRect r = w.GetView().GetRect();
	x1 = (r.Left/tileSize.x);
	x2 = ((r.Right/tileSize.x))+1;
	y1 = (r.Top/tileSize.y);
	y2 = ((r.Bottom/tileSize.y))+1;
	
	// dibuja las lineas verticales
	for(float i = x1; i <= x2; i++){
		sf::Shape Line = sf::Shape::Line(i*tileIndividualSize.x, y1*tileIndividualSize.y, i*tileIndividualSize.x, y2*tileIndividualSize.y, 1, sf::Color(255,255,255));
		w.Draw(Line);
	}
	
	// dibuja las lineas horizontales
	for(float i = y1; i<=y2; i++){
		sf::Shape Line = sf::Shape::Line(x1*tileIndividualSize.x, i*tileIndividualSize.y, x2*tileIndividualSize.x, i*tileIndividualSize.y, 1, sf::Color(255,255,255));
		w.Draw(Line);
	}
}

// genera y guarda una imagen con el tilemap
void Nivel::SaveToImage(string& filename){
	//// crea una imagen
	//sf::Image imagen(levelSize.x*tileSize.x, levelSize.y*tileSize.y, sf::Color(0, 0, 0, 0));
	//// dibuja los tiles en la imagen
	//for(unsigned i=0; i<levelSize.y; i++){
	//	for(unsigned j=0; j<levelSize.x; j++){
	//		if(tiles[i][j].iImage!=-1){				
	//			sm.GetImage(tiles[i][j].iImage,tiles[i][j].rect);			
	//			imagen.Copy(sm.InternalImage(), j*tileSize.x, i*tileSize.y);
	//		}
	//	}
	//}
	//// guarda la imagen
	//imagen.SaveToFile(filename);
}

void Nivel::RenderTile(const int& index,const FloatRect& rect,sf::RenderWindow &w)
{
	sm.SelectImage(index,rect.Left,rect.Top);
	w.Draw(sm);
}