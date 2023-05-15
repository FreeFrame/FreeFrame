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

#include "ColourConversion.h"
#include "ImageMath.h"

#include "math.h"

#include "PeteHelpers.h"

U32 Pete_ConvertRGBToHSV(U32 ColourRGB) {

	const int cnFixedShift=8;
	const int cnFixedPrec=(1<<cnFixedShift);

	const int nSourceRed=(ColourRGB&(0xff<<SHIFT_RED))>>SHIFT_RED;
	const int nSourceGreen=(ColourRGB&(0xff<<SHIFT_GREEN))>>SHIFT_GREEN;
	const int nSourceBlue=(ColourRGB&(0xff<<SHIFT_BLUE))>>SHIFT_BLUE;

	int iMax, iMin;
	if (nSourceRed > nSourceGreen) {
		iMax = nSourceRed;
		iMin = nSourceGreen;
	} else {
		iMin = nSourceRed;
		iMax = nSourceGreen;
	}

	if (nSourceBlue > iMax)
		iMax = nSourceBlue;

	if (nSourceBlue < iMin)
		iMin = nSourceBlue;

	int nValue = iMax;

	int nSaturation;
	if (iMax != 0) {
		nSaturation = 256 * (iMax - iMin) / iMax;
	} else {
		nSaturation = 0;
	}

	int nHue;
	if (nSaturation == 0) {
		nHue = 0;
	} else {
		int xHue;
		int nDelta;

		nDelta = (iMax - iMin);

		if (nSourceRed == iMax) {
			xHue = ((nSourceGreen - nSourceBlue)<<cnFixedShift) / nDelta;
		} else if (nSourceGreen == iMax) {
			xHue = (2<<cnFixedShift) + (((nSourceBlue - nSourceRed)<<cnFixedShift) / nDelta);
		} else {
			xHue = (4<<cnFixedShift) + (((nSourceRed - nSourceGreen)<<cnFixedShift) / nDelta);
		}

		xHue *= 64;

		if (xHue < 0) {
			xHue += ((64*6)<<cnFixedShift);
		}

		nHue = xHue>>cnFixedShift;

	}

	nHue=(nHue*2)/3;

	U32 ResultHSV;

	ResultHSV=(nHue<<SHIFT_RED)|(nSaturation<<SHIFT_GREEN)|(nValue<<SHIFT_BLUE);

	return ResultHSV;

}

U32 Pete_ConvertHSVToRGB(U32 ColourHSV) {

	const int cnFixedShift=8;
	const int cnFixedPrec=(1<<cnFixedShift);

	const int nHue=(((ColourHSV>>SHIFT_RED)&0xff)*3)/2;
	const int nSaturation=(ColourHSV>>SHIFT_GREEN)&0xff;
	const int nValue=(ColourHSV>>SHIFT_BLUE)&0xff;

	const int xHueSextant = (nHue*cnFixedPrec) / 64;
	const int nIntSextant = (xHueSextant>>cnFixedShift);
	const int xf = xHueSextant%cnFixedPrec;;
	const int xValue = (nValue<<(cnFixedShift-8));
	const int xSaturation = (nSaturation<<(cnFixedShift-8));

	const int xp = (xValue * (cnFixedPrec - xSaturation))>>cnFixedShift;
	const int xq = (xValue * (cnFixedPrec -
		((xSaturation * xf)>>cnFixedShift)))>>cnFixedShift;
	const int xt = (xValue * (cnFixedPrec -
		((xSaturation * (cnFixedPrec - xf))>>cnFixedShift) ))>>cnFixedShift;

	int nOutputRed;
	int nOutputGreen;
	int nOutputBlue;

	switch (nIntSextant) {
		case 0:
		nOutputRed = xValue>>(cnFixedShift-8);
		nOutputGreen = xt>>(cnFixedShift-8);
		nOutputBlue = xp>>(cnFixedShift-8);
		break;

		case 1:
		nOutputRed = xq>>(cnFixedShift-8);
		nOutputGreen = xValue>>(cnFixedShift-8);
		nOutputBlue = xp>>(cnFixedShift-8);
		break;

		case 2:
		nOutputRed = xp>>(cnFixedShift-8);
		nOutputGreen = xValue>>(cnFixedShift-8);
		nOutputBlue = xt>>(cnFixedShift-8);
		break;

		case 3:
		nOutputRed = xp>>(cnFixedShift-8);
		nOutputGreen = xq>>(cnFixedShift-8);
		nOutputBlue = xValue>>(cnFixedShift-8);
		break;

		case 4:
		nOutputRed = xt>>(cnFixedShift-8);
		nOutputGreen = xp>>(cnFixedShift-8);
		nOutputBlue = xValue>>(cnFixedShift-8);
		break;

		case 5:
		nOutputRed = xValue>>(cnFixedShift-8);
		nOutputGreen = xp>>(cnFixedShift-8);
		nOutputBlue = xq>>(cnFixedShift-8);
		break;
	}

	U32 ResultRGB;

	ResultRGB=(nOutputRed<<SHIFT_RED)|(nOutputGreen<<SHIFT_GREEN)|(nOutputBlue<<SHIFT_BLUE);

	return ResultRGB;

}
