#ifndef INCLUDE_BOXFILTER_H
#define INCLUDE_BOXFILTER_H

#include "PeteHelpers.h"

void Pete_DoBoxFilterBlur(U32* pSource,U32* pIntermediate,U32* pOutput,int nKernelSizeX,int nKernelSizeY,int nWidth,int nHeight,int nPassCount);

#endif // INCLUDE_BOXFILTER_H