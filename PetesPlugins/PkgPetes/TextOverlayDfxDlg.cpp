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
#include "TextOverlayDfxDlg.h"
#include "DfxDlg.h"
#include "DfxMemoryHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CFontInfo::InitChooseFont
//      Fill CHOOSEFONT cf so that it can be passed to ChooseFont
void CFontInfo::InitChooseFont(HWND hwnd,LPLOGFONT pLogFont)
{
//    ZeroMemory(&lf, sizeof(lf));
    ZeroMemory(&cf, sizeof(cf));
    cf.lStructSize = sizeof(cf);
    cf.hwndOwner = hwnd;
    cf.lpLogFont = pLogFont;
    cf.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
}
//---------------------------------------------------------------------------

//-------------------------- Global Variables -------------------------------

//HWND hWnd;
//HINSTANCE hInst;

//---------------------------------------------------------------------------

// ErrMsgBox
//      Displays a MsgBox with the error message for GetLastError.
//      Uses FormatMessage to retrieve the message.
//      The code was taken from the FormatMessage help topic.
//
void ErrMsgBox()
{
    LPVOID lpMsgBuf;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL
    );

    // Display the string.
    MessageBox(NULL, (char*)lpMsgBuf, "GetLastError", MB_OK|MB_ICONINFORMATION);

    // Free the buffer.
    LocalFree( lpMsgBuf );
}
//---------------------------------------------------------------------------

// MeasureFont
//      Fills CFontInfo fi (global) with text metrics and char widths
//      -> hdc: HDC that the font is currently selected into
//
void CTextOverlayDfxDlg::MeasureFont(HDC hdc)
{
    GetTextMetrics(hdc, &fi.tm);
    // Measure TrueType fonts with GetCharABCWidths:
    if (!GetCharABCWidths(hdc, 32, 255, fi.abc))
    // If it's not a TT font, use GetTextExtentPoint32 to fill array abc:
    {
        ZeroMemory(fi.abc, 224*sizeof(ABC));    // set abcA and abcC to 0
                                                // (abcB will be overwritten)
        SIZE Size;
        // for all characters, beginning with " ":
        for (int i=32; i < 256; i++)
        {
            // get width of character...
            GetTextExtentPoint32(hdc, (char*)&i, 1, &Size);
            // ...and store it in abcB:
            fi.abc[i-32].abcB = Size.cx;
        }
    }
}
//---------------------------------------------------------------------------

void CTextOverlayDfxDlg::LoadFont(void)
{

	// invoke ChooseFont common dialog:
    if (ChooseFont(&fi.cf))
    {
        // create an HFONT:
        if (fi.hFont) { DeleteObject(fi.hFont); fi.hFont = NULL; }
        fi.hFont = CreateFontIndirect(&m_CandidateFont);

        // get HDC:
		HWND hWnd=GetSafeHwnd();
        HDC hdc = ::GetDC(hWnd);

        // select font:
        SelectObject(hdc, fi.hFont);

        // get text metrics and char widths:
        MeasureFont(hdc);

        // release HDC:
        ::ReleaseDC(hWnd, hdc);

        // redraw window:
        ::RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
    }
}
//---------------------------------------------------------------------------

    #define GetFilePointer(x) SetFilePointer(x, 0, 0, FILE_CURRENT)
    #define ERROR_BREAK(x) { ErrMsgBox(); throw (int)(x); }

