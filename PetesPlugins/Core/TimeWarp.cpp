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


#include "TimeWarp.h"
#include "PeteHelpers.h"

#include <time.h>

enum ETimeWarpStyle {
	ePolariStyle,
	eSineSlurStyle,
	eStutterStyle,
	eCustomStyle,
	eTimeWarpStyle_end
};

void Pete_TimeWarp_StoreInputFrame(SPete_TimeWarp_Data* pInstanceData,U32* pInput);
void Pete_TimeWarp_UpdatePlaybackOffset(SPete_TimeWarp_Data* pInstanceData,SPete_TimeWarp_Settings* pSettings);

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
		"Style",
		0.0f,
		0.0f,
		3.0f,
		PETE_PARAM_FLOAT
	}
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_TimeWarp_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_TimeWarp_Init(SPete_TimeWarp_Data* pInstanceData,int nWidth, int nHeight,int nFramesToStore) {

	Pete_TimeWarp_DeInit(pInstanceData);

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
		Pete_TimeWarp_DeInit(pInstanceData);
		return 0;
	}

	return 1;

}

void Pete_TimeWarp_DeInit(SPete_TimeWarp_Data* pInstanceData) {

	if (pInstanceData->hStoredFrames!=NULL) {
		Pete_FreeHandle(pInstanceData->hStoredFrames);
		pInstanceData->hStoredFrames=NULL;
	}

}

void Pete_TimeWarp_Render(SPete_TimeWarp_Data* pInstanceData,SPete_TimeWarp_Settings* pSettings,U32* pSource,U32* pOutput) {

	if (pInstanceData->hStoredFrames==NULL) {
		return;
	}

	Pete_TimeWarp_StoreInputFrame(pInstanceData,pSource);

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nNumPixels=nWidth*nHeight;
	const int nNumBytes=(nNumPixels*sizeof(U32));

	U32* pStoredFrames=(U32*)Pete_LockHandle(pInstanceData->hStoredFrames);
	if (pStoredFrames==NULL) {
		return;
	}

	int nCurrentPlaybackOffset=static_cast<int>(pInstanceData->PlaybackOffset);

	nCurrentPlaybackOffset=GateInt(nCurrentPlaybackOffset,0,(pInstanceData->nCurrentStoredFramesCount-1));

	int nCurrentPlaybackFrame=
		(pInstanceData->nCurrentStorageFrame)-nCurrentPlaybackOffset;

	if (nCurrentPlaybackFrame<0) {
		nCurrentPlaybackFrame+=pInstanceData->nCurrentStoredFramesCount;
	}

	U32*const pStoredSource=pStoredFrames+(nNumPixels*nCurrentPlaybackFrame);

	memcpy(pOutput,pStoredSource,nNumBytes);

	Pete_TimeWarp_UpdatePlaybackOffset(pInstanceData,pSettings);

}

void Pete_TimeWarp_StoreInputFrame(SPete_TimeWarp_Data* pInstanceData,U32* pInput) {

	const int nNumPixels = pInstanceData->nWidth*pInstanceData->nHeight;

	assert((pInstanceData->nCurrentStorageFrame>=0)&&
		(pInstanceData->nCurrentStorageFrame<pInstanceData->nMaxStoredFramesCount));

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

void Pete_TimeWarp_UpdatePlaybackOffset(SPete_TimeWarp_Data* pInstanceData,SPete_TimeWarp_Settings* pSettings) {

	int nStyle=(int)(pSettings->m_Style);
	ETimeWarpStyle eStyle=(ETimeWarpStyle)(nStyle);

	switch (eStyle) {

		case ePolariStyle: {

			const float TimePerBeat=
				(60.0f/pSettings->m_PolariBPM);

			const int nTicksPerBeat=TimePerBeat*CLOCKS_PER_SEC;

			int nCurrentTime=clock();
			int nElapsedTime=
				(nCurrentTime-pInstanceData->nLastPolariTime);

			float ElapsedTime=
				(nElapsedTime/(float)(CLOCKS_PER_SEC));

			if ((ElapsedTime>TimePerBeat)||(pSettings->m_PolariDoLoop>0.0f)) {

				if (!(pSettings->m_PolariDoLoop>0.0f)) {
					pInstanceData->nLastPolariTime=
						(nCurrentTime/nTicksPerBeat)*nTicksPerBeat;
				}

				pInstanceData->PlaybackOffset=0.0f;

			}

			float FramesCount=static_cast<float>(pInstanceData->nCurrentStoredFramesCount);
			if (FramesCount==0.0f) {
				FramesCount=1.0f;
			}

			pInstanceData->PlaybackOffset+=2.0f;
			if (pInstanceData->PlaybackOffset>=pInstanceData->nCurrentStoredFramesCount) {
				pInstanceData->PlaybackOffset=fmodf(pInstanceData->PlaybackOffset,FramesCount);
			}

			if (pInstanceData->PlaybackOffset<0.0f) {
				pInstanceData->PlaybackOffset=FramesCount-fmodf(-pInstanceData->PlaybackOffset,FramesCount);
			}
						   
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

			const float RandomFloat=rand()/static_cast<float>(RAND_MAX);

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

