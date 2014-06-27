// SpriteMonje.h
//
//	Clase que representa un sprite de un monje
//
//	Los sprites de los monjes tienen un tratamiento especial, puesto que cada
//	sprite tiene la direcci�n de los gr�ficos de la cara del monje al que
//	representa, y como todos los monjes tienen el mismo traje, una vez pintada
//	la cara del monje se pasa a pintar su traje
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _SPRITE_MONJE_H_
#define _SPRITE_MONJE_H_


#include "Sprite.h"

namespace Abadia {


class SpriteMonje : public Sprite
{
// campos
public:
	int animacionTraje;				// n�mero de la animaci�n del traje de los monjes

protected:
	static int despAnimTraje[16];	// tabla con los desplazamientos a los gr�ficos de los trajes

// m�todos
public:
	virtual void dibuja(Sprite *spr, UINT8 *bufferMezclas, int lgtudClipX, int lgtudClipY, int dist1X, int dist2X, int dist1Y, int dist2Y);

	// inicializaci�n y limpieza
	SpriteMonje();
	virtual ~SpriteMonje();
};


}

#endif	// _SPRITE_MONJE_H_
