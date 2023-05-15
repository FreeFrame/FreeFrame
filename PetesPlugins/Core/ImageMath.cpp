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

#include "ImageMath.h"

#include "math.h"
#ifndef PETE_MAC_OSX
#include "mmintrin.h"
#endif // PETE_MAC_OS_X

#include "PeteHelpers.h"

#ifndef PETE_MAC_OSX
void Pete_DoInPlaceSubtract(U32* pImage,U32* pInputToSubtract,int nWidth,int nHeight) {

	// This only works with even numbers of pixels, so if there's an image with odd
	// dimensions, skip the final pixel!
	const int nPixelCount=((nWidth*nHeight)/2)*2;

	U32* pCurrentImage=pImage;
	U32* pImageEnd=(pImage+nPixelCount);

	U32* pCurrentInput=pInputToSubtract;

	while (pCurrentImage!=pImageEnd) {

		__asm {

			mov		esi,[pCurrentImage]
			mov		edi,[pCurrentInput]

			movq	mm1,[edi]

			psubusb	mm1,[esi]

			movq	[esi],mm1

		}

		pCurrentImage+=2;
		pCurrentInput+=2;

	}

	_m_empty();

}

void Pete_DoInPlaceAddition(U32* pImage,U32* pInputToAdd,int nWidth,int nHeight) {

	// This only works with even numbers of pixels, so if there's an image with odd
	// dimensions, skip the final pixel!
	const int nPixelCount=((nWidth*nHeight)/2)*2;

	U32* pCurrentImage=pImage;
	U32* pImageEnd=(pImage+nPixelCount);

	U32* pCurrentInput=pInputToAdd;

	while (pCurrentImage!=pImageEnd) {

		__asm {

			mov		esi,[pCurrentImage]
			mov		edi,[pCurrentInput]

			movq	mm1,[edi]

			paddusb	mm1,[esi]

			movq	[esi],mm1

		}

		pCurrentImage+=2;
		pCurrentInput+=2;

	}

	_m_empty();

}

void Pete_DoInPlaceSubtract_OddButNice(U32* pImage,U32* pInputToSubtract,int nWidth,int nHeight) {

	// This only works with even numbers of pixels, so if there's an image with odd
	// dimensions, skip the final pixel!
	const int nPixelCount=((nWidth*nHeight)/2)*2;

	U32* pCurrentImage=pImage;
	U32* pImageEnd=(pImage+nPixelCount);

	U32* pCurrentInput=pInputToSubtract;

	__asm {

		pxor		mm0,mm0

	}

	while (pCurrentImage!=pImageEnd) {

		__asm {

			mov			esi,[pCurrentImage]
			mov			edi,[pCurrentInput]

			movq		mm1,[edi]

			psubusb		mm1,[esi]

			movq		[esi],mm1

		}

		pCurrentImage+=1;
		pCurrentInput+=1;

	}

	_m_empty();

}

