#ifndef INCLUDE_GRAPHTYPES_H
#define INCLUDE_GRAPHTYPES_H

#include "PeteHelpers.h"
#include "FreeFrame.h"
#include "Windows.h"

enum EPete_AllocationType {
	eAlloc_Heap,
	eAlloc_Flattened,
	eAlloc_UnFlattened,
};

#define PETE_MAX_PARAMS 32
#define PETE_MAX_SWITCHER_INPUTS (64)

typedef struct SPete_PluginParameter {
	char m_pName[16];
	float m_Default;
	U32 m_ExternalType;
} SPete_PluginParameter;

typedef struct SPete_PluginInfo {
	char m_pDLLPath[MAX_PATH];
	int m_nInputCount;
	int m_nParameterCount;
	FF_Main_FuncPtr m_pMainFunc;
	HMODULE m_hModuleHandle;
	char m_pName[20];
	int m_nReferenceCount;
	U32 m_FlattenedInfoOffset;
	U32 m_FlattenedDLLImageOffset;
	U32 m_FlattenedDLLImageSize;
	SPete_PluginParameter m_pParameterInfo[PETE_MAX_PARAMS];
} SPete_PluginInfo;

typedef struct SPete_UIPos {
	int nX;
	int nY;
} SPete_UIPos;

enum EPete_NodeType {
	eType_Effect_Input,
	eType_Effect_Output,
	eType_Effect_Plugin,
	eType_Effect_Switcher,
	eType_Parameter_External,
	eType_Parameter_Constant,
	eType_Parameter_Keyboard,
	eType_Parameter_Midi,
};

struct SPete_ParameterNode;

typedef struct SPete_Node {
	SPete_UIPos m_Pos;
	SPete_UIPos m_Size;
	EPete_NodeType m_eType;
	struct SPete_ParameterNode** m_ppParameters;
	U32 m_LastVisitTime;
	EPete_AllocationType m_AllocationType;
} SPete_Node;

typedef struct SPete_EffectNode : public SPete_Node {
	SPete_PluginInfo* m_pPluginInfo;
	struct SPete_EffectNode** m_ppInputs;
	int m_nInputIndex;
	U32 m_InstanceCookie;
	U32* m_pInternalOutputBuffer;
	U32* m_pOutput;
	int m_nSwitcherInputCount;
	U32* m_pExternalOutputBuffer;
	U32 m_Reserved[7];
} SPete_EffectNode;

typedef struct SPete_ParameterNode : public SPete_Node {
	int m_nParametersCount;
	float m_OutputValue;
	int m_nExternalIndex;
	char m_pExternalLabel[16];
	float m_ExternalDefault;
	U32 m_ExternalType;
	U32 m_Reserved[7];
} SPete_ParameterNode;

typedef struct SPete_NodeListEntry {
	SPete_Node* m_pNode;
	struct SPete_NodeListEntry* m_pNext;
	U32 m_FlattenedOffset;
} SPete_NodeListEntry;

typedef struct SPete_GraphData {
	int m_nStructSize;
	SPete_NodeListEntry* m_pNodeListHead;
	EPete_AllocationType m_PluginInfoAllocType;
	SPete_PluginInfo* m_pPluginInfo;
	SPete_Node* m_pRootNode;
	int m_nPluginInfoCount;
	char* m_pFlattenedData;
	int m_nFlattenedDataSize;
	bool m_bIsValid;
	char m_pName[32];
	char m_pUniqueID[5];
	char m_pDescription[1024];
	bool m_bStorePluginCopies;
	U32 m_Reserved[16];
} SPete_GraphData;

#endif // INCLUDE_GRAPHTYPES_H