#ifndef INCLUDE_PETE2DMATRIX_H
#define INCLUDE_PETE2DMATRIX_H

#include <math.h>

typedef struct SPete_2dMatrix {
	float m[3][3];
} SPete_2dMatrix;

typedef struct SPete_2dVector {
	float x;
	float y;
} SPete_2dVector;

void Pete_2dMatrix_SetToIdentity(SPete_2dMatrix* pMatrix);
void Pete_2dMatrix_SetToAxisScale(float ScaleX,float ScaleY,SPete_2dMatrix* poutResult);
void Pete_2dMatrix_SetToRotation(float Rotation,SPete_2dMatrix* poutResult);
void Pete_2dMatrix_SetToTranslation(float TranslationX,float TranslationY,SPete_2dMatrix* poutResult);
void Pete_2dMatrix_SetToDirectionalScale(float NormalX,float NormalY,float Scale,SPete_2dMatrix* poutResult);

void Pete_2dMatrix_Concatenate(SPete_2dMatrix* pinFirst,SPete_2dMatrix* pinSecond,SPete_2dMatrix* poutResult);

void Pete_2dMatrix_TransformVector(SPete_2dVector* pinVector,SPete_2dMatrix* pinMatrix,SPete_2dVector* poutResult);

inline void Pete_2dMatrix_TransformVectorList(SPete_2dVector* pinVectors,int nVectorCount,SPete_2dMatrix* pinMatrix,SPete_2dVector* poutResults) {
	int nCount;
	for (nCount=0; nCount<nVectorCount; nCount+=1) {
		Pete_2dMatrix_TransformVector(&pinVectors[nCount],pinMatrix,&poutResults[nCount]);
	}
}


inline float Pete_2dVector_DotProduct(SPete_2dVector* pinA,SPete_2dVector* pinB) {
	return (pinA->x*pinB->x)+(pinA->y*pinB->y);
}

inline void Pete_2dVector_Add(SPete_2dVector* pinA,SPete_2dVector* pinB,SPete_2dVector* poutResult) {
	poutResult->x=(pinA->x+pinB->x);
	poutResult->y=(pinA->y+pinB->y);
}

inline void Pete_2dVector_Subtract(SPete_2dVector* pinA,SPete_2dVector* pinB,SPete_2dVector* poutResult) {
	poutResult->x=(pinA->x-pinB->x);
	poutResult->y=(pinA->y-pinB->y);
}

inline void Pete_2dVector_Scale(SPete_2dVector* pinA,float Scale,SPete_2dVector* poutResult) {
	poutResult->x=(pinA->x*Scale);
	poutResult->y=(pinA->y*Scale);
}

inline float Pete_2dVector_Magnitude(SPete_2dVector* pinA) {
	return sqrt(Pete_2dVector_DotProduct(pinA,pinA));
}

#endif // INCLUDE_PETE2DMATRIX_H