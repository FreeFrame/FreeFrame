///////////////////////////////////////////////////////////////////////////////////
// FreeFrame.cpp
//
// FreeFrame Open Video Plugin Prototype
// C Version
//
// Implementation of a plugin interface for the FreeFrame API
//
// www.freeframe.org
// marcus@freeframe.org

/*

Copyright (c) 2002, Marcus Clements www.freeframe.org
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of FreeFrame nor the names of its
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

///////////////////////////////////////////////////////////////////////////////////////////
//
// includes 
//

#include "FreeFrame.h"
#include "FreeFrameSample.h"  // replace this with your plugins header


///////////////////////////////////////////////////////////////////////////////////////////
// Windows DLL Entry point
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

///////////////////////////////////////////////////////////////////////////////////////
// plugMain - The one and only exposed function
// parameters: 
//	functionCode - tells the plugin which function is being called
//  pParam - 32-bit parameter or 32-bit pointer to parameter structure
//
// PLUGIN DEVELOPERS:  you shouldn't need to change this function
//
// All parameters are cast as 32-bit untyped pointers and cast to appropriate
// types here
// 
// All return values are cast to 32-bit untyped pointers here before return to 
// the host
//

#ifdef WIN32
 __declspec(dllexport) LPVOID __stdcall plugMain(DWORD functionCode, LPVOID pParam, DWORD reserved ) 
#elif LINUX
   LPVOID plugMain( DWORD functionCode, LPVOID pParam, DWORD reserved )
#endif	
{
	switch(functionCode) {

	case FF_GETINFO:
		return (LPVOID) getInfo();
	
	case FF_INITIALISE:
		return (LPVOID) initialise( (VideoInfoStruct*) pParam);
	
	case FF_DEINITIALISE:
		return (LPVOID) deInitialise();
	
	case FF_GETNUMPARAMETERS:
		return (LPVOID) getNumParameters();

	case FF_GETPARAMETERNAME:
		return (LPVOID) getParameterName( (DWORD) pParam );
	
	case FF_GETPARAMETERDEFAULT:
		return (LPVOID)(DWORD) getParameterDefault( (DWORD) pParam );
	
	case FF_GETPARAMETERDISPLAY:
		return  (LPVOID) getParameterDisplay( (DWORD) pParam );
	
	// parameters are passed in here as a packed struct of two DWORDS:
	// index and value
	case FF_SETPARAMETER:
		return (LPVOID) setParameter( (SetParameterStruct*) pParam );
	
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

