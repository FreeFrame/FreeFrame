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

#include "PeteWrapperPrologue.cpp"

#include "Mixer.h"

/*-------------------------------------------------*/

#ifndef PETE_NATIVE_AE
static PlugInfoStruct g_PluginInfo={
	PETE_FF_MAJORVERSION,
	PETE_FF_MINORVERSION,
	{'L','K','E','Y'},
	"Mixer",
	0
};
#endif // PETE_NATIVE_AE

/*-------------------------------------------------*/

#define PLUGIN_NAMESTRING "Mixer"

#define PLUGIN_SETTINGS SPete_Mixer_Settings
#define PLUGIN_DATA SPete_Mixer_Data

#define PLUGIN_GETPARAMETERS Pete_Mixer_GetParameters
#define PLUGIN_INIT Pete_Mixer_Init
#define PLUGIN_DEINIT Pete_Mixer_DeInit
#define PLUGIN_RENDER Pete_Mixer_Render

#define PLUGIN_INPUT_COUNT (2)

#include "PeteWrapperEpilogue.cpp"
