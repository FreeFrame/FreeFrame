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


#include "FishEye.h"
#include "PeteHelpers.h"

static SPete_Parameter g_Parameters[]={
	{
		"Distortion",
		0.2f,
		0.0f,
		5.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Max Radius",
		1.0f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Centre X",
		0.5f,
		0.0f,
		1.0f,
		PETE_PARAM_POS_X
	},
	{
		"Centre Y",
		0.5f,
		0.0f,
		1.0f,
		PETE_PARAM_POS_Y
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

#ifdef PETE_NATIVE_AE
#define PETE_BICUBICSAMPLE
#endif

void Pete_FishEye_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

const int nACosTableSize=1024;

int Pete_FishEye_Init(SPete_FishEye_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_FishEye_DeInit(pInstanceData);

	const int nNumPixels=(nWidth*nHeight);
	const int nRadiusTableByteCount=(nNumPixels*sizeof(float));

	pInstanceData->hRadiusTable=
		Pete_NewHandle(nRadiusTableByteCount);
	if (pInstanceData->hRadiusTable==NULL) {
		Pete_FishEye_DeInit(pInstanceData);
		return 0;
	}

	float* pRadiusTable=(float*)
		Pete_LockHandle(pInstanceData->hRadiusTable);
	if (pRadiusTable==NULL) {
		return 0;
	}

	float* pCurrentRadiusTable=pRadiusTable;
	float* pRadiusTableEnd=(pRadiusTable+nNumPixels);
	int nY=0;
	while (pCurrentRadiusTable<pRadiusTableEnd) {

		float* pRadiusTableLineEnd=
			(pCurrentRadiusTable+nWidth);

		int nX=0;
		while (pCurrentRadiusTable<pRadiusTableLineEnd) {

			float CurrentX=(nX);///(float)(nWidth/2));
			float CurrentY=(nY);///(float)(nHeight/2));

			*pCurrentRadiusTable=
				sqrt((CurrentX*CurrentX)+(CurrentY*CurrentY));

			pCurrentRadiusTable+=1;

			nX+=1;

		}

		nY+=1;
	}

	const int nACosTableByteCount=(nACosTableSize*sizeof(float));
	pInstanceData->hACosTable=
		Pete_NewHandle(nACosTableByteCount);
	if (pInstanceData->hACosTable==NULL) {
		Pete_FishEye_DeInit(pInstanceData);
		return 0;
	}

	float* pACosTable=(float*)
		Pete_LockHandle(pInstanceData->hACosTable);
	if (pACosTable==NULL) {
		return 0;
	}

	int nCount;
	for (nCount=0; nCount<nACosTableSize; nCount+=1) {

		pACosTable[nCount]=acos(nCount/(float)nACosTableSize);

	}

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_FishEye_DeInit(SPete_FishEye_Data* pInstanceData) {

	if (pInstanceData->hRadiusTable!=NULL) {
		Pete_FreeHandle(pInstanceData->hRadiusTable);
		pInstanceData->hRadiusTable=NULL;
	}

	if (pInstanceData->hACosTable!=NULL) {
		Pete_FreeHandle(pInstanceData->hACosTable);
		pInstanceData->hACosTable=NULL;
	}

}

void Pete_FishEye_Render(SPete_FishEye_Data* pInstanceData,SPete_FishEye_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;
	const int nNumPixels=(nWidth*nHeight);

	float CentreX=(pSettings->m_CentreX*nWidth);
	CentreX=GateInt(CentreX,0,(nWidth-1));

	float CentreY=(pSettings->m_CentreY*nHeight);
	CentreY=GateInt(CentreY,0,(nHeight-1));

	const float Distortion=pSettings->m_Distortion;
	const float DiagonalRadius=
		sqrt((nWidth/2)*(nWidth/2)+(nHeight/2)*(nHeight/2));
	const float MaxRadius=(pSettings->m_MaxRadius*DiagonalRadius);
	float MaxRadiusRecip;
	if (MaxRadius==0.0f) {
		MaxRadiusRecip=0.0f;
	} else {
		MaxRadiusRecip=1.0f/MaxRadius;
	}
	const float S = MaxRadius / log((Distortion*MaxRadius)+1);

	float* pRadiusTable=(float*)
		Pete_LockHandle(pInstanceData->hRadiusTable);
	if (pRadiusTable==NULL) {
		return;
	}

	float* pACosTable=(float*)
		Pete_LockHandle(pInstanceData->hACosTable);
	if (pACosTable==NULL) {
		return;
	}

	U32* pCurrentOutput=pOutput;
	const U32* pOutputEnd=(pOutput+nNumPixels);

	float OutputY=0.0f;
	while (pCurrentOutput!=pOutputEnd) {

		const U32* pOutputLineStart=pCurrentOutput;
		const U32* pOutputLineEnd=pCurrentOutput+nWidth;

		float OutputX=0.0f;
		while (pCurrentOutput!=pOutputLineEnd) {

			const float DeltaX=(OutputX-CentreX);
			const float DeltaY=(OutputY-CentreY);

			const int nRadiusTableX=AbsInt(DeltaX);
			const int nRadiusTableY=AbsInt(DeltaY);

			const float* pRadiusTableEntry=
				pRadiusTable+(nRadiusTableY*nWidth)+nRadiusTableX;

			float CurrentRadius=(*pRadiusTableEntry);
			if (CurrentRadius==0.0f) {
				CurrentRadius=0.0001f;
			}

//			const float NewRadius=S*log(1.0f+Distortion*CurrentRadius);
//			const float NewRadius=(exp(CurrentRadius/S)-1.0f)/Distortion;
			if (CurrentRadius>=MaxRadius) {

				*pCurrentOutput=0x00000000;

			} else {

				const float ACosInput=
					(CurrentRadius*MaxRadiusRecip);

//				const float ACosResult=
//					acos(ACosInput);

				const int nACosTableIndex=
					(ACosInput*nACosTableSize);

				const float ACosResult=
					pACosTable[nACosTableIndex];

				const float NewRadius=
					1.0f/
					(1.0f+(ACosResult*Distortion));

				const float CurrentScale=NewRadius;

#ifndef PETE_BICUBICSAMPLE

				const int nSourceX=(CentreX+(DeltaX*CurrentScale));
				const int nSourceY=(CentreY+(DeltaY*CurrentScale));

				if ((nSourceX<0)||
					(nSourceX>=nWidth)||
					(nSourceY<0)||
					(nSourceY>=nHeight)) {

					*pCurrentOutput=0x00000000;

				} else {

					const U32*const pCurrentSource=
						(pSource+(nSourceY*nWidth)+nSourceX);

					U32 SourceColour=*pCurrentSource;

					U32 OutputColour=SourceColour;

					*pCurrentOutput=OutputColour;

				}

#else // PETE_BICUBICSAMPLE

				const int nSourceXFP=(CentreX+(DeltaX*CurrentScale))*(1<<16);
				const int nSourceYFP=(CentreY+(DeltaY*CurrentScale))*(1<<16);

				const int nSourceXFraction=(nSourceXFP&0x0000ffff);
				const int nOneMinusXFraction=(0x10000-nSourceXFraction);
				const int nSourceX=(nSourceXFP>>16);

				const int nSourceYFraction=(nSourceYFP&0x0000ffff);
				const int nOneMinusYFraction=(0x10000-nSourceYFraction);
				const int nSourceY=(nSourceYFP>>16);

				if ((nSourceX<0)||
					(nSourceX>=(nWidth-1))||
					(nSourceY<0)||
					(nSourceY>=(nHeight-1))) {

					*pCurrentOutput=0x00000000;

				} else {

					PF_Pixel* pLeftTopSource=
						((PF_Pixel*)(pSource+(nSourceY*nWidth)))+nSourceX;

					PF_Pixel* pRightTopSource=
						((PF_Pixel*)(pSource+(nSourceY*nWidth)))+(nSourceX+1);

					PF_Pixel* pLeftBottomSource=
						((PF_Pixel*)(pSource+((nSourceY+1)*nWidth)))+nSourceX;

					PF_Pixel* pRightBottomSource=
						((PF_Pixel*)(pSource+((nSourceY+1)*nWidth)))+(nSourceX+1);

					PF_Pixel TopColour;
					TopColour.red=
						((pLeftTopSource->red*nOneMinusXFraction)+
						(pRightTopSource->red*nSourceXFraction))>>16;
					TopColour.green=
						((pLeftTopSource->green*nOneMinusXFraction)+
						(pRightTopSource->green*nSourceXFraction))>>16;
					TopColour.blue=
						((pLeftTopSource->blue*nOneMinusXFraction)+
						(pRightTopSource->blue*nSourceXFraction))>>16;
					TopColour.alpha=
						((pLeftTopSource->alpha*nOneMinusXFraction)+
						(pRightTopSource->alpha*nSourceXFraction))>>16;

					PF_Pixel BottomColour;
					BottomColour.red=
						((pLeftBottomSource->red*nOneMinusXFraction)+
						(pRightBottomSource->red*nSourceXFraction))>>16;
					BottomColour.green=
						((pLeftBottomSource->green*nOneMinusXFraction)+
						(pRightBottomSource->green*nSourceXFraction))>>16;
					BottomColour.blue=
						((pLeftBottomSource->blue*nOneMinusXFraction)+
						(pRightBottomSource->blue*nSourceXFraction))>>16;
					BottomColour.alpha=
						((pLeftBottomSource->alpha*nOneMinusXFraction)+
						(pRightBottomSource->alpha*nSourceXFraction))>>16;


					PF_Pixel SourceColour;
					SourceColour.red=
						((TopColour.red*nOneMinusYFraction)+
						(BottomColour.red*nSourceYFraction))>>16;
					SourceColour.green=
						((TopColour.green*nOneMinusYFraction)+
						(BottomColour.green*nSourceYFraction))>>16;
					SourceColour.blue=
						((TopColour.blue*nOneMinusYFraction)+
						(BottomColour.blue*nSourceYFraction))>>16;
					SourceColour.alpha=
						((TopColour.alpha*nOneMinusYFraction)+
						(BottomColour.alpha*nSourceYFraction))>>16;

					*(PF_Pixel*)pCurrentOutput=SourceColour;

				}

#endif // PETE_BICUBICSAMPLE

			}

			pCurrentOutput+=1;

			OutputX+=1.0f;

		}

		OutputY+=1.0f;

	}


}
