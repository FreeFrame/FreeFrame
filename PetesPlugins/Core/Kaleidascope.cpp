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

#include "Kaleidascope.h"

#include "PeteHelpers.h"
#include "Pete2dMatrix.h"
#include "SimpleMirror.h"

#define PETE_KALEIDASCOPE_HALFLINE_BIT		(1<<0)

const int nCosTableSizeShift=10;
const int nCosTableSize=(1<<nCosTableSizeShift);

const int nFixedShift=16;
const int nFixedMult=(1<<nFixedShift);
const int nFixedMask=(nFixedMult-1);

const int nDistanceShift=6;
const int nDistanceMult=(1<<nDistanceShift);

const float FixedAngleMult=(nFixedMult/Pete_TwoPi);

const int nHalfPiFA=(nFixedMult/4);

const int nFAToCosTableShift=(nFixedShift-nCosTableSizeShift);

const float Pete_Kaleidascope_Epsilon=0.0001f;

const int nMaxDivisions=64;

struct SPete_Kaleidascope_PartitionData {
	SPete_Kaleidascope_Line* pYNegLines;
	int	nYNegLinesCount;
	SPete_Kaleidascope_Line* pYPosLines;
	int nYPosLinesCount;
};

void Pete_Kaleidascope_SetupAngleTable(SPete_Kaleidascope_Data* pInstanceData);
void Pete_Kaleidascope_SetupCosTable(SPete_Kaleidascope_Data* pInstanceData);
void Pete_Kaleidascope_SetupLines(SPete_Kaleidascope_Data* pInstanceData,SPete_Kaleidascope_Settings* pSettings,int* poutLinesCount);
void Pete_Kaleidascope_PartitionLines(SPete_Kaleidascope_Line* pLinesStart,int nLinesCount,SPete_Kaleidascope_PartitionData* poutPartitionData);
void Pete_Kaleidascope_CreateAllTransforms(SPete_Kaleidascope_Data* pInstanceData,SPete_Kaleidascope_Settings* pSettings,SPete_2dMatrix* pTransforms);
void Pete_Kaleidascope_Render_Old(SPete_Kaleidascope_Data* pInstanceData,SPete_Kaleidascope_Settings* pSettings,U32* pSource,U32* pOutput);
extern "C" int Pete_Kaleidascope_LinesSortFunction(const void* pElem1,const void* pElem2);
void Pete_Kaleidascope_Dev(SPete_Kaleidascope_Data* pInstanceData,SPete_Kaleidascope_Settings* pSettings,U32* pSource,U32* pOutput);
void Pete_Kaleidascope_RenderAE(SPete_Kaleidascope_Data* pInstanceData,SPete_Kaleidascope_Settings* pSettings,U32* pSource,U32* pOutput);

static int* g_pCurrentCosTable; // Pete- Hack to avoid accessing this table via 2 indirections
// (eg pInstanceData->hCosTable==2 memory accesses)

const int nMaxLines=128;

