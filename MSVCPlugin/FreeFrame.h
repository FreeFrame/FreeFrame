//////////////////////////////////////////////////////////////
// FreeFrame.h

#ifndef __FREEFRAME_H__
#define __FREEFRAME_H__

//////////////////////////////////////////////////////////////
//
// includes
//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

#elif defined LINUX

//typedefs for linux - in windows these are defined in files included by windows.h
typedef unsigned int DWORD;
typedef void * LPVOID;
typedef char BYTE;

#endif



//////////////////////////////////////////////////////////////
// #defines

#define FF_EFFECT				0
#define FF_FAIL					0xFFFFFFFF

#define FF_GETINFO				0
#define FF_INITIALISE			1
#define FF_DEINITIALISE			2
#define FF_PROCESSFRAME			3
#define FF_GETNUMPARAMETERS		4
#define FF_GETPARAMETERNAME		5
#define FF_GETPARAMETERDEFAULT	6
#define FF_GETPARAMETERDISPLAY	7
#define FF_SETPARAMETER			8
#define FF_GETPARAMETER			9
#define FF_GETPLUGINCAPS		10


struct PlugInfoStruct {
	DWORD	APIMajorVersion;
	DWORD	APIMinorVersion;
	BYTE	uniqueID[4];			// 4 chars uniqueID - not null terminated
	BYTE	pluginName[16];			// 16 chars plugin friendly name - not null terminated
	DWORD	pluginType;				// Effect or source
};

struct ParameterStruct {
	float value;
	float defaultValue;
	char displayValue[16];
	char name[16];
};


//////////////////////////////////////////////////////////////
// Function prototypes

#ifdef WIN32

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved );

__declspec(dllexport) LPVOID __stdcall plugMain(DWORD functionCode, LPVOID pParam);

#elif LINUX

  LPVOID plugMain(DWORD functionCode,LPVOID pParam);

#endif

PlugInfoStruct*	getInfo();							// get information about plugin
DWORD	initialise();								// allocate memory, set defaults
DWORD	deinitialise();								// deallocate memory, tidy up
DWORD	getNumParameters();							// returns number of parameters
char*	getParameterName(DWORD index);				// returns string with the name if parameter n
float	getParameterDefault(DWORD index);			// returns value  of parameter n
char*	getParameterDisplay(DWORD index);			// returns string containing value of parameter to display
DWORD	setParameter(DWORD index, float value);		// returns success/fail - sets the value of parameter n in response to user
float	getParameter(DWORD index);					// returns value of parameter n
DWORD	processFrame(LPVOID pFrame);				// process frame of video, returns success/fail
DWORD	getPluginCaps(DWORD cap);					// plugin capabilities returns true/false

#endif
