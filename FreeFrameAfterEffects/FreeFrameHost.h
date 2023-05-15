/*******************************************************************/
/*                                                                 */
/*                      ADOBE CONFIDENTIAL                         */
/*                   _ _ _ _ _ _ _ _ _ _ _ _ _                     */
/*                                                                 */
/* Copyright 2001 Adobe Systems Incorporated                       */
/* All Rights Reserved.                                            */
/*                                                                 */
/* NOTICE:  All information contained herein is, and remains the   */
/* property of Adobe Systems Incorporated and its suppliers, if    */
/* any.  The intellectual and technical concepts contained         */
/* herein are proprietary to Adobe Systems Incorporated and its    */
/* suppliers and may be covered by U.S. and Foreign Patents,       */
/* patents in process, and are protected by trade secret or        */
/* copyright law.  Dissemination of this information or            */
/* reproduction of this material is strictly forbidden unless      */
/* prior written permission is obtained from Adobe Systems         */
/* Incorporated.                                                   */
/*                                                                 */
/*******************************************************************/

/*
	SDK_Noise.h

	Part of the Adobe After Effects 5.5 SDK.
	Copyright 2001 Adobe Systems Incorporated.
	All Rights Reserved.


*/

#pragma once
#ifndef SDK_NOISE_H
#define SDK_NOISE_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "entry.h"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_EffectCBSuites.h"
#include "AE_Macros.h"
#include "PF_Suite_Helper.h"
#include "String_Utils.h"
#include "Param_Utils.h"

#ifdef MSWindows
#include <Windows.h>
#define DESCRIPTION	"\nCopyright 2003 Pete Warden.\nFreeFrame Host"

#else
#include <Quickdraw.h>
#define DESCRIPTION "\rCopyright 2003 Pete Warden.\rFreeFrame Host"
#endif

#define PLUGINDIR_PREFIX "c:\\Program Files\\Adobe\\After Effects 5.5\\Plug-ins\\"

#define NAME			"FreeFrameHost"
#define	MAJOR_VERSION	1
#define	MINOR_VERSION	4
#define	BUG_VERSION		0
#define	STAGE_VERSION	PF_Stage_DEVELOP
#define	BUILD_VERSION	0

enum {
	FILTER_INPUT = 0,
	FILTER_EFFECT,
	FILTER_SUPERSAMPLE,
	FILTER_PARAM0,
	FILTER_PARAM1,
	FILTER_PARAM2,
	FILTER_PARAM3,
	FILTER_PARAM4,
	FILTER_PARAM5,
	FILTER_PARAM6,
	FILTER_PARAM7,
	FILTER_PARAM8,
	FILTER_PARAM9,
	FILTER_PARAM10,
	FILTER_PARAM11,
	FILTER_PARAM12,
	FILTER_PARAM13,
	FILTER_PARAM14,
	FILTER_PARAM15,
	FILTER_NUM_PARAMS
};

enum {
	POPUP_EFFECT_ID = 1,
	POPUP_SUPERSAMPLE_ID = 2,
	SLIDER_PARAM0_ID = 3,
	SLIDER_PARAM1_ID = 4,
	SLIDER_PARAM2_ID = 5,
	SLIDER_PARAM3_ID = 6,
	SLIDER_PARAM4_ID = 7,
	SLIDER_PARAM5_ID = 8,
	SLIDER_PARAM6_ID = 9,
	SLIDER_PARAM7_ID = 10,
	SLIDER_PARAM8_ID = 11,
	SLIDER_PARAM9_ID = 12,
	SLIDER_PARAM10_ID = 13,
	SLIDER_PARAM11_ID = 14,
	SLIDER_PARAM12_ID = 15,
	SLIDER_PARAM13_ID = 16,
	SLIDER_PARAM14_ID = 17,
	SLIDER_PARAM15_ID = 18
};

const int nFreeFrameParamCount=16;

#define FILTER_NOISE_MIN	0
#define FILTER_NOISE_MAX	10000
#define FILTER_NOISE_DFLT	50
#define SLIDER_MIN			0
#define	SLIDER_MAX			1000

#define RESTRICT_BOUNDS			0
#define FIX_SLIDER_PRECISION	1
#define DISPLAY_FLAGS			1

typedef struct {

	PF_ProgPtr	ref;
	float		mean;
	float		var;
	PF_SampPB	samp_pb;
	PF_InData	*in_data;
	PF_World*	pSource;
	PF_UtilCallbacks* pUtils;

} FilterInfo;

DllExport
PF_Err
main (
	PF_Cmd			cmd,
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output,
	void*			pExtra);

static PF_Err
About (
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output );

static PF_Err
GlobalSetup (
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output );

static PF_Err
ParamsSetup (
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output);

static PF_Err
Render (
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output );

static PF_Err
FilterImage (
	long		refcon,
	long		x,
	long		y,
	PF_Pixel	*in,
	PF_Pixel	*out);

#endif // SDK_NOISE_H