#include "EditGraph.h"

#include <windows.h>
#include <stdio.h>

#include "resource.h"

#include "PetesChain.h"
#include "RunGraph.h"
#include "GraphUtil.h"

int EditGraph_GetChainBinByteCount(void);
bool EditGraph_FillChainBinData(char* pBinData);
bool EditGraph_CopyFile(char* pSourceFileName,char* pDestFileName);
void EditGraph_DrawNode(HWND hWindow,HDC hDeviceContext,SPete_Node* pNode);
void EditGraph_DrawOutputNode(HWND hWindow,HDC hDeviceContext,SPete_EffectNode* pEffectNode,SPete_UISelection* pSelection);
void EditGraph_DrawInputNode(HWND hWindow,HDC hDeviceContext,SPete_EffectNode* pEffectNode,SPete_UISelection* pSelection);
void EditGraph_DrawPluginNode(HWND hWindow,HDC hDeviceContext,SPete_EffectNode* pEffectNode,SPete_UISelection* pSelection);
void EditGraph_DrawSwitcherNode(HWND hWindow,HDC hDeviceContext,SPete_EffectNode* pEffectNode,SPete_UISelection* pSelection);
void EditGraph_DrawExternalParamNode(HWND hWindow,HDC hDeviceContext,SPete_ParameterNode* pParamNode,SPete_UISelection* pSelection);
void EditGraph_DrawConstantParamNode(HWND hWindow,HDC hDeviceContext,SPete_ParameterNode* pParamNode,SPete_UISelection* pSelection);
void EditGraph_DrawKeyboardParamNode(HWND hWindow,HDC hDeviceContext,SPete_ParameterNode* pParamNode,SPete_UISelection* pSelection);
void EditGraph_DrawMidiParamNode(HWND hWindow,HDC hDeviceContext,SPete_ParameterNode* pParamNode,SPete_UISelection* pSelection);
void EditGraph_DrawUnfilledBox(HDC hDeviceContext,int nPosX,int nPosY,int nWidth,int nHeight,U32 Colour,int nLineWidth);
void EditGraph_DrawFilledBox(HDC hDeviceContext,int nPosX,int nPosY,int nWidth,int nHeight,U32 Colour);
void EditGraph_DrawEffectConnections(HDC hDeviceContext,SPete_EffectNode* pEffectNode,SPete_UISelection* pSelection);
void EditGraph_DrawLine(HDC hDeviceContext,int nStartX,int nStartY,int nEndX,int nEndY,U32 Colour,int nLineWidth);
void EditGraph_DrawInputTrident(HDC hDeviceContext,SPete_Node* pNode,int nInputCount);
void EditGraph_GetInputPos(SPete_Node* pNode,int nInputIndex,int nInputTotal,SPete_UIPos* poutPos);
void EditGraph_DrawNullConnection(HDC hDeviceContext,int nPosX,int nPosY);
void EditGraph_GetOutputPos(SPete_Node* pNode,SPete_UIPos* poutPos);
void EditGraph_DrawConnection(HDC hDeviceContext,int nStartX,int nStartY,int nEndX,int nEndY);
void EditGraph_DrawOutputSpike(HDC hDeviceContext,SPete_EffectNode* pNode);
void EditGraph_DrawParameterConnections(HDC hDeviceContext,SPete_Node* pNode,SPete_UISelection* pSelection);
void EditGraph_DrawParameterTrident(HDC hDeviceContext,SPete_Node* pNode,int nParamCount);
void EditGraph_GetParameterPos(SPete_Node* pNode,int nParamIndex,int nParamTotal,SPete_UIPos* poutPos);
void EditGraph_GetParameterOutputPos(SPete_Node* pNode,SPete_UIPos* poutPos);
void EditGraph_DrawParameterOutputSpike(HDC hDeviceContext,SPete_Node* pNode);
void EditGraph_DrawText(HDC hDeviceContext,int nPosX,int nPosY,int nWidth,int nHeight,char* pText,char* pFontName,int nSize,U32 Colour,EPete_TextJustifaction eJustification);
void EditGraph_DrawArrowHead(HDC hDeviceContext,int nPosX,int nPosY,int nSize,bool bIsVertical,U32 Colour);
bool EditGraph_UIEvent_Idle(SPete_GraphData* pGraph,EEditGraph_UIEvent eEvent,int nPosX,int nPosY,void* pExtraData);
bool EditGraph_UIEvent_Dragging(SPete_GraphData* pGraph,EEditGraph_UIEvent eEvent,int nPosX,int nPosY,void* pExtraData);
void EditGraph_GetSelectionAtPos(SPete_GraphData* pGraph,int nPosX,int nPosY,SPete_UISelection* poutSelection);
void EditGraph_WindowToGraphSpace(int nWindowPosX,int nWindowPosY,int* poutGraphPosX,int* poutGraphPosY);
void EditGraph_GraphToWindowSpace(int nGraphPosX,int nGraphPosY,int* poutWindowPosX,int* poutWindowPosY);
void EditGraph_ClearBackground(HDC hDeviceContext,int nWidth,int nHeight,U32 Colour);
bool EditGraph_GetFileToOpen(char* pExtension,char* pTitle,char* poutFileName);
BOOL CALLBACK EditGraph_FFParamProperties_Callback(HWND hDialogBox,UINT Message,WPARAM wParam,LPARAM lParam);
void EditGraph_GetParameterBox(SPete_Node* pNode,int nParamIndex,SPete_UIRect* poutBox);
bool EditGraph_IsInBox(SPete_UIPos* pPos,SPete_UIRect* pBox);
void EditGraph_GetParameterOutputBox(SPete_Node* pNode,SPete_UIRect* poutBox);
void EditGraph_GetInputBox(SPete_Node* pNode,int nInputIndex,SPete_UIRect* poutBox);
void EditGraph_GetImageOutputBox(SPete_Node* pNode,SPete_UIRect* poutBox);
BOOL CALLBACK EditGraph_InputProperties_Callback(HWND hDialogBox,UINT Message,WPARAM wParam,LPARAM lParam);
void EditGraph_GetParameterLine(SPete_Node* pNode,int nParamIndex,SPete_UILine* poutLine);
void EditGraph_GetInputLine(SPete_Node* pNode,int nInputIndex,SPete_UILine* poutLine);
bool EditGraph_IsNearLine(SPete_UIPos* pPos,SPete_UILine* pLine,int nTolerance);
bool EditGraph_WriteResourceAsFile(char* pFileName);
BOOL CALLBACK EditGraph_SwitcherProperties_Callback(HWND hDialogBox,UINT Message,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK EditGraph_FileProperties_Callback(HWND hDialogBox,UINT Message,WPARAM wParam,LPARAM lParam);

typedef struct SPete_DialogArgs {
	SPete_GraphData* m_pGraph;
	SPete_Node* m_pNode;
} SPete_DialogArgs;


char* pTemplateFileName="FFTemplate.dll";

const int nInputTridentStartHeight=16;
const int nInputTridentEndHeight=8;
const int nInputTridentSeperation=12;
const int nInputTridentWidth=2;

const int nOutputSpikeHeight=16;

const int nParamTridentStartWidth=16;
const int nParamTridentEndWidth=8;
const int nParamTridentSeperation=12;
const int nParamTridentWidth=2;

const int nParameterOutputSpikeWidth=16;

const int nDefaultFontSize=16;
const int nSmallFontSize=8;

const int nArrowHeadSize=5;

const int nLineTolerance=6;

SPete_UISelection g_Selection;

enum EPete_UIState {
	eState_Idle,
	eState_Dragging,
} g_eUIState;

SPete_UIPos g_DrawOrigin;


void EditGraph_SaveDataFile(SPete_GraphData* pGraph,char* pFileName) {

	g_Selection.m_pNodeEntry=NULL;
	
	GraphUtil_FlattenGraph(pGraph);

	FILE* pOutputFile=fopen(pFileName,"wb");

	if (pOutputFile==NULL) {
		char MessageString[1024];
		sprintf(MessageString,"Output file '%s' could not be opened",pFileName);
		MessageBox(NULL,MessageString,"",MB_OK);
		return;
	}

	fwrite(pGraph->m_pFlattenedData,pGraph->m_nFlattenedDataSize,1,pOutputFile);

	fclose(pOutputFile);

	GraphUtil_FreeFlattenedData(pGraph);

}

void EditGraph_SaveAsFreeFrameDLL(SPete_GraphData* pGraph,char* pFileName) {

	g_Selection.m_pNodeEntry=NULL;

	GraphUtil_FlattenGraph(pGraph);

	const bool bWriteResult=
		EditGraph_WriteResourceAsFile(pFileName);

	if (!bWriteResult) {
		return;
	}

	HANDLE hOutputFile=
		BeginUpdateResource(pFileName,TRUE);

	if (hOutputFile==NULL) {
		char MessageString[1024];
		sprintf(MessageString,"Output file '%s' could not be loaded",pFileName);
		MessageBox(NULL,MessageString,"",MB_OK);
		return;
	}

	BOOL bUpdateResourceResult=UpdateResource(
		hOutputFile,
		"GRAPH",
		MAKEINTRESOURCE(CHAIN_DATA_RESOURCE_ID),
		0,
		pGraph->m_pFlattenedData,
		pGraph->m_nFlattenedDataSize);

	if (bUpdateResourceResult==FALSE) {
		MessageBox(NULL,"UpdateResource() failed","",MB_OK);
		return;
	}

	EndUpdateResource(hOutputFile,FALSE);

	GraphUtil_FreeFlattenedData(pGraph);

}

bool EditGraph_CopyFile(char* pSourceFileName,char* pDestFileName) {

	FILE* pSourceFile=fopen(pSourceFileName,"rb");
	if (pSourceFile==NULL) {
		char MessageString[1024];
		sprintf(MessageString,"Source file '%s' couldn't be opened",pSourceFileName);
		MessageBox(NULL,MessageString,"",MB_OK);
		return false;
	}

	FILE* pDestFile=fopen(pDestFileName,"wb");
	if (pDestFile==NULL) {
		char MessageString[1024];
		sprintf(MessageString,"Destination file '%s' couldn't be opened",pDestFileName);
		MessageBox(NULL,MessageString,"",MB_OK);
		return false;
	}

	fseek(pSourceFile,0,SEEK_END);
	const int nFileLength=ftell(pSourceFile);
	fseek(pSourceFile,0,SEEK_SET);

	char* pCopyData=(char*)(malloc(nFileLength));

	if (pCopyData==NULL) {
		MessageBox(NULL,"Copy data buffer could not be allocated","",MB_OK);
		return false;
	}

	fread(pCopyData,nFileLength,1,pSourceFile);
	fwrite(pCopyData,nFileLength,1,pDestFile);

	free(pCopyData);

	fclose(pDestFile);
	fclose(pSourceFile);

	return true;

}

void EditGraph_DrawAll(HWND hWindow,HDC hDeviceContext,RECT* pClientRect,SPete_GraphData* pGraph) {

	if (!pGraph->m_bIsValid) {
		return;
	}

	const int nWidth=pClientRect->right-pClientRect->left;
	const int nHeight=pClientRect->bottom-pClientRect->top;

	EditGraph_ClearBackground(hDeviceContext,nWidth,nHeight,0x00cccccc);

	SPete_NodeListEntry* pCurrentEntry=pGraph->m_pNodeListHead;

	while (pCurrentEntry!=NULL) {

		EditGraph_DrawNode(hWindow,hDeviceContext,pCurrentEntry->m_pNode);

		pCurrentEntry=pCurrentEntry->m_pNext;

	}

}

void EditGraph_DrawNode(HWND hWindow,HDC hDeviceContext,SPete_Node* pNode) {

	SPete_UISelection* pSelection;
	if ((g_Selection.m_pNodeEntry!=NULL)&&
		(g_Selection.m_pNodeEntry->m_pNode==pNode)) {
		pSelection=&g_Selection;
	} else {
		pSelection=NULL;
	}

	switch(pNode->m_eType) {

		case eType_Effect_Output: {

			EditGraph_DrawOutputNode(
				hWindow,
				hDeviceContext,
				(SPete_EffectNode*)pNode,
				pSelection);
								  
		}break;

		case eType_Effect_Input: {

			EditGraph_DrawInputNode(
				hWindow,
				hDeviceContext,
				(SPete_EffectNode*)pNode,
				pSelection);

		}break;

		case eType_Effect_Plugin: {

			EditGraph_DrawPluginNode(
				hWindow,
				hDeviceContext,
				(SPete_EffectNode*)pNode,
				pSelection);

		}break;

		case eType_Effect_Switcher: {

			EditGraph_DrawSwitcherNode(
				hWindow,
				hDeviceContext,
				(SPete_EffectNode*)pNode,
				pSelection);

		}break;

		case eType_Parameter_External: {

			EditGraph_DrawExternalParamNode(
				hWindow,
				hDeviceContext,
				(SPete_ParameterNode*)pNode,
				pSelection);

		}break;

		case eType_Parameter_Constant: {

			EditGraph_DrawConstantParamNode(
				hWindow,
				hDeviceContext,
				(SPete_ParameterNode*)pNode,
				pSelection);

		}break;

		case eType_Parameter_Keyboard : {

			EditGraph_DrawKeyboardParamNode(
				hWindow,
				hDeviceContext,
				(SPete_ParameterNode*)pNode,
				pSelection);

		}break;
		
		case eType_Parameter_Midi: {

			EditGraph_DrawMidiParamNode(
				hWindow,
				hDeviceContext,
				(SPete_ParameterNode*)pNode,
				pSelection);
			
		}break;

		default: {
			// do nothing
		}break;

	}

}


void EditGraph_DrawOutputNode(HWND hWindow,HDC hDeviceContext,SPete_EffectNode* pEffectNode,SPete_UISelection* pSelection) {

	const bool bIsSelected=
		((pSelection!=NULL)&&
		(pSelection->m_eType==eSelect_Body));

	U32 BoxFillColour=0x00996699;
	U32 BoxLineColour=0x00000000;
	U32 LabelColour=0x00FFFFC0;

	if (bIsSelected) {
		BoxFillColour^=0x00ffffff;
		BoxLineColour^=0x00ffffff;
		LabelColour^=0x00ffffff;
	}

	const int nOriginX=pEffectNode->m_Pos.nX;
	const int nOriginY=pEffectNode->m_Pos.nY;

	const int nBoxWidth=pEffectNode->m_Size.nX;
	const int nBoxHeight=pEffectNode->m_Size.nY;

	const int nLeftX=(nOriginX-(nBoxWidth/2));
	const int nTopY=(nOriginY-(nBoxHeight/2));

	EditGraph_DrawFilledBox(hDeviceContext,nLeftX,nTopY,nBoxWidth,nBoxHeight,BoxFillColour);
	EditGraph_DrawUnfilledBox(hDeviceContext,nLeftX,nTopY,nBoxWidth,nBoxHeight,BoxLineColour,1);

	EditGraph_DrawEffectConnections(hDeviceContext,pEffectNode,pSelection);
	EditGraph_DrawParameterConnections(hDeviceContext,pEffectNode,pSelection);

	EditGraph_DrawText(
		hDeviceContext,
		pEffectNode->m_Pos.nX,pEffectNode->m_Pos.nY,
		pEffectNode->m_Size.nX,pEffectNode->m_Size.nY,
		"Output",
		"Arial",nDefaultFontSize,
		LabelColour,
		eJustify_Centre);

}

void EditGraph_DrawInputNode(HWND hWindow,HDC hDeviceContext,SPete_EffectNode* pEffectNode,SPete_UISelection* pSelection) {

	const bool bIsSelected=
		((pSelection!=NULL)&&
		(pSelection->m_eType==eSelect_Body));

	U32 BoxFillColour=0x00996699;
	U32 BoxLineColour=0x00000000;
	U32 LabelColour=0x00FFFFC0;

	if (bIsSelected) {
		BoxFillColour^=0x00ffffff;
		BoxLineColour^=0x00ffffff;
		LabelColour^=0x00ffffff;
	}

	const int nOriginX=pEffectNode->m_Pos.nX;
	const int nOriginY=pEffectNode->m_Pos.nY;

	const int nBoxWidth=pEffectNode->m_Size.nX;
	const int nBoxHeight=pEffectNode->m_Size.nY;

	const int nLeftX=(nOriginX-(nBoxWidth/2));
	const int nTopY=(nOriginY-(nBoxHeight/2));

	EditGraph_DrawFilledBox(hDeviceContext,nLeftX,nTopY,nBoxWidth,nBoxHeight,BoxFillColour);
	EditGraph_DrawUnfilledBox(hDeviceContext,nLeftX,nTopY,nBoxWidth,nBoxHeight,BoxLineColour,1);

	EditGraph_DrawEffectConnections(hDeviceContext,pEffectNode,pSelection);
	EditGraph_DrawParameterConnections(hDeviceContext,pEffectNode,pSelection);

	char pLabelText[1024];
	sprintf(pLabelText,"Input %d",pEffectNode->m_nInputIndex+1);

	EditGraph_DrawText(
		hDeviceContext,
		pEffectNode->m_Pos.nX,pEffectNode->m_Pos.nY,
		pEffectNode->m_Size.nX,pEffectNode->m_Size.nY,
		pLabelText,
		"Arial",nDefaultFontSize,
		LabelColour,
		eJustify_Centre);

}

void EditGraph_DrawPluginNode(HWND hWindow,HDC hDeviceContext,SPete_EffectNode* pEffectNode,SPete_UISelection* pSelection) {

	const bool bIsSelected=
		((pSelection!=NULL)&&
		(pSelection->m_eType==eSelect_Body));

	U32 BoxFillColour=0x00996699;
	U32 BoxLineColour=0x00000000;
	U32 LabelColour=0x00FFFFC0;

	if (bIsSelected) {
		BoxFillColour^=0x00ffffff;
		BoxLineColour^=0x00ffffff;
		LabelColour^=0x00ffffff;
	}

	const int nOriginX=pEffectNode->m_Pos.nX;
	const int nOriginY=pEffectNode->m_Pos.nY;

	const int nBoxWidth=pEffectNode->m_Size.nX;
	const int nBoxHeight=pEffectNode->m_Size.nY;

	const int nLeftX=(nOriginX-(nBoxWidth/2));
	const int nTopY=(nOriginY-(nBoxHeight/2));

	EditGraph_DrawFilledBox(hDeviceContext,nLeftX,nTopY,nBoxWidth,nBoxHeight,BoxFillColour);
	EditGraph_DrawUnfilledBox(hDeviceContext,nLeftX,nTopY,nBoxWidth,nBoxHeight,BoxLineColour,1);

	EditGraph_DrawEffectConnections(hDeviceContext,pEffectNode,pSelection);
	EditGraph_DrawParameterConnections(hDeviceContext,pEffectNode,pSelection);

	SPete_PluginInfo* pPluginInfo=pEffectNode->m_pPluginInfo;
	char* pLabelText=pPluginInfo->m_pName;

	EditGraph_DrawText(
		hDeviceContext,
		pEffectNode->m_Pos.nX,pEffectNode->m_Pos.nY,
		pEffectNode->m_Size.nX,pEffectNode->m_Size.nY,
		pLabelText,
		"Arial",nDefaultFontSize,
		LabelColour,
		eJustify_Centre);

	if (bIsSelected) {

		const int nParamsCount=GraphUtil_GetParameterCount(pEffectNode);

		SPete_PluginParameter* pParamsInfo=pPluginInfo->m_pParameterInfo;

		int nCount;
		for (nCount=0; nCount<nParamsCount; nCount+=1) {

			char* pParamName;
			if (nCount>0) {

				SPete_PluginParameter* pCurrentParam=&pParamsInfo[nCount-1];

				pParamName=pCurrentParam->m_pName;

			} else {

				pParamName="Bypass";

			}

			SPete_UIPos ParamPos;
			EditGraph_GetParameterPos(pEffectNode,nCount,nParamsCount,&ParamPos);

			const int nLeftX=(ParamPos.nX-100);
			const int nRightX=(ParamPos.nX+80);
			const int nTopY=(ParamPos.nY-5);
			const int nBottomY=(ParamPos.nY+1);

			const U32 ParamColour=0x00000000;

			EditGraph_DrawText(
				hDeviceContext,
				nLeftX,nTopY,
				(nRightX-nLeftX),(nBottomY-nTopY),
				pParamName,
				"Arial",nSmallFontSize,
				ParamColour,
				eJustify_Right);

		}


	}

}

void EditGraph_DrawSwitcherNode(HWND hWindow,HDC hDeviceContext,SPete_EffectNode* pEffectNode,SPete_UISelection* pSelection) {

	const bool bIsSelected=
		((pSelection!=NULL)&&
		(pSelection->m_eType==eSelect_Body));

	U32 BoxFillColour=0x00996699;
	U32 BoxLineColour=0x00000000;
	U32 LabelColour=0x00FFFFC0;

	if (bIsSelected) {
		BoxFillColour^=0x00ffffff;
		BoxLineColour^=0x00ffffff;
		LabelColour^=0x00ffffff;
	}

	const int nOriginX=pEffectNode->m_Pos.nX;
	const int nOriginY=pEffectNode->m_Pos.nY;

	const int nBoxWidth=pEffectNode->m_Size.nX;
	const int nBoxHeight=pEffectNode->m_Size.nY;

	const int nLeftX=(nOriginX-(nBoxWidth/2));
	const int nTopY=(nOriginY-(nBoxHeight/2));

	EditGraph_DrawFilledBox(hDeviceContext,nLeftX,nTopY,nBoxWidth,nBoxHeight,BoxFillColour);
	EditGraph_DrawUnfilledBox(hDeviceContext,nLeftX,nTopY,nBoxWidth,nBoxHeight,BoxLineColour,1);

	EditGraph_DrawEffectConnections(hDeviceContext,pEffectNode,pSelection);
	EditGraph_DrawParameterConnections(hDeviceContext,pEffectNode,pSelection);

	char* pLabelText="Switcher";

	EditGraph_DrawText(
		hDeviceContext,
		pEffectNode->m_Pos.nX,pEffectNode->m_Pos.nY,
		pEffectNode->m_Size.nX,pEffectNode->m_Size.nY,
		pLabelText,
		"Arial",nDefaultFontSize,
		LabelColour,
		eJustify_Centre);

	if (bIsSelected) {

		SPete_UIPos ParamPos;
		EditGraph_GetParameterPos(pEffectNode,0,1,&ParamPos);

		const int nLeftX=(ParamPos.nX-100);
		const int nRightX=(ParamPos.nX+80);
		const int nTopY=(ParamPos.nY-5);
		const int nBottomY=(ParamPos.nY+1);

		const U32 ParamColour=0x00000000;

		char* pParamName="Selector";

		EditGraph_DrawText(
			hDeviceContext,
			nLeftX,nTopY,
			(nRightX-nLeftX),(nBottomY-nTopY),
			pParamName,
			"Arial",nSmallFontSize,
			ParamColour,
			eJustify_Right);

	}

}

void EditGraph_DrawExternalParamNode(HWND hWindow,HDC hDeviceContext,SPete_ParameterNode* pParamNode,SPete_UISelection* pSelection) {

	const bool bIsSelected=
		((pSelection!=NULL)&&
		(pSelection->m_eType==eSelect_Body));

	U32 BoxFillColour=0x00996699;
	U32 BoxLineColour=0x00000000;
	U32 LabelColour=0x00FFFFC0;

	if (bIsSelected) {
		BoxFillColour^=0x00ffffff;
		BoxLineColour^=0x00ffffff;
		LabelColour^=0x00ffffff;
	}

	const int nOriginX=pParamNode->m_Pos.nX;
	const int nOriginY=pParamNode->m_Pos.nY;

	const int nBoxWidth=pParamNode->m_Size.nX;
	const int nBoxHeight=pParamNode->m_Size.nY;

	const int nLeftX=(nOriginX-(nBoxWidth/2));
	const int nTopY=(nOriginY-(nBoxHeight/2));

	EditGraph_DrawFilledBox(hDeviceContext,nLeftX,nTopY,nBoxWidth,nBoxHeight,BoxFillColour);
	EditGraph_DrawUnfilledBox(hDeviceContext,nLeftX,nTopY,nBoxWidth,nBoxHeight,BoxLineColour,1);

	EditGraph_DrawParameterConnections(hDeviceContext,pParamNode,pSelection);

	char pLabelText[1024];
	sprintf(pLabelText,"FF In %d",pParamNode->m_nExternalIndex+1);

	EditGraph_DrawText(
		hDeviceContext,
		pParamNode->m_Pos.nX,pParamNode->m_Pos.nY,
		pParamNode->m_Size.nX,pParamNode->m_Size.nY,
		pLabelText,
		"Arial",nDefaultFontSize,
		LabelColour,
		eJustify_Centre);

}

void EditGraph_DrawConstantParamNode(HWND hWindow,HDC hDeviceContext,SPete_ParameterNode* pParamNode,SPete_UISelection* pSelection) {

	const bool bIsSelected=
		((pSelection!=NULL)&&
		(pSelection->m_eType==eSelect_Body));

	U32 BoxFillColour=0x00996699;
	U32 BoxLineColour=0x00000000;
	U32 LabelColour=0x00FFFFC0;

	if (bIsSelected) {
		BoxFillColour^=0x00ffffff;
		BoxLineColour^=0x00ffffff;
		LabelColour^=0x00ffffff;
	}

	const int nOriginX=pParamNode->m_Pos.nX;
	const int nOriginY=pParamNode->m_Pos.nY;

	const int nBoxWidth=pParamNode->m_Size.nX;
	const int nBoxHeight=pParamNode->m_Size.nY;

	const int nLeftX=(nOriginX-(nBoxWidth/2));
	const int nTopY=(nOriginY-(nBoxHeight/2));

	EditGraph_DrawFilledBox(hDeviceContext,nLeftX,nTopY,nBoxWidth,nBoxHeight,BoxFillColour);
	EditGraph_DrawUnfilledBox(hDeviceContext,nLeftX,nTopY,nBoxWidth,nBoxHeight,BoxLineColour,1);

	EditGraph_DrawParameterConnections(hDeviceContext,pParamNode,pSelection);

}

void EditGraph_DrawKeyboardParamNode(HWND hWindow,HDC hDeviceContext,SPete_ParameterNode* pParamNode,SPete_UISelection* pSelection) {

	const bool bIsSelected=
		((pSelection!=NULL)&&
		(pSelection->m_eType==eSelect_Body));

	U32 BoxFillColour=0x00996699;
	U32 BoxLineColour=0x00000000;
	U32 LabelColour=0x00FFFFC0;

	if (bIsSelected) {
		BoxFillColour^=0x00ffffff;
		BoxLineColour^=0x00ffffff;
		LabelColour^=0x00ffffff;
	}

	const int nOriginX=pParamNode->m_Pos.nX;
	const int nOriginY=pParamNode->m_Pos.nY;

	const int nBoxWidth=pParamNode->m_Size.nX;
	const int nBoxHeight=pParamNode->m_Size.nY;

	const int nLeftX=(nOriginX-(nBoxWidth/2));
	const int nTopY=(nOriginY-(nBoxHeight/2));

	EditGraph_DrawFilledBox(hDeviceContext,nLeftX,nTopY,nBoxWidth,nBoxHeight,BoxFillColour);
	EditGraph_DrawUnfilledBox(hDeviceContext,nLeftX,nTopY,nBoxWidth,nBoxHeight,BoxLineColour,1);

	EditGraph_DrawParameterConnections(hDeviceContext,pParamNode,pSelection);

}

void EditGraph_DrawMidiParamNode(HWND hWindow,HDC hDeviceContext,SPete_ParameterNode* pParamNode,SPete_UISelection* pSelection) {

	const bool bIsSelected=
		((pSelection!=NULL)&&
		(pSelection->m_eType==eSelect_Body));

	U32 BoxFillColour=0x00996699;
	U32 BoxLineColour=0x00000000;
	U32 LabelColour=0x00FFFFC0;

	if (bIsSelected) {
		BoxFillColour^=0x00ffffff;
		BoxLineColour^=0x00ffffff;
		LabelColour^=0x00ffffff;
	}

	const int nOriginX=pParamNode->m_Pos.nX;
	const int nOriginY=pParamNode->m_Pos.nY;

	const int nBoxWidth=pParamNode->m_Size.nX;
	const int nBoxHeight=pParamNode->m_Size.nY;

	const int nLeftX=(nOriginX-(nBoxWidth/2));
	const int nTopY=(nOriginY-(nBoxHeight/2));

	EditGraph_DrawFilledBox(hDeviceContext,nLeftX,nTopY,nBoxWidth,nBoxHeight,BoxFillColour);
	EditGraph_DrawUnfilledBox(hDeviceContext,nLeftX,nTopY,nBoxWidth,nBoxHeight,BoxLineColour,1);

	EditGraph_DrawParameterConnections(hDeviceContext,pParamNode,pSelection);

}

void EditGraph_DrawUnfilledBox(HDC hDeviceContext,int nPosX,int nPosY,int nWidth,int nHeight,U32 Colour,int nLineWidth) {

	EditGraph_GraphToWindowSpace(nPosX,nPosY,&nPosX,&nPosY);
	
	HPEN hCurrentPen=
		CreatePen(PS_SOLID,nLineWidth,Colour);

	SelectObject(hDeviceContext,hCurrentPen);

	POINT pRectPoints[5]={
		{nPosX,			nPosY},
		{(nPosX+nWidth),nPosY},
		{(nPosX+nWidth),(nPosY+nHeight)},
		{nPosX,			(nPosY+nHeight)},
		{nPosX,			nPosY},
	};

	Polyline(hDeviceContext,pRectPoints,5);

	DeleteObject(hCurrentPen);

}

void EditGraph_DrawFilledBox(HDC hDeviceContext,int nPosX,int nPosY,int nWidth,int nHeight,U32 Colour) {

	EditGraph_GraphToWindowSpace(nPosX,nPosY,&nPosX,&nPosY);

	HPEN hCurrentPen=
		CreatePen(PS_SOLID,1,Colour);

	SelectObject(hDeviceContext,hCurrentPen);

	LOGBRUSH CurrentBrush;
	CurrentBrush.lbStyle=BS_SOLID;
	CurrentBrush.lbColor=Colour;

	HBRUSH hCurrentBrush=CreateBrushIndirect(&CurrentBrush);

	SelectObject(hDeviceContext,hCurrentBrush);

	Rectangle(hDeviceContext,
		nPosX,nPosY,
		nPosX+nWidth,nPosY+nHeight);

	DeleteObject(hCurrentBrush);

	DeleteObject(hCurrentPen);

}

void EditGraph_DrawEffectConnections(HDC hDeviceContext,SPete_EffectNode* pEffectNode,SPete_UISelection* pSelection) {

	const bool bIsInputSelected=
		((pSelection!=NULL)&&
		(pSelection->m_eType==eSelect_Input));

	const bool bIsOutputSelected=
		((pSelection!=NULL)&&
		(pSelection->m_eType==eSelect_Output));

	const int nInputCount=GraphUtil_GetInputCount(pEffectNode);
	const int nParamCount=GraphUtil_GetParameterCount(pEffectNode);
	
	SPete_UIPos* pNodePos=(&pEffectNode->m_Pos);

	if (nInputCount>0) {

		EditGraph_DrawInputTrident(hDeviceContext,pEffectNode,nInputCount);

	}

	int nCount;
	for (nCount=0; nCount<nInputCount; nCount+=1) {

		SPete_UIPos StartPos;
		EditGraph_GetInputPos(pEffectNode,nCount,nInputCount,&StartPos);

		if ((bIsInputSelected)&&
			(pSelection->m_nIndex==nCount)) {

			SPete_UIPos EndPos=pSelection->m_ConnectionEnd;

			EditGraph_DrawConnection(
				hDeviceContext,
				EndPos.nX,EndPos.nY,
				StartPos.nX,StartPos.nY);

		} else {

			SPete_EffectNode* pInputNode=pEffectNode->m_ppInputs[nCount];

			if (pInputNode==NULL) {

				EditGraph_DrawNullConnection(hDeviceContext,StartPos.nX,StartPos.nY);

			} else {

				SPete_UIPos InputPos;

				EditGraph_GetOutputPos(pInputNode,&InputPos);

				EditGraph_DrawConnection(
					hDeviceContext,
					InputPos.nX,InputPos.nY,
					StartPos.nX,StartPos.nY);

			}

		}

	}

	EditGraph_DrawOutputSpike(hDeviceContext,pEffectNode);

	if (bIsOutputSelected) {

		SPete_UIPos StartPos;
		EditGraph_GetOutputPos(pEffectNode,&StartPos);

		SPete_UIPos EndPos=pSelection->m_ConnectionEnd;

		EditGraph_DrawConnection(
			hDeviceContext,
			EndPos.nX,EndPos.nY,
			StartPos.nX,StartPos.nY);

	}

}


void EditGraph_DrawLine(HDC hDeviceContext,int nStartX,int nStartY,int nEndX,int nEndY,U32 Colour,int nLineWidth) {

	EditGraph_GraphToWindowSpace(nStartX,nStartY,&nStartX,&nStartY);
	EditGraph_GraphToWindowSpace(nEndX,nEndY,&nEndX,&nEndY);

	HPEN hCurrentPen=
		CreatePen(PS_SOLID,nLineWidth,Colour);

	SelectObject(hDeviceContext,hCurrentPen);

	POINT pRectPoints[2]={
		{nStartX,nStartY},
		{nEndX,nEndY},
	};

	Polyline(hDeviceContext,pRectPoints,2);

	DeleteObject(hCurrentPen);

}

void EditGraph_DrawInputTrident(HDC hDeviceContext,SPete_Node* pNode,int nInputCount) {

	const U32 TridentColour=0x00de6100;
	
	SPete_UIPos* pOrigin=&pNode->m_Pos;

	const int nHeight=pNode->m_Size.nY;
	const int nHalfHeight=(nHeight/2);

	const int nTridentStartY=(pOrigin->nY-nHalfHeight);
	const int nTridentMidY=(nTridentStartY-nInputTridentStartHeight);
	const int nTridentEndY=(nTridentMidY-nInputTridentEndHeight);

	SPete_UIPos TridentStart;
	TridentStart.nX=pOrigin->nX;
	TridentStart.nY=nTridentStartY;

	EditGraph_DrawLine(
		hDeviceContext,
		TridentStart.nX,TridentStart.nY-1,
		TridentStart.nX,nTridentMidY,
		TridentColour,3);

	const int nForkSeperation=nInputTridentSeperation;

	const int nTridentWidth=((nInputCount-1)*nForkSeperation);

	const int nForkLeftX=(pOrigin->nX-(nTridentWidth/2));
	const int nForkRightX=(pOrigin->nX+(nTridentWidth/2));

	EditGraph_DrawLine(
		hDeviceContext,
		nForkLeftX,nTridentMidY,
		nForkRightX,nTridentMidY,
		TridentColour,3);

	int nCount;
	for (nCount=0; nCount<nInputCount; nCount+=1) {

		const int nCurrentX=
			(nForkLeftX+(nCount*nForkSeperation));

		EditGraph_DrawLine(
			hDeviceContext,
			nCurrentX,nTridentMidY,
			nCurrentX,nTridentEndY,
			TridentColour,3);

	}

	EditGraph_DrawArrowHead(
		hDeviceContext,
		TridentStart.nX,TridentStart.nY,
		nArrowHeadSize,
		true,
		TridentColour);

}

void EditGraph_GetInputPos(SPete_Node* pNode,int nInputIndex,int nInputTotal,SPete_UIPos* poutPos) {

	SPete_UIPos* pOrigin=&pNode->m_Pos;

	const int nHeight=pNode->m_Size.nY;
	const int nHalfHeight=(nHeight/2);

	const int nTridentStartY=(pOrigin->nY-nHalfHeight);
	const int nTridentMidY=(nTridentStartY-nInputTridentStartHeight);
	const int nTridentEndY=(nTridentMidY-nInputTridentEndHeight);

	const int nForkSeperation=nInputTridentSeperation;

	const int nTridentWidth=((nInputTotal-1)*nForkSeperation);

	const int nForkLeftX=(pOrigin->nX-(nTridentWidth/2));

	poutPos->nX=
		(nForkLeftX+(nInputIndex*nForkSeperation));

	poutPos->nY=nTridentEndY;

}

void EditGraph_DrawNullConnection(HDC hDeviceContext,int nPosX,int nPosY) {

	EditGraph_DrawFilledBox(hDeviceContext,
		(nPosX-2),(nPosY-2),
		5,5,
		0x00000000);

}

void EditGraph_GetOutputPos(SPete_Node* pNode,SPete_UIPos* poutPos) {

	SPete_UIPos* pOrigin=&pNode->m_Pos;

	const int nHeight=pNode->m_Size.nY;
	const int nHalfHeight=(nHeight/2);

	const int nOutputStartY=(pOrigin->nY+nHalfHeight);
	const int nOutputEndY=(nOutputStartY+nOutputSpikeHeight);

	poutPos->nX=pNode->m_Pos.nX;
	poutPos->nY=nOutputEndY;

}


void EditGraph_DrawConnection(HDC hDeviceContext,int nStartX,int nStartY,int nEndX,int nEndY) {

	const U32 Colour=0x00000000;

	EditGraph_DrawLine(hDeviceContext,nStartX,nStartY,nEndX,nEndY,Colour,1);

}

void EditGraph_DrawOutputSpike(HDC hDeviceContext,SPete_EffectNode* pNode) {

	if (pNode->m_eType==eType_Effect_Output) {
		return;
	}

	SPete_UIPos SpikeEnd;
	EditGraph_GetOutputPos(pNode,&SpikeEnd);

	const U32 Colour=0x00de6100;

	EditGraph_DrawLine(
		hDeviceContext,
		SpikeEnd.nX,SpikeEnd.nY-nOutputSpikeHeight,
		SpikeEnd.nX,SpikeEnd.nY-1,
		Colour,1);

	EditGraph_DrawLine(
		hDeviceContext,
		SpikeEnd.nX-1,SpikeEnd.nY-nOutputSpikeHeight,
		SpikeEnd.nX-1,SpikeEnd.nY-1,
		Colour,1);

	EditGraph_DrawLine(
		hDeviceContext,
		SpikeEnd.nX+1,SpikeEnd.nY-nOutputSpikeHeight,
		SpikeEnd.nX+1,SpikeEnd.nY-1,
		Colour,1);

	EditGraph_DrawArrowHead(
		hDeviceContext,
		SpikeEnd.nX,SpikeEnd.nY,
		nArrowHeadSize,
		true,
		Colour);

}

void EditGraph_DrawParameterConnections(HDC hDeviceContext,SPete_Node* pNode,SPete_UISelection* pSelection) {

	const bool bIsInputSelected=
		((pSelection!=NULL)&&
		(pSelection->m_eType==eSelect_Parameter_Input));

	const bool bIsOutputSelected=
		((pSelection!=NULL)&&
		(pSelection->m_eType==eSelect_Parameter_Output));

	const int nParamCount=GraphUtil_GetParameterCount(pNode);
	
	SPete_UIPos* pNodePos=(&pNode->m_Pos);

	if (nParamCount>0) {

		EditGraph_DrawParameterTrident(hDeviceContext,pNode,nParamCount);

	}

	int nCount;
	for (nCount=0; nCount<nParamCount; nCount+=1) {

		SPete_UIPos StartPos;
		EditGraph_GetParameterPos(pNode,nCount,nParamCount,&StartPos);

		if ((bIsInputSelected)&&
			(pSelection->m_nIndex==nCount)) {

			SPete_UIPos EndPos=pSelection->m_ConnectionEnd;

			EditGraph_DrawConnection(
				hDeviceContext,
				EndPos.nX,EndPos.nY,
				StartPos.nX,StartPos.nY);

		} else {

			SPete_ParameterNode* pParamNode=pNode->m_ppParameters[nCount];

			if (pParamNode==NULL) {

				EditGraph_DrawNullConnection(hDeviceContext,StartPos.nX,StartPos.nY);

			} else {

				SPete_UIPos InputPos;

				EditGraph_GetParameterOutputPos(pParamNode,&InputPos);

				EditGraph_DrawConnection(
					hDeviceContext,
					InputPos.nX,InputPos.nY,
					StartPos.nX,StartPos.nY);

			}

		}

	}

	EditGraph_DrawParameterOutputSpike(hDeviceContext,pNode);

	if (bIsOutputSelected) {

		SPete_UIPos StartPos;
		EditGraph_GetParameterOutputPos(pNode,&StartPos);

		SPete_UIPos EndPos=pSelection->m_ConnectionEnd;

		EditGraph_DrawConnection(
			hDeviceContext,
			EndPos.nX,EndPos.nY,
			StartPos.nX,StartPos.nY);

	}


}

void EditGraph_DrawParameterTrident(HDC hDeviceContext,SPete_Node* pNode,int nParamCount) {

	if (nParamCount==0) {
		return;
	}
	
	const U32 TridentColour=0x00de6100;
	
	SPete_UIPos* pOrigin=&pNode->m_Pos;

	const int nWidth=pNode->m_Size.nX;
	const int nHalfWidth=(nWidth/2);

	const int nTridentStartX=(pOrigin->nX-nHalfWidth);
	const int nTridentMidX=(nTridentStartX-nParamTridentStartWidth);
	const int nTridentEndX=(nTridentMidX-nParamTridentEndWidth);

	SPete_UIPos TridentStart;
	TridentStart.nX=nTridentStartX;
	TridentStart.nY=pOrigin->nY;

	EditGraph_DrawLine(
		hDeviceContext,
		TridentStart.nX-1,TridentStart.nY,
		nTridentMidX,TridentStart.nY,
		TridentColour,3);

	const int nForkSeperation=nParamTridentSeperation;

	const int nTridentHeight=((nParamCount-1)*nForkSeperation);

	const int nForkTopY=(pOrigin->nY-(nTridentHeight/2));
	const int nForkBottomY=(pOrigin->nY+(nTridentHeight/2));

	EditGraph_DrawLine(
		hDeviceContext,
		nTridentMidX,nForkTopY,
		nTridentMidX,nForkBottomY,
		TridentColour,3);

	int nCount;
	for (nCount=0; nCount<nParamCount; nCount+=1) {

		const int nCurrentY=
			(nForkTopY+(nCount*nForkSeperation));

		EditGraph_DrawLine(
			hDeviceContext,
			nTridentMidX,nCurrentY,
			nTridentEndX,nCurrentY,
			TridentColour,3);

	}

	EditGraph_DrawArrowHead(
		hDeviceContext,
		TridentStart.nX,TridentStart.nY,
		nArrowHeadSize,
		false,
		TridentColour);
}

void EditGraph_GetParameterPos(SPete_Node* pNode,int nParamIndex,int nParamTotal,SPete_UIPos* poutPos) {

	SPete_UIPos* pOrigin=&pNode->m_Pos;

	const int nWidth=pNode->m_Size.nX;
	const int nHalfWidth=(nWidth/2);

	const int nTridentStartX=(pOrigin->nX-nHalfWidth);
	const int nTridentMidX=(nTridentStartX-nParamTridentStartWidth);
	const int nTridentEndX=(nTridentMidX-nParamTridentEndWidth);

	const int nForkSeperation=nParamTridentSeperation;

	const int nTridentHeight=((nParamTotal-1)*nForkSeperation);

	const int nForkTopY=(pOrigin->nY-(nTridentHeight/2));
	
	poutPos->nX=nTridentEndX;

	poutPos->nY=
		(nForkTopY+(nParamIndex*nForkSeperation));

}

void EditGraph_GetParameterOutputPos(SPete_Node* pNode,SPete_UIPos* poutPos) {

	SPete_UIPos* pOrigin=&pNode->m_Pos;

	const int nWidth=pNode->m_Size.nX;
	const int nHalfWidth=(nWidth/2);

	const int nOutputStartX=(pOrigin->nX+nHalfWidth);
	const int nOutputEndX=(nOutputStartX+nParameterOutputSpikeWidth);

	poutPos->nX=nOutputEndX;
	poutPos->nY=pNode->m_Pos.nY;

}

void EditGraph_DrawParameterOutputSpike(HDC hDeviceContext,SPete_Node* pNode) {

	const bool bHasParameterOutput=
		GraphUtil_HasParameterOutput(pNode);

	if (!bHasParameterOutput) {
		return;
	}

	SPete_UIPos SpikeEnd;
	EditGraph_GetParameterOutputPos(pNode,&SpikeEnd);

	const U32 Colour=0x00de6100;

	EditGraph_DrawLine(
		hDeviceContext,
		SpikeEnd.nX-1,SpikeEnd.nY,
		SpikeEnd.nX-nParameterOutputSpikeWidth,SpikeEnd.nY,
		Colour,1);

	EditGraph_DrawLine(
		hDeviceContext,
		SpikeEnd.nX-1,SpikeEnd.nY-1,
		SpikeEnd.nX-nParameterOutputSpikeWidth,SpikeEnd.nY-1,
		Colour,1);

	EditGraph_DrawLine(
		hDeviceContext,
		SpikeEnd.nX-1,SpikeEnd.nY+1,
		SpikeEnd.nX-nParameterOutputSpikeWidth,SpikeEnd.nY+1,
		Colour,1);

	EditGraph_DrawArrowHead(
		hDeviceContext,
		SpikeEnd.nX,SpikeEnd.nY,
		nArrowHeadSize,
		false,
		Colour);

}


void EditGraph_DrawText(HDC hDeviceContext,int nPosX,int nPosY,int nWidth,int nHeight,char* pText,char* pFontName,int nSize,U32 Colour,EPete_TextJustifaction eJustification) {

	EditGraph_GraphToWindowSpace(nPosX,nPosY,&nPosX,&nPosY);

	HFONT hCurrentFont=CreateFont(
		nSize,
		0,
		0,
		0,
		FW_NORMAL,
		FALSE,
		FALSE,
		FALSE,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FF_DONTCARE|DEFAULT_PITCH,
		pFontName);

	if (hCurrentFont==NULL) {
		return;
	}

	SelectObject(hDeviceContext,hCurrentFont);

	RECT TextRect={
		nPosX-(nWidth/2),nPosY-(nHeight/2),
		nPosX+(nWidth/2),nPosY+(nHeight/2),
	};

	SetBkMode(hDeviceContext,TRANSPARENT);
	SetTextColor(hDeviceContext,Colour);

	U32 DrawTextFlags;
	switch (eJustification) {

		case eJustify_Centre: {
			DrawTextFlags=(DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}break;

		case eJustify_Left: {
			DrawTextFlags=(DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		}break;

		case eJustify_Right: {
			DrawTextFlags=(DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
		}break;

		default: {
			assert(false);
			DrawTextFlags=0;
		}break;
	}

	DrawText(
		hDeviceContext,
		pText,strlen(pText),
		&TextRect,
		DrawTextFlags);

	DeleteObject(hCurrentFont);

}

void EditGraph_DrawArrowHead(HDC hDeviceContext,int nPosX,int nPosY,int nSize,bool bIsVertical,U32 Colour) {

	EditGraph_GraphToWindowSpace(nPosX,nPosY,&nPosX,&nPosY);

	HPEN hCurrentPen=
		CreatePen(PS_SOLID,1,Colour);

	SelectObject(hDeviceContext,hCurrentPen);

	LOGBRUSH CurrentBrush;
	CurrentBrush.lbStyle=BS_SOLID;
	CurrentBrush.lbColor=Colour;

	HBRUSH hCurrentBrush=CreateBrushIndirect(&CurrentBrush);

	SelectObject(hDeviceContext,hCurrentBrush);

	POINT pHorizontalPoints[3]={
		{nPosX,			nPosY},
		{(nPosX-nSize),	(nPosY-nSize)},
		{(nPosX-nSize),	(nPosY+nSize)}};

	POINT pVerticalPoints[3]={
		{nPosX,			nPosY},
		{(nPosX-nSize),	(nPosY-nSize)},
		{(nPosX+nSize),	(nPosY-nSize)}};

	if (bIsVertical) {
	
		Polygon(hDeviceContext,pVerticalPoints,3);

	} else {

		Polygon(hDeviceContext,pHorizontalPoints,3);

	}

	DeleteObject(hCurrentBrush);

	DeleteObject(hCurrentPen);

}

void EditGraph_RunGraph(SPete_GraphData* pGraph) {

	SPete_RunTimeEnvironment Environment;

	const int nWidth=320;
	const int nHeight=240;
	const int nScreenByteCount=
		(nWidth*nHeight*sizeof(U32));

	RunGraph_Init(pGraph,nWidth,nHeight,&Environment);

	Environment.m_pOutput=(U32*)(malloc(nScreenByteCount));

	Environment.m_ppInputs[0]=(U32*)(malloc(nScreenByteCount));
	Environment.m_nInputsCount=1;

	RunGraph_Run(pGraph->m_pRootNode,&Environment);

	RunGraph_DeInit(pGraph->m_pRootNode,&Environment);

}

void EditGraph_InitUI(SPete_GraphData* pGraph) {

	g_eUIState=eState_Idle;
	g_Selection.m_pNodeEntry=NULL;

}

void EditGraph_DeInitUI(SPete_GraphData* pGraph) {

}

bool EditGraph_UIEvent(SPete_GraphData* pGraph,EEditGraph_UIEvent eEvent,int nPosX,int nPosY,void* pExtraData) {

	int nGraphPosX;
	int nGraphPosY;

	EditGraph_WindowToGraphSpace(nPosX,nPosY,&nGraphPosX,&nGraphPosY);

	bool bUpdateWindow;

	switch (g_eUIState) {

		case eState_Idle: {

			bUpdateWindow=
				EditGraph_UIEvent_Idle(pGraph,eEvent,nGraphPosX,nGraphPosY,pExtraData);

		}break;

		case eState_Dragging: {

			bUpdateWindow=
				EditGraph_UIEvent_Dragging(pGraph,eEvent,nGraphPosX,nGraphPosY,pExtraData);

		}break;

		default: {
			bUpdateWindow=false;
		}break;

	}

	return bUpdateWindow;

}

bool EditGraph_UIEvent_Idle(SPete_GraphData* pGraph,EEditGraph_UIEvent eEvent,int nPosX,int nPosY,void* pExtraData) {

	bool bUpdateWindow;

	EPete_UIState eNewState=g_eUIState;

	switch (eEvent) {

		case eEvent_Mouse_LDown: {

			SPete_UISelection NewSelection;

			EditGraph_GetSelectionAtPos(pGraph,nPosX,nPosY,&NewSelection);

			if ((NewSelection.m_pNodeEntry==NULL)&&
				(g_Selection.m_pNodeEntry==NULL)) {

				g_Selection=NewSelection;
				eNewState=eState_Dragging;

				bUpdateWindow=false;

			} else {

				bUpdateWindow=true;

				if ((NewSelection.m_pNodeEntry==g_Selection.m_pNodeEntry)&&
					(NewSelection.m_eType==g_Selection.m_eType)&&
					(NewSelection.m_nIndex==g_Selection.m_nIndex)) {

					g_Selection.m_pNodeEntry=NULL;

				} else {

					g_Selection=NewSelection;
					eNewState=eState_Dragging;

				}

			}


		}break;

		case eEvent_Mouse_LUp: {

			bUpdateWindow=false;

		}break;

		case eEvent_Mouse_LDoubleClick: {

			bUpdateWindow=false;

		}break;

		case eEvent_Mouse_RDown: {

			SPete_UISelection NewSelection;

			EditGraph_GetSelectionAtPos(pGraph,nPosX,nPosY,&NewSelection);

			g_Selection=NewSelection;

			InvalidateRect(g_hWindow,NULL,FALSE);
			UpdateWindow(g_hWindow);

			if (NewSelection.m_pNodeEntry==NULL) {

				HMENU hGeneralPopupMenu=LoadMenu(g_hWindowInstance,MAKEINTRESOURCE(IDC_GENERALPOPUP));
				HMENU hPopupSubMenu=GetSubMenu(hGeneralPopupMenu,0);

				int nWindowPosX;
				int nWindowPosY;

				EditGraph_GraphToWindowSpace(nPosX,nPosY,&nWindowPosX,&nWindowPosY);

				POINT ScreenPoint=
					{nWindowPosX,nWindowPosY};

				ClientToScreen(g_hWindow,&ScreenPoint);

				int nMenuSelection=TrackPopupMenu(hPopupSubMenu,
					(TPM_NONOTIFY|TPM_RETURNCMD|TPM_CENTERALIGN|TPM_VCENTERALIGN|TPM_LEFTBUTTON),
					ScreenPoint.x,ScreenPoint.y,
					0,g_hWindow,NULL);

				switch (nMenuSelection) {

					case IDM_NEWEFFECT: {
						
						char pEffectFileName[MAX_PATH]="";

						bool bFileFound=
							EditGraph_GetFileToOpen(
							"dll",
							"Open a FreeFrame Effect",
							pEffectFileName);						

						if (bFileFound) {

							SPete_PluginInfo* pPluginInfo=
								GraphUtil_AddPluginInfo(pGraph,pEffectFileName);

							if (pPluginInfo!=NULL) {

								GraphUtil_MakePluginNode(pGraph,pPluginInfo,nPosX,nPosY);

							}
							
						}


					}break;

					case IDM_NEWFFPARAM: {

						SPete_ParameterNode CandidateNode;

						int nHighestFFParam=-1;

						SPete_NodeListEntry* pCurrentEntry=pGraph->m_pNodeListHead;
						while (pCurrentEntry!=NULL) {

							SPete_Node* pNode=pCurrentEntry->m_pNode;

							if (pNode->m_eType==eType_Parameter_External) {

								SPete_ParameterNode* pParamNode=
									(SPete_ParameterNode*)(pNode);

								if (pParamNode->m_nExternalIndex>nHighestFFParam) {
									nHighestFFParam=pParamNode->m_nExternalIndex;
								}
							
							}

							pCurrentEntry=pCurrentEntry->m_pNext;
						}


						CandidateNode.m_eType=eType_Parameter_External;
						CandidateNode.m_ExternalDefault=0.0f;
						CandidateNode.m_pExternalLabel[0]='\0';
						CandidateNode.m_nExternalIndex=(nHighestFFParam+1);
						CandidateNode.m_ExternalType=FF_TYPE_STANDARD;

						SPete_DialogArgs DialogArgs;
						DialogArgs.m_pGraph=pGraph;
						DialogArgs.m_pNode=&CandidateNode;

						const int nDialogBoxResult=
							DialogBoxParam(
							g_hWindowInstance,
							MAKEINTRESOURCE(IDD_PROPERTIES_FFPARAM),
							g_hWindow,
							EditGraph_FFParamProperties_Callback,
							(U32)(&DialogArgs));

						if (nDialogBoxResult==IDOK) {

							GraphUtil_MakeExternalParamNode(
								pGraph,
								CandidateNode.m_nExternalIndex,
								CandidateNode.m_pExternalLabel,
								CandidateNode.m_ExternalDefault,
								CandidateNode.m_ExternalType,
								nPosX,nPosY);

						}

					}break;

					case IDM_NEWINPUT: {

						SPete_EffectNode CandidateNode;

						int nHighestInputIndex=-1;

						SPete_NodeListEntry* pCurrentEntry=pGraph->m_pNodeListHead;
						while (pCurrentEntry!=NULL) {

							SPete_Node* pNode=pCurrentEntry->m_pNode;

							if (pNode->m_eType==eType_Effect_Input) {

								SPete_EffectNode* pEffectNode=
									(SPete_EffectNode*)(pNode);

								if (pEffectNode->m_nInputIndex>nHighestInputIndex) {
									nHighestInputIndex=pEffectNode->m_nInputIndex;
								}
							
							}

							pCurrentEntry=pCurrentEntry->m_pNext;
						}

						CandidateNode.m_eType=eType_Effect_Input;
						CandidateNode.m_nInputIndex=(nHighestInputIndex+1);

						SPete_DialogArgs DialogArgs;
						DialogArgs.m_pGraph=pGraph;
						DialogArgs.m_pNode=&CandidateNode;

						const int nDialogBoxResult=
							DialogBoxParam(
							g_hWindowInstance,
							MAKEINTRESOURCE(IDD_PROPERTIES_INPUT),
							g_hWindow,
							EditGraph_InputProperties_Callback,
							(U32)(&DialogArgs));

						if (nDialogBoxResult==IDOK) {

							GraphUtil_MakeInputNode(
								pGraph,
								CandidateNode.m_nInputIndex,
								nPosX,nPosY);

						}

					}break;

					case IDM_NEWSWITCHER: {

						SPete_EffectNode CandidateNode;

						int nHighestInputIndex=-1;

						CandidateNode.m_eType=eType_Effect_Switcher;
						CandidateNode.m_nSwitcherInputCount=2;

						SPete_DialogArgs DialogArgs;
						DialogArgs.m_pGraph=pGraph;
						DialogArgs.m_pNode=&CandidateNode;

						const int nDialogBoxResult=
							DialogBoxParam(
							g_hWindowInstance,
							MAKEINTRESOURCE(IDD_PROPERTIES_SWITCHER),
							g_hWindow,
							EditGraph_SwitcherProperties_Callback,
							(U32)(&DialogArgs));

						if (nDialogBoxResult==IDOK) {

							GraphUtil_MakeSwitcherNode(
								pGraph,
								CandidateNode.m_nSwitcherInputCount,
								nPosX,nPosY);

						}

					}break;

					case IDM_FILE_PROPERTIES: {

						EditGraph_FilePropertiesDialog(pGraph);
												   
					}break;

					default: {
						// do nothing
					}break;

				}

				DestroyMenu(hGeneralPopupMenu);

			} else {

				SPete_Node* pNode=g_Selection.m_pNodeEntry->m_pNode;

				EPete_NodeType eNodeType=pNode->m_eType;

				EEditGraph_SelectionType eSelectType=g_Selection.m_eType;

				UINT ResourceID;

				if ((eSelectType==eSelect_Input)||
					(eSelectType==eSelect_Parameter_Input)) {

					ResourceID=IDC_CONNECTIONPOPUP;

				} else {

					if ((eNodeType==eType_Parameter_External)||
						(eNodeType==eType_Effect_Input)||
						(eNodeType==eType_Effect_Switcher)) {

						ResourceID=IDC_NODEPOPUP;

					} else {

						ResourceID=IDC_NODEPOPUP_NOPROPERTIES;

					}

				}

				HMENU hGeneralPopupMenu=LoadMenu(g_hWindowInstance,MAKEINTRESOURCE(ResourceID));
				HMENU hPopupSubMenu=GetSubMenu(hGeneralPopupMenu,0);

				int nWindowPosX;
				int nWindowPosY;

				EditGraph_GraphToWindowSpace(nPosX,nPosY,&nWindowPosX,&nWindowPosY);

				POINT ScreenPoint=
					{nWindowPosX,nWindowPosY};

				ClientToScreen(g_hWindow,&ScreenPoint);

				int nMenuSelection=TrackPopupMenu(hPopupSubMenu,
					(TPM_NONOTIFY|TPM_RETURNCMD|TPM_CENTERALIGN|TPM_VCENTERALIGN|TPM_LEFTBUTTON),
					ScreenPoint.x,ScreenPoint.y,
					0,g_hWindow,NULL);

				switch (nMenuSelection) {

					case IDM_DELETE: {

						if (pNode->m_eType==eType_Effect_Output) {
							MessageBox(NULL,"You cannot delete the output","",MB_OK);
						} else {
							GraphUtil_DeleteNode(pGraph,pNode);
						}

					}break;

					case IDM_PROPERTIES: {

						switch (eNodeType) {
								
							case eType_Parameter_External: {

								SPete_ParameterNode* pParamNode=
									(SPete_ParameterNode*)(pNode);

								SPete_ParameterNode CandidateNode=*pParamNode;

								SPete_DialogArgs DialogArgs;
								DialogArgs.m_pGraph=pGraph;
								DialogArgs.m_pNode=&CandidateNode;

								const int nDialogBoxResult=
									DialogBoxParam(
									g_hWindowInstance,
									MAKEINTRESOURCE(IDD_PROPERTIES_FFPARAM),
									g_hWindow,
									EditGraph_FFParamProperties_Callback,
									(U32)(&DialogArgs));

								if (nDialogBoxResult==IDOK) {

									*pParamNode=CandidateNode;

								}

														   
							}break;
								
							case eType_Effect_Input: {

								SPete_EffectNode* pEffectNode=
									(SPete_EffectNode*)(pNode);

								SPete_EffectNode CandidateNode=*pEffectNode;

								SPete_DialogArgs DialogArgs;
								DialogArgs.m_pGraph=pGraph;
								DialogArgs.m_pNode=&CandidateNode;

								const int nDialogBoxResult=
									DialogBoxParam(
									g_hWindowInstance,
									MAKEINTRESOURCE(IDD_PROPERTIES_INPUT),
									g_hWindow,
									EditGraph_InputProperties_Callback,
									(U32)(&DialogArgs));

								if (nDialogBoxResult==IDOK) {

									*pEffectNode=CandidateNode;

								}

														   
							}break;

							case eType_Effect_Switcher: {

								SPete_EffectNode* pEffectNode=
									(SPete_EffectNode*)(pNode);

								SPete_EffectNode CandidateNode=*pEffectNode;

								SPete_DialogArgs DialogArgs;
								DialogArgs.m_pGraph=pGraph;
								DialogArgs.m_pNode=&CandidateNode;

								const int nDialogBoxResult=
									DialogBoxParam(
									g_hWindowInstance,
									MAKEINTRESOURCE(IDD_PROPERTIES_SWITCHER),
									g_hWindow,
									EditGraph_SwitcherProperties_Callback,
									(U32)(&DialogArgs));

								if (nDialogBoxResult==IDOK) {

									*pEffectNode=CandidateNode;

								}

														   
							}break;

							default: {
								// do nothing
							}break;

						}

					}break;

					case IDM_DELETECONNECTION: {

						if (eSelectType==eSelect_Input) {

							SPete_EffectNode* pEffectNode=
								(SPete_EffectNode*)(pNode);

							const int nInputIndex=g_Selection.m_nIndex;

							pEffectNode->m_ppInputs[nInputIndex]=NULL;

						} else if (eSelectType==eSelect_Parameter_Input) {
						
							const int nParamIndex=g_Selection.m_nIndex;

							pNode->m_ppParameters[nParamIndex]=NULL;

						}

						g_Selection.m_pNodeEntry=NULL;

					}break;

				}

			}
			
			bUpdateWindow=true;

		}break;

		case eEvent_Mouse_Move: {

			bUpdateWindow=false;

		}break;

		default: {

			bUpdateWindow=false;

		}break;

	}

	g_eUIState=eNewState;

	return bUpdateWindow;

}

bool EditGraph_UIEvent_Dragging(SPete_GraphData* pGraph,EEditGraph_UIEvent eEvent,int nPosX,int nPosY,void* pExtraData) {

	bool bUpdateWindow;

	EPete_UIState eNewState=g_eUIState;

	switch (eEvent) {

		case eEvent_Mouse_LDown: {

			bUpdateWindow=false;

		}break;

		case eEvent_Mouse_LUp: {

			SPete_UISelection* pSelection=&g_Selection;
			SPete_NodeListEntry* pNodeEntry=pSelection->m_pNodeEntry;

			if (pNodeEntry!=NULL) {

				SPete_Node* pNode=pNodeEntry->m_pNode;

				switch (pSelection->m_eType) {

					case eSelect_Body: {

					}break;

					case eSelect_Connection: {

					}break;

					case eSelect_Input: {

						SPete_UISelection Candidate;

						EditGraph_GetSelectionAtPos(pGraph,nPosX,nPosY,&Candidate);

						const int nInputIndex=g_Selection.m_nIndex;

						SPete_UIPos EndPos;

						if ((Candidate.m_pNodeEntry!=NULL)&&
							(Candidate.m_eType==eSelect_Output)) {

							SPete_Node* pCandidateNode=Candidate.m_pNodeEntry->m_pNode;

							EditGraph_GetOutputPos(pCandidateNode,&EndPos);
				
							SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);

							pEffectNode->m_ppInputs[nInputIndex]=
								(SPete_EffectNode*)(pCandidateNode);

						} else {

							const int nInputTotal=GraphUtil_GetInputCount(pNode);

							EditGraph_GetInputPos(pNode,nInputIndex,nInputTotal,&EndPos);

						}

						g_Selection.m_pNodeEntry=NULL;

					}break;

					case eSelect_Output: {

						SPete_UISelection Candidate;

						EditGraph_GetSelectionAtPos(pGraph,nPosX,nPosY,&Candidate);

						const int nInputIndex=g_Selection.m_nIndex;

						SPete_UIPos EndPos;

						if ((Candidate.m_pNodeEntry!=NULL)&&
							(Candidate.m_eType==eSelect_Input)) {

							SPete_Node* pCandidateNode=Candidate.m_pNodeEntry->m_pNode;
							SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pCandidateNode);

							const int nInputIndex=Candidate.m_nIndex;

							pEffectNode->m_ppInputs[nInputIndex]=
								(SPete_EffectNode*)(pNode);

						}

						g_Selection.m_pNodeEntry=NULL;

					}break;

					case eSelect_Parameter_Input: {

						SPete_UISelection Candidate;

						EditGraph_GetSelectionAtPos(pGraph,nPosX,nPosY,&Candidate);

						const int nParamIndex=g_Selection.m_nIndex;

						SPete_UIPos EndPos;

						if ((Candidate.m_pNodeEntry!=NULL)&&
							(Candidate.m_eType==eSelect_Parameter_Output)) {

							SPete_ParameterNode* pOtherNode=(SPete_ParameterNode*)
								Candidate.m_pNodeEntry->m_pNode;
							
							pNode->m_ppParameters[nParamIndex]=pOtherNode;

							if ((pOtherNode->m_eType==eType_Parameter_External)&&
								(pNode->m_eType==eType_Effect_Plugin)) {

								char pDefaultName[1024];
								float DefaultValue;
								if (nParamIndex==0) {
									sprintf(pDefaultName,"%d-Bypass Effect",pOtherNode->m_nExternalIndex);
									DefaultValue=0.0f;
								} else {

									SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);

									SPete_PluginInfo* pPluginInfo=pEffectNode->m_pPluginInfo;

									SPete_PluginParameter* pParamInfo=
										&pPluginInfo->m_pParameterInfo[nParamIndex-1];

									sprintf(pDefaultName,"%d-%s",
										pOtherNode->m_nExternalIndex,
										pParamInfo->m_pName);

									DefaultValue=pParamInfo->m_Default;

								}

								strcpy(pOtherNode->m_pExternalLabel,pDefaultName);

								pOtherNode->m_ExternalDefault=DefaultValue;

							}

						}

						g_Selection.m_pNodeEntry=NULL;

					}break;

					case eSelect_Parameter_Output: {

						SPete_UISelection Candidate;

						EditGraph_GetSelectionAtPos(pGraph,nPosX,nPosY,&Candidate);

						const int nParamIndex=Candidate.m_nIndex;

						SPete_UIPos EndPos;

						if ((Candidate.m_pNodeEntry!=NULL)&&
							(Candidate.m_eType==eSelect_Parameter_Input)) {

							SPete_ParameterNode* pOtherNode=(SPete_ParameterNode*)
								Candidate.m_pNodeEntry->m_pNode;

							SPete_ParameterNode* pParamNode=(SPete_ParameterNode*)
								(pNode);

							pOtherNode->m_ppParameters[nParamIndex]=pParamNode;

							if ((pNode->m_eType==eType_Parameter_External)&&
								(pOtherNode->m_eType==eType_Effect_Plugin)) {

								char* pDefaultName;
								float DefaultValue;
								if (nParamIndex==0) {
									pDefaultName="Bypass Effect";
									DefaultValue=0.0f;
								} else {

									SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pOtherNode);

									SPete_PluginInfo* pPluginInfo=pEffectNode->m_pPluginInfo;

									SPete_PluginParameter* pParamInfo=
										&pPluginInfo->m_pParameterInfo[nParamIndex-1];

									pDefaultName=pParamInfo->m_pName;
									DefaultValue=pParamInfo->m_Default;
								}

								strcpy(pParamNode->m_pExternalLabel,pDefaultName);

								pParamNode->m_ExternalDefault=DefaultValue;

							}

						}

						g_Selection.m_pNodeEntry=NULL;

					}break;

				}

			}

			eNewState=eState_Idle;

			bUpdateWindow=true;

		}break;

		case eEvent_Mouse_LDoubleClick: {

			bUpdateWindow=false;

		}break;

		case eEvent_Mouse_RDown: {

			bUpdateWindow=false;

		}break;

		case eEvent_Mouse_Move: {

			SPete_UISelection* pSelection=&g_Selection;
			SPete_NodeListEntry* pNodeEntry=pSelection->m_pNodeEntry;

			if (pNodeEntry==NULL) {

				nPosX+=g_DrawOrigin.nX;
				nPosY+=g_DrawOrigin.nY;

				const int nDeltaX=(nPosX-pSelection->m_DragOffset.nX);
				const int nDeltaY=(nPosY-pSelection->m_DragOffset.nY);

				g_DrawOrigin.nX=(pSelection->m_OriginalDrawOrigin.nX+nDeltaX);
				g_DrawOrigin.nY=(pSelection->m_OriginalDrawOrigin.nY+nDeltaY);

			} else {

				SPete_Node* pNode=pNodeEntry->m_pNode;

				switch (pSelection->m_eType) {

					case eSelect_Body: {

						pNode->m_Pos.nX=
							(nPosX+pSelection->m_DragOffset.nX);

						pNode->m_Pos.nY=
							(nPosY+pSelection->m_DragOffset.nY);

					}break;

					case eSelect_Connection: {

					}break;

					case eSelect_Input: {

						SPete_UISelection Candidate;

						EditGraph_GetSelectionAtPos(pGraph,nPosX,nPosY,&Candidate);

						SPete_UIPos EndPos;

						if ((Candidate.m_pNodeEntry!=NULL)&&
							(Candidate.m_eType==eSelect_Output)) {

							SPete_Node* pCandidateNode=Candidate.m_pNodeEntry->m_pNode;

							EditGraph_GetOutputPos(pCandidateNode,&EndPos);

						} else {

							EndPos.nX=nPosX;
							EndPos.nY=nPosY;

						}

						g_Selection.m_ConnectionEnd=EndPos;

					}break;

					case eSelect_Output: {

						SPete_UISelection Candidate;

						EditGraph_GetSelectionAtPos(pGraph,nPosX,nPosY,&Candidate);

						SPete_UIPos EndPos;

						if ((Candidate.m_pNodeEntry!=NULL)&&
							(Candidate.m_eType==eSelect_Input)) {

							SPete_Node* pCandidateNode=Candidate.m_pNodeEntry->m_pNode;

							const int nInputTotal=GraphUtil_GetInputCount(pCandidateNode);

							const int nInputIndex=Candidate.m_nIndex;

							EditGraph_GetInputPos(pCandidateNode,nInputIndex,nInputTotal,&EndPos);

						} else {

							EndPos.nX=nPosX;
							EndPos.nY=nPosY;

						}

						g_Selection.m_ConnectionEnd=EndPos;

					}break;

					case eSelect_Parameter_Input: {

						SPete_UISelection Candidate;

						EditGraph_GetSelectionAtPos(pGraph,nPosX,nPosY,&Candidate);

						SPete_UIPos EndPos;

						if ((Candidate.m_pNodeEntry!=NULL)&&
							(Candidate.m_eType==eSelect_Parameter_Output)) {

							SPete_Node* pCandidateNode=Candidate.m_pNodeEntry->m_pNode;

							EditGraph_GetParameterOutputPos(pCandidateNode,&EndPos);

						} else {

							EndPos.nX=nPosX;
							EndPos.nY=nPosY;

						}

						g_Selection.m_ConnectionEnd=EndPos;

					}break;
				
					case eSelect_Parameter_Output: {

						SPete_UISelection Candidate;

						EditGraph_GetSelectionAtPos(pGraph,nPosX,nPosY,&Candidate);

						SPete_UIPos EndPos;

						if ((Candidate.m_pNodeEntry!=NULL)&&
							(Candidate.m_eType==eSelect_Parameter_Input)) {

							SPete_Node* pCandidateNode=Candidate.m_pNodeEntry->m_pNode;

							const int nParamIndex=Candidate.m_nIndex;
							const int nParamTotal=GraphUtil_GetParameterCount(pCandidateNode);

							EditGraph_GetParameterPos(pCandidateNode,nParamIndex,nParamTotal,&EndPos);

						} else {

							EndPos.nX=nPosX;
							EndPos.nY=nPosY;

						}

						g_Selection.m_ConnectionEnd=EndPos;

					}break;
				}

			}

			bUpdateWindow=true;

		}break;

		default: {

			bUpdateWindow=false;

		}break;

	}

	g_eUIState=eNewState;

	return bUpdateWindow;

}

