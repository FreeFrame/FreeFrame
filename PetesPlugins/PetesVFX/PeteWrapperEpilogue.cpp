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

#ifdef PETE_NATIVE_AE

#include "AETemplate.cpp"

#else // PETE_NATIVE_AE

#ifdef PETE_MAC_OSX
#include "FreeFrameTemplate.cpp"
#else // PETE_MAC_OSX
#include "..\FreeFrameTemplate.cpp"
#endif // PETE_MAC_OSX

#ifndef PETE_MAC_OSX
#include "..\TZTTemplate.cpp"
#endif // PETE_MAC_OSX

#endif // PETE_NATIVE_AE