#ifndef INCLUDE_IMAGEMATH_H
#define INCLUDE_IMAGEMATH_H

#include "PeteHelpers.h"

void Pete_DoInPlaceSubtract(U32* pImage,U32* pInputToSubtract,int nWidth,int nHeight);
void Pete_DoInPlaceAddition(U32* pImage,U32* pInputToAdd,int nWidth,int nHeight);
void Pete_DoInPlaceAdditionReplaceAlpha(U32* pImage,U32* pInputToAdd,int nWidth,int nHeight);
void Pete_DoInPlaceAdditionScaled(U32* pImage,U32* pInputToAdd,int nWidth,int nHeight,float Scale);
void Pete_DoInPlaceSubtractScaled(U32* pImage,U32* pInputToSubtract,int nWidth,int nHeight,float Scale);
void Pete_DoInPlaceSubtract_OddButNice(U32* pImage,U32* pInputToSubtract,int nWidth,int nHeight);
U32 Pete_GetImageAreaAverage(int nLeftX,int nTopY,int nDeltaX,int nDeltaY,U32* pImageData,int nImageWidth,int nImageHeight);

#endif // INCLUDE_IMAGEMATH_H