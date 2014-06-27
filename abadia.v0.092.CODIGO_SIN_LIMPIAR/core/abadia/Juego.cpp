///Juego.cpp
//
/////////////////////////////////////////////////////////////////////////////

#include <string>

#include "../systems/cpc6128.h"
#include "../IAudioPlugin.h"
#include "../InputHandler.h"
#include "../IPalette.h"
#include "../TimingHandler.h"
#include "../Vigasoco.h"

#include "Abad.h"
#include "Adso.h"
#include "Berengario.h"
#include "Bernardo.h"
#include "BuscadorRutas.h"
#include "Controles.h"
#include "GestorFrases.h"
#include "Guillermo.h"
#include "InfoJuego.h"
#include "Jorge.h"
#include "Juego.h"
#include "Logica.h"
#include "Malaquias.h"
#include "Marcador.h"
#include "Monje.h"
#include "MotorGrafico.h"
#include "Objeto.h"
#include "Paleta.h"
#include "Pergamino.h"
#include "Personaje.h"
#include "PersonajeConIA.h"
#include "Puerta.h"
#include "RejillaPantalla.h"
#include "Severino.h"
#include "Sprite.h"
#include "SpriteLuz.h"
#include "SpriteMonje.h"

#include "Serializar.h"

#include "sonidos.h"

// memcpy
#include <string.h>

using namespace Abadia;

#ifdef _EE
//const char *Juego::savefile = "host0:/abadia.save";
const char *Juego::savefile = "mc0:ABADIA/abadia.save";
#else
#ifdef __native_client__
const char *Juego::savefile = "/save/abadia.save";
#else
const char *Juego::savefile = "abadia.save";
#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// inicialización y limpieza
/////////////////////////////////////////////////////////////////////////////

Juego::Juego(UINT8 *romData, CPC6128 *cpc)
{
	idioma=0; // 0 español
	GraficosCPC=false;
	// apunta a los datos del juego, pero saltándose la de la presentación
	roms = romData + 0x4000;

	cpc6128 = cpc;

	// inicia los sprites del juego
	for (int i = 0; i < numSprites; i++){
		sprites[i] = 0;
	}

	// inicia los personajes del juego
	for (int i = 0; i < numPersonajes; i++){
		personajes[i] = 0;
	}

	// inicia las puertas del juego
	for (int i = 0; i < numPuertas; i++){
		puertas[i] = 0;
	}

	// inicia los objetos del juego
	for (int i = 0; i < numObjetos; i++){
		objetos[i] = 0;
	}

	timer = 0;

	// crea los objetos principales que usará el juego
	//paleta = new Paleta();
	paleta = new Paleta(romData+0x24000-1); // le pasamos los datos de la paleta VGA
	pergamino = new Pergamino();
	motor = new MotorGrafico(buffer, 8192);
	marcador = new Marcador();
	logica = new Logica(roms, buffer, 8192); 
	infoJuego = new InfoJuego();
	controles = new Controles();

	pausa = false;
	modoInformacion = false;
}

Juego::~Juego()
{
	// borra los sprites del juego
	for (int i = 0; i < numSprites; i++){
		delete sprites[i];
	}

	// borra los personajes del juego
	for (int i = 0; i < numPersonajes; i++){
		delete personajes[i];
	}

	// borra las puertas del juego
	for (int i = 0; i < numPuertas; i++){
		delete puertas[i];
	}

	// borra los objetos del juego
	for (int i = 0; i < numObjetos; i++){
		delete objetos[i];
	}

	delete infoJuego;
	delete logica;
	delete marcador;
	delete motor;
	delete pergamino;
	delete paleta;

	delete controles;
}

void Juego::ReiniciaPantalla(void)
{
	// limpia el área de juego y dibuja el marcador
	// CPC limpiaAreaJuego(0);
	limpiaAreaJuego(12); // el 0 es el cyan en CPC, no se cual poner en VGA
	// pongo el 12 que es un amarillo cantoso, para comparar con Abadia32
	marcador->dibujaMarcador();

	// inicia el contador de la interrupción
	contadorInterrupcion = 0;

	// pone una posición de pantalla inválida para que se redibuje la pantalla
	motor->posXPantalla = motor->posYPantalla = -1;

	// dibuja los objetos que tiene guillermo en el marcador
	marcador->dibujaObjetos(personajes[0]->objetos, 0xff);

	// inicia el marcador (día y momento del día, obsequium y el espacio de las frases)
	marcador->muestraDiaYMomentoDia();
	marcador->decrementaObsequium(0);
	marcador->limpiaAreaFrases();
}

/////////////////////////////////////////////////////////////////////////////
// método principal del juego
/////////////////////////////////////////////////////////////////////////////

