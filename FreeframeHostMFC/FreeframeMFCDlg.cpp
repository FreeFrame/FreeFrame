/*

Copyright (c) 2003, Marcus Clements www.freeframe.org
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of FreeFrame nor the names of its
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

// FreeframeMFCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FreeframeMFC.h"
#include "FreeframeMFCDlg.h"
//#include <afxcmn.h>
#include "FFUtils.h"
#include <afxdlgs.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFreeframeMFCDlg dialog

CFreeframeMFCDlg::CFreeframeMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFreeframeMFCDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFreeframeMFCDlg)
	m_plugName = _T("");
	m_plugMinorVersion = _T("");
	m_plugMajorVersion = _T("");
	m_plugID = _T("");
	m_plugType = _T("");
	m_param0Val = _T("");
	m_param2Val = _T("");
	m_param1Val = _T("");
	m_param3Val = _T("");
	m_numParameters = _T("");
	m_param0name = _T("");
	m_param1name = _T("");
	m_param2name = _T("");
	m_param3name = _T("");
	m_param0default = _T("");
	m_param0display = _T("");
	m_param1default = _T("");
	m_param1display = _T("");
	m_param2default = _T("");
	m_param2display = _T("");
	m_param3default = _T("");
	m_param3display = _T("");
	m_AVIfile = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFreeframeMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFreeframeMFCDlg)
	DDX_Control(pDX, IDC_CHOOSE_PLUGIN, m_choosePlugin);
	DDX_Control(pDX, IDC_SLIDER3, m_slider3);
	DDX_Control(pDX, IDC_SLIDER2, m_slider2);
	DDX_Control(pDX, IDC_SLIDER1, m_slider1);
	DDX_Control(pDX, IDC_SLIDER0, m_slider0);
	DDX_Text(pDX, IDC_PLUG_NAME, m_plugName);
	DDX_Text(pDX, IDC_PLUG_MINOR_VERSION, m_plugMinorVersion);
	DDX_Text(pDX, IDC_PLUG_MAJOR_VERSION, m_plugMajorVersion);
	DDX_Text(pDX, IDC_PLUG_ID, m_plugID);
	DDX_Text(pDX, IDC_PLUG_TYPE, m_plugType);
	DDX_Text(pDX, IDC_PARAM0_VAL, m_param0Val);
	DDX_Text(pDX, IDC_PARAM2_VAL, m_param2Val);
	DDX_Text(pDX, IDC_PARAM1_VAL, m_param1Val);
	DDX_Text(pDX, IDC_PARAM3_VAL, m_param3Val);
	DDX_Text(pDX, IDC_NUMPARAMETERS, m_numParameters);
	DDX_Text(pDX, IDC_PARAM0, m_param0name);
	DDX_Text(pDX, IDC_PARAM1, m_param1name);
	DDX_Text(pDX, IDC_PARAM2, m_param2name);
	DDX_Text(pDX, IDC_PARAM3, m_param3name);
	DDX_Text(pDX, IDC_PARAM0_DEFAULT, m_param0default);
	DDX_Text(pDX, IDC_PARAM0_DISPLAY, m_param0display);
	DDX_Text(pDX, IDC_PARAM1_DEFAULT, m_param1default);
	DDX_Text(pDX, IDC_PARAM1_DISPLAY, m_param1display);
	DDX_Text(pDX, IDC_PARAM2_DEFAULT, m_param2default);
	DDX_Text(pDX, IDC_PARAM2_DISPLAY, m_param2display);
	DDX_Text(pDX, IDC_PARAM3_DEFAULT, m_param3default);
	DDX_Text(pDX, IDC_PARAM3_DISPLAY, m_param3display);
	DDX_Text(pDX, IDC_AVI_FILE, m_AVIfile);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFreeframeMFCDlg, CDialog)
	//{{AFX_MSG_MAP(CFreeframeMFCDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnLoadAVI)
	ON_BN_CLICKED(IDC_BUTTON2, OnGetFrame)
	ON_BN_CLICKED(IDC_BUTTON3, OnPlay)
	ON_BN_CLICKED(IDC_BUTTON4, OnPause)
	ON_BN_CLICKED(IDC_BUTTON5, OnPlugInfo)
	ON_BN_CLICKED(IDC_BUTTON6, OnPlugInitialise)
	ON_BN_CLICKED(IDC_BUTTON7I, OnPlugInstantiate)
	ON_BN_CLICKED(IDC_GET_EFF_FRAME, OnGetEffFrame)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_PLAY_EFFECTED, OnPlayEffected)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER0, OnReleasedcaptureSlider0)
	ON_BN_CLICKED(IDC_GETNUMPARAMETERS, OnGetNumParameters)
	ON_BN_CLICKED(IDC_GETPARAMETERNAMES, OnGetparameternames)
	ON_BN_CLICKED(IDC_GETPARAMDEFAULTS, OnGetparamdefaults)
	ON_BN_CLICKED(IDC_GET_PARAMETERS, OnGetParameters)
	ON_BN_CLICKED(IDC_GET_PARAMETER_DISPLAYS, OnGetParameterDisplays)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, OnReleasedcaptureSlider1)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER2, OnReleasedcaptureSlider2)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER3, OnReleasedcaptureSlider3)
	ON_BN_CLICKED(IDC_DEINSTANTIATE, OnDeinstantiate)
	ON_BN_CLICKED(IDC_DEINITIALISE, OnDeinitialise)
	ON_BN_CLICKED(IDC_BUTTON8, OnButton8)
	ON_LBN_SELCHANGE(IDC_CHOOSE_PLUGIN, OnSelchangeChoosePlugin)
	ON_BN_CLICKED(IDC_LOAD_PLUGIN, OnLoadPlugin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFreeframeMFCDlg message handlers

BOOL CFreeframeMFCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	hBitmap = 0;
	hdibdc = DrawDibOpen();
	if (!hdibdc) {
		MessageBox("Couldn't DrawDibOpen");
	}

	//populate plugin chooser
	WIN32_FIND_DATA findData;

	HANDLE hdir = FindFirstFile("plugins/*.dll", &findData);
	if (hdir != INVALID_HANDLE_VALUE) {
		m_choosePlugin.AddString( findData.cFileName );
		while (FindNextFile( hdir, &findData) ) {
			m_choosePlugin.AddString( findData.cFileName );
		}
	}

	player.window = this;
	screen.bottom = 0;
	screen.left = 130;
	screen.right = 0;
	screen.top = 30;


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFreeframeMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFreeframeMFCDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if (IsIconic())
	{
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// Draw the icon
		//dc.DrawIcon(x, y, m_hIcon);
	}
	else if (player.pBitmapInfo)
	{
		hBitmap = CreateDIBitmap(	
			dc, 
			&player.pBitmapInfo->bmiHeader, 
			CBM_INIT, 
			player.pFrame,
			player.pBitmapInfo,
			DIB_RGB_COLORS );

		if (!hBitmap) {
			LPVOID lpMsgBuf;
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
			);
			// Process any inserts in lpMsgBuf.
			// ...
			// Display the string.
			MessageBox((LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
			// Free the buffer.
			LocalFree( lpMsgBuf );

			//MessageBox("Bitmap not created");
			return;
		}

		if (!DrawDibDraw(	
			hdibdc,
			dc,
			screen.left, 
			screen.top, 
			player.videoInfo.frameWidth, 
			player.videoInfo.frameHeight,
			&player.pBitmapInfo->bmiHeader,
			player.pFrame,
			0,
			0,
			player.videoInfo.frameWidth, 
			player.videoInfo.frameHeight,
			0  ) ) {
			MessageBox("Couldn't draw bitmap");
		}

	}
	DeleteObject(hBitmap);
	this->ReleaseDC(&dc);

	CDialog::OnPaint();	
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFreeframeMFCDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFreeframeMFCDlg::OnLoadAVI() 
{
	CFileDialog AVIfile(TRUE);
	AVIfile.m_ofn.lpstrInitialDir = "video";
	AVIfile.DoModal();
	m_AVIfile = AVIfile.m_ofn.lpstrFile;
	UpdateData(FALSE);

	player.openAVI(m_AVIfile.GetBuffer(1));

	screen.bottom = player.videoInfo.frameHeight + screen.top;
	screen.right = player.videoInfo.frameWidth + screen.left;
}

void CFreeframeMFCDlg::OnGetFrame() 
{

	player.getFrame();
	
	if (!player.pBitmapInfo) {
		MessageBox("No bitmap info");
	}

	player.nextFrame();
	InvalidateRect(NULL, FALSE); 
}
	

void CFreeframeMFCDlg::OnPlay() 
{
	SetTimer(0, 40, 0);
}

void CFreeframeMFCDlg::OnPause() 
{
	KillTimer(0);
	KillTimer(1);

}

void CFreeframeMFCDlg::OnPlugInfo() 
{
	plugHost.getPlugInfo();
	char tempStr[32];
	char pluginID[5];
	pluginID[4] = 0;

	sprintf(tempStr,"%d", plugHost.pPlugInfo->APIMajorVersion);
	m_plugMajorVersion = tempStr;

	sprintf(tempStr,"%d", plugHost.pPlugInfo->APIMinorVersion);
	m_plugMinorVersion = tempStr;

	memcpy(pluginID, plugHost.pPlugInfo->uniqueID, 4);
	m_plugID = tempStr;

	memcpy(tempStr, plugHost.pPlugInfo->pluginName, 16);
	tempStr[16] = 0;
	m_plugName = tempStr;

	sprintf(tempStr,"%d", plugHost.pPlugInfo->pluginType);
	m_plugType = tempStr;

	UpdateData(FALSE);
}

void CFreeframeMFCDlg::OnPlugInitialise() 
{
	DWORD result = plugHost.initialise(&player.videoInfo);
	if (result) {
		char tempStr[32];
		sprintf(tempStr, "Initialise Error %d", result);
		MessageBox(tempStr);
	}
}

void CFreeframeMFCDlg::OnPlugInstantiate() 
{
	DWORD result = plugHost.instantiate(&player.videoInfo);
	if (result == FF_FAIL) {
		char tempStr[32];
		sprintf(tempStr, "Instantiate Error %d", result);
		MessageBox(tempStr);
	}
	char tempStr[32];
	sprintf(tempStr, "Instantiate OK %d", result);
	MessageBox(tempStr);
}

void CFreeframeMFCDlg::OnDeinstantiate() 
{
	DWORD result = plugHost.deInstantiate();
	if (result) {
		char tempStr[32];
		sprintf(tempStr, "Deinstantiate Error %d", result);
		MessageBox(tempStr);
	}
}

void CFreeframeMFCDlg::OnGetEffFrame() 
{

	player.getFrame();

	if (!player.pBitmapInfo) {
		MessageBox("No bitmap info");
	}
	
	DWORD result = plugHost.processFrame(player.pFrame);

	if (result) {
		char tempStr[32];
		sprintf(tempStr, "Process Frame Error %d", result);
		MessageBox(tempStr);
	}

	player.nextFrame();
	InvalidateRect(NULL, FALSE); 
	
}

void CFreeframeMFCDlg::OnTimer(UINT nIDEvent) 
{
	player.getFrame();
	
	if (nIDEvent == 1) {
		
		plugHost.processFrame(player.pFrame);
	}	
		
	player.nextFrame();
	
	
	InvalidateRect(&screen, FALSE); 
	
	
	CDialog::OnTimer(nIDEvent);
}

void CFreeframeMFCDlg::OnPlayEffected() 
{
	SetTimer(1, 40, 0);
	
}

BOOL CFreeframeMFCDlg::DestroyWindow() 
{
	DrawDibClose(hdibdc);
	
	return CDialog::DestroyWindow();
}

BOOL CFreeframeMFCDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	switch (wParam) {
	case 0:
		break;
	case IDC_SLIDER0:
		CSliderCtrl *slider0 = (CSliderCtrl*) this->GetDlgItem(IDC_SLIDER0);
		
		int pos = slider0->GetPos();
		char tempStr[16];
		sprintf(tempStr, "%d", pos);
		SetDlgItemText(IDC_PARAM0_VAL, tempStr);
		break;
	}
	
	return CDialog::OnNotify(wParam, lParam, pResult);
}


void CFreeframeMFCDlg::OnGetNumParameters() 
{
	char tempStr[32];
	plugHost.getNumParameters();
	sprintf(tempStr,"%d", plugHost.numParameters);
	m_numParameters = tempStr;
	UpdateData(FALSE);
	
}

void CFreeframeMFCDlg::OnGetparameternames() 
{
	plugHost.getParameterNames();
	if (plugHost.numParameters > 0) m_param0name = plugHost.parameters[0]->name;		
	if (plugHost.numParameters > 1) m_param1name = plugHost.parameters[1]->name;		
	if (plugHost.numParameters > 2) m_param2name = plugHost.parameters[2]->name;		
	if (plugHost.numParameters > 3) m_param3name = plugHost.parameters[3]->name;
	UpdateData(FALSE);
}

void CFreeframeMFCDlg::OnGetparamdefaults() 
{
	char tempStr[32];
	plugHost.getParameterDefaults();
	if (plugHost.numParameters > 0) {
		sprintf(tempStr,"%f", plugHost.parameters[0]->defaultValue);
		m_param0default = tempStr;
	}
	if (plugHost.numParameters > 1) {
		sprintf(tempStr,"%f", plugHost.parameters[1]->defaultValue);
		m_param1default = tempStr;
	}
	if (plugHost.numParameters > 2) {
		sprintf(tempStr,"%f", plugHost.parameters[2]->defaultValue);
		m_param2default = tempStr;
	}
	if (plugHost.numParameters > 3) {
		sprintf(tempStr,"%f", plugHost.parameters[3]->defaultValue);
		m_param3default = tempStr;
	}
	UpdateData(FALSE);
}

void CFreeframeMFCDlg::OnGetParameters() 
{
	char tempStr[32];
	plugHost.getParameters();
	if (plugHost.numParameters > 0) {
		sprintf(tempStr,"%f", plugHost.parameters[0]->value);
		m_param0Val = tempStr;
	}
	if (plugHost.numParameters > 1) {
		sprintf(tempStr,"%f", plugHost.parameters[1]->value);
		m_param1Val = tempStr;
	}
	if (plugHost.numParameters > 2) {
		sprintf(tempStr,"%f", plugHost.parameters[2]->value);
		m_param2Val = tempStr;
	}
	if (plugHost.numParameters > 3) {
		sprintf(tempStr,"%f", plugHost.parameters[3]->value);
		m_param3Val = tempStr;
	}
	UpdateData(FALSE);
}

void CFreeframeMFCDlg::OnGetParameterDisplays() 
{
	plugHost.getParameterDisplays();
	if (plugHost.numParameters > 0) m_param0display = plugHost.parameters[0]->displayValue;		
	if (plugHost.numParameters > 1) m_param1display = plugHost.parameters[1]->displayValue;		
	if (plugHost.numParameters > 2) m_param2display = plugHost.parameters[2]->displayValue;		
	if (plugHost.numParameters > 3) m_param3display = plugHost.parameters[3]->displayValue;
	UpdateData(FALSE);
}

void CFreeframeMFCDlg::UpdateParameterDisplay(int n)
{
	OnGetParameters();
	OnGetParameterDisplays();
}

void CFreeframeMFCDlg::OnReleasedcaptureSlider0(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetParameterFromSlider(&m_slider0, 0);
	*pResult = 0;
}

void CFreeframeMFCDlg::OnReleasedcaptureSlider1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetParameterFromSlider(&m_slider1, 1);
	*pResult = 0;
}

void CFreeframeMFCDlg::OnReleasedcaptureSlider2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetParameterFromSlider(&m_slider2, 2);
	*pResult = 0;
}

void CFreeframeMFCDlg::OnReleasedcaptureSlider3(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetParameterFromSlider(&m_slider3, 3);
	*pResult = 0;
}


void CFreeframeMFCDlg::OnDeinitialise() 
{
	plugHost.deInitialise();	
}

void CFreeframeMFCDlg::SetParameterFromSlider(CSliderCtrl *pSlider, DWORD index)
{
	float value = intToParam(pSlider->GetPos(), pSlider->GetRangeMin(), pSlider->GetRangeMax() );
	plugHost.setParameter(index, value);
	UpdateParameterDisplay(index);

}

void CFreeframeMFCDlg::OnButton8() 
{
	// TODO: Add your control notification handler code here
	
}

void CFreeframeMFCDlg::OnChooseAvi() 
{

}


void CFreeframeMFCDlg::OnSelchangeChoosePlugin() 
{
	UpdateData(FALSE);
}

void CFreeframeMFCDlg::OnLoadPlugin() 
{
	plugHost.unloadPlugin();
	char plugName[256];
	m_choosePlugin.GetText( m_choosePlugin.GetCurSel(), plugName);
	plugHost.loadPlugin(plugName);	
}
