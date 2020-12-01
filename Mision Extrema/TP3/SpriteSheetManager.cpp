#include "SpriteSheetManager.h"
#include "TextureManager.h"

void SpriteSheetManager::Load(string filename, unsigned nCols, unsigned nRows,unsigned LevelRows,unsigned LevelCols){
	
	// cargamos 
	const sf::Image &sheet = TextureManager::GetInstance().GetTexture(filename);
	
	cols = nCols;
	rows = nRows;

	tiles.clear();
	tiles.resize(0);

	// segun la cantidad de filas/columnas calculamos el tamanio
	// de las subimagenes
	
	subimgw = sheet.GetWidth()/nCols;
	subimgh = sheet.GetHeight()/nRows;
	
	unsigned i, j;
	
	for(i=0; i<rows; i++){
		for(j=0; j<cols; j++){
			// cargamos r con los datos del rectangulo
			r.Left=subimgw*j;
			r.Top=subimgh*i;
			r.Right=r.Left+subimgw;
			r.Bottom=r.Top+subimgh;		
			tiles.push_back(r);
		}
	}
	SetImage(sheet);	
}

const IntRect& SpriteSheetManager::GetRect(int index)
{
	return tiles[index];
}

void SpriteSheetManager::GetImage(unsigned i,FloatRect &rect){
	SetSubRect(tiles[i]);	
	SetX(rect.Left);
	SetY(rect.Top);	
}

const Image& SpriteSheetManager::InternalImage()
{	
	return *Sprite::GetImage();
}

void SpriteSheetManager::SelectImage(const int &index,const float &x,const float &y)
{	
	SetSubRect(tiles[index]);
	SetX(x);
	SetY(y);		
}