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

#include "TimeBlur.h"
#include "PeteHelpers.h"

#include "math.h"
#include <string.h>
#ifndef PETE_MAC_OSX
#include "mmintrin.h"
#endif // PETE_MAC_OSX


void Pete_TimeBlur_AddNewFrame(SPete_TimeBlur_Data* pInstanceData,U32* pSource);
void Pete_TimeBlur_CalcDisplayFrame(SPete_TimeBlur_Data* pInstanceData);
void Pete_TimeBlur_AddNewFrameAndSubtractOld(SPete_TimeBlur_Data* pInstanceData,U32* pSource,U32* pNewFrame,U32* pOldFrame);
void Pete_TimeBlur_SubtractOld(SPete_TimeBlur_Data* pInstanceData,U32* pOldFrame);
void Pete_TimeBlur_AddNew(SPete_TimeBlur_Data* pInstanceData,U32* pNewFrame);
void Pete_TimeBlur_StoreNewFrame(SPete_TimeBlur_Data* pInstanceData,U32* pSource,U32* pNewFrame);

static SPete_Parameter g_Parameters[]={
	{
		"Time",
		0.2f,
		0.07f,
		1.0f,
		PETE_PARAM_FLOAT
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

const int nMaxFrameCount=50;

void Pete_TimeBlur_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

#ifdef PETE_MAC_OSX
#define PETE_USE_C
#endif // PETE_MAC_OSX

int Pete_TimeBlur_Init(SPete_TimeBlur_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_TimeBlur_DeInit(pInstanceData);

	const int nPixelCount=(nWidth*nHeight);

	const int nAverageFrameByteCount=
		(nPixelCount*8);

	pInstanceData->hAverageFrame=
		Pete_NewHandle(nAverageFrameByteCount);

	if (pInstanceData->hAverageFrame==NULL) {
		Pete_TimeBlur_DeInit(pInstanceData);
		return 0;
	}

	Pete_ZeroMemory((char*)Pete_LockHandle(pInstanceData->hAverageFrame),nAverageFrameByteCount);
	
	const int nDisplayFrameByteCount=
		(nPixelCount*sizeof(U32))+4; // Pete- +4 to let the quad writes spill over

	pInstanceData->hDisplayFrame=
		Pete_NewHandle(nDisplayFrameByteCount);

	if (pInstanceData->hDisplayFrame==NULL) {
		Pete_TimeBlur_DeInit(pInstanceData);
		return 0;
	}
	
	if (Pete_LockHandle(pInstanceData->hDisplayFrame)==NULL) {
		return 0;
	}

	Pete_ZeroMemory((char*)Pete_LockHandle(pInstanceData->hDisplayFrame),nDisplayFrameByteCount);

	const int nFrameBufferByteCount=
		(nPixelCount*sizeof(U32)*nMaxFrameCount);

	pInstanceData->hFrameBuffer=
		Pete_NewHandle(nFrameBufferByteCount);
	if (pInstanceData->hFrameBuffer==NULL) {
		Pete_TimeBlur_DeInit(pInstanceData);
		return 0;
	}
	if (Pete_LockHandle(pInstanceData->hFrameBuffer)==NULL) {
		return 0;
	}
	Pete_ZeroMemory((char*)Pete_LockHandle(pInstanceData->hFrameBuffer),nFrameBufferByteCount);

	pInstanceData->nCurrentFrameIndex=0;
	pInstanceData->nOldFrameIndex=1;

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_TimeBlur_DeInit(SPete_TimeBlur_Data* pInstanceData) {

	if (pInstanceData->hAverageFrame!=NULL) {
		Pete_FreeHandle(pInstanceData->hAverageFrame);
		pInstanceData->hAverageFrame=NULL;
	}

	if (pInstanceData->hDisplayFrame!=NULL) {
		Pete_FreeHandle(pInstanceData->hDisplayFrame);
		pInstanceData->hDisplayFrame=NULL;
	}

	if (pInstanceData->hFrameBuffer!=NULL) {
		Pete_FreeHandle(pInstanceData->hFrameBuffer);
		pInstanceData->hFrameBuffer=NULL;
	}

}

#if 1
void Pete_TimeBlur_Render(SPete_TimeBlur_Data* pInstanceData,SPete_TimeBlur_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nPixelCount=(nWidth*nHeight);

	int nCurrentFrameIndex=pInstanceData->nCurrentFrameIndex;

	int nOldFrameIndex=pInstanceData->nOldFrameIndex;

	U32* pFrameBuffer=(U32*)Pete_LockHandle(pInstanceData->hFrameBuffer);
	if (pFrameBuffer==NULL) {
		return;
	}

	U32* pNewFrame=pFrameBuffer+
		(nPixelCount*nCurrentFrameIndex);

	U32* pOldFrame=pFrameBuffer+
		(nPixelCount*nOldFrameIndex);
	
	Pete_TimeBlur_AddNewFrameAndSubtractOld(pInstanceData,pSource,pNewFrame,pOldFrame);
	
	nOldFrameIndex+=1;

	if (nOldFrameIndex<0) {
		nOldFrameIndex+=nMaxFrameCount;
	}

	if (nOldFrameIndex>=nMaxFrameCount) {
		nOldFrameIndex-=nMaxFrameCount;
	}

	const float Time=pSettings->m_Time;
	const int nDesiredFrameOffset=
		(int)(Time*(nMaxFrameCount-1));

	int nCurrentFrameOffset;
	if (nCurrentFrameIndex>nOldFrameIndex) {
		nCurrentFrameOffset=
			(nCurrentFrameIndex-nOldFrameIndex);
	} else {
		nCurrentFrameOffset=
			(nCurrentFrameIndex-nOldFrameIndex)+nMaxFrameCount;
	}

	nCurrentFrameOffset=((nCurrentFrameOffset%nMaxFrameCount)+nMaxFrameCount)%nMaxFrameCount;

	while (nCurrentFrameOffset>nDesiredFrameOffset) {

		U32* pOldFrame=pFrameBuffer+
			(nPixelCount*nOldFrameIndex);

		Pete_TimeBlur_SubtractOld(pInstanceData,pOldFrame);

		nOldFrameIndex+=1;

		if (nOldFrameIndex<0) {
			nOldFrameIndex+=nMaxFrameCount;
		}

		if (nOldFrameIndex>=nMaxFrameCount) {
			nOldFrameIndex-=nMaxFrameCount;
		}
	
		nCurrentFrameOffset-=1;

	}

	while (nCurrentFrameOffset<nDesiredFrameOffset) {

		nOldFrameIndex-=1;

		if (nOldFrameIndex<0) {
			nOldFrameIndex+=nMaxFrameCount;
		}

		if (nOldFrameIndex>=nMaxFrameCount) {
			nOldFrameIndex-=nMaxFrameCount;
		}

		U32* pNewFrame=pFrameBuffer+
			(nPixelCount*nOldFrameIndex);

		Pete_TimeBlur_AddNew(pInstanceData,pNewFrame);
	
		nCurrentFrameOffset+=1;

	}

	Pete_TimeBlur_CalcDisplayFrame(pInstanceData);

	pInstanceData->nCurrentFrameIndex+=1;

	if (pInstanceData->nCurrentFrameIndex>=nMaxFrameCount) {
		pInstanceData->nCurrentFrameIndex=0;
	}

	if (nOldFrameIndex<0) {
		nOldFrameIndex+=nMaxFrameCount;
	}

	if (nOldFrameIndex>=nMaxFrameCount) {
		nOldFrameIndex-=nMaxFrameCount;
	}

	pInstanceData->nOldFrameIndex=nOldFrameIndex;

	const int nByteCount=(nPixelCount*sizeof(U32));
	
	U32* pDisplayFrame=(U32*)Pete_LockHandle(pInstanceData->hDisplayFrame);
	if (pDisplayFrame==NULL) {
		return;
	}

	memcpy(pOutput,pDisplayFrame,nByteCount);

}

#else

void Pete_TimeBlur_Render(SPete_TimeBlur_Data* pInstanceData,SPete_TimeBlur_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nPixelCount=(nWidth*nHeight);

	int nCurrentFrameIndex=pInstanceData->nCurrentFrameIndex;

	int nOldFrameIndex=pInstanceData->nOldFrameIndex;

	U32* pFrameBuffer=(U32*)Pete_LockHandle(pInstanceData->hFrameBuffer);
	if (pFrameBuffer==NULL) {
		return;
	}

	U32* pNewFrame=pFrameBuffer+
		(nPixelCount*nCurrentFrameIndex);

	U32* pOldFrame=pFrameBuffer+
		(nPixelCount*nOldFrameIndex);
	
	Pete_TimeBlur_AddNewFrameAndSubtractOld(pInstanceData,pSource,pNewFrame,pOldFrame);
	
	nOldFrameIndex+=1;

	if (nOldFrameIndex>=nMaxFrameCount) {
		nOldFrameIndex-=nMaxFrameCount;
	}

	const float Time=pSettings->m_Time;
	const int nDesiredFrameOffset=
		(int)(Time*(nMaxFrameCount-1));

	int nCurrentFrameOffset;
	if (nCurrentFrameIndex>nOldFrameIndex) {
		nCurrentFrameOffset=
			(nCurrentFrameIndex-nOldFrameIndex);
	} else {
		nCurrentFrameOffset=
			(nCurrentFrameIndex-nOldFrameIndex)+nMaxFrameCount;
	}

	nCurrentFrameOffset=((nCurrentFrameOffset%nMaxFrameCount)+nMaxFrameCount)%nMaxFrameCount;

	while (nCurrentFrameOffset>nDesiredFrameOffset) {

		U32* pOldFrame=pFrameBuffer+
			(nPixelCount*nOldFrameIndex);

		Pete_TimeBlur_SubtractOld(pInstanceData,pOldFrame);

		nOldFrameIndex+=1;

		if (nOldFrameIndex<0) {
			nOldFrameIndex+=nMaxFrameCount;
		}

		if (nOldFrameIndex>=nMaxFrameCount) {
			nOldFrameIndex-=nMaxFrameCount;
		}
	
		nCurrentFrameOffset-=1;

	}

	while (nCurrentFrameOffset<nDesiredFrameOffset) {

		nOldFrameIndex-=1;

		if (nOldFrameIndex<0) {
			nOldFrameIndex+=nMaxFrameCount;
		}

		if (nOldFrameIndex>=nMaxFrameCount) {
			nOldFrameIndex-=nMaxFrameCount;
		}

		U32* pNewFrame=pFrameBuffer+
			(nPixelCount*nOldFrameIndex);

		Pete_TimeBlur_AddNew(pInstanceData,pNewFrame);
	
		nCurrentFrameOffset+=1;

	}

	Pete_TimeBlur_CalcDisplayFrame(pInstanceData);

	pInstanceData->nCurrentFrameIndex+=1;

	if (pInstanceData->nCurrentFrameIndex>=nMaxFrameCount) {
		pInstanceData->nCurrentFrameIndex=0;
	}

	if (nOldFrameIndex<0) {
		nOldFrameIndex+=nMaxFrameCount;
	}

	if (nOldFrameIndex>=nMaxFrameCount) {
		nOldFrameIndex-=nMaxFrameCount;
	}

	pInstanceData->nOldFrameIndex=nOldFrameIndex;

	const int nByteCount=(nPixelCount*sizeof(U32));
	
	U32* pDisplayFrame=(U32*)Pete_LockHandle(pInstanceData->hDisplayFrame);
	if (pDisplayFrame==NULL) {
		return;
	}

	memcpy(pOutput,pDisplayFrame,nByteCount);

}
#endif

#ifndef PETE_USE_C

void Pete_TimeBlur_AddNewFrame(SPete_TimeBlur_Data* pInstanceData,U32* pSource) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nPixelCount=(nWidth*nHeight);

	U32* pSourceEnd=(pSource+nPixelCount);

	U32* pCurrentSource=pSource;
	__m64* pCurrentAverage=(__m64*)(pInstanceData->hAverageFrame);

	__asm {
		pxor		mm0,mm0
	}

	while (pCurrentSource<pSourceEnd) {

		__asm {

			mov			esi,pCurrentSource

			movq		mm1,[esi]

			mov			edi,pCurrentAverage

			punpcklbw	mm1,mm0

			movq		mm2,[edi]

			paddw		mm2,mm1

			movq		[edi],mm2

		}

		pCurrentSource+=1;
		pCurrentAverage+=1;

	}

	_m_empty();

}

void Pete_TimeBlur_CalcDisplayFrame(SPete_TimeBlur_Data* pInstanceData) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nPixelCount=(nWidth*nHeight);

	U32* pOutput=(U32*)Pete_LockHandle(pInstanceData->hDisplayFrame);

	U32* pOutputEnd=(pOutput+nPixelCount);

	U32* pCurrentOutput=pOutput;
	__m64* pCurrentAverage=(__m64*)(pInstanceData->hAverageFrame);

	const int nCurrentFrameIndex=pInstanceData->nCurrentFrameIndex;
	const int nOldFrameIndex=pInstanceData->nOldFrameIndex;

	int nCurrentFrameOffset;
	if (nCurrentFrameIndex>nOldFrameIndex) {
		nCurrentFrameOffset=
			(nCurrentFrameIndex-nOldFrameIndex);
	} else {
		nCurrentFrameOffset=
			(nCurrentFrameIndex-nOldFrameIndex)+nMaxFrameCount;
	}

	if (nCurrentFrameOffset<=0) {
		nCurrentFrameOffset=1;
	}

	const int nReciprocalFrameCount=
		((1<<16)/nCurrentFrameOffset);

	const __m64 RecipReg=
		_mm_set_pi16(
		nReciprocalFrameCount,
		nReciprocalFrameCount,
		nReciprocalFrameCount,
		nReciprocalFrameCount);

	__asm {
		pxor		mm0,mm0
		movq		mm1,RecipReg
	}

	while (pCurrentOutput<pOutputEnd) {

		__asm {

			mov			edi,pCurrentAverage

			mov			esi,pCurrentOutput

			movq		mm7,[edi]

			pmulhw		mm7,mm1

			packuswb	mm7,mm0

			movq		[esi],mm7

		}

		pCurrentOutput+=1;
		pCurrentAverage+=1;

	}

	_m_empty();

}

void Pete_TimeBlur_AddNewFrameAndSubtractOld(SPete_TimeBlur_Data* pInstanceData,U32* pSource,U32* pNewFrame,U32* pOldFrame) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nPixelCount=(nWidth*nHeight);

	U32* pSourceEnd=(pSource+nPixelCount);

	U32* pCurrentSource=pSource;
	__m64* pCurrentAverage=(__m64*)(pInstanceData->hAverageFrame);
	U32* pCurrentNewFrame=pNewFrame;
	U32* pCurrentOldFrame=pOldFrame;

	__asm {
		pxor		mm0,mm0
	}

	while (pCurrentSource<pSourceEnd) {

		__asm {

			mov			esi,pCurrentSource

			mov			eax,[esi]

			movq		mm1,[esi]

			mov			esi,pCurrentOldFrame

			movq		mm3,[esi]

			punpcklbw	mm3,mm0

			mov			edi,pCurrentAverage

			punpcklbw	mm1,mm0

			movq		mm2,[edi]

			paddw		mm2,mm1

			psubw		mm2,mm3

			movq		[edi],mm2

			mov			esi,pCurrentNewFrame

			mov			[esi],eax

		}

		pCurrentSource+=1;
		pCurrentAverage+=1;
		pCurrentNewFrame+=1;
		pCurrentOldFrame+=1;

	}

	_m_empty();

}

