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

#include "Levels.h"

/*-------------------------------------------------*/

#ifndef PETE_NATIVE_AE
static struct PlugInfoStruct g_PluginInfo={
	PETE_FF_MAJORVERSION,
	PETE_FF_MINORVERSION,
	{'L','V','L','S'},
	"Levels",
	0
};
#endif // PETE_NATIVE_AE

/*-------------------------------------------------*/

#define PLUGIN_NAMESTRING "Levels"

#define PLUGIN_SETTINGS SPete_Levels_Settings
#define PLUGIN_DATA SPete_Levels_Data

#define PLUGIN_GETPARAMETERS Pete_Levels_GetParameters
#define PLUGIN_INIT Pete_Levels_Init
#define PLUGIN_DEINIT Pete_Levels_DeInit
#define PLUGIN_RENDER Pete_Levels_Render

#include "PeteWrapperEpilogue.cpp"