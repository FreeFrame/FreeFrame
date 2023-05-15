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


#include "TVPixel.h"
#include "PeteHelpers.h"
#include "ImageMath.h"

static SPete_Parameter g_Parameters[]={
	{
		"Pixel Height",
		6.0f,
		2.0f,
		255.0f,
		PETE_PARAM_FLOAT
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_TVPixel_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

void Pete_TVPixel_StoreBackDrop(SPete_TVPixel_Data* pInstanceData,U32* pInput);

int Pete_TVPixel_Init(SPete_TVPixel_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_TVPixel_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_TVPixel_DeInit(SPete_TVPixel_Data* pInstanceData) {

}

void Pete_TVPixel_Render(SPete_TVPixel_Data* pInstanceData,SPete_TVPixel_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nNumPixels=nWidth*nHeight;
	const int nNumBytes=(nNumPixels*sizeof(U32));

	const int nPixelHeight=static_cast<int>(pSettings->m_Height);
	const int nHalfPixelHeight=(nPixelHeight/2);

	const int nStartY=-nHalfPixelHeight;
	const int nEndY=(nHeight+nHalfPixelHeight);

	int nCurrentY;
	for (nCurrentY=nStartY; nCurrentY<nEndY; nCurrentY+=nPixelHeight) {

		const int nLeftStartY=GateInt(nCurrentY,0,(nHeight-1));
		const int nLeftEndY=GateInt(nCurrentY+nPixelHeight,0,(nHeight-1));

		U32* pOutputLineLeftStart=pOutput+(nLeftStartY*nWidth);


		const int nRightStartY=GateInt(nCurrentY+nHalfPixelHeight,0,(nHeight-1));
		const int nRightEndY=GateInt(nCurrentY+nPixelHeight+nHalfPixelHeight,0,(nHeight-1));

		U32* pOutputLineRightStart=pOutput+(nRightStartY*nWidth);


		int nCurrentX;
		for (nCurrentX=0; nCurrentX<nWidth; nCurrentX+=8) {

			U32 AverageColourLeft=
				Pete_GetImageAreaAverage(
				nCurrentX,nCurrentY,
				4,nPixelHeight,
				pSource,
				nWidth,nHeight);

			U32 AverageColourRight=
				Pete_GetImageAreaAverage(
				nCurrentX+4,nCurrentY+nHalfPixelHeight,
				4,nPixelHeight,
				pSource,
				nWidth,nHeight);

			U32 RedComponentLeft=
				(AverageColourLeft&0x00ff0000);
			RedComponentLeft*=3;
			RedComponentLeft>>=1;
			RedComponentLeft=GateInt(RedComponentLeft,0,0x00ff0000);
			RedComponentLeft&=0x00ff0000;

			U32 GreenComponentLeft=
				(AverageColourLeft&0x0000ff00);
			GreenComponentLeft*=3;
			GreenComponentLeft>>=1;
			GreenComponentLeft=GateInt(GreenComponentLeft,0,0x0000ff00);
			GreenComponentLeft&=0x0000ff00;

			U32 BlueComponentLeft=
				(AverageColourLeft&0x000000ff);
			BlueComponentLeft*=3;
			BlueComponentLeft>>=1;
			BlueComponentLeft=GateInt(BlueComponentLeft,0,0x000000ff);
			BlueComponentLeft&=0x000000ff;


			U32 RedComponentRight=
				(AverageColourRight&0x00ff0000);
			RedComponentRight*=3;
			RedComponentRight>>=1;
			RedComponentRight=GateInt(RedComponentRight,0,0x00ff0000);
			RedComponentRight&=0x00ff0000;

			U32 GreenComponentRight=
				(AverageColourRight&0x0000ff00);
			GreenComponentRight*=3;
			GreenComponentRight>>=1;
			GreenComponentRight=GateInt(GreenComponentRight,0,0x0000ff00);
			GreenComponentRight&=0x0000ff00;

			U32 BlueComponentRight=
				(AverageColourRight&0x000000ff);
			BlueComponentRight*=3;
			BlueComponentRight>>=1;
			BlueComponentRight=GateInt(BlueComponentRight,0,0x000000ff);
			BlueComponentRight&=0x000000ff;

			int nPixelY;
			U32* pCurrentOutput=(pOutputLineLeftStart+nCurrentX);

			for (nPixelY=nLeftStartY; nPixelY<(nLeftEndY-1); nPixelY+=1) {

				*pCurrentOutput=RedComponentLeft;
				pCurrentOutput+=1;

				*pCurrentOutput=GreenComponentLeft;
				pCurrentOutput+=1;

				*pCurrentOutput=BlueComponentLeft;
				pCurrentOutput+=1;

				*pCurrentOutput=0x00000000;
				pCurrentOutput+=1;

				pCurrentOutput+=(nWidth-4);

			}

			if (nPixelY<nLeftEndY) {

				*pCurrentOutput=0x00000000;
				pCurrentOutput+=1;
				*pCurrentOutput=0x00000000;
				pCurrentOutput+=1;
				*pCurrentOutput=0x00000000;
				pCurrentOutput+=1;
				*pCurrentOutput=0x00000000;
				pCurrentOutput+=1;

			}

			pCurrentOutput=(pOutputLineRightStart+nCurrentX+4);

			for (nPixelY=nRightStartY; nPixelY<(nRightEndY-1); nPixelY+=1) {

				*pCurrentOutput=RedComponentRight;
				pCurrentOutput+=1;

				*pCurrentOutput=GreenComponentRight;
				pCurrentOutput+=1;

				*pCurrentOutput=BlueComponentRight;
				pCurrentOutput+=1;

				*pCurrentOutput=0x00000000;
				pCurrentOutput+=1;

				pCurrentOutput+=(nWidth-4);

			}

			if (nPixelY<nRightEndY) {

				*pCurrentOutput=0x00000000;
				pCurrentOutput+=1;
				*pCurrentOutput=0x00000000;
				pCurrentOutput+=1;
				*pCurrentOutput=0x00000000;
				pCurrentOutput+=1;
				*pCurrentOutput=0x00000000;
				pCurrentOutput+=1;

			}

		}

	}

}