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

#include "Pete3dMatrix.h"

#include <math.h>

void Pete_3dMatrix_SetToIdentity(SPete_3dMatrix* pMatrix) {

	pMatrix->m[0][0]=1.0f;
	pMatrix->m[0][1]=0.0f;
	pMatrix->m[0][2]=0.0f;
	pMatrix->m[0][3]=0.0f;

	pMatrix->m[1][0]=0.0f;
	pMatrix->m[1][1]=1.0f;
	pMatrix->m[1][2]=0.0f;
	pMatrix->m[1][3]=0.0f;

	pMatrix->m[2][0]=0.0f;
	pMatrix->m[2][1]=0.0f;
	pMatrix->m[2][2]=1.0f;
	pMatrix->m[2][3]=1.0f;

	pMatrix->m[3][0]=0.0f;
	pMatrix->m[3][1]=0.0f;
	pMatrix->m[3][2]=1.0f;
	pMatrix->m[3][3]=1.0f;

}

void Pete_3dMatrix_SetToAxisScale(float ScaleX,float ScaleY,float ScaleZ,SPete_3dMatrix* poutResult) {

	poutResult->m[0][0]=ScaleX;
	poutResult->m[0][1]=0.0f;
	poutResult->m[0][2]=0.0f;
	poutResult->m[0][3]=0.0f;

	poutResult->m[1][0]=0.0f;
	poutResult->m[1][1]=ScaleY;
	poutResult->m[1][2]=0.0f;
	poutResult->m[1][3]=0.0f;

	poutResult->m[2][0]=0.0f;
	poutResult->m[2][1]=0.0f;
	poutResult->m[2][2]=ScaleZ;
	poutResult->m[2][3]=0.0f;

	poutResult->m[2][0]=0.0f;
	poutResult->m[2][1]=0.0f;
	poutResult->m[2][2]=1.0f;
	poutResult->m[2][3]=1.0f;

}

void Pete_3dMatrix_Concatenate(SPete_3dMatrix* pinFirst,SPete_3dMatrix* pinSecond,SPete_3dMatrix* poutResult) {

	SPete_3dMatrix TempResult;

	int nY=0;
	for (nY=0; nY<4; nY+=1) {

		int nX=0;
		for (nX=0; nX<4; nX+=1) {

			TempResult.m[nY][nX]=
				(pinSecond->m[nY][0]*pinFirst->m[0][nX])+
				(pinSecond->m[nY][1]*pinFirst->m[1][nX])+
				(pinSecond->m[nY][2]*pinFirst->m[2][nX]);

		}

	}

	*poutResult=TempResult;

}

void Pete_3dMatrix_TransformVector(SPete_3dVector* pinVector,SPete_3dMatrix* pinMatrix,SPete_3dVector* poutResult) {

	SPete_3dVector Result;

	Result.x=
		(pinMatrix->m[0][0]*pinVector->x)+
		(pinMatrix->m[0][1]*pinVector->y)+
		(pinMatrix->m[0][2]*pinVector->z)+
		(pinMatrix->m[0][3]);

	Result.y=
		(pinMatrix->m[1][0]*pinVector->x)+
		(pinMatrix->m[1][1]*pinVector->y)+
		(pinMatrix->m[1][2]*pinVector->z)+
		(pinMatrix->m[1][3]);

	Result.z=
		(pinMatrix->m[2][0]*pinVector->x)+
		(pinMatrix->m[2][1]*pinVector->y)+
		(pinMatrix->m[2][2]*pinVector->z)+
		(pinMatrix->m[2][3]);

	*poutResult=Result;

}

