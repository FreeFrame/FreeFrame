#ifndef INCLUDE_PETE3DMATRIX_H
#define INCLUDE_PETE3DMATRIX_H

#include <math.h>

typedef struct SPete_3dMatrix {
	float m[4][4];
} SPete_3dMatrix;

typedef struct SPete_3dVector {
	float x;
	float y;
	float z;
} SPete_3dVector;

void Pete_3dMatrix_SetToIdentity(SPete_3dMatrix* pMatrix);
void Pete_3dMatrix_SetToAxisScale(float ScaleX,float ScaleY,SPete_3dMatrix* poutResult);

void Pete_3dMatrix_Concatenate(SPete_3dMatrix* pinFirst,SPete_3dMatrix* pinSecond,SPete_3dMatrix* poutResult);

void Pete_3dMatrix_TransformVector(SPete_3dVector* pinVector,SPete_3dMatrix* pinMatrix,SPete_3dVector* poutResult);

inline void Pete_3dMatrix_TransformVectorList(SPete_3dVector* pinVectors,int nVectorCount,SPete_3dMatrix* pinMatrix,SPete_3dVector* poutResults) {
	int nCount;
	for (nCount=0; nCount<nVectorCount; nCount+=1) {
		Pete_3dMatrix_TransformVector(&pinVectors[nCount],pinMatrix,&poutResults[nCount]);
	}
}


inline float Pete_3dVector_DotProduct(SPete_3dVector* pinA,SPete_3dVector* pinB) {
	return (pinA->x*pinB->x)+(pinA->y*pinB->y)+(pinA->z*pinB->z);
}

inline void Pete_3dVector_Add(SPete_3dVector* pinA,SPete_3dVector* pinB,SPete_3dVector* poutResult) {
	poutResult->x=(pinA->x+pinB->x);
	poutResult->y=(pinA->y+pinB->y);
	poutResult->z=(pinA->z+pinB->z);
}

inline void Pete_3dVector_Subtract(SPete_3dVector* pinA,SPete_3dVector* pinB,SPete_3dVector* poutResult) {
	poutResult->x=(pinA->x-pinB->x);
	poutResult->y=(pinA->y-pinB->y);
	poutResult->z=(pinA->z-pinB->z);
}

inline void Pete_3dVector_Scale(SPete_3dVector* pinA,float Scale,SPete_3dVector* poutResult) {
	poutResult->x=(pinA->x*Scale);
	poutResult->y=(pinA->y*Scale);
	poutResult->z=(pinA->z*Scale);
}

inline float Pete_3dVector_Magnitude(SPete_3dVector* pinA) {
	return sqrtf(Pete_3dVector_DotProduct(pinA,pinA));
}

inline void Pete_3dVector_Normalize(SPete_3dVector* pinA) {
	const float Magnitude=Pete_3dVector_Magnitude(pinA);
	const float RecipMagnitude=(1.0f/Magnitude);
	Pete_3dVector_Scale(pinA,RecipMagnitude,pinA);
}

#endif // INCLUDE_PETE3DMATRIX_H