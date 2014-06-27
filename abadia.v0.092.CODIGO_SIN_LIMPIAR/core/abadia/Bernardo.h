// Bernardo.h
//
//	Clase que representa a Bernardo Gui
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _BERNARDO_H_
#define _BERNARDO_H_


#include "Monje.h"

namespace Abadia {


class Bernardo : public Monje
{
// campos
public:
	bool estaEnLaAbadia;						// indica si est� en la abad�a o no

protected:
	static PosicionJuego posicionesPredef[5];	// posiciones a las que puede ir el personaje seg�n el estado

// m�todos
public:
	virtual void piensa();

	// inicializaci�n y limpieza
	Bernardo(SpriteMonje *spr);
	virtual ~Bernardo();
};


}

#endif	// _BERNARDO_H_
