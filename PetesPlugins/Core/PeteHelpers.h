#ifndef INCLUDE_PETEHELPERS_H
#define INCLUDE_PETEHELPERS_H

#include "stdlib.h"
#include "string.h"
#include "math.h"

// Pete- Put this in a per-platform section, or remove as appropriate
typedef unsigned long U32; 
typedef unsigned short U16; 

#ifdef PETE_MAC_OSX
#define assert(X) ((void)0)
#else
#include <assert.h>
#endif // PETE_MAC_OSX

#define SHIFT_ALPHA	(24)
#define SHIFT_RED	(16)
#define SHIFT_GREEN	(8)
#define SHIFT_BLUE	(0)

inline int GateInt(int nValue,int nMin,int nMax) {

	if (nValue<nMin) {
		return nMin;
	} else if (nValue>nMax) {
		return nMax;
	} else {
		return nValue;
	}
}

inline int MinimumInt(int nFirst,int nSecond) {

	if (nFirst<nSecond) {
		return nFirst;
	} else {
		return nSecond;
	}

}

inline int MaximumInt(int nFirst,int nSecond) {

	if (nFirst>nSecond) {
		return nFirst;
	} else {
		return nSecond;
	}

}

inline int ModInt(int nValue,int nRange) {
	return ((nValue%nRange)+nRange)%nRange;
}

inline void* GatePtr(void* pValue,void* pMin,void* pMax) {

	if (pValue<pMin) {
		return pMin;
	} else if (pValue>pMax) {
		return pMax;
	} else {
		return pValue;
	}
}

inline int AbsInt(int inValue) {
	if (inValue>0) {
		return inValue;
	} else {
		return -inValue;
	}
}

inline int GetTiled(int inValue,const int nMax) {
	int nOutValue=(inValue%nMax);

	if (nOutValue<0) {
		nOutValue=((nMax-1)+nOutValue);
	}

	return nOutValue;
}

inline int GetMirrored(int inValue,const int nMax) {

	const int nTwoMax=(nMax*2);

	int nOutValue=GetTiled(inValue,nTwoMax);

	if (nOutValue>=nMax) {
		nOutValue=((nTwoMax-1)-nOutValue);
	}

	return nOutValue;
}

inline float GetRandomFloat(void) {
	return rand()/static_cast<float>(RAND_MAX);
}

static inline int GetSign(int inValue) {

	if (inValue<0) {
		return -1;
	} else {
		return 1;
	}
}

static inline void Get2dTangent(float inX,float inY,float* poutX,float* poutY) {
	*poutX=inY;
	*poutY=-inX;
}

static inline float Dot2d(float Ax,float Ay,float Bx,float By) {
	return ((Ax*Bx)+(Ay*By));
}

static inline void Normalise2d(float* pX,float* pY) {
	const float MagSqr=Dot2d(*pX,*pY,*pX,*pY);
	float Magnitude=(float)sqrt(MagSqr);
	if (Magnitude<=0.0f) {
		Magnitude=0.001f;
	}
	const float RecipMag=1.0f/Magnitude;

	*pX*=RecipMag;
	*pY*=RecipMag;
}

static inline U32 MungeU32(U32 Seed,int nIterations) {

	int nCount;
	for (nCount=0; nCount<nIterations; nCount+=1) {

		Seed+=(Seed^((Seed<<13)|(Seed>>19)));
		Seed+=1;

	}

	return Seed;
	
}

const int cnBiggestSignedInt=0x7fffffff;

const float Pete_Pi=3.141582f;
const float Pete_TwoPi=(2.0f*Pete_Pi);
const float Pete_HalfPi=(0.5f*Pete_Pi);

typedef struct SPete_Parameter {
	char* pName;
	float Default;
	float Minimum;
	float Maximum;
	U32 Type;
} SPete_Parameter;