void Pete_DoInPlaceAdditionScaled(U32* pImage,U32* pInputToSubtract,int nWidth,int nHeight,float Scale) {

	// This only works with even numbers of pixels, so if there's an image with odd
	// dimensions, skip the final pixel!
	const int nPixelCount=((nWidth*nHeight)/2)*2;

	U32* pCurrentImage=pImage;
	U32* pImageEnd=(pImage+nPixelCount);

	U32* pCurrentInput=pInputToSubtract;

	const int nScaleFixedPoint=static_cast<int>(Scale*16.0f);

	const __m64 ScaleReg=
		_mm_set_pi16(
		nScaleFixedPoint,
		nScaleFixedPoint,
		nScaleFixedPoint,
		nScaleFixedPoint);

	__asm {

		pxor		mm0,mm0

		movq		mm7,ScaleReg

	}

	while (pCurrentImage!=pImageEnd) {

		__asm {

			mov			esi,[pCurrentImage]
			mov			edi,[pCurrentInput]

			movq		mm1,[edi]	// mm1 is 2 image colour pixels
			movq		mm2,mm1		// mm2 will hold the higher pixel from mm1

			movq		mm3,[esi]	// mm3 is 2 colours to scale and add
			movq		mm4,mm3		// mm4 will hold the higher pixel from mm3

			punpcklbw	mm1,mm0		// mm1 holds 16 bpc lower image pixel
			punpckhbw	mm2,mm0		// mm2 holds 16 bpc higher image pixel

			punpcklbw	mm3,mm0		// mm3 holds 16 bpc lower additive pixel
			punpckhbw	mm4,mm0		// mm4 holds 16 bpc higher additive pixel

			pmullw		mm3,mm7		// scale lower additive pixel
			pmullw		mm4,mm7		// scale higher additive pixel

			psrlw		mm3,4		// get scaled lower additive pixel into correct range
			psrlw		mm4,4		// get scaled lower additive pixel into correct range

			paddusw		mm1,mm3		// add the lower scaled additive pixel to the image
			paddusw		mm2,mm4		// add the higher scaled additive pixel to the image

			packuswb	mm1,mm2		// pack the two 16bpc result pixels back into a 8bpc pixel

			movq		[esi],mm1	// store the result back in the image

		}

		pCurrentImage+=2;
		pCurrentInput+=2;

	}

	_m_empty();

}

void Pete_DoInPlaceSubtractScaled(U32* pImage,U32* pInputToSubtract,int nWidth,int nHeight,float Scale) {

	// This only works with even numbers of pixels, so if there's an image with odd
	// dimensions, skip the final pixel!
	const int nPixelCount=((nWidth*nHeight)/2)*2;

	U32* pCurrentImage=pImage;
	U32* pImageEnd=(pImage+nPixelCount);

	U32* pCurrentInput=pInputToSubtract;

	const int nScaleFixedPoint=static_cast<int>(Scale*256.0f);

	const __m64 ScaleReg=
		_mm_set_pi16(
		nScaleFixedPoint,
		nScaleFixedPoint,
		nScaleFixedPoint,
		nScaleFixedPoint);

	__asm {

		pxor		mm0,mm0

		movq		mm7,ScaleReg

	}

	while (pCurrentImage!=pImageEnd) {

		__asm {

			mov			esi,[pCurrentImage]
			mov			edi,[pCurrentInput]

			movq		mm1,[edi]

			movq		mm2,[esi]

			movq		mm3,mm1

			psubusb		mm3,mm2

			movq		mm2,mm3
			movq		mm4,mm3

			punpcklbw	mm2,mm0
			punpckhbw	mm4,mm0

			pmullw		mm2,mm7
			pmullw		mm4,mm7

			psrlw		mm2,8
			psrlw		mm4,8

			packuswb	mm2,mm4

			paddusb		mm1,mm2

			movq		[esi],mm1

		}

		pCurrentImage+=2;
		pCurrentInput+=2;

	}

	_m_empty();

}

#else // PETE_MAC_OSX

