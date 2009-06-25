#include <SDL/SDL.h>
#include <dlfcn.h>

#include "pez.h"
#include "pezdef.h"

prim P_sdl_init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
}

prim P_maybe_lock()
{
	SDL_Surface *s = S0;
	if(SDL_MUSTLOCK(s)) 
		SDL_LockSurface(s);
	Pop;
}

struct primfcn pez_ffi_definitions[] = {
	{"0SDL-INIT", P_sdl_init},
	{"0?LOCK", P_maybe_lock},
	{0, 0},
};

void pez_ffi_init()
{
}
