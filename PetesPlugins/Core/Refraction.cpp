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


#include "Refraction.h"
#include "PeteHelpers.h"

static SPete_Parameter g_Parameters[]={
	{
		"Refraction",
		2.0f,
		0.01f,
		16.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Cell Width",
		16.0f,
		1.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Cell Height",
		16.0f,
		1.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Allow Magnification",
		1.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_Refraction_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_Refraction_Init(SPete_Refraction_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_Refraction_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_Refraction_DeInit(SPete_Refraction_Data* pInstanceData) {

	// do nothing

}

void Pete_Refraction_Render(SPete_Refraction_Data* pInstanceData,SPete_Refraction_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int cnFixedShift=8;
	const int cnFixedPrec=(1<<cnFixedShift);

	const int nWidth=pInstanceData->nWidth;
	const int nHalfWidth=(nWidth/2);
	const int nHeight=pInstanceData->nHeight;
	const int nHalfHeight=(nHeight/2);

	const int nNumPixels = nWidth*nHeight;

	float GatedRefraction=pSettings->m_Refraction;
	if ((pSettings->m_DoAllowMagnification==0.0f)&&(pSettings->m_Refraction<1.0f)) {
		GatedRefraction=1.0f;
	}

	const int nRefraction=static_cast<int>(GatedRefraction*256.0f);

	int nCellWidth=static_cast<int>(pSettings->m_CellWidth);
	if (nCellWidth<=0) {
		nCellWidth=1;
	}
	int nCellHeight=static_cast<int>(pSettings->m_CellHeight);
	if (nCellHeight<=0) {
		nCellHeight=1;
	}


	const int nHalfCellWidth=(nCellWidth/2);
	const int nHalfCellHeight=(nCellHeight/2);

	const int nYStartOffset=(nHalfCellHeight*(256))/nRefraction;
	const int nXStartOffset=(nHalfCellWidth*(256))/nRefraction;

	U32* pCurrentOutput=pOutput;
	const U32* pOutputEnd=(pOutput+nNumPixels);

	int nY=-nHalfHeight+nHalfCellHeight;
	while (pCurrentOutput!=pOutputEnd) {

		const U32* pOutputLineStart=pCurrentOutput;
		const U32* pOutputLineEnd=pCurrentOutput+nWidth;

		const int nYCentre=(((nY+(GetSign(nY)*nHalfCellHeight))/nCellHeight)*nCellHeight)+nHalfCellHeight;
		const int nYDist=(nY-nYCentre);
		int nSourceY=((nYDist*nRefraction)>>8)+nYCentre+nHalfHeight;
		nSourceY=GateInt(nSourceY,0,(nHeight-1));

		const U32* pSourceLineStart=(pSource+(nSourceY*nWidth));

		int nX=-nHalfWidth+nHalfCellWidth;
		while (pCurrentOutput!=pOutputLineEnd) {

			const int nXCentre=(((nX+(GetSign(nX)*nHalfCellWidth))/nCellWidth)*nCellWidth)+nHalfCellWidth;
			const int nXDist=(nX-nXCentre);
			int nSourceX=((nXDist*nRefraction)>>8)+nXCentre+nHalfWidth;
			nSourceX=GateInt(nSourceX,0,(nWidth-1));

			const U32* pCurrentSource=pSourceLineStart+nSourceX;

			*pCurrentOutput=*pCurrentSource;

			pCurrentOutput+=1;
			nX+=1;

		}

		nY+=1;

	}

}
