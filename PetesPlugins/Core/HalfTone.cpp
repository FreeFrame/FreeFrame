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

#include "HalfTone.h"
#include "PeteHelpers.h"

#include <math.h>

#include "ImageMath.h"

static SPete_Parameter g_Parameters[]={
	{
		"Dot Size",
		8.0f,
		1.0f,
		31.0f,
		PETE_PARAM_INT
	},
	{
		"Style",
		0.0f,
		0.0f,
		4.0f,
		PETE_PARAM_INT
	},
	{
		"Angle",
		0.0f,
		0.0f,
		Pete_TwoPi,
		PETE_PARAM_ANGLE
	},
	{
		"Smoothing",
		128.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

const int nMaxCellSize=32;
unsigned char g_pDotFuncTable[nMaxCellSize*nMaxCellSize];
unsigned char g_pGreyScaleTable[512];

enum {
	eRoundStyle,
	eLineStyle,
	eDiamondStyle,
	eEuclideanStyle,
	ePSDiamond,
};

const int nFPShift=16;
const int nFPMult=(1<<nFPShift);

struct SPete_HalfTone_Point {
	int nX;
	int nY;
};

struct SPete_HalfTone_Vertex {
	SPete_HalfTone_Point Pos;
	SPete_HalfTone_Point TexCoords;
};

int Pete_HalfTone_RoundDotFunc(float X,float Y);
int Pete_HalfTone_LineDotFunc(float X,float Y);
int Pete_HalfTone_DiamondDotFunc(float X,float Y);
int Pete_HalfTone_EuclideanDotFunc(float X,float Y);
int Pete_HalfTone_PSDiamondDotFunc(float X,float Y);
void Pete_HalfTone_Rotate(SPete_HalfTone_Point* pinPoint,SPete_HalfTone_Point* poutPoint,float Angle);
void Pete_HalfTone_MakeDotFuncTable(unsigned char* pDotFuncTableStart,int nCellSize,int nStyle);
void Pete_HalfTone_CalcCorners(int nWidth,int nHeight,float AngleRadians,int nCellSize,
	SPete_HalfTone_Point* poutLeft,
	SPete_HalfTone_Point* poutRight,
	SPete_HalfTone_Point* poutTop,
	SPete_HalfTone_Point* poutBottom);
void Pete_HalfTone_GetRasterizationPoints(
	SPete_HalfTone_Point* pinPoints,
	SPete_HalfTone_Point* poutLeft,
	SPete_HalfTone_Point* poutRight,
	SPete_HalfTone_Point* poutTop,
	SPete_HalfTone_Point* poutBottom);
void Pete_HalfTone_HeightSortPoints(SPete_HalfTone_Point* pPoints,int nPointCount);
void Pete_HalfTone_CalcSpanEnds(
	SPete_HalfTone_Point* pinLeft,
	SPete_HalfTone_Point* pinRight,
	SPete_HalfTone_Point* pinTop,
	SPete_HalfTone_Point* pinBottom,
	int nY,int* poutLeftX,int* poutRightX);
void Pete_HalfTone_LerpAlongEdges(
	SPete_HalfTone_Point* pStart,
	SPete_HalfTone_Point* pMiddle,
	SPete_HalfTone_Point* pEnd,
	int nY,int* poutX);
void Pete_HalfTone_RotateMultiple(SPete_HalfTone_Point* pinPoints,
	SPete_HalfTone_Point* poutPoints,
	int nPointCount,
	float Angle);
void Pete_HalfTone_CalcSpanEnds_Vertex(
	SPete_HalfTone_Vertex* pinLeft,
	SPete_HalfTone_Vertex* pinRight,
	SPete_HalfTone_Vertex* pinTop,
	SPete_HalfTone_Vertex* pinBottom,
	int nY,
	SPete_HalfTone_Vertex* poutLeft,
	SPete_HalfTone_Vertex* poutRight);
void Pete_HalfTone_LerpAlongEdges_Vertex(
	SPete_HalfTone_Vertex* pStart,
	SPete_HalfTone_Vertex* pMiddle,
	SPete_HalfTone_Vertex* pEnd,
	int nY,
	SPete_HalfTone_Vertex* poutVertex);
void Pete_HalfTone_GetRasterizationVertices(
	SPete_HalfTone_Vertex* pinVertices,
	SPete_HalfTone_Vertex* poutLeft,
	SPete_HalfTone_Vertex* poutRight,
	SPete_HalfTone_Vertex* poutTop,
	SPete_HalfTone_Vertex* poutBottom);
int Pete_HalfTone_GetLowestVertex(SPete_HalfTone_Vertex* pVertices,int nVertexCount);
void Pete_HalfTone_RotateMultipleVertices(SPete_HalfTone_Vertex* pinVertices,
	SPete_HalfTone_Vertex* poutVertices,
	int nVertexCount,
	float Angle);
void Pete_HalfTone_MakeGreyScaleTable(unsigned char* pGreyScaleTableStart,int nSmoothingThreshold);

void Pete_HalfTone_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_HalfTone_Init(SPete_HalfTone_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_HalfTone_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_HalfTone_DeInit(SPete_HalfTone_Data* pInstanceData) {
	// do nothing
}

void Pete_HalfTone_Render(SPete_HalfTone_Data* pInstanceData,SPete_HalfTone_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	int nCellSize=static_cast<int>(pSettings->m_CellSize);
	nCellSize=GateInt(nCellSize,0,nMaxCellSize);

	int nStyle=static_cast<int>(pSettings->m_Style);
	nStyle=GateInt(nStyle,0,4);

	int nSmoothingThreshold=static_cast<int>(pSettings->m_Smoothing);
	nSmoothingThreshold=GateInt(nSmoothingThreshold,0,255);

	const float AngleRadians=pSettings->m_Angle;

	const int nHalfCellSize=(nCellSize/2);
	const int nCellSizeFP=(nCellSize<<nFPShift);

	const int nHalfWidth=(nWidth/2);
	const int nHalfHeight=(nHeight/2);

	unsigned char* pDotFuncTableStart=&g_pDotFuncTable[0];

	Pete_HalfTone_MakeDotFuncTable(pDotFuncTableStart,nCellSize,nStyle);

	unsigned char* pGreyScaleTableStart=&g_pGreyScaleTable[0];

	Pete_HalfTone_MakeGreyScaleTable(pGreyScaleTableStart,nSmoothingThreshold);

	SPete_HalfTone_Point Left;
	SPete_HalfTone_Point Right;
	SPete_HalfTone_Point Top;
	SPete_HalfTone_Point Bottom;
	Pete_HalfTone_CalcCorners(nWidth,nHeight,AngleRadians,nCellSize,&Left,&Right,&Top,&Bottom);

	U32 TestColour=0x00ffffff;

	int nCurrentV;
	for (nCurrentV=Bottom.nY; nCurrentV<Top.nY; nCurrentV+=nCellSizeFP) {

		int nSnappedV=((nCurrentV+(nCellSizeFP*1024))/nCellSize);
		nSnappedV>>=nFPShift;
		nSnappedV<<=nFPShift;
		nSnappedV*=nCellSize;
		nSnappedV-=(nCellSizeFP*1024);

		int nLeftU;
		int nRightU;
		Pete_HalfTone_CalcSpanEnds(&Left,&Right,&Top,&Bottom,nSnappedV,&nLeftU,&nRightU);

		int nCurrentU;
		for (nCurrentU=nLeftU; nCurrentU<nRightU; nCurrentU+=nCellSizeFP) {

			int nSnappedU=(((nCurrentU+(nCellSizeFP*1024))/nCellSize));
			nSnappedU>>=nFPShift;
			nSnappedU<<=nFPShift;
			nSnappedU*=nCellSize;
			nSnappedU-=(nCellSizeFP*1024);
			
			SPete_HalfTone_Vertex RotatedPoints[4]={
				{{nSnappedU,nSnappedV},								{0,0}},
				{{(nSnappedU+nCellSizeFP),nSnappedV},				{nCellSizeFP-nFPMult,0}},
				{{(nSnappedU+nCellSizeFP),(nSnappedV+nCellSizeFP)},	{nCellSizeFP-nFPMult,nCellSizeFP-nFPMult}},
				{{nSnappedU,(nSnappedV+nCellSizeFP)},				{0,nCellSizeFP-nFPMult}}
			};

			SPete_HalfTone_Vertex ScreenSpacePoints[4];
			Pete_HalfTone_RotateMultipleVertices(
				&RotatedPoints[0],
				&ScreenSpacePoints[0],
				4,
				AngleRadians);

			int nCount;
			for (nCount=0; nCount<4; nCount+=1) {

				ScreenSpacePoints[nCount].Pos.nX+=(nHalfWidth<<nFPShift);
				ScreenSpacePoints[nCount].Pos.nY+=(nHalfHeight<<nFPShift);

				ScreenSpacePoints[nCount].Pos.nX&=0xffff0000;
				ScreenSpacePoints[nCount].Pos.nY&=0xffff0000;

			}

			SPete_HalfTone_Vertex CellLeft;
			SPete_HalfTone_Vertex CellRight;
			SPete_HalfTone_Vertex CellTop;
			SPete_HalfTone_Vertex CellBottom;
			Pete_HalfTone_GetRasterizationVertices(
				&ScreenSpacePoints[0],
				&CellLeft,&CellRight,&CellTop,&CellBottom);

			const int nSampleTopY=(ScreenSpacePoints[0].Pos.nY>>nFPShift);
			const int nSampleBottomY=(nSampleTopY+nCellSize);
			const int nSampleCenterY=(nSampleTopY+nHalfCellSize);

			const int nSampleLeftX=(ScreenSpacePoints[0].Pos.nX>>nFPShift);
			const int nSampleRightX=(nSampleLeftX+nCellSize);
			const int nSampleCenterX=(nSampleLeftX+nHalfCellSize);

			const U32 AverageColour=
				Pete_GetImageAreaAverage(
					nSampleLeftX,nSampleTopY,
					nCellSize,nCellSize,
					pSource,nWidth,nHeight);

			int nLuminance=GetLuminance(AverageColour)/256;
			nLuminance+=256;

			int nCurrentYFP;
			for (nCurrentYFP=CellBottom.Pos.nY; nCurrentYFP<=CellTop.Pos.nY; nCurrentYFP+=nFPMult) {

				if (nCurrentYFP<0) {
					continue;
				}

				if (nCurrentYFP>=(nHeight<<nFPShift)) {
					break;
				}

				SPete_HalfTone_Vertex SpanStart;
				SPete_HalfTone_Vertex SpanEnd;
				Pete_HalfTone_CalcSpanEnds_Vertex(
					&CellLeft,&CellRight,&CellTop,&CellBottom,nCurrentYFP,
					&SpanStart,&SpanEnd);

				const int nCellLeftX=(SpanStart.Pos.nX>>nFPShift);
				const int nCellRightX=(SpanEnd.Pos.nX>>nFPShift);

				const int nCurrentY=(nCurrentYFP>>nFPShift);

				int nLengthX=(nCellRightX-nCellLeftX);
				if (nLengthX<1) {
					nLengthX=1;
				}

				const int nGradientU=
					(SpanEnd.TexCoords.nX-SpanStart.TexCoords.nX)/nLengthX;
				const int nGradientV=
					(SpanEnd.TexCoords.nY-SpanStart.TexCoords.nY)/nLengthX;

				int nTexU=SpanStart.TexCoords.nX;
				int nTexV=SpanStart.TexCoords.nY;

				int nCurrentX;
				for (nCurrentX=nCellLeftX; nCurrentX<nCellRightX;
					nCurrentX+=1,nTexU+=nGradientU,nTexV+=nGradientV) {

					if (nCurrentX<0) {
						continue;
					}

					if (nCurrentX>=nWidth) {
						break;
					}

					const int nTexUInt=(nTexU>>nFPShift);
					const int nTexVInt=(nTexV>>nFPShift);

					unsigned char* pCurrentDotFunc=
						pDotFuncTableStart+
						(nTexVInt*nCellSize)+
						nTexUInt;

					const int nDotFuncResult=*pCurrentDotFunc;

					const int nDiff=nLuminance-nDotFuncResult;

					const int nGreyValue=pGreyScaleTableStart[nDiff];

//					if (nDotFuncResult>=nLuminance) {
//
//						nGreyValue=0;
//
//					} else {
//
//						const int nDiff=(nLuminance-nDotFuncResult);
//						
//						if (nDiff>nSmoothingThreshold) {
//							nGreyValue=255;
//						} else {
//							nGreyValue=(nDiff*255)/nSmoothingThreshold;
//						}
//						
//					}

//					if ((nTexUInt>=nCellSize)||
//						(nTexVInt>=nCellSize)) {
//						nGreyValue=0x0000ff00;
//					} else if ((nTexUInt<0)||
//						(nTexVInt<0)) {
//						nGreyValue=0x000000ff;
//					} else {
//						nGreyValue=0x00ffffff;
//					}

//					nDotFuncResult=((nTexUInt^nTexVInt)&0x4)*63;

					const U32 OutputColour=
						(nGreyValue<<SHIFT_RED)|
						(nGreyValue<<SHIFT_GREEN)|
						(nGreyValue<<SHIFT_BLUE);
//						(nDotFuncResult<<SHIFT_RED)|
//						(nDotFuncResult<<SHIFT_GREEN)|
//						(nDotFuncResult<<SHIFT_BLUE);

					U32* pCurrentOutput=
						pOutput+(nCurrentY*nWidth)+nCurrentX;

					*pCurrentOutput=OutputColour;

//					*pCurrentOutput=TestColour;

				}


			}

//			TestColour^=0x00ff00ff;
		
		}

	}

}

int Pete_HalfTone_RoundDotFunc(float X,float Y) {

	const float XSquared=(X*X);
	const float YSquared=(Y*Y);

	const float Result=(2.0f-(XSquared+YSquared))/2.0f;

	return static_cast<int>(Result*255.0f);

}

int Pete_HalfTone_LineDotFunc(float X,float Y) {

	const float Result=(1.0f-fabsf(Y));

	return static_cast<int>(Result*255.0f);

}

int Pete_HalfTone_DiamondDotFunc(float X,float Y) {

	const float Result=(2.0f-(fabsf(X)+fabsf(Y)))/2.0f;

	return static_cast<int>(Result*255.0f);

}

int Pete_HalfTone_EuclideanDotFunc(float X,float Y) {

	const float AbsX=fabsf(X);
	const float AbsY=fabsf(Y);
	
	float Result;

	if ((AbsX+AbsY)>1.0f) {

		Result=((AbsY-1.0f)*(AbsY-1.0f)+
		(AbsX-1.0f)*(AbsX-1.0f))-2.0f;

	} else {

		Result=2.0f-(AbsY*AbsY+AbsX*AbsX);

	}

	Result/=2.0f;

	return static_cast<int>(Result*255.0f);

}

int Pete_HalfTone_PSDiamondDotFunc(float X,float Y) {

	const float AbsX=fabsf(X);
	const float AbsY=fabsf(Y);
	
	float Result;

	if ((AbsX+AbsY)<=1.5f) {
		
		Result=2.0f-(AbsX*AbsX+AbsY*AbsY); 

	} else if ((AbsX+AbsY)<=1.23f) { 

		Result=2.0f-((AbsY*0.76f)+AbsX);

	} else {

		Result=((AbsY-1.0f)*(AbsY-1.0f)+
			(AbsX-1.0f)*(AbsX-1.0f))-2.0f;	
	
	}

	Result/=2.0f;

	return static_cast<int>(Result*255.0f);

}

void Pete_HalfTone_Rotate(SPete_HalfTone_Point* pinPoint,SPete_HalfTone_Point* poutPoint,float Angle) {

	const int CosFP=static_cast<int>(cos(Angle)*nFPMult);
	const int SinFP=static_cast<int>(sin(Angle)*nFPMult);

	poutPoint->nX=
		(CosFP*(pinPoint->nX>>nFPShift))+
		(SinFP*(pinPoint->nY>>nFPShift));

	poutPoint->nY=
		(CosFP*(pinPoint->nY>>nFPShift))-
		(SinFP*(pinPoint->nX>>nFPShift));

}

void Pete_HalfTone_MakeDotFuncTable(unsigned char* pDotFuncTableStart,int nCellSize,int nStyle) {

	const int nHalfCellSize=(nCellSize/2);

	unsigned char* pCurrentDotFunc=pDotFuncTableStart;

	int nCurrentY;
	for (nCurrentY=0; nCurrentY<nCellSize; nCurrentY+=1) {

		float NormalY=(nCurrentY-nHalfCellSize)/(nCellSize*0.5f);

		int nCurrentX;
		for (nCurrentX=0; nCurrentX<nCellSize; nCurrentX+=1,pCurrentDotFunc+=1) {

			float NormalX=(nCurrentX-nHalfCellSize)/(nCellSize*0.5f);

			int nDotFuncResult;
			switch (nStyle) {

				case eRoundStyle: {
					nDotFuncResult=Pete_HalfTone_RoundDotFunc(NormalX,NormalY);
				}break;

				case eLineStyle: {
					nDotFuncResult=Pete_HalfTone_LineDotFunc(NormalX,NormalY);
				}break;

				case eDiamondStyle: {
					nDotFuncResult=Pete_HalfTone_DiamondDotFunc(NormalX,NormalY);
				}break;

				case eEuclideanStyle: {
					nDotFuncResult=Pete_HalfTone_EuclideanDotFunc(NormalX,NormalY);
				}break;

				case ePSDiamond: {
					nDotFuncResult=Pete_HalfTone_PSDiamondDotFunc(NormalX,NormalY);
				}break;

				default: {
					assert(false);
					nDotFuncResult=255;
				}break;

			}

			*pCurrentDotFunc=nDotFuncResult;

		}


	}

}

void Pete_HalfTone_CalcCorners(int nWidth,int nHeight,float AngleRadians,int nCellSize,
	SPete_HalfTone_Point* poutLeft,
	SPete_HalfTone_Point* poutRight,
	SPete_HalfTone_Point* poutTop,
	SPete_HalfTone_Point* poutBottom) {

	const int nHalfWidth=(nWidth/2);
	const int nHalfHeight=(nHeight/2);

	const int nHalfWidthFP=(nHalfWidth<<nFPShift);
	const int nHalfHeightFP=(nHalfHeight<<nFPShift);

	const int nCellSizeFP=(nCellSize<<nFPShift);

	const int nXExtentFP=nHalfWidthFP+(2*nCellSizeFP);
	const int nYExtentFP=nHalfHeightFP+(2*nCellSizeFP);

	SPete_HalfTone_Point OriginalCorners[4]={
		{-nXExtentFP,-nYExtentFP},
		{nXExtentFP,-nYExtentFP},
		{-nXExtentFP,nYExtentFP},
		{nXExtentFP,nYExtentFP},
	};
	SPete_HalfTone_Point RotatedCorners[4];

	int nCount;
	for (nCount=0; nCount<4; nCount+=1) {
		Pete_HalfTone_Rotate(
			&OriginalCorners[nCount],
			&RotatedCorners[nCount],
			-AngleRadians);
	}

	Pete_HalfTone_GetRasterizationPoints(
		&RotatedCorners[0],
		poutLeft,
		poutRight,
		poutTop,
		poutBottom);

}

void Pete_HalfTone_GetRasterizationPoints(
	SPete_HalfTone_Point* pinPoints,
	SPete_HalfTone_Point* poutLeft,
	SPete_HalfTone_Point* poutRight,
	SPete_HalfTone_Point* poutTop,
	SPete_HalfTone_Point* poutBottom) {

	Pete_HalfTone_HeightSortPoints(pinPoints,4);

	*poutBottom=pinPoints[0];
	*poutTop=pinPoints[3];

	if (pinPoints[1].nX<pinPoints[2].nX) {
		*poutLeft=pinPoints[1];
		*poutRight=pinPoints[2];
	} else {
		*poutLeft=pinPoints[2];
		*poutRight=pinPoints[1];
	}

}

void Pete_HalfTone_HeightSortPoints(SPete_HalfTone_Point* pPoints,int nPointCount) {

	int nCount;
	for (nCount=0; nCount<nPointCount; nCount+=1) {

		int nLowestIndex=nCount;

		int nIndex;
		for (nIndex=(nCount+1); nIndex<nPointCount; nIndex+=1) {

			if (pPoints[nIndex].nY<
				pPoints[nLowestIndex].nY) {

				nLowestIndex=nIndex;

			} else if (pPoints[nIndex].nY==
				pPoints[nLowestIndex].nY) {

				if (pPoints[nIndex].nX<
					pPoints[nLowestIndex].nX) {

					nLowestIndex=nIndex;

				}

			}

		}

		SPete_HalfTone_Point SwapTemp;
		SwapTemp=pPoints[nCount];
		pPoints[nCount]=pPoints[nLowestIndex];
		pPoints[nLowestIndex]=SwapTemp;

	}

}

void Pete_HalfTone_CalcSpanEnds(
	SPete_HalfTone_Point* pinLeft,
	SPete_HalfTone_Point* pinRight,
	SPete_HalfTone_Point* pinTop,
	SPete_HalfTone_Point* pinBottom,
	int nY,int* poutLeftX,int* poutRightX) {

	Pete_HalfTone_LerpAlongEdges(pinBottom,pinLeft,pinTop,nY,poutLeftX);

	Pete_HalfTone_LerpAlongEdges(pinBottom,pinRight,pinTop,nY,poutRightX);

}

void Pete_HalfTone_LerpAlongEdges(
	SPete_HalfTone_Point* pStart,
	SPete_HalfTone_Point* pMiddle,
	SPete_HalfTone_Point* pEnd,
	int nY,int* poutX) {
	
	if (nY<pMiddle->nY) {

		const int nYDist=pMiddle->nY-pStart->nY;

		if (nYDist<nFPMult) {

			*poutX=pStart->nX;
		
		} else {

			const int nOneMinusLerpValue=
				(nY-pStart->nY)/(nYDist>>nFPShift);
			const int nLerpValue=nFPMult-nOneMinusLerpValue;

			*poutX=
				(nLerpValue*(pStart->nX>>nFPShift))+
				(nOneMinusLerpValue*(pMiddle->nX>>nFPShift));

		}

	} else {

		const int nYDist=pEnd->nY-pMiddle->nY;

		if (nYDist<nFPMult) {

			*poutX=pMiddle->nX;

		} else {

			const int nOneMinusLerpValue=
				(nY-pMiddle->nY)/(nYDist>>nFPShift);
			const int nLerpValue=nFPMult-nOneMinusLerpValue;

			*poutX=
				(nLerpValue*(pMiddle->nX>>nFPShift))+
				(nOneMinusLerpValue*(pEnd->nX>>nFPShift));

		}

	}

}

void Pete_HalfTone_RotateMultiple(SPete_HalfTone_Point* pinPoints,
	SPete_HalfTone_Point* poutPoints,
	int nPointCount,
	float Angle) {

	const int CosFP=static_cast<int>(cos(Angle)*nFPMult);
	const int SinFP=static_cast<int>(sin(Angle)*nFPMult);

	int nCount;
	for (nCount=0; nCount<nPointCount; nCount+=1) {

		poutPoints[nCount].nX=
			(CosFP*(pinPoints[nCount].nX>>nFPShift))+
			(SinFP*(pinPoints[nCount].nY>>nFPShift));

		poutPoints[nCount].nY=
			(CosFP*(pinPoints[nCount].nY>>nFPShift))-
			(SinFP*(pinPoints[nCount].nX>>nFPShift));

	}

}

void Pete_HalfTone_CalcSpanEnds_Vertex(
	SPete_HalfTone_Vertex* pinLeft,
	SPete_HalfTone_Vertex* pinRight,
	SPete_HalfTone_Vertex* pinTop,
	SPete_HalfTone_Vertex* pinBottom,
	int nY,
	SPete_HalfTone_Vertex* poutLeft,
	SPete_HalfTone_Vertex* poutRight) {

	Pete_HalfTone_LerpAlongEdges_Vertex(pinBottom,pinLeft,pinTop,nY,poutLeft);

	Pete_HalfTone_LerpAlongEdges_Vertex(pinBottom,pinRight,pinTop,nY,poutRight);

}

void Pete_HalfTone_LerpAlongEdges_Vertex(
	SPete_HalfTone_Vertex* pStart,
	SPete_HalfTone_Vertex* pMiddle,
	SPete_HalfTone_Vertex* pEnd,
	int nY,
	SPete_HalfTone_Vertex* poutVertex) {
	
	const int nMiddleY=pMiddle->Pos.nY;

	if (nY<nMiddleY) {

		const int nStartX=pStart->Pos.nX;
		const int nStartY=pStart->Pos.nY;
		const int nStartU=pStart->TexCoords.nX;
		const int nStartV=pStart->TexCoords.nY;

		const int nYDist=nMiddleY-nStartY;

		if (nYDist<nFPMult) {

			poutVertex->Pos.nX=nStartX;
			poutVertex->TexCoords.nX=nStartU;
			poutVertex->TexCoords.nY=nStartV;
			
		} else {

			const int nMiddleX=pMiddle->Pos.nX;
			const int nMiddleU=pMiddle->TexCoords.nX;
			const int nMiddleV=pMiddle->TexCoords.nY;

			const int nOneMinusLerpValue=
				(nY-nStartY)/(nYDist>>nFPShift);
			const int nLerpValue=nFPMult-nOneMinusLerpValue;

			poutVertex->Pos.nX=
				(nLerpValue*(nStartX>>nFPShift))+
				(nOneMinusLerpValue*(nMiddleX>>nFPShift));

			poutVertex->TexCoords.nX=
				(nLerpValue*(nStartU>>nFPShift))+
				(nOneMinusLerpValue*(nMiddleU>>nFPShift));

			poutVertex->TexCoords.nY=
				(nLerpValue*(nStartV>>nFPShift))+
				(nOneMinusLerpValue*(nMiddleV>>nFPShift));
		}

	} else {

		const int nEndY=pEnd->Pos.nY;

		const int nYDist=nEndY-nMiddleY;

		const int nMiddleX=pMiddle->Pos.nX;
		const int nMiddleU=pMiddle->TexCoords.nX;
		const int nMiddleV=pMiddle->TexCoords.nY;

		if (nYDist<nFPMult) {

			poutVertex->Pos.nX=nMiddleX;
			poutVertex->TexCoords.nX=nMiddleU;
			poutVertex->TexCoords.nY=nMiddleV;

		} else {

			const int nEndX=pEnd->Pos.nX;
			const int nEndU=pEnd->TexCoords.nX;
			const int nEndV=pEnd->TexCoords.nY;

			const int nOneMinusLerpValue=
				(nY-nMiddleY)/(nYDist>>nFPShift);
			const int nLerpValue=nFPMult-nOneMinusLerpValue;

			poutVertex->Pos.nX=
				(nLerpValue*(nMiddleX>>nFPShift))+
				(nOneMinusLerpValue*(nEndX>>nFPShift));

			poutVertex->TexCoords.nX=
				(nLerpValue*(nMiddleU>>nFPShift))+
				(nOneMinusLerpValue*(nEndU>>nFPShift));

			poutVertex->TexCoords.nY=
				(nLerpValue*(nMiddleV>>nFPShift))+
				(nOneMinusLerpValue*(nEndV>>nFPShift));

		}

	}

	poutVertex->Pos.nY=nY;

}

void Pete_HalfTone_GetRasterizationVertices(
	SPete_HalfTone_Vertex* pinVertices,
	SPete_HalfTone_Vertex* poutLeft,
	SPete_HalfTone_Vertex* poutRight,
	SPete_HalfTone_Vertex* poutTop,
	SPete_HalfTone_Vertex* poutBottom) {

	const int nLowestVertex=Pete_HalfTone_GetLowestVertex(pinVertices,4);

	*poutBottom=pinVertices[(nLowestVertex+0)%4];
	*poutTop=pinVertices[(nLowestVertex+2)%4];

//	if (pinVertices[1].Pos.nX<pinVertices[2].Pos.nX) {
	*poutLeft=pinVertices[(nLowestVertex+3)%4];
	*poutRight=pinVertices[(nLowestVertex+1)%4];
//	} else {
//		*poutLeft=pinVertices[2];
//		*poutRight=pinVertices[1];
//	}

//	int DirDot=
//		(poutTop->Pos.nX-poutLeft->Pos.nX)*
//		(poutBottom->Pos.nX-poutRight->Pos.nX);
//
//	
//	if ((poutLeft->Pos.nX==poutRight->Pos.nX)||(DirDot>0)) {
//
//		SPete_HalfTone_Vertex SwapTemp;
//
//		if (poutLeft->Pos.nX>poutTop->Pos.nX) {
//
//			SwapTemp=*poutLeft;
//			*poutLeft=*poutTop;
//			*poutTop=SwapTemp;
//
//		} else {
//
//			SwapTemp=*poutRight;
//			*poutRight=*poutBottom;
//			*poutBottom=SwapTemp;
//
//		}
//
//		poutTop->TexCoords.nX=0x00080000;
//		poutTop->TexCoords.nY=0x00080000;
//
//		poutBottom->TexCoords.nX=0x00080000;
//		poutBottom->TexCoords.nY=0x00080000;
//
//		poutLeft->TexCoords.nX=0x00080000;
//		poutLeft->TexCoords.nY=0x00080000;
//
//		poutRight->TexCoords.nX=0x00080000;
//		poutRight->TexCoords.nY=0x00080000;
//
//	}

}

int Pete_HalfTone_GetLowestVertex(SPete_HalfTone_Vertex* pVertices,int nVertexCount) {

//	int nCount;
//	for (nCount=0; nCount<nVertexCount; nCount+=1) {

	int nLowestIndex=0;

	int nIndex;
	for (nIndex=1; nIndex<nVertexCount; nIndex+=1) {

		if (pVertices[nIndex].Pos.nY<
			pVertices[nLowestIndex].Pos.nY) {

			nLowestIndex=nIndex;

		} else if (pVertices[nIndex].Pos.nY==
			pVertices[nLowestIndex].Pos.nY) {

			if (pVertices[nIndex].Pos.nX<
				pVertices[nLowestIndex].Pos.nX) {

				nLowestIndex=nIndex;

			}

		}

	}

//		SPete_HalfTone_Vertex SwapTemp;
//		SwapTemp=pVertices[nCount];
//		pVertices[nCount]=pVertices[nLowestIndex];
//		pVertices[nLowestIndex]=SwapTemp;
//
//	}

	return nLowestIndex;

}

void Pete_HalfTone_RotateMultipleVertices(SPete_HalfTone_Vertex* pinVertices,
	SPete_HalfTone_Vertex* poutVertices,
	int nVertexCount,
	float Angle) {

	const int CosFP=static_cast<int>(cos(Angle)*nFPMult);
	const int SinFP=static_cast<int>(sin(Angle)*nFPMult);

	int nCount;
	for (nCount=0; nCount<nVertexCount; nCount+=1) {

		poutVertices[nCount].Pos.nX=
			(CosFP*(pinVertices[nCount].Pos.nX>>nFPShift))+
			(SinFP*(pinVertices[nCount].Pos.nY>>nFPShift));

		poutVertices[nCount].Pos.nY=
			(CosFP*(pinVertices[nCount].Pos.nY>>nFPShift))-
			(SinFP*(pinVertices[nCount].Pos.nX>>nFPShift));

		poutVertices[nCount].TexCoords=pinVertices[nCount].TexCoords;

	}

}

void Pete_HalfTone_MakeGreyScaleTable(unsigned char* pGreyScaleTableStart,int nSmoothingThreshold) {

	if (nSmoothingThreshold<=0) {
		nSmoothingThreshold=1;
	}

	int nCount;
	for (nCount=0; nCount<512; nCount+=1) {

		const int nDiff=nCount-256;

		int nGreyValue;
		if (nDiff<0) {

			nGreyValue=0;

		} else {

			if (nDiff>nSmoothingThreshold) {
				nGreyValue=255;
			} else {
				nGreyValue=(nDiff*255)/nSmoothingThreshold;
			}
			
		}

		pGreyScaleTableStart[nCount]=nGreyValue;

	}

}
