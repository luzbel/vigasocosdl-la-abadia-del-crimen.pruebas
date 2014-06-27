// FijarOrientacion.h
//
//	Clases para indicar las posiciones a las que hay que ir seg�n la orientaci�n a coger
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _FIJAR_ORIENTACION_H_
#define _FIJAR_ORIENTACION_H_


namespace Abadia {

class RejillaPantalla;			// definido en RejillaPantalla.h

class FijarOrientacion
{
// m�todos
public:
	virtual void fijaPos(RejillaPantalla *rejilla) = 0;

	// inicializaci�n y limpieza
	FijarOrientacion(){}
	virtual ~FijarOrientacion(){}

protected:
	void marcaPosiciones(RejillaPantalla *rejilla, int posX, int posY, int incX, int incY);
};


class FijaOrientacion0 : public FijarOrientacion
{
// m�todos
public:
	virtual void fijaPos(RejillaPantalla *rejilla);
};

class FijaOrientacion1 : public FijarOrientacion
{
// m�todos
public:
	virtual void fijaPos(RejillaPantalla *rejilla);
};

class FijaOrientacion2 : public FijarOrientacion
{
// m�todos
public:
	virtual void fijaPos(RejillaPantalla *rejilla);
};

class FijaOrientacion3 : public FijarOrientacion
{
// m�todos
public:
	virtual void fijaPos(RejillaPantalla *rejilla);
};

}

#endif	// _FIJAR_ORIENTACION_H_
