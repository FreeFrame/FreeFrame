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

#include "BoxFilter.h"

#include "math.h"

#ifndef PETE_MAC_OSX
#include "mmintrin.h"
#endif // PETE_MAC_OSX

#include "PeteHelpers.h"
#include "PeteProcessor.h"

#ifndef PETE_MAC_OSX
void Pete_DoBoxFilterBlur(U32* pSourceBuffer,U32* pIntermediateBuffer,U32* pOutputBuffer,int nKernelSizeX,int nKernelSizeY,int nWidth,int nHeight,int nPassCount) {
	
	if ((pSourceBuffer==NULL)||
		(pIntermediateBuffer==NULL)||
		(pOutputBuffer==NULL)) {
		return;
	}

	nKernelSizeX=GateInt(nKernelSizeX,1,((nWidth/2)-1));
	nKernelSizeY=GateInt(nKernelSizeY,1,((nWidth/2)-1));

	const int nKernelWidthX=(nKernelSizeX*2)+1;

	const int nKernelSizeMultX=
		(1<<16)/nKernelWidthX;

	const int nKernelWidthY=(nKernelSizeY*2)+1;

	const int nKernelSizeMultY=
		(1<<16)/nKernelWidthY;

	const __m64 InitMultRegX=
		_mm_set_pi16(
		(nKernelSizeX+1),
		(nKernelSizeX+1),
		(nKernelSizeX+1),
		(nKernelSizeX+1));

	const __m64 RecipRegX=
		_mm_set_pi16(
		nKernelSizeMultX,
		nKernelSizeMultX,
		nKernelSizeMultX,
		nKernelSizeMultX);

	const __m64 WriteMask=
		_mm_set_pi32(
		0xffffffff,
		0x00000000);
	
	int nY;
	for (nY=0; nY<nHeight; ++nY) {

		const int nLineStartOffset=(nY*nWidth);

		int nCurrentPass;
		for (nCurrentPass=0; nCurrentPass<nPassCount; nCurrentPass+=1) {

			U32* pLineSource;
			U32* pLineOutput;

			if (nCurrentPass==0) {
				pLineSource=pSourceBuffer;
				pLineOutput=pIntermediateBuffer;
			} else if ((nCurrentPass&0x1)==1) {
				pLineSource=pIntermediateBuffer;
				pLineOutput=pOutputBuffer;
			} else {
				pLineSource=pOutputBuffer;
				pLineOutput=pIntermediateBuffer;
			}

			U32* pSourceLineStart=
				(pLineSource+nLineStartOffset);

			U32* pSourceLineLast=
				pSourceLineStart+(nWidth-1);

			U32* pCurrentOutput=pLineOutput+nLineStartOffset;


//			U32 FirstPixelColour=*pSourceLineStart;
//			const int nFirstPixelRed=(FirstPixelColour>>SHIFT_RED)&0xff;
//			const int nFirstPixelGreen=(FirstPixelColour>>SHIFT_GREEN)&0xff;
//			const int nFirstPixelBlue=(FirstPixelColour>>SHIFT_BLUE)&0xff;

//			U32 LastPixelColour=*pSourceLineLast;
//			const int nLastPixelRed=(LastPixelColour>>SHIFT_RED)&0xff;
//			const int nLastPixelGreen=(LastPixelColour>>SHIFT_GREEN)&0xff;
//			const int nLastPixelBlue=(LastPixelColour>>SHIFT_BLUE)&0xff;

//			int nRedTotal=(nFirstPixelRed*(nKernelSize+1));
//			int nGreenTotal=(nFirstPixelGreen*(nKernelSize+1));
//			int nBlueTotal=(nFirstPixelBlue*(nKernelSize+1));

			// mm0 is running total
			// mm1 is first pixel colour
			// mm2 is last pixel colour
			// mm3 is (1/sample count)<<16
			// mm4 is zero register
			__asm {

				pxor		mm4,mm4

				movq		mm7,InitMultRegX
				
				mov			esi,pSourceLineStart;
				movq		mm1,[esi]
				punpcklbw	mm1,mm4

				mov			esi,pSourceLineLast;
				movq		mm2,[esi]
				punpcklbw	mm2,mm4

				movq		mm3,RecipRegX

				movq		mm0,mm1
				pmullw		mm0,mm7


			}

			U32* pRightSample=pSourceLineStart+1;

			int nK;
			for (nK=1; nK<=nKernelSizeX; nK+=1) {

//				U32 PixelColour=*pRightSample;
//				const int nPixelRed=(PixelColour>>SHIFT_RED)&0xff;
//				const int nPixelGreen=(PixelColour>>SHIFT_GREEN)&0xff;
//				const int nPixelBlue=(PixelColour>>SHIFT_BLUE)&0xff;
//
//				nRedTotal+=nPixelRed;
//				nGreenTotal+=nPixelGreen;
//				nBlueTotal+=nPixelBlue;

				__asm {

					mov			esi,pRightSample
					movq		mm7,[esi]
					punpcklbw	mm7,mm4

					paddw		mm0,mm7

				}

				pRightSample+=1;

			}

			U32* pLeftSample=pSourceLineStart-nKernelSizeX;

			// Pete- Beginning case, should only be a few pixels (comparatively), so
			// left fairly unoptimized

			int nX;
			for (nX=0; nX<nKernelSizeX; ++nX) {

//				const int nOutputRed=(nRedTotal*nKernelSizeMult)>>SHIFT_RED;
//				const int nOutputGreen=(nGreenTotal*nKernelSizeMult)>>16;
//				const int nOutputBlue=(nBlueTotal*nKernelSizeMult)>>16;
//
//				const U32 OutputColour=
//					(nOutputRed<<SHIFT_RED)|
//					(nOutputGreen<<SHIFT_GREEN)|
//					(nOutputBlue<<SHIFT_BLUE);
//
//				*pCurrentIntermediate=OutputColour;

				__asm {

					movq		mm7,mm0
					pmulhw		mm7,mm3

					packuswb	mm7,mm4
					mov			esi,pCurrentOutput
					movq		mm6,[esi]
					pand		mm6,WriteMask
					por			mm7,mm6
					movq		[esi],mm7

				}


//				nLeftRed=nFirstPixelRed;
//				nLeftGreen=nFirstPixelGreen;
//				nLeftBlue=nFirstPixelBlue;

				__asm {

					psubw		mm0,mm1

				}

//				int nRightRed;
//				int nRightGreen;
//				int nRightBlue;
				if (pRightSample>=pSourceLineLast) {

//					nRightRed=nLastPixelRed;
//					nRightGreen=nLastPixelGreen;
//					nRightBlue=nLastPixelBlue;

					__asm {

						paddw		mm0,mm2

					}

				} else {

//					U32 RightColour=*pRightSample;
//					nRightRed=(RightColour>>SHIFT_RED)&0xff;
//					nRightGreen=(RightColour>>SHIFT_GREEN)&0xff;
//					nRightBlue=(RightColour>>SHIFT_BLUE)&0xff;

					__asm {

						mov			esi,pRightSample
						movq		mm7,[esi]
						punpcklbw	mm7,mm4
						paddw		mm0,mm7

					}

				}

//				nRedTotal-=nLeftRed;
//				nRedTotal+=nRightRed;
//
//				nGreenTotal-=nLeftGreen;
//				nGreenTotal+=nRightGreen;
//
//				nBlueTotal-=nLeftBlue;
//				nBlueTotal+=nRightBlue;

				pLeftSample+=1;
				pRightSample+=1;

				pCurrentOutput+=1;
				
			}

			// Pete- Most pixels are dealt with here, so optimize to buggery

			const int nMidLoopMax=(nWidth-(nKernelSizeX+1));

			__asm {

				mov			eax,pCurrentOutput
				mov			ebx,pLeftSample
				mov			ecx,pRightSample

				mov			edx,nX
				mov			edi,nMidLoopMax

Pete_DoBoxFilterBlur_MidLoopX_Start:

				cmp			edx,edi
				jge			Pete_DoBoxFilterBlur_MidLoopX_End

					movq		mm6,mm0
					pmulhw		mm6,mm3

//				U32 LeftColour=*pLeftSample;
//				nLeftRed=(LeftColour>>SHIFT_RED)&0xff;
//				nLeftGreen=(LeftColour>>SHIFT_GREEN)&0xff;
//				nLeftBlue=(LeftColour>>SHIFT_BLUE)&0xff;
//
					movq		mm7,[ebx]
					punpcklbw	mm7,mm4
					psubw		mm0,mm7


//				U32 RightColour=*pRightSample;
//				nRightRed=(RightColour>>SHIFT_RED)&0xff;
//				nRightGreen=(RightColour>>SHIFT_GREEN)&0xff;
//				nRightBlue=(RightColour>>SHIFT_BLUE)&0xff;

					movq		mm7,[ecx]
					punpcklbw	mm7,mm4
					paddw		mm0,mm7

//				nRedTotal-=nLeftRed;
//				nRedTotal+=nRightRed;
//
//				nGreenTotal-=nLeftGreen;
//				nGreenTotal+=nRightGreen;
//
//				nBlueTotal-=nLeftBlue;
//				nBlueTotal+=nRightBlue;

//				pLeftSample+=1;
//				pRightSample+=1;
//
//				pCurrentOutput+=1;

					add		ebx,4
					add		ecx,4

//				const int nOutputRed=(nRedTotal*nKernelSizeMult)>>SHIFT_RED;
//				const int nOutputGreen=(nGreenTotal*nKernelSizeMult)>>16;
//				const int nOutputBlue=(nBlueTotal*nKernelSizeMult)>>16;
//
//				const U32 OutputColour=
//					(nOutputRed<<SHIFT_RED)|
//					(nOutputGreen<<SHIFT_GREEN)|
//					(nOutputBlue<<SHIFT_BLUE);
//
//				*pCurrentIntermediate=OutputColour;

					movq		mm7,mm0
					pmulhw		mm7,mm3

					packuswb	mm6,mm7
					movq		[eax],mm6

//				int nLeftRed;
//				int nLeftGreen;
//				int nLeftBlue;
//
//				U32 LeftColour=*pLeftSample;
//				nLeftRed=(LeftColour>>SHIFT_RED)&0xff;
//				nLeftGreen=(LeftColour>>SHIFT_GREEN)&0xff;
//				nLeftBlue=(LeftColour>>SHIFT_BLUE)&0xff;

					movq		mm7,[ebx]
					punpcklbw	mm7,mm4
					psubw		mm0,mm7

//				int nRightRed;
//				int nRightGreen;
//				int nRightBlue;
//
//				U32 RightColour=*pRightSample;
//				nRightRed=(RightColour>>SHIFT_RED)&0xff;
//				nRightGreen=(RightColour>>SHIFT_GREEN)&0xff;
//				nRightBlue=(RightColour>>SHIFT_BLUE)&0xff;

					movq		mm7,[ecx]
					punpcklbw	mm7,mm4
					paddw		mm0,mm7

//				nRedTotal-=nLeftRed;
//				nRedTotal+=nRightRed;
//
//				nGreenTotal-=nLeftGreen;
//				nGreenTotal+=nRightGreen;
//
//				nBlueTotal-=nLeftBlue;
//				nBlueTotal+=nRightBlue;

					add			eax,8
					add			ebx,4
					add			ecx,4

//				pLeftSample+=2;
//				pRightSample+=2;
//
//				pCurrentOutput+=2;

					add			edx,2
					jmp			Pete_DoBoxFilterBlur_MidLoopX_Start

Pete_DoBoxFilterBlur_MidLoopX_End:
				
				mov			pCurrentOutput,eax
				mov			pLeftSample,ebx
				mov			pRightSample,ecx

				mov			nX,edx
				
			}

			// Pete- Ending case, should only be a few pixels (comparatively), so
			// left fairly unoptimized

			for (; nX<nWidth; ++nX) {

//				const int nOutputRed=(nRedTotal*nKernelSizeMult)>>16;
//				const int nOutputGreen=(nGreenTotal*nKernelSizeMult)>>16;
//				const int nOutputBlue=(nBlueTotal*nKernelSizeMult)>>16;
//
//				const U32 OutputColour=
//					(nOutputRed<<SHIFT_RED)|
//					(nOutputGreen<<SHIFT_GREEN)|
//					(nOutputBlue<<SHIFT_BLUE);
//
//				*pCurrentIntermediate=OutputColour;

				__asm {

					movq		mm7,mm0
					pmulhw		mm7,mm3

					packuswb	mm7,mm4
					mov			esi,pCurrentOutput
					movq		mm6,[esi]
					pand		mm6,WriteMask
					por			mm7,mm6
					movq		[esi],mm7

				}

//				int nLeftRed;
//				int nLeftGreen;
//				int nLeftBlue;
				if (pLeftSample<=pSourceLineStart) {

//					nLeftRed=nFirstPixelRed;
//					nLeftGreen=nFirstPixelGreen;
//					nLeftBlue=nFirstPixelBlue;

					__asm {

						psubw		mm0,mm1

					}

				} else {

//					U32 LeftColour=*pLeftSample;
//					nLeftRed=(LeftColour>>SHIFT_RED)&0xff;
//					nLeftGreen=(LeftColour>>SHIFT_GREEN)&0xff;
//					nLeftBlue=(LeftColour>>SHIFT_BLUE)&0xff;

					__asm {

						mov			esi,pLeftSample
						movq		mm7,[esi]
						punpcklbw	mm7,mm4
						psubw		mm0,mm7

					}

				}

//				int nRightRed;
//				int nRightGreen;
//				int nRightBlue;

//				nRightRed=nLastPixelRed;
//				nRightGreen=nLastPixelGreen;
//				nRightBlue=nLastPixelBlue;

				__asm {

					paddw		mm0,mm2

				}

//				nRedTotal-=nLeftRed;
//				nRedTotal+=nRightRed;
//
//				nGreenTotal-=nLeftGreen;
//				nGreenTotal+=nRightGreen;
//
//				nBlueTotal-=nLeftBlue;
//				nBlueTotal+=nRightBlue;

				pLeftSample+=1;
				pRightSample+=1;

				pCurrentOutput+=1;
				
			}



		}

	}

	const __m64 InitMultRegY=
		_mm_set_pi16(
		(nKernelSizeY+1),
		(nKernelSizeY+1),
		(nKernelSizeY+1),
		(nKernelSizeY+1));

	const __m64 RecipRegY=
		_mm_set_pi16(
		nKernelSizeMultY,
		nKernelSizeMultY,
		nKernelSizeMultY,
		nKernelSizeMultY);

	const int nMaxX=(nWidth-1);

	int nX;
	for (nX=0; nX<nMaxX; nX+=2) {

		int nCurrentPass;
		for (nCurrentPass=0; nCurrentPass<nPassCount; nCurrentPass+=1) {

			U32* pLineSource;
			U32* pLineOutput;

			if ((nCurrentPass&0x1)==1) {

				if ((nPassCount&0x1)==1) {
					pLineSource=pOutputBuffer;
					pLineOutput=pIntermediateBuffer;
				} else {
					pLineSource=pIntermediateBuffer;
					pLineOutput=pOutputBuffer;
				}

			} else {

				if ((nPassCount&0x1)==1) {
					pLineSource=pIntermediateBuffer;
					pLineOutput=pOutputBuffer;
				} else {
					pLineSource=pOutputBuffer;
					pLineOutput=pIntermediateBuffer;
				}

			}

			U32* pCurrentOutput=
				(pLineOutput+nX);

			U32* pSourceRowStart=
				(pLineSource+nX);

			U32* pSourceRowLast=
				pSourceRowStart+((nHeight-1)*nWidth);

//			U32 FirstPixelColour=*pSourceRowStart;
//			const int nFirstPixelRed=(FirstPixelColour>>SHIFT_RED)&0xff;
//			const int nFirstPixelGreen=(FirstPixelColour>>SHIFT_GREEN)&0xff;
//			const int nFirstPixelBlue=(FirstPixelColour>>SHIFT_BLUE)&0xff;
//	
//			U32 LastPixelColour=*pSourceRowLast;
//			const int nLastPixelRed=(LastPixelColour>>SHIFT_RED)&0xff;
//			const int nLastPixelGreen=(LastPixelColour>>SHIFT_GREEN)&0xff;
//			const int nLastPixelBlue=(LastPixelColour>>SHIFT_BLUE)&0xff;
//
//			int nRedTotal=(nFirstPixelRed*(nKernelSize+1));
//			int nGreenTotal=(nFirstPixelGreen*(nKernelSize+1));
//			int nBlueTotal=(nFirstPixelBlue*(nKernelSize+1));

			__m64 SecondRowFirstPixelColour;
			__m64 SecondRowLastPixelColour;

			// mm0 is running total
			// mm1 is first pixel colour
			// mm2 is last pixel colour
			// mm3 is (1/sample count)<<16
			// mm4 is second row total
			// mm5 is second row first pixel
			// mm6 is second row second pixel
			__asm {

				pxor		mm7,mm7

				mov			esi,pSourceRowStart;
				movq		mm1,[esi]
				movq		mm5,mm1
				punpcklbw	mm1,mm7
				punpckhbw	mm5,mm7
				movq		SecondRowFirstPixelColour,mm5

				mov			esi,pSourceRowLast;

				movq		mm2,[esi]
				movq		mm6,mm2
				punpcklbw	mm2,mm7
				punpckhbw	mm6,mm7
				movq		SecondRowLastPixelColour,mm6

				movq		mm3,RecipRegY

				movq		mm7,InitMultRegY
				
				movq		mm0,mm1
				pmullw		mm0,mm7

				movq		mm4,mm5
				pmullw		mm4,mm7

			}

			
			U32* pBottomSample=pSourceRowStart+nWidth;

			__asm {

				pxor		mm6,mm6

			}

			int nK;
			for (nK=1; nK<=nKernelSizeY; nK+=1) {

//				U32 PixelColour=*pBottomSample;
//				const int nPixelRed=(PixelColour>>SHIFT_RED)&0xff;
//				const int nPixelGreen=(PixelColour>>SHIFT_GREEN)&0xff;
//				const int nPixelBlue=(PixelColour>>SHIFT_BLUE)&0xff;
//	
//				nRedTotal+=nPixelRed;
//				nGreenTotal+=nPixelGreen;
//				nBlueTotal+=nPixelBlue;

				 __asm {

					mov			esi,pBottomSample
					movq		mm7,[esi]
					movq		mm5,mm7
					punpcklbw	mm7,mm6
					punpckhbw	mm5,mm6

					paddw		mm0,mm7
					paddw		mm4,mm5

				}

				pBottomSample+=nWidth;

			}

			U32* pTopSample=
				pSourceRowStart-(nKernelSizeY*nWidth);

			// Pete- Beginning case, should only be a few pixels (comparatively), so
			// left fairly unoptimized

			int nY;
			for (nY=0; nY<nHeight; ++nY) {

//				const int nOutputRed=(nRedTotal*nKernelSizeMult)>>SHIFT_RED;
//				const int nOutputGreen=(nGreenTotal*nKernelSizeMult)>>16;
//				const int nOutputBlue=(nBlueTotal*nKernelSizeMult)>>16;
//	
//				const U32 OutputColour=
//					(nOutputRed<<SHIFT_RED)|
//					(nOutputGreen<<SHIFT_GREEN)|
//					(nOutputBlue<<SHIFT_BLUE);
//	
//				*pCurrentOutput=OutputColour;


				 __asm {

					movq		mm7,mm0
					pmulhw		mm7,mm3

					movq		mm6,mm4
					pmulhw		mm6,mm3
					
					packuswb	mm7,mm6
					mov			esi,pCurrentOutput
					movq		[esi],mm7

				}

//				int nTopRed;
//				int nTopGreen;
//				int nTopBlue;
				if (pTopSample<=pSourceRowStart) {

//					nTopRed=nFirstPixelRed;
//					nTopGreen=nFirstPixelGreen;
//					nTopBlue=nFirstPixelBlue;

					 __asm {

						psubw		mm0,mm1

						movq		mm5,SecondRowFirstPixelColour
						psubw		mm4,mm5

					}

				} else {

//					U32 TopColour=*pTopSample;
//					nTopRed=(TopColour>>SHIFT_RED)&0xff;
//					nTopGreen=(TopColour>>SHIFT_GREEN)&0xff;
//					nTopBlue=(TopColour>>SHIFT_BLUE)&0xff;

					 __asm {

						pxor		mm5,mm5
						mov			esi,pTopSample
						movq		mm7,[esi]
						movq		mm6,mm7

						punpcklbw	mm7,mm5
						psubw		mm0,mm7

						punpckhbw	mm6,mm5
						psubw		mm4,mm6

					}

				}

//				int nBottomRed;
//				int nBottomGreen;
//				int nBottomBlue;
				if (pBottomSample>=pSourceRowLast) {

//					nBottomRed=nLastPixelRed;
//					nBottomGreen=nLastPixelGreen;
//					nBottomBlue=nLastPixelBlue;

					 __asm {

						paddw		mm0,mm2
						
						movq		mm6,SecondRowLastPixelColour
						paddw		mm4,mm6

					}

				} else {

//					U32 BottomColour=*pBottomSample;
//					nBottomRed=(BottomColour>>SHIFT_RED)&0xff;
//					nBottomGreen=(BottomColour>>SHIFT_GREEN)&0xff;
//					nBottomBlue=(BottomColour>>SHIFT_BLUE)&0xff;

					 __asm {

						pxor		mm6,mm6
						mov			esi,pBottomSample
						movq		mm7,[esi]
						movq		mm5,mm7

						punpcklbw	mm7,mm6
						paddw		mm0,mm7

						punpckhbw	mm5,mm6
						paddw		mm4,mm5

					 }

				}

//				nRedTotal+=nBottomRed;
//				nRedTotal-=nTopRed;
//	
//				nGreenTotal+=nBottomGreen;
//				nGreenTotal-=nTopGreen;
//	
//				nBlueTotal+=nBottomBlue;
//				nBlueTotal-=nTopBlue;

				pTopSample+=nWidth;
				pBottomSample+=nWidth;

				pCurrentOutput+=nWidth;
				
			}	

			
		}

	}

	_m_empty();

}