#define PETE_PARAM_FLOAT		(0)
#define PETE_PARAM_BOOL			(1)
#define PETE_PARAM_INT			(2)
#define PETE_PARAM_POS_X		(3)
#define PETE_PARAM_POS_Y		(4)
#define PETE_PARAM_COLOUR_R		(5)
#define PETE_PARAM_COLOUR_G		(6)
#define PETE_PARAM_COLOUR_B		(7)
#define PETE_PARAM_ANGLE		(8)

#define PETE_VJO_VERSION		(350<<16)

#define PETE_FF_MAJORVERSION	(1)
#define PETE_FF_MINORVERSION	(21)

static inline void Pete_ZeroMemory(void* pMemory,int nCount) {

	char* pCurrent=(char*)pMemory;
	char* pEnd=(pCurrent+nCount);
	while (pCurrent<pEnd) {
		*pCurrent=0;
		pCurrent+=1;
	}
//	memset(pMemory,0,nCount);
}

typedef U32		PETE_PIXELDATA32;
#define SIZEOF_PETE_PIXELDATA32 (4)

typedef U32		PETE_PIXELDATA24;
#define SIZEOF_PETE_PIXELDATA24 (3)

typedef U16		PETE_PIXELDATA16;
#define SIZEOF_PETE_PIXELDATA16 (2)

static inline void Pete_CopyAndConvert24BitTo32Bit(PETE_PIXELDATA24* pSource,PETE_PIXELDATA32* pOutput,int nPixelCount) {

	char* pSourceEnd=((char*)pSource)+(nPixelCount*SIZEOF_PETE_PIXELDATA24);
	char* pCurrentSource=((char*)pSource);
	char* pCurrentOutput=((char*)pOutput);

	while (pCurrentSource<pSourceEnd) {

		*((PETE_PIXELDATA32*)pCurrentOutput)=
		*((PETE_PIXELDATA24*)pCurrentSource);

		pCurrentSource+=SIZEOF_PETE_PIXELDATA24;
		pCurrentOutput+=SIZEOF_PETE_PIXELDATA32;

	}

}

static inline void Pete_CopyAndConvert32BitTo24Bit(PETE_PIXELDATA32* pSource,PETE_PIXELDATA24* pOutput,int nPixelCount) {

	char* pSourceEnd=((char*)pSource)+(nPixelCount*SIZEOF_PETE_PIXELDATA32);
	char* pCurrentSource=((char*)pSource);
	char* pCurrentOutput=((char*)pOutput);

	while (pCurrentSource<pSourceEnd) {

		*((PETE_PIXELDATA24*)pCurrentOutput)=
		*((PETE_PIXELDATA32*)pCurrentSource);

		pCurrentSource+=SIZEOF_PETE_PIXELDATA32;
		pCurrentOutput+=SIZEOF_PETE_PIXELDATA24;

	}

}

static inline void Pete_InPlaceConvert24BitTo32Bit(PETE_PIXELDATA24* pBuffer,int nPixelCount) {

	char* pBufferStart=(char*)pBuffer;
	
	char* pBuffer32Current=(pBufferStart+((nPixelCount-1)*SIZEOF_PETE_PIXELDATA32));
	char* pBuffer24Current=(pBufferStart+((nPixelCount-1)*SIZEOF_PETE_PIXELDATA24));

	while (pBuffer32Current>=pBufferStart) {

		*((PETE_PIXELDATA32*)pBuffer32Current)=
		*((PETE_PIXELDATA24*)pBuffer24Current);

		pBuffer32Current-=SIZEOF_PETE_PIXELDATA32;
		pBuffer24Current-=SIZEOF_PETE_PIXELDATA24;

	}

}

