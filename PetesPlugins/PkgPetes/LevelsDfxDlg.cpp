/* Copyright (C) 2002 Pete Warden

Effect plugin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the Lesser GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "stdafx.h"
#include "LevelsDfxDlg.h"
#include "DfxDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLevelsDfxDlg::CLevelsDfxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLevelsDfxDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLevelsDfxDlg)
	//}}AFX_DATA_INIT

	m_bStyleChanged=false;

}

void CLevelsDfxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLevelsDfxDlg)
	//}}AFX_DATA_MAP
}

void CLevelsDfxDlg::EnableControls()
{
	UpdateData();
}

BOOL CLevelsDfxDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	PlaceDfxDialog(GetSafeHwnd()); // replaces the window so that it is not over the preview! the "Center"-property of your dialog must be enabled to center it horizontally!

    SendDlgItemMessage(
		IDC_LEVELS_STYLE_LIST,
		CB_SETCURSEL,
		m_eStyle,
		0L);
	EnableControls();

	return TRUE;
}


void CLevelsDfxDlg::OnOK() {

	ELevelsStyle NewStyle = (ELevelsStyle)SendDlgItemMessage(
		IDC_LEVELS_STYLE_LIST,
		CB_GETCURSEL,
		0,
		0L);

	m_bStyleChanged=(NewStyle!=m_eStyle);

	m_eStyle=NewStyle;

	CDialog::OnOK();

}