void Pete_DoInPlaceSubtract(U32* pImage,U32* pInputToSubtract,int nWidth,int nHeight) {

	const int nPixelCount=(nWidth*nHeight);

	U32* pCurrentImage=pImage;
	U32* pImageEnd=(pImage+nPixelCount);

	U32* pCurrentInput=pInputToSubtract;

	while (pCurrentImage!=pImageEnd) {

                const U32 ImageColour=*pCurrentImage;
                const U32 InputColour=*pCurrentInput;

                const int nImageRed=(ImageColour>>SHIFT_RED)&0xff;
                const int nImageGreen=(ImageColour>>SHIFT_GREEN)&0xff;
                const int nImageBlue=(ImageColour>>SHIFT_BLUE)&0xff;
                const int nImageAlpha=(ImageColour>>SHIFT_ALPHA)&0xff;

                const int nInputRed=(InputColour>>SHIFT_RED)&0xff;
                const int nInputGreen=(InputColour>>SHIFT_GREEN)&0xff;
                const int nInputBlue=(InputColour>>SHIFT_BLUE)&0xff;
                const int nInputAlpha=(InputColour>>SHIFT_ALPHA)&0xff;

                int nOutputRed=(nImageRed-nInputRed);
                nOutputRed=GateInt(nOutputRed,0,255);

                int nOutputGreen=(nImageGreen-nInputGreen);
                nOutputGreen=GateInt(nOutputGreen,0,255);

                int nOutputBlue=(nImageBlue-nInputBlue);
                nOutputBlue=GateInt(nOutputBlue,0,255);

                int nOutputAlpha=(nImageAlpha-nInputAlpha);
                nOutputAlpha=GateInt(nOutputAlpha,0,255);

                U32 OutputColour=
                    (nOutputRed<<SHIFT_RED)|
                    (nOutputGreen<<SHIFT_GREEN)|
                    (nOutputBlue<<SHIFT_BLUE)|
                    (nOutputAlpha<<SHIFT_ALPHA);

                *pCurrentImage=OutputColour;

		pCurrentImage+=1;
		pCurrentInput+=1;

	}

}

void Pete_DoInPlaceAddition(U32* pImage,U32* pInputToAdd,int nWidth,int nHeight) {

	const int nPixelCount=(nWidth*nHeight);

	U32* pCurrentImage=pImage;
	U32* pImageEnd=(pImage+nPixelCount);

	U32* pCurrentInput=pInputToAdd;

	while (pCurrentImage!=pImageEnd) {

                const U32 ImageColour=*pCurrentImage;
                const U32 InputColour=*pCurrentInput;

                const int nImageRed=(ImageColour>>SHIFT_RED)&0xff;
                const int nImageGreen=(ImageColour>>SHIFT_GREEN)&0xff;
                const int nImageBlue=(ImageColour>>SHIFT_BLUE)&0xff;
                const int nImageAlpha=(ImageColour>>SHIFT_ALPHA)&0xff;

                const int nInputRed=(InputColour>>SHIFT_RED)&0xff;
                const int nInputGreen=(InputColour>>SHIFT_GREEN)&0xff;
                const int nInputBlue=(InputColour>>SHIFT_BLUE)&0xff;
                const int nInputAlpha=(InputColour>>SHIFT_ALPHA)&0xff;

                int nOutputRed=(nImageRed+nInputRed);
                nOutputRed=GateInt(nOutputRed,0,255);

                int nOutputGreen=(nImageGreen+nInputGreen);
                nOutputGreen=GateInt(nOutputGreen,0,255);

                int nOutputBlue=(nImageBlue+nInputBlue);
                nOutputBlue=GateInt(nOutputBlue,0,255);

                int nOutputAlpha=(nImageAlpha+nInputAlpha);
                nOutputAlpha=GateInt(nOutputAlpha,0,255);

                U32 OutputColour=
                    (nOutputRed<<SHIFT_RED)|
                    (nOutputGreen<<SHIFT_GREEN)|
                    (nOutputBlue<<SHIFT_BLUE)|
                    (nOutputAlpha<<SHIFT_ALPHA);

                *pCurrentImage=OutputColour;

		pCurrentImage+=1;
		pCurrentInput+=1;

	}

}

