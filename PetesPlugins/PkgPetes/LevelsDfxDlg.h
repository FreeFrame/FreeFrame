#ifndef INCLUDE_LEVELSDFXDLG_H
#define INCLUDE_LEVELSDFXDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

enum ELevelsStyle {
	eUniformStyle,
	eSeperateRGBStyle,
	eAutoUniformStyle,
	eAutoSeperateRGBStyle,
	eLevelsStyle_end
};

class CLevelsDfxDlg : public CDialog
{
public:
	CLevelsDfxDlg(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CLevelsDfxDlg)
	enum { IDD = IDD_DIALOG_LEVELS };
	BOOL	m_bDisplay0;
	BOOL	m_bDisplay1;
	BOOL	m_bDisplay2;
	BOOL	m_bConfirm0;
	BOOL	m_bConfirm1;
	BOOL	m_bConfirm2;
	//}}AFX_DATA

	ELevelsStyle m_eStyle;
	bool m_bAllowInversion;
	bool m_bStyleChanged;

	//{{AFX_VIRTUAL(CLevelsDfxDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	void EnableControls();

	virtual void OnOK();
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}

#endif // INCLUDE_LEVELSDFXDLG_H
