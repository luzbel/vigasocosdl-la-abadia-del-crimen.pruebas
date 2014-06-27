// Severino.h
//
//	Clase que representa a Severino
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _SEVERINO_H_
#define _SEVERINO_H_


#include "Monje.h"

namespace Abadia {


class Severino : public Monje
{
// campos
public:
	bool estaVivo;								// indica si el personaje est� vivo

protected:
	static PosicionJuego posicionesPredef[4];	// posiciones a las que puede ir el personaje seg�n el estado

// m�todos
public:
	virtual void piensa();

	// inicializaci�n y limpieza
	Severino(SpriteMonje *spr);
	virtual ~Severino();
};


}

#endif	// _SEVERINO_H_
