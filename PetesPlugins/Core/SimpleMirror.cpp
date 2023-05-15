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

#include "SimpleMirror.h"
#include "PeteHelpers.h"

static SPete_Parameter g_Parameters[]={
	{
		"Angle",
		0.0f,
		0.0f,
		Pete_TwoPi,
		PETE_PARAM_ANGLE
	},
	{
		"Distance",
		0.0f,
		-1.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"SimpleMirror Everything",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_SimpleMirror_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_SimpleMirror_Init(SPete_SimpleMirror_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_SimpleMirror_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_SimpleMirror_DeInit(SPete_SimpleMirror_Data* pInstanceData) {
	// do nothing
}

void Pete_SimpleMirror_Render(SPete_SimpleMirror_Data* pInstanceData,SPete_SimpleMirror_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const float Width=nWidth;
	const float Height=nHeight;

	const float HalfWidth=(Width/2.0f);
	const float HalfHeight=(Height/2.0f);

	const float AngleRadians=pSettings->m_Angle;

	const float NormalX=cos(AngleRadians);
	const float NormalY=sin(AngleRadians);

	const float MaxDist=
		sqrt(
		(HalfWidth*HalfWidth)+
		(HalfHeight*HalfHeight));

	const float PlaneD=MaxDist*pSettings->m_PlaneD;

	const bool bSimpleMirrorEverything=(pSettings->m_DoSimpleMirrorAll>0.0f);

	const int nPixelsCount=(nWidth*nHeight);

	U32* pOutputStart=pOutput;
	U32* pOutputEnd=(pOutput+nPixelsCount);

	U32* pCurrentOutput=pOutputStart;
	U32* pCurrentSource=pSource;

	float CurrentY=-HalfHeight;
	while (pCurrentOutput<pOutputEnd) {

		U32* pOutputLineStart=pCurrentOutput;
		U32* pOutputLineEnd=(pOutputLineStart+nWidth);

		const float StartX=-HalfWidth;

		const float StartVDotNMinusD=
			((StartX*NormalX)+
			(CurrentY*NormalY))-
			PlaneD;

		const float EndX=HalfWidth;

		float EndVDotNMinusD=
			((EndX*NormalX)+
			(CurrentY*NormalY))-
			PlaneD;

		const float VDotNMinusDInc=
			(EndVDotNMinusD-StartVDotNMinusD)/nWidth;

		float VDotNMinusD=StartVDotNMinusD;

		float CurrentX=StartX;

		while (pCurrentOutput<pOutputLineEnd) {

			if ((VDotNMinusD>0.0f)&&(!bSimpleMirrorEverything)) {

				*pCurrentOutput=*pCurrentSource;

			} else {

				const float TwoVDotNMinusD=2.0f*VDotNMinusD;

				const float SourceX=CurrentX-(TwoVDotNMinusD*NormalX);
				const float SourceY=CurrentY-(TwoVDotNMinusD*NormalY);

				int nSourceX=static_cast<int>(SourceX+HalfWidth);
				int nSourceY=static_cast<int>(SourceY+HalfHeight);

				nSourceX=GetMirrored(nSourceX,nWidth);
				nSourceY=GetMirrored(nSourceY,nHeight);

				U32* pSimpleMirrorSource=
					pSource+
					(nSourceY*nWidth)+
					nSourceX;

				*pCurrentOutput=*pSimpleMirrorSource;

			}

			CurrentX+=1.0f;
			VDotNMinusD+=VDotNMinusDInc;

			pCurrentOutput+=1;
			pCurrentSource+=1;

		}

		CurrentY+=1.0f;

	}
}