void CTextOverlayDfxDlg::SaveFont(void)
{
    HDC OffscrDC=NULL;
    HBITMAP OffscrBmp=NULL;
    LPBITMAPINFO lpbi=NULL;
    LPVOID lpvBits=NULL;
    HANDLE BmpFile=INVALID_HANDLE_VALUE;

	HWND hWnd=GetSafeHwnd();

try
{
    // Use fontname as filename by default:
    strcpy(OpenFileName.lpstrFile, m_pLogFont->lfFaceName);
    // Let user specify file to save to:
    if (!GetSaveFileName(&OpenFileName)) return;

    // Redraw window now so that the preview will not be erased later:
//    ::RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
//    ::UpdateWindow(hWnd);

    // Create an offscreen device context:
    OffscrDC = CreateCompatibleDC(0);

    if (fi.hFont) SelectObject(OffscrDC, fi.hFont);

    // Create an offscreen bitmap:
    int width=16*fi.tm.tmMaxCharWidth;
    int height=14*fi.tm.tmHeight;
    HBITMAP OffscrBmp = CreateCompatibleBitmap(OffscrDC, width, height);
    if (!OffscrBmp) ERROR_BREAK(0);
    // Select bitmap into DC:
    HBITMAP OldBmp = (HBITMAP)SelectObject(OffscrDC, OffscrBmp);
    if (!OldBmp) ERROR_BREAK(1);

    // Clear background to black:
    SelectObject(OffscrDC, GetStockObject(BLACK_BRUSH));
    Rectangle(OffscrDC, 0, 0, width, height);
    SetBkMode(OffscrDC, TRANSPARENT);     // do not fill character background
    SetTextColor(OffscrDC, RGB(255, 255, 255)); // text color white

    // Draw characters:
    unsigned char c;
    for (int y=0; y < 14; y++)
    for (int x=0; x < 16; x++)
    {
        c = (unsigned char)(y*16+x+32);
        TextOut(OffscrDC, x*fi.tm.tmMaxCharWidth-fi.abc[c-32].abcA,
            y*fi.tm.tmHeight, (const char*)&c, 1);
            // *) See remark in WM_PAINT handler as to why abcA is subtracted.
    }

// Following code to store an HDC to a BMP file was taken from the tutorial
// "Using the GDI to Take DirectDraw Screenshots", which can be found at
// www.mr-gamemaker.com.
// The code is ready to store other formats than monochrome, although we
// do not make use of that yet.

    // GetDIBits requires format info about the bitmap. We can have GetDIBits
    // fill a structure with that info if we pass a NULL pointer for lpvBits:
    // Reserve memory for bitmap info (BITMAPINFOHEADER + largest possible
    // palette):
    if ((lpbi = (LPBITMAPINFO)(new char[sizeof(BITMAPINFOHEADER) +
        256 * sizeof(RGBQUAD)])) == NULL) ERROR_BREAK(4);
    ZeroMemory(&lpbi->bmiHeader, sizeof(BITMAPINFOHEADER));
    lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    // Get info but first de-select OffscrBmp because GetDIBits requires it:
    SelectObject(OffscrDC, OldBmp);
    if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, NULL, lpbi, DIB_RGB_COLORS))
        ERROR_BREAK(5);

    // Reserve memory for bitmap bits:
    if ((lpvBits = new char[lpbi->bmiHeader.biSizeImage]) == NULL)
        ERROR_BREAK(6);

    // Have GetDIBits convert OffscrBmp to a DIB (device-independent bitmap):
    if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, lpvBits, lpbi,
        DIB_RGB_COLORS)) ERROR_BREAK(7);

    // Create a file to save the DIB to:
    if ((BmpFile = CreateFile(OpenFileName.lpstrFile,
                              GENERIC_WRITE,
                              0, NULL,
                              CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL)) == INVALID_HANDLE_VALUE) ERROR_BREAK(8);

    DWORD Written;    // number of bytes written by WriteFile

    // Write a file header to the file:
    BITMAPFILEHEADER bmfh;
    bmfh.bfType = 19778;        // 'BM'
    // bmfh.bfSize = ???        // we'll write that later
    // The bitmap format requires the Reserved fields to be 0.
    // Our format is DDF. We store ID values into these fields:
    bmfh.bfReserved1 = 'DD';    // "DirectDraw...
    bmfh.bfReserved2 = 'FF';    // "...Font File"
    // bmfh.bfOffBits = ???     // we'll write that later
    if (!WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, NULL))
        ERROR_BREAK(9);
    if (Written < sizeof(bmfh)) ERROR_BREAK(9);

    // Write BITMAPINFOHEADER to the file:
    if (!WriteFile(BmpFile, &lpbi->bmiHeader, sizeof(BITMAPINFOHEADER),
        &Written, NULL)) ERROR_BREAK(10);
    if (Written < sizeof(BITMAPINFOHEADER)) ERROR_BREAK(10);

    // Calculate size of palette:
    int PalEntries;
    // 16-bit or 32-bit bitmaps require bit masks:
    if (lpbi->bmiHeader.biCompression == BI_BITFIELDS) PalEntries = 3;
    else
        // bitmap is palettized?
        PalEntries = (lpbi->bmiHeader.biBitCount <= 8) ?
            // 2^biBitCount palette entries max.:
            1 << lpbi->bmiHeader.biBitCount
        // bitmap is TrueColor -> no palette:
        : 0;
    // If biClrsUsed use only biClrUsed palette entries:
    if (lpbi->bmiHeader.biClrUsed) PalEntries = lpbi->bmiHeader.biClrUsed;

    // Write palette to the file:
    if (PalEntries)
    {
        if (!WriteFile(BmpFile, &lpbi->bmiColors, PalEntries * sizeof(RGBQUAD),
            &Written, NULL)) ERROR_BREAK(11);
        if (Written < PalEntries * sizeof(RGBQUAD)) ERROR_BREAK(11);
    }

    // The current position in the file (at the beginning of the bitmap bits)
    // will be saved to the BITMAPFILEHEADER:
    bmfh.bfOffBits = GetFilePointer(BmpFile);

    // Write bitmap bits to the file:
    if (!WriteFile(BmpFile, lpvBits, lpbi->bmiHeader.biSizeImage,
        &Written, NULL)) ERROR_BREAK(12);
    if (Written < lpbi->bmiHeader.biSizeImage) ERROR_BREAK(12);

    // The current pos. in the file is the final file size and will be saved:
    bmfh.bfSize = GetFilePointer(BmpFile);

    // Store the font info and character widths to the file:
    if (WriteFile(BmpFile, &fi.tm, sizeof(fi.tm), &Written, NULL) &&
    WriteFile(BmpFile, fi.abc, 224*sizeof(ABC), &Written, NULL) &&
    WriteFile(BmpFile, m_pLogFont, sizeof(*m_pLogFont), &Written, NULL)
        == NULL) ERROR_BREAK(13);

    // We have all the info for the file header. Save the updated version:
    SetFilePointer(BmpFile, 0, 0, FILE_BEGIN);
    if (!WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, NULL))
        ERROR_BREAK(14);
    if (Written < sizeof(bmfh)) ERROR_BREAK(14);

	int nDummy=0;

	m_hNewFontImageData=ALLOCBUFFER(lpbi->bmiHeader.biSizeImage,nDummy);

	char* pNewFontImageData=(char*)GETPTR(m_hNewFontImageData);

	memcpy(pNewFontImageData,lpvBits,lpbi->bmiHeader.biSizeImage);

	FREEPTR(m_hNewFontImageData);

	m_nNewFontImageDataWidth=lpbi->bmiHeader.biWidth;
	m_nNewFontImageDataHeight=lpbi->bmiHeader.biHeight;

    // Blt preview to screen:
