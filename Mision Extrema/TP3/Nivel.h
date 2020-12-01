#ifndef _Nivel_
#define _Nivel_

#include <iostream>
#include <vector>
#include <SFML/Graphics/RenderWindow.hpp>
#include "SpriteSheetManager.h"
#include "ParallaxLayer.h"
#include "AnimatedBase.h"
#include "SceneBase.h"
#include "Utils.h"

class SceneBase;

using namespace std;
using namespace sf;
using namespace Utils;

class Nivel{
private:
	struct Level
	{
		public:
		int index;
		string file;
	};
	
	//-->

	Sprite* backbuff;
	Image*  imgBackbuff;

	Sprite* backbuff_over;
	Image*  imgBackbuff_over;

	string level_file;
	string capasparalaxFiles[2];
		
	// la matriz de tiles	
	Tile *tiles;
	int maxGlobalIndex;

	//Matriz con los jump a los otro niveles
	map<int, Level*> nextLevels;
		
	int iEnemigos;

	// nombre del archivo tileset
	string tileset_filename;

	// manejador de las imagenes del tileset
	SpriteSheetManager sm;

	vector<ParallaxLayer*> capasParallax;
	
	float g_xo,g_xf,g_yo,g_yf;
	// aca vamos a guardar el tamano final de la vista (en tiles)
	float realtiles_x, realtiles_y;

	void Unload();
	void PrepareBackBuff();
	void PrepareLookupCollisionTable();
	
	Nivel(){;};
	Nivel(string level_file);
	Nivel(Nivel const& other){;};
	Nivel& operator=(Nivel const& other){;};
	
	static Nivel* _instance;

	mutable bool GameWon;

public:	
	vector<int> CollisionTable;

	int GetMaxGlobalIndex() const
	{
		return maxGlobalIndex;
	};

	int CalculateGlobalIndex(const int& x, const int& y) const;
	int ConvertPointToGlobalIndex(const float& pos_x,const float& pos_y) const;
	void ConverGlobalIndexToIndex(const int& globalindex,int& pos_x,int& pos_y) const;
	int GetDirPoint(const int& gloabindex,const Utils::TCollDirection& dir) const;
	void RenderTile(const int& index,const FloatRect& rect,sf::RenderWindow &w);
	
	//tamaño del nivel en tiles (ancho x alto)
	sf::Vector2i levelSize;
	//tamaño de los tiles (ancho x alto)
	sf::Vector2i tileSize;	

	//tamaño de los tiles (ancho x alto)
	sf::Vector2i tileIndividualSize;

	//Donde aparece el player
	Vector2f vEntryPoint;

	bool isNeedNextLoadLevel;

	string name;	
	int irKeys[5];
	bool isDebug;

	sf::Sprite imgLevel;
	string fileNextLevel;

	~Nivel();

	// salvar y guardar un nivel
	void Load();
	void Save(string& file);
	
	// tamano de tiles y del nivel
	const sf::Vector2i& GetTileSize()
	{
		return tileSize;
	};

	const sf::Vector2i& GetLevelSize()
	{
		return levelSize;
	};

	void SetNextLevel(const int& levelIndex)
	{
		isNeedNextLoadLevel = true;
		fileNextLevel = nextLevels[levelIndex]->file;
	};
	
	// dibujar el nivel en pantalla
	void Draw(sf::RenderWindow& w);
	void DrawOverLayer(sf::RenderWindow& w);		
	void CleanUpLookupCollisionTable();

	// funciones para inicializar la vista
	sf::View& InitLevelView(int res_x, int res_y, int tiles_x=-1, int tiles_y=-1);
	sf::View& InitLevelView();
	
	// algunas funciones utiles
	void DrawGrid(sf::RenderWindow& w);
	void SaveToImage(string& filename);
	
	static Nivel* Instance()
	{
		return _instance;
	};

	void Init(string level_file)
	{
		if(_instance == nullptr)
		{
			_instance = new Nivel(level_file);
			_instance->Load();
		};		
	};

	void Release()
	{
		if(_instance != nullptr)
		{
			delete _instance;
			_instance = nullptr;
		}
	};

	Tile *const GetTile(const int& globalIndex) const
	{
		if(globalIndex <= maxGlobalIndex)
		{
			return &tiles[globalIndex];
		}
		return nullptr;
	};
	
	void UpdateParallax(const Vector2f& oldCenter,const Vector2f& newCenter,const View& levelView)
	{
		for(unsigned i=0; i<capasParallax.size(); i++){
			capasParallax[i]->SetPosition(levelView);
			capasParallax[i]->Move(-(oldCenter.x-newCenter.x), oldCenter.y-newCenter.y);
		}
	};

	void CheckGameFinish()
	{
		GameWon = false;
		int sum = 0;
		for(int i=0;i<5;i++)
		{
			sum += irKeys[i];
		}

		if(sum == 5)
		{
			GameWon = true;
		}
	};

	bool& GetGameWon() const
	{
		return GameWon;
	}
};

#endif