void Pete_TimeBlur_SubtractOld(SPete_TimeBlur_Data* pInstanceData,U32* pOldFrame) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nPixelCount=(nWidth*nHeight);

	U32* pOldFrameEnd=(pOldFrame+nPixelCount);

	__m64* pCurrentAverage=(__m64*)(pInstanceData->hAverageFrame);
	U32* pCurrentOldFrame=pOldFrame;

	__asm {
		pxor		mm0,mm0
	}

	while (pCurrentOldFrame<pOldFrameEnd) {

		__asm {

			mov			esi,pCurrentOldFrame

			movq		mm3,[esi]

			punpcklbw	mm3,mm0

			mov			edi,pCurrentAverage

			movq		mm2,[edi]

			psubw		mm2,mm3

			movq		[edi],mm2

		}

		pCurrentAverage+=1;
		pCurrentOldFrame+=1;

	}

	_m_empty();

}

void Pete_TimeBlur_AddNew(SPete_TimeBlur_Data* pInstanceData,U32* pNewFrame) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nPixelCount=(nWidth*nHeight);

	U32* pNewFrameEnd=(pNewFrame+nPixelCount);

	__m64* pCurrentAverage=(__m64*)(pInstanceData->hAverageFrame);
	U32* pCurrentNewFrame=pNewFrame;

	__asm {
		pxor		mm0,mm0
	}

	while (pCurrentNewFrame<pNewFrameEnd) {

		__asm {

			mov			esi,pCurrentNewFrame

			movq		mm3,[esi]

			punpcklbw	mm3,mm0

			mov			edi,pCurrentAverage

			movq		mm2,[edi]

			paddw		mm2,mm3

			movq		[edi],mm2

		}

		pCurrentAverage+=1;
		pCurrentNewFrame+=1;

	}

	_m_empty();

}