void EditGraph_GetSelectionAtPos(SPete_GraphData* pGraph,int nPosX,int nPosY,SPete_UISelection* poutSelection) {

	SPete_NodeListEntry* pCurrent=pGraph->m_pNodeListHead;

	bool bSelectionFound=false;	

	SPete_UIPos Pos={nPosX,nPosY};

	while ((pCurrent!=NULL)&&(!bSelectionFound)) {

		SPete_Node* pNode=pCurrent->m_pNode;

		SPete_UIPos* pPos=&pNode->m_Pos;
		SPete_UIPos* pSize=&pNode->m_Size;

		const int nCentreX=pPos->nX;
		const int nCentreY=pPos->nY;

		const int nWidth=pSize->nX;
		const int nHeight=pSize->nY;

		const int nHalfWidth=(nWidth/2);
		const int nHalfHeight=(nHeight/2);

		const int nBoxLeftX=(nCentreX-nHalfWidth);
		const int nBoxRightX=(nCentreX+nHalfWidth);

		const int nBoxTopY=(nCentreY-nHalfHeight);
		const int nBoxBottomY=(nCentreY+nHalfHeight);

		if ((nPosX>=nBoxLeftX)&&
			(nPosX<nBoxRightX)&&
			(nPosY>=nBoxTopY)&&
			(nPosY<nBoxBottomY)) {

			poutSelection->m_pNodeEntry=GraphUtil_FindListEntry(pGraph,pNode);
			poutSelection->m_eType=eSelect_Body;
			poutSelection->m_nIndex=0;
			poutSelection->m_DragOffset.nX=nCentreX-nPosX;
			poutSelection->m_DragOffset.nY=nCentreY-nPosY;
			poutSelection->m_ConnectionEnd.nX=nPosX;
			poutSelection->m_ConnectionEnd.nY=nPosY;

			bSelectionFound=true;

		}

		const int nParamCount=GraphUtil_GetParameterCount(pNode);

		int nCount;
		for (nCount=0; nCount<nParamCount; nCount+=1) {

			SPete_UIRect BoundingBox;
			EditGraph_GetParameterBox(pNode,nCount,&BoundingBox);

			const bool bIsInsideBox=
				EditGraph_IsInBox(&Pos,&BoundingBox);

			bool bIsInsideLine;
			if (pNode->m_ppParameters[nCount]==NULL) {

				bIsInsideLine=false;

			} else {

				SPete_UILine BoundingLine;
				EditGraph_GetParameterLine(pNode,nCount,&BoundingLine);

				bIsInsideLine=
					EditGraph_IsNearLine(&Pos,&BoundingLine,nLineTolerance);

			}

			if ((bIsInsideBox)||(bIsInsideLine)) {

				poutSelection->m_pNodeEntry=GraphUtil_FindListEntry(pGraph,pNode);
				poutSelection->m_eType=eSelect_Parameter_Input;
				poutSelection->m_nIndex=nCount;
				poutSelection->m_DragOffset.nX=nCentreX-nPosX;
				poutSelection->m_DragOffset.nY=nCentreY-nPosY;
				poutSelection->m_ConnectionEnd.nX=nPosX;
				poutSelection->m_ConnectionEnd.nY=nPosY;

				bSelectionFound=true;

			}

		}

		SPete_UIRect ParamOutputBox;
		EditGraph_GetParameterOutputBox(pNode,&ParamOutputBox);

		const bool bIsInParamOutputBox=
			EditGraph_IsInBox(&Pos,&ParamOutputBox);

		if (bIsInParamOutputBox) {

			poutSelection->m_pNodeEntry=GraphUtil_FindListEntry(pGraph,pNode);
			poutSelection->m_eType=eSelect_Parameter_Output;
			poutSelection->m_nIndex=0;
			poutSelection->m_DragOffset.nX=nCentreX-nPosX;
			poutSelection->m_DragOffset.nY=nCentreY-nPosY;
			poutSelection->m_ConnectionEnd.nX=nPosX;
			poutSelection->m_ConnectionEnd.nY=nPosY;

			bSelectionFound=true;

		}

		const int nInputCount=GraphUtil_GetInputCount(pNode);

		for (nCount=0; nCount<nInputCount; nCount+=1) {

			SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);

			SPete_UIRect BoundingBox;
			EditGraph_GetInputBox(pNode,nCount,&BoundingBox);

			const bool bIsInsideBox=
				EditGraph_IsInBox(&Pos,&BoundingBox);

			bool bIsInsideLine;
			if (pEffectNode->m_ppInputs[nCount]==NULL) {

				bIsInsideLine=false;

			} else {

				SPete_UILine BoundingLine;
				EditGraph_GetInputLine(pNode,nCount,&BoundingLine);

				bIsInsideLine=
					EditGraph_IsNearLine(&Pos,&BoundingLine,nLineTolerance);

			}

			if ((bIsInsideBox)||(bIsInsideLine)) {

				poutSelection->m_pNodeEntry=GraphUtil_FindListEntry(pGraph,pNode);
				poutSelection->m_eType=eSelect_Input;
				poutSelection->m_nIndex=nCount;
				poutSelection->m_DragOffset.nX=nCentreX-nPosX;
				poutSelection->m_DragOffset.nY=nCentreY-nPosY;
				poutSelection->m_ConnectionEnd.nX=nPosX;
				poutSelection->m_ConnectionEnd.nY=nPosY;

				bSelectionFound=true;

			}


		}

		SPete_UIRect ImageOutputBox;
		EditGraph_GetImageOutputBox(pNode,&ImageOutputBox);

		const bool bIsInImageOutputBox=
			EditGraph_IsInBox(&Pos,&ImageOutputBox);

		if (bIsInImageOutputBox) {

			poutSelection->m_pNodeEntry=GraphUtil_FindListEntry(pGraph,pNode);
			poutSelection->m_eType=eSelect_Output;
			poutSelection->m_nIndex=0;
			poutSelection->m_DragOffset.nX=nCentreX-nPosX;
			poutSelection->m_DragOffset.nY=nCentreY-nPosY;
			poutSelection->m_ConnectionEnd.nX=nPosX;
			poutSelection->m_ConnectionEnd.nY=nPosY;

			bSelectionFound=true;

		}

		pCurrent=pCurrent->m_pNext;

	}

	if (!bSelectionFound) {

		poutSelection->m_pNodeEntry=NULL;
		poutSelection->m_DragOffset.nX=(nPosX+g_DrawOrigin.nX);
		poutSelection->m_DragOffset.nY=(nPosY+g_DrawOrigin.nY);
		poutSelection->m_OriginalDrawOrigin=g_DrawOrigin;
		poutSelection->m_ConnectionEnd.nX=nPosX;
		poutSelection->m_ConnectionEnd.nY=nPosY;

	}

}

