#if !defined(DFXDLG__100556625728379859__INCLUDED_)
#define DFXDLG__100556625728379859__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

#include "windows.h"
#include "commdlg.h"

enum ETextOverlayStyle {
	ePolariStyle,
	eSineSlurStyle,
	eStutterStyle,
	eCustomStyle,
	eTextOverlayStyle_end
};

class CFontInfo
{
public:
    CFontInfo() { hFont=NULL; }
    ~CFontInfo() { if (hFont) DeleteObject(hFont); }
    void InitChooseFont(HWND hwnd,LPLOGFONT pLogFont);

    HFONT hFont;                // HFONT used to draw with this font
//    LOGFONT lf;                 // font description for CreateFontIndirect
	CHOOSEFONT cf;              // font description for ChooseFont dialog
	TEXTMETRIC tm;              // text metrics, e.g. character height
    ABC abc[224];               // character widths
};

class CTextOverlayDfxDlg : public CDialog
{
public:
	CTextOverlayDfxDlg(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CTextOverlayDfxDlg)
	enum { IDD = IDD_DIALOG_TEXTOVERLAY };
	BOOL	m_bDisplay0;
	BOOL	m_bDisplay1;
	BOOL	m_bDisplay2;
	BOOL	m_bConfirm0;
	BOOL	m_bConfirm1;
	BOOL	m_bConfirm2;
	//}}AFX_DATA

	ETextOverlayStyle m_eStyle;
	int m_nFramesToStore;
	bool m_bStyleChanged;
	LOGFONT* m_pLogFont;
	LOGFONT m_CandidateFont;

	HGLOBAL m_hNewFontImageData;
	int m_nNewFontImageDataWidth;
	int m_nNewFontImageDataHeight;

	CFontInfo fi;                   // currently opened font
	OPENFILENAME OpenFileName;      // used for GetSaveFileName
	char m_TextFileName[MAX_PATH];    // buffer for OpenFileName.lpstrFile
	char* m_pInputFileName;

	//{{AFX_VIRTUAL(CTextOverlayDfxDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	void EnableControls();

	void MeasureFont(HDC hdc);
	void LoadFont(void);
	void SaveFont(void);
	void BuildFontImage(void);
	void GetTextFileName(void);

	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);
};

//{{AFX_INSERT_LOCATION}}

#endif