void Juego::run()
{
	// obtiene los recursos para el juego
	timer = VigasocoMain->getTimingHandler();
	controles->init(VigasocoMain->getInputHandler());
	audio_plugin = VigasocoMain->getAudioPlugin();

	// muestra la imagen de presentación

	muestraPresentacion();

	// pruebas menu elegir idioma
	// TODO sacar a una funcion
	// limpia el área que ocupa el marcador
	limpiaAreaJuego(0); // VGA
	marcador->limpiaAreaMarcador();
		// TODO poner en su idioma, no solo en español
	marcador->imprimeFrase("0 CASTELLANO", 88, 32, 4, 0);
	marcador->imprimeFrase("1 ENGLISH", 88, 48, 4, 0);
	marcador->imprimeFrase("2 PORTUGUÉS BRASIL", 88, 64, 4, 0); //TODO falta el gorrito en la e
	marcador->imprimeFrase("3 CATALÁN", 88, 80, 4, 0);
	marcador->imprimeFrase("4 GALLEGO", 88, 96, 4, 0);
	marcador->imprimeFrase("5 ITALIANO", 88, 112, 4, 0);
	marcador->imprimeFrase("6 FINES", 88, 128, 4, 0);
	marcador->imprimeFrase("7 PORTUGUES", 88, 144, 4, 0);


// TODO
// mejorar esto para no depender de teclas
// y que asi tambien valga para dreamcast, ps2, y otras consolas
// sin teclado
// TODO: hacer que no salga solo aqui
// deberia ser un menu emergente, donde moverse
// y cambiar idioma, pausa, cargar y grabar...	
	{
		bool salir=false;
		while(salir==false) {	
			losControles->actualizaEstado();
			if (losControles->estaSiendoPulsado(KEYBOARD_0))
			{
				salir=true;
				idioma=0;
			}
			if (losControles->estaSiendoPulsado(KEYBOARD_1))
			{
				salir=true;
				idioma=1;
			}
			if (losControles->estaSiendoPulsado(KEYBOARD_2))
			{
				salir=true;
				idioma=2;
			}
			if (losControles->estaSiendoPulsado(KEYBOARD_3))
			{
				salir=true;
				idioma=3;
			}
			if (losControles->estaSiendoPulsado(KEYBOARD_4))
			{
				salir=true;
				idioma=4;
			}
			if (losControles->estaSiendoPulsado(KEYBOARD_5))
			{
				salir=true;
				idioma=5;
			}
			if (losControles->estaSiendoPulsado(KEYBOARD_6))
			{
				salir=true;
				idioma=6;
			}
			if (losControles->estaSiendoPulsado(KEYBOARD_7))
			{
				salir=true;
				idioma=7;
			}
		}
	
	}

	// muestra el pergamino de presentación

	muestraIntroduccion();

	// crea las entidades del juego (sprites, personajes, puertas y objetos)
	creaEntidadesJuego();


	// genera los gráficos flipeados en x de las entidades que lo necesiten
	generaGraficosFlipeados();


	// inicialmente la cámara sigue a guillermo
	motor->personaje = personajes[0];


	// inicia el objeto que muestra información interna del juego
	infoJuego->inicia();


	// limpia el área que ocupa el marcador
	marcador->limpiaAreaMarcador();


	// obtiene las direcciones de los datos relativos a la habitación del espejo
	logica->despHabitacionEspejo();


	// aquí ya se ha completado la inicialización de datos para el juego
	// ahora realiza la inicialización para poder empezar a jugar una partida
	while (true){

		// inicia la lógica del juego
		logica->inicia();


despues_de_cargar_o_iniciar:
		ReiniciaPantalla();


		while (true){	// el bucle principal del juego empieza aquí
			// actualiza el estado de los controles
			controles->actualizaEstado();

			// obtiene el contador de la animación de guillermo para saber si se generan caminos en esta iteración
			elBuscadorDeRutas->contadorAnimGuillermo = laLogica->guillermo->contadorAnimacion;

			// comprueba si se debe abrir el espejo
			logica->compruebaAbreEspejo();


			// comprueba si se ha pulsado la pausa
			compruebaPausa();


			//comprueba si se intenta cargar/grabar la partida
			compruebaSave();


			if ( compruebaLoad() ) goto despues_de_cargar_o_iniciar;


			// comprueba si se quieren cambiar de graficos 
			// CPC a VGA o viceversa
			compruebaCambioCPC_VGA();


			// actualiza las variables relacionadas con el paso del tiempo
			logica->actualizaVariablesDeTiempo();


			// si guillermo ha muerto, empieza una partida
			if (muestraPantallaFinInvestigacion()){
				break;
			}


			// comprueba si guillermo lee el libro, y si lo hace sin guantes, lo mata
			logica->compruebaLecturaLibro();


			// comprueba si hay que avanzar la parte del momento del día en el marcador
			marcador->realizaScrollMomentoDia();


			// comprueba si hay que ejecutar las acciones programadas según el momento del día
			logica->ejecutaAccionesMomentoDia();


			// comprueba si hay opciones de que la cámara siga a otro personaje y calcula los bonus obtenidos
			logica->compruebaBonusYCambiosDeCamara();


			// comprueba si se ha cambiado de pantalla y actúa en consecuencia
			motor->compruebaCambioPantalla();


			// comprueba si los personajes cogen o dejan algún objeto
			logica->compruebaCogerDejarObjetos();


			// comprueba si se abre o se cierra alguna puerta
			logica->compruebaAbrirCerrarPuertas();


			// ejecuta la lógica de los personajes
			for (int i = 0; i < numPersonajes; i++){
				personajes[i]->run();

			}

			// indica que en esta iteración no se ha generado ningún camino
			logica->buscRutas->generadoCamino = false;

			// actualiza el sprite de la luz para que se mueva siguiendo a adso
			actualizaLuz();


			// si guillermo o adso están frente al espejo, muestra su reflejo
			laLogica->realizaReflejoEspejo();


			// si está en modo información, muestra la información interna del juego
/* codigo original
			if (modoInformacion){
				infoJuego->muestraInfo();
			}


			// dibuja la pantalla si fuera necesario
			motor->dibujaPantalla();

			// dibuja los sprites visibles que hayan cambiado
			motor->dibujaSprites();
*/
// prueba a que no quede tan feo con transparencia que no se elimina
			if (cambioModoInformacion && modoInformacion ) {
				limpiaAreaJuego(12);
				cambioModoInformacion=false;
			}

			if (cambioModoInformacion && !modoInformacion ) {
				// intento de repintar la pantalla
				// falta comprobar si no da problemas
				// con puertas, objetos, pantalla con
				// iluminacion y lampara, etc
				// ya que MotorGrafico::compruebaCambioPantalla()
				// parece mas complejo que esto
				// pues si ,a veces se queda sin pintar
				// los sprites
				//motor->hayQueRedibujar=true;

				//pruebo a forzar el cambio
				limpiaAreaJuego(12);
			 	motor->compruebaCambioPantalla(true);	
				cambioModoInformacion=false;
			}

			if (modoInformacion){
				infoJuego->muestraInfo();
			} else {

			// dibuja la pantalla si fuera necesario
			motor->dibujaPantalla();

			// dibuja los sprites visibles que hayan cambiado
			motor->dibujaSprites();
			}

			// espera un poco para actualizar el estado del juego
			while (contadorInterrupcion < 0x24){
				timer->sleep(5);
			}

			if (laLogica->guillermo->contadorAnimacion==1)
			{
				audio_plugin->Play(SONIDOS::Pasos);
			}

			// reinicia el contador de la interrupción
			contadorInterrupcion = 0;
		}
	}
}

