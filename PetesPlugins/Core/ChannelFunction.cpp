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


#include "ChannelFunction.h"

#include "PeteHelpers.h"

void Pete_ChannelFunction_Render(SPete_ChannelFunction_Settings* pSettings,U32* pSource, U32* pOutput) {
	
	const int nWidth=pSettings->m_nWidth;
	const int nHeight=pSettings->m_nHeight;

	const int*const pRedTable=pSettings->m_nRedTable;
	const int*const pGreenTable=pSettings->m_nGreenTable;
	const int*const pBlueTable=pSettings->m_nBlueTable;

	const int nNumPixels = nWidth*nHeight;

	U32* pCurrentSource=pSource;
	U32* pCurrentOutput=pOutput;
	const U32* pSourceEnd=(pSource+nNumPixels);

	while (pCurrentSource!=pSourceEnd) {
		
		const U32 SourceColour=*pCurrentSource;

		const unsigned int nSourceRed=(SourceColour>>SHIFT_RED)&0xff;
		const unsigned int nSourceGreen=(SourceColour>>SHIFT_GREEN)&0xff;
		const unsigned int nSourceBlue=(SourceColour>>SHIFT_BLUE)&0xff;
		const unsigned int nSourceAlpha=(SourceColour&(((U32)0xff)<<SHIFT_ALPHA));

		const int nOutputRed=pRedTable[nSourceRed];
		const int nOutputGreen=pGreenTable[nSourceGreen];
		const int nOutputBlue=pBlueTable[nSourceBlue];

		const U32 OutputColour=
			(nOutputRed)|
			(nOutputGreen)|
			(nOutputBlue)|
			(nSourceAlpha);

		*pCurrentOutput=OutputColour;

		pCurrentSource+=1;
		pCurrentOutput+=1;

	}


}

