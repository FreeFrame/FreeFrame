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


#include "TimeSlice.h"
#include "PeteHelpers.h"

enum ETimeSliceStyle {
	ePolariStyle,
	eSineSlurStyle,
	eStutterStyle,
	eCustomStyle,
	eTimeSliceStyle_end
};

void Pete_TimeSlice_StoreInputFrame(SPete_TimeSlice_Data* pInstanceData,U32* pInput);
void Pete_TimeSlice_UpdatePlaybackOffset(SPete_TimeSlice_Data* pInstanceData,SPete_TimeSlice_Settings* pSettings,float Time);
void Pete_TimeSlice_CopyRectangle(SPete_TimeSlice_Data* pInstanceData,U32* pInput,U32* pOutput,int nLeftX,int nTopY,int nRightX,int nBottomY);

static SPete_Parameter g_Parameters[]={
	{
		"SineSlur Period",
		30.0f,
		0.0f,
		300.0f,
		PETE_PARAM_FLOAT
	},
	{
		"SineSlur Amplitude",
		0.5f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Stutter Amplitude",
		1.0f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Custom Playback Offset",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Horizontal",
		1.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Slice Size",
		2.0f,
		1.0f,
		100.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Style",
		0.0f,
		0.0f,
		3.0f,
		PETE_PARAM_FLOAT
	}
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_TimeSlice_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_TimeSlice_Init(SPete_TimeSlice_Data* pInstanceData,int nWidth, int nHeight,int nFramesToStore) {

	Pete_TimeSlice_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;
	pInstanceData->nMaxStoredFramesCount=nFramesToStore;

	pInstanceData->nCurrentSlice=0;
	pInstanceData->nCurrentStorageFrame=0;
	pInstanceData->nCurrentStoredFramesCount=0;
	pInstanceData->PlaybackOffset=0.0f;

	pInstanceData->hStoredFrames=
		Pete_NewHandle(nWidth*nHeight*nFramesToStore*sizeof(U32));
	if (pInstanceData->hStoredFrames==NULL) {
		Pete_TimeSlice_DeInit(pInstanceData);
		return 0;
	}

	return 1;

}

void Pete_TimeSlice_DeInit(SPete_TimeSlice_Data* pInstanceData) {

	if (pInstanceData->hStoredFrames!=NULL) {
		Pete_FreeHandle(pInstanceData->hStoredFrames);
		pInstanceData->hStoredFrames=NULL;
	}

}

void Pete_TimeSlice_Render(SPete_TimeSlice_Data* pInstanceData,SPete_TimeSlice_Settings* pSettings,U32* pSource,U32* pOutput) {

	if (pInstanceData->hStoredFrames==NULL) {
		return;
	}

	Pete_TimeSlice_StoreInputFrame(pInstanceData,pSource);

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nNumPixels=nWidth*nHeight;
	const int nNumBytes=(nNumPixels*sizeof(U32));

	U32* pStoredFrames=(U32*)Pete_LockHandle(pInstanceData->hStoredFrames);
	if (pStoredFrames==NULL) {
		return;
	}

	bool bIsDone=false;
	int nSliceStart=0;
	float Time=0.0f;
	while (!bIsDone) {

		Pete_TimeSlice_UpdatePlaybackOffset(pInstanceData,pSettings,Time);
		Time+=1.0f;

		int nCurrentPlaybackOffset=static_cast<int>(pInstanceData->PlaybackOffset);

		nCurrentPlaybackOffset=GateInt(nCurrentPlaybackOffset,0,(pInstanceData->nCurrentStoredFramesCount-1));

		int nCurrentPlaybackFrame=
			(pInstanceData->nCurrentStorageFrame)-nCurrentPlaybackOffset;

		if (nCurrentPlaybackFrame<0) {
			nCurrentPlaybackFrame+=pInstanceData->nCurrentStoredFramesCount;
		}

		U32*const pStoredSource=pStoredFrames+(nNumPixels*nCurrentPlaybackFrame);
		
		int nLeftX;
		int nTopY;
		int nRightX;
		int nBottomY;
		if (pSettings->m_DoHorizontal) {

			nLeftX=0;
			nRightX=nWidth;

			nTopY=nSliceStart;
			nBottomY=nTopY+(int)(pSettings->m_SliceSize);

			nSliceStart+=(int)(pSettings->m_SliceSize);
			if (nSliceStart>=nHeight) {
				bIsDone=true;
			}

		} else {

			nTopY=0;
			nBottomY=nHeight;

			nLeftX=nSliceStart;
			nRightX=nLeftX+(int)(pSettings->m_SliceSize);

			nSliceStart+=(int)(pSettings->m_SliceSize);
			if (nSliceStart>=nWidth) {
				bIsDone=true;
			}

		}

		Pete_TimeSlice_CopyRectangle(pInstanceData,pStoredSource,pOutput,nLeftX,nTopY,nRightX,nBottomY);

	}

}

void Pete_TimeSlice_StoreInputFrame(SPete_TimeSlice_Data* pInstanceData,U32* pInput) {

	const int nNumPixels = pInstanceData->nWidth*pInstanceData->nHeight;

	assert((pInstanceData->nCurrentStorageFrame>=0)&&(pInstanceData->nCurrentStorageFrame<pInstanceData->nMaxStoredFramesCount));

	U32* pStoredFrames=(U32*)Pete_LockHandle(pInstanceData->hStoredFrames);
	if (pStoredFrames==NULL) {
		return;
	}

	U32*const pDest=pStoredFrames+(nNumPixels*pInstanceData->nCurrentStorageFrame);

	memcpy(pDest,pInput,(nNumPixels*sizeof(U32)));

	pInstanceData->nCurrentStoredFramesCount += 1;
	if (pInstanceData->nCurrentStoredFramesCount>=pInstanceData->nMaxStoredFramesCount) {
		pInstanceData->nCurrentStoredFramesCount=pInstanceData->nMaxStoredFramesCount;
	}

	pInstanceData->nCurrentStorageFrame +=1;
	if (pInstanceData->nCurrentStorageFrame>=pInstanceData->nMaxStoredFramesCount) {
		pInstanceData->nCurrentStorageFrame%=pInstanceData->nMaxStoredFramesCount;
	}

}

void Pete_TimeSlice_UpdatePlaybackOffset(SPete_TimeSlice_Data* pInstanceData,SPete_TimeSlice_Settings* pSettings,float Time) {

	int nStyle=(int)(pSettings->m_Style/eTimeSliceStyle_end);
	ETimeSliceStyle eStyle=(ETimeSliceStyle)(nStyle);

	switch (eStyle) {

		case ePolariStyle: {

			float FramesCount=static_cast<float>((pInstanceData->nCurrentStoredFramesCount-2.0f)*2);
			if (FramesCount==0.0f) {
				FramesCount=1.0f;
			}

			pInstanceData->PlaybackOffset=Time;
			if (pInstanceData->PlaybackOffset>=FramesCount) {
				pInstanceData->PlaybackOffset=fmodf(pInstanceData->PlaybackOffset,FramesCount);
			}

			if (pInstanceData->PlaybackOffset<0.0f) {
				pInstanceData->PlaybackOffset=FramesCount-fmodf(-pInstanceData->PlaybackOffset,FramesCount);
			}

			if (pInstanceData->PlaybackOffset>=(FramesCount/2.0f)) {
				pInstanceData->PlaybackOffset=FramesCount-pInstanceData->PlaybackOffset;
			}

			pInstanceData->PlaybackOffset+=1.0f;
						   
		}break;

		case eSineSlurStyle: {

			pInstanceData->SineSlurAngle+=(Pete_Pi/pSettings->m_SineSlurPeriod);
			pInstanceData->SineSlurAngle=fmodf(pInstanceData->SineSlurAngle,2.0f*Pete_Pi);
			
			const float HalfFramesCount=
				static_cast<float>(pInstanceData->nCurrentStoredFramesCount)/2.0f;
			
			pInstanceData->PlaybackOffset=HalfFramesCount+
				(sin(pInstanceData->SineSlurAngle)*HalfFramesCount*pSettings->m_SineSlurAmplitude);
							 
		}break;

		case eStutterStyle: {

			const float RandomFloat=GetRandomFloat();

			pInstanceData->PlaybackOffset=
				RandomFloat*pSettings->m_StutterAmplitude*static_cast<float>(pInstanceData->nCurrentStoredFramesCount);
							
		}break;

		case eCustomStyle: {

			pInstanceData->PlaybackOffset=
				pSettings->m_CustomPlaybackOffset*static_cast<float>(pInstanceData->nCurrentStoredFramesCount);
			
		}break;

		default: {
			assert(false);
		}break;

	}

}

void Pete_TimeSlice_CopyRectangle(SPete_TimeSlice_Data* pInstanceData,U32* pInput,U32* pOutput,int nLeftX,int nTopY,int nRightX,int nBottomY) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nStartX=GateInt(nLeftX,0,(nWidth-1));
	const int nEndX=GateInt(nRightX,0,(nWidth-1));

	const int nStartY=GateInt(nTopY,0,(nHeight-1));
	const int nEndY=GateInt(nBottomY,0,(nHeight-1));

	int nY;
	for (nY=nStartY; nY<nEndY; ++nY) {

		U32* pInputLineStart=pInput+(nY*nWidth);
		U32* pOutputLineStart=pOutput+(nY*nWidth);

		U32* pCurrentInput=pInputLineStart+nStartX;
		U32* pCurrentOutput=pOutputLineStart+nStartX;

		int nX;
		for (nX=nStartX; nX<nEndX; ++nX) {

			*pCurrentOutput=*pCurrentInput;

			++pCurrentInput;
			++pCurrentOutput;

		}

	}

}

