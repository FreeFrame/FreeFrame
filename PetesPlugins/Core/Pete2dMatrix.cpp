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

#include "Pete2dMatrix.h"

#include <math.h>

void Pete_2dMatrix_SetToIdentity(SPete_2dMatrix* pMatrix) {

	pMatrix->m[0][0]=1.0f;
	pMatrix->m[0][1]=0.0f;
	pMatrix->m[0][2]=0.0f;

	pMatrix->m[1][0]=0.0f;
	pMatrix->m[1][1]=1.0f;
	pMatrix->m[1][2]=0.0f;

	pMatrix->m[2][0]=0.0f;
	pMatrix->m[2][1]=0.0f;
	pMatrix->m[2][2]=1.0f;

}

void Pete_2dMatrix_SetToAxisScale(float ScaleX,float ScaleY,SPete_2dMatrix* poutResult) {

	poutResult->m[0][0]=ScaleX;
	poutResult->m[0][1]=0.0f;
	poutResult->m[0][2]=0.0f;

	poutResult->m[1][0]=0.0f;
	poutResult->m[1][1]=ScaleY;
	poutResult->m[1][2]=0.0f;

	poutResult->m[2][0]=0.0f;
	poutResult->m[2][1]=0.0f;
	poutResult->m[2][2]=1.0f;

}

void Pete_2dMatrix_SetToRotation(float Rotation,SPete_2dMatrix* poutResult) {

	const float CosRotation=cos(Rotation);
	const float SinRotation=sin(Rotation);

	poutResult->m[0][0]=CosRotation;
	poutResult->m[0][1]=-SinRotation;
	poutResult->m[0][2]=0.0f;

	poutResult->m[1][0]=SinRotation;
	poutResult->m[1][1]=CosRotation;
	poutResult->m[1][2]=0.0f;

	poutResult->m[2][0]=0.0f;
	poutResult->m[2][1]=0.0f;
	poutResult->m[2][2]=1.0f;


}

void Pete_2dMatrix_SetToTranslation(float TranslationX,float TranslationY,SPete_2dMatrix* poutResult) {

	poutResult->m[0][0]=1.0f;
	poutResult->m[0][1]=0.0f;
	poutResult->m[0][2]=TranslationX;

	poutResult->m[1][0]=0.0f;
	poutResult->m[1][1]=1.0f;
	poutResult->m[1][2]=TranslationY;

	poutResult->m[2][0]=0.0f;
	poutResult->m[2][1]=0.0f;
	poutResult->m[2][2]=1.0f;

}

void Pete_2dMatrix_SetToDirectionalScale(float NormalX,float NormalY,float Scale,SPete_2dMatrix* poutResult) {

	const float ScaleMinusOne=(Scale-1.0f);

	poutResult->m[0][0]=1.0f+(NormalX*NormalX*ScaleMinusOne);
	poutResult->m[0][1]=(NormalX*NormalY*ScaleMinusOne);
	poutResult->m[0][2]=0.0f;

	poutResult->m[1][0]=(NormalY*NormalX*ScaleMinusOne);
	poutResult->m[1][1]=1.0f+(NormalY*NormalY*ScaleMinusOne);
	poutResult->m[1][2]=0.0f;

	poutResult->m[2][0]=0.0f;
	poutResult->m[2][1]=0.0f;
	poutResult->m[2][2]=1.0f;

}

void Pete_2dMatrix_Concatenate(SPete_2dMatrix* pinFirst,SPete_2dMatrix* pinSecond,SPete_2dMatrix* poutResult) {

	SPete_2dMatrix TempResult;

	int nY=0;
	for (nY=0; nY<3; nY+=1) {

		int nX=0;
		for (nX=0; nX<3; nX+=1) {

			TempResult.m[nY][nX]=
				(pinSecond->m[nY][0]*pinFirst->m[0][nX])+
				(pinSecond->m[nY][1]*pinFirst->m[1][nX])+
				(pinSecond->m[nY][2]*pinFirst->m[2][nX]);

		}

	}

	*poutResult=TempResult;

}

void Pete_2dMatrix_TransformVector(SPete_2dVector* pinVector,SPete_2dMatrix* pinMatrix,SPete_2dVector* poutResult) {

	SPete_2dVector Result;

	Result.x=
		(pinMatrix->m[0][0]*pinVector->x)+
		(pinMatrix->m[0][1]*pinVector->y)+
		(pinMatrix->m[0][2]);

	Result.y=
		(pinMatrix->m[1][0]*pinVector->x)+
		(pinMatrix->m[1][1]*pinVector->y)+
		(pinMatrix->m[1][2]);

	*poutResult=Result;

}