#else // PETE_MAC_OSX

void Pete_DoBoxFilterBlur_CVersion(U32* pSourceBuffer,U32* pIntermediateBuffer,U32* pOutputBuffer,int nKernelSizeX,int nKernelSizeY,int nWidth,int nHeight,int nPassCount);
void Pete_DoBoxFilterBlur_AltiVecVersion(U32* pSourceBuffer,U32* pIntermediateBuffer,U32* pOutputBuffer,int nKernelSizeX,int nKernelSizeY,int nWidth,int nHeight,int nPassCount);

void Pete_DoBoxFilterBlur(U32* pSourceBuffer,U32* pIntermediateBuffer,U32* pOutputBuffer,int nKernelSizeX,int nKernelSizeY,int nWidth,int nHeight,int nPassCount) {

//	const bool bHasAltiVec=Pete_IsAltiVecAvailable();
//	
//	if (bHasAltiVec) {
//		Pete_DoBoxFilterBlur_AltiVecVersion(pSourceBuffer,pIntermediateBuffer,pOutputBuffer,nKernelSizeX,nKernelSizeY,nWidth,nHeight,nPassCount);
//	} else {
		Pete_DoBoxFilterBlur_CVersion(pSourceBuffer,pIntermediateBuffer,pOutputBuffer,nKernelSizeX,nKernelSizeY,nWidth,nHeight,nPassCount);	
//	}

}