//    HDC hdc = ::GetDC(hWnd);
//    SelectObject(OffscrDC, OffscrBmp);
//    BitBlt(hdc, 0, 0, width, height, OffscrDC, 0, 0, SRCCOPY);
//    ReleaseDC(hWnd, hdc);
}
catch (int &errorcode)
{
    char Buf[100];
    wsprintf(Buf, "Screenshot error #%i", errorcode);
    OutputDebugString(Buf);
}
catch (...)
{
    OutputDebugString("Screenshot error");
}

    if (OffscrDC) DeleteDC(OffscrDC);
    if (OffscrBmp) DeleteObject(OffscrBmp);
    if (lpbi) delete[] lpbi;
    if (lpvBits) delete[] lpvBits;
    if (BmpFile != INVALID_HANDLE_VALUE) CloseHandle(BmpFile);
}


void CTextOverlayDfxDlg::BuildFontImage(void)
{
    HDC OffscrDC=NULL;
    LPBITMAPINFO lpbi=NULL;
    LPVOID lpvBits=NULL;

	HWND hWnd=GetSafeHwnd();

    // Create an offscreen device context:
    OffscrDC = CreateCompatibleDC(0);

    if (fi.hFont) SelectObject(OffscrDC, fi.hFont);

    // Create an offscreen bitmap:
    int width=16*fi.tm.tmMaxCharWidth;
    int height=14*fi.tm.tmHeight;
    HBITMAP OffscrBmp = CreateCompatibleBitmap(OffscrDC, width, height);
    if (!OffscrBmp) ERROR_BREAK(0);
    // Select bitmap into DC:
    HBITMAP OldBmp = (HBITMAP)SelectObject(OffscrDC, OffscrBmp);
    if (!OldBmp) ERROR_BREAK(1);

    // Clear background to black:
    SelectObject(OffscrDC, GetStockObject(BLACK_BRUSH));
    Rectangle(OffscrDC, 0, 0, width, height);
    SetBkMode(OffscrDC, TRANSPARENT);     // do not fill character background
    SetTextColor(OffscrDC, RGB(255, 255, 255)); // text color white

    // Draw characters:
    unsigned char c;
    for (int y=0; y < 14; y++)
    for (int x=0; x < 16; x++)
    {
        c = (unsigned char)(y*16+x+32);
        TextOut(OffscrDC, x*fi.tm.tmMaxCharWidth-fi.abc[c-32].abcA,
            y*fi.tm.tmHeight, (const char*)&c, 1);
            // *) See remark in WM_PAINT handler as to why abcA is subtracted.
    }

    // GetDIBits requires format info about the bitmap. We can have GetDIBits
    // fill a structure with that info if we pass a NULL pointer for lpvBits:
    // Reserve memory for bitmap info (BITMAPINFOHEADER + largest possible
    // palette):
    if ((lpbi = (LPBITMAPINFO)(new char[sizeof(BITMAPINFOHEADER) +
        256 * sizeof(RGBQUAD)])) == NULL) ERROR_BREAK(4);
    ZeroMemory(&lpbi->bmiHeader, sizeof(BITMAPINFOHEADER));
    lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    // Get info but first de-select OffscrBmp because GetDIBits requires it:
    SelectObject(OffscrDC, OldBmp);
    if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, NULL, lpbi, DIB_RGB_COLORS))
        return;

    // Reserve memory for bitmap bits:
    if ((lpvBits = new char[lpbi->bmiHeader.biSizeImage]) == NULL)
        ERROR_BREAK(6);

    // Have GetDIBits convert OffscrBmp to a DIB (device-independent bitmap):
    if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, lpvBits, lpbi,
        DIB_RGB_COLORS)) return;

	int nDummy=0;

	m_hNewFontImageData=ALLOCBUFFER(lpbi->bmiHeader.biSizeImage,nDummy);

	char* pNewFontImageData=(char*)GETPTR(m_hNewFontImageData);

	memcpy(pNewFontImageData,lpvBits,lpbi->bmiHeader.biSizeImage);

	FREEPTR(m_hNewFontImageData);

	m_nNewFontImageDataWidth=lpbi->bmiHeader.biWidth;
	m_nNewFontImageDataHeight=lpbi->bmiHeader.biHeight;

    if (OffscrDC) DeleteDC(OffscrDC);
    if (OffscrBmp) DeleteObject(OffscrBmp);
    if (lpbi) delete[] lpbi;
    if (lpvBits) delete[] lpvBits;
}