void EditGraph_WindowToGraphSpace(int nWindowPosX,int nWindowPosY,int* poutGraphPosX,int* poutGraphPosY) {

	*poutGraphPosX=(nWindowPosX-g_DrawOrigin.nX);
	*poutGraphPosY=(nWindowPosY-g_DrawOrigin.nY);

}

void EditGraph_GraphToWindowSpace(int nGraphPosX,int nGraphPosY,int* poutWindowPosX,int* poutWindowPosY) {

	*poutWindowPosX=(nGraphPosX+g_DrawOrigin.nX);
	*poutWindowPosY=(nGraphPosY+g_DrawOrigin.nY);

}

void EditGraph_ClearBackground(HDC hDeviceContext,int nWidth,int nHeight,U32 Colour) {

	HPEN hCurrentPen=
		CreatePen(PS_SOLID,1,Colour);

	SelectObject(hDeviceContext,hCurrentPen);

	LOGBRUSH CurrentBrush;
	CurrentBrush.lbStyle=BS_SOLID;
	CurrentBrush.lbColor=Colour;

	HBRUSH hCurrentBrush=CreateBrushIndirect(&CurrentBrush);

	SelectObject(hDeviceContext,hCurrentBrush);

	Rectangle(hDeviceContext,
		0,0,
		nWidth,nHeight);

	DeleteObject(hCurrentBrush);

	DeleteObject(hCurrentPen);

}