// limpia el área de juego de color que se le pasa y los bordes de negro
void Juego::limpiaAreaJuego(int color)
{
	/* CPC
	cpc6128->fillMode1Rect(0, 0, 32, 160, 3);
	cpc6128->fillMode1Rect(32, 0, 256, 160, color);
	cpc6128->fillMode1Rect(32 + 256, 0, 32, 160, 3);
	*/
	// VGA
	cpc6128->fillMode1Rect(0, 0, 32, 160, 0);
	cpc6128->fillMode1Rect(32, 0, 256, 160, color);
	cpc6128->fillMode1Rect(32 + 256, 0, 32, 160, 0);
}


// flipea respecto a x todos los gráficos del juego que lo necesiten
void Juego::generaGraficosFlipeados()
{
	generaGraficosFlipeadosVGA(); //  VGA

	UINT8 tablaFlipX[256];

	// inicia la tabla para flipear los gráficos
	for (int i = 0; i < 256; i++){
		// extrae los pixels
		int pixel0 = cpc6128->unpackPixelMode1(i, 0);
		int pixel1 = cpc6128->unpackPixelMode1(i, 1);
		int pixel2 = cpc6128->unpackPixelMode1(i, 2);
		int pixel3 = cpc6128->unpackPixelMode1(i, 3);

		int data = 0;

		// combina los pixels en orden inverso
		data = cpc6128->packPixelMode1(data, 0, pixel3);
		data = cpc6128->packPixelMode1(data, 1, pixel2);
		data = cpc6128->packPixelMode1(data, 2, pixel1);
		data = cpc6128->packPixelMode1(data, 3, pixel0);

		// guarda el resultado
		tablaFlipX[i] = data;
	}

	// genera los gráficos de las animaciones de guillermo flipeados respecto a x
	flipeaGraficos(tablaFlipX, &roms[0x0a300], &roms[0x16300], 5, 0x366);
	flipeaGraficos(tablaFlipX, &roms[0x0a666], &roms[0x16666], 4, 0x084);

	// genera los gráficos de las animaciones de adso flipeados respecto a x
	flipeaGraficos(tablaFlipX, &roms[0x0a6ea], &roms[0x166ea], 5, 0x1db);
	flipeaGraficos(tablaFlipX, &roms[0x0a8c5], &roms[0x168c5], 4, 0x168);

	// genera los gráficos de los trajes de los monjes flipeados respecto a x
	flipeaGraficos(tablaFlipX, &roms[0x0ab59], &roms[0x16b59], 5, 0x2d5);

	// genera los gráficos de las caras de los monjes flipeados respecto a x
	flipeaGraficos(tablaFlipX, &roms[0x0b103], &roms[0x17103], 5, 0x2bc);

	// genera los gráficos de las puertas flipeados respecto a x
	flipeaGraficos(tablaFlipX, &roms[0x0aa49], &roms[0x16a49], 6, 0x0f0);
}