CTextOverlayDfxDlg::CTextOverlayDfxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextOverlayDfxDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextOverlayDfxDlg)
	//}}AFX_DATA_INIT

	m_bStyleChanged=false;
}

void CTextOverlayDfxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextOverlayDfxDlg)
	//}}AFX_DATA_MAP
}

void CTextOverlayDfxDlg::EnableControls()
{
	UpdateData();
}

BOOL CTextOverlayDfxDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	PlaceDfxDialog(GetSafeHwnd()); // replaces the window so that it is not over the preview! the "Center"-property of your dialog must be enabled to center it horizontally!

	HWND hWnd=GetSafeHwnd();

	if (m_pLogFont!=NULL) {
		m_CandidateFont=*m_pLogFont;
	}

	if (m_pInputFileName!=NULL) {
		strcpy((&m_TextFileName[0]),m_pInputFileName);
	} else {
		m_TextFileName[0]='\0';
	}

    // Initialize CFontInfo fi:
    fi.InitChooseFont(hWnd,&m_CandidateFont);


//    HDC hdc = ::GetDC(hWnd);
//    GetObject(GetCurrentObject(hdc, OBJ_FONT), sizeof(LOGFONT), m_pLogFont);
//    MeasureFont(hdc);
//    ::ReleaseDC(hWnd, hdc);
    // create HPENs for the constants in TPenTypes:
