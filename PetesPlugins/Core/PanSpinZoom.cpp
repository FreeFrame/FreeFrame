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

#include "PanSpinZoom.h"

#include <math.h>

#include "PeteHelpers.h"
#include "Pete2dMatrix.h"

static SPete_Parameter g_Parameters[]={
	{
		"Origin X",
		0.5f,
		0.0f,
		1.0f,
		PETE_PARAM_POS_X
	},
	{
		"Origin Y",
		0.5f,
		0.0f,
		1.0f,
		PETE_PARAM_POS_Y
	},
	{
		"Rotation",
		0.0f,
		0.0f,
		Pete_TwoPi,
		PETE_PARAM_ANGLE
	},
	{
		"Scale",
		1.0f,
		0.25f,
		4.0f,
		PETE_PARAM_FLOAT
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_PanSpinZoom_CreateTransform(SPete_PanSpinZoom_Data* pInstanceData,SPete_PanSpinZoom_Settings* pSettings,SPete_2dMatrix* poutTransform);

void Pete_PanSpinZoom_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_PanSpinZoom_Init(SPete_PanSpinZoom_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_PanSpinZoom_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_PanSpinZoom_DeInit(SPete_PanSpinZoom_Data* pInstanceData) {
	// do nothing
}

void Pete_PanSpinZoom_Render(SPete_PanSpinZoom_Data* pInstanceData,SPete_PanSpinZoom_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	SPete_2dMatrix Transform;
	Pete_PanSpinZoom_CreateTransform(pInstanceData,pSettings,&Transform);

	U32* pCurrentOutput=pOutput;

	float Y;
	for (Y=0.0f; Y<nHeight; Y+=1.0f) {

		float X;
		for (X=0.0f; X<nWidth; X+=1.0f) {

			SPete_2dVector CurrentPos={X,Y};

			SPete_2dVector SourcePos;
			Pete_2dMatrix_TransformVector(&CurrentPos,&Transform,&SourcePos);

			const int nSourceX=SourcePos.x;
//			nSourceX=GetTiled(nSourceX,nWidth);

			const int nSourceY=SourcePos.y;
//			nSourceY=GetTiled(nSourceY,nHeight);

			if ((nSourceX>=0)&&
				(nSourceX<nWidth)&&
				(nSourceY>=0)&&
				(nSourceY<nHeight)) {


				U32* pCurrentSource=
					pSource+(nSourceY*nWidth)+nSourceX;

				*pCurrentOutput=*pCurrentSource;

			} else {

				*pCurrentOutput=0x00000000;

			}

			pCurrentOutput+=1;

		}

	}

}

void Pete_PanSpinZoom_CreateTransform(SPete_PanSpinZoom_Data* pInstanceData,SPete_PanSpinZoom_Settings* pSettings,SPete_2dMatrix* poutTransform) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;

	const float HalfWidth=nWidth/2.0f;
	const float HalfHeight=nHeight/2.0f;

	SPete_2dMatrix ScreenToWorld;
	Pete_2dMatrix_SetToTranslation(-HalfWidth,-HalfHeight,&ScreenToWorld);

	const float OriginX=(pSettings->OriginX-0.5f);
	const float OriginY=(pSettings->OriginY-0.5f);

	SPete_2dMatrix PanTransform;
	Pete_2dMatrix_SetToTranslation((-OriginX*nWidth),(-OriginY*nHeight),&PanTransform);

	const float Rotation=pSettings->Rotation;

	SPete_2dMatrix RotationTransform;
	Pete_2dMatrix_SetToRotation(Rotation,&RotationTransform);

	const float Scale=pSettings->Scale;

	SPete_2dMatrix ScaleTransform;
	Pete_2dMatrix_SetToAxisScale(Scale,Scale,&ScaleTransform);

	SPete_2dMatrix WorldToScreen;
	Pete_2dMatrix_SetToTranslation(HalfWidth,HalfHeight,&WorldToScreen);

	Pete_2dMatrix_Concatenate(&ScreenToWorld,&PanTransform,poutTransform);
	Pete_2dMatrix_Concatenate(poutTransform,&RotationTransform,poutTransform);
	Pete_2dMatrix_Concatenate(poutTransform,&ScaleTransform,poutTransform);
	Pete_2dMatrix_Concatenate(poutTransform,&WorldToScreen,poutTransform);

}
