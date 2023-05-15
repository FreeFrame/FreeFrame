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
#include "MetaImageDfxDlg.h"
#include "DfxDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMetaImageDfxDlg::CMetaImageDfxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMetaImageDfxDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMetaImageDfxDlg)
	//}}AFX_DATA_INIT

}

void CMetaImageDfxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMetaImageDfxDlg)
	//}}AFX_DATA_MAP
}

void CMetaImageDfxDlg::EnableControls()
{
	UpdateData();
}

BOOL CMetaImageDfxDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	PlaceDfxDialog(GetSafeHwnd()); // replaces the window so that it is not over the preview! the "Center"-property of your dialog must be enabled to center it horizontally!

	SetDlgItemInt(IDC_METAIMAGE_NUMBEROFINPUTS_EDIT,m_nInputCount,FALSE);

	EnableControls();

	return TRUE;
}


void CMetaImageDfxDlg::OnOK() {

	BOOL bSuccess;
	m_nInputCount=GetDlgItemInt(IDC_METAIMAGE_NUMBEROFINPUTS_EDIT,&bSuccess,FALSE);

	CDialog::OnOK();

}

