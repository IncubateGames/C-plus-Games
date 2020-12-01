#ifndef SPRITESHEETMANAGER_H
#define SPRITESHEETMANAGER_H
#include <vector>
#include <SFML/Graphics/Sprite.hpp>

using namespace std;
using namespace sf;

class SpriteSheetManager : public Sprite {
private:
	unsigned subimgw, subimgh;
	vector<sf::IntRect> tiles;	
	sf::IntRect r;
	unsigned cols, rows; 
public:
	// carga un sprite sheet
	void Load(string filename, unsigned nCols, unsigned nRows=1,unsigned LevelRows=1,unsigned LevelCols=1);
	// devuelve la imagen en la posicion i
	void GetImage(unsigned i,FloatRect& rect);
	const Image& InternalImage();
	const IntRect& GetRect(int index);	
	void SelectImage(const int &index,const float &x,const float &y);
};

#endif


