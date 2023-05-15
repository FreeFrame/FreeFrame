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



#include "Wave.h"
#include "PeteHelpers.h"

static SPete_Parameter g_Parameters[]={
	{
		"Friction",
		0.95f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Speed",
		0.2f,
		0.0f,
		1.0f,
		PETE_PARAM_FLOAT
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_Wave_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_Wave_Init(SPete_Wave_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_Wave_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	const int nBufferByteCount=
		nWidth*nHeight*sizeof(SPete_Wave_Cell);

	pInstanceData->hWaveBuffer=
		Pete_NewHandle(nBufferByteCount);
	if (pInstanceData->hWaveBuffer==NULL) {
		Pete_Wave_DeInit(pInstanceData);
		return 0;
	}

	SPete_Wave_Cell* pWaveBuffer=(SPete_Wave_Cell*)Pete_LockHandle(pInstanceData->hWaveBuffer);

	Pete_ZeroMemory(pWaveBuffer,nBufferByteCount);

	return 1;

}

void Pete_Wave_DeInit(SPete_Wave_Data* pInstanceData) {

	if (pInstanceData->hWaveBuffer!=NULL) {
		Pete_FreeHandle(pInstanceData->hWaveBuffer);
		pInstanceData->hWaveBuffer=NULL;
	}

}

void Pete_Wave_Render(SPete_Wave_Data* pInstanceData,SPete_Wave_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nNumPixels = nWidth*nHeight;

	const float Friction=pSettings->m_Friction;
	const float Speed=pSettings->m_Speed;

	U32* pCurrentSource=pSource;
	const U32* pSourceEnd=(pSource+nNumPixels);

	SPete_Wave_Cell*const pWaveDataStart=(SPete_Wave_Cell*)Pete_LockHandle(pInstanceData->hWaveBuffer);
	if (pWaveDataStart==NULL) {
		return;
	}
	SPete_Wave_Cell*const pWaveDataEnd=pWaveDataStart+nNumPixels;

	SPete_Wave_Cell* pCurrentWaveData=pWaveDataStart;

	while (pCurrentSource!=pSourceEnd) {

		const U32* pSourceLineStart=pCurrentSource;
		const U32* pSourceLineEnd=pCurrentSource+nWidth;

		SPete_Wave_Cell* const pWaveDataLineStart=
			pCurrentWaveData;

		SPete_Wave_Cell* const pWaveDataLineEnd=
			pCurrentWaveData+nWidth;

		SPete_Wave_Cell* pPrevLineWaveData=pCurrentWaveData-nWidth;
		if (pPrevLineWaveData<pWaveDataStart) {
			pPrevLineWaveData=pWaveDataStart;
		}

		SPete_Wave_Cell* pNextLineWaveData=pCurrentWaveData+nWidth;
		if (pNextLineWaveData>(pWaveDataEnd-nWidth)) {
			pNextLineWaveData=(pWaveDataEnd-nWidth);
		}

		SPete_Wave_Cell* pPrevWaveData=pCurrentWaveData-1;
		SPete_Wave_Cell* pNextWaveData=pCurrentWaveData+1;

		while (pCurrentSource!=pSourceLineEnd) {

			const U32 SourceColour=*pCurrentSource;

			const float SourceRed=(float)((SourceColour>>SHIFT_RED)&0xff);
			const float SourceGreen=(float)((SourceColour>>SHIFT_GREEN)&0xff);
			const float SourceBlue=(float)((SourceColour>>SHIFT_BLUE)&0xff);

			SPete_Wave_Cell* pLeftWaveData=pPrevWaveData;
			if (pLeftWaveData<pWaveDataLineStart) {
				pLeftWaveData=pWaveDataLineStart;
			}

			SPete_Wave_Cell* pRightWaveData=pNextWaveData;
			if (pRightWaveData>=pWaveDataLineEnd) {
				pRightWaveData=(pWaveDataLineEnd-1);
			}

			const float CurrentRed=pCurrentWaveData->RedHeight;
			const float CurrentGreen=pCurrentWaveData->GreenHeight;
			const float CurrentBlue=pCurrentWaveData->BlueHeight;

			float DesiredRed=SourceRed;
			DesiredRed+=pPrevLineWaveData->RedHeight;
			DesiredRed+=pNextLineWaveData->RedHeight;
			DesiredRed+=pLeftWaveData->RedHeight;
			DesiredRed+=pRightWaveData->RedHeight;
			DesiredRed*=(1.0f/5.0f);

			float DesiredGreen=SourceGreen;
			DesiredGreen+=pPrevLineWaveData->GreenHeight;
			DesiredGreen+=pNextLineWaveData->GreenHeight;
			DesiredGreen+=pLeftWaveData->GreenHeight;
			DesiredGreen+=pRightWaveData->GreenHeight;
			DesiredGreen*=(1.0f/5.0f);

			float DesiredBlue=SourceBlue;
			DesiredBlue+=pPrevLineWaveData->BlueHeight;
			DesiredBlue+=pNextLineWaveData->BlueHeight;
			DesiredBlue+=pLeftWaveData->BlueHeight;
			DesiredBlue+=pRightWaveData->BlueHeight;
			DesiredBlue*=(1.0f/5.0f);

			const float DeltaRed=DesiredRed-CurrentRed;
			const float DeltaGreen=DesiredGreen-CurrentGreen;
			const float DeltaBlue=DesiredBlue-CurrentBlue;

			const float AccelRed=DeltaRed*Speed;
			const float AccelGreen=DeltaGreen*Speed;
			const float AccelBlue=DeltaBlue*Speed;

			float VelocityRed=pCurrentWaveData->RedVelocity;
			VelocityRed+=AccelRed;
			VelocityRed*=Friction;
			pCurrentWaveData->RedVelocity=VelocityRed;

			float VelocityGreen=pCurrentWaveData->GreenVelocity;
			VelocityGreen+=AccelGreen;
			VelocityGreen*=Friction;
			pCurrentWaveData->GreenVelocity=VelocityGreen;

			float VelocityBlue=pCurrentWaveData->BlueVelocity;
			VelocityBlue+=AccelBlue;
			VelocityBlue*=Friction;
			pCurrentWaveData->BlueVelocity=VelocityBlue;

			pCurrentSource+=1;
			pCurrentWaveData+=1;
			pPrevLineWaveData+=1;
			pNextLineWaveData+=1;
			pPrevWaveData+=1;
			pNextWaveData+=1;

		}

	}

	pCurrentWaveData=pWaveDataStart;
	U32* pCurrentOutput=pOutput;

	while (pCurrentWaveData!=pWaveDataEnd) {

		float CurrentRed=pCurrentWaveData->RedHeight;
		CurrentRed+=pCurrentWaveData->RedVelocity;
		pCurrentWaveData->RedHeight=CurrentRed;

		float CurrentGreen=pCurrentWaveData->GreenHeight;
		CurrentGreen+=pCurrentWaveData->GreenVelocity;
		pCurrentWaveData->GreenHeight=CurrentGreen;

		float CurrentBlue=pCurrentWaveData->BlueHeight;
		CurrentBlue+=pCurrentWaveData->BlueVelocity;
		pCurrentWaveData->BlueHeight=CurrentBlue;

		const int nOutputRed=GateInt(static_cast<int>(CurrentRed),0,255);
		const int nOutputGreen=GateInt(static_cast<int>(CurrentGreen),0,255);
		const int nOutputBlue=GateInt(static_cast<int>(CurrentBlue),0,255);

		const U32 OutputColour=
			((nOutputRed&0xff)<<SHIFT_RED)|
			((nOutputGreen&0xff)<<SHIFT_GREEN)|
			((nOutputBlue&0xff)<<SHIFT_BLUE);

		*pCurrentOutput=OutputColour;

		pCurrentOutput+=1;
		pCurrentWaveData+=1;

	}

}