void Pete_DoBoxFilterBlur_CVersion(U32* pSourceBuffer,U32* pIntermediateBuffer,U32* pOutputBuffer,int nKernelSizeX,int nKernelSizeY,int nWidth,int nHeight,int nPassCount) {
        
    if ((pSourceBuffer==NULL)||
        (pIntermediateBuffer==NULL)||
        (pOutputBuffer==NULL)) {
        return;
    }

    nKernelSizeX=GateInt(nKernelSizeX,1,((nWidth/2)-1));
    nKernelSizeY=GateInt(nKernelSizeY,1,((nWidth/2)-1));

    const int nKernelWidthX=(nKernelSizeX*2)+1;

    const int nKernelSizeMultX=
        (1<<16)/nKernelWidthX;

    const int nKernelWidthY=(nKernelSizeY*2)+1;

    const int nKernelSizeMultY=
        (1<<16)/nKernelWidthY;

    int nY;
    for (nY=0; nY<nHeight; ++nY) {

        const int nLineStartOffset=(nY*nWidth);

        int nCurrentPass;
        for (nCurrentPass=0; nCurrentPass<nPassCount; nCurrentPass+=1) {

            U32* pLineSource;
            U32* pLineOutput;

            if (nCurrentPass==0) {
                pLineSource=pSourceBuffer;
                pLineOutput=pIntermediateBuffer;
            } else if ((nCurrentPass&0x1)==1) {
                pLineSource=pIntermediateBuffer;
                pLineOutput=pOutputBuffer;
            } else {
                pLineSource=pOutputBuffer;
                pLineOutput=pIntermediateBuffer;
            }

            U32* pSourceLineStart=
                (pLineSource+nLineStartOffset);

            U32* pSourceLineLast=
                pSourceLineStart+(nWidth-1);

            U32* pCurrentOutput=pLineOutput+nLineStartOffset;


            U32 FirstPixelColour=*pSourceLineStart;
            const int nFirstPixelRed=(FirstPixelColour>>SHIFT_RED)&0xff;
            const int nFirstPixelGreen=(FirstPixelColour>>SHIFT_GREEN)&0xff;
            const int nFirstPixelBlue=(FirstPixelColour>>SHIFT_BLUE)&0xff;
			const int nFirstPixelAlpha=(FirstPixelColour>>SHIFT_ALPHA)&0xff;

            U32 LastPixelColour=*pSourceLineLast;
            const int nLastPixelRed=(LastPixelColour>>SHIFT_RED)&0xff;
            const int nLastPixelGreen=(LastPixelColour>>SHIFT_GREEN)&0xff;
            const int nLastPixelBlue=(LastPixelColour>>SHIFT_BLUE)&0xff;
			const int nLastPixelAlpha=(LastPixelColour>>SHIFT_ALPHA)&0xff;

            int nRedTotal=(nFirstPixelRed*(nKernelSizeX+1));
            int nGreenTotal=(nFirstPixelGreen*(nKernelSizeX+1));
            int nBlueTotal=(nFirstPixelBlue*(nKernelSizeX+1));
            int nAlphaTotal=(nFirstPixelAlpha*(nKernelSizeX+1));

            U32* pRightSample=pSourceLineStart+1;

            int nK;
            for (nK=1; nK<=nKernelSizeX; nK+=1) {

                U32 PixelColour=*pRightSample;
                const int nPixelRed=(PixelColour>>SHIFT_RED)&0xff;
                const int nPixelGreen=(PixelColour>>SHIFT_GREEN)&0xff;
                const int nPixelBlue=(PixelColour>>SHIFT_BLUE)&0xff;
                const int nPixelAlpha=(PixelColour>>SHIFT_ALPHA)&0xff;

                nRedTotal+=nPixelRed;
                nGreenTotal+=nPixelGreen;
                nBlueTotal+=nPixelBlue;
                nAlphaTotal+=nPixelAlpha;

                pRightSample+=1;

            }

            U32* pLeftSample=pSourceLineStart-nKernelSizeX;

            int nX;
            for (nX=0; nX<nWidth; ++nX) {

                const int nOutputRed=(nRedTotal*nKernelSizeMultX)>>16;
                const int nOutputGreen=(nGreenTotal*nKernelSizeMultX)>>16;
                const int nOutputBlue=(nBlueTotal*nKernelSizeMultX)>>16;
                const int nOutputAlpha=(nAlphaTotal*nKernelSizeMultX)>>16;

                const U32 OutputColour=
                    (nOutputRed<<SHIFT_RED)|
                    (nOutputGreen<<SHIFT_GREEN)|
                    (nOutputBlue<<SHIFT_BLUE)|
                    (nOutputAlpha<<SHIFT_ALPHA);

                *pCurrentOutput=OutputColour;

                int nLeftRed;
                int nLeftGreen;
                int nLeftBlue;
                int nLeftAlpha;

                if (pLeftSample<pSourceLineStart) {

                    nLeftRed=nFirstPixelRed;
                    nLeftGreen=nFirstPixelGreen;
                    nLeftBlue=nFirstPixelBlue;
                    nLeftAlpha=nFirstPixelAlpha;

                } else {

                    U32 LeftColour=*pLeftSample;
                    nLeftRed=(LeftColour>>SHIFT_RED)&0xff;
                    nLeftGreen=(LeftColour>>SHIFT_GREEN)&0xff;
                    nLeftBlue=(LeftColour>>SHIFT_BLUE)&0xff;
                    nLeftAlpha=(LeftColour>>SHIFT_ALPHA)&0xff;

                }

                int nRightRed;
                int nRightGreen;
                int nRightBlue;
                int nRightAlpha;
                if (pRightSample>=pSourceLineLast) {

                    nRightRed=nLastPixelRed;
                    nRightGreen=nLastPixelGreen;
                    nRightBlue=nLastPixelBlue;
                    nRightAlpha=nLastPixelAlpha;

                } else {

                    U32 RightColour=*pRightSample;
                    nRightRed=(RightColour>>SHIFT_RED)&0xff;
                    nRightGreen=(RightColour>>SHIFT_GREEN)&0xff;
                    nRightBlue=(RightColour>>SHIFT_BLUE)&0xff;
                    nRightAlpha=(RightColour>>SHIFT_ALPHA)&0xff;

                }

                nRedTotal-=nLeftRed;
                nRedTotal+=nRightRed;

                nGreenTotal-=nLeftGreen;
                nGreenTotal+=nRightGreen;

                nBlueTotal-=nLeftBlue;
                nBlueTotal+=nRightBlue;

                nAlphaTotal-=nLeftAlpha;
                nAlphaTotal+=nRightAlpha;

                pLeftSample+=1;
                pRightSample+=1;

                pCurrentOutput+=1;
				
            }

        }

    }
    
    int nX;
    for (nX=0; nX<nWidth; ++nX) {

        const int nRowStartOffset=nX;

        int nCurrentPass;
        for (nCurrentPass=0; nCurrentPass<nPassCount; nCurrentPass+=1) {

            U32* pRowSource;
            U32* pRowOutput;

            if ((nCurrentPass&0x1)==1) {

                if ((nPassCount&0x1)==1) {
                    pRowSource=pOutputBuffer;
                    pRowOutput=pIntermediateBuffer;
                } else {
                    pRowSource=pIntermediateBuffer;
                    pRowOutput=pOutputBuffer;
                }

            } else {

                if ((nPassCount&0x1)==1) {
                    pRowSource=pIntermediateBuffer;
                    pRowOutput=pOutputBuffer;
                } else {
                    pRowSource=pOutputBuffer;
                    pRowOutput=pIntermediateBuffer;
                }

            }

            U32* pCurrentOutput=
                (pRowOutput+nRowStartOffset);

            U32* pSourceRowStart=
                (pRowSource+nRowStartOffset);

            U32* pSourceRowLast=
                pSourceRowStart+((nHeight-1)*nWidth);

            U32 FirstPixelColour=*pSourceRowStart;
            const int nFirstPixelRed=(FirstPixelColour>>SHIFT_RED)&0xff;
            const int nFirstPixelGreen=(FirstPixelColour>>SHIFT_GREEN)&0xff;
            const int nFirstPixelBlue=(FirstPixelColour>>SHIFT_BLUE)&0xff;
            const int nFirstPixelAlpha=(FirstPixelColour>>SHIFT_ALPHA)&0xff;
	
            U32 LastPixelColour=*pSourceRowLast;
            const int nLastPixelRed=(LastPixelColour>>SHIFT_RED)&0xff;
            const int nLastPixelGreen=(LastPixelColour>>SHIFT_GREEN)&0xff;
            const int nLastPixelBlue=(LastPixelColour>>SHIFT_BLUE)&0xff;
            const int nLastPixelAlpha=(LastPixelColour>>SHIFT_ALPHA)&0xff;

            int nRedTotal=(nFirstPixelRed*(nKernelSizeY+1));
            int nGreenTotal=(nFirstPixelGreen*(nKernelSizeY+1));
            int nBlueTotal=(nFirstPixelBlue*(nKernelSizeY+1));
            int nAlphaTotal=(nFirstPixelAlpha*(nKernelSizeY+1));
			
            U32* pBottomSample=pSourceRowStart+nWidth;

            int nK;
            for (nK=1; nK<=nKernelSizeY; nK+=1) {

		U32 PixelColour=*pBottomSample;
		const int nPixelRed=(PixelColour>>SHIFT_RED)&0xff;
		const int nPixelGreen=(PixelColour>>SHIFT_GREEN)&0xff;
		const int nPixelBlue=(PixelColour>>SHIFT_BLUE)&0xff;
		const int nPixelAlpha=(PixelColour>>SHIFT_ALPHA)&0xff;
	
		nRedTotal+=nPixelRed;
                nGreenTotal+=nPixelGreen;
		nBlueTotal+=nPixelBlue;
		nAlphaTotal+=nPixelAlpha;

                pBottomSample+=nWidth;

            }

            U32* pTopSample=
                pSourceRowStart-(nKernelSizeY*nWidth);

            int nY;
            for (nY=0; nY<nHeight; ++nY) {

                const int nOutputRed=(nRedTotal*nKernelSizeMultY)>>16;
                const int nOutputGreen=(nGreenTotal*nKernelSizeMultY)>>16;
                const int nOutputBlue=(nBlueTotal*nKernelSizeMultY)>>16;
                const int nOutputAlpha=(nAlphaTotal*nKernelSizeMultY)>>16;

                const U32 OutputColour=
                    (nOutputRed<<SHIFT_RED)|
                    (nOutputGreen<<SHIFT_GREEN)|
                    (nOutputBlue<<SHIFT_BLUE)|
                    (nOutputAlpha<<SHIFT_ALPHA);

                *pCurrentOutput=OutputColour;

                int nTopRed;
                int nTopGreen;
                int nTopBlue;
                int nTopAlpha;
                if (pTopSample<=pSourceRowStart) {

                    nTopRed=nFirstPixelRed;
                    nTopGreen=nFirstPixelGreen;
                    nTopBlue=nFirstPixelBlue;
                    nTopAlpha=nFirstPixelAlpha;

                } else {

                    U32 TopColour=*pTopSample;
                    nTopRed=(TopColour>>SHIFT_RED)&0xff;
                    nTopGreen=(TopColour>>SHIFT_GREEN)&0xff;
                    nTopBlue=(TopColour>>SHIFT_BLUE)&0xff;
                    nTopAlpha=(TopColour>>SHIFT_ALPHA)&0xff;

                }

                int nBottomRed;
                int nBottomGreen;
                int nBottomBlue;
                int nBottomAlpha;
                if (pBottomSample>=pSourceRowLast) {

                    nBottomRed=nLastPixelRed;
                    nBottomGreen=nLastPixelGreen;
                    nBottomBlue=nLastPixelBlue;
                    nBottomAlpha=nLastPixelAlpha;

                } else {

                    U32 BottomColour=*pBottomSample;
                    nBottomRed=(BottomColour>>SHIFT_RED)&0xff;
                    nBottomGreen=(BottomColour>>SHIFT_GREEN)&0xff;
                    nBottomBlue=(BottomColour>>SHIFT_BLUE)&0xff;
                    nBottomAlpha=(BottomColour>>SHIFT_ALPHA)&0xff;

                }

                nRedTotal+=nBottomRed;
                nRedTotal-=nTopRed;
	
                nGreenTotal+=nBottomGreen;
                nGreenTotal-=nTopGreen;
	
                nBlueTotal+=nBottomBlue;
                nBlueTotal-=nTopBlue;

                nAlphaTotal+=nBottomAlpha;
                nAlphaTotal-=nTopAlpha;

                pTopSample+=nWidth;
                pBottomSample+=nWidth;

                pCurrentOutput+=nWidth;
                				
            }

        }

    }

}