// flipea respecto a x todos los gráficos del juego que lo necesiten
void Juego::generaGraficosFlipeadosVGA()
{
	UINT8 *romsVGA = &roms[0x24000-1-0x4000];
	UINT8 *romsVGAFlip = &roms[0x24000 + 174065 -1 - 0x4000];
	int dest = 0;
	int size = 57240-53760;
	// genera los gráficos de las animaciones de guillermo flipeados respecto a x
	flipeaGraficosVGA(&romsVGA[53760], &romsVGAFlip[dest], 5*4, size);
	dest += size;
	size = 57768-57240; // ok, coincide con los 0x084 que se pasaba para CPC * 4 ya que en VGA cada pixel es un byte , y no 4 pixel en un byte
	flipeaGraficosVGA(&romsVGA[57240], &romsVGAFlip[dest], 4*4, size );
	dest += size;

	// genera los gráficos de las animaciones de adso flipeados respecto a x
	size = 59668 - 57768; // ok, coincide con 0x1db *4 
	flipeaGraficosVGA(&romsVGA[57768], &romsVGAFlip[dest], 5*4, size);
	dest += size;
	size = 61108 - 59668; // ok , es 0x168 * 4
	flipeaGraficosVGA(&romsVGA[59668], &romsVGAFlip[dest], 4*4, size);

	// genera los gráficos de los trajes de los monjes flipeados respecto a x
	dest += size;
	size = 64008 - 61108; // ok , es 0x2d5*4
	flipeaGraficosVGA(&romsVGA[61108], &romsVGAFlip[dest], 5*4, size);

	// genera los gráficos de las caras de los monjes flipeados respecto a x
	dest += size;
	dest+=2900; // TODO: cambio temporal, para que la distancia entre los graficos y su homologo flipeados sea siempre 120305
// a ver si hay suerte, y en estos 2900 bytes lo que estan son los graficos de las puertas que son los que nos faltan !!! -> pues no, estan justo detras de las caras
	size = 69708 - 66908; // OK , es 0x2bc*4
	flipeaGraficosVGA(&romsVGA[66908], &romsVGAFlip[dest], 5*4, size);

	// genera los gráficos de las puertas flipeados respecto a x
	dest += size;
	size = 24*40; // OK , es 0x0f0 * 4
	flipeaGraficosVGA(&romsVGA[69708], &romsVGAFlip[dest], 6*4, size);
}

// copia los gráficos de origen en el destino y los flipea
void Juego::flipeaGraficos(UINT8 *tablaFlip, UINT8 *src, UINT8 *dest, int ancho, int bytes)
{
	// copia los gráficos del origen al destino
	memcpy(dest, src, bytes);

	// calcula las variables que controlan el bucle
	int numLineas = bytes/ancho;
	int numIntercambios = (ancho + 1)/2;

	// recorre todas las líneas que forman el gráfico
	for (int j = 0; j < numLineas; j++){
		UINT8 *ptr1 = dest;
		UINT8 *ptr2 = ptr1 + ancho - 1;

		// realiza los intercambios necesarios para flipear esta línea
		for (int i = 0; i < numIntercambios; i++){
			UINT8 aux = *ptr1;
			*ptr1 = tablaFlip[*ptr2];
			*ptr2 = tablaFlip[aux];

			ptr1++;
			ptr2--;
		}

		// pasa a la siguiente línea
		dest = dest + ancho;
	}
}

// copia los gráficos de origen en el destino y los flipea
void Juego::flipeaGraficosVGA(UINT8 *src, UINT8 *dest, int ancho, int bytes)
{
	// copia los gráficos del origen al destino
	memcpy(dest, src, bytes);

	// calcula las variables que controlan el bucle
	int numLineas = bytes/ancho;
	int numIntercambios = (ancho + 1)/2;

	// recorre todas las líneas que forman el gráfico
	for (int j = 0; j < numLineas; j++){
		UINT8 *ptr1 = dest;
		UINT8 *ptr2 = ptr1 + ancho - 1;

		// realiza los intercambios necesarios para flipear esta línea
		for (int i = 0; i < numIntercambios; i++){
			UINT8 aux = *ptr1;
			*ptr1 = *ptr2;
			*ptr2 = aux;

			ptr1++;
			ptr2--;
		}

		// pasa a la siguiente línea
		dest = dest + ancho;
	}
}

