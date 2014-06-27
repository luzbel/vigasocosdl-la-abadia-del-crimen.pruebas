// SDLMain.cpp
//
// Based on VIGASOCO Project Win32 port (c) 2003 by MAB
//	SDL port @2006,2007,2008,2009,2010,2011,2012,2012+1,2014 by Luzbel
//
//	See readme.txt for license and usage information.
//
/////////////////////////////////////////////////////////////////////////////

#include "VigasocoSDL.h"
#include "SDL.h"
#include "iostream"

#ifdef __native_client__
#include <sys/mount.h>
#include <errno.h>
#endif

typedef std::vector<std::string> Strings;

// default options
std::string g_game("abadia");
std::string g_drawPluginsDLL("libVigasocoSDLDrawPlugin.so");
std::string g_drawPlugin("win8");

std::string g_audioPluginsDLL("libVigasocoSDLAudioPlugin.so");
std::string g_audioPlugin("SDLAudioPlugin");

Strings g_inputPluginsDLLs;
Strings g_inputPlugins;
Strings g_paths;

// parser helper function
bool parseCommandLine(std::string cmdLine);

void showError(std::string error);
void showUsage(std::string error);

/////////////////////////////////////////////////////////////////////////////
//	SDL application entry point
/////////////////////////////////////////////////////////////////////////////

#ifdef __native_client__
int internal_game_main(void *ptr);

int game_main(void *ptr)
{
	SDL_Thread *thread=NULL;
        int threadReturnValue;

			// la rom, archivos de sonido, graficos VGA,etc.
			// se descargan via http 
	//pruebas
	//con localhost no monta bien, uso ubuntu que es lo que hay en el /etc/hostname
	// y /etc/host para pruebas locales
	//int result = mount("http://ubuntu/", "/roms", "httpfs", 0, "");
	//int result = mount("http://192.168.159.176/", "/roms", "httpfs", 0, "");

	//almacenado en google drive
	//int result = mount("https://googledrive.com/host/0B2D2jhRcf6o-Z2VwcUp0TDBIa0k/", "/roms", "httpfs", 0, "");
	// los de google drive han cambiado el enlace, al ir al interior redirigen a este otro
	// ver si see puede recibir como parametro desde la web para evitar que si lo cambian otra vez no funcione
	int result = mount("https://d7a3675a31436e0742da09235a5ca70b3ec1902d.googledrive.com/host/0B2D2jhRcf6o-Z2VwcUp0TDBIa0k/", "/roms", "httpfs", 0, "");
	printf("result mount httpfs =%d %s\n",result,strerror(errno));

	//almacenado en google drive la version para 99redpotions
	//int result = mount("https://googledrive.com/host/0B2D2jhRcf6o-Z2VwcUp0TDBIa0k/abadia99rp/", "/roms", "httpfs", 0, "");
	//printf("result mount httpfs =%d %s\n",result,strerror(errno));

			// para poder cargar y grabar la partida
			// se usa el almacenamiento persistente de HTML5
			// en el javascrip de la pagina hay que reservar el espacio
			// y el usuario debe aceptar la solicitud en el navegador
			// 
			// para comprobar que se ha generado el fichero abadia.save
			// ir en el navegador a 
			// filesystem:http://ubuntu/persistent/
			// cambiando ubuntu por el hosting correspondiente
	//result = mount("UNUSED", "/save", "html5fs", 0, "type=PERSISTENT,expected_size=1024*20");
	result = mount("", "/save", "html5fs", 0, "type=PERSISTENT,expected_size=20480");
	printf("result mount html5fs =%d %d %s\n",result,errno,strerror(errno));

	thread=SDL_CreateThread(internal_game_main,(void *)NULL);
	if(NULL==thread)
        {
                //printf("ERR createthread\n");
        }
        else
        {
                SDL_WaitThread( thread, &threadReturnValue);
                //printf("despues de waitthread\n");
        }
	return 0;
}

