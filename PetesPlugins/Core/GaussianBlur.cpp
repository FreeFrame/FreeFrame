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

#include "GaussianBlur.h"

#include "math.h"

#include "PeteHelpers.h"
#include "BoxFilter.h"
#include "ImageMath.h"

float Pete_GaussianBlur_EvalFunc(float t,float A, float C);
int Pete_GaussianBlur_SetupKernel(float* pKernel,int nKernelMaxSize,float Radius,float EdgeFactor);
void Pete_DoGaussianBlur(SPete_GaussianBlur_Data* pInstanceData,SPete_GaussianBlur_Settings* pSettings,U32* pSource,U32* pOutput);

static SPete_Parameter g_Parameters[]={
	{
		"Radius",
		5.0f,
		1.0f,
		63.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Edge Factor",
		0.002f,
		0.0f,
		0.1f,
		PETE_PARAM_FLOAT
	},
	{
		"Fake It",
		1.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Passes",
		3.0f,
		1.0f,
		6.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Do UnSharpMask",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Aspect Ratio",
		1.0f,
		0.0f,
		2.0f,
		PETE_PARAM_FLOAT
	},
	{
		"UnSharpMask Amount",
		0.5f,
		0.0f,
		2.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Do Odd But Cool",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_GaussianBlur_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_GaussianBlur_Init(SPete_GaussianBlur_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_GaussianBlur_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	const int nChannels=4;
	const int nSizeOfIntermediateBuffer=
		nWidth*nHeight*nChannels*sizeof(float);

	pInstanceData->hIntermediateBuffer=
		Pete_NewHandle(nSizeOfIntermediateBuffer);

	if (pInstanceData->hIntermediateBuffer==NULL) {
		Pete_GaussianBlur_DeInit(pInstanceData);
		return 0;
	}

	const int nMaxKernelSize=nWidth;

	pInstanceData->nMaxKernelSize=nMaxKernelSize;

	pInstanceData->hKernel=
		Pete_NewHandle(nMaxKernelSize*sizeof(float));

	if (pInstanceData->hKernel==NULL) {
		Pete_GaussianBlur_DeInit(pInstanceData);
		return 0;
	}

	return 1;

}

void Pete_GaussianBlur_DeInit(SPete_GaussianBlur_Data* pInstanceData) {

	if (pInstanceData->hIntermediateBuffer!=NULL) {
		Pete_FreeHandle(pInstanceData->hIntermediateBuffer);
		pInstanceData->hIntermediateBuffer=NULL;
	}

	if (pInstanceData->hKernel!=NULL) {
		Pete_FreeHandle(pInstanceData->hKernel);
		pInstanceData->hKernel=NULL;
	}

}

void Pete_GaussianBlur_Render(SPete_GaussianBlur_Data* pInstanceData,SPete_GaussianBlur_Settings* pSettings,U32* pSource,U32* pOutput) {

	if (pSettings->m_DoFakeIt>0.0f) {

		U32* pIntermediateBuffer=
			(U32*)Pete_LockHandle(pInstanceData->hIntermediateBuffer);
		if (pIntermediateBuffer==NULL) {
			return;
		}

		const int nMaxIterations=
			(int)(ceilf(pSettings->m_FakeItPasses));


		const float AspectRatio=pSettings->m_AspectRatio;
		const float BaseRadius=pSettings->m_Radius;

		float RadiusX;
		float RadiusY;
		if (AspectRatio<1.0f) {
			RadiusX=BaseRadius;
			RadiusY=(BaseRadius*AspectRatio);
		} else {
			RadiusX=(BaseRadius*(2.0f-AspectRatio));
			RadiusY=BaseRadius;
		}

		const int nBoxFilterSizeX=
			(int)(ceilf(RadiusX));

		const int nBoxFilterSizeY=
			(int)(ceilf(RadiusY));

		Pete_DoBoxFilterBlur(
			pSource,
			pIntermediateBuffer,
			pOutput,
			nBoxFilterSizeX,
			nBoxFilterSizeY,
			pInstanceData->nWidth,
			pInstanceData->nHeight,
			nMaxIterations);

		Pete_UnLockHandle(pInstanceData->hIntermediateBuffer);

	} else {

		Pete_DoGaussianBlur(pInstanceData,pSettings,pSource,pOutput);

	}

	if (pSettings->m_DoOddButCool>0.0f) {

		Pete_DoInPlaceSubtract_OddButNice(pOutput,pSource,pInstanceData->nWidth,pInstanceData->nHeight);

	} else 	if (pSettings->m_DoUnSharpMask>0.0f) {

		Pete_DoInPlaceSubtractScaled(pOutput,pSource,pInstanceData->nWidth,pInstanceData->nHeight,pSettings->m_UnSharpMaskAmount);

	}

}

void Pete_DoGaussianBlur(SPete_GaussianBlur_Data* pInstanceData,SPete_GaussianBlur_Settings* pSettings,U32* pSource,U32* pOutput) {

	float*const pKernel=(float*)Pete_LockHandle(pInstanceData->hKernel);
	if (pKernel==NULL) {
		return;
	}
	float*const pIntermediateBuffer=(float*)Pete_LockHandle(pInstanceData->hIntermediateBuffer);
	if (pIntermediateBuffer==NULL) {
		return;
	}

	if ((pIntermediateBuffer==NULL)||
		(pKernel==NULL)) {
		return;
	}

	const int nKernelSize=
		Pete_GaussianBlur_SetupKernel(
		pKernel,
		pInstanceData->nMaxKernelSize,
		pSettings->m_Radius,
		pSettings->m_EdgeFactor);

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nKernelOffset=(nKernelSize-1)/2;

	float* pCurrentBuffer=pIntermediateBuffer;

	U32*const pSourceLastLineStart=pSource+((nHeight-1)*nWidth);

	const int nPreMultYKernelOffset=(nKernelOffset*nWidth);

	float*const pKernelEnd=pKernel+nKernelSize;

	int nY;
	for (nY=0; nY<nHeight; ++nY) {

		U32*const pCurrentLineStart=pSource+(nY*nWidth);

		int nX;
		for (nX=0; nX<nWidth; ++nX) {

			float ResultRed=0.0f;
			float ResultGreen=0.0f;
			float ResultBlue=0.0f;
			float ResultAlpha=0.0f;

			U32*const pCurrentRowStart=
				pCurrentLineStart+nX;

			U32* pSampleRowStart=
				pCurrentRowStart-nPreMultYKernelOffset;

			if (pSampleRowStart<pSource) {
				pSampleRowStart=pSource+nX;
			}

			U32* pSampleRowEnd=
				pCurrentRowStart+nPreMultYKernelOffset;

			if (pSampleRowEnd>=pSourceLastLineStart) {
				pSampleRowEnd=pSourceLastLineStart+nX;
			}

			U32* pCurrentSample=
				pCurrentRowStart-nPreMultYKernelOffset;

			float* pCurrentKernel;
			for (pCurrentKernel=pKernel; pCurrentKernel!=pKernelEnd; pCurrentKernel+=1,pCurrentSample+=nWidth) {

				const float KernelScale=*pCurrentKernel;

				const U32* const pCurrentSource=(U32*)
					GatePtr(pCurrentSample,pSampleRowStart,pSampleRowEnd);

				const U32 SourceColour=*pCurrentSource;

				const int nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
				const int nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
				const int nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;
				const int nSourceAlpha=(SourceColour>>SHIFT_ALPHA)&0xff;

				ResultRed+=(nSourceRed*KernelScale);
				ResultGreen+=(nSourceGreen*KernelScale);
				ResultBlue+=(nSourceBlue*KernelScale);
				ResultAlpha+=(nSourceAlpha*KernelScale);

			}

			*pCurrentBuffer=ResultRed;
			pCurrentBuffer+=1;

			*pCurrentBuffer=ResultGreen;
			pCurrentBuffer+=1;

			*pCurrentBuffer=ResultBlue;
			pCurrentBuffer+=1;

			*pCurrentBuffer=ResultAlpha;
			pCurrentBuffer+=1;

		}

	}

	U32* pCurrentOutput=pOutput;

	for (nY=0; nY<nHeight; ++nY) {

		float*const pCurrentSourceLineStart=
			pIntermediateBuffer+(nY*nWidth*3);

		float*const pCurrentSourceLineEnd=
			pCurrentSourceLineStart+((nWidth-1)*3);

		float* pCurrentSourceStart=
			pCurrentSourceLineStart-(nKernelOffset*3);

		int nX;
		for (nX=0; nX<nWidth; ++nX,pCurrentSourceStart+=3) {

			float ResultRed=0.0f;
			float ResultGreen=0.0f;
			float ResultBlue=0.0f;
			float ResultAlpha=0.0f;

			float* pCurrentSourceUngated=pCurrentSourceStart;
			float* pCurrentKernel;
			for (pCurrentKernel=pKernel; pCurrentKernel!=pKernelEnd; pCurrentKernel+=1,pCurrentSourceUngated+=3) {

				const float KernelScale=*pCurrentKernel;

				const float*const pCurrentSource=(float*)
					GatePtr(pCurrentSourceUngated,
					pCurrentSourceLineStart,
					pCurrentSourceLineEnd);

				const float SourceRed=*(pCurrentSource+0);
				const float SourceGreen=*(pCurrentSource+1);
				const float SourceBlue=*(pCurrentSource+2);
				const float SourceAlpha=*(pCurrentSource+2);

				ResultRed+=(SourceRed*KernelScale);
				ResultGreen+=(SourceGreen*KernelScale);
				ResultBlue+=(SourceBlue*KernelScale);
				ResultAlpha+=(SourceAlpha*KernelScale);

			}

			const int nOutputRed=(int)(ResultRed);
			const int nOutputGreen=(int)(ResultGreen);
			const int nOutputBlue=(int)(ResultBlue);
			const int nOutputAlpha=(int)(ResultAlpha);

			const U32 OutputColour=
				(nOutputRed<<SHIFT_RED)|
				(nOutputGreen<<SHIFT_GREEN)|
				(nOutputBlue<<SHIFT_BLUE)|
				(nOutputAlpha<<SHIFT_ALPHA);

			*pCurrentOutput=OutputColour;
			pCurrentOutput+=1;

		}

	}

	Pete_UnLockHandle(pInstanceData->hKernel);
	Pete_UnLockHandle(pInstanceData->hIntermediateBuffer);

}

float Pete_GaussianBlur_EvalFunc(float t,float A, float C) {

	const float tSquared=(t*t);
	const float CTimestSquared=(C*tSquared);
	const float ExpResult=expf(CTimestSquared);
	const float Result=A*ExpResult;

	return Result;

}

int Pete_GaussianBlur_SetupKernel(float* pKernel,int nKernelMaxSize,float Radius,float EdgeFactor) {

	const float MaxRadius=floorf((nKernelMaxSize-1)/2.0f);

	if (Radius>MaxRadius) {
		Radius=MaxRadius;
	}

	if (Radius<1.0f) {
		Radius=1.0f;
	}

	if (EdgeFactor<0.0001f) {
		EdgeFactor=0.0001f;
	}

	if (EdgeFactor>1.0f) {
		EdgeFactor=1.0f;
	}

	const float IntegralRadius=ceilf(Radius);
	const int nKernelSize=((int)(IntegralRadius*2)+1);

	const float C=logf(sqrtf(EdgeFactor))/
		(IntegralRadius*IntegralRadius);

	float FuncTotal=0.0f;
	int nCount;
	for (nCount=0; nCount<nKernelSize; nCount+=1) {

		const float t=(nCount-IntegralRadius);

		const float Result=Pete_GaussianBlur_EvalFunc(t,1.0f,C);

		FuncTotal+=Result;

		pKernel[nCount]=Result;

	}

	const float KernelScale=1.0f/FuncTotal;

	for (nCount=0; nCount<nKernelSize; nCount+=1) {

		pKernel[nCount]*=KernelScale;

	}

	return nKernelSize;
}

