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

#include "Smear.h"
#include "PeteHelpers.h"

static SPete_Parameter g_Parameters[]={
	{
		"Angle",
		0.0f,
		0.0f,
		Pete_TwoPi,
		PETE_PARAM_ANGLE
	},
	{	"Position X",
		0.5f,
		0.0f,
		1.0f,
		PETE_PARAM_POS_X
	},
	{	"Position Y",
		0.5f,
		0.0f,
		1.0f,
		PETE_PARAM_POS_Y
	},
	{
		"Smear Everything",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Bevel Size",
		0.0f,
		0.0f,
		4000.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Bevel Scale",
		0.5f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_Smear_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_Smear_Init(SPete_Smear_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_Smear_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_Smear_DeInit(SPete_Smear_Data* pInstanceData) {
	// do nothing
}

void Pete_Smear_Render(SPete_Smear_Data* pInstanceData,SPete_Smear_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const float Width=nWidth;
	const float Height=nHeight;

	const float HalfWidth=(Width/2.0f);
	const float HalfHeight=(Height/2.0f);

	const float AngleRadians=pSettings->m_Angle;

	const float NormalX=cos(AngleRadians);
	const float NormalY=sin(AngleRadians);

	const float PosX=(pSettings->m_PosX-0.5f)*nWidth;
	const float PosY=(pSettings->m_PosY-0.5f)*nHeight;

	const float BevelStartD=((PosX*NormalX)+(PosY*NormalY));
	const float BevelSize=pSettings->m_BevelSize;
	const float BevelScale=0.5f-(pSettings->m_BevelScale/2.0f);
	
	const bool bSmearEverything=(pSettings->m_DoSmearAll>0.0f);

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
			BevelStartD;
		
		const float EndX=HalfWidth;

		float EndVDotNMinusD=
			((EndX*NormalX)+
			(CurrentY*NormalY))-
			BevelStartD;

		const float VDotNMinusDInc=
			(EndVDotNMinusD-StartVDotNMinusD)/nWidth;

		float VDotNMinusD=StartVDotNMinusD;

		float CurrentX=StartX;

		while (pCurrentOutput<pOutputLineEnd) {

			if ((VDotNMinusD>=0.0f)&&(!bSmearEverything)) {

				*pCurrentOutput=*pCurrentSource;

			} else {

				float NormalScale;
				if (-VDotNMinusD<BevelSize) {
				
					const float LerpValue=
						(-VDotNMinusD/BevelSize);
						
					const float CurrentScale=
						(LerpValue*BevelScale);

					NormalScale=(VDotNMinusD*CurrentScale);
					
				} else {
				
					NormalScale=(VDotNMinusD+BevelSize)-
						(BevelSize*BevelScale);
					
				}			

				const float SourceX=CurrentX-(NormalScale*NormalX);
				const float SourceY=CurrentY-(NormalScale*NormalY);
		
				int nSourceX=static_cast<int>(SourceX+HalfWidth);
				int nSourceY=static_cast<int>(SourceY+HalfHeight);

				nSourceX=GetMirrored(nSourceX,nWidth);
				nSourceY=GetMirrored(nSourceY,nHeight);

				U32* pSmearSource=
					pSource+
					(nSourceY*nWidth)+
					nSourceX;

				*pCurrentOutput=*pSmearSource;
			
			}

			CurrentX+=1.0f;
			VDotNMinusD+=VDotNMinusDInc;

			pCurrentOutput+=1;
			pCurrentSource+=1;

		}
		
		CurrentY+=1.0f;

	}
}