void Pete_DoInPlaceAdditionScaled(U32* pImage,U32* pInputToAdd,int nWidth,int nHeight,float Scale) {

	const int nPixelCount=(nWidth*nHeight);

	const int nScaleFixedPoint=static_cast<int>(Scale*16.0f);

	U32* pCurrentImage=pImage;
	U32* pImageEnd=(pImage+nPixelCount);

	U32* pCurrentInput=pInputToAdd;

	while (pCurrentImage!=pImageEnd) {

                const U32 ImageColour=*pCurrentInput;
                const U32 InputColour=*pCurrentImage;

                const int nImageRed=(ImageColour>>SHIFT_RED)&0xff;
                const int nImageGreen=(ImageColour>>SHIFT_GREEN)&0xff;
                const int nImageBlue=(ImageColour>>SHIFT_BLUE)&0xff;
                const int nImageAlpha=(ImageColour>>SHIFT_ALPHA)&0xff;

                const int nInputRed=(InputColour>>SHIFT_RED)&0xff;
                const int nInputGreen=(InputColour>>SHIFT_GREEN)&0xff;
                const int nInputBlue=(InputColour>>SHIFT_BLUE)&0xff;
                const int nInputAlpha=(InputColour>>SHIFT_ALPHA)&0xff;

				const int nScaledRed=(nInputRed*nScaleFixedPoint)>>4;
				const int nScaledGreen=(nInputGreen*nScaleFixedPoint)>>4;
				const int nScaledBlue=(nInputBlue*nScaleFixedPoint)>>4;
				const int nScaledAlpha=(nInputAlpha*nScaleFixedPoint)>>4;

                int nOutputRed=(nImageRed+nScaledRed);
                nOutputRed=GateInt(nOutputRed,0,255);

                int nOutputGreen=(nImageGreen+nScaledGreen);
                nOutputGreen=GateInt(nOutputGreen,0,255);

                int nOutputBlue=(nImageBlue+nScaledBlue);
                nOutputBlue=GateInt(nOutputBlue,0,255);

                int nOutputAlpha=(nImageAlpha+nScaledAlpha);
                nOutputAlpha=GateInt(nOutputAlpha,0,255);

                U32 OutputColour=
                    (nOutputRed<<SHIFT_RED)|
                    (nOutputGreen<<SHIFT_GREEN)|
                    (nOutputBlue<<SHIFT_BLUE)|
                    (nOutputAlpha<<SHIFT_ALPHA);

                *pCurrentImage=OutputColour;

		pCurrentImage+=1;
		pCurrentInput+=1;

	}

}

void Pete_DoInPlaceSubtract_OddButNice(U32* pImage,U32* pInputToSubtract,int nWidth,int nHeight) {

	const int nPixelCount=(nWidth*nHeight);

	U32* pCurrentImage=pImage;
	U32* pImageEnd=(pImage+nPixelCount);

	U32* pCurrentInput=pInputToSubtract;

	while (pCurrentImage!=pImageEnd) {

                const U32 ImageColour=*pCurrentImage;
                const U32 InputColour=*pCurrentInput;

                const int nImageRed=(ImageColour>>SHIFT_RED)&0xff;
                const int nImageGreen=(ImageColour>>SHIFT_GREEN)&0xff;
                const int nImageBlue=(ImageColour>>SHIFT_BLUE)&0xff;
                const int nImageAlpha=(ImageColour>>SHIFT_ALPHA)&0xff;

                const int nInputRed=(InputColour>>SHIFT_RED)&0xff;
                const int nInputGreen=(InputColour>>SHIFT_GREEN)&0xff;
                const int nInputBlue=(InputColour>>SHIFT_BLUE)&0xff;
                const int nInputAlpha=(InputColour>>SHIFT_ALPHA)&0xff;

                int nOutputRed=(nImageRed-nInputRed);
                nOutputRed=GateInt(nOutputRed,0,255);

                int nOutputGreen=(nImageGreen-nInputGreen);
                nOutputGreen=GateInt(nOutputGreen,0,255);

                int nOutputBlue=(nImageBlue-nInputBlue);
                nOutputBlue=GateInt(nOutputBlue,0,255);

                int nOutputAlpha=(nImageAlpha-nInputAlpha);
                nOutputAlpha=GateInt(nOutputAlpha,0,255);

                nOutputRed-=nInputRed;
                nOutputRed=GateInt(nOutputRed,0,255);

                nOutputGreen-=nInputGreen;
                nOutputGreen=GateInt(nOutputGreen,0,255);

                nOutputBlue-=nInputBlue;
                nOutputBlue=GateInt(nOutputBlue,0,255);

                nOutputAlpha-=nInputAlpha;
                nOutputAlpha=GateInt(nOutputAlpha,0,255);

                U32 OutputColour=
                    (nOutputRed<<SHIFT_RED)|
                    (nOutputGreen<<SHIFT_GREEN)|
                    (nOutputBlue<<SHIFT_BLUE)|
                    (nOutputAlpha<<SHIFT_ALPHA);

                *pCurrentImage=OutputColour;

		pCurrentImage+=1;
		pCurrentInput+=1;

	}

}