//	int i;
//    for (i=0; i < 5; i++)
//        Pens[i] = CreatePen(PS_DOT, 1, PenColors[i]);
    // prepare OPENFILENAME structure for GetSaveFileName:
    ZeroMemory(&OpenFileName, sizeof(OpenFileName));
    OpenFileName.lStructSize = sizeof(OPENFILENAME);//OPENFILENAME_SIZE_VERSION_400;   // = 76
        // (See remark at the end of this file!)
    OpenFileName.hwndOwner = hWnd;
    OpenFileName.lpstrFilter = "Text files (*.txt)\0"
        "*.txt\0";
    OpenFileName.nFilterIndex = 1;
    OpenFileName.lpstrFile = &(m_TextFileName[0]);
    OpenFileName.nMaxFile = MAX_PATH;
    OpenFileName.lpstrTitle = "Save as DirectDraw Font";
    OpenFileName.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    OpenFileName.lpstrDefExt = "bmp";

	m_hNewFontImageData=NULL;

//	SetDlgItemInt(IDC_TextOverlay_FRAMESTOSTORE_EDIT,m_nFramesToStore,FALSE);
//    SendDlgItemMessage(
//		IDC_TextOverlay_PLAYBACKSTYLE_LIST,
//		CB_SETCURSEL,
//		m_eStyle,
//		0L);
	EnableControls();

	return TRUE;
}


void CTextOverlayDfxDlg::OnOK() {

//	BOOL bSuccess;
//	m_nFramesToStore=GetDlgItemInt(IDC_TextOverlay_FRAMESTOSTORE_EDIT,&bSuccess,FALSE);
//
//	ETextOverlayStyle NewStyle=(ETextOverlayStyle)SendDlgItemMessage(
//		IDC_TextOverlay_PLAYBACKSTYLE_LIST,
//		CB_GETCURSEL,
//		0,
//		0L);
//
//	m_bStyleChanged=(NewStyle!=m_eStyle);
//
//	m_eStyle=NewStyle;

//	LoadFont();
//	SaveFont();

	*m_pLogFont=m_CandidateFont;
	strcpy(m_pInputFileName,(&m_TextFileName[0]));

	CDialog::OnOK();

}

void CTextOverlayDfxDlg::GetTextFileName(void) {

    if (!GetOpenFileName(&OpenFileName)) return;

}

BOOL CTextOverlayDfxDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra,
								  AFX_CMDHANDLERINFO* pHandlerInfo) {

	if (nID==ID_CHOOSEFONT) {

		LoadFont();

	} else if (nID==ID_LOADTEXT) {

		GetTextFileName();

	}

	return CDialog::OnCmdMsg(nID,nCode,pExtra,pHandlerInfo);

}