#else // PETE_MAC_OSX

struct SPete_Pixel64 {
	U16 Red;
	U16 Green;
	U16 Blue;
	U16 Alpha;
};

void Pete_TimeBlur_AddNewFrame(SPete_TimeBlur_Data* pInstanceData,U32* pSource) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nPixelCount=(nWidth*nHeight);

	U32* pSourceEnd=(pSource+nPixelCount);

	U32* pCurrentSource=pSource;
	SPete_Pixel64* pCurrentAverage=(SPete_Pixel64*)Pete_LockHandle(pInstanceData->hAverageFrame);

	while (pCurrentSource<pSourceEnd) {

		const U32 SourceColour=*pCurrentSource;

		const int nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
		const int nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
		const int nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;
		const int nSourceAlpha=(SourceColour>>SHIFT_ALPHA)&0xff;
		
		pCurrentAverage->Red+=nSourceRed;
		pCurrentAverage->Green+=nSourceGreen;
		pCurrentAverage->Blue+=nSourceBlue;
		pCurrentAverage->Alpha+=nSourceAlpha;

		pCurrentSource+=1;
		pCurrentAverage+=1;

	}

}

void Pete_TimeBlur_CalcDisplayFrame(SPete_TimeBlur_Data* pInstanceData) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nPixelCount=(nWidth*nHeight);

	U32* pOutput=(U32*)Pete_LockHandle(pInstanceData->hDisplayFrame);
	if (pOutput==NULL) {
		return;
	}

	U32* pOutputEnd=(pOutput+nPixelCount);

	U32* pCurrentOutput=pOutput;
	SPete_Pixel64* pCurrentAverage=(SPete_Pixel64*)Pete_LockHandle(pInstanceData->hAverageFrame);

	const int nCurrentFrameIndex=pInstanceData->nCurrentFrameIndex;
	const int nOldFrameIndex=pInstanceData->nOldFrameIndex;

	int nCurrentFrameOffset;
	if (nCurrentFrameIndex>nOldFrameIndex) {
		nCurrentFrameOffset=
			(nCurrentFrameIndex-nOldFrameIndex);
	} else {
		nCurrentFrameOffset=
			(nCurrentFrameIndex-nOldFrameIndex)+nMaxFrameCount;
	}

	if (nCurrentFrameOffset<=0) {
		nCurrentFrameOffset=1;
	}

	const int nReciprocalFrameCount=
		((1<<16)/nCurrentFrameOffset);

	while (pCurrentOutput<pOutputEnd) {

		const int nOutputRed=
			(pCurrentAverage->Red*nReciprocalFrameCount)>>16;

		const int nOutputGreen=
			(pCurrentAverage->Green*nReciprocalFrameCount)>>16;
		
		const int nOutputBlue=
			(pCurrentAverage->Blue*nReciprocalFrameCount)>>16;

		const int nOutputAlpha=
			(pCurrentAverage->Alpha*nReciprocalFrameCount)>>16;

		U32 OutputColour=
			(nOutputRed<<SHIFT_RED)|
			(nOutputGreen<<SHIFT_GREEN)|
			(nOutputBlue<<SHIFT_BLUE)|
			(nOutputAlpha<<SHIFT_ALPHA);
			
		*pCurrentOutput=OutputColour;

		pCurrentOutput+=1;
		pCurrentAverage+=1;

	}

}