static inline void Pete_CopyAndConvert16Bit565To32Bit(PETE_PIXELDATA16* pSource,PETE_PIXELDATA32* pOutput,int nPixelCount) {

	char* pSourceEnd=((char*)pSource)+(nPixelCount*SIZEOF_PETE_PIXELDATA16);
	char* pCurrentSource=((char*)pSource);
	char* pCurrentOutput=((char*)pOutput);

	while (pCurrentSource<pSourceEnd) {

		PETE_PIXELDATA16 SourceColour=
			*((PETE_PIXELDATA16*)pCurrentSource);

		const int nMaskedRed=(SourceColour>>11)&31;
		const int nMaskedGreen=(SourceColour>>5)&63;
		const int nMaskedBlue=(SourceColour>>0)&31;

		const int nNormalizedRed=(nMaskedRed<<3)|(nMaskedRed>>2);
		const int nNormalizedGreen=(nMaskedGreen<<2)|(nMaskedGreen>>4);
		const int nNormalizedBlue=(nMaskedBlue<<3)|(nMaskedBlue>>2);

		const PETE_PIXELDATA32 OutputColour=
			(nNormalizedRed<<16)|
			(nNormalizedGreen<<8)|
			(nNormalizedBlue<<0);

		*((PETE_PIXELDATA32*)pCurrentOutput)=OutputColour;

		pCurrentSource+=SIZEOF_PETE_PIXELDATA16;
		pCurrentOutput+=SIZEOF_PETE_PIXELDATA32;

	}

}

static inline void Pete_CopyAndConvert32BitTo16Bit565(PETE_PIXELDATA32* pSource,PETE_PIXELDATA16* pOutput,int nPixelCount) {

	char* pSourceEnd=((char*)pSource)+(nPixelCount*SIZEOF_PETE_PIXELDATA32);
	char* pCurrentSource=((char*)pSource);
	char* pCurrentOutput=((char*)pOutput);

	while (pCurrentSource<pSourceEnd) {

		PETE_PIXELDATA32 SourceColour=
			*((PETE_PIXELDATA32*)pCurrentSource);

		const int nSourceRed=(SourceColour>>16)&0xff;
		const int nSourceGreen=(SourceColour>>8)&0xff;
		const int nSourceBlue=(SourceColour>>0)&0xff;

		const int nMaskedRed=(nSourceRed>>3);
		const int nMaskedGreen=(nSourceGreen>>2);
		const int nMaskedBlue=(nSourceBlue>>3);

		PETE_PIXELDATA16 OutputColour=
			(nMaskedRed<<11)|
			(nMaskedGreen<<5)|
			(nMaskedBlue<<0);

		*((PETE_PIXELDATA16*)pCurrentOutput)=OutputColour;

		pCurrentSource+=SIZEOF_PETE_PIXELDATA32;
		pCurrentOutput+=SIZEOF_PETE_PIXELDATA16;

	}

}

inline int GetLuminance(const U32 inColour) {
	const int nRed=(inColour&(0xff<<SHIFT_RED))>>16;
	const int nGreen=(inColour&(0xff<<SHIFT_GREEN))>>8;
	const int nBlue=(inColour&(0xff<<SHIFT_BLUE))>>0;

	const int nLuminance = 
		((90 * nRed)+
		(115 * nGreen)+
		(51 * nBlue));

	return nLuminance;
}

#if defined(PETE_NATIVE_AE)

#include "AE_Effect.h"

typedef PF_Handle SPete_MemHandle;

SPete_MemHandle Pete_NewHandle(int nBytesToAlloc);
void Pete_FreeHandle(SPete_MemHandle InHandle);
void* Pete_LockHandle(SPete_MemHandle InHandle);
void Pete_UnLockHandle(SPete_MemHandle InHandle);

inline int GetLuminance(PF_Pixel Colour) {

	const int nLuminance = 
		((90 * Colour.red)+
		(115 * Colour.green)+
		(51 * Colour.blue));

	return nLuminance;
}

#else // PETE_NATIVE_AE

typedef void* SPete_MemHandle;

inline SPete_MemHandle Pete_NewHandle(int nBytesToAlloc) {
	
	return malloc(nBytesToAlloc);

}
	
inline void Pete_FreeHandle(SPete_MemHandle InHandle) {

	free(InHandle);
	
}

inline void* Pete_LockHandle(SPete_MemHandle InHandle) {

	return InHandle;
	
}

inline void Pete_UnLockHandle(SPete_MemHandle InHandle) {

	// do nothing

}

#endif // PETE_NATIVE_AE

#endif // INCLUDE_PETEHELPERS_H