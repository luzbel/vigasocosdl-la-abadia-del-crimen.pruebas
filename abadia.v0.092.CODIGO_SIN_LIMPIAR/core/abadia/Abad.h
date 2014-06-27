// Abad.h
//
//	Clase que representa al abad
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _ABAD_H_
#define _ABAD_H_


#include "Monje.h"

namespace Abadia {


class Abad : public Monje
{
// campos
public:
	int contador;								// contador usado en varias situaciones
	int numFrase;								// indica la frase que debe decir en varias situaciones
	int guillermoBienColocado;					// indica si guillermo est� bien colocado en misa o en el refectorio
	int lleganLosMonjes;						// indica si los monjes han llegado a misa o al refectorio
	bool guillermoHaCogidoElPergamino;			// indica si guillermo ha cogido el pergamino cuando lo vigilaba berengario

protected:
	static PosicionJuego posicionesPredef[10];	// posiciones a las que puede ir el personaje seg�n el estado
	static int monjesIglesiaEnPrima[7];			// indica los personajes que deben estar en la misa de prima seg�n el d�a
	static int frasesIglesiaEnPrima[7];			// indica los frases que se dicen despu�s de la misa de prima seg�n el d�a
	static int monjesEnRefectorio[7];			// indica los personajes que deben estar en el refectorio de prima seg�n el d�a
	static int monjesIglesiaEnVisperas[7];		// indica los personajes que deben estar en la misa de v�speras seg�n el d�a

// m�todos
public:
	virtual void piensa();

	// inicializaci�n y limpieza
	Abad(SpriteMonje *spr);
	virtual ~Abad();

protected:
	void recriminaAGuillermo();

	void avisaAGuillermoOPasea();
	void paseaPorLaAbadia();
	void diceFraseAGuillermoEnTercia();

	void esperaParaComenzarActo();
	void compruebaPosGuillermoEnIglesia();
	void compruebaPosGuillermoEnRefectorio();
	void compruebaPosGuillermo(int posX, int posY, int orientacion);
	void compruebaPosMonjesEnIglesiaEnVisperas(int numDia);
	void compruebaPosMonjesEnIglesiaEnPrima(int numDia);
	void compruebaPosMonjesEnRefectorio(int numDia);
};


}

#endif	// _ABAD_H_
