// Objeto.cpp
//
/////////////////////////////////////////////////////////////////////////////

#include "MotorGrafico.h"
#include "Personaje.h"
#include "Objeto.h"
#include "Sprite.h"

using namespace Abadia;

/////////////////////////////////////////////////////////////////////////////
// inicializaci�n y limpieza
/////////////////////////////////////////////////////////////////////////////

Objeto::Objeto(Sprite *spr) : EntidadJuego(spr)
{
	seEstaCogiendo = false;
	seHaCogido = false;
	personaje = 0;
}

Objeto::~Objeto()
{
}

/////////////////////////////////////////////////////////////////////////////
// actualizaci�n del entorno cuando un objeto es visible en la pantalla actual
/////////////////////////////////////////////////////////////////////////////

// actualiza la posici�n del sprite dependiendo de su posici�n con respecto a la c�mara
void Objeto::notificaVisibleEnPantalla(int posXPant, int posYPant, int profundidad)
{
	// si el objeto no se ha cogido
	if (!seHaCogido){
		// marca el sprite para dibujar
		sprite->haCambiado = true;
		sprite->esVisible = true;
		sprite->profundidad = profundidad;

		// ajusta la posici�n del sprite (-8, -8)
		sprite->posXPant = posXPant - 2;
		sprite->posYPant = posYPant - 8;
	}
}

/////////////////////////////////////////////////////////////////////////////
// m�todos relacionados con coger/dejar los objetos
/////////////////////////////////////////////////////////////////////////////

// comprueba si el personaje puede coger el objeto
bool Objeto::seHaCogidoPor(Personaje *pers, int mascara)
{
	// si el objeto se est� cogiendo o dejando, no puede ser cogido
	if (seEstaCogiendo) return false;

	// guarda la posici�n del objeto
	int posXObj = posX;
	int posYObj = posY;
	int alturaObj = altura;

	// si el objeto est� cogido, su posici�n viene dada por la del personaje que lo tiene
	if (seHaCogido){
		// si el personaje no puede quitar objetos, sale
		if (!pers->puedeQuitarObjetos) return false;

		// obtiene la posici�n del personaje
		posXObj = personaje->posX;
		posYObj = personaje->posY;
		alturaObj = personaje->altura;
	}

	// comprueba si el personaje est� en una posici�n que permita coger el objeto
	int difAltura = alturaObj - pers->altura;
	if ((difAltura < 0) || (difAltura >= 5)) return false;

	int posXPers = pers->posX + 2*elMotorGrafico->tablaDespOri[pers->orientacion][0];
	if (posXObj != posXPers) return false;

	int posYPers = pers->posY + 2*elMotorGrafico->tablaDespOri[pers->orientacion][1];
	if (posYObj != posYPers) return false;

	// si el objeto est� cogido por un personaje, se lo quita
	if (seHaCogido){
		personaje->objetos = personaje->objetos ^ mascara;
	}

	// si el sprite del objeto es visible, indica que va a desaparecer
	if (sprite->esVisible){
		sprite->haCambiado = true;
		sprite->desaparece = true;
	}

	// guarda el personaje que tiene el objeto, indica que el objeto se ha cogido e inicia el contador
	personaje = pers;
	seHaCogido = true;
	seEstaCogiendo = true;
	pers->contadorObjetos = 0x10;
	pers->objetos = pers->objetos | mascara;

	return true;
}

// deja el objeto que ten�a el personaje en la posici�n indicada
void Objeto::dejar(Personaje *pers, int mascara, int posXObj, int posYObj, int alturaObj)
{
	// guarda la posici�n y orientaci�n del objeto e indica que ya no est� cogido
	posX = posXObj;
	posY = posYObj;
	altura = alturaObj;
	orientacion = pers->orientacion ^ 0x02;
	seHaCogido = false;
	personaje = 0;
	
	// inicia el contador para coger/dejar objetos y le quita el objeto al personaje
	pers->contadorObjetos = 0x10;
	pers->objetos = pers->objetos & (~mascara);

	// salta a la rutina de redibujado de objetos para redibujar solo el objeto que se deja
	// actualiza la posici�n del sprite seg�n la c�mara
	int posXPant, posYPant, sprPosY;

	if (elMotorGrafico->actualizaCoordCamara(this, posXPant, posYPant, sprPosY) != -1){
		notificaVisibleEnPantalla(posXPant, posYPant, sprPosY);
	} else {
		sprite->esVisible = false;
	}

	sprite->oldPosXPant = sprite->posXPant;
	sprite->oldPosYPant = sprite->posYPant;
}