static SPete_Parameter g_Parameters[]={
	{
		"Divisions",
		7.0f,
		0.0f,
		64.0f,
		PETE_PARAM_INT
	},
	{
		"Output Angle",
		0.0f,
		0.0f,
		Pete_TwoPi,
		PETE_PARAM_ANGLE
	},
	{
		"Source Angle",
		0.0f,
		0.0f,
		Pete_TwoPi,
		PETE_PARAM_ANGLE
	},
	{
		"Source Centre X",
		0.5f,
		0.0f,
		1.0f,
		PETE_PARAM_POS_X
	},
	{
		"Source Centre Y",
		0.5f,
		0.0f,
		1.0f,
		PETE_PARAM_POS_Y
	},
	{
		"Output Centre X",
		0.5f,
		0.0f,
		1.0f,
		PETE_PARAM_POS_X
	},
	{
		"Output Centre Y",
		0.5f,
		0.0f,
		1.0f,
		PETE_PARAM_POS_Y
	},
	{
		"Reflection Line Proportion",
		0.5f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Source Angle Proportion",
		1.0f,
		0.1f,
		10.0f,
		PETE_PARAM_FLOAT
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_Kaleidascope_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

inline int Pete_Kaleidascope_CosFA(int nAngleFA) {

	const int nGatedAngleFA=nAngleFA&nFixedMask;

	const int nEntryIndex=(nGatedAngleFA>>nFAToCosTableShift);

	return g_pCurrentCosTable[nEntryIndex];
}

inline int Pete_Kaleidascope_SinFA(int nAngleFA) {

	const int nIncrementedAngleFA=(nAngleFA+nHalfPiFA);

	const int nGatedAngleFA=nIncrementedAngleFA&nFixedMask;

	const int nEntryIndex=(nGatedAngleFA>>nFAToCosTableShift);

	return g_pCurrentCosTable[nEntryIndex];
}

int Pete_Kaleidascope_Init(SPete_Kaleidascope_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_Kaleidascope_DeInit(pInstanceData);

	const int nNumberOfPixels=nWidth*nHeight;
	const int nNumberOfBytes=
		nNumberOfPixels*sizeof(SPete_AngleTable_Entry);
	pInstanceData->hAngleTable=
		Pete_NewHandle(nNumberOfBytes);

	if (pInstanceData->hAngleTable==NULL) {
		Pete_Kaleidascope_DeInit(pInstanceData);
		return 0;
	}

	const int nCosTableByteCount=nCosTableSize*sizeof(int);
	pInstanceData->hCosTable=
		Pete_NewHandle(nCosTableByteCount);

	if (pInstanceData->hCosTable==NULL) {
		Pete_Kaleidascope_DeInit(pInstanceData);
		return 0;
	}

	const int nLinesByteCount=nMaxLines*sizeof(SPete_Kaleidascope_Line);
	pInstanceData->hLines=
		Pete_NewHandle(nLinesByteCount);
	pInstanceData->nMaxLines=nMaxLines;

	if (pInstanceData->hLines==NULL) {
		Pete_Kaleidascope_DeInit(pInstanceData);
		return 0;
	}

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	Pete_Kaleidascope_SetupAngleTable(pInstanceData);
	Pete_Kaleidascope_SetupCosTable(pInstanceData);

	return 1;

}

void Pete_Kaleidascope_DeInit(SPete_Kaleidascope_Data* pInstanceData) {

	if (pInstanceData->hAngleTable!=NULL) {
		Pete_FreeHandle(pInstanceData->hAngleTable);
		pInstanceData->hAngleTable=NULL;
	}

	if (pInstanceData->hCosTable!=NULL) {
		Pete_FreeHandle(pInstanceData->hCosTable);
		pInstanceData->hCosTable=NULL;
	}

	if (pInstanceData->hLines!=NULL) {
		Pete_FreeHandle(pInstanceData->hLines);
		pInstanceData->hLines=NULL;
	}

}

void Pete_Kaleidascope_Render_Old(SPete_Kaleidascope_Data* pInstanceData,SPete_Kaleidascope_Settings* pSettings,U32* pSource,U32* pOutput) {

	g_pCurrentCosTable=(int*)Pete_LockHandle(pInstanceData->hCosTable);
	if (g_pCurrentCosTable==NULL) {
		return;
	}

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;
	const float Width=(float)(nWidth);
	const float HalfWidth=(Width/2.0f);
	const float Height=(float)(nHeight);
	const float HalfHeight=(Height/2.0f);

	const int nHalfWidthFP=static_cast<int>(HalfWidth*nFixedMult*nDistanceMult);
	const int nHalfHeightFP=static_cast<int>(HalfWidth*nFixedMult*nDistanceMult);

	const int nNumPixels = nWidth*nHeight;

	float Divisions=ceilf(pSettings->m_Divisions);
	if (Divisions<1.0f) {
		Divisions=1.0f;
	}

	const float AnglePreIncrement=pSettings->m_OutputAnglePreIncrement;
	const int nAnglePreIncrementFA=static_cast<int>(AnglePreIncrement*FixedAngleMult);

	const float AngleInterval=(Pete_TwoPi/Divisions);
	const int nAngleIntervalFA=static_cast<int>(AngleInterval*FixedAngleMult);
	const float HalfAngleInterval=(AngleInterval/2.0f);
	const int nHalfAngleIntervalFA=static_cast<int>(HalfAngleInterval*FixedAngleMult);

	SPete_AngleTable_Entry* pTableStart=(SPete_AngleTable_Entry*)Pete_LockHandle(pInstanceData->hAngleTable);
	if (pTableStart==NULL) {
		return;
	}

	SPete_AngleTable_Entry* pTableCurrent=pTableStart;

	U32*const pSourceStart=pSource;
	U32* pCurrentOutput=pOutput;
	const U32* pOutputEnd=(pOutput+nNumPixels);

	while (pCurrentOutput!=pOutputEnd) {

		const U32* pOutputLineEnd=pCurrentOutput+nWidth;

		while (pCurrentOutput!=pOutputLineEnd) {

			int nAngleFA=pTableCurrent->nAngleFA;
			int nDist=pTableCurrent->nDist;

			nAngleFA+=nAnglePreIncrementFA;

			int nMirroredAngleFA=(nAngleFA%nAngleIntervalFA);
			if (nMirroredAngleFA>nHalfAngleIntervalFA) {
				nMirroredAngleFA=(nAngleIntervalFA-nMirroredAngleFA);
			}

			nMirroredAngleFA-=nAnglePreIncrementFA;

			int nResultXFP=Pete_Kaleidascope_CosFA(nMirroredAngleFA);
			int nResultYFP=Pete_Kaleidascope_SinFA(nMirroredAngleFA);

			nResultXFP*=nDist;
			nResultYFP*=nDist;

			nResultXFP+=nHalfWidthFP;
			nResultYFP+=nHalfHeightFP;

			int nSourceX=(nResultXFP>>(nFixedShift+nDistanceShift));
			int nSourceY=(nResultYFP>>(nFixedShift+nDistanceShift));

			nSourceX=GetMirrored(nSourceX,nWidth);
			nSourceY=GetMirrored(nSourceY,nHeight);

			U32* pCurrentSource=
				pSourceStart+
				(nSourceY*nWidth)+
				nSourceX;

			*pCurrentOutput=*pCurrentSource;

			pCurrentOutput+=1;
			pTableCurrent+=1;

		}

	}

}

void Pete_Kaleidascope_SetupAngleTable(SPete_Kaleidascope_Data* pInstanceData) {

	SPete_AngleTable_Entry* pTableStart=(SPete_AngleTable_Entry*)Pete_LockHandle(pInstanceData->hAngleTable);
	if (pTableStart==NULL) {
		return;
	}

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const float Width=(float)(nWidth);
	const float HalfWidth=(Width/2.0f);
	const float Height=(float)(nHeight);
	const float HalfHeight=(Height/2.0f);

	const int nNumPixels=(nWidth*nHeight);

	SPete_AngleTable_Entry* pCurrent=pTableStart;
	const SPete_AngleTable_Entry* pTableEnd=(pTableStart+nNumPixels);

	float YPos=-HalfHeight;
	while (pCurrent!=pTableEnd) {

		const SPete_AngleTable_Entry* pLineEnd=pCurrent+nWidth;

		float XPos=-HalfWidth;
		while (pCurrent!=pLineEnd) {

			float Angle=atan2(YPos,XPos);
			if (Angle<0.0f) {
				Angle+=Pete_TwoPi;
			}
			float Dist=sqrt((XPos*XPos)+(YPos*YPos));

			pCurrent->nAngleFA=static_cast<int>(Angle*FixedAngleMult);
			pCurrent->nDist=static_cast<int>(Dist*nDistanceMult);

			XPos+=1.0f;
			pCurrent+=1;

		}

		YPos+=1.0f;

	}

}

void Pete_Kaleidascope_SetupCosTable(SPete_Kaleidascope_Data* pInstanceData) {

	int* pTableStart=(int*)Pete_LockHandle(pInstanceData->hCosTable);
	if (pTableStart==NULL) {
		return;
	}

	int nCount;
	for (nCount=0; nCount<nCosTableSize; nCount+=1) {

		float Angle=(nCount/(float)(nCosTableSize))*Pete_TwoPi;

		float Result=cos(Angle);

		pTableStart[nCount]=static_cast<int>(Result*nFixedMult);

	}

}

void Pete_Kaleidascope_Render(SPete_Kaleidascope_Data* pInstanceData,SPete_Kaleidascope_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	if (pSettings->m_Divisions<1.0f) {

		const int nByteCount=(nWidth*nHeight*sizeof(U32));
		memcpy(pOutput,pSource,nByteCount);

		return;

	} else if (pSettings->m_Divisions<2.0f) {

		SPete_SimpleMirror_Data SimpleMirrorData;
		SimpleMirrorData.nWidth=nWidth;
		SimpleMirrorData.nHeight=nHeight;

		SPete_SimpleMirror_Settings SimpleMirrorSettings;
		SimpleMirrorSettings.m_Angle=(pSettings->m_OutputAnglePreIncrement/Pete_TwoPi)*360.0f;
		SimpleMirrorSettings.m_DoSimpleMirrorAll=0.0f;
		SimpleMirrorSettings.m_PlaneD=0.0f;

		Pete_SimpleMirror_Render(&SimpleMirrorData,&SimpleMirrorSettings,pSource,pOutput);

		return;

	}

	int nLinesCount;
	Pete_Kaleidascope_SetupLines(pInstanceData,pSettings,&nLinesCount);

	SPete_Kaleidascope_Line* pLinesStart=(SPete_Kaleidascope_Line*)Pete_LockHandle(pInstanceData->hLines);
	if (pLinesStart==NULL) {
		return;
	}

	const float Width=(float)(nWidth);
	const float HalfWidth=(Width/2.0f);
	const float Height=(float)(nHeight);
	const float HalfHeight=(Height/2.0f);

	const float SourceStartAngle=pSettings->m_SourceAnglePreIncrement;
	float SourceHalfAngle=
		(Pete_TwoPi/(ceilf(pSettings->m_Divisions)*2.0f));
	SourceHalfAngle*=pSettings->m_SourceAngleProportion;
	SourceHalfAngle+=SourceStartAngle;

	const float StartUOffset=(pSettings->m_SourceCentreX*Width);
	const float StartUGradient=cos(SourceStartAngle);
	const float StartVOffset=(pSettings->m_SourceCentreY*Height);
	const float StartVGradient=sin(SourceStartAngle);

	const float HalfUOffset=(pSettings->m_SourceCentreX*Width);
	const float HalfUGradient=cos(SourceHalfAngle);
	const float HalfVOffset=(pSettings->m_SourceCentreY*Height);
	const float HalfVGradient=sin(SourceHalfAngle);

	SPete_Kaleidascope_PartitionData PartitionData;
	Pete_Kaleidascope_PartitionLines(pLinesStart,nLinesCount,&PartitionData);

	const float OutputCentreX=(pSettings->m_OutputCentreX*(Width-1));
	const float OutputCentreY=(pSettings->m_OutputCentreY*(Height-1));

	const float LeftX=-OutputCentreX;
	const float RightX=Width-OutputCentreX;

	float CurrentY=-OutputCentreY;

	int nScanLine;
	for (nScanLine=0; nScanLine<nHeight; nScanLine+=1) {

		SPete_Kaleidascope_Line* pLinesGroupStart;
		int nLinesGroupCount;
		SPete_Kaleidascope_Line* pFirstLineOtherGroup;
		SPete_Kaleidascope_Line* pLastLineOtherGroup;

		if (CurrentY<0.0f) {
			pLinesGroupStart=PartitionData.pYNegLines;
			nLinesGroupCount=PartitionData.nYNegLinesCount;
			pFirstLineOtherGroup=PartitionData.pYPosLines;
			pLastLineOtherGroup=
				PartitionData.pYPosLines+(PartitionData.nYPosLinesCount-1);
		} else {
			pLinesGroupStart=PartitionData.pYPosLines;
			nLinesGroupCount=PartitionData.nYPosLinesCount;
			pFirstLineOtherGroup=PartitionData.pYNegLines;
			pLastLineOtherGroup=
				PartitionData.pYNegLines+(PartitionData.nYNegLinesCount-1);
		}

		SPete_Kaleidascope_Line* pLinesGroupEnd=pLinesGroupStart+nLinesGroupCount;

		SPete_Kaleidascope_Line* pCurrentLine=pLinesGroupStart;

		float PreviousIntersectionX=0.0f;
		float PreviousRowU;
		float PreviousRowV;

		U32* pOutputLineStart=pOutput+(nScanLine*nWidth);

		while ((pCurrentLine<=pLinesGroupEnd)&&(PreviousIntersectionX<RightX)) {

			const bool bIsFinalSpan=(pCurrentLine==pLinesGroupEnd);
			const bool bIsFirstSpan=(pCurrentLine==pLinesGroupStart);

			float IntersectionX;
			float IntersectionT;
			if (bIsFinalSpan) {
				IntersectionT=0.0f;
				IntersectionX=RightX;
			} else if (fabsf(pCurrentLine->Y)<Pete_Kaleidascope_Epsilon) {
				if (pCurrentLine->Y<0.0f) {
					IntersectionT=CurrentY/-Pete_Kaleidascope_Epsilon;
				} else {
					IntersectionT=CurrentY/Pete_Kaleidascope_Epsilon;
				}
				IntersectionX=(pCurrentLine->X*IntersectionT);
			} else {
				IntersectionT=(CurrentY/pCurrentLine->Y);
				IntersectionX=(pCurrentLine->X*IntersectionT);
			}

			float RowEndU;
			float RowEndV;

			bool bDebugIsHalfLine;

			if (bIsFirstSpan) {

				SPete_Kaleidascope_Line* pLine1=pFirstLineOtherGroup;
				SPete_Kaleidascope_Line* pLine2=pCurrentLine;

				float Line1IntersectionY;
				float Line1IntersectionT;
				if (fabsf(pLine1->X)<Pete_Kaleidascope_Epsilon) {
					if (pLine1->X<0.0f) {
						Line1IntersectionT=LeftX/-Pete_Kaleidascope_Epsilon;
					} else {
						Line1IntersectionT=LeftX/Pete_Kaleidascope_Epsilon;
					}
					Line1IntersectionY=(pLine1->Y*IntersectionT);
				} else {
					Line1IntersectionT=(LeftX/pLine1->X);
					Line1IntersectionY=(pLine1->Y*Line1IntersectionT);
				}

				float Line2IntersectionY;
				float Line2IntersectionT;
				if (fabsf(pLine2->X)<Pete_Kaleidascope_Epsilon) {
					if (pLine2->X<0.0f) {
						Line2IntersectionT=LeftX/-Pete_Kaleidascope_Epsilon;
					} else {
						Line2IntersectionT=LeftX/Pete_Kaleidascope_Epsilon;
					}
					Line2IntersectionY=(pLine2->Y*Line2IntersectionT);
				} else {
					Line2IntersectionT=(LeftX/pLine2->X);
					Line2IntersectionY=(pLine2->Y*Line2IntersectionT);
				}

				bool bIsHalfLine=(pLine2->Flags&PETE_KALEIDASCOPE_HALFLINE_BIT);
				bDebugIsHalfLine=bIsHalfLine;
				float Line1U;
				float Line1V;
				float Line2U;
				float Line2V;
				if (bIsHalfLine) {
					Line1U=StartUOffset+(Line1IntersectionT*StartUGradient);
					Line1V=StartVOffset+(Line1IntersectionT*StartVGradient);
					Line2U=HalfUOffset+(Line2IntersectionT*HalfUGradient);
					Line2V=HalfVOffset+(Line2IntersectionT*HalfVGradient);
				} else {
					Line1U=HalfUOffset+(Line1IntersectionT*HalfUGradient);
					Line1V=HalfVOffset+(Line1IntersectionT*HalfVGradient);
					Line2U=StartUOffset+(Line2IntersectionT*StartUGradient);
					Line2V=StartVOffset+(Line2IntersectionT*StartVGradient);
				}

				const float YDist=
					(Line2IntersectionY-Line1IntersectionY);

				const float OneMinusLerpValue=(CurrentY-Line1IntersectionY)/YDist;
				const float LerpValue=(1.0f-OneMinusLerpValue);

				PreviousRowU=(Line1U*LerpValue)+(Line2U*OneMinusLerpValue);
				PreviousRowV=(Line1V*LerpValue)+(Line2V*OneMinusLerpValue);

				if (fabsf(pLine1->X)<Pete_Kaleidascope_Epsilon) {
					if (pLine1->X<0.0f) {
						PreviousIntersectionX=-Pete_Kaleidascope_Epsilon*Line1IntersectionT;
					} else {
						PreviousIntersectionX=Pete_Kaleidascope_Epsilon*Line1IntersectionT;
					}
				} else {
					PreviousIntersectionX=(pLine1->X*Line1IntersectionT);
				}

				if (bIsHalfLine) {
					RowEndU=HalfUOffset+(IntersectionT*HalfUGradient);
					RowEndV=HalfVOffset+(IntersectionT*HalfVGradient);
				} else {
					RowEndU=StartUOffset+(IntersectionT*StartUGradient);
					RowEndV=StartVOffset+(IntersectionT*StartVGradient);
				}

			} else if (bIsFinalSpan) {

				SPete_Kaleidascope_Line* pLine1=(pCurrentLine-1);
				SPete_Kaleidascope_Line* pLine2=pLastLineOtherGroup;

				float Line1IntersectionY;
				float Line1IntersectionT;
				if (fabsf(pLine1->X)<Pete_Kaleidascope_Epsilon) {
					Line1IntersectionT=10000.0f;
					Line1IntersectionY=(pLine1->Y*10000.0f);
				} else {
					Line1IntersectionT=(RightX/pLine1->X);
					Line1IntersectionY=(pLine1->Y*Line1IntersectionT);
				}

				float Line2IntersectionY;
				float Line2IntersectionT;
				if (fabsf(pLine2->X)<Pete_Kaleidascope_Epsilon) {
					Line2IntersectionT=10000.0f;
					Line2IntersectionY=(pLine2->Y*Line2IntersectionT);
				} else {
					Line2IntersectionT=(RightX/pLine2->X);
					Line2IntersectionY=(pLine2->Y*Line2IntersectionT);
				}


				bool bIsHalfLine=(pLine2->Flags&PETE_KALEIDASCOPE_HALFLINE_BIT);
				bDebugIsHalfLine=bIsHalfLine;

				float Line1U;
				float Line1V;
				float Line2U;
				float Line2V;
				if (bIsHalfLine) {
					Line1U=StartUOffset+(Line1IntersectionT*StartUGradient);
					Line1V=StartVOffset+(Line1IntersectionT*StartVGradient);
					Line2U=HalfUOffset+(Line2IntersectionT*HalfUGradient);
					Line2V=HalfVOffset+(Line2IntersectionT*HalfVGradient);
				} else {
					Line1U=HalfUOffset+(Line1IntersectionT*HalfUGradient);
					Line1V=HalfVOffset+(Line1IntersectionT*HalfVGradient);
					Line2U=StartUOffset+(Line2IntersectionT*StartUGradient);
					Line2V=StartVOffset+(Line2IntersectionT*StartVGradient);
				}

				const float YDist=
					(Line2IntersectionY-Line1IntersectionY);

				const float OneMinusLerpValue=(CurrentY-Line1IntersectionY)/YDist;
				const float LerpValue=(1.0f-OneMinusLerpValue);

				RowEndU=(Line1U*LerpValue)+(Line2U*OneMinusLerpValue);
				RowEndV=(Line1V*LerpValue)+(Line2V*OneMinusLerpValue);

			} else {

				bool bIsHalfLine=(pCurrentLine->Flags&PETE_KALEIDASCOPE_HALFLINE_BIT);
				bDebugIsHalfLine=bIsHalfLine;

				if (bIsHalfLine) {
					RowEndU=HalfUOffset+(IntersectionT*HalfUGradient);
					RowEndV=HalfVOffset+(IntersectionT*HalfVGradient);
				} else {
					RowEndU=StartUOffset+(IntersectionT*StartUGradient);
					RowEndV=StartVOffset+(IntersectionT*StartVGradient);
				}

			}

			if (IntersectionX>LeftX) {

				int nRowStartX;

				if (PreviousIntersectionX<LeftX) {

					nRowStartX=0;

					const float XDist=(IntersectionX-PreviousIntersectionX);
					const float OneMinusLerpValue=(LeftX-PreviousIntersectionX)/XDist;
					const float LerpValue=(1.0f-OneMinusLerpValue);

					PreviousRowU=(PreviousRowU*LerpValue)+(RowEndU*OneMinusLerpValue);
					PreviousRowV=(PreviousRowV*LerpValue)+(RowEndV*OneMinusLerpValue);

				} else {

					nRowStartX=static_cast<int>(PreviousIntersectionX-LeftX);

				}

				int nRowEndX;

				if (bIsFinalSpan) {
					nRowEndX=static_cast<int>(RightX-LeftX);
				} else if (IntersectionX>RightX) {

					nRowEndX=static_cast<int>(RightX-LeftX);

					const float XDist=(IntersectionX-PreviousIntersectionX);
					const float OneMinusLerpValue=(RightX-PreviousIntersectionX)/XDist;
					const float LerpValue=(1.0f-OneMinusLerpValue);

					RowEndU=(PreviousRowU*LerpValue)+(RowEndU*OneMinusLerpValue);
					RowEndV=(PreviousRowV*LerpValue)+(RowEndV*OneMinusLerpValue);

				} else {
					nRowEndX=static_cast<int>(IntersectionX-LeftX);
				}


				U32* pRowStart=pOutputLineStart+nRowStartX;
				int nRowLength=(nRowEndX-nRowStartX);
				if (nRowLength<=0) {
					nRowLength=1;
				}
				U32*const pSpanEnd=(pRowStart+nRowLength);

				const int nFPShift=16;
				const int nFPMult=(1<<nFPShift);

				float CurrentU=PreviousRowU;
				float CurrentV=PreviousRowV;

				float DeltaU=(RowEndU-PreviousRowU)/nRowLength;
				float DeltaV=(RowEndV-PreviousRowV)/nRowLength;

				int nCurrentU=static_cast<int>(CurrentU*nFPMult);
				int nCurrentV=static_cast<int>(CurrentV*nFPMult);

				const int nWidthFP=(nWidth<<nFPShift);
				const int nHeightFP=(nHeight<<nFPShift);

				nCurrentU+=(nWidthFP*10);
				nCurrentV+=(nHeightFP*10);

				int nDeltaU=static_cast<int>(DeltaU*nFPMult);
				int nDeltaV=static_cast<int>(DeltaV*nFPMult);

				const int nTwoWidth=(nWidth*2)<<nFPShift;
				const int nTwoWidthMinusOne=(nTwoWidth-(1<<nFPShift));

				const int nTwoHeight=(nHeight*2)<<nFPShift;
				const int nTwoHeightMinusOne=(nTwoHeight-(1<<nFPShift));

				U32* pCurrentOutput=pRowStart;
				while (pCurrentOutput<pSpanEnd) {

					int nNextU;
					if (nDeltaU>=0) {
						nNextU=((nCurrentU+nWidthFP)/nWidthFP)*nWidthFP;
					} else {
						nNextU=((nCurrentU-(1<<nFixedShift))/nWidthFP)*nWidthFP;
					}

					int nUDist;
					if (nDeltaU!=0) {
						nUDist=(nNextU-nCurrentU)/nDeltaU;
						nUDist+=1;
					} else {
						nUDist=cnBiggestSignedInt;
					}

					int nNextV;
					if (nDeltaV>=0) {
						nNextV=((nCurrentV+nHeightFP)/nHeightFP)*nHeightFP;
					} else {
						nNextV=((nCurrentV-(1<<nFixedShift))/nHeightFP)*nHeightFP;
					}
					int nVDist;
					if (nDeltaV!=0) {
						nVDist=(nNextV-nCurrentV)/nDeltaV;
						nVDist+=1;
					} else {
						nVDist=cnBiggestSignedInt;
					}

					int nMinDist;
					if (nUDist<nVDist) {
						nMinDist=nUDist;
					} else {
						nMinDist=nVDist;
					}

					int nStartU=nCurrentU%nTwoWidth;
					if (nStartU>=nWidthFP) {
						nStartU=(nTwoWidthMinusOne-nStartU);
					}
					nStartU=GateInt(nStartU,0,nWidthFP-(1<<nFPShift));

					int nStartV=nCurrentV%nTwoHeight;
					if (nStartV>=nHeightFP) {
						nStartV=(nTwoHeightMinusOne-nStartV);
					}
					nStartV=GateInt(nStartV,0,nHeightFP-(1<<nFPShift));

					int nEndU=(nCurrentU+(nMinDist*nDeltaU))%nTwoWidth;
					if (nEndU>=nWidthFP) {
						nEndU=(nTwoWidthMinusOne-nEndU);
					}
					nEndU=GateInt(nEndU,0,nWidthFP-(1<<nFPShift));

					int nEndV=(nCurrentV+(nMinDist*nDeltaV))%nTwoHeight;
					if (nEndV>=nHeightFP) {
						nEndV=(nTwoHeightMinusOne-nEndV);
					}
					nEndV=GateInt(nEndV,0,nHeightFP-(1<<nFPShift));

					int nLocalDeltaU;
					int nLocalDeltaV;
					if (nMinDist<1) {
						nLocalDeltaU=0;
						nLocalDeltaV=0;
					} else {
						nLocalDeltaU=(nEndU/nMinDist)-(nStartU/nMinDist);
						nLocalDeltaV=(nEndV/nMinDist)-(nStartV/nMinDist);
					}
					int nLocalCurrentU=nStartU;
					int nLocalCurrentV=nStartV;

					U32* pLocalSpanEnd=(pCurrentOutput+nMinDist);
					if ((pLocalSpanEnd>pSpanEnd)||(nMinDist==cnBiggestSignedInt)) {
						pLocalSpanEnd=pSpanEnd;
					}

					while (pCurrentOutput<pLocalSpanEnd) {

						const int nUIntegral=(nLocalCurrentU>>nFPShift);
						const int nVIntegral=(nLocalCurrentV>>nFPShift);

//						if ((nUIntegral>=nWidth)||
//							(nUIntegral<0)||
//							(nVIntegral>=nHeight)||
//							(nVIntegral<0)) {
//
//							int foo=0;
//							foo+=1;
//
//						}


//						if ((nUIntegral^nVIntegral)&32) {
//							*pCurrentOutput=0x00ffffff;
//						} else {
//							*pCurrentOutput=0x00000000;
//						}

						U32* pCurrentSource=
							pSource+(nVIntegral*nWidth)+nUIntegral;

						*pCurrentOutput=*pCurrentSource;

//						if (nMinDist==1) {
//							*pCurrentOutput=0x00000000;
//						}

						pCurrentOutput+=1;
						nLocalCurrentU+=nLocalDeltaU;
						nLocalCurrentV+=nLocalDeltaV;

					}

					if (nMinDist<1) {
						nCurrentU+=nDeltaU;
						nCurrentV+=nDeltaV;
					} else {
						nCurrentU+=(nMinDist*nDeltaU);
						nCurrentV+=(nMinDist*nDeltaV);
					}
				}
			}

			PreviousIntersectionX=IntersectionX;
			PreviousRowU=RowEndU;
			PreviousRowV=RowEndV;

			pCurrentLine+=1;

		}

		CurrentY+=1.0f;

	}

}

void Pete_Kaleidascope_SetupLines(SPete_Kaleidascope_Data* pInstanceData,SPete_Kaleidascope_Settings* pSettings,int* poutLinesCount) {

	const float MaxDivisions=((pInstanceData->nMaxLines/2)-1.0f);

	float Divisions=ceilf(pSettings->m_Divisions);
	if (Divisions<1.0f) {
		Divisions=1.0f;
	} else if (Divisions>MaxDivisions) {
		Divisions=MaxDivisions;
	}

	const int nDivisionsInt=static_cast<int>(Divisions);

	const float AnglePreIncrement=pSettings->m_OutputAnglePreIncrement+0.001;

	const float AngleInterval=(Pete_TwoPi/Divisions);
	const float HalfAngleInterval=(AngleInterval*pSettings->m_ReflectionLineProportion);

	const int nLinesCount=(nDivisionsInt*2);

	SPete_Kaleidascope_Line* pLinesStart=(SPete_Kaleidascope_Line*)Pete_LockHandle(pInstanceData->hLines);
	if (pLinesStart==NULL) {
		return;
	}
	SPete_Kaleidascope_Line* pLinesEnd=pLinesStart+nLinesCount;

	SPete_Kaleidascope_Line* pCurrentLine=pLinesStart;

	int nCurrentDivision;
	for (nCurrentDivision=0; nCurrentDivision<nDivisionsInt; nCurrentDivision+=1) {

		float StartAngle=
			AnglePreIncrement+(nCurrentDivision*AngleInterval);
		StartAngle=fmodf(StartAngle,Pete_TwoPi);

		pCurrentLine->X=cos(StartAngle);
		pCurrentLine->Y=sin(StartAngle);
		pCurrentLine->Flags=0;

		pCurrentLine+=1;

		float HalfAngle=(StartAngle+HalfAngleInterval);
		HalfAngle=fmodf(HalfAngle,Pete_TwoPi);

		pCurrentLine->X=cos(HalfAngle);
		pCurrentLine->Y=sin(HalfAngle);
		pCurrentLine->Flags=PETE_KALEIDASCOPE_HALFLINE_BIT;

		pCurrentLine+=1;

	}

	*poutLinesCount=nLinesCount;

}

extern "C" int Pete_Kaleidascope_LinesSortFunction(const void* pElem1,const void* pElem2) {

	SPete_Kaleidascope_Line* pFirstLine=(SPete_Kaleidascope_Line*)pElem1;
	SPete_Kaleidascope_Line* pSecondLine=(SPete_Kaleidascope_Line*)pElem2;

	const float FirstYIsNeg=(pFirstLine->Y<0.0f);
	const float SecondYIsNeg=(pSecondLine->Y<0.0f);

	if (FirstYIsNeg!=SecondYIsNeg) {

		if (FirstYIsNeg) {
			return -1;
		} else {
			return 1;
		}

	} else {

		const float FirstX=pFirstLine->X;
		const float SecondX=pSecondLine->X;

		if (FirstX<SecondX) {
			return -1;
		} else if (FirstX>SecondX) {
			return 1;
		} else {
			return 0;
		}

	}

}

void Pete_Kaleidascope_PartitionLines(SPete_Kaleidascope_Line* pLinesStart,int nLinesCount,SPete_Kaleidascope_PartitionData* poutPartitionData) {

	qsort((void*)pLinesStart,nLinesCount,sizeof(SPete_Kaleidascope_Line),&Pete_Kaleidascope_LinesSortFunction);

	SPete_Kaleidascope_Line* pLinesEnd=pLinesStart+nLinesCount;

	SPete_Kaleidascope_Line* pCurrentLine=pLinesStart;
	while ((pCurrentLine<pLinesEnd)&&(pCurrentLine->Y<0.0f)) {
		pCurrentLine+=1;
	}

	const int nYNegCount=pCurrentLine-pLinesStart;

	poutPartitionData->pYNegLines=pLinesStart;
	poutPartitionData->nYNegLinesCount=nYNegCount;
	poutPartitionData->pYPosLines=pLinesStart+nYNegCount;
	poutPartitionData->nYPosLinesCount=(nLinesCount-nYNegCount);

}

void Pete_Kaleidascope_RenderAE(SPete_Kaleidascope_Data* pInstanceData,SPete_Kaleidascope_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	SPete_2dMatrix Transforms[nMaxDivisions];
	Pete_Kaleidascope_CreateAllTransforms(pInstanceData,pSettings,&Transforms[0]);

	int nDivisionCount=(int)(pSettings->m_Divisions);
	nDivisionCount=GateInt(nDivisionCount,1,(nMaxDivisions-1));

	const float DivisionAngle=(Pete_TwoPi/nDivisionCount);

	const float CentreX=(nWidth/2.0f);
	const float CentreY=(nHeight/2.0f);

	U32* pCurrentOutput=pOutput;

	float Y;
	for (Y=0.0f; Y<nHeight; Y+=1.0f) {

		const float DeltaY=(Y-CentreY);

		float X;
		for (X=0.0f; X<nWidth; X+=1.0f) {

			const float DeltaX=(X-CentreX);

			float Angle=atan2(DeltaX,DeltaY);
			Angle+=Pete_Pi;

			int nCurrentDivision=(int)floorf(Angle/DivisionAngle);
			nCurrentDivision=GateInt(nCurrentDivision,0,(nDivisionCount-1));

			SPete_2dMatrix* pCurrentTransform=&Transforms[nCurrentDivision];

			SPete_2dVector CurrentPos={X,Y};

			SPete_2dVector SourcePos;
			Pete_2dMatrix_TransformVector(&CurrentPos,pCurrentTransform,&SourcePos);

			int nSourceX=SourcePos.x;
			nSourceX=GetMirrored(nSourceX,nWidth);

			int nSourceY=SourcePos.y;
			nSourceY=GetMirrored(nSourceY,nHeight);

			U32* pCurrentSource=
				pSource+(nSourceY*nWidth)+nSourceX;

			*pCurrentOutput=*pCurrentSource;

			pCurrentOutput+=1;

		}

	}

}

void Pete_Kaleidascope_CreateAllTransforms(SPete_Kaleidascope_Data* pInstanceData,SPete_Kaleidascope_Settings* pSettings,SPete_2dMatrix* pTransforms) {

	int nDivisionCount=(int)(pSettings->m_Divisions);
	nDivisionCount=GateInt(nDivisionCount,1,(nMaxDivisions-1));

	const float DivisionAngle=(Pete_TwoPi/nDivisionCount);

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const float HalfWidth=nWidth/2.0f;
	const float HalfHeight=nHeight/2.0f;

	SPete_2dMatrix ScreenToWorld;
	Pete_2dMatrix_SetToTranslation(-HalfWidth,-HalfHeight,&ScreenToWorld);

	const float OriginX=pSettings->m_SourceCentreX;
	const float OriginY=pSettings->m_SourceCentreY;

	SPete_2dMatrix PanTransform;
	Pete_2dMatrix_SetToTranslation(-(OriginX*nWidth),-(OriginY*nHeight),&PanTransform);

	const float ScaleNormalX=cos(-DivisionAngle);
	const float ScaleNormalY=sin(-DivisionAngle);

	SPete_2dMatrix DirectionalScaleTransform;
	Pete_2dMatrix_SetToDirectionalScale(ScaleNormalX,ScaleNormalY,-1.0f,&DirectionalScaleTransform);

	SPete_2dMatrix WorldToScreen;
	Pete_2dMatrix_SetToTranslation(HalfWidth,HalfHeight,&WorldToScreen);

	int nCurrentDivision;
	for (nCurrentDivision=0; nCurrentDivision<nDivisionCount; nCurrentDivision+=1) {

		SPete_2dMatrix* pCurrentTransform=&pTransforms[nCurrentDivision];

		const bool bIsMirroredDivision=((nCurrentDivision&0x1)==0x1);

		float RotationAngle;
		if (!bIsMirroredDivision) {
			RotationAngle=DivisionAngle*nCurrentDivision;
		} else {
			RotationAngle=DivisionAngle*(nCurrentDivision-1);
		}

		SPete_2dMatrix RotationTransform;
		Pete_2dMatrix_SetToRotation(RotationAngle,&RotationTransform);

		Pete_2dMatrix_SetToIdentity(pCurrentTransform);

		Pete_2dMatrix_Concatenate(pCurrentTransform,&ScreenToWorld,pCurrentTransform);

		Pete_2dMatrix_Concatenate(pCurrentTransform,&RotationTransform,pCurrentTransform);

		if (bIsMirroredDivision) {
			Pete_2dMatrix_Concatenate(pCurrentTransform,&DirectionalScaleTransform,pCurrentTransform);
		}

		Pete_2dMatrix_Concatenate(pCurrentTransform,&PanTransform,pCurrentTransform);

		Pete_2dMatrix_Concatenate(pCurrentTransform,&WorldToScreen,pCurrentTransform);

	}

}


void Pete_Kaleidascope_Dev(SPete_Kaleidascope_Data* pInstanceData,SPete_Kaleidascope_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	U32* pCurrentOutput=pOutput;

	SPete_2dVector GridA={1.0f,0.0f};
	SPete_2dVector GridB={0.0f,1.0f};

	const float Angle=pSettings->m_OutputAnglePreIncrement;

	SPete_2dMatrix RotationTransform;
	Pete_2dMatrix_SetToRotation(Angle,&RotationTransform);

	Pete_2dMatrix_TransformVector(&GridA,&RotationTransform,&GridA);
//	Pete_2dMatrix_TransformVector(&GridB,&RotationTransform,&GridB);

	const float GridMagA=20.0f;
	const float GridMagB=20.0f;

	const float RecipGridMagA=(1.0f/GridMagA);
	const float RecipGridMagB=(1.0f/GridMagB);

	const float Scale=2.0f;

	float Y;
	for (Y=0.0f; Y<nHeight; Y+=1.0f) {

		float X;
		for (X=0.0f; X<nWidth; X+=1.0f) {

			SPete_2dVector CurrentPos={X,Y};

			const float PosDotA=Pete_2dVector_DotProduct(&CurrentPos,&GridA);
			const float PosDotB=Pete_2dVector_DotProduct(&CurrentPos,&GridB);

			const float IntegralPosDotA=(floorf(PosDotA*RecipGridMagA)+0.5f)*GridMagA;
			const float IntegralPosDotB=(floorf(PosDotB*RecipGridMagB)+0.5f)*GridMagB;

			SPete_2dVector ScaledA;
			Pete_2dVector_Scale(&GridA,IntegralPosDotA,&ScaledA);

			SPete_2dVector ScaledB;
			Pete_2dVector_Scale(&GridB,IntegralPosDotB,&ScaledB);

			SPete_2dVector Centre;
			Pete_2dVector_Add(&ScaledA,&ScaledB,&Centre);

			SPete_2dVector SourcePos;
			Pete_2dVector_Subtract(&CurrentPos,&Centre,&SourcePos);

			Pete_2dVector_Scale(&SourcePos,Scale,&SourcePos);

			Pete_2dVector_Add(&SourcePos,&Centre,&SourcePos);

			int nSourceX=SourcePos.x;
			nSourceX=GetMirrored(nSourceX,nWidth);

			int nSourceY=SourcePos.y;
			nSourceY=GetMirrored(nSourceY,nHeight);

			U32* pCurrentSource=
				pSource+(nSourceY*nWidth)+nSourceX;

			*pCurrentOutput=*pCurrentSource;

			pCurrentOutput+=1;

		}

	}

}