void Pete_DoInPlaceSubtractScaled(U32* pImage,U32* pInputToSubtract,int nWidth,int nHeight,float Scale) {

	const int nScaleFixedPoint=static_cast<int>(Scale*256.0f);

	const int nPixelCount=(nWidth*nHeight);

	U32* pCurrentImage=pImage;
	U32* pImageEnd=(pImage+nPixelCount);

	U32* pCurrentInput=pInputToSubtract;

	while (pCurrentImage!=pImageEnd) {

                const U32 ImageColour=*pCurrentImage;
                const U32 InputColour=*pCurrentInput;

                const int nImageRed=(ImageColour>>SHIFT_RED)&0xff;
                const int nImageGreen=(ImageColour>>SHIFT_GREEN)&0xff;
                const int nImageBlue=(ImageColour>>SHIFT_BLUE)&0xff;
                const int nImageAlpha=(ImageColour>>SHIFT_ALPHA)&0xff;

                const int nInputRed=(InputColour>>SHIFT_RED)&0xff;
                const int nInputGreen=(InputColour>>SHIFT_GREEN)&0xff;
                const int nInputBlue=(InputColour>>SHIFT_BLUE)&0xff;
                const int nInputAlpha=(InputColour>>SHIFT_ALPHA)&0xff;

                int nOutputRed=(nImageRed-nInputRed);
                nOutputRed*=nScaleFixedPoint;
                nOutputRed>>=8;
                nOutputRed=GateInt(nOutputRed,0,255);

                int nOutputGreen=(nImageGreen-nInputGreen);
                nOutputGreen*=nScaleFixedPoint;
                nOutputGreen>>=8;
                nOutputGreen=GateInt(nOutputGreen,0,255);

                int nOutputBlue=(nImageBlue-nInputBlue);
                nOutputBlue*=nScaleFixedPoint;
                nOutputBlue>>=8;
                nOutputBlue=GateInt(nOutputBlue,0,255);

                int nOutputAlpha=(nImageAlpha-nInputAlpha);
                nOutputAlpha*=nScaleFixedPoint;
                nOutputAlpha>>=8;
                nOutputAlpha=GateInt(nOutputAlpha,0,255);

                U32 OutputColour=
                    (nOutputRed<<SHIFT_RED)|
                    (nOutputGreen<<SHIFT_GREEN)|
                    (nOutputBlue<<SHIFT_BLUE)|
                    (nOutputAlpha<<SHIFT_ALPHA);

                *pCurrentImage=OutputColour;

		pCurrentImage+=1;
		pCurrentInput+=1;

	}


}


#endif // PETE_MAC_OSX

