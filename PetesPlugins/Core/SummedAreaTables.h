#ifndef INCLUDE_SUMMEDAREATABLES_H
#define INCLUDE_SUMMEDAREATABLES_H

#include "PeteHelpers.h"

struct SPete_SATEntry {
	U32 Alpha;
	U32 Red;
	U32 Green;
	U32 Blue;
};

void Pete_SummedAreaTable_Construct(U32* pSource,int nWidth,int nHeight,SPete_SATEntry* poutTable);

inline void Pete_SummedAreaTable_Subtract(SPete_SATEntry* pResult,const SPete_SATEntry* pValue) {
	pResult->Red-=pValue->Red;
	pResult->Green-=pValue->Green;
	pResult->Blue-=pValue->Blue;
	pResult->Alpha-=pValue->Alpha;
}

inline void Pete_SummedAreaTable_Add(SPete_SATEntry* pResult,const SPete_SATEntry* pValue) {
	pResult->Red+=pValue->Red;
	pResult->Green+=pValue->Green;
	pResult->Blue+=pValue->Blue;
	pResult->Alpha+=pValue->Alpha;
}

inline U32 Pete_SummedAreaTable_GetAverage(SPete_SATEntry* pTable,int nWidth,int nHeight,int nLeft,int nTop,int nRight,int nBottom) {

	const int nTopTimesWidth=(nTop*nWidth);
	const int nBottomTimesWidth=(nBottom*nWidth);

	const SPete_SATEntry* pTopLeftEntry=
		(pTable+nTopTimesWidth+nLeft);

	const SPete_SATEntry* pTopRightEntry=
		(pTable+nTopTimesWidth+nRight);

	const SPete_SATEntry* pBottomLeftEntry=
		(pTable+nBottomTimesWidth+nLeft);

	const SPete_SATEntry* pBottomRightEntry=
		(pTable+nBottomTimesWidth+nRight);

	SPete_SATEntry SumTotal=*pBottomRightEntry;
	Pete_SummedAreaTable_Subtract(&SumTotal,pTopRightEntry);
	Pete_SummedAreaTable_Subtract(&SumTotal,pBottomLeftEntry);
	Pete_SummedAreaTable_Add(&SumTotal,pTopLeftEntry);

	const int nRectArea=
		(nRight-nLeft)*(nBottom-nTop);

	const int nOutputRed=(SumTotal.Red/nRectArea);
	const int nOutputGreen=(SumTotal.Green/nRectArea);
	const int nOutputBlue=(SumTotal.Blue/nRectArea);
	const int nOutputAlpha=(SumTotal.Alpha/nRectArea);

	const U32 OutputColour=
		(nOutputRed<<SHIFT_RED)|
		(nOutputGreen<<SHIFT_GREEN)|
		(nOutputBlue<<SHIFT_BLUE)|
		(nOutputAlpha<<SHIFT_ALPHA);

	return OutputColour;

}

#endif // INCLUDE_SUMMEDAREATABLES_H