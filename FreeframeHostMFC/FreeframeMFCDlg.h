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

// FreeframeMFCDlg.h : header file
//

#if !defined(AFX_FREEFRAMEMFCDLG_H__48CB7B07_88D5_4F03_80E6_C51A522B58FE__INCLUDED_)
#define AFX_FREEFRAMEMFCDLG_H__48CB7B07_88D5_4F03_80E6_C51A522B58FE__INCLUDED_

#include "FreeframeHost.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




VOID CALLBACK TimerProc(
  HWND hwnd,         // handle to window
  UINT uMsg,         // WM_TIMER message
  UINT_PTR idEvent,  // timer identifier
  DWORD dwTime       // current system time
);



/////////////////////////////////////////////////////////////////////////////
// CFreeframeMFCDlg dialog

class CFreeframeMFCDlg : public CDialog
{
// Construction
public:
	//CFileDialog AVIfile;
	void SetParameterFromSlider(CSliderCtrl * pSlider, DWORD index);
	void UpdateParameterDisplay(int n);
	RECT screen;
	FreeframeHost plugHost;
	HBITMAP hBitmap;
	HDRAWDIB hdibdc;
	CFreeframeMFCDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFreeframeMFCDlg)
	enum { IDD = IDD_FREEFRAMEMFC_DIALOG };
	CListBox	m_choosePlugin;
	CSliderCtrl	m_slider3;
	CSliderCtrl	m_slider2;
	CSliderCtrl	m_slider1;
	CSliderCtrl	m_slider0;
	CString	m_plugName;
	CString	m_plugMinorVersion;
	CString	m_plugMajorVersion;
	CString	m_plugID;
	CString	m_plugType;
	CString	m_param0Val;
	CString	m_param2Val;
	CString	m_param1Val;
	CString	m_param3Val;
	CString	m_numParameters;
	CString	m_param0name;
	CString	m_param1name;
	CString	m_param2name;
	CString	m_param3name;
	CString	m_param0default;
	CString	m_param0display;
	CString	m_param1default;
	CString	m_param1display;
	CString	m_param2default;
	CString	m_param2display;
	CString	m_param3default;
	CString	m_param3display;
	CString	m_AVIfile;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFreeframeMFCDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
	AVIPlayer player;
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFreeframeMFCDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLoadAVI();
	afx_msg void OnGetFrame();
	afx_msg void OnPlay();
	afx_msg void OnPause();
	afx_msg void OnPlugInfo();
	afx_msg void OnPlugInitialise();
	afx_msg void OnGetEffFrame();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPlayEffected();
	afx_msg void OnReleasedcaptureSlider0(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetNumParameters();
	afx_msg void OnGetparameternames();
	afx_msg void OnGetparamdefaults();
	afx_msg void OnGetParameters();
	afx_msg void OnGetParameterDisplays();
	afx_msg void OnReleasedcaptureSlider1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSlider2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSlider3(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeinitialise();
	afx_msg void OnButton8();
	afx_msg void OnChooseAvi();
	afx_msg void OnSelchangeChoosePlugin();
	afx_msg void OnLoadPlugin();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FREEFRAMEMFCDLG_H__48CB7B07_88D5_4F03_80E6_C51A522B58FE__INCLUDED_)
