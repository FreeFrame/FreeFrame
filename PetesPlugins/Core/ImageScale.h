#ifndef INCLUDE_IMAGESCALE_H
#define INCLUDE_IMAGESCALE_H

#include "PeteHelpers.h"

void Pete_ImageCopyAndScale(U32* pSource,U32* pOutput,
	int nSourceWidth,int nSourceHeight,
	int nOutputWidth,int nOutputHeight);

#endif // INCLUDE_IMAGESCALE_H