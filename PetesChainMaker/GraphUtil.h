#ifndef INCLUDE_GRAPHUTIL_H
#define INCLUDE_GRAPHUTIL_H

#include "GraphTypes.h"

typedef void (*Iterate_FuncPtr)(SPete_Node* pNode,void* pCookie);

void GraphUtil_Init(SPete_GraphData* pGraph);
void GraphUtil_InitFromFlat(SPete_GraphData* pGraph,char* pFlatData,int nFlatDataSize);
void GraphUtil_DeInit(SPete_GraphData* pGraph);

bool GraphUtil_DoLoadPlugin(SPete_GraphData* pGraph,SPete_PluginInfo* pPluginInfo);
void GraphUtil_DoUnLoadPlugin(SPete_PluginInfo* pPluginInfo);
void GraphUtil_FlattenGraph(SPete_GraphData* pGraph);
void GraphUtil_AttachInput(SPete_EffectNode* pEffectNode,int nIndex,SPete_EffectNode* pInputNode);
void GraphUtil_AttachParameter(SPete_Node* pNode,int nIndex,SPete_ParameterNode* pInputNode);
SPete_Node* GraphUtil_MakeOutputNode(SPete_GraphData* pGraph,int nPosX,int nPosY);
SPete_Node* GraphUtil_MakePluginNode(SPete_GraphData* pGraph,SPete_PluginInfo* pPluginInfo,int nPosX,int nPosY);
SPete_Node* GraphUtil_MakeInputNode(SPete_GraphData* pGraph,int nInputIndex,int nPosX,int nPosY);
SPete_Node* GraphUtil_MakeSwitcherNode(SPete_GraphData* pGraph,int nSwitcherInputCount,int nPosX,int nPosY);
SPete_Node* GraphUtil_MakeExternalParamNode(SPete_GraphData* pGraph,int nIndex,char* pName,float Default,U32 Type,int nPosX,int nPosY);
int GraphUtil_GetInputCount(SPete_Node* pNode);
int GraphUtil_GetParameterCount(SPete_Node* pNode);
void GraphUtil_FreeFlattenedData(SPete_GraphData* pGraph);
void GraphUtil_GetEffectParamInfo(SPete_Node* pNode,int nIndex,SPete_PluginParameter* poutParamInfo);
SPete_NodeListEntry* GraphUtil_FindListEntry(SPete_GraphData* pGraph,SPete_Node* pNode);
SPete_PluginInfo* GraphUtil_AddPluginInfo(SPete_GraphData* pGraph,char* pFileName);
void GraphUtil_Iterate(SPete_Node* pNode,Iterate_FuncPtr pfnAction,void* pCookie);
void GraphUtil_FindHighestFFParameter(SPete_Node* pNode,void* pCookie);
void GraphUtil_DeleteNode(SPete_GraphData* pGraph,SPete_Node* pNode);
bool GraphUtil_HasParameterOutput(SPete_Node* pNode);
bool GraphUtil_HasImageOutput(SPete_Node* pNode);
void GraphUtil_PrunedIterate(SPete_Node* pNode,Iterate_FuncPtr pfnAction,void* pCookie);

#endif // INCLUDE_GRAPHUTIL_H