void Pete_DoBoxFilterBlur_AltiVecVersion(U32* pSourceBuffer,U32* pIntermediateBuffer,U32* pOutputBuffer,int nKernelSizeX,int nKernelSizeY,int nWidth,int nHeight,int nPassCount) {

#if 1
		Pete_DoBoxFilterBlur_CVersion(pSourceBuffer,pIntermediateBuffer,pOutputBuffer,nKernelSizeX,nKernelSizeY,nWidth,nHeight,nPassCount);	
#else
        
    if ((pSourceBuffer==NULL)||
        (pIntermediateBuffer==NULL)||
        (pOutputBuffer==NULL)) {
        return;
    }

    nKernelSizeX=GateInt(nKernelSizeX,1,((nWidth/2)-1));
    nKernelSizeY=GateInt(nKernelSizeY,1,((nWidth/2)-1));

    const int nKernelWidthX=(nKernelSizeX*2)+1;

    const int nKernelSizeMultX=
        (1<<16)/nKernelWidthX;

    const int nKernelWidthY=(nKernelSizeY*2)+1;

    const int nKernelSizeMultY=
        (1<<16)/nKernelWidthY;

    int nY;
    for (nY=0; nY<nHeight; ++nY) {

        const int nLineStartOffset=(nY*nWidth);

        int nCurrentPass;
        for (nCurrentPass=0; nCurrentPass<nPassCount; nCurrentPass+=1) {

            U32* pLineSource;
            U32* pLineOutput;

            if (nCurrentPass==0) {
                pLineSource=pSourceBuffer;
                pLineOutput=pIntermediateBuffer;
            } else if ((nCurrentPass&0x1)==1) {
                pLineSource=pIntermediateBuffer;
                pLineOutput=pOutputBuffer;
            } else {
                pLineSource=pOutputBuffer;
                pLineOutput=pIntermediateBuffer;
            }

            U32* pSourceLineStart=
                (pLineSource+nLineStartOffset);

            U32* pSourceLineLast=
                pSourceLineStart+(nWidth-1);

            U32* pCurrentOutput=pLineOutput+nLineStartOffset;


            U32 FirstPixelColour=*pSourceLineStart;
            const int nFirstPixelRed=(FirstPixelColour>>SHIFT_RED)&0xff;
            const int nFirstPixelGreen=(FirstPixelColour>>SHIFT_GREEN)&0xff;
            const int nFirstPixelBlue=(FirstPixelColour>>SHIFT_BLUE)&0xff;

            U32 LastPixelColour=*pSourceLineLast;
            const int nLastPixelRed=(LastPixelColour>>SHIFT_RED)&0xff;
            const int nLastPixelGreen=(LastPixelColour>>SHIFT_GREEN)&0xff;
            const int nLastPixelBlue=(LastPixelColour>>SHIFT_BLUE)&0xff;

            int nRedTotal=(nFirstPixelRed*(nKernelSizeX+1));
            int nGreenTotal=(nFirstPixelGreen*(nKernelSizeX+1));
            int nBlueTotal=(nFirstPixelBlue*(nKernelSizeX+1));

			vector int TotalOverall={0,nRedTotal,nGreenTotal,nBlueTotal};

            U32* pRightSample=pSourceLineStart+1;

            int nK;
            for (nK=1; nK<=nKernelSizeX; nK+=1) {

                U32 PixelColour=*pRightSample;
                const int nPixelRed=(PixelColour>>SHIFT_RED)&0xff;
                const int nPixelGreen=(PixelColour>>SHIFT_GREEN)&0xff;
                const int nPixelBlue=(PixelColour>>SHIFT_BLUE)&0xff;

				vector int CurrentPixel={0,nPixelRed,nPixelGreen,nPixelBlue};
		
				TotalOverall=vec_add(TotalOverall,CurrentPixel);

                pRightSample+=1;

            }

            U32* pLeftSample=pSourceLineStart-nKernelSizeX;

			vector 

            int nX;
            for (nX=0; nX<nWidth; ++nX) {

                const int nOutputRed=(nRedTotal*nKernelSizeMultX)>>16;
                const int nOutputGreen=(nGreenTotal*nKernelSizeMultX)>>16;
                const int nOutputBlue=(nBlueTotal*nKernelSizeMultX)>>16;

                const U32 OutputColour=
                    (nOutputRed<<SHIFT_RED)|
                    (nOutputGreen<<SHIFT_GREEN)|
                    (nOutputBlue<<SHIFT_BLUE);

                *pCurrentOutput=OutputColour;

                int nLeftRed;
                int nLeftGreen;
                int nLeftBlue;

                if (pLeftSample<pSourceLineStart) {

                    nLeftRed=nFirstPixelRed;
                    nLeftGreen=nFirstPixelGreen;
                    nLeftBlue=nFirstPixelBlue;

                } else {

                    U32 LeftColour=*pLeftSample;
                    nLeftRed=(LeftColour>>SHIFT_RED)&0xff;
                    nLeftGreen=(LeftColour>>SHIFT_GREEN)&0xff;
                    nLeftBlue=(LeftColour>>SHIFT_BLUE)&0xff;

                }

                int nRightRed;
                int nRightGreen;
                int nRightBlue;
                if (pRightSample>=pSourceLineLast) {

                    nRightRed=nLastPixelRed;
                    nRightGreen=nLastPixelGreen;
                    nRightBlue=nLastPixelBlue;

                } else {

                    U32 RightColour=*pRightSample;
                    nRightRed=(RightColour>>SHIFT_RED)&0xff;
                    nRightGreen=(RightColour>>SHIFT_GREEN)&0xff;
                    nRightBlue=(RightColour>>SHIFT_BLUE)&0xff;

                }

                nRedTotal-=nLeftRed;
                nRedTotal+=nRightRed;

                nGreenTotal-=nLeftGreen;
                nGreenTotal+=nRightGreen;

                nBlueTotal-=nLeftBlue;
                nBlueTotal+=nRightBlue;

                pLeftSample+=1;
                pRightSample+=1;

                pCurrentOutput+=1;
				
            }

        }

    }
    
    int nX;
    for (nX=0; nX<nWidth; ++nX) {

        const int nRowStartOffset=nX;

        int nCurrentPass;
        for (nCurrentPass=0; nCurrentPass<nPassCount; nCurrentPass+=1) {

            U32* pRowSource;
            U32* pRowOutput;

            if ((nCurrentPass&0x1)==1) {

                if ((nPassCount&0x1)==1) {
                    pRowSource=pOutputBuffer;
                    pRowOutput=pIntermediateBuffer;
                } else {
                    pRowSource=pIntermediateBuffer;
                    pRowOutput=pOutputBuffer;
                }

            } else {

                if ((nPassCount&0x1)==1) {
                    pRowSource=pIntermediateBuffer;
                    pRowOutput=pOutputBuffer;
                } else {
                    pRowSource=pOutputBuffer;
                    pRowOutput=pIntermediateBuffer;
                }

            }

            U32* pCurrentOutput=
                (pRowOutput+nRowStartOffset);

            U32* pSourceRowStart=
                (pRowSource+nRowStartOffset);

            U32* pSourceRowLast=
                pSourceRowStart+((nHeight-1)*nWidth);

            U32 FirstPixelColour=*pSourceRowStart;
            const int nFirstPixelRed=(FirstPixelColour>>SHIFT_RED)&0xff;
            const int nFirstPixelGreen=(FirstPixelColour>>SHIFT_GREEN)&0xff;
            const int nFirstPixelBlue=(FirstPixelColour>>SHIFT_BLUE)&0xff;
	
            U32 LastPixelColour=*pSourceRowLast;
            const int nLastPixelRed=(LastPixelColour>>SHIFT_RED)&0xff;
            const int nLastPixelGreen=(LastPixelColour>>SHIFT_GREEN)&0xff;
            const int nLastPixelBlue=(LastPixelColour>>SHIFT_BLUE)&0xff;

            int nRedTotal=(nFirstPixelRed*(nKernelSizeY+1));
            int nGreenTotal=(nFirstPixelGreen*(nKernelSizeY+1));
            int nBlueTotal=(nFirstPixelBlue*(nKernelSizeY+1));
			
            U32* pBottomSample=pSourceRowStart+nWidth;

            int nK;
            for (nK=1; nK<=nKernelSizeY; nK+=1) {

		U32 PixelColour=*pBottomSample;
		const int nPixelRed=(PixelColour>>SHIFT_RED)&0xff;
		const int nPixelGreen=(PixelColour>>SHIFT_GREEN)&0xff;
		const int nPixelBlue=(PixelColour>>SHIFT_BLUE)&0xff;
	
		nRedTotal+=nPixelRed;
                nGreenTotal+=nPixelGreen;
		nBlueTotal+=nPixelBlue;

                pBottomSample+=nWidth;

            }

            U32* pTopSample=
                pSourceRowStart-(nKernelSizeY*nWidth);

            int nY;
            for (nY=0; nY<nHeight; ++nY) {

                const int nOutputRed=(nRedTotal*nKernelSizeMultY)>>16;
                const int nOutputGreen=(nGreenTotal*nKernelSizeMultY)>>16;
                const int nOutputBlue=(nBlueTotal*nKernelSizeMultY)>>16;

                const U32 OutputColour=
                    (nOutputRed<<SHIFT_RED)|
                    (nOutputGreen<<SHIFT_GREEN)|
                    (nOutputBlue<<SHIFT_BLUE);

                *pCurrentOutput=OutputColour;

                int nTopRed;
                int nTopGreen;
                int nTopBlue;
                if (pTopSample<=pSourceRowStart) {

                    nTopRed=nFirstPixelRed;
                    nTopGreen=nFirstPixelGreen;
                    nTopBlue=nFirstPixelBlue;

                } else {

                    U32 TopColour=*pTopSample;
                    nTopRed=(TopColour>>SHIFT_RED)&0xff;
                    nTopGreen=(TopColour>>SHIFT_GREEN)&0xff;
                    nTopBlue=(TopColour>>SHIFT_BLUE)&0xff;

                }

                int nBottomRed;
                int nBottomGreen;
                int nBottomBlue;
                if (pBottomSample>=pSourceRowLast) {

                    nBottomRed=nLastPixelRed;
                    nBottomGreen=nLastPixelGreen;
                    nBottomBlue=nLastPixelBlue;

                } else {

                    U32 BottomColour=*pBottomSample;
                    nBottomRed=(BottomColour>>SHIFT_RED)&0xff;
                    nBottomGreen=(BottomColour>>SHIFT_GREEN)&0xff;
                    nBottomBlue=(BottomColour>>SHIFT_BLUE)&0xff;

                }

                nRedTotal+=nBottomRed;
                nRedTotal-=nTopRed;
	
                nGreenTotal+=nBottomGreen;
                nGreenTotal-=nTopGreen;
	
                nBlueTotal+=nBottomBlue;
                nBlueTotal-=nTopBlue;

                pTopSample+=nWidth;
                pBottomSample+=nWidth;

                pCurrentOutput+=nWidth;
                				
            }

        }

    }
    
#endif

}

#endif // PETE_MAC_OSX
