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

#include "BackLight.h"
#include "PeteHelpers.h"

//#define PETE_DEBUG_BACKLIGHT	// Enable this define to get an infinite loop when the effect writes outside of
								// the output buffer

static SPete_Parameter g_Parameters[]={
	{
		"Spike Scale",
		127.0f,
		0.0f,
		255.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Spike Floor",
		0.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Spike Ceiling",
		255.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_BackLight_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_BackLight_Init(SPete_BackLight_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_BackLight_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_BackLight_DeInit(SPete_BackLight_Data* pInstanceData) {

	// do nothing

}

void Pete_BackLight_Render(SPete_BackLight_Data* pInstanceData,SPete_BackLight_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nFixedShift=8;
	const int nFixedMult=(1<<nFixedShift);

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nHalfWidth=(nWidth/2);
	const int nHalfHeight=(nHeight/2);

	const int nNumPixels = nWidth*nHeight;

	U32* pCurrentSource=pSource;
	U32* pCurrentOutput=pOutput;
	const U32* pSourceEnd=(pSource+nNumPixels);
	const U32* pOutputEnd=(pOutput+nNumPixels);

	Pete_ZeroMemory(pOutput,sizeof(U32)*nNumPixels);

	const int nSpikeScale=static_cast<int>(pSettings->m_SpikeScale);
	const int nSpikeFloor=static_cast<int>(pSettings->m_SpikeFloor);
	const int nSpikeCeiling=static_cast<int>(pSettings->m_SpikeCeiling);

	int nY=0;
	while (pCurrentSource!=pSourceEnd) {

		const U32* pSourceLineStart=pCurrentSource;
		const U32* pSourceLineEnd=pCurrentSource+nWidth;

		int nX=0;
		while (pCurrentSource!=pSourceLineEnd) {

			U32 SourceColour=*pCurrentSource;

			int nRed=(SourceColour&(0xff<<SHIFT_RED))>>SHIFT_RED;
			int nGreen=(SourceColour&(0xff<<SHIFT_GREEN))>>SHIFT_GREEN;
			int nBlue=(SourceColour&(0xff<<SHIFT_BLUE))>>SHIFT_BLUE;

			int nLuminance =
				((90 * nRed)+
				(115 * nGreen)+
				(51 * nBlue));

			nLuminance>>=8;
//			SourceColour|=(nLuminance<<24);

			nLuminance=GateInt(nLuminance,nSpikeFloor,nSpikeCeiling);
			nLuminance-=nSpikeFloor;

			const int nLength=((nLuminance*nSpikeScale)>>nFixedShift);

			int nDeltaX=((nX-nHalfWidth)*nLength)>>8;
			int nDeltaY=((nY-nHalfHeight)*nLength)>>8;

			int nEndX=nX+nDeltaX;
			if (nEndX>nWidth) {
				nEndX=nWidth;
			} else if (nEndX<0) {
				nEndX=0;
			}

			int nEndY=nY+nDeltaY;
			if (nEndY>nHeight) {
				nEndY=nHeight;
			} else if (nEndY<0) {
				nEndY=0;
			}

			int nXInc;
			if (nDeltaX<0) {
				nXInc=-1;
			} else {
				nXInc=1;
			}

			int nYInc;
			if (nDeltaY<0) {
				nYInc=-1;
			} else {
				nYInc=1;
			}

			nDeltaX*=nXInc;
			nDeltaY*=nYInc;

			int nCurrentX=nX;
			int nCurrentY=nY;

			if ((nDeltaX==0)&&(nDeltaY==0)) {
				nDeltaX=1;
				nEndX+=1;
				nEndY+=1;
			} else if (nDeltaX==0) {
				nEndX+=1;
			} else if (nDeltaY==0) {
				nEndY+=1;
			}

			U32* pDest=(pOutput+(nCurrentY*nWidth)+nCurrentX);
			const int nDestYInc=(nWidth*nYInc);
			const int nDestXInc=nXInc;

			if (nDeltaX>nDeltaY) {

				int	nCounter=nDeltaY;

				while ((nCurrentX!=nEndX)&&(nCurrentY!=nEndY)) {

#ifdef PETE_DEBUG_BACKLIGHT
					if ((nCurrentX<0)||
						(nCurrentX>=nWidth)||
						(nCurrentY<0)||
						(nCurrentY>=nHeight)||
						(pDest<pOutput)||
						(pDest>=pOutputEnd)) {

						while (true); // Pete- Infinite loop, easy way to tell if this triggered!

					}
#endif // PETE_DEBUG_BACKLIGHT

					const U32 DestColour=*pDest;
					if (DestColour<SourceColour) {
						*pDest=SourceColour;
					} else {
						break;
					}

					if (nCounter>=nDeltaX) {
						nCounter-=nDeltaX;
						nCurrentY+=nYInc;
						pDest+=nDestYInc;
					}
					nCurrentX+=nXInc;
					pDest+=nDestXInc;

					nCounter+=nDeltaY;

				}


			} else {

				int	nCounter=nDeltaX;

				while ((nCurrentX!=nEndX)&&(nCurrentY!=nEndY)) {


#ifdef PETE_DEBUG_BACKLIGHT
					if ((nCurrentX<0)||
						(nCurrentX>=nWidth)||
						(nCurrentY<0)||
						(nCurrentY>=nHeight)||
						(pDest<pOutput)||
						(pDest>=pOutputEnd)) {

						while (true); // Pete- Infinite loop, easy way to tell if this triggered!

					}
#endif // PETE_DEBUG_BACKLIGHT

					const U32 DestColour=*pDest;
					if (DestColour<SourceColour) {
						*pDest=SourceColour;
					} else {
						break;
					}

					if (nCounter>=nDeltaY) {
						nCounter-=nDeltaY;
						nCurrentX+=nXInc;
						pDest+=nDestXInc;
					}
					nCurrentY+=nYInc;
					pDest+=nDestYInc;

					nCounter+=nDeltaX;

				};

			}

			pCurrentSource+=1;
			pCurrentOutput+=1;
			nX+=1;

		}

		nY+=1;

	}


}

