////////////////////////////////////////////////////////////////////////////////////
// FreeFrameSample.cpp
//
// FreeFrame Open Video Plugin 
// C Version
//
// Implementation of the Free Frame sample plugin
//
// www.freeframe.org
// marcus@freeframe.org

/*

Copyright (c) 2002, Marcus Clements www.freeframe.org
All rights reserved.

FreeFrame 1.0 upgrade by Russell Blakeborough
email: boblists@brightonart.org

Extra parameter checking added by Dan Kent (10/07/2004)
email: dan@syzygy-visuals.co.uk

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

#include "FreeFrameSample.h"
#include <string.h>
#include <stdio.h>

#define NUM_PARAMS 3
#define NUM_INPUTS 1
/////////////////////////////////////////////////////
//
// Plugin Globals
//
// these are defined globally in this sample plugin for simplicities sake
// Plugin developers should allocate memory from the heap for this stuff
//

PlugInfoStruct plugInfo;
PlugExtendedInfoStruct plugExtInfo;
ParamConstantsStruct paramConstants[NUM_PARAMS];


/////////////////////////////////////////////////////
//
// Sample Function implementation
//

///////////////////////////////////////////////////////////////////////////////////////
// getInfo
//
// gets information about the plugin - version, unique id, short name and type 
// This function should be identical in all future versions of the FreeFrame API
//  
// return values (32-bit pointer to PlugInfoStruct)
// FF_FAIL on error
// 32-bit pointer to PlugInfoStruct
//
// HOST: call this function first to get version information
//       the version defines the other fucntion codes that are supported
//       supported function codes are listed in the documentation www.freeframe.org

PlugInfoStruct* getInfo() 
{
	plugInfo.APIMajorVersion = 1;		// number before decimal point in version nums
	plugInfo.APIMinorVersion = 000;		// this is the number after the decimal point
										// so version 0.511 has major num 0, minor num 501
	char ID[5] = "FFP1";		 // this *must* be unique to your plugin 
								 // see www.freeframe.org for a list of ID's already taken
	char name[17] = "CStd_Plugin_Base";
	
	memcpy(plugInfo.uniqueID, ID, 4);
	memcpy(plugInfo.pluginName, name, 16);
	plugInfo.pluginType = FF_EFFECT;

	return &plugInfo;
}


///////////////////////////////////////////////////////////////////////////////////////
// initialise
//
// do nothing for now - plugin instantiate is where the init happens now

DWORD initialise()
{
    // populate the parameters constants structs
	paramConstants[0].defaultValue = 0.5f;
	paramConstants[1].defaultValue = 0.5f;
	paramConstants[2].defaultValue = 0.5f;

	char tempName1[17] = "red";
	char tempName2[17] = "green";
	char tempName3[17] = "blue";

	memcpy(paramConstants[0].name, tempName1, 16);
	memcpy(paramConstants[1].name, tempName2, 16);
	memcpy(paramConstants[2].name, tempName3, 16);

	return FF_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////
// deinitialise
//
// Tidy up   
// Deallocate memory
//
// return values (DWORD)
// FF_SUCCESS - success
// non-zero - fail (error values to be defined)
//
// HOST: This *must* be the last function called on the plugin

DWORD deInitialise()
{
	return FF_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////
// getNumParameters 
//
// returns number of parameters in plugin
// 
// return values (DWORD)
// number of parameters
// FF_FAIL on error
//

DWORD getNumParameters()
{
	return NUM_PARAMS;  
}

///////////////////////////////////////////////////////////////////////////////////////
// getParameterName
//
// returns pointer to 16 byte char array containing the name of parameter specified by index
//
// parameters:
// DWORD index - index of parameter 
//
// return values (32-bit pointer to char):
// 32-bit pointer to array of char
// pointer to empty string if param index is not valid
//

char* getParameterName(DWORD index)
{
	//check that the param index is valid
	if(index >= 0 && index < NUM_PARAMS)
		return paramConstants[index].name;
	else
		return "";
}


///////////////////////////////////////////////////////////////////////////////////////
// getParameterDefault
//
// returns default value of parameter specified by index as 32-bit float 0<=value<=1
//
// parameters:
// DWORD index - index of parameter 
//
// return values (32-bit float):
// 32-bit float value
// FF_FAIL on error
//

float getParameterDefault(DWORD index)
{
	//check that the param index is valid
	if(index >= 0 && index < NUM_PARAMS)
		return paramConstants[index].defaultValue;
	else
		return (float)FF_FAIL;
}

///////////////////////////////////////////////////////////////////////////////////////
// getParameterDisplay
//
// returns pointer to array of 16 char containing a string to display as the value of
// parameter index
//
// parameters:
// DWORD index - index of parameter 
//
// return values (32-bit pointer to char):
// 32-bit pointer to array of char
// pointer to empty string if the parameter index is not valid
//

char* plugClass::getParameterDisplay(DWORD index)
{
	//check that the param index is valid
	if(index >= 0 && index < NUM_PARAMS)
	{
		// fill the array with spaces first
		for (int n=0; n<16; n++) {
			paramDynamicData[index].displayValue[n] = ' ';
		}
		sprintf(paramDynamicData[index].displayValue, "%f",paramDynamicData[index].value);
		return paramDynamicData[index].displayValue;
	}
	else
		return "";
}

///////////////////////////////////////////////////////////////////////////////////////
// setParameter
//
// Sets the value of parameter specified by index
// value is a 32-bit float 0<=value<=1
//
// parameters:
// DWORD index - index of parameter 
// 32-bit float value
//
// return values (DWORD):
// FF_FAIL on error
//

DWORD plugClass::setParameter(SetParameterStruct* pParam)
{
	//check that the param index is valid
	if(pParam->index >= 0 && pParam->index < NUM_PARAMS)
	{
		paramDynamicData[pParam->index].value = pParam->value;
		return FF_SUCCESS;
	}
	else
	{
		return FF_FAIL;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// getParameter
//
// returns value of parameter specified by index as 32-bit float 0<=value<=1
//
// parameters:
// DWORD index - index of parameter 
//
// return values (32-bit float):
// 32-bit float value
// FF_FAIL on error
//

float plugClass::getParameter(DWORD index)
{
	//check that the param index is valid
	if(index >= 0 && index < NUM_PARAMS)
		return paramDynamicData[index].value;
	else
		return (float)FF_FAIL;
}

///////////////////////////////////////////////////////////////////////////////////////
// processFrame
//
// process a frame of video
//
// parameters:
// 32-bit pointer to byte array containing frame of video
//
// return values (DWORD):
// FF_SUCCESS
// FF_FAIL on error
//

DWORD plugClass::processFrame(LPVOID pFrame)
{
	switch (videoInfo.bitDepth) {
		case 1:
			return processFrame24Bit(pFrame);
		case 2:
			return processFrame32Bit(pFrame);
		default:
			return FF_FAIL;
	}

}

///////////////////////////////////////////////////////////////////////////////////////
// processFrame24Bit
//
// process a frame of 24 bit video
//
// parameters:
// 32-bit pointer to byte array containing frame of video
//
// return values (DWORD):
// FF_SUCCESS
// FF_FAIL on error
//

DWORD plugClass::processFrame24Bit(LPVOID pFrame)
{
	VideoPixel24bit* pPixel = (VideoPixel24bit*) pFrame;
	for (DWORD x = 0; x < videoInfo.frameWidth; x++) {
	  for (DWORD y = 0; y < videoInfo.frameHeight; y++) {
	    // this is very slow! Should be a lookup table

		pPixel->red = (BYTE) (pPixel->red * paramDynamicData[0].value);
	    pPixel->green = (BYTE) (pPixel->green * paramDynamicData[1].value);
	    pPixel->blue = (BYTE) (pPixel->blue * paramDynamicData[2].value);

		pPixel++;
	  }
	}

	return FF_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////
// processFrame32Bit
//
// process a frame of 32 bit video
//
// parameters:
// 32-bit pointer to byte array containing frame of video
//
// return values (DWORD):
// FF_SUCCESS
// FF_FAIL on error
//

DWORD plugClass::processFrame32Bit(LPVOID pFrame)
{
	VideoPixel32bit* pPixel = (VideoPixel32bit*) pFrame;
	for (DWORD x = 0; x < videoInfo.frameWidth; x++) {
	  for (DWORD y = 0; y < videoInfo.frameHeight; y++) {
	    // this is very slow! Should be a lookup table
	    pPixel->red = (BYTE) (pPixel->red * paramDynamicData[0].value);
	    pPixel->green = (BYTE) (pPixel->green * paramDynamicData[1].value);
	    pPixel->blue = (BYTE) (pPixel->blue * paramDynamicData[2].value);
	    pPixel++;
	  }
	}

	return FF_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////
// processFrameCopy
//
// processes a frame of video from one buffer to another 
//
// parameters:
// 32-bit pointer to a structure containing a pointer to an array of input
// buffers, the number of input buffers, and a pointer to an output frame
// buffer
//
// return values (DWORD):
// FF_SUCCESS
// FF_FAIL on error
//

DWORD plugClass::processFrameCopy(ProcessFrameCopyStruct* pFrameData)
{
	if (pFrameData->numInputFrames<NUM_INPUTS) {
		return FF_FAIL;
	}

	switch (videoInfo.bitDepth) {
		case 1:
			return processFrameCopy24Bit(pFrameData);
		case 2:
			return processFrameCopy32Bit(pFrameData);
		default:
			return FF_FAIL;
	}

}

///////////////////////////////////////////////////////////////////////////////////////
// processFrameCopy24Bit
//
// process a frame of 24 bit video
//
// parameters:
// 32-bit pointer to byte array containing frame of video
//
// return values (DWORD):
// FF_SUCCESS
// FF_FAIL on error
//

DWORD plugClass::processFrameCopy24Bit(ProcessFrameCopyStruct* pFrameData)
{
	VideoPixel24bit* pInputPixel = (VideoPixel24bit*) pFrameData->InputFrames[0];
	VideoPixel24bit* pOutputPixel = (VideoPixel24bit*) pFrameData->OutputFrame;
	for (DWORD x = 0; x < videoInfo.frameWidth; x++) {
	  for (DWORD y = 0; y < videoInfo.frameHeight; y++) {
	    // this is very slow! Should be a lookup table
	    pOutputPixel->red = (BYTE) (pInputPixel->red * paramDynamicData[0].value);
	    pOutputPixel->green = (BYTE) (pInputPixel->green * paramDynamicData[1].value);
	    pOutputPixel->blue = (BYTE) (pInputPixel->blue * paramDynamicData[2].value);
	    pInputPixel++;
		pOutputPixel++;
	  }
	}

	return FF_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////
// processFrameCopy32Bit
//
// process a frame of 32 bit video
//
// parameters:
// 32-bit pointer to byte array containing frame of video
//
// return values (DWORD):
// FF_SUCCESS
// FF_FAIL on error
//

DWORD plugClass::processFrameCopy32Bit(ProcessFrameCopyStruct* pFrameData)
{
	VideoPixel32bit* pInputPixel = (VideoPixel32bit*) pFrameData->InputFrames[0];
	VideoPixel32bit* pOutputPixel = (VideoPixel32bit*) pFrameData->OutputFrame;
	for (DWORD x = 0; x < videoInfo.frameWidth; x++) {
	  for (DWORD y = 0; y < videoInfo.frameHeight; y++) {
	    // this is very slow! Should be a lookup table
	    pOutputPixel->red = (BYTE) (pInputPixel->red * paramDynamicData[0].value);
	    pOutputPixel->green = (BYTE) (pInputPixel->green * paramDynamicData[1].value);
	    pOutputPixel->blue = (BYTE) (pInputPixel->blue * paramDynamicData[2].value);
	    pInputPixel++;
		pOutputPixel++;
	  }
	}
	
	return FF_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////
// getpluginCaps
//
// returns true or false to indicate whether cappable of feature specified by index
//
// parameters:
// DWORD index - index of parameter 
// allowed values:
// 0 - 16 bit video
// 1 - 24 bit video
// 2 - 32 bit video
//
// return values (DWORD):
// FF_TRUE
// FF_FALSE
//

DWORD getPluginCaps(DWORD index)
{
	switch (index) {

	case FF_CAP_16BITVIDEO:
		return FF_FALSE;

	case FF_CAP_24BITVIDEO:
		return FF_TRUE;

	case FF_CAP_32BITVIDEO:
		return FF_TRUE;

	case FF_CAP_PROCESSFRAMECOPY:
		return FF_FALSE;

	case FF_CAP_MINIMUMINPUTFRAMES:
		return NUM_INPUTS;

	case FF_CAP_MAXIMUMINPUTFRAMES:
		return NUM_INPUTS;

	case FF_CAP_COPYORINPLACE:
		return FF_FALSE;

	default:
		return FF_FALSE;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// instantiate
//
// Run up plugin instance - plugObj
// Prepare the Plug-in instance for processing.  
// Set default values, allocate memory
// When the plug-in returns from this function it must be ready to proces a frame
//
// return values (pointer to a plugObj - FF Dword Instance ID)
// FF_FAIL
//
// HOST: This function *must* return before a call to processFrame

LPVOID instantiate(VideoInfoStruct* pVideoInfo)
{

	// Create local pointer to plugObject
	plugClass *pPlugObj;

	// create new instance of plugClassq
	pPlugObj = new plugClass;

	// make a copy of the VideoInfoStruct
	pPlugObj->videoInfo.frameWidth = pVideoInfo->frameWidth;
	pPlugObj->videoInfo.frameHeight = pVideoInfo->frameHeight;
	pPlugObj->videoInfo.bitDepth = pVideoInfo->bitDepth;

	// this shouldn't happen if the host is checking the capabilities properly
	pPlugObj->vidmode = pPlugObj->videoInfo.bitDepth;
	if (pPlugObj->vidmode >2 || pPlugObj->vidmode < 0) {
	  return (LPVOID) FF_FAIL;
	}

	pPlugObj->paramDynamicData[0].value = 0.5f;
	pPlugObj->paramDynamicData[1].value = 0.5f;
	pPlugObj->paramDynamicData[2].value = 0.5f;

	// Russell - return pointer to the plugin instance object we have created

	// return pointer to object cast as LPVOID 
	return (LPVOID) pPlugObj;
}

///////////////////////////////////////////////////////////////////////////////////////
// deInstantiate
//
// Run down plugin instance
//
// Deallocate memory used by this instance

DWORD deInstantiate(LPVOID instanceID)
{

	// declare pPlugObj - pointer to this instance
	plugClass *pPlugObj;

	// typecast LPVOID into pointer to a plugClass
	pPlugObj = (plugClass*) instanceID;

	delete pPlugObj; // todo: ? success / fail?
	
	return FF_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////
// getExtendedInfo
//

LPVOID getExtendedInfo()
{

	plugExtInfo.PluginMajorVersion = 1;
	plugExtInfo.PluginMinorVersion = 10;

	// I'm just passing null for description etc for now
	// todo: send through description and about
	plugExtInfo.Description = NULL;
	plugExtInfo.About = NULL;

	// FF extended data block is not in use by the API yet
	// we will define this later if we want to
	plugExtInfo.FreeFrameExtendedDataSize = 0;
	plugExtInfo.FreeFrameExtendedDataBlock = NULL;

	return (LPVOID) &plugExtInfo;

	////////////////////////////////////////////////////

}