U32 Pete_GetImageAreaAverage(int nLeftX,int nTopY,int nDeltaX,int nDeltaY,U32* pImageData,int nImageWidth,int nImageHeight) {

	if (nLeftX<0) {
		nDeltaX-=(0-nLeftX);
		nLeftX=0;
	}

	if (nTopY<0) {
		nDeltaY-=(0-nTopY);
		nTopY=0;
	}

	if ((nLeftX+nDeltaX)>=nImageWidth) {
		nDeltaX-=((nLeftX+nDeltaX)-(nImageWidth-1));
	}

	if ((nTopY+nDeltaY)>=nImageHeight) {
		nDeltaY-=((nTopY+nDeltaY)-(nImageHeight-1));
	}

	if ((nDeltaX<1)||(nDeltaY<1)) {
		return 0;
	}

	U32* pSourceStart=
		pImageData+
		(nTopY*nImageWidth)+
		nLeftX;

	U32* pSourceEnd=
		pSourceStart+
		(nDeltaY*nImageWidth);

	U32* pCurrentSource=pSourceStart;

	int nRedTotal=0;
	int nGreenTotal=0;
	int nBlueTotal=0;

	while (pCurrentSource<pSourceEnd) {

		U32* pSourceLineStart=pCurrentSource;
		U32* pSourceLineEnd=pCurrentSource+nDeltaX;

		while (pCurrentSource<pSourceLineEnd) {

			const U32 CurrentColour=*pCurrentSource;

			const int nCurrentRed=(CurrentColour>>SHIFT_RED)&0xff;
			const int nCurrentGreen=(CurrentColour>>SHIFT_GREEN)&0xff;
			const int nCurrentBlue=(CurrentColour>>SHIFT_BLUE)&0xff;

			nRedTotal+=nCurrentRed;
			nGreenTotal+=nCurrentGreen;
			nBlueTotal+=nCurrentBlue;

			pCurrentSource+=1;

		}

		pCurrentSource=pSourceLineStart+nImageWidth;

	}

	const int nTotalSamples=(nDeltaX*nDeltaY);

	const int nRedAverage=(nRedTotal/nTotalSamples);
	const int nGreenAverage=(nGreenTotal/nTotalSamples);
	const int nBlueAverage=(nBlueTotal/nTotalSamples);

	return (nRedAverage<<SHIFT_RED)|(nGreenAverage<<SHIFT_GREEN)|(nBlueAverage<<SHIFT_BLUE);

}

void Pete_DoInPlaceAdditionReplaceAlpha(U32* pImage,U32* pInputToAdd,int nWidth,int nHeight) {

	const int nPixelCount=(nWidth*nHeight);

	U32* pCurrentImage=pImage;
	U32* pImageEnd=(pImage+nPixelCount);

	U32* pCurrentInput=pInputToAdd;

	while (pCurrentImage!=pImageEnd) {

                const U32 ImageColour=*pCurrentImage;
                const U32 InputColour=*pCurrentInput;

                const int nImageRed=(ImageColour>>SHIFT_RED)&0xff;
                const int nImageGreen=(ImageColour>>SHIFT_GREEN)&0xff;
                const int nImageBlue=(ImageColour>>SHIFT_BLUE)&0xff;
                const int nImageAlpha=(ImageColour>>SHIFT_ALPHA)&0xff;

                int nInputRed=(InputColour>>SHIFT_RED)&0xff;
                int nInputGreen=(InputColour>>SHIFT_GREEN)&0xff;
                int nInputBlue=(InputColour>>SHIFT_BLUE)&0xff;
                int nInputAlpha=(InputColour>>SHIFT_ALPHA)&0xff;

				nInputRed=(nInputRed*nInputAlpha)>>8;
				nInputGreen=(nInputGreen*nInputAlpha)>>8;
				nInputBlue=(nInputBlue*nInputAlpha)>>8;

                int nOutputRed=(nImageRed+nInputRed);
                nOutputRed=GateInt(nOutputRed,0,255);

                int nOutputGreen=(nImageGreen+nInputGreen);
                nOutputGreen=GateInt(nOutputGreen,0,255);

                int nOutputBlue=(nImageBlue+nInputBlue);
                nOutputBlue=GateInt(nOutputBlue,0,255);

                int nOutputAlpha;
                if (nImageAlpha>nInputAlpha) {
                	nOutputAlpha=nImageAlpha;
                } else {
                	nOutputAlpha=nInputAlpha;
                }

                U32 OutputColour=
                    (nOutputRed<<SHIFT_RED)|
                    (nOutputGreen<<SHIFT_GREEN)|
                    (nOutputBlue<<SHIFT_BLUE)|
                    (nOutputAlpha<<SHIFT_ALPHA);

                *pCurrentImage=OutputColour;

		pCurrentImage+=1;
		pCurrentInput+=1;

	}

}

