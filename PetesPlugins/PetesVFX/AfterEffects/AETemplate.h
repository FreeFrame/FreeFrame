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
#define DESCRIPTION	"\nCopyright 2003 Pete Warden.\n"PLUGIN_NAMESTRING"\nSee http://www.petewarden.com for more..."

#else
#include <Quickdraw.h>
#define DESCRIPTION "\rCopyright 2003 Pete Warden.\r"PLUGIN_NAMESTRING"\rSee http://www.petewarden.com for more..."
#endif

#define NAME			PLUGIN_NAMESTRING
#define	MAJOR_VERSION	1
#define	MINOR_VERSION	4
#define	BUG_VERSION		0
#define	STAGE_VERSION	PF_Stage_DEVELOP
#define	BUILD_VERSION	0

#define SOURCELAYER_PARAMINDEX		(0)
#define SUPERSAMPLE_PARAMINDEX		(1)

#define OPACITY_PARAMOFFSET			(0)
#define TRANSFERMODE_PARAMOFFSET	(1)

#define POPUP_SUPERSAMPLE_ID		(1)

#define SLIDER_OPACITY_IDOFFSET		(0)
#define POPUP_TRANSFERMODE_IDOFFSET	(1)			

#define SUPERSAMPLE_SIZE_MAX		(4)
#define SUPERSAMPLE_SAMPLE_MAX		(SUPERSAMPLE_SIZE_MAX*SUPERSAMPLE_SIZE_MAX)

#define RESTRICT_BOUNDS			0
#define FIX_SLIDER_PRECISION	1
#define DISPLAY_FLAGS			1

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