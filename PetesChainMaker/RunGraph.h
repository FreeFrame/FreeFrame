#ifndef INCLUDE_RUNGRAPH_H
#define INCLUDE_RUNGRAPH_H

#include "PeteHelpers.h"
#include "GraphTypes.h"

typedef struct SPete_RunTimeEnvironment {
	int m_nVideoWidth;
	int m_nVideoHeight;
	void* m_pInstanceData;
	float* m_pFFParams;
	SPete_PluginParameter* m_pParamsInfo;
	int m_nFFParamsCount;
	U32** m_ppInputs;
	int m_nInputsCount;
	U32* m_pOutput;
	U32* m_pBlankInput;
	SPete_GraphData* m_pGraph;
} SPete_RunTimeEnvironment;

void RunGraph_Init(SPete_GraphData* pGraph,int nWidth,int nHeight,SPete_RunTimeEnvironment* pEnvironment);
void RunGraph_DeInit(SPete_Node* pRootNode,SPete_RunTimeEnvironment* pEnvironment);

void RunGraph_Run(SPete_Node* pRootNode,SPete_RunTimeEnvironment* pEnvironment);

#endif // INCLUDE_RUNGRAPH_H