// actualiza el sprite de la luz para que se mueva siguiendo a adso
void Juego::actualizaLuz()
{
	// desactiva el sprite de la luz
	sprites[spriteLuz]->esVisible = false;

	// si la pantalla está iluminada, sale
	if (motor->pantallaIluminada) return;

	// si adso no es visible en la pantalla actual
	if (!(personajes[1]->sprite->esVisible)){
		for (int i = 0; i < numSprites; i++){
			if (sprites[i]->esVisible){
				sprites[i]->haCambiado = false;
			}
		}

		return;
	}

	// actualiza las características del sprite de la luz según la posición del personaje
	SpriteLuz *sprLuz = (SpriteLuz *) sprites[spriteLuz];
	sprLuz->ajustaAPersonaje(personajes[1]);
}

// comprueba si se debe pausar el juego
void Juego::compruebaPausa()
{
	// si se ha pulsado suprimir, se para hasta que se vuelva a pulsar
	if (controles->seHaPulsado(KEYBOARD_SUPR)){
		pausa = true;

		while (true){
			controles->actualizaEstado();
			timer->sleep(10);
			if (controles->seHaPulsado(KEYBOARD_SUPR)){
				pausa = false;
				break;
			}
		}
	}
}

// comprueba si se desea cambiar los graficos VGA por CPC
void Juego::compruebaCambioCPC_VGA()
{
	// ESTE FUNCIONAMIENTO NO SE CORRESPONDE
	// CON EL DE LA VERSION ORIGINAL

	if (controles->seHaPulsado(FUNCTION_2))
	{
		// En Memoria esta la rom de 0x24000 bytes 
		//  seguidos de 174065 bytes con los graficos de 8 bits a usar
		//  seguidos de 174065+21600 bytes con los graficos VGA de 8 bits
		//  seguidos de 174065+21600 bytes con los graficos CPC de 8 bits
		// asi que copiamos despues de la rom los graficos que vamos a usar
		// (los 21600 son los graficos flipeados)
		if (GraficosCPC)
		{
			memcpy(	&roms[0x24000-1-0x4000],
				&roms[0x24000-1-0x4000+(174065+21600)],
				174065);

			GraficosCPC=false;
		}
		else
		{
			memcpy(	&roms[0x24000-1-0x4000],
				&roms[0x24000-1-0x4000+(174065+21600)*2],
				174065);

			GraficosCPC=true;
		}

		// genera los gráficos flipeados en x de las entidades que lo necesiten
		generaGraficosFlipeados();

		ReiniciaPantalla();		

	}
}

// comprueba si se desea grabar la partida
void Juego::compruebaSave()
{
	// ESTE FUNCIONAMIENTO NO SE CORRESPONDE
	// CON EL DE LA VERSION ORIGINAL

	if (controles->seHaPulsado(KEYBOARD_G))
	{
		// Frase vacia para parar la frase actual
		elGestorFrases->muestraFraseYa(0x38);
		// Esperamos a que se limpie el marcador
		while (elGestorFrases->mostrandoFrase)
		{
			elGestorFrases->actualizaEstado();
		} 

		// Preguntamos
		// CPC elMarcador->imprimeFrase("¿GRABAR? S:N", 110, 164, 2, 3);
		//elMarcador->imprimeFrase(",.WÑ", 110, 164, 4, 0); // VGA
		elMarcador->imprimeFrase("¿GRABAR? S:N", 110, 164, 4, 0); // VGA
		//se estaba guardando el ¿ como multibyte c2bf 
		// en vez de un simple char bf
		// con imprimeFrase no se cambia la Ñ por la W
		// la ¿ por el caracter adecuado ...
		//elMarcador->imprimeFrase("\xbfGRABAR? S:N", 110, 164, 4, 0); // VGA
		// TODO esto deberia tener su numero de frase  y estar traducido
		// e imprimirse con  
		//elGestorFrases->muestraFrase¿ya?

		do
		{
			losControles->actualizaEstado();

			if (losControles->estaSiendoPulsado(KEYBOARD_S))
			{
#ifdef __native_client__
				{
					// En Chrome bajo Linux se graba bien
					// pero en Chrome bajo windows da un error
					// al grabar indicando que no hay espacio libre
					// En Chrome "Versión 35.0.1904.0 canary"
					// al menos se puede si se crea inicialmente el
					// archivo para añadir, se cierra y luego se abre
					// ya para truncar
					std::ofstream out(savefile,
						std::ofstream::out|std::ofstream::app);
					out.close();
				}
#endif

				std::ofstream out(savefile,
					std::ofstream::out|std::ofstream::trunc);
	
				out << logica; 

				if ( out.fail() )
				{
					/* CPC
					elMarcador->imprimeFrase("            ", 110, 164, 2, 3);
					elMarcador->imprimeFrase("¡¡¡ERROR!!!", 110, 164, 2, 3); */
					// VGA
					elMarcador->imprimeFrase("            ", 110, 164, 4, 0);
					elMarcador->imprimeFrase("¡¡¡ERROR!!!", 110, 164, 4, 0);
					do
					{
						losControles->actualizaEstado();
					}while (losControles->estaSiendoPulsado(P1_UP) == false);
				}
				break;
			}
		}
		while (losControles->estaSiendoPulsado(KEYBOARD_N) == false);
		elGestorFrases->muestraFraseYa(0x38);
	}
}