bool EditGraph_GetFileToOpen(char* pExtension,char* pTitle,char* poutFileName) {

	char pFilter[16];

	OPENFILENAME OpenFileData;
	OpenFileData.lStructSize=sizeof(OpenFileData);
	OpenFileData.hwndOwner=g_hWindow;
	OpenFileData.hInstance=g_hWindowInstance;
	strcpy(pFilter,"*.");
	strcat(pFilter,pExtension);
	pFilter[strlen(pFilter)+1]='\0';
	OpenFileData.lpstrFilter=pFilter;
	OpenFileData.lpstrCustomFilter=NULL;
	OpenFileData.nMaxCustFilter=0;
	OpenFileData.nFilterIndex=0;
	OpenFileData.lpstrFile=poutFileName;
	OpenFileData.nMaxFile=(MAX_PATH-1);
	OpenFileData.lpstrFileTitle=NULL;
	OpenFileData.nMaxFileTitle=0;
	OpenFileData.lpstrInitialDir=NULL;
	OpenFileData.lpstrTitle=pTitle;
	OpenFileData.Flags=OFN_FILEMUSTEXIST;
	OpenFileData.nFileOffset=0;
	OpenFileData.nFileExtension=0;
	OpenFileData.lpstrDefExt=pExtension;
	OpenFileData.lCustData=0;
	OpenFileData.lpfnHook=NULL;
	OpenFileData.lpTemplateName=NULL;

	BOOL bOpenFileNameResult=
		GetOpenFileName(&OpenFileData);

//	if (bOpenFileNameResult) {
//		strcpy(poutFileName,OpenFileData.lpstrFile);
//	}
	
	return bOpenFileNameResult;

}

