// Monje.cpp
//
/////////////////////////////////////////////////////////////////////////////

#include "Monje.h"
#include "MotorGrafico.h"
#include "SpriteMonje.h"

using namespace Abadia;

/////////////////////////////////////////////////////////////////////////////
// tabla de la animaci�n de los monjes
/////////////////////////////////////////////////////////////////////////////

// CPC
/*
Personaje::DatosFotograma Monje::tablaAnimacion[8] = {
	{ 0x0000, 0x05, 0x22 },
	{ 0x0000, 0x05, 0x24 },
	{ 0x0000, 0x05, 0x22 },
	{ 0x0000, 0x05, 0x22 },
	{ 0x0000, 0x05, 0x21 },
	{ 0x0000, 0x05, 0x23 },
	{ 0x0000, 0x05, 0x21 },
	{ 0x0000, 0x05, 0x21 }
};
*/
Personaje::DatosFotograma Monje::tablaAnimacion[8] = {
	{ 0x0000, 0x05, 0x22 },
	{ 0x0000, 0x05, 0x24 },
	{ 0x0000, 0x05, 0x22 },
	{ 0x0000, 0x05, 0x22 },
	{ 0x0000, 0x05, 0x21 },
	{ 0x0000, 0x05, 0x23 },
	{ 0x0000, 0x05, 0x21 },
	{ 0x0000, 0x05, 0x21 }
};

/////////////////////////////////////////////////////////////////////////////
// inicializaci�n y limpieza
/////////////////////////////////////////////////////////////////////////////

Monje::Monje(SpriteMonje *spr) : PersonajeConIA(spr)
{
	// guarda una referencia al sprite del monje
	sprMonje = spr;

	// asigna la tabla de animaci�n del personaje
	animacion = tablaAnimacion;
	numFotogramas = 8;

	// inicialmente no tiene datos de cara
	datosCara[0] = datosCara[1] = 0x0000;
}

Monje::~Monje()
{
}

/////////////////////////////////////////////////////////////////////////////
// animaci�n del monje
/////////////////////////////////////////////////////////////////////////////

// calcula el fotograma que hay que poner al monje
Personaje::DatosFotograma * Monje::calculaFotograma()
{
	// obtiene la orientaci�n del personaje seg�n la posici�n de la c�mara
	int oriCamara = elMotorGrafico->ajustaOrientacionSegunCamara(orientacion);

	// actualiza la animaci�n del traje
	sprMonje->animacionTraje = (oriCamara << 2) | contadorAnimacion;

	// selecciona un fotograma dependiendo de la orientaci�n y de si el personaje va hacia la derecha o a la izquierda
	int numAnim = (((oriCamara + 1) & 0x02) << 1) | contadorAnimacion;

	assert(numAnim < numFotogramas);

	// modifica los datos del fotograma con la direcci�n de la cara del personaje
	animacion[numAnim].dirGfx = datosCara[(numAnim & 0x04) ? 1 : 0];

	// devuelve los datos del fotograma de la animaci�n del personaje
	return &animacion[numAnim];
}
