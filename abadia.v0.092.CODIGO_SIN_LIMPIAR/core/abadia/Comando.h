// Comando.h
//
//	Clase que define el interfaz de los comandos que puede ejecutar el generador de bloques
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _COMANDO_H_
#define _COMANDO_H_


namespace Abadia {

class GeneradorPantallas;			// definido en GeneradorPantallas.h


class Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen) = 0;

	// inicializaci�n y limpieza
	Comando(){}
	virtual ~Comando(){}
};


}

#endif	// _COMANDO_H_