BOOL CALLBACK EditGraph_FFParamProperties_Callback(HWND hDialogBox,UINT Message,WPARAM wParam,LPARAM lParam) {

	static SPete_ParameterNode* pParamNode=NULL;
	static SPete_GraphData* pGraph=NULL;

	switch (Message) {

		case WM_INITDIALOG: {

			HWND hwndOwner; 
			RECT rc, rcDlg, rcOwner; 
 
			if ((hwndOwner = GetParent(hDialogBox)) == NULL) 
			{
				hwndOwner = GetDesktopWindow(); 
			}

			GetWindowRect(hwndOwner, &rcOwner); 
			GetWindowRect(hDialogBox, &rcDlg); 
			CopyRect(&rc, &rcOwner); 
 
			 // Offset the owner and dialog box rectangles so that 
			 // right and bottom values represent the width and 
			 // height, and then offset the owner again to discard 
			 // space taken up by the dialog box. 
 
			OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top); 
			OffsetRect(&rc, -rc.left, -rc.top); 
			OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom); 
 
			 // The new position is the sum of half the remaining 
			 // space and the owner's original position. 
 
			SetWindowPos(hDialogBox, 
				HWND_TOP, 
				rcOwner.left + (rc.right / 2), 
				rcOwner.top + (rc.bottom / 2), 
				0, 0,          // ignores size arguments 
				SWP_NOSIZE); 
 
			SPete_DialogArgs* pDialogArgs=(SPete_DialogArgs*)(lParam);

			pParamNode=(SPete_ParameterNode*)(pDialogArgs->m_pNode);
			pGraph=pDialogArgs->m_pGraph;

			char pIndexString[256];

			sprintf(pIndexString,"%d",pParamNode->m_nExternalIndex+1);

			SendDlgItemMessage(
				hDialogBox,
				IDC_FFINDEX,
				WM_SETTEXT,
				0,
				(LPARAM)(pIndexString));

			SendDlgItemMessage(
				hDialogBox,
				IDC_FFNAME,
				WM_SETTEXT,
				0,
				(LPARAM)(pParamNode->m_pExternalLabel));

			char pDefaultString[256];

			sprintf(pDefaultString,"%f",pParamNode->m_ExternalDefault);

			SendDlgItemMessage(
				hDialogBox,
				IDC_FFDEFAULT,
				WM_SETTEXT,
				0,
				(LPARAM)(pDefaultString));

			UINT CheckState;

			if (pParamNode->m_ExternalType==FF_TYPE_BOOLEAN) {
				CheckState=BST_CHECKED;
			} else {
				CheckState=BST_UNCHECKED;
			}

			SendDlgItemMessage(
				hDialogBox,
				IDC_FFISBOOLEAN,
				BM_SETCHECK,
				CheckState,
				0);

			return TRUE;

		}break;

		case WM_COMMAND: {

			if (LOWORD(wParam)==IDOK) {

				char pIndexString[256];

				SendDlgItemMessage(
					hDialogBox,
					IDC_FFINDEX,
					WM_GETTEXT,
					256,
					(LPARAM)(pIndexString));

				const int nExternalIndex=atoi(pIndexString)-1;

				bool bIsInUse=false;

				SPete_NodeListEntry* pCurrentEntry=pGraph->m_pNodeListHead;
				while (pCurrentEntry!=NULL) {

					SPete_Node* pNode=pCurrentEntry->m_pNode;

					if (pNode->m_eType==eType_Parameter_External) {

						SPete_ParameterNode* pParamNode=
							(SPete_ParameterNode*)(pNode);

						if (pParamNode->m_nExternalIndex==nExternalIndex) {
							bIsInUse=true;
						}
					
					}

					pCurrentEntry=pCurrentEntry->m_pNext;
				}

				if ((bIsInUse)&&(pParamNode->m_nExternalIndex!=nExternalIndex)) {
					MessageBox(g_hWindow,"The FF Parameter Index is already in use, please choose a different one","",MB_OK);
					break;
				}

				pParamNode->m_nExternalIndex=nExternalIndex;

				SendDlgItemMessage(
					hDialogBox,
					IDC_FFNAME,
					WM_GETTEXT,
					16,
					(LPARAM)(pParamNode->m_pExternalLabel));

				char pDefaultString[256];

				SendDlgItemMessage(
					hDialogBox,
					IDC_FFDEFAULT,
					WM_GETTEXT,
					256,
					(LPARAM)(pDefaultString));

				pParamNode->m_ExternalDefault=atof(pDefaultString);

				UINT CheckState=SendDlgItemMessage(
					hDialogBox,
					IDC_FFISBOOLEAN,
					BM_GETCHECK,
					0,
					0);

				if (CheckState==BST_CHECKED) {
					pParamNode->m_ExternalType=FF_TYPE_BOOLEAN;
				} else {
					pParamNode->m_ExternalType=FF_TYPE_STANDARD;
				}

				EndDialog(hDialogBox, LOWORD(wParam));
				return TRUE;				
				
			} else if (LOWORD(wParam)==IDCANCEL) {

				EndDialog(hDialogBox, LOWORD(wParam));
				return TRUE;
			}

		}break;

	}

	return FALSE;

}

