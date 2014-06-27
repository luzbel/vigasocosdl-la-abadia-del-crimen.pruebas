// Comandos.h
//
//	Clase con los comandos que puede ejecutar el generador de pantallas para crear los bloques
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _COMANDOS_H_
#define _COMANDOS_H_

#include "Comando.h"


namespace Abadia {

class GeneradorPantallas;			// definido en GeneradorPantallas.h


class ChangePC : public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class Call: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class CallPreserve: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class WhileParam1: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class WhileParam2: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class EndWhile: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class EndBlock: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class IncParam1: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class IncParam2: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class DecParam1: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class DecParam2: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class IncTilePosX: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class DecTilePosX: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class IncTilePosY: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class DecTilePosY: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class UpdateTilePosX: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class UpdateTilePosY: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class PushTilePos: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class PopTilePos: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class DrawTileDecY: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class DrawTileIncX: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class DrawTileDecX: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class FlipX: public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

class UpdateReg : public Comando
{
// m�todos
public:
	virtual bool ejecutar(GeneradorPantallas *gen);
};

}

#endif	// _COMANDOS_H_
