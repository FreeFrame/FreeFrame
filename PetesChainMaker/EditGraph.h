#ifndef INCLUDE_PETESCHAINMAKER_H
#define INCLUDE_PETESCHAINMAKER_H

#include <windows.h>

#include "PeteHelpers.h"
#include "FreeFrame.h"

#include "GraphTypes.h"

enum EEditGraph_UIEvent {
	eEvent_Mouse_LDown,
	eEvent_Mouse_LUp,
	eEvent_Mouse_LDoubleClick,
	eEvent_Mouse_RDown,
	eEvent_Mouse_Move,
};

enum EEditGraph_SelectionType {
	eSelect_Body,
	eSelect_Input,
	eSelect_Parameter_Input,
	eSelect_Parameter_Output,
	eSelect_Output,
	eSelect_Connection,
};

typedef struct SPete_UISelection {
	SPete_NodeListEntry* m_pNodeEntry;
	EEditGraph_SelectionType m_eType;
	int m_nIndex;
	SPete_UIPos m_DragOffset;
	SPete_UIPos m_OriginalDrawOrigin;
	SPete_UIPos m_ConnectionEnd;
} SPete_UISelection;

typedef RECT SPete_UIRect;

typedef struct SPete_UILine {
	SPete_UIPos Start;
	SPete_UIPos End;
} SPete_UILine;

enum EPete_TextJustifaction {
	eJustify_Centre,
	eJustify_Left,
	eJustify_Right,
};

extern HINSTANCE g_hWindowInstance;
extern HWND g_hWindow;

void EditGraph_SaveDataFile(SPete_GraphData* pGraph,char* pFileName);
void EditGraph_SaveAsFreeFrameDLL(SPete_GraphData* pGraph,char* pFileName);
void EditGraph_DrawAll(HWND hWindow,HDC hDeviceContext,RECT* pClientRect,SPete_GraphData* pGraph);
void EditGraph_RunGraph(SPete_GraphData* pGraph);
void EditGraph_LoadFreeFrameDLL(SPete_GraphData* pGraph,char* pFileName);

void EditGraph_InitUI(SPete_GraphData* pGraph);
void EditGraph_DeInitUI(SPete_GraphData* pGraph);
bool EditGraph_UIEvent(SPete_GraphData* pGraph,EEditGraph_UIEvent eEvent,int nPosX,int nPosY,void* pExtraData);

#endif // INCLUDE_PETESCHAINMAKER_H