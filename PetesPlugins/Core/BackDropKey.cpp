/* Copyright (C) 2002 Pete Warden

Effect plugin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the Lesser GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

/*
	The idea behind this effect is to create a matte that corresponds to
	everything in the image that isn't part of the background. This is done
	by taking a reference frame when the capture frame variable is on, and
	then for every subsequent frame, subtracting the RGB channels of every
	pixel from the same pixel in the reference frame, squaring each channels
	result, and then summing them to get a difference metric. This metric is
	then compared against a threshold value to determine if it's different
	enough from the background to be marked white as foreground in the matte,
	or left black to represent the background.

*/

#include "BackDropKey.h"
#include "PeteHelpers.h"

static SPete_Parameter g_Parameters[]={
	{
		"Capture Frame",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Difference Threshold",
		48.0f,
		0.0f,
		255.0f,
		PETE_PARAM_FLOAT
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_BackDropKey_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

void Pete_BackDropKey_StoreBackDrop(SPete_BackDropKey_Data* pInstanceData,U32* pInput);

int Pete_BackDropKey_Init(SPete_BackDropKey_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_BackDropKey_DeInit(pInstanceData);

	const int nNumPixels=nWidth*nHeight;
	const int nNumBytes=(nNumPixels*sizeof(U32));

	pInstanceData->hBackDropBuffer=Pete_NewHandle(nNumBytes);

	if (pInstanceData->hBackDropBuffer==NULL) {
		Pete_BackDropKey_DeInit(pInstanceData);
		return 0;
	}

	pInstanceData->bBackDropBufferFilled=false;

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_BackDropKey_DeInit(SPete_BackDropKey_Data* pInstanceData) {

	if (pInstanceData->hBackDropBuffer!=NULL) {
		Pete_FreeHandle(pInstanceData->hBackDropBuffer);
		pInstanceData->hBackDropBuffer=NULL;
	}

}

void Pete_BackDropKey_Render(SPete_BackDropKey_Data* pInstanceData,SPete_BackDropKey_Settings* pSettings,U32* pSource,U32* pOutput) {

	if ((!pInstanceData->bBackDropBufferFilled)||
		(pSettings->m_UseThisFrameAsBackDrop>0.0f)) {

		Pete_BackDropKey_StoreBackDrop(pInstanceData,pSource);

		pInstanceData->bBackDropBufferFilled=true;

	}

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nNumPixels=nWidth*nHeight;
	const int nNumBytes=(nNumPixels*sizeof(U32));

	const int nThreshold=static_cast<int>(pSettings->m_Threshold);
	const int nThresholdSqrd=(nThreshold*nThreshold);

	U32* pBackDropFrame=(U32*)Pete_LockHandle(pInstanceData->hBackDropBuffer);
	if (pBackDropFrame==NULL) {
		return;
	}

	U32* pCurrentSource=pSource;
	U32* pCurrentOutput=pOutput;
	U32* pCurrentBackDrop=pBackDropFrame;
	const U32* pOutputEnd=(pOutput+nNumPixels);

	while (pCurrentOutput!=pOutputEnd) {

		const U32 SourceColour=*pCurrentSource;
		const U32 BackDropColour=*pCurrentBackDrop;

		const int nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
		const int nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
		const int nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;

		const int nBackDropRed=(BackDropColour>>SHIFT_RED)&0xff;
		const int nBackDropGreen=(BackDropColour>>SHIFT_GREEN)&0xff;
		const int nBackDropBlue=(BackDropColour>>SHIFT_BLUE)&0xff;

		const int nDiffRed=(nSourceRed-nBackDropRed);
		const int nDiffGreen=(nSourceGreen-nBackDropGreen);
		const int nDiffBlue=(nSourceBlue-nBackDropBlue);

		const int nDiffRedSqrd=(nDiffRed*nDiffRed);
		const int nDiffGreenSqrd=(nDiffGreen*nDiffGreen);
		const int nDiffBlueSqrd=(nDiffBlue*nDiffBlue);

		const int nTotalDiffSqrd=
			(nDiffRedSqrd)+
			(nDiffGreenSqrd)+
			(nDiffBlueSqrd);

		if (nTotalDiffSqrd<nThresholdSqrd) {
			*pCurrentOutput=0;
		} else {
			*pCurrentOutput=0xffffffff;
		}

		pCurrentOutput+=1;
		pCurrentSource+=1;
		pCurrentBackDrop+=1;

	}

	Pete_UnLockHandle(pInstanceData->hBackDropBuffer);

}

void Pete_BackDropKey_StoreBackDrop(SPete_BackDropKey_Data* pInstanceData,U32* pInput) {

	const int nNumPixels = (pInstanceData->nWidth*pInstanceData->nHeight);
	const int nNumBytes=(nNumPixels*sizeof(U32));

	U32* pBackDropBuffer=(U32*)Pete_LockHandle(pInstanceData->hBackDropBuffer);
	if (pBackDropBuffer==NULL) {
		return;
	}

	memcpy(pBackDropBuffer,pInput,nNumBytes);

	Pete_UnLockHandle(pInstanceData->hBackDropBuffer);

}
