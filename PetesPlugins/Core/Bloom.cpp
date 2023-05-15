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

#include "Bloom.h"

#include "math.h"

#include "PeteHelpers.h"
#include "BoxFilter.h"
#include "Levels.h"
#include "ImageMath.h"

static SPete_Parameter g_Parameters[]={
	{
		"Radius",
		15.0f,
		1.0f,
		255.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Passes",
		2.0f,
		1.0f,
		10.0f,
		PETE_PARAM_FLOAT
	},
	{
		"Floor",
		240.0f,
		0.0f,
		255.0f,
		PETE_PARAM_INT
	},
	{
		"Ceiling",
		248.0f,
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
	{
		"Boost Light",
		1.0f,
		0.0f,
		10.0f,
		PETE_PARAM_FLOAT
	}
};
static int g_nParametersCount=sizeof(g_Parameters)/sizeof(g_Parameters[0]);

void Pete_Bloom_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=&g_Parameters[0];
	*poutnParametersCount=g_nParametersCount;
}

int Pete_Bloom_Init(SPete_Bloom_Data* pInstanceData,int nWidth, int nHeight) {

	Pete_Bloom_DeInit(pInstanceData);

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	const int nChannels=3;
	const int nSizeOfIntermediateBuffer=
		nWidth*nHeight*nChannels*sizeof(float);

	pInstanceData->hIntermediateBuffer=
		Pete_NewHandle(nSizeOfIntermediateBuffer);

	if (pInstanceData->hIntermediateBuffer==NULL) {
		Pete_Bloom_DeInit(pInstanceData);
		return 0;
	}

	const int nLevelsInitResult=
		Pete_Levels_Init(&(pInstanceData->m_LevelsInstanceData),
		nWidth,
		nHeight);

	if (nLevelsInitResult==0) {
		Pete_Bloom_DeInit(pInstanceData);
		return 0;
	}

	return 1;

}

void Pete_Bloom_DeInit(SPete_Bloom_Data* pInstanceData) {

	if (pInstanceData->hIntermediateBuffer!=NULL) {
		Pete_FreeHandle(pInstanceData->hIntermediateBuffer);
		pInstanceData->hIntermediateBuffer=NULL;
	}

	Pete_Levels_DeInit(&(pInstanceData->m_LevelsInstanceData));

}

void Pete_Bloom_Render(SPete_Bloom_Data* pInstanceData,SPete_Bloom_Settings* pSettings,U32* pSource,U32* pOutput) {

	U32* pIntermediateBuffer=
		(U32*)Pete_LockHandle(pInstanceData->hIntermediateBuffer);
	if (pIntermediateBuffer==NULL) {
		return;
	}

	const float AspectRatio=pSettings->m_AspectRatio;
	const float BaseRadius=pSettings->m_Radius;
	const float LightBoost=pSettings->m_BoostLight;

	float RadiusX;
	float RadiusY;
	if (AspectRatio<1.0f) {
		RadiusX=BaseRadius;
		RadiusY=(BaseRadius*AspectRatio);
	} else {
		RadiusX=(BaseRadius*(2.0f-AspectRatio));
		RadiusY=BaseRadius;
	}

	const int nBoxFilterSizeX=
		(int)(ceilf(RadiusX));

	const int nBoxFilterSizeY=
		(int)(ceilf(RadiusY));

	const int nMaxIterations=
		(int)(ceilf(pSettings->m_FakeItPasses));

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
		pSource,
		pOutput);

	Pete_DoBoxFilterBlur(
		pOutput,
		pIntermediateBuffer,
		pOutput,
		nBoxFilterSizeX,
		nBoxFilterSizeY,
		pInstanceData->nWidth,
		pInstanceData->nHeight,
		1);

	if (LightBoost!=1.0f) {

		Pete_DoInPlaceAdditionScaled(pOutput,pOutput,pInstanceData->nWidth,pInstanceData->nHeight,(LightBoost-1.0f));

	}

	Pete_DoBoxFilterBlur(
		pOutput,
		pIntermediateBuffer,
		pOutput,
		nBoxFilterSizeX,
		nBoxFilterSizeY,
		pInstanceData->nWidth,
		pInstanceData->nHeight,
		1);

#ifdef PETE_NATIVE_AE

	Pete_DoInPlaceAdditionReplaceAlpha(pOutput,pSource,pInstanceData->nWidth,pInstanceData->nHeight);

#else // PETE_NATIVE_AE

	Pete_DoInPlaceAddition(pOutput,pSource,pInstanceData->nWidth,pInstanceData->nHeight);

#endif // PETE_NATIVE_AE
	Pete_UnLockHandle(pInstanceData->hIntermediateBuffer);

}

