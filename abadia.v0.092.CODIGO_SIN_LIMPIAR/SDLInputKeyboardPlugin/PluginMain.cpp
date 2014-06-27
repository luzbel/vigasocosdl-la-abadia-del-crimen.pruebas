// PluginMain.cpp
//
/////////////////////////////////////////////////////////////////////////////

// Para DECLSPEC en SDL_loadso.h
#include "SDL.h"

#include "SDLInputKeyboardPlugin.h"

static const char * const description = "VIGASOCO SDL Keyboard Plugin v1.0";

static const char *plugins[] = {
	"SDLInputPlugin" 
};

/////////////////////////////////////////////////////////////////////////////
// plugin creation/destruction
/////////////////////////////////////////////////////////////////////////////

extern "C" DECLSPEC
#ifdef __VIGASOCO_SDL_STATIC__
void create_INPUT_Plugin(const char *name,void**plugin)
#else
void createPlugin(const char *name,void**plugin)
#endif
{
	if (strcmp(name, plugins[0]) == 0){
		*plugin = new SDLInputKeyboardPlugin();
	} else {
		*plugin = NULL;
	}
}

extern "C" DECLSPEC
#ifdef __VIGASOCO_SDL_STATIC__
void destroy_INPUT_Plugin(IInputPlugin *plugin)
#else
void destroyPlugin(IInputPlugin *plugin)
#endif
{
	delete plugin;
}

/////////////////////////////////////////////////////////////////////////////
// plugin information
/////////////////////////////////////////////////////////////////////////////

extern "C" DECLSPEC
#ifdef __VIGASOCO_SDL_STATIC__
int get_INPUT_Version()
#else
int getVersion()
#endif
{
	return 1;
}

extern "C" DECLSPEC
#ifdef __VIGASOCO_SDL_STATIC__
int get_INPUT_Type()
#else
int getType()
#endif
{
	return INPUT_PLUGIN;
}

extern "C" DECLSPEC
#ifdef __VIGASOCO_SDL_STATIC__
const char **get_INPUT_Plugins(int *num)
#else
const char **getPlugins(int *num)
#endif
{
	*num = sizeof(plugins)/sizeof(plugins[0]);
	return plugins;
}

extern "C" DECLSPEC
#ifdef __VIGASOCO_SDL_STATIC__
const char *get_INPUT_DLLDescription(int *num)
#else
const char *getDLLDescription(int *num)
#endif
{
	return description;
}
