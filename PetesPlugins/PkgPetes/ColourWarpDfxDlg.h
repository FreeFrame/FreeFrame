#if !defined(DFXDLG__100556625728379859__INCLUDED_)
#define DFXDLG__100556625728379859__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

enum EColourWarpStyle {
	ePolariStyle,
	eSineSlurStyle,
	eStutterStyle,
	eCustomStyle,
	eColourWarpStyle_end
};

class CColourWarpDfxDlg : public CDialog
{
public:
	CColourWarpDfxDlg(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CColourWarpDfxDlg)
	enum { IDD = IDD_DIALOG_TIMEWARP };
	BOOL	m_bDisplay0;
	BOOL	m_bDisplay1;
	BOOL	m_bDisplay2;
	BOOL	m_bConfirm0;
	BOOL	m_bConfirm1;
	BOOL	m_bConfirm2;
	//}}AFX_DATA

	EColourWarpStyle m_eStyle;
	int m_nFramesToStore;

	//{{AFX_VIRTUAL(CColourWarpDfxDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	void EnableControls();

	virtual void OnOK();
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}

#endif