void EditGraph_GetParameterBox(SPete_Node* pNode,int nParamIndex,SPete_UIRect* poutBox) {

	const int nParamTotal=GraphUtil_GetParameterCount(pNode);

	SPete_UIPos ParamOrigin;
	EditGraph_GetParameterPos(pNode,nParamIndex,nParamTotal,&ParamOrigin);

	const int nHalfForkSeperation=(nParamTridentSeperation/2);

	const int nLeftX=ParamOrigin.nX-(nHalfForkSeperation*2);
	const int nRightX=ParamOrigin.nX+nParamTridentEndWidth+nParamTridentStartWidth;

	const int nTopY=ParamOrigin.nY-nHalfForkSeperation;
	const int nBottomY=ParamOrigin.nY+nHalfForkSeperation;

	poutBox->left=nLeftX;
	poutBox->right=nRightX;
	poutBox->top=nTopY;
	poutBox->bottom=nBottomY;

}

bool EditGraph_IsInBox(SPete_UIPos* pPos,SPete_UIRect* pBox) {

	return (
		(pPos->nX>=pBox->left)&&
		(pPos->nX<pBox->right)&&
		(pPos->nY>=pBox->top)&&
		(pPos->nY<pBox->bottom));
	
}

void EditGraph_GetParameterOutputBox(SPete_Node* pNode,SPete_UIRect* poutBox) {

	const bool bHasOutput=GraphUtil_HasParameterOutput(pNode);

	if (!bHasOutput) {

		poutBox->left=0;
		poutBox->right=0;
		poutBox->top=0;
		poutBox->bottom=0;

		return;

	}

	SPete_UIPos ParamOrigin;
	EditGraph_GetParameterOutputPos(pNode,&ParamOrigin);

	const int nHalfForkSeperation=(nParamTridentSeperation/2);

	const int nLeftX=ParamOrigin.nX-nParameterOutputSpikeWidth;
	const int nRightX=ParamOrigin.nX+nHalfForkSeperation;

	const int nTopY=ParamOrigin.nY-(nHalfForkSeperation*2);
	const int nBottomY=ParamOrigin.nY+nHalfForkSeperation;

	poutBox->left=nLeftX;
	poutBox->right=nRightX;
	poutBox->top=nTopY;
	poutBox->bottom=nBottomY;

}

