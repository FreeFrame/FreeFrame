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

#include "Burn.h"
#include "ChannelFunction.h"
#include "PeteHelpers.h"

void Pete_Burn_SetupCFSettings(SPete_Burn_Data* pInstanceData,SPete_Burn_Settings* pInSettings,SPete_ChannelFunction_Settings* pOutSettings);

void Pete_Burn_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=NULL;
	*poutnParametersCount=0;
}

int Pete_Burn_Init(SPete_Burn_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_Burn_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_Burn_DeInit(SPete_Burn_Data* pInstanceData) {

	// do nothing

}

void Pete_Burn_Render(SPete_Burn_Data* pInstanceData,SPete_Burn_Settings* pSettings,U32* pSource,U32* pOutput) {

	SPete_ChannelFunction_Settings CFSettings;

	Pete_Burn_SetupCFSettings(pInstanceData,pSettings,&CFSettings);

	Pete_ChannelFunction_Render(&CFSettings,pSource,pOutput);

}

void Pete_Burn_SetupCFSettings(SPete_Burn_Data* pInstanceData,SPete_Burn_Settings* pInSettings,SPete_ChannelFunction_Settings* pOutSettings) {

	int*const pRedTable=&(pOutSettings->m_nRedTable[0]);
	int*const pGreenTable=&(pOutSettings->m_nGreenTable[0]);
	int*const pBlueTable=&(pOutSettings->m_nBlueTable[0]);

	int nCount;
	for (nCount=0; nCount<256; nCount+=1) {

		const unsigned int nSourceRed=nCount;
		const unsigned int nSourceGreen=nCount;
		const unsigned int nSourceBlue=nCount;

		unsigned int nOutputRed=
			256-((256*(255-nSourceRed))/(nSourceRed+1));
		unsigned int nOutputGreen=
			256-((256*(255-nSourceGreen))/(nSourceGreen+1));
		unsigned int nOutputBlue=
			256-((256*(255-nSourceBlue))/(nSourceBlue+1));

		nOutputRed=GateInt(nOutputRed,0,255);
		nOutputGreen=GateInt(nOutputGreen,0,255);
		nOutputBlue=GateInt(nOutputBlue,0,255);

		pRedTable[nCount]=(nOutputRed<<SHIFT_RED);
		pGreenTable[nCount]=(nOutputGreen<<SHIFT_GREEN);
		pBlueTable[nCount]=(nOutputBlue<<SHIFT_BLUE);

	}

	pOutSettings->m_nWidth=pInstanceData->nWidth;
	pOutSettings->m_nHeight=pInstanceData->nHeight;

}
