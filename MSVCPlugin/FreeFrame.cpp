// FreeFrame.cpp : Defines the entry point for the DLL application.
//


#include "StdAfx.h"
#include "FreeFrame.h"
#include <stdio.h>
#include <string.h>

#define NUM_PARAMS 3

//////////////////////////////////////////////////////////////
// DLL Entry point
//
// notes: we may want to capture hModule as the instance of the host...

#ifdef WIN32
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
#endif

///////////////////////////////////////////////////////////////
// main - The one and only exposed function
// parameters: 
//	functionCode - tells the plugin which function is being called
//  pParam - 32-bit pointer to parameter

#ifdef WIN32
 __declspec(dllexport) LPVOID __stdcall plugMain(DWORD functionCode, LPVOID pParam) 
#elif LINUX
   LPVOID plugMain(DWORD functionCode,LPVOID pParam)
#endif	
{
	switch(functionCode) {

	case FF_GETINFO:
		return (LPVOID) getInfo();
	
	case FF_INITIALISE:
		return (LPVOID) initialise();
	
	case FF_DEINITIALISE:
		return (LPVOID) deinitialise();
	
	case FF_GETNUMPARAMETERS:
		return (LPVOID) getNumParameters();

	case FF_GETPARAMETERNAME:
		return (LPVOID) getParameterName( (DWORD) pParam );
	
	case FF_GETPARAMETERDEFAULT:
		return (LPVOID)(DWORD) getParameterDefault( (DWORD) pParam );
	
	case FF_GETPARAMETERDISPLAY:
		return  (LPVOID) getParameterDisplay( (DWORD) pParam );
	
	case FF_SETPARAMETER:
		return (LPVOID) setParameter( (DWORD) pParam, *((float*)(pParam)+1) );
	
	case FF_PROCESSFRAME:
		return (LPVOID) processFrame(pParam);

	case FF_GETPARAMETER:
		return (LPVOID)(DWORD) getParameter((DWORD) pParam);

	case FF_GETPLUGINCAPS:
		return (LPVOID) getPluginCaps( (DWORD) pParam);
	
	default:
		return (LPVOID) FF_FAIL;
	}
 }


/////////////////////////////////////////////////////
//
// Plugin Globals
//

PlugInfoStruct info;
ParameterStruct parameters[NUM_PARAMS];

PlugInfoStruct* getInfo() 
{
	info.APIMajorVersion = 0;
	info.APIMinorVersion = 1;
	char ID[5] = "MPB1";
	char name[17] = "MSVC_Plugin_Base";
	
	memcpy(info.uniqueID, ID, 4);
	memcpy(info.pluginName, name, 16);
	info.pluginType = FF_EFFECT;

	return &info;
}

// set up the parameters
DWORD initialise()
{
	parameters[0].defaultValue = 0.0f;
	parameters[1].defaultValue = 0.5f;
	parameters[2].defaultValue = 1.0f;
	parameters[0].value = 0.0f;
	parameters[1].value = 0.5f;
	parameters[2].value = 1.0f;
	char tempName1[17] = "MSVC_plug_param1";
	char tempName2[17] = "MSVC_plug_param2";
	char tempName3[17] = "MSVC_plug_param3";
	memcpy(parameters[0].name, tempName1, 16);
	memcpy(parameters[1].name, tempName2, 16);
	memcpy(parameters[2].name, tempName3, 16);
	return 23;
}

// deallocate memory, tidy up
DWORD deinitialise()
{
	return 23;
}


// returns number of parameters
DWORD getNumParameters()
{
	return NUM_PARAMS;  // 1 param for now
}

// returns string with the name 0f parameter n
char* getParameterName(DWORD index)
{
	return parameters[index].name;
}

// returns value  of parameter n
float getParameterDefault(DWORD index)
{
	return parameters[index].defaultValue;
}

// returns string containing value of parameter to display
char* getParameterDisplay(DWORD index)
{

	sprintf(parameters[index].displayValue, "%f",parameters[index].value);
	return parameters[index].displayValue;
}

// returns success/fail - sets the value of parameter n in response to user
DWORD setParameter(DWORD index, float value)
{
	parameters[index].value = value;
	return 0;
}

float getParameter(DWORD index)
{
	return parameters[index].value;
}

// process frame of video, returns success/fail
DWORD processFrame(LPVOID pFrame)
{
	return 999;
}

DWORD getPluginCaps(DWORD cap)
{
	return 0;
}