void EditGraph_GetInputBox(SPete_Node* pNode,int nInputIndex,SPete_UIRect* poutBox) {

	const int nInputTotal=GraphUtil_GetInputCount(pNode);

	SPete_UIPos InputOrigin;
	EditGraph_GetInputPos(pNode,nInputIndex,nInputTotal,&InputOrigin);

	const int nHalfForkSeperation=(nInputTridentSeperation/2);

	const int nLeftX=InputOrigin.nX-nHalfForkSeperation;
	const int nRightX=InputOrigin.nX+nHalfForkSeperation;

	const int nTopY=InputOrigin.nY-(nHalfForkSeperation*2);
	const int nBottomY=InputOrigin.nY+nInputTridentEndHeight+nInputTridentStartHeight;

	poutBox->left=nLeftX;
	poutBox->right=nRightX;
	poutBox->top=nTopY;
	poutBox->bottom=nBottomY;

}

void EditGraph_GetImageOutputBox(SPete_Node* pNode,SPete_UIRect* poutBox) {

	const bool bHasOutput=GraphUtil_HasImageOutput(pNode);

	if (!bHasOutput) {

		poutBox->left=0;
		poutBox->right=0;
		poutBox->top=0;
		poutBox->bottom=0;

		return;

	}

	SPete_UIPos OutputOrigin;
	EditGraph_GetOutputPos(pNode,&OutputOrigin);

	const int nHalfForkSeperation=(nInputTridentSeperation/2);

	const int nLeftX=OutputOrigin.nX-nHalfForkSeperation;
	const int nRightX=OutputOrigin.nX+nHalfForkSeperation;

	const int nTopY=OutputOrigin.nY-nOutputSpikeHeight;
	const int nBottomY=OutputOrigin.nY+nHalfForkSeperation;

	poutBox->left=nLeftX;
	poutBox->right=nRightX;
	poutBox->top=nTopY;
	poutBox->bottom=nBottomY;

}

void EditGraph_LoadFreeFrameDLL(SPete_GraphData* pGraph,char* pFileName) {

	HMODULE hModule=LoadLibrary(pFileName);

	if (hModule==INVALID_HANDLE_VALUE) {
		char MessageString[1024];
		sprintf(MessageString,"Input file '%s' could not be loaded",pFileName);
		MessageBox(NULL,MessageString,"",MB_OK);
		return;
	}

	HRSRC hChainDataResource=FindResource(
		hModule,
		MAKEINTRESOURCE(CHAIN_DATA_RESOURCE_ID),
		"GRAPH");

	if (hChainDataResource==NULL) {
		MessageBox(NULL,"CHAIN_DATA resource not found","",MB_OK);
		return;
	}
	
	HGLOBAL hChainData=LoadResource(
		hModule,hChainDataResource);

	if (hChainData==NULL) {
		MessageBox(NULL,"CHAIN_DATA resource couldn't be loaded","",MB_OK);
		return;
	}

	const DWORD nSizeOfChainData=SizeofResource(
		hModule,hChainDataResource);

	if (nSizeOfChainData==0) {
		MessageBox(NULL,"CHAIN_DATA resource couldn't be sized","",MB_OK);
		return;
	}

	char* pChainData=(char*)LockResource(hChainData);
	if (pChainData==NULL) {
		MessageBox(NULL,"CHAIN_DATA resource couldn't be locked","",MB_OK);
		return;
	}

	char* pFlatDataCopy=(char*)(malloc(nSizeOfChainData));

	if (pFlatDataCopy==NULL) {
		MessageBox(NULL,"Memory allocation failed","",MB_OK);
		return;
	}

	memcpy(pFlatDataCopy,pChainData,nSizeOfChainData);

	UnlockResource(hChainData);

	GraphUtil_InitFromFlat(pGraph,pFlatDataCopy,nSizeOfChainData);

	FreeLibrary(hModule);
	
}

