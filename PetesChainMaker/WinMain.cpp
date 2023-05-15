// PetesChainMaker.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include <commdlg.h>

#include "EditGraph.h"
#include "RunGraph.h"
#include "GraphUtil.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

SPete_GraphData g_GraphData;

HINSTANCE g_hWindowInstance;
HWND g_hWindow;

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PETESCHAINMAKER, szWindowClass, MAX_LOADSTRING);

	MyRegisterClass(hInstance);

	GraphUtil_Init(&g_GraphData);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_PETESCHAINMAKER);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS ;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_PETESCHAINMAKER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);

	LOGBRUSH CurrentBrush;
	CurrentBrush.lbStyle=BS_SOLID;
	CurrentBrush.lbColor=0x00cccccc;

	HBRUSH hCurrentBrush=CreateBrushIndirect(&CurrentBrush);

	wcex.hbrBackground	= hCurrentBrush;//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_PETESCHAINMAKER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	g_hWindowInstance=hInst;

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

	g_hWindow=hWnd;

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	bool bUpdateWindow=false;

	switch (message)
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;

//				case IDM_RUN: {
//					EditGraph_RunGraph(&g_GraphData);
//				}break;
//
//				case IDM_FLATTEN: {
//					GraphUtil_FlattenGraph(&g_GraphData);
//					char* pFlatData=g_GraphData.m_pFlattenedData;
//					int nFlatDataSize=g_GraphData.m_nFlattenedDataSize;
//
//					char* pStoredData=(char*)malloc(nFlatDataSize);
//					memcpy(pStoredData,pFlatData,nFlatDataSize);
//
//					GraphUtil_DeInit(&g_GraphData);
//
//					GraphUtil_InitFromFlat(&g_GraphData,pStoredData,nFlatDataSize);
//
//				}break;

				case IDM_NEW: {

					const int nMessageBoxResult=
						MessageBox(NULL,"This will wipe the current graph","",MB_OKCANCEL);

					if (nMessageBoxResult==IDOK) {

						GraphUtil_Init(&g_GraphData);

						bUpdateWindow=true;

					}

				}break;

				case IDM_SAVEAS: {

					char pOutputFileName[MAX_PATH]="";

					OPENFILENAME OpenFileData;
					OpenFileData.lStructSize=sizeof(OpenFileData);
					OpenFileData.hwndOwner=hWnd;
					OpenFileData.hInstance=hInst;
					OpenFileData.lpstrFilter="*.dll";
					OpenFileData.lpstrCustomFilter=NULL;
					OpenFileData.nMaxCustFilter=0;
					OpenFileData.nFilterIndex=0;
					OpenFileData.lpstrFile=pOutputFileName;
					OpenFileData.nMaxFile=(MAX_PATH-1);
					OpenFileData.lpstrFileTitle=NULL;
					OpenFileData.nMaxFileTitle=0;
					OpenFileData.lpstrInitialDir=NULL;
					OpenFileData.lpstrTitle="Save As FreeFramePlugin...";
					OpenFileData.Flags=0;
					OpenFileData.nFileOffset=0;
					OpenFileData.nFileExtension=0;
					OpenFileData.lpstrDefExt="dll";
					OpenFileData.lCustData=0;
					OpenFileData.lpfnHook=NULL;
					OpenFileData.lpTemplateName=NULL;

					BOOL bOpenFileNameResult=
						GetSaveFileName(&OpenFileData);

					if (bOpenFileNameResult) {

						EditGraph_SaveAsFreeFrameDLL(
							&g_GraphData,
							OpenFileData.lpstrFile);

					}

				}break;

				case IDM_OPEN: {

					char pOutputFileName[MAX_PATH]="";

					OPENFILENAME OpenFileData;
					OpenFileData.lStructSize=sizeof(OpenFileData);
					OpenFileData.hwndOwner=hWnd;
					OpenFileData.hInstance=hInst;
					OpenFileData.lpstrFilter="*.dll";
					OpenFileData.lpstrCustomFilter=NULL;
					OpenFileData.nMaxCustFilter=0;
					OpenFileData.nFilterIndex=0;
					OpenFileData.lpstrFile=pOutputFileName;
					OpenFileData.nMaxFile=(MAX_PATH-1);
					OpenFileData.lpstrFileTitle=NULL;
					OpenFileData.nMaxFileTitle=0;
					OpenFileData.lpstrInitialDir=NULL;
					OpenFileData.lpstrTitle="Open FreeFramePlugin...";
					OpenFileData.Flags=0;
					OpenFileData.nFileOffset=0;
					OpenFileData.nFileExtension=0;
					OpenFileData.lpstrDefExt="dll";
					OpenFileData.lCustData=0;
					OpenFileData.lpfnHook=NULL;
					OpenFileData.lpTemplateName=NULL;

					BOOL bOpenFileNameResult=
						GetOpenFileName(&OpenFileData);

					if (bOpenFileNameResult) {

						EditGraph_LoadFreeFrameDLL(
							&g_GraphData,
							OpenFileData.lpstrFile);

						bUpdateWindow=true;

					}

				}break;

				case IDM_FILE_PROPERTIES: {

					EditGraph_FilePropertiesDialog(&g_GraphData);

				}break;

				case IDM_EXIT:
					GraphUtil_DeInit(&g_GraphData);
					DestroyWindow(hWnd);
				break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT: {
			PAINTSTRUCT PaintStruct;
			HDC hDeviceContext=BeginPaint(hWnd, &PaintStruct);

			// TODO: Add any drawing code here...
			RECT ClientRect;
			GetClientRect(hWnd, &ClientRect);

			const int nWidth=ClientRect.right-ClientRect.left;
			const int nHeight=ClientRect.bottom-ClientRect.top;

			HDC hMemoryDC=CreateCompatibleDC(hDeviceContext);
			HBITMAP hMemoryBitmap=
				CreateCompatibleBitmap(hDeviceContext,
				nWidth,nHeight);
			SelectObject(hMemoryDC,hMemoryBitmap);

//			DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER);
			EditGraph_DrawAll(hWnd,hMemoryDC,&ClientRect,&g_GraphData);

			BitBlt(hDeviceContext,0,0,nWidth,nHeight,hMemoryDC,0,0,SRCCOPY);

			DeleteObject(hMemoryDC);
			DeleteObject(hMemoryBitmap);

			EndPaint(hWnd, &PaintStruct);
		}break;

		case WM_DESTROY:
			GraphUtil_DeInit(&g_GraphData);
			PostQuitMessage(0);
			break;

		case WM_LBUTTONDOWN: {
			const int nPosX=LOWORD(lParam);
			const int nPosY=HIWORD(lParam);

			bUpdateWindow=EditGraph_UIEvent(&g_GraphData,eEvent_Mouse_LDown,nPosX,nPosY,NULL);

		}break;

		case WM_LBUTTONUP: {
			const int nPosX=LOWORD(lParam);
			const int nPosY=HIWORD(lParam);

			bUpdateWindow=EditGraph_UIEvent(&g_GraphData,eEvent_Mouse_LUp,nPosX,nPosY,NULL);

		}break;

		case WM_LBUTTONDBLCLK: {
			const int nPosX=LOWORD(lParam);
			const int nPosY=HIWORD(lParam);

			bUpdateWindow=EditGraph_UIEvent(&g_GraphData,eEvent_Mouse_LDoubleClick,nPosX,nPosY,NULL);

		}break;

		case WM_RBUTTONDOWN: {
			const int nPosX=LOWORD(lParam);
			const int nPosY=HIWORD(lParam);

			bUpdateWindow=EditGraph_UIEvent(&g_GraphData,eEvent_Mouse_RDown,nPosX,nPosY,NULL);

		}break;

		case WM_MOUSEMOVE: {
			const int nPosX=LOWORD(lParam);
			const int nPosY=HIWORD(lParam);

			bUpdateWindow=EditGraph_UIEvent(&g_GraphData,eEvent_Mouse_Move,nPosX,nPosY,NULL);

		}break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }

   if (bUpdateWindow) {
	   InvalidateRect(hWnd,NULL,FALSE);
	   UpdateWindow(hWnd);
   }

   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
