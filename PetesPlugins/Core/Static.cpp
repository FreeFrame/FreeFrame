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


#include "Static.h"
#include "PeteHelpers.h"

#include <time.h>

static SPete_Parameter g_Parameters[]={
	{
		"Grey Scale",
		1.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
	{
		"Two Tone",
		0.0f,
		0.0f,
		1.0f,
		PETE_PARAM_BOOL
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_Static_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_Static_Init(SPete_Static_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_Static_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_Static_DeInit(SPete_Static_Data* pInstanceData) {

	// do nothing

}

void Pete_Static_Render(SPete_Static_Data* pInstanceData,SPete_Static_Settings* pSettings,U32* pSource,U32* pOutput) {

	static U32 Seed=0;
	Seed+=time(NULL);

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const int nNumPixels = nWidth*nHeight;

	U32* pCurrentOutput=pOutput;
	const U32* pOutputEnd=(pOutput+nNumPixels);

	if (pSettings->m_DoGreyScale>0.0f) {

		if (pSettings->m_DoTwoTone>0.0f) {

			while (pCurrentOutput!=pOutputEnd) {

				Seed^=(((Seed<<13)|(Seed>>19))+1);

				const U32 OutputColour=((signed long)(Seed))>>31;

				*pCurrentOutput=OutputColour;

				pCurrentOutput+=1;

			}

		} else {

			while (pCurrentOutput!=pOutputEnd) {

				Seed^=(((Seed<<13)|(Seed>>19))+1);
				
				U32 OutputColour=(Seed&0xff);
				OutputColour|=(OutputColour<<SHIFT_GREEN);
				OutputColour|=(OutputColour<<SHIFT_RED);

				*pCurrentOutput=OutputColour;

				pCurrentOutput+=1;

			}

		}


	} else {

		if (pSettings->m_DoTwoTone>0.0f) {

			while (pCurrentOutput!=pOutputEnd) {

				Seed^=(((Seed<<13)|(Seed>>19))+1);

				U32 OutputRed=((signed long)(Seed<<0))>>31;
				U32 OutputGreen=((signed long)(Seed<<1))>>31;
				U32 OutputBlue=((signed long)(Seed<<2))>>31;

				OutputRed&=(0xff<<SHIFT_RED);
				OutputGreen&=(0xff<<SHIFT_GREEN);
				OutputBlue&=(0xff<<SHIFT_BLUE);

				U32 OutputColour=OutputRed|OutputGreen|OutputBlue;

				*pCurrentOutput=OutputColour;

				pCurrentOutput+=1;

			}

		} else {

			while (pCurrentOutput!=pOutputEnd) {

				Seed^=(((Seed<<13)|(Seed>>19))+1);

				*pCurrentOutput=Seed;

				pCurrentOutput+=1;

			}

		}

	}

}
