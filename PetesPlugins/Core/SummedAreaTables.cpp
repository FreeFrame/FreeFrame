/* Copyright (C) 2003 Pete Warden

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

#include "SummedAreaTables.h"

void Pete_SummedAreaTable_Construct(U32* pSource,int nWidth,int nHeight,SPete_SATEntry* poutTable) {

	int nY;
	for (nY=0; nY<nHeight; nY+=1) {

		U32 nTotalRed=0;
		U32 nTotalGreen=0;
		U32 nTotalBlue=0;
		U32 nTotalAlpha=0;

		U32* pCurrentSource=
			(pSource+(nY*nWidth));

		SPete_SATEntry* pCurrentTable=
			(poutTable+(nY*nWidth));

		int nX;
		for (nX=0; nX<nWidth; nX+=1) {

			const U32 SourceColour=*pCurrentSource;

			const int nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
			const int nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
			const int nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;
			const int nSourceAlpha=(SourceColour>>SHIFT_ALPHA)&0xff;

			nTotalRed+=nSourceRed;
			nTotalGreen+=nSourceGreen;
			nTotalBlue+=nSourceBlue;
			nTotalAlpha+=nSourceAlpha;

			pCurrentTable->Red=nTotalRed;
			pCurrentTable->Green=nTotalGreen;
			pCurrentTable->Blue=nTotalBlue;
			pCurrentTable->Alpha=nTotalAlpha;

			pCurrentSource+=1;
			pCurrentTable+=1;

		}

	}

	int nX;
	for (nX=0; nX<nWidth; nX+=1) {

		SPete_SATEntry* pCurrentTable=
			(poutTable+nX);

		int nY;
		for (nY=1; nY<nHeight; nY+=1) {

			Pete_SummedAreaTable_Add(pCurrentTable+(nWidth),pCurrentTable);

			pCurrentTable+=nWidth;

		}
	
	}

}
