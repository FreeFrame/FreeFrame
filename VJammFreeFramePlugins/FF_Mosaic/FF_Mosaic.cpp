///////////////////////////////////////////////////////////////////////////////////
// FF_Mosaic.cpp
//
// FreeFrame Open Video Plugin 
// C Version
//
// Implementation of the Difference plugin
//
// www.freeframe.org
// marcus@freeframe.org

/*

Copyright (c) 2002, Marcus Clements www.freeframe.org
All rights reserved.

This software is licensed under the GNU LESSER GENERAL PUBLIC LICENSE
See LGPLLicense.txt included with these files

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

///////////////////////////////////////////////////////////////////////////////////////////
//
// includes 
//

#include "FF_Mosaic.h"

#include <stdio.h>

#define NUM_PARAMS 2

/////////////////////////////////////////////////////
//
// Plugin Globals
//
// these are defined globally in this sample plugin for simplicities sake
// Plugin developers should allocate memory from the heap for this stuff
//

PlugInfoStruct plugInfo;
VideoInfoStruct videoInfo;
ParameterStruct parameters[NUM_PARAMS];
int blockWidth;
int blockHeight;


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
	plugInfo.APIMajorVersion = 0;
	plugInfo.APIMinorVersion = 500; // lets keep this in sync with the delphi host for now
	char ID[5] = "FFM1";		 // this *must* be unique to your plugin 
								 // see www.freeframe.org for a list of ID's already taken
	char name[17] = "Mosaic 1";
	
	memcpy(plugInfo.uniqueID, ID, 4);
	memcpy(plugInfo.pluginName, name, 16);
	plugInfo.pluginType = FF_EFFECT;

	return &plugInfo;
}

///////////////////////////////////////////////////////////////////////////////////////
// initialise
//
// Prepare the Plug-in for processing.  
// Set default values, allocate memory
// When the plug-in returns from this function it must be ready to proces a frame
//
// return values (DWORD)
// FF_SUCCESS - success
// non-zero - fail (error values to be defined)
//
// HOST: This function *must* return before a call to processFrame

DWORD initialise(VideoInfoStruct* pVideoInfo)
{
	// make a copy of the VideoInfoStruct
	videoInfo.frameWidth = pVideoInfo->frameWidth;
	videoInfo.frameHeight = pVideoInfo->frameHeight;
	videoInfo.bitDepth = pVideoInfo->bitDepth;

	// this shouldn't happen if the host is checking the capabilities properly
	if (videoInfo.bitDepth != 1)
	{
		return FF_FAIL;
	}

	// populate the parameters structs
	parameters[0].defaultValue = 0.0f;
	parameters[1].defaultValue = 0.0f;
	//parameters[2].defaultValue = 0.5f;
	parameters[0].value = parameters[0].defaultValue;
	parameters[1].value = parameters[1].defaultValue;
	//parameters[2].value = 0.5f;
	char tempName1[17] = "width";
	char tempName2[17] = "height";
	//char tempName3[17] = "blue";
	memcpy(parameters[0].name, tempName1, 16);
	memcpy(parameters[1].name, tempName2, 16);
	//memcpy(parameters[2].name, tempName3, 16);

	blockHeight = blockWidth = 1;

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
// FF_FAIL on error
//

char* getParameterName(DWORD index)
{
	return parameters[index].name;
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
	return parameters[index].defaultValue;
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
// FF_FAIL on error
//

char* getParameterDisplay(DWORD index)
{
	try {
	int iValue;
	// fill the array with spaces first
	for (int n=0; n<16; n++) {
		parameters[index].displayValue[n] = ' ';
	}

	switch (index) {
	case 0:
		iValue = blockWidth;
		break;
	case 1:
		iValue = blockHeight;
		break;
	default:
		iValue = 0;
	}
	sprintf(parameters[index].displayValue, "%d",iValue);
	return parameters[index].displayValue;
	}
	catch (...) {
	}
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

DWORD setParameter(SetParameterStruct* pParam)
{
	try {
		parameters[pParam->index].value = pParam->value;
		switch (pParam->index) {
		case 0:
			blockWidth = (int) ((pParam->value * (videoInfo.frameWidth-1)) + 1);
			break;
		case 1:
			blockHeight = (int) ((pParam->value * (videoInfo.frameHeight-1)) + 1);
			break;
		default:
			return FF_FAIL;
		}
		return FF_SUCCESS;
	}
	catch (...) {
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

float getParameter(DWORD index)
{
	return parameters[index].value;
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

DWORD processFrame(LPVOID pFrame)
{
	try {
		BYTE *pBuffer = (BYTE*) pFrame;
		BYTE *tempPtr;
		DWORD x=0;
		DWORD y=0;
		
		long averageBlue = 0;
		long averageGreen = 0;
		long averageRed = 0;
		
		VideoPixel24bit avPixel;
		DWORD sx, sy;
		
		
		DWORD tempBlockWidth, tempBlockHeight;
		
		if (blockWidth == 1 && blockHeight == 1)
			return FF_SUCCESS;
		
		for (y = 0; y < videoInfo.frameHeight; y+=blockHeight) {
			if (videoInfo.frameHeight - y < blockHeight)
				tempBlockHeight = videoInfo.frameHeight - y;
			else
				tempBlockHeight = blockHeight;
			for (x = 0; x < videoInfo.frameWidth; x+=blockWidth) {
				tempPtr = pBuffer;
				if (videoInfo.frameWidth - x < blockWidth)
					tempBlockWidth = videoInfo.frameWidth - x;
				else
					tempBlockWidth = blockWidth;
				DWORD byteWidth;
				for (sy=0; sy<tempBlockHeight; sy++) {
					for (sx=0; sx<tempBlockWidth; sx++) {
						byteWidth = sx * 3;
						averageBlue += *(tempPtr + byteWidth);
						averageGreen += *(tempPtr + byteWidth + 1); 
						averageRed += *(tempPtr + byteWidth + 2);
					}
					tempPtr += videoInfo.frameWidth * 3;
				}
				tempPtr = pBuffer;
				int numPixels = tempBlockHeight * tempBlockWidth;
				avPixel.blue = (BYTE) (averageBlue / numPixels);
				avPixel.green = (BYTE) (averageGreen / numPixels);
				avPixel.red = (BYTE) (averageRed / numPixels);
				averageBlue = averageGreen = averageRed = 0;
				for (sy=0; sy<tempBlockHeight; sy++) {
					for (sx=0; sx<tempBlockWidth; sx++) {
						byteWidth = sx * 3;
						*(tempPtr + byteWidth) = avPixel.blue;
						*(tempPtr + byteWidth + 1) = avPixel.green;
						*(tempPtr + byteWidth + 2) = avPixel.red;
					}
					tempPtr += videoInfo.frameWidth * 3;
				}
				pBuffer += tempBlockWidth * 3;
			}
			pBuffer += videoInfo.frameWidth*(tempBlockHeight -1) * 3;
		}
		
		return FF_SUCCESS;
	}
	catch (...) {
	}
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
	case 0:
		return FF_FALSE;
	case 1:
		return FF_TRUE;
	case 2:
		return FF_FALSE;
	default:
		return FF_FALSE;
	}
}

//----------------------------------------------------------------------------------------------------------
// 
float powerToFrac(int power)
{
    float returnParm;
    if      (power <= 1) returnParm = (float) 0.0;
    else if (power == 2) returnParm = (float) 0.15;
    else if (power == 4) returnParm = (float) 0.3;
    else if (power == 8) returnParm = (float) 0.45;
    else if (power == 16) returnParm = (float) 0.6;
    else if (power == 32) returnParm = (float) 0.75;
    else if (power == 64) returnParm = (float) 0.9;
    else  returnParm = (float) 0.9;
    return returnParm;
}

//----------------------------------------------------------------------------------------------------------
// Slider moved changing number of slices
int fracToPower(float frac)
{
    
    if      (frac < 0.15) return 1;
    else if (frac < 0.3) return 2;
    else if (frac < 0.45) return 4;
    else if (frac < 0.6) return 8;
    else if (frac < 0.75) return 16;
    else if (frac < 0.9) return 32;
    else  return 64;
}