void Pete_TimeBlur_AddNewFrameAndSubtractOld(SPete_TimeBlur_Data* pInstanceData,U32* pSource,U32* pNewFrame,U32* pOldFrame) {

	Pete_TimeBlur_StoreNewFrame(pInstanceData,pSource,pNewFrame);
	Pete_TimeBlur_AddNew(pInstanceData,pSource);
	Pete_TimeBlur_SubtractOld(pInstanceData,pOldFrame);
	
}

void Pete_TimeBlur_SubtractOld(SPete_TimeBlur_Data* pInstanceData,U32* pSource) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nPixelCount=(nWidth*nHeight);

	U32* pSourceEnd=(pSource+nPixelCount);

	U32* pCurrentSource=pSource;
	SPete_Pixel64* pCurrentAverage=(SPete_Pixel64*)Pete_LockHandle(pInstanceData->hAverageFrame);

	while (pCurrentSource<pSourceEnd) {

		const U32 SourceColour=*pCurrentSource;

		const int nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
		const int nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
		const int nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;
		const int nSourceAlpha=(SourceColour>>SHIFT_ALPHA)&0xff;
		
		pCurrentAverage->Red-=nSourceRed;
		pCurrentAverage->Green-=nSourceGreen;
		pCurrentAverage->Blue-=nSourceBlue;
		pCurrentAverage->Alpha-=nSourceAlpha;

		pCurrentSource+=1;
		pCurrentAverage+=1;

	}

}

void Pete_TimeBlur_StoreNewFrame(SPete_TimeBlur_Data* pInstanceData,U32* pSource,U32* pNewFrame) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;
	const int nPixelCount=(nWidth*nHeight);
	const int nByteCount=(nPixelCount*sizeof(U32));
	
	memcpy(pNewFrame,pSource,nByteCount);
	
}	

void Pete_TimeBlur_AddNew(SPete_TimeBlur_Data* pInstanceData,U32* pSource) {
	Pete_TimeBlur_AddNewFrame(pInstanceData,pSource);
}

#endif // PETE_MAC_OSX
