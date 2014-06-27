// Malaquias.h
//
//	Clase que representa a Malaqu�as
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _MALAQUIAS_H_
#define _MALAQUIAS_H_

#include <iosfwd>
#include "Monje.h"

namespace Abadia {


class Malaquias : public Monje
{
// campos
public:
	int estaMuerto;								// indica si el personaje est� muerto o muri�ndose
	int estado2;								// guarda informaci�n extra sobre el estado del personaje

protected:
	int contadorEnScriptorium;					// indica el tiempo que guillermo est� sin salir del scriptorium
	static PosicionJuego posicionesPredef[9];	// posiciones a las que puede ir el personaje seg�n el estado

// m�todos
public:
	virtual void piensa();

	// inicializaci�n y limpieza
	Malaquias(SpriteMonje *spr);
	virtual ~Malaquias();

	// cargar/salvar
	friend std::ofstream& operator<<(std::ofstream&,const Malaquias* const);
	friend std::ifstream& operator>>(std::ifstream&,Malaquias* const);

protected:
	virtual void avanzaAnimacionOMueve();
};


}

#endif	// _MALAQUIAS_H_
