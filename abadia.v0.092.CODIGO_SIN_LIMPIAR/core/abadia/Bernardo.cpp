// Bernardo.cpp
//
/////////////////////////////////////////////////////////////////////////////

#include "Abad.h"
#include "Bernardo.h"
#include "BuscadorRutas.h"
#include "GestorFrases.h"
#include "Guillermo.h"
#include "Juego.h"
#include "Logica.h"
#include "Marcador.h"

using namespace Abadia;

/////////////////////////////////////////////////////////////////////////////
// posiciones a las que puede ir el personaje seg�n el estado
/////////////////////////////////////////////////////////////////////////////

PosicionJuego Bernardo::posicionesPredef[5] = {
	PosicionJuego(ABAJO, 0x8c, 0x48, 0x02),		// posici�n en la iglesia
	PosicionJuego(ARRIBA, 0x32, 0x35, 0x02),	// posici�n en el refectorio
	PosicionJuego(IZQUIERDA, 0x3d, 0x5c, 0x0f),	// posici�n de su mesa en el scriptorium
	PosicionJuego(DERECHA, 0xbc, 0x15, 0x02),	// celda de los monjes
	PosicionJuego(ARRIBA, 0x88, 0xa8, 0x00)		// salida de la abad�a
};

/////////////////////////////////////////////////////////////////////////////
// inicializaci�n y limpieza
/////////////////////////////////////////////////////////////////////////////

Bernardo::Bernardo(SpriteMonje *spr) : Monje(spr)
{
	// coloca los datos de la cara de bernardo
	// CPC
	/*
	datosCara[0] = 0xb293;
	datosCara[1] = 0xb293 + 0x32;
	*/
	// VGA
        datosCara[0] = 68508;
        datosCara[1] = 68508+0x32*4;

	mascarasPuertasBusqueda = 0x3f;

	// asigna las posiciones predefinidas
	posiciones = posicionesPredef;
}

Bernardo::~Bernardo()
{
}

/////////////////////////////////////////////////////////////////////////////
// comportamiento
/////////////////////////////////////////////////////////////////////////////

// Los estados en los que puede estar bernardo son:
//		0x00 -> estado incial
//		0x07 -> estado en el que persigue a guillermo hasta quitarle el pergamino
//		0x14 -> estado en el que ya no tiene nada que hacer, por lo que tan s�lo se pasea por la abad�a
void Bernardo::piensa()
{
	// si bernardo no est� en la abad�a, sale
	if (!estaEnLaAbadia){
		elBuscadorDeRutas->seBuscaRuta = false;

		return;
	}

	// si es sexta, va al comedor
	if (laLogica->momentoDia == SEXTA){
		aDondeVa = 1;

		return;
	}

	// si es prima, va a la iglesia
	if (laLogica->momentoDia == PRIMA){
		aDondeVa = 0;

		return;
	}

	// al quinto d�a, abandona la abad�a
	if (laLogica->dia == 5){
		// si llega a la salida de las escaleras, se va de la abad�a
		if (aDondeHaLlegado == 4){
			estaEnLaAbadia = false;
			posX = posY = altura = 0;
		}

		// se va de la abad�a
		aDondeVa = 4;
	}

	// en completas o por la noche, se va a la celda de los monjes
	if ((laLogica->momentoDia == COMPLETAS) || (laLogica->momentoDia == NOCHE)){
		aDondeVa = 3;

		return;
	}

	// si es v�speras, va a la iglesia
	if (laLogica->momentoDia == VISPERAS){
		aDondeVa = 0;

		return;
	}

	// si ya no tiene nada que hacer y ha llegado a su destino, se mueve a una posici�n aleatoria
	if (estado == 0x14){
		if (aDondeHaLlegado == aDondeVa){
			aDondeVa = laLogica->numeroAleatorio & 0x03;
		}
		
		return;
	}

	// si es el cuarto d�a
	if (laLogica->dia == 4){
		// si va a por el abad y ya le ha dado el pergamino
		if ((aDondeVa == POS_ABAD) && ((laLogica->abad->objetos & PERGAMINO) == PERGAMINO)){
			// indica que ya no tiene nada que hacer
			estado = 0x14;

			// va al refectorio
			aDondeVa = 1;

			// cambia el estado del abad para que deje el pergamino en su celda
			laLogica->abad->estado = 0x15;

			return;
		}
	}

	// si bernardo tiene el pergamino, va a d�rselo al abad
	if ((objetos & PERGAMINO) == PERGAMINO){
		aDondeVa = POS_ABAD;

		// cambia la m�scara de los objetos para no volver a coger el pergamino
		mascaraObjetos = 0;

		return;
	}

	// si el pergamino est� a buen recaudo o el abad va a echar a guillermo, bernardo ya no tiene nada que hacer
	if (laLogica->pergaminoGuardado || ((laLogica->abad->objetos & PERGAMINO) == PERGAMINO) || (laLogica->abad->estado == 0x0b)){
		// va al scriptorium
		aDondeVa = 2;
		estado = 0x14;

		return;
	}

	laLogica->pergaminoGuardado = false;

	// deshabilita el contador para que avance el momento del d�a de forma autom�tica
	laLogica->duracionMomentoDia = 0;

	// si guillermo tiene el pergamino
	if ((laLogica->guillermo->objetos & PERGAMINO) == PERGAMINO){
		// si est� persiguiendo a guillermo
		if (estado == 7){
			aDondeVa = POS_GUILLERMO;

			// si est� cerca de guillermo, le exige el manuscrito y decrementa su vida
			if (estaCerca(laLogica->guillermo)){
				if (!elGestorFrases->mostrandoFrase){
					// pone en el marcador la frase DADME EL MANUSCRITO, FRAY GUILLERMO
					elGestorFrases->muestraFrase(0x05);

					elMarcador->decrementaObsequium(2);
				}
			}
		} else if (estaCerca(laLogica->guillermo)){
			// si est� cerca de guillermo, se va a la celda de los monjes
			aDondeVa = 3;
		} else {
			// cambia al estado de seguir a guillermo
			estado = 7;
		}
	} else {
		// si guillermo no tiene el pergamino, va hacia donde est� el pergamino
		aDondeVa = POS_PERGAMINO;
	}
}
