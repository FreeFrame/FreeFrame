#ifndef INCLUDE_LIVEFEED_H
#define INCLUDE_LIVEFEED_H

#include "PeteHelpers.h"

struct SPete_LiveFeed_Settings {
};

struct SPete_LiveFeed_Data {
	int nWidth;
	int nHeight;
	int nDeviceIndex;
};

void Pete_LiveFeed_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount);
int Pete_LiveFeed_Init(SPete_LiveFeed_Data* pInstanceData,int nWidth, int nHeight,int nDeviceIndex);
void Pete_LiveFeed_DeInit(SPete_LiveFeed_Data* pInstanceData);
void Pete_LiveFeed_Render(SPete_LiveFeed_Data* pInstanceData,SPete_LiveFeed_Settings* pSettings,U32* pSource, U32* pOutput);

#endif // INCLUDE_LIVEFEED_H