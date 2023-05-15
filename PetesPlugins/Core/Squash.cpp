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


#include "Squash.h"
#include "PeteHelpers.h"

static SPete_Parameter g_Parameters[]={
	{
		"Horizontal",
		1.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Size",
		1.0f,
		-8.0f,
		8.0f,
		PETE_PARAM_FLOAT
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_Squash_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_Squash_Init(SPete_Squash_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_Squash_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_Squash_DeInit(SPete_Squash_Data* pInstanceData) {

	// do nothing

}

void Pete_Squash_Render(SPete_Squash_Data* pInstanceData,SPete_Squash_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHalfWidth=nWidth/2;
	const int nHeight=pInstanceData->nHeight;
	const int nHalfHeight=nHeight/2;

	const int nNumPixels=nWidth*nHeight;

	float XMult;
	float YMult;

	float Size=pSettings->m_Size;
	if (Size==0.0f) {
		Size=0.001f;
	}

	if (pSettings->m_DoHorizontal>0.0f) {
		XMult=1.0f;
		YMult=1.0f/Size;
	} else {
		XMult=1.0f/Size;
		YMult=1.0f;
	}

	U32* pSourceStart=pSource;

	U32* pCurrentOutput=pOutput;
	const U32* pOutputEnd=(pOutput+nNumPixels);

	float OutputY=-nHalfHeight;
	while (pCurrentOutput!=pOutputEnd) {

		const U32* pOutputLineEnd=pCurrentOutput+nWidth;

		float OutputX=-nHalfWidth;
		while (pCurrentOutput!=pOutputLineEnd) {

			int nSourceX=static_cast<int>(OutputX*XMult);
			int nSourceY=static_cast<int>(OutputY*YMult);

			nSourceX+=nHalfWidth;
			nSourceY+=nHalfHeight;

			nSourceX=GetTiled(nSourceX,nWidth);
			nSourceY=GetTiled(nSourceY,nHeight);

			U32* pCurrentSource=
				pSourceStart+
				(nSourceY*nWidth)+
				nSourceX;

			*pCurrentOutput=*pCurrentSource;

			pCurrentOutput+=1;
			OutputX+=1;

		}

		OutputY+=1;
	}

}