// comprueba si se desea cargar la partida
bool Juego::compruebaLoad()
{
	// ESTE FUNCIONAMIENTO NO SE CORRESPONDE
	// CON EL DE LA VERSION ORIGINAL

	if (controles->seHaPulsado(KEYBOARD_C))
	{

		// Frase vacia para parar la frase actual
		elGestorFrases->muestraFraseYa(0x38);

		// Esperamos a que se limpie el marcador
		while (elGestorFrases->mostrandoFrase)
		{
			elGestorFrases->actualizaEstado();
		} 

		// Preguntamos
		// CPC elMarcador->imprimeFrase("¿CARGAR? S:N", 110, 164, 2, 3);
		elMarcador->imprimeFrase("¿CARGAR? S:N", 110, 164, 4, 0);  // VGA

		do
		{
			losControles->actualizaEstado();

			if (losControles->estaSiendoPulsado(KEYBOARD_S))
			{
				std::ifstream in(savefile);
				in >> logica;
				if ( in.fail() )
				{
					/* CPC
					elMarcador->imprimeFrase("            ", 110, 164, 2, 3);
					elMarcador->imprimeFrase("¡¡¡ERROR!!!", 110, 164, 2, 3); */
					// VGA
					elMarcador->imprimeFrase("            ", 110, 164, 4, 0);
					elMarcador->imprimeFrase("¡¡¡ERROR!!!", 110, 164, 4, 0);
					// el juego ha podido quedar destrozado
					// lo suyo seria cargar en un objeto logica temporal
					// y luego intercambiar los punteros
					// pero como es un singleton...
					// intentemos , al menos, reiniciar
					do
					{
						losControles->actualizaEstado();
					}while (losControles->estaSiendoPulsado(P1_UP) == false);
					// CPC elMarcador->imprimeFrase("           ", 110, 164, 2, 3);
					elMarcador->imprimeFrase("           ", 110, 164, 4, 0); // VGA
					logica->inicia();
					// devolvemos true, para que se reinicie todo
					return true;
				}
				else
				{
					return true;
				}

			}

		}
		while (losControles->estaSiendoPulsado(KEYBOARD_N) == false);
		elGestorFrases->muestraFraseYa(0x38);
	} 
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// métodos para mostrar distintas las pantallas de distintas situaciones del juego
/////////////////////////////////////////////////////////////////////////////

// muestra la imagen de presentación del juego
void Juego::muestraPresentacion()
{
	// fija la paleta de la presentación
	paleta->setIntroPalette();

	// muestra la pantalla de la presentación

	/* CPC
	cpc6128->showMode0Screen(roms - 0x4000);
	*/

	//VGA
	UINT8 *romsVGA = &roms[0x24000-1-0x4000];
	cpc6128->showVGAScreen(romsVGA + 0x1ADF0);

	// espera 5 segundos
	timer->sleep(5000);
}

// muestra el pergamino de presentación
void Juego::muestraIntroduccion()
{
	audio_plugin->Play(SONIDOS::Inicio,true);

	// muestra la introducción
	pergamino->muestraTexto(Pergamino::pergaminoInicio[idioma]);

	// coloca la paleta negra
	paleta->setGamePalette(0);

	// espera a que se suelte el botón
	bool espera = true;

	while (espera){
		controles->actualizaEstado();
		timer->sleep(1);
		espera = controles->estaSiendoPulsado(P1_BUTTON1);
	}

	audio_plugin->Stop(SONIDOS::Inicio);
}

// muestra indefinidamente el pergamino del final
void Juego::muestraFinal()
{
	audio_plugin->Play(SONIDOS::Final,true);
	while (true){
		// muestra el texto del final
		pergamino->muestraTexto(Pergamino::pergaminoFinal[idioma]);
	}
}

// muestra la parte de misión completada. Si se ha completado el juego, muestra el final
bool Juego::muestraPantallaFinInvestigacion()
{
	// si guillermo está vivo, sale
	if (!logica->haFracasado) return false;

	// indica que la cámara siga a guillermo y lo haga ya
	laLogica->numPersonajeCamara = 0x80;

	// si está mostrando una frase por el marcador, espera a que se termine de mostrar
	if (elGestorFrases->mostrandoFrase) return false;

	// oculta el área de juego
	// CPC limpiaAreaJuego(3);
	limpiaAreaJuego(0); // VGA

	// calcula el porcentaje de misión completada. Si se ha completado el juego, muestra el final
	int porc = logica->calculaPorcentajeMision();

	std::string frase1[8] = {
		"  HAS RESUELTO EL", // 0 castellano
		"YOU HAVE SOLVED", // 1 INGLES
		"  VOCÊ RESOLVEU", // 2 PORTUGUES BRASIL
		" HAS  RESOLT EL", // 3 CATALÁN
		"  RESOLVICHELO", // 4 GALLEGO
		"HAI RISOLTO IL", // 5 ITALIANO
		"YOU HAVE SOLVED", // 6 FINES
		"   RESOLVESTE " // 7 PORTUGUES
	};
	std::string porcentaje[8] = {
		"  XX POR CIENTO DE", // 0 castellano
		"  XX  PER  CENT", // 1 ingles
		"  XX POR CENTO DA", // 2 portugues brasil
		"  XX PER CENT DE", // 3 CATALÁN
		"  XX POR CENTO DA", // 4 GALLEGO
		"  XX PER CENTO", // 5 ITALIANO
		"  XX  PER  CENT", // 6 fines
		"  XX POR CENTO DA" // 7 PORTUGUES
	};
	std::string frase3[8] = {
		"  LA INVESTIGACIÓN", // 0 castellano
		" OF THE RESEARCH", // 1 INGLES
		"   INVESTIGAÇÃO", // 2 PORTUGUES BRASIL
		" LA INVESTIGACIÓ", // 3 CATALÁN
		"  INVESTIGACIÓN", // 4 GALLEGO
		" DELL'INDAGINE", // 5 italiano
		" OF THE RESEARCH", // 6 FINES
		"   INVESTIGAÇÃO" // 7 PORTUGUES
	};
	std::string frase4[8] = {
		"PULSA ESPACIO PARA EMPEZAR", // 0 castellano
		"PULSA ESPACIO PARA EMPEZAR", // 1 ingles
		"PULSA ESPACIO PARA EMPEZAR", // 2 portugues brasil
		"PULSA ESPACIO PARA EMPEZAR", // 3 catalán
		"PULSA ESPACIO PARA EMPEZAR", // 4 gallego
		"PULSA ESPACIO PARA EMPEZAR", // 5 italiano
		"PULSA ESPACIO PARA EMPEZAR", // 6 fines
		"PULSA ESPACIO PARA EMPEZAR" // 7 portugues
	};
	porcentaje[idioma][2] = ((porc/10) % 10) + 0x30;
	porcentaje[idioma][3] = (porc % 10) + 0x30;

	// CPC
	//marcador->imprimeFrase("HAS RESUELTO EL", 96, 32, 2, 3);
	//marcador->imprimeFrase(porcentaje, 88, 48, 2, 3);
	//marcador->imprimeFrase("DE LA INVESTIGACION", 90, 64, 2, 3);
	//marcador->imprimeFrase("PULSA ESPACIO PARA EMPEZAR", 56, 128, 2, 3);
	//
	// VGA
	//marcador->imprimeFrase("HAS RESUELTO EL", 96, 32, 4, 0);
	//marcador->imprimeFrase(porcentaje[idioma], 88, 48, 4, 0);
	//marcador->imprimeFrase("DE LA INVESTIGACION", 90, 64, 4, 0);
	//marcador->imprimeFrase("PULSA ESPACIO PARA EMPEZAR", 56, 128, 4, 0);
	// VGA con idiomas
	marcador->imprimeFrase(frase1[idioma], 96, 32, 4, 0);
	marcador->imprimeFrase(porcentaje[idioma], 88, 48, 4, 0);
	marcador->imprimeFrase(frase3[idioma], 90, 64, 4, 0);
	marcador->imprimeFrase(frase4[idioma], 56, 128, 4, 0);

	// espera a que se pulse y se suelte el botón
	bool espera = true;

	while (espera){
		controles->actualizaEstado();
		timer->sleep(1);
		espera = !(controles->estaSiendoPulsado(P1_BUTTON1) || controles->estaSiendoPulsado(KEYBOARD_SPACE));
	}

	espera = true;

	while (espera){
		controles->actualizaEstado();
		timer->sleep(1);
		espera = controles->estaSiendoPulsado(P1_BUTTON1) || controles->estaSiendoPulsado(KEYBOARD_SPACE);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// creación de las entidades del juego
/////////////////////////////////////////////////////////////////////////////

// crea los sprites, personajes, puertas y objetos del juego
void Juego::creaEntidadesJuego()
{
	// sprites de los personajes

	// sprite de guillermo
	sprites[0] = new Sprite();

	// sprite de adso
	sprites[1] = new Sprite();

	// sprite de los monjes
	for (int i = 2; i < 8; i++){
		sprites[i] = new SpriteMonje();
	}

	// sprite de las puertas
	for (int i = primerSpritePuertas; i < primerSpritePuertas + numPuertas; i++){
		sprites[i] = new Sprite();
		sprites[i]->ancho = sprites[i]->oldAncho = 0x06;
		sprites[i]->alto = sprites[i]->oldAlto = 0x28;
	}

	// CPC int despObjetos[8] = { 0x88f0, 0x9fb0, 0x9f80, 0xa010, 0x9fe0, 0x9fe0, 0x9fe0, 0x88c0 };
	// VGA
	/*
	int despObjetos[8] = { 
		0x24000 - 1 - 0x4000 + 11200,  // LIBRO
		0x24000 - 1 - 0x4000 + 34496,  // GUANTES
		0x24000 - 1 - 0x4000 + 34304,  // GAFAS
		0x24000 - 1 - 0x4000 + 34880,  // ?PERGAMINO?
		0x24000 - 1 - 0x4000 + 34688,  // LLAVE
		0x24000 - 1 - 0x4000 + 34688,
		0x24000 - 1 - 0x4000 + 34688,
		0x24000 - 1 - 0x4000 + 11008  // ?LAMPARA?
	}; */
	// En Sprite::dibujaVGA ya le anyade el 0x24000 - 1 - 0x4000 para saltarse la rom CPC e ir a los graficos VGA
	// ¡¡¡ ojo !!! , entonces en Marcador::dibujaObjetos hay que incluir este salto ...
	int despObjetos[8] = { 
		11200,  // LIBRO
		34496,  // GUANTES
		34304,  // GAFAS
		34880,  // ?PERGAMINO?
		34688,  // LLAVE
		34688,
		34688,
		11008  // ?LAMPARA?
	};// TODO: los desplazamientos estan bien, lo que no se si se corresponde
	// del todo es el indice, o sea, que el objeto 7 en el juego es la lampara
	// y el 3 el pergamino... 

	// sprite de los objetos
	for (int i = primerSpriteObjetos; i < primerSpriteObjetos + numObjetos; i++){
		sprites[i] = new Sprite();
		sprites[i]->ancho = sprites[i]->oldAncho = 0x04;
		sprites[i]->alto = sprites[i]->oldAlto = 0x0c;
		// CPC sprites[i]->despGfx = despObjetos[i - primerSpriteObjetos];
		// VGA
		// 0x24000 para pasar las roms CPC
		// -1 de empezar a contar en el cero
		// - 0x4000 ya que en el puntero con el que se trabaja se han pasado los primeros 0x4000 bytes con la pantalla de presentacion CPC
		// +34304 que es donde esta el tile 228 , que es por donde anda la imagen del primer objeto
		// como los objetos son de 16*12 (¡¡¡ojo!!! no son como el resto de tiles!!!)
		// sumo 16*12* el numero de objeto que busco
		//sprites[i]->despGfx = 0x24000 - 1 - 0x4000 + 34304 + (i - primerSpriteObjetos)*16*12;
		//muy bonito , si los graficos estuviesen en el mismo orden logico que los objetos en el juego
	
		// igual que en CPC , pero con el array despObjetos "actualizado" a las posiciones VGA
		sprites[i]->despGfx = despObjetos[i - primerSpriteObjetos];

	}

	// sprite de los reflejos en el espejo
	sprites[spritesReflejos] = new Sprite();
	sprites[spritesReflejos + 1] = new Sprite();

	// sprite de la luz
	sprites[spriteLuz] = new SpriteLuz();

	// crea los personajes del juego
	personajes[0] = new Guillermo(sprites[0]);
	personajes[1] = new Adso(sprites[1]);
	personajes[2] = new Malaquias((SpriteMonje *)sprites[2]);
	personajes[3] = new Abad((SpriteMonje *)sprites[3]);
	personajes[4] = new Berengario((SpriteMonje *)sprites[4]);
	personajes[5] = new Severino((SpriteMonje *)sprites[5]);
	personajes[6] = new Jorge((SpriteMonje *)sprites[6]);
	personajes[7] = new Bernardo((SpriteMonje *)sprites[7]);

	// inicia los valores comunes
	for (int i = 0; i < 8; i++){
		personajes[i]->despX = -2;
		personajes[i]->despY = -34;
	}
	personajes[1]->despY = -32;
	
	// crea las puertas del juego
	for (int i = 0; i < numPuertas; i++){
		puertas[i] = new Puerta(sprites[primerSpritePuertas + i]);
	}

	// crea los objetos del juego
	for (int i = 0; i < numObjetos; i++){
		objetos[i] = new Objeto(sprites[primerSpriteObjetos + i]);
	}
}
