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

#include "ImageScale.h"

#include "PeteHelpers.h"

void Pete_ImageCopyAndScale(U32* pSource,U32* pOutput,
	int nSourceWidth,int nSourceHeight,
	int nOutputWidth,int nOutputHeight) {

	const int nFPShift=16;
	const int nFPMult=(1<<nFPShift);

	const int nNumOutputPixels=(nOutputWidth*nOutputHeight);

	U32* pOutputEnd=pOutput+nNumOutputPixels;

	U32* pCurrentOutput=pOutput;

	if ((nOutputWidth==0)||(nOutputHeight==0)) {
		return;
	}

	const int nSourceXInc=
		((nSourceWidth<<nFPShift)/nOutputWidth);
	const int nSourceYInc=
		((nSourceHeight<<nFPShift)/nOutputHeight);

	int nSourceY=0;
	while (pCurrentOutput<pOutputEnd) {

		U32* pOutputLineEnd=(pCurrentOutput+nOutputWidth);

		U32* pSourceLineStart=
			pSource+
			((nSourceY>>nFPShift)*nSourceWidth);

		int nSourceX=0;

		while (pCurrentOutput<pOutputLineEnd) {

			U32* pCurrentSource=
				pSourceLineStart+(nSourceX>>nFPShift);			

			*pCurrentOutput=*pCurrentSource;

			pCurrentOutput+=1;
			nSourceX+=nSourceXInc;

		}

		nSourceY+=nSourceYInc;

	}

}
