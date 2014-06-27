// MezcladorSprites.h
//
//	Clase que representa al mezclador de los sprites. Esta clase se encarga de actualizar la
//	pantalla generada mezclando los sprites entre las capas de tiles seg�n la profundidad.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _MEZCLADOR_SPRITES_H_
#define _MEZCLADOR_SPRITES_H_


class CPC6128;						// definido en CPC6128.h

namespace Abadia {

class GeneradorPantallas;			// definido en GeneradorPantallas.h
class Sprite;						// definido en Sprite.h

class MezcladorSprites
{
// campos
public:
	GeneradorPantallas *genPant;	// generador de las pantallas

	UINT8 *bufferMezclas;			// buffer para mezclar los sprites
	int lgtudBufferMezclas;			// longitud del buffer de mezclas
	int sgtePosBuffer;				// siguiente posici�n libre del buffer de mezclas

	int listaSprites[16];			// lista de sprites a dibujar en un momento dado
	int numSprites;					// n�mero de sprites visibles en la pantalla actual
	int numSpritesRedib;			// n�mero de sprites que hay que redibujar
	int posSprOrigen;				// posici�n de la lista del sprite original
	int posSprActual;				// posici�n de la lista del sprite que se est� procesando

	int bufTilesPosX, bufTilesPosY;	// variables auxiliares

protected:
	UINT8 *roms;					// puntero a los datos del juego
	CPC6128	*cpc6128;				// objeto de ayuda para realizar operaciones gr�ficas del cpc6128

// m�todos
public:
	// dibuja los sprites
	void mezclaSprites(Sprite **sprites, int num);

	// inicializaci�n y limpieza
	MezcladorSprites(GeneradorPantallas *generador, UINT8 *buffer, int lgtudBuffer);
	~MezcladorSprites();

protected:
	// mezcla de tiles al buffer de sprites
	void combinaTile(Sprite *spr, int tile, int despBufSprites);
	void dibujaTilesEntreProfundidades(Sprite *spr, int profMinX, int profMinY, int profMaxX, int profMaxY, int desp, bool ultimaPasada);

	// postprocesado de los sprites
	void postProcesaSprites(Sprite **sprites, int num);
	void vuelcaBufferAPantalla(Sprite *spr);

	// m�todos de ayuda
	bool recortaSprite(int posVis, int lgtudVis, int pos, int lgtud, int &lgtudClip, int &dist1, int &dist2);
	bool estaEnBufferTiles(int bufPosX, int bufPosY);
};


}

#endif	// _MEZCLADOR_SPRITES_H_
