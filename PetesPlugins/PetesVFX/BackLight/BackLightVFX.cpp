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

#include "BackLight.h"

/*-------------------------------------------------*/
#ifndef PETE_NATIVE_AE

static PlugInfoStruct g_PluginInfo={
	PETE_FF_MAJORVERSION,
	PETE_FF_MINORVERSION,
	{'B','K','L','T'},
	"BackLight",
	0
};
#endif // PETE_NATIVE_AE

/*-------------------------------------------------*/

#define PLUGIN_NAMESTRING "BackLight"

#define PLUGIN_SETTINGS SPete_BackLight_Settings
#define PLUGIN_DATA SPete_BackLight_Data

#define PLUGIN_GETPARAMETERS Pete_BackLight_GetParameters
#define PLUGIN_INIT Pete_BackLight_Init
#define PLUGIN_DEINIT Pete_BackLight_DeInit
#define PLUGIN_RENDER Pete_BackLight_Render

#define PLUGIN_HANDLES_ALPHA

#include "PeteWrapperEpilogue.cpp"