BOOL CALLBACK EditGraph_InputProperties_Callback(HWND hDialogBox,UINT Message,WPARAM wParam,LPARAM lParam) {

	static SPete_EffectNode* pEffectNode=NULL;
	static SPete_GraphData* pGraph=NULL;

	switch (Message) {

		case WM_INITDIALOG: {

			HWND hwndOwner; 
			RECT rc, rcDlg, rcOwner; 
 
			if ((hwndOwner = GetParent(hDialogBox)) == NULL) 
			{
				hwndOwner = GetDesktopWindow(); 
			}

			GetWindowRect(hwndOwner, &rcOwner); 
			GetWindowRect(hDialogBox, &rcDlg); 
			CopyRect(&rc, &rcOwner); 
 
			 // Offset the owner and dialog box rectangles so that 
			 // right and bottom values represent the width and 
			 // height, and then offset the owner again to discard 
			 // space taken up by the dialog box. 
 
			OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top); 
			OffsetRect(&rc, -rc.left, -rc.top); 
			OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom); 
 
			 // The new position is the sum of half the remaining 
			 // space and the owner's original position. 
 
			SetWindowPos(hDialogBox, 
				HWND_TOP, 
				rcOwner.left + (rc.right / 2), 
				rcOwner.top + (rc.bottom / 2), 
				0, 0,          // ignores size arguments 
				SWP_NOSIZE); 
 
			SPete_DialogArgs* pDialogArgs=(SPete_DialogArgs*)(lParam);

			pEffectNode=(SPete_EffectNode*)(pDialogArgs->m_pNode);
			pGraph=pDialogArgs->m_pGraph;

			char pIndexString[256];

			sprintf(pIndexString,"%d",pEffectNode->m_nInputIndex+1);

			SendDlgItemMessage(
				hDialogBox,
				IDC_INPUTINDEX,
				WM_SETTEXT,
				0,
				(LPARAM)(pIndexString));

			return TRUE;

		}break;

		case WM_COMMAND: {

			if (LOWORD(wParam)==IDOK) {

				char pIndexString[256];

				SendDlgItemMessage(
					hDialogBox,
					IDC_INPUTINDEX,
					WM_GETTEXT,
					256,
					(LPARAM)(pIndexString));

				const int nInputIndex=atoi(pIndexString)-1;

				bool bIsInUse=false;

				SPete_NodeListEntry* pCurrentEntry=pGraph->m_pNodeListHead;
				while (pCurrentEntry!=NULL) {

					SPete_Node* pNode=pCurrentEntry->m_pNode;

					if (pNode->m_eType==eType_Effect_Input) {

						SPete_EffectNode* pEffectNode=
							(SPete_EffectNode*)(pNode);

						if (pEffectNode->m_nInputIndex==nInputIndex) {
							bIsInUse=true;
						}
					
					}

					pCurrentEntry=pCurrentEntry->m_pNext;
				}

				if ((bIsInUse)&&(pEffectNode->m_nInputIndex!=nInputIndex)) {
					MessageBox(g_hWindow,"That input number is already in use, please choose a different one","",MB_OK);
					break;
				}

				if (nInputIndex!=0) {
					MessageBox(g_hWindow,"This version can only have a single input node, numbered 1","",MB_OK);
					break;
				}

				pEffectNode->m_nInputIndex=nInputIndex;

				EndDialog(hDialogBox, LOWORD(wParam));
				return TRUE;				
				
			} else if (LOWORD(wParam)==IDCANCEL) {

				EndDialog(hDialogBox, LOWORD(wParam));
				return TRUE;
			}

		}break;

	}

	return FALSE;

}

void EditGraph_GetParameterLine(SPete_Node* pNode,int nParamIndex,SPete_UILine* poutLine) {

	const int nParamTotal=GraphUtil_GetParameterCount(pNode);

	EditGraph_GetParameterPos(pNode,nParamIndex,nParamTotal,&poutLine->Start);

	SPete_ParameterNode* pConnection=pNode->m_ppParameters[nParamIndex];

	if (pConnection==NULL) {
		poutLine->End=poutLine->Start;
		return;
	}

	EditGraph_GetParameterOutputPos(pConnection,&poutLine->End);

}

void EditGraph_GetInputLine(SPete_Node* pNode,int nInputIndex,SPete_UILine* poutLine) {

	const int nInputTotal=GraphUtil_GetInputCount(pNode);

	if (nInputTotal==0) {

		poutLine->Start.nX=0;
		poutLine->Start.nY=0;
		poutLine->End.nX=0;
		poutLine->End.nY=0;

		return;

	}

	EditGraph_GetInputPos(pNode,nInputIndex,nInputTotal,&poutLine->Start);

	SPete_EffectNode* pEffectNode=(SPete_EffectNode*)(pNode);

	SPete_EffectNode* pConnection=pEffectNode->m_ppInputs[nInputIndex];

	if (pConnection==NULL) {
		poutLine->End=poutLine->Start;
		return;
	}

	EditGraph_GetOutputPos(pConnection,&poutLine->End);

}

bool EditGraph_IsNearLine(SPete_UIPos* pPos,SPete_UILine* pLine,int nTolerance) {

	const float StartX=pLine->Start.nX;
	const float StartY=pLine->Start.nY;

	const float DeltaX=(pLine->End.nX-StartX);
	const float DeltaY=(pLine->End.nY-StartY);

	const float PosX=(pPos->nX-StartX);
	const float PosY=(pPos->nY-StartY);

	const float DeltaMagSquared=
		(DeltaX*DeltaX)+
		(DeltaY*DeltaY);

	const float DeltaMag=sqrt(DeltaMagSquared);

	const float DirectionX=(DeltaX/DeltaMag);
	const float DirectionY=(DeltaY/DeltaMag);

	const float PosDotDirection=
		(PosX*DirectionX)+
		(PosY*DirectionY);

	if ((PosDotDirection<0.0f)||
		(PosDotDirection>=DeltaMag)) {

		return false;

	}

	const float NearestX=PosDotDirection*DirectionX;	
	const float NearestY=PosDotDirection*DirectionY;
	
	const float PosToNearestX=(PosX-NearestX);
	const float PosToNearestY=(PosY-NearestY);

	const float PosToNearestSquared=
		(PosToNearestX*PosToNearestX)+
		(PosToNearestY*PosToNearestY);

	const float PosToNearestDistance=sqrt(PosToNearestSquared);

	if (PosToNearestDistance<nTolerance) {
		return true;
	} else {
		return false;
	}

}

bool EditGraph_WriteResourceAsFile(char* pFileName) {

	HMODULE hModule=NULL;

	HRSRC hFlatDLLResource=FindResource(
		hModule,
		MAKEINTRESOURCE(IDR_FFTEMPLATE_DLL),
		"FLAT_DLL");

	if (hFlatDLLResource==NULL) {
		MessageBox(NULL,"FLAT_DLL resource not found","",MB_OK);
		return false;
	}
	
	HGLOBAL hFlatDLL=LoadResource(
		hModule,hFlatDLLResource);

	if (hFlatDLL==NULL) {
		MessageBox(NULL,"FLAT_DLL resource couldn't be loaded","",MB_OK);
		return false;
	}

	const DWORD nSizeOfFlatDLL=SizeofResource(
		hModule,hFlatDLLResource);

	if (nSizeOfFlatDLL==0) {
		MessageBox(NULL,"FLAT_DLL resource couldn't be sized","",MB_OK);
		return false;
	}

	char* pFlatDLL=(char*)LockResource(hFlatDLL);
	if (pFlatDLL==NULL) {
		MessageBox(NULL,"FLAT_DLL resource couldn't be locked","",MB_OK);
		return false;
	}

	FILE* pOutputFile=fopen(pFileName,"wb");
	if (pOutputFile==NULL) {
		char pMessageString[1024];
		sprintf(pMessageString,"Couldn't create output file '%s'",pFileName);
		MessageBox(NULL,pMessageString,"",MB_OK);
		return false;
	}

	fwrite(pFlatDLL,nSizeOfFlatDLL,1,pOutputFile);

	fclose(pOutputFile);

	UnlockResource(hFlatDLL);

	return true;

}

BOOL CALLBACK EditGraph_SwitcherProperties_Callback(HWND hDialogBox,UINT Message,WPARAM wParam,LPARAM lParam) {

	static SPete_EffectNode* pEffectNode=NULL;
	static SPete_GraphData* pGraph=NULL;

	switch (Message) {

		case WM_INITDIALOG: {

			HWND hwndOwner; 
			RECT rc, rcDlg, rcOwner; 
 
			if ((hwndOwner = GetParent(hDialogBox)) == NULL) 
			{
				hwndOwner = GetDesktopWindow(); 
			}

			GetWindowRect(hwndOwner, &rcOwner); 
			GetWindowRect(hDialogBox, &rcDlg); 
			CopyRect(&rc, &rcOwner); 
 
			 // Offset the owner and dialog box rectangles so that 
			 // right and bottom values represent the width and 
			 // height, and then offset the owner again to discard 
			 // space taken up by the dialog box. 
 
			OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top); 
			OffsetRect(&rc, -rc.left, -rc.top); 
			OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom); 
 
			 // The new position is the sum of half the remaining 
			 // space and the owner's original position. 
 
			SetWindowPos(hDialogBox, 
				HWND_TOP, 
				rcOwner.left + (rc.right / 2), 
				rcOwner.top + (rc.bottom / 2), 
				0, 0,          // ignores size arguments 
				SWP_NOSIZE); 
 
			SPete_DialogArgs* pDialogArgs=(SPete_DialogArgs*)(lParam);

			pEffectNode=(SPete_EffectNode*)(pDialogArgs->m_pNode);
			pGraph=pDialogArgs->m_pGraph;

			char pInputCountString[256];

			sprintf(pInputCountString,"%d",pEffectNode->m_nSwitcherInputCount);

			SendDlgItemMessage(
				hDialogBox,
				IDC_INPUTCOUNT,
				WM_SETTEXT,
				0,
				(LPARAM)(pInputCountString));

			return TRUE;

		}break;

		case WM_COMMAND: {

			if (LOWORD(wParam)==IDOK) {

				char pInputCountString[256];

				SendDlgItemMessage(
					hDialogBox,
					IDC_INPUTCOUNT,
					WM_GETTEXT,
					256,
					(LPARAM)(pInputCountString));

				const int nInputCount=atoi(pInputCountString);

				if (nInputCount<1) {
					MessageBox(g_hWindow,"You must have at least one switcher input","",MB_OK);
					break;
				} else if (nInputCount>=PETE_MAX_SWITCHER_INPUTS) {
					char pMessageString[1024];
					sprintf(pMessageString,"You cannot have more that %d switcher inputs",PETE_MAX_SWITCHER_INPUTS);
					MessageBox(g_hWindow,pMessageString,"",MB_OK);
					break;
				}

				pEffectNode->m_nSwitcherInputCount=nInputCount;

				EndDialog(hDialogBox, LOWORD(wParam));
				return TRUE;				
				
			} else if (LOWORD(wParam)==IDCANCEL) {

				EndDialog(hDialogBox, LOWORD(wParam));
				return TRUE;
			}

		}break;

	}

	return FALSE;

}

void EditGraph_FilePropertiesDialog(SPete_GraphData* pGraph) {

	SPete_GraphData CandidateGraph=*pGraph;

	const int nDialogBoxResult=
		DialogBoxParam(
		g_hWindowInstance,
		MAKEINTRESOURCE(IDD_PROPERTIES_FILE),
		g_hWindow,
		EditGraph_FileProperties_Callback,
		(U32)(&CandidateGraph));

	if (nDialogBoxResult==IDOK) {

		*pGraph=CandidateGraph;

	}

}

BOOL CALLBACK EditGraph_FileProperties_Callback(HWND hDialogBox,UINT Message,WPARAM wParam,LPARAM lParam) {

	static SPete_GraphData* pGraph=NULL;

	switch (Message) {

		case WM_INITDIALOG: {

			HWND hwndOwner; 
			RECT rc, rcDlg, rcOwner; 
 
			if ((hwndOwner = GetParent(hDialogBox)) == NULL) 
			{
				hwndOwner = GetDesktopWindow(); 
			}

			GetWindowRect(hwndOwner, &rcOwner); 
			GetWindowRect(hDialogBox, &rcDlg); 
			CopyRect(&rc, &rcOwner); 
 
			 // Offset the owner and dialog box rectangles so that 
			 // right and bottom values represent the width and 
			 // height, and then offset the owner again to discard 
			 // space taken up by the dialog box. 
 
			OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top); 
			OffsetRect(&rc, -rc.left, -rc.top); 
			OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom); 
 
			 // The new position is the sum of half the remaining 
			 // space and the owner's original position. 
 
			SetWindowPos(hDialogBox, 
				HWND_TOP, 
				rcOwner.left + (rc.right / 2), 
				rcOwner.top + (rc.bottom / 2), 
				0, 0,          // ignores size arguments 
				SWP_NOSIZE); 
 
			pGraph=(SPete_GraphData*)(lParam);

			SendDlgItemMessage(
				hDialogBox,
				IDC_PLUGIN_NAME,
				WM_SETTEXT,
				0,
				(LPARAM)(pGraph->m_pName));

			SendDlgItemMessage(
				hDialogBox,
				IDC_PLUGIN_4_LETTER_ID,
				WM_SETTEXT,
				0,
				(LPARAM)(pGraph->m_pUniqueID));

			SendDlgItemMessage(
				hDialogBox,
				IDC_PLUGIN_DESCRIPTION,
				WM_SETTEXT,
				0,
				(LPARAM)(pGraph->m_pDescription));

			return TRUE;

		}break;

		case WM_COMMAND: {

			if (LOWORD(wParam)==IDOK) {

				SendDlgItemMessage(
					hDialogBox,
					IDC_PLUGIN_NAME,
					WM_GETTEXT,
					sizeof(pGraph->m_pName)-1,
					(LPARAM)(pGraph->m_pName));

				char pUniqueIDString[256];

				SendDlgItemMessage(
					hDialogBox,
					IDC_PLUGIN_4_LETTER_ID,
					WM_GETTEXT,
					255,
					(LPARAM)(pUniqueIDString));

				if (strlen(pUniqueIDString)!=4) {
					MessageBox(g_hWindow,"The ID must be 4 letters long","",MB_OK);
					break;
				}

				pGraph->m_pUniqueID[0]=pUniqueIDString[0];
				pGraph->m_pUniqueID[1]=pUniqueIDString[1];
				pGraph->m_pUniqueID[2]=pUniqueIDString[2];
				pGraph->m_pUniqueID[3]=pUniqueIDString[3];
				pGraph->m_pUniqueID[4]='\0';

				SendDlgItemMessage(
					hDialogBox,
					IDC_PLUGIN_DESCRIPTION,
					WM_GETTEXT,
					sizeof(pGraph->m_pDescription)-1,
					(LPARAM)(pGraph->m_pDescription));

				EndDialog(hDialogBox, LOWORD(wParam));
				return TRUE;				
				
			} else if (LOWORD(wParam)==IDCANCEL) {

				EndDialog(hDialogBox, LOWORD(wParam));
				return TRUE;
			}

		}break;

	}

	return FALSE;

}
