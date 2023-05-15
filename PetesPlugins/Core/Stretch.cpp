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


#include "Stretch.h"
#include "PeteHelpers.h"

static SPete_Parameter g_Parameters[]={
	{
		"Mirror",
		1.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Top Left X",
		0.0f,
		-2.0f,
		2.0f,
		PETE_PARAM_POS_X
	},
	{
		"Top Left Y",
		0.0f,
		-2.0f,
		2.0f,
		PETE_PARAM_POS_Y
	},
	{
		"Top Right X",
		1.0f,
		-2.0f,
		2.0f,
		PETE_PARAM_POS_X
	},
	{
		"Top Right Y",
		0.0f,
		-2.0f,
		2.0f,
		PETE_PARAM_POS_Y
	},
	{
		"Bottom Right X",
		1.0f,
		-2.0f,
		2.0f,
		PETE_PARAM_POS_X
	},
	{
		"Bottom Right Y",
		1.0f,
		-2.0f,
		2.0f,
		PETE_PARAM_POS_Y
	},
	{
		"Bottom Left X",
		0.0f,
		-2.0f,
		2.0f,
		PETE_PARAM_POS_X
	},
	{
		"Bottom Left Y",
		1.0f,
		-2.0f,
		2.0f,
		PETE_PARAM_POS_Y
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_Stretch_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_Stretch_Init(SPete_Stretch_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_Stretch_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_Stretch_DeInit(SPete_Stretch_Data* pInstanceData) {

	// do nothing

}

void Pete_Stretch_Render(SPete_Stretch_Data* pInstanceData,SPete_Stretch_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const float RecipWidth=1.0f/nWidth;
	const int nHeight=pInstanceData->nHeight;
	const int nSignMask=0x7fffffff;
	const float StretchEpsilon=0.0001f;

	const int nNumPixels=nWidth*nHeight;

	U32* pCurrentOutput=pOutput;
	const U32* pOutputEnd=(pOutput+nNumPixels);

	const float LeftDeltaX=pSettings->m_BottomLeftX-pSettings->m_TopLeftX;
	const float LeftDeltaY=pSettings->m_BottomLeftY-pSettings->m_TopLeftY;

	const float RightDeltaX=pSettings->m_TopRightX-pSettings->m_BottomRightX;
	const float RightDeltaY=pSettings->m_TopRightY-pSettings->m_BottomRightY;

	const float TopDeltaX=pSettings->m_TopRightX-pSettings->m_TopLeftX;
	const float TopDeltaY=pSettings->m_TopRightY-pSettings->m_TopLeftY;

	const float BottomDeltaX=pSettings->m_BottomLeftX-pSettings->m_BottomRightX;
	const float BottomDeltaY=pSettings->m_BottomLeftY-pSettings->m_BottomRightY;

	float LeftTangentX;
	float LeftTangentY;
	Get2dTangent(LeftDeltaX,LeftDeltaY,&LeftTangentX,&LeftTangentY);
	Normalise2d(&LeftTangentX,&LeftTangentY);

	float RightTangentX;
	float RightTangentY;
	Get2dTangent(RightDeltaX,RightDeltaY,&RightTangentX,&RightTangentY);
	Normalise2d(&RightTangentX,&RightTangentY);

	float TopTangentX;
	float TopTangentY;
	Get2dTangent(TopDeltaX,TopDeltaY,&TopTangentX,&TopTangentY);
	Normalise2d(&TopTangentX,&TopTangentY);

	float BottomTangentX;
	float BottomTangentY;
	Get2dTangent(BottomDeltaX,BottomDeltaY,&BottomTangentX,&BottomTangentY);
	Normalise2d(&BottomTangentX,&BottomTangentY);

	const float LeftD=Dot2d(LeftTangentX,LeftTangentY,pSettings->m_TopLeftX,pSettings->m_TopLeftY);
	const float RightD=Dot2d(RightTangentX,RightTangentY,pSettings->m_BottomRightX,pSettings->m_BottomRightY);
	const float TopD=Dot2d(TopTangentX,TopTangentY,pSettings->m_TopLeftX,pSettings->m_TopLeftY);
	const float BottomD=Dot2d(BottomTangentX,BottomTangentY,pSettings->m_BottomRightX,pSettings->m_BottomRightY);

	const float WidthScale=(nWidth-1.0f);
	const float HeightScale=(nHeight-1.0f);

	const bool bMirror=(pSettings->m_DoMirror>0.0f);

	int nY=0;
	while (pCurrentOutput!=pOutputEnd) {

		const U32* pOutputLineStart=pCurrentOutput;
		const U32* pOutputLineEnd=pCurrentOutput+nWidth;

		const float Y=(nY/(float)nHeight);

		const float StartX=0.0f;
		const float EndX=1.0f;

		float X=StartX;
		const float XInc=(EndX-StartX)/nWidth;

		const float LeftDotStart=Dot2d(LeftTangentX,LeftTangentY,StartX,Y);
		const float RightDotStart=Dot2d(RightTangentX,RightTangentY,StartX,Y);
		const float TopDotStart=Dot2d(TopTangentX,TopTangentY,StartX,Y);
		const float BottomDotStart=Dot2d(BottomTangentX,BottomTangentY,StartX,Y);

		const float LeftDistStart=LeftDotStart-LeftD;
		const float RightDistStart=RightDotStart-RightD;
		const float TopDistStart=TopDotStart-TopD;
		const float BottomDistStart=BottomDotStart-BottomD;

		const float UTotalStart=LeftDistStart+RightDistStart;
		const float VTotalStart=TopDistStart+BottomDistStart;

		const float LeftDotEnd=Dot2d(LeftTangentX,LeftTangentY,EndX,Y);
		const float RightDotEnd=Dot2d(RightTangentX,RightTangentY,EndX,Y);
		const float TopDotEnd=Dot2d(TopTangentX,TopTangentY,EndX,Y);
		const float BottomDotEnd=Dot2d(BottomTangentX,BottomTangentY,EndX,Y);

		const float LeftDistEnd=LeftDotEnd-LeftD;
		const float RightDistEnd=RightDotEnd-RightD;
		const float TopDistEnd=TopDotEnd-TopD;
		const float BottomDistEnd=BottomDotEnd-BottomD;

		const float UTotalEnd=LeftDistEnd+RightDistEnd;
		const float VTotalEnd=TopDistEnd+BottomDistEnd;

		float LeftDist=LeftDistStart;
		const float LeftDistInc=(LeftDistEnd-LeftDistStart)*RecipWidth;

		float TopDist=TopDistStart;
		const float TopDistInc=(TopDistEnd-TopDistStart)*RecipWidth;

		float UTotal=UTotalStart;
		const float UTotalInc=(UTotalEnd-UTotalStart)*RecipWidth;

		float VTotal=VTotalStart;
		const float VTotalInc=(VTotalEnd-VTotalStart)*RecipWidth;

		while (pCurrentOutput!=pOutputLineEnd) {

			float CurrentU;
			float CurrentV;

			if ((UTotal>StretchEpsilon)||(UTotal<-StretchEpsilon)) {
				CurrentU=(LeftDist/UTotal);
			} else {
				CurrentU=0.0f;
			}

			if ((VTotal>StretchEpsilon)||(VTotal<-StretchEpsilon)) {
				CurrentV=(TopDist/VTotal);
			} else {
				CurrentV=0.0f;
			}


			int nU=static_cast<int>(CurrentU*WidthScale);
			int nV=static_cast<int>(CurrentV*HeightScale);

			if (!bMirror) {

				if (nU<0) {
					nU=-nU;
					nU=nU%nWidth;
					nU=(nWidth-1)-nU;
				} else {
					nU=nU%nWidth;
				}

				if (nV<0) {
					nV=-nV;
					nV=nV%nHeight;
					nV=(nHeight-1)-nV;
				} else {
					nV=nV%nHeight;
				}

			} else {

				if (nU<0) {
					nU=-nU;
					nU=nU%(nWidth*2);
					nU=((nWidth*2)-1)-nU;
				} else {
					nU=nU%(nWidth*2);
				}

				if (nU>=nWidth) {
					nU=((nWidth*2)-1)-nU;
				}

				if (nV<0) {
					nV=-nV;
					nV=nV%(nHeight*2);
					nV=((nHeight*2)-1)-nV;
				} else {
					nV=nV%(nHeight*2);
				}

				if (nV>=nHeight) {
					nV=((nHeight*2)-1)-nV;
				}
			}


			const U32*const pCurrentSource=(pSource+(nV*nWidth)+nU);

			U32 SourceColour=*pCurrentSource;

			U32 OutputColour=SourceColour;

			*pCurrentOutput=OutputColour;

			pCurrentOutput+=1;

			LeftDist+=LeftDistInc;
			TopDist+=TopDistInc;

			UTotal+=UTotalInc;
			VTotal+=VTotalInc;

		}

		nY+=1;

	}


}
