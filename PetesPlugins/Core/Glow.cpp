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

#include "Glow.h"

#include "math.h"

#include "PeteHelpers.h"
#include "BoxFilter.h"
#include "Levels.h"
#include "ImageMath.h"

static SPete_Parameter g_Parameters[]={
	{
		"Inner Radius",
		5.0f,
		1.0f,
		255.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Outer Radius",
		15.0f,
		1.0f,
		255.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Passes",
		1.0f,
		1.0f,
		10.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Floor",
		0.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Ceiling",
		32.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Aspect Ratio",
		1.0f,
		0.0f,
		2.0f,
		PETE_PARAM_FLOAT
	},
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_Glow_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_Glow_Init(SPete_Glow_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_Glow_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	const int nChannels=3;
	const int nSizeOfIntermediateBuffer=
		nWidth*nHeight*nChannels*sizeof(float);

	pInstanceData->hIntermediateBuffer=
		Pete_NewHandle(nSizeOfIntermediateBuffer);

	if (pInstanceData->hIntermediateBuffer==NULL) {
		Pete_Glow_DeInit(pInstanceData);
		return 0;
	}

	pInstanceData->hOuterBuffer=
		Pete_NewHandle(nSizeOfIntermediateBuffer);

	if (pInstanceData->hOuterBuffer==NULL) {
		Pete_Glow_DeInit(pInstanceData);
		return 0;
	}

	const int nLevelsInitResult=
		Pete_Levels_Init(&(pInstanceData->m_LevelsInstanceData),
		nWidth,
		nHeight);

	if (nLevelsInitResult==0) {
		Pete_Glow_DeInit(pInstanceData);
		return 0;
	}

	return 1;

}

void Pete_Glow_DeInit(SPete_Glow_Data* pInstanceData) {

	if (pInstanceData->hIntermediateBuffer!=NULL) {
		Pete_FreeHandle(pInstanceData->hIntermediateBuffer);
		pInstanceData->hIntermediateBuffer=NULL;
	}

	if (pInstanceData->hOuterBuffer!=NULL) {
		Pete_FreeHandle(pInstanceData->hOuterBuffer);
		pInstanceData->hOuterBuffer=NULL;
	}

	Pete_Levels_DeInit(&(pInstanceData->m_LevelsInstanceData));

}

void Pete_Glow_Render(SPete_Glow_Data* pInstanceData,SPete_Glow_Settings* pSettings,U32* pSource,U32* pOutput) {

	U32* pIntermediateBuffer=
		(U32*)Pete_LockHandle(pInstanceData->hIntermediateBuffer);
	if (pIntermediateBuffer==NULL) {
		return;
	}

	U32* pOuterBuffer=
		(U32*)Pete_LockHandle(pInstanceData->hOuterBuffer);
	if (pOuterBuffer==NULL) {
		return;
	}

	const float AspectRatio=pSettings->m_AspectRatio;
	const float BaseInnerRadius=pSettings->m_InnerRadius;
	const float BaseOuterRadius=pSettings->m_OuterRadius;

	float InnerRadiusX;
	float InnerRadiusY;
	float OuterRadiusX;
	float OuterRadiusY;

	if (AspectRatio<1.0f) {

		InnerRadiusX=BaseInnerRadius;
		InnerRadiusY=(BaseInnerRadius*AspectRatio);

		OuterRadiusX=BaseOuterRadius;
		OuterRadiusY=(BaseOuterRadius*AspectRatio);

	} else {

		InnerRadiusX=(BaseInnerRadius*(2.0f-AspectRatio));
		InnerRadiusY=BaseInnerRadius;

		OuterRadiusX=(BaseOuterRadius*(2.0f-AspectRatio));
		OuterRadiusY=BaseOuterRadius;

	}

	const int nInnerBoxFilterSizeX=
		(int)(ceilf(InnerRadiusX));

	const int nInnerBoxFilterSizeY=
		(int)(ceilf(InnerRadiusY));

	const int nOuterBoxFilterSizeX=
		(int)(ceilf(OuterRadiusX));

	const int nOuterBoxFilterSizeY=
		(int)(ceilf(OuterRadiusY));

	const int nMaxIterations=
		(int)(ceilf(pSettings->m_FakeItPasses));
		
	Pete_DoBoxFilterBlur(
		pSource,
		pIntermediateBuffer,
		pOutput,
		nInnerBoxFilterSizeX,
		nInnerBoxFilterSizeY,
		pInstanceData->nWidth,
		pInstanceData->nHeight,
		nMaxIterations);

	Pete_DoBoxFilterBlur(
		pSource,
		pIntermediateBuffer,
		pOuterBuffer,
		nOuterBoxFilterSizeX,
		nOuterBoxFilterSizeY,
		pInstanceData->nWidth,
		pInstanceData->nHeight,
		nMaxIterations);

	Pete_DoInPlaceSubtract(pOutput,pOuterBuffer,pInstanceData->nWidth,pInstanceData->nHeight);

	SPete_Levels_Settings LevelSettings;
	LevelSettings.m_DoAllowInversion=0.0f;
	LevelSettings.m_DoAuto=0.0f;
	LevelSettings.m_DoUniform=100.0f;
	LevelSettings.m_UniformInputFloor=pSettings->m_Floor;
	LevelSettings.m_UniformInputCeiling=pSettings->m_Ceiling;
	LevelSettings.m_UniformOutputFloor=0.0f;
	LevelSettings.m_UniformOutputCeiling=255.0f;

	Pete_Levels_Render(
		&(pInstanceData->m_LevelsInstanceData),
		&LevelSettings,
		pOutput,
		pOutput);
	
	Pete_DoInPlaceAddition(pOutput,pSource,pInstanceData->nWidth,pInstanceData->nHeight);
	
}

