// RDTSCTimer.cpp
//
/////////////////////////////////////////////////////////////////////////////

#include "RDTSCTimer.h"
#include "SDL.h"

// Para SDL_HasRDTSC()
#include "SDL_cpuinfo.h"

/////////////////////////////////////////////////////////////////////////////
// initialization and cleanup
/////////////////////////////////////////////////////////////////////////////

RDTSCTimer::RDTSCTimer()
{
}

RDTSCTimer::~RDTSCTimer()
{
}

bool RDTSCTimer::init()
{
	// if the CPU doesn't support the RDTSC instruction, init fails
	if (!supportsRDTSC()){
		return false;
	}

	_ticksPerSecond = calcTicksPerSecond();
	_ticksPerMilliSecond = _ticksPerSecond/1000;

	return true;
}

void RDTSCTimer::end()
{
}

/////////////////////////////////////////////////////////////////////////////
// timer methods
/////////////////////////////////////////////////////////////////////////////

INT64 RDTSCTimer::getTime()
{
	INT64 result;

#ifdef __GNUC__
	__asm__ __volatile__("rdtsc" : "=A" (result));
#else
	__asm {
		rdtsc
		lea ebx,[result]
		mov [ebx],eax
		mov [ebx+4],edx
	}
#endif

	return result;
}

INT64 RDTSCTimer::getTicksPerSecond()
{
	return _ticksPerSecond;
}

// windows Sleep function isn't precise enough, so here it's a better sleep method
void RDTSCTimer::sleep(UINT32 milliseconds)
{
	INT64 time1, time2; 
	bool finished = false;

	time1 = getTime();
	
	while (!finished){
		time2 = getTime();

		finished = (time2 - time1) >= _ticksPerMilliSecond*milliseconds;
		if (!finished){
			SDL_Delay(0);
		}
	} 
// No es suficiente con un simple SDL_Delay???
// Count on a delay granularity of at least 10 ms. 
//	SDL_Delay(milliseconds);
}

/////////////////////////////////////////////////////////////////////////////
// helper methods
/////////////////////////////////////////////////////////////////////////////

INT64 RDTSCTimer::calcTicksPerSecond()
{
	// raise the priority for accurate timing
	// ¡¡¡ FALTA POR IMPLEMENTAR !!!

	// wait for 0.25 seconds
	INT64 begin = RDTSCTimer::getTime();

	SDL_Delay(1000/4);

	INT64 end = RDTSCTimer::getTime();

	// restore the previous priority
	// ¡¡¡ FALTA POR IMPLEMENTAR !!!

	return (end - begin)*4;
}

bool RDTSCTimer::supportsRDTSC()
{
	/*
	int cpuFeatures=0;
#ifdef __GNUC__
        __asm__ __volatile__(
                "mov $1, %%eax\n"
                "cpuid\n"
                "mov %%edx,%0" : "=r"(cpuFeatures)::"eax","ebx","ecx","edx" );
#else
	__asm {
		mov eax, 1
		cpuid
		mov cpuFeatures, edx
	}
#endif

        return ((cpuFeatures & 0x10) == 0x10);
	*/
	return SDL_HasRDTSC();
}
