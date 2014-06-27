// TransformacionesCamara.h
//
//	Clases que definen las transformaciones de coordenadas seg�n la posici�n de la c�mara
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _TRANSFORMACIONES_CAMARA_H_
#define _TRANSFORMACIONES_CAMARA_H_


namespace Abadia {

class TransformacionCamara
{
// m�todos
public:
	virtual void transforma(int &x, int &y) = 0;

	// inicializaci�n y limpieza
	TransformacionCamara(){}
	virtual ~TransformacionCamara(){}
};


class Camara0 : public TransformacionCamara
{
// m�todos
public:
	virtual void transforma(int &x, int &y);
};

class Camara1 : public TransformacionCamara
{
// m�todos
public:
	virtual void transforma(int &x, int &y);
};

class Camara2 : public TransformacionCamara
{
// m�todos
public:
	virtual void transforma(int &x, int &y);
};

class Camara3 : public TransformacionCamara
{
// m�todos
public:
	virtual void transforma(int &x, int &y);
};

}

#endif	// _TRANSFORMACIONES_CAMARA_H_
