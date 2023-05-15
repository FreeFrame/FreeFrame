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


#include "LionelBlur.h"
#include "PeteHelpers.h"

const int nBlurMaxRadius=256;

static SPete_Parameter g_Parameters[]={
	{
		"Blur Radius",
		15.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Angle",
		0.0f,
		0.0f,
		Pete_TwoPi,
		PETE_PARAM_ANGLE
	}
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_LionelBlur_Render_Old(SPete_LionelBlur_Data* pInstanceData,SPete_LionelBlur_Settings* pSettings,U32* pSource,U32* pOutput);

void Pete_LionelBlur_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_LionelBlur_Init(SPete_LionelBlur_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_LionelBlur_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_LionelBlur_DeInit(SPete_LionelBlur_Data* pInstanceData) {

	// do nothing

}

void Pete_LionelBlur_Render(SPete_LionelBlur_Data* pInstanceData,SPete_LionelBlur_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int cnFixedShift=8;
	const int cnFixedPrec=(1<<cnFixedShift);

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nNumPixels=(nWidth*nHeight);
	const int nNumBytes=(nNumPixels*sizeof(U32));

	memset(pOutput,0xff,nNumBytes);

	int nBlurRadius=static_cast<int>(pSettings->m_BlurRadius);
	nBlurRadius=GateInt(nBlurRadius,1,(nBlurMaxRadius-1));

	const int nBlurRadiusRecipFP=
		((1<<16)/nBlurRadius);

	const float Angle=pSettings->m_Angle;

	const float DirX=cos(Angle);
	const float DirY=sin(Angle);

	const float AbsDirX=fabsf(DirX);
	const float AbsDirY=fabsf(DirY);

	float DirA;
	float DirB;

	int nSideA;
	int nSideB;

	int nAddressIncA;
	int nAddressIncB;

	if (AbsDirY>AbsDirX) {

		DirA=DirX;
		DirB=DirY;

		nSideA=nWidth;
		nSideB=nHeight;

		nAddressIncA=1;
		nAddressIncB=nWidth;

	} else {

		DirA=DirY;
		DirB=DirX;

		nSideA=nHeight;
		nSideB=nWidth;

		nAddressIncA=nWidth;
		nAddressIncB=1;

	}

	int nIncB;
	int nLineOriginB;
	int nLineEndB;
	if (DirB>0.0f) {
		nIncB=1;
		nLineOriginB=0;
		nLineEndB=nSideB;
	} else {
		nIncB=-1;
		nLineOriginB=(nSideB-1);
		nLineEndB=-1;
	}
	const int nDeltaB=(nIncB*nSideB);
	const int nAbsDeltaB=AbsInt(nDeltaB);
	const int nDeltaA=(int)((nDeltaB/DirB)*DirA);
	const int nAbsDeltaA=AbsInt(nDeltaA);

	int nStartA;
	int nEndA;
	int nIncA;
	if (DirA>0.0f) {
		nStartA=-nAbsDeltaA;
		nEndA=nSideA;
		nIncA=1;
	} else {
		nStartA=0;
		nEndA=(nSideA+nAbsDeltaA);
		nIncA=-1;
	}

	int nLineOriginA;
	for (nLineOriginA=nStartA; nLineOriginA<nEndA; nLineOriginA+=1) {

		int nCounter;
		int nClippedLineOriginA;
		int nClippedLineOriginB;

		if (nLineOriginA<0) {

			const int nClipDistA=(0-nLineOriginA);
			const int nClipTimesDeltaB=(nClipDistA*nDeltaB);
			int nClipDistB;
			if (nClipTimesDeltaB>0) {
				nClipDistB=(nClipTimesDeltaB+(nDeltaA-1))/nDeltaA;
				nCounter=((nClipDistB*nDeltaA)-nClipTimesDeltaB);
			} else {
				nClipDistB=(nClipTimesDeltaB-(nDeltaA-1))/nDeltaA;
				nCounter=-((nClipDistB*nDeltaA)-nClipTimesDeltaB);
			}
			nClippedLineOriginB=(nLineOriginB+nClipDistB);
			nClippedLineOriginA=0;

		} else if (nLineOriginA>=nSideA) {

			const int nClipDistA=(nLineOriginA-(nSideA-1));
			const int nClipTimesDeltaB=(nClipDistA*nDeltaB);
			int nClipDistB;
			if (nClipTimesDeltaB>0) {
				nClipDistB=(nClipTimesDeltaB+(nAbsDeltaA-1))/nAbsDeltaA;
				nCounter=((nClipDistB*nAbsDeltaA)-nClipTimesDeltaB);
			} else {
				nClipDistB=(nClipTimesDeltaB-(nAbsDeltaA-1))/nAbsDeltaA;
				nCounter=-((nClipDistB*nAbsDeltaA)-nClipTimesDeltaB);
			}

			nClippedLineOriginB=(nLineOriginB+nClipDistB);
			nClippedLineOriginA=(nSideA-1);

		} else {

			nClippedLineOriginA=nLineOriginA;
			nClippedLineOriginB=nLineOriginB;
			nCounter=0;

		}

		const int nLineEndA=(nLineOriginA+nDeltaA);

		int nClippedLineEndB;
		if (nLineEndA<0) {

			const int nDistA=(nClippedLineOriginA+1);
			const int nDistB=((nAbsDeltaB*nDistA)+(nAbsDeltaA-1)-nCounter)/nAbsDeltaA;
			nClippedLineEndB=(nClippedLineOriginB+(nDistB*nIncB));

		} else if (nLineEndA>=nSideA) {

			const int nDistA=(nSideA-nClippedLineOriginA);
			const int nDistB=((nAbsDeltaB*nDistA)+(nAbsDeltaA-1)-nCounter)/nAbsDeltaA;
			nClippedLineEndB=(nClippedLineOriginB+(nDistB*nIncB));

		} else {

			nClippedLineEndB=nLineEndB;

		}

		int nCurrentA=nClippedLineOriginA;
		int nCurrentB=nClippedLineOriginB;

		int nCurrentAddressOffset=((nCurrentB*nAddressIncB)+(nCurrentA*nAddressIncA));

		const int nSignedAddressIncA=(nIncA*nAddressIncA);
		const int nSignedAddressIncB=(nIncB*nAddressIncB);

		U32 PrevCols[nBlurMaxRadius];
		memset(&(PrevCols[0]),0,(nBlurRadius*sizeof(U32)));

		int nColIndex=0;

		int nAverageRed=0;
		int nAverageGreen=0;
		int nAverageBlue=0;
		int nAverageAlpha=0;

		for (; nCurrentB!=nClippedLineEndB; nCurrentB+=nIncB) {

			U32* pCurrentSource=(pSource+nCurrentAddressOffset);
			U32* pCurrentOutput=(pOutput+nCurrentAddressOffset);

			U32 SourceColour=*pCurrentSource;

			const int nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
			const int nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
			const int nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;
			const int nSourceAlpha=(SourceColour>>SHIFT_ALPHA)&0xff;

			const int nMungedRed=(nSourceRed*nBlurRadiusRecipFP)>>16;
			const int nMungedGreen=(nSourceGreen*nBlurRadiusRecipFP)>>16;
			const int nMungedBlue=(nSourceBlue*nBlurRadiusRecipFP)>>16;
			const int nMungedAlpha=(nSourceAlpha*nBlurRadiusRecipFP)>>16;

			const U32 MungedSourceColour=
				(nMungedRed<<SHIFT_RED)|
				(nMungedGreen<<SHIFT_GREEN)|
				(nMungedBlue<<SHIFT_BLUE)|
				(nMungedAlpha<<SHIFT_ALPHA);

			nColIndex+=1;
			if (nColIndex==nBlurRadius) {
				nColIndex=0;
			}

			const U32 PreviousColour=PrevCols[nColIndex];

			const int nPreviousRed=(PreviousColour>>SHIFT_RED)&0xff;
			const int nPreviousGreen=(PreviousColour>>SHIFT_GREEN)&0xff;
			const int nPreviousBlue=(PreviousColour>>SHIFT_BLUE)&0xff;
			const int nPreviousAlpha=(PreviousColour>>SHIFT_ALPHA)&0xff;

			nAverageRed-=nPreviousRed;
			nAverageGreen-=nPreviousGreen;
			nAverageBlue-=nPreviousBlue;
			nAverageAlpha-=nPreviousAlpha;

			nAverageRed+=nMungedRed;
			nAverageGreen+=nMungedGreen;
			nAverageBlue+=nMungedBlue;
			nAverageAlpha+=nMungedAlpha;

			PrevCols[nColIndex]=MungedSourceColour;

			const U32 AverageColour=
				(nAverageRed<<SHIFT_RED)|
				(nAverageGreen<<SHIFT_GREEN)|
				(nAverageBlue<<SHIFT_BLUE)|
				(nAverageAlpha<<SHIFT_ALPHA);

			const int nCurrentLuminance=GetLuminance(SourceColour);
			const int nAverageLuminance=GetLuminance(AverageColour);

			U32 OutputColour;
			if (nCurrentLuminance>nAverageLuminance) {

				OutputColour=SourceColour;

			} else {

				OutputColour=AverageColour;

			}

			*pCurrentOutput=OutputColour;

			nCounter+=nAbsDeltaA;
			nCurrentAddressOffset+=nSignedAddressIncB;
			if (nCounter>=nAbsDeltaB) {
				nCounter-=nAbsDeltaB;
				nCurrentAddressOffset+=nSignedAddressIncA;
			}

		}

	}

}