int internal_game_main(void *ptr)
{
	int argc=1;
	const char *argv[]={ "test" };
	//argc=3; const char *argv[]={ "da-igual", "abadia" "-video:libVigasocoSDLDrawPlugin.so,wingris8" };;

#else
int main(int argc,char **argv)
{
#endif
	std::string szCmdLine;
	for ( int icont=1;icont<argc;icont++)
	{
		szCmdLine+=argv[icont];
		szCmdLine+=" ";	
	}

	if (!parseCommandLine(szCmdLine)){
		showUsage("Unknown parameters! Read the documentation for detailed information about usage.");
		return -1;
	}
	VigasocoSDL VIGASOCO( g_game, g_drawPluginsDLL, g_drawPlugin,
				g_inputPluginsDLLs, g_inputPlugins,
				g_audioPluginsDLL, g_audioPlugin,
				g_paths);
	// init the game
	if (!VIGASOCO.init(g_game)){
		VIGASOCO.end();
		SDL_Quit(); //TODO: REVISAR SI ES CORRECTO HACER EL SDL_Quit aqui
		// if there was an error, show it and exit
		showError(VIGASOCO.getError());

		return -1;
	}

	// run until the user has closed the window
	VIGASOCO.mainLoop();

	// cleanup
	VIGASOCO.end();

	SDL_Quit();

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//	Parser helper functions
/////////////////////////////////////////////////////////////////////////////

void split(std::string source, char splitChar, Strings *strings)
{
	std::string::size_type index, prevIndex = 0;
	
	do {
		// find next occurrence of the splitChar
		index = source.find(splitChar, prevIndex);

		// get substring from previous occurrence to this occurrence
		std::string substr = (source.substr(prevIndex, index - prevIndex));

		// if it's not the empty substring, save it
		if (substr.size() != 0){
			strings->push_back(substr);
		}

		prevIndex = index + 1;
	} while (index != std::string::npos);
}

bool parseVideo(Strings &params)
{
	for (std::string::size_type i = 1; i < params.size(); i++){
		Strings subParams;

		// split parameter in DLL and plugin
		split(params[i], ',', &subParams);

		if (subParams.size() != 2){
			return false;
		}

		// save DLL and plugin
		g_drawPluginsDLL = subParams[0];
		g_drawPlugin = subParams[1];
	}

	return true;
}

bool parseInputs(Strings &params)
{
	for (std::string::size_type i = 1; i < params.size(); i++){
		Strings subParams;

		// split parameter in DLL and plugin groups
		split(params[i], ';', &subParams);

		for (std::string::size_type j = 0; j < subParams.size(); j++){
			Strings pluginInfo;

			// split parameter in DLL and plugin
			split(subParams[j], ',', &pluginInfo);

			if (pluginInfo.size() != 2){
				return false;
			}

			// save DLL and plugin
			g_inputPluginsDLLs.push_back(pluginInfo[0]);
			g_inputPlugins.push_back(pluginInfo[1]);
		}
	}

	return true;
}

bool parsePaths(Strings &params)
{
	if (params.size() != 2){
		return false;
	}

	// split multiple paths
	split(params[1], ';', &g_paths);

	return true;
}

bool parseCommands(Strings &params)
{
	for (std::string::size_type i = 1; i < params.size(); i++){
		Strings subParams;

		// split parameter in command and arguments
		split(params[i], ':', &subParams);

		// process known commands
		if (subParams[0] == "-video"){
			if (!parseVideo(subParams))	return false;
		} else if (subParams[0] == "-input"){
			if (!parseInputs(subParams))	return false;
		} else if (subParams[0] == "-path"){
			if (!parsePaths(subParams))	return false;
		} else {	// error
			return false;
		}
	}

	return true;
}

bool parseCommandLine(std::string cmdLine)
{
	Strings params;

	// split user parameters
	split(cmdLine, ' ', &params);

	// parse user parameters
	if (params.size() > 0){
		g_game = params[0];

		if (params.size() > 1){
			if (!parseCommands(params)){
				return false;
			}
		}
	}

	// if the user hasn't set any input plugin, set the default one
	if (g_inputPluginsDLLs.size() == 0){
		g_inputPluginsDLLs.push_back("libVigasocoSDLInputPlugin.so");
		g_inputPlugins.push_back("SDLInputPlugin");
	}

	return true;
}

void showError(std::string error)
{
	if (error == ""){
		std::cerr << 
		"Unexpected error loading VIGASOCO. "
		"Read the manual for usage information!" << std::endl <<
		"Error!" << std::endl;
	} else {
		std::cerr << error.c_str() << std::endl << "Error!" << std::endl;
	} 
}

void showUsage(std::string error)
{
	std::cerr << (error +
		"\nUsage: vigasoco <game> "
		"-video:<pluginDLL>, <plugin> "
		"-input:{<pluginDLL>, <plugin>;}* <pluginDLL>, <plugin>"
		"-path:{<path>;}* <path>"
		) << std::endl << "Error!" << std::endl;
}
