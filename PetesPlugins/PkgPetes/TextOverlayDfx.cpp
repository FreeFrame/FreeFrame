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

#include <math.h>

#include "PeteHelpers.h"

#include "pkgSAMPLES.h"
#include "TextOverlayDfxDlg.h"
#include "WrapDynamicVars.h"
#include "DfxMemoryHelper.h"

const int cnStartingChar=32;
const int cnEndChar=256;

const int cnFontCharCount=(cnEndChar-cnStartingChar);

const int cnTextBufferSize=256;

const int cnVirtualKeyCount=256;

class CTextOverlayDfx: public CDFX
{
	DECLARE_EFFECT()

public:
	~CTextOverlayDfx();
	CTextOverlayDfx();
	
	BOOL	UpdateConfig();
	BOOL	Initialize();
	BOOL	SetupDialog();
	BOOL	Render(CScreen **ppInput, CScreen *pOutput);
	HBITMAP	GetLabelBitmap();
	DWORD	GetMemoryUsage();
	BOOL	ConfirmRenderSource(int i);
	DWORD	GetInputState();

protected:

	void InitializeFont(void);
	void DeinitializeFont(void);

	void BuildFontImage(void);
	void MeasureFont(HDC hdc);

	void DrawString(DWORD* pOutput,int nXOrigin,int nYOrigin,char* pString,char* pStringEnd,DWORD dwColour);
	int	DrawCharacter(DWORD* pOutput,int nXOrigin,int nYOrigin,char Character,DWORD dwColour);
	void DrawTextCentreJustified(DWORD* pOutput,char* pStartData,char* pEndData,int nXOrigin,int nYOrigin,DWORD dwColour);
	void DrawTextLeftJustified(DWORD* pOutput,char* pStartData,char* pEndData,int nXOrigin,int nYOrigin,DWORD dwColour);
	void DrawTextCentreJustifiedWrap(DWORD* pOutput,char* pStartData,char* pEndData,int nXOrigin,int nYOrigin,DWORD dwColour);
	void DrawTextLeftJustifiedWrap(DWORD* pOutput,char* pStartData,char* pEndData,int nXOrigin,int nYOrigin,DWORD dwColour);
	int GetCharWidth(char Character);
	char* GetStringLineWidth(char* pString,int* poutWidth);
	char* GetStringLineWidthWithWrap(char* pString,int nWrapWidth,int* poutWidth);
	char* GetNextNewLine(char* pString);
	int GetNextLineOffset(char* pTextData);
	char* GetPreviousNewLine(char* pString,char* pStringStart);
	int GetPreviousLineOffset(char* pTextData);
	
	BOOL	GetConfigData(CConfigData *p);
	void	SetConfigData(CConfigData *p);

	void	WriteOutputFrameTest(DWORD* pSource,DWORD* pOutput);
	void	WriteOutputFrame(DWORD* pSource,DWORD* pOutput);
	void	LoadTextFile(void);
	void	ReadTypedText(void);

	int     m_nXRes;		// X resolution of outputscreen
	int     m_nYRes;		// Y resolution of outputscreen

	DWORD   m_nMemUsage;

	HGLOBAL	m_hFontImageData;
	int		m_nFontImageDataWidth;
	int		m_nFontImageDataHeight;

	LOGFONT	m_FontInfo;
	TEXTMETRIC m_FontTextMetrics;
    ABC		m_FontCharacterSizes[cnFontCharCount];

	char	m_TextFileName[MAX_PATH];
	HGLOBAL	m_hTextData;
	int		m_nTextDataLength;

	float	m_TextPosX;
	int		m_nTextPosXID;

	float	m_TextPosY;
	int		m_nTextPosYID;

	bool	m_bEnabled;
	int		m_nEnabledID;
	
	bool	m_bCentreJustify;
	int		m_nCentreJustifyID;

	bool	m_bWordWrap;
	int		m_nWordWrapID;

	float	m_TextColourRed;
	int		m_nTextColourRedID;
	
	float	m_TextColourGreen;
	int		m_nTextColourGreenID;

	float	m_TextColourBlue;
	int		m_nTextColourBlueID;

	bool	m_bShowSingleLine;
	int		m_nShowSingleLineID;

	bool	m_bLastGoToNextLine;
	bool	m_bGoToNextLine;
	int		m_nGoToNextLineID;

	bool	m_bLastGoToPreviousLine;
	bool	m_bGoToPreviousLine;
	int		m_nGoToPreviousLineID;

	int		m_nCurrentLineOffset;

	bool	m_bTypedTextMode;
	int		m_nTypedTextModeID;

	bool	m_bInstantTyping;
	int		m_nInstantTypingID;

	char	m_pInputText[cnTextBufferSize];
	char	m_pDisplayText[cnTextBufferSize];

	int		m_nVirtualKeyStates[cnVirtualKeyCount];

};

BEGIN_INPUTSCREENS_DESC(CTextOverlayDfx)
{{-1,-1}, "RGB 0"},
END_INPUTSCREENS_DESC(CTextOverlayDfx)

BEGIN_OUTPUTSCREENS_DESC(CTextOverlayDfx)
{-1,-1},  ""
END_OUTPUTSCREENS_DESC(CTextOverlayDfx)

BEGIN_EFFECT_DESC(CTextOverlayDfx, 10203216861479968, PETE_VJO_VERSION , "Petes\\TextOverlay", "Pete Warden", "www.petewarden.com")
END_EFFECT_DESC(CTextOverlayDfx) 

REGISTER_DFXFACTORY(CTextOverlayDfx)

const int cnFramesToStore=25;

static inline int Gate(int nValue,int nMin,int nMax) {

	if (nValue<nMin) {
		return nMin;
	} else if (nValue>nMax) {
		return nMax;
	} else {
		return nValue;
	}
}

CTextOverlayDfx::CTextOverlayDfx()
{
	m_nXRes = 0;
	m_nYRes = 0;
	
	m_hFontImageData=NULL;
	m_nFontImageDataWidth=0;
	m_nFontImageDataHeight=0;

	m_hTextData=NULL;
	m_nTextDataLength=0;

	strcpy((&m_TextFileName[0]),"");

	m_nCurrentLineOffset=0;

	m_TextPosX=0.5f;
	m_TextPosY=0.5f;
	m_bCentreJustify=true;
	m_bWordWrap=true;
	m_bEnabled=true;
	
	m_bShowSingleLine=true;
	
	m_bGoToNextLine=false;
	m_bLastGoToNextLine=false;

	m_bGoToPreviousLine=false;
	m_bLastGoToPreviousLine=false;

	m_bTypedTextMode=false;
	m_bInstantTyping=false;
	strcpy((&m_pInputText[0]),"");
	strcpy((&m_pDisplayText[0]),"");

	ZeroMemory(&m_nVirtualKeyStates[0],sizeof(int)*cnVirtualKeyCount);

	m_nMemUsage = sizeof(CTextOverlayDfx);

	InitializeFont();

}

void CTextOverlayDfx::InitializeFont(void) {

    ZeroMemory(&m_FontInfo, sizeof(m_FontInfo));
    HDC hdc = ::GetDC(0);
    GetObject(GetCurrentObject(hdc, OBJ_FONT), sizeof(LOGFONT), &m_FontInfo);
    ::ReleaseDC(0, hdc);

	BuildFontImage();

}

void CTextOverlayDfx::BuildFontImage(void) {

    HDC OffscrDC=NULL;
    LPBITMAPINFO lpbi=NULL;
    LPVOID lpvBits=NULL;

    // Create an offscreen device context:
    OffscrDC = CreateCompatibleDC(0);

    HFONT hFont=CreateFontIndirect(&m_FontInfo);

	HDC hdc = ::GetDC(0);

    // select font:
    SelectObject(hdc, hFont);
    
	if (hFont) SelectObject(OffscrDC, hFont);

	MeasureFont(OffscrDC);

    // Create an offscreen bitmap:
    int width=16*m_FontTextMetrics.tmMaxCharWidth;
//	width+=(32-(width%32));

    int height=14*m_FontTextMetrics.tmHeight;
    HBITMAP OffscrBmp = CreateCompatibleBitmap(OffscrDC, width, height);
    if (!OffscrBmp) return;
    // Select bitmap into DC:
    HBITMAP OldBmp = (HBITMAP)SelectObject(OffscrDC, OffscrBmp);
    if (!OldBmp) return;

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
        TextOut(OffscrDC,
			x*m_FontTextMetrics.tmMaxCharWidth-m_FontCharacterSizes[c-cnStartingChar].abcA,
            y*m_FontTextMetrics.tmHeight,
			(const char*)&c, 1);
    }

    if ((lpbi = (LPBITMAPINFO)(new char[sizeof(BITMAPINFOHEADER) +
        256 * sizeof(RGBQUAD)])) == NULL) return;
    ZeroMemory(&lpbi->bmiHeader, sizeof(BITMAPINFOHEADER));
    lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    // Get info but first de-select OffscrBmp because GetDIBits requires it:
    SelectObject(OffscrDC, OldBmp);
    if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, NULL, lpbi, DIB_RGB_COLORS))
        return;

	if ((width&0x1f)!=0) {
		width|=0x1f;
		width+=1;
	}

    // Reserve memory for bitmap bits:
    if ((lpvBits = new char[lpbi->bmiHeader.biSizeImage]) == NULL)
		return;

    // Have GetDIBits convert OffscrBmp to a DIB (device-independent bitmap):
    if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, lpvBits, lpbi,
        DIB_RGB_COLORS)) return;

	m_hFontImageData=ALLOCBUFFER(lpbi->bmiHeader.biSizeImage,m_nMemUsage);

	char* pNewFontImageData=(char*)GETPTR(m_hFontImageData);
	
	memcpy(pNewFontImageData,lpvBits,lpbi->bmiHeader.biSizeImage);

	FREEPTR(m_hFontImageData);
	
	m_nFontImageDataWidth=width;
	m_nFontImageDataHeight=height;

    if (OffscrDC) DeleteDC(OffscrDC);
    if (OffscrBmp) DeleteObject(OffscrBmp);
    if (lpbi) delete[] lpbi;
    if (lpvBits) delete[] lpvBits;
}

void CTextOverlayDfx::MeasureFont(HDC hdc)
{
    
	GetTextMetrics(hdc, &m_FontTextMetrics);
    // Measure TrueType fonts with GetCharABCWidths:
    if (!GetCharABCWidths(hdc, 32, 255, &m_FontCharacterSizes[0]))
    // If it's not a TT font, use GetTextExtentPoint32 to fill array abc:
    {
        ZeroMemory(&m_FontCharacterSizes[0], cnFontCharCount*sizeof(ABC));    // set abcA and abcC to 0
                                                // (abcB will be overwritten)
        SIZE Size;
        // for all characters, beginning with " ":
        for (int i=cnStartingChar; i < cnEndChar; i++)
        {
            // get width of character...
            GetTextExtentPoint32(hdc, (char*)&i, 1, &Size);
            // ...and store it in abcB:
            m_FontCharacterSizes[i-cnStartingChar].abcB = Size.cx;
        }
    }
}

void CTextOverlayDfx::DeinitializeFont(void) {

	if (m_hFontImageData!=NULL) {

		FREEBUFFER(m_hFontImageData,m_nMemUsage);

	}

}


CTextOverlayDfx::~CTextOverlayDfx()
{

	DeinitializeFont();

}

HBITMAP	CTextOverlayDfx::GetLabelBitmap()			
{
	return (HBITMAP)HBITMAP_LAYERS;
}

BOOL	CTextOverlayDfx::SetupDialog()
{
	VJO_MANAGE_STATE();
	CTextOverlayDfxDlg	dlg;

	dlg.m_pLogFont=&m_FontInfo;
	dlg.m_pInputFileName=&(m_TextFileName[0]);

	g_hVJOPreviewWnd = m_pEngine->GetOutputWindow();
	if(dlg.DoModal() != IDOK) return FALSE;

	DeinitializeFont();
	
	BuildFontImage();

	LoadTextFile();

	return TRUE;

}

void	CTextOverlayDfx::SetConfigData(CConfigData *p)
{

	ASSERT(p);
	p->SetInt("!", 1);

	const int nTextFileNameSize = strlen((&m_TextFileName[0]))+1;
	p->SetBuffer("T", (&m_TextFileName[0]), nTextFileNameSize);

	const int nFontInfoSize = sizeof(m_FontInfo);
	p->SetBuffer("F", (&m_FontInfo), nFontInfoSize);

}


BOOL	CTextOverlayDfx::GetConfigData(CConfigData *p)
{
	ASSERT(p);
	if(!p->GetInt("!", 0)) return FALSE;

	int nTextFileNameSize = 0;
	char* pTextFileName = (char*)p->GetBuffer("T", nTextFileNameSize);
	if ((pTextFileName != NULL) && (nTextFileNameSize > 0))
	{
		memcpy((&m_TextFileName[0]), pTextFileName, nTextFileNameSize);
	}

	int nFontInfoSize = 0;
	LOGFONT* pFontInfo = (LOGFONT*)p->GetBuffer("F", nFontInfoSize);
	if ((pFontInfo != NULL) && (nFontInfoSize > 0))
	{
		memcpy(&m_FontInfo, pFontInfo, nFontInfoSize);
	}
	
	return TRUE;	
}

BOOL	CTextOverlayDfx::UpdateConfig()
{
	CConfigData *pConfigData = m_pEngine->GetCurrentConfig();
	if(!GetConfigData(pConfigData))
		SetConfigData(pConfigData);

	m_nTextPosXID=
		RegisterFloat(m_pEngine,&m_TextPosX,"Text Pos X",-100.0f,100.0f,200.0f,120000.0f);
	m_nTextPosYID=
		RegisterFloat(m_pEngine,&m_TextPosY,"Text Pos Y",-100.0f,100.0f,200.0f,120000.0f);

	m_nCentreJustifyID=
		RegisterBool(m_pEngine,&m_bCentreJustify,"Centre Justify");
	m_nWordWrapID=
		RegisterBool(m_pEngine,&m_bWordWrap,"Word Wrap");
	m_nEnabledID=
		RegisterBool(m_pEngine,&m_bEnabled,"Enabled");

	m_nTextColourRedID=
		RegisterFloat(m_pEngine,&m_TextColourRed,"R",0.0f,255.0f);

	m_nTextColourGreenID=
		RegisterFloat(m_pEngine,&m_TextColourGreen,"G",0.0f,255.0f);

	m_nTextColourBlueID=
		RegisterFloat(m_pEngine,&m_TextColourBlue,"B",0.0f,255.0f);


	m_nShowSingleLineID=
		RegisterBool(m_pEngine,&m_bShowSingleLine,"Show Single Line");
	
	m_nGoToNextLineID=
		RegisterBool(m_pEngine,&m_bGoToNextLine,"Go To Next Line");

	m_nGoToPreviousLineID=
		RegisterBool(m_pEngine,&m_bGoToPreviousLine,"Go To Previous Line");

	m_nTypedTextModeID=
		RegisterBool(m_pEngine,&m_bTypedTextMode,"Typed Text Mode");

	m_nInstantTypingID=
		RegisterBool(m_pEngine,&m_bInstantTyping,"Instant Typing");

	return TRUE;
}

BOOL	CTextOverlayDfx::Initialize()
{
	SIZE sz = m_pEngine->GetOutputRes();
	m_nXRes = sz.cx;
	m_nYRes = sz.cy;

	LoadTextFile();
	BuildFontImage();

	return TRUE;
}

void	CTextOverlayDfx::LoadTextFile(void) {

	if (strlen(m_TextFileName)==0) {
		return;
	}
	
	HANDLE OpenFileHandle=CreateFile(
		(&m_TextFileName[0]),
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (OpenFileHandle==INVALID_HANDLE_VALUE) {
		char ErrorMessage[MAX_PATH*2];
		sprintf(&ErrorMessage[0],"Couldn't open file '%s'!",&m_TextFileName[0]);
		MessageBox(NULL,&ErrorMessage[0],"Error",MB_OK);
		return;
	}

	int nFileSize=GetFileSize(OpenFileHandle,NULL);

	REALLOCBUFFER(m_hTextData,nFileSize+1,m_nMemUsage);
	
	if (m_hTextData==NULL) {
		MessageBox(NULL,"Couldn't allocate memory!","Error",MB_OK);
		return;
	}

	char* pTextData=(char*)GETPTR(m_hTextData);
	if (pTextData==NULL) {
		MessageBox(NULL,"Couldn't get pointer from m_hTextData handle!","Error",MB_OK);
		return;
	}

	DWORD nNumberOfBytesRead;
	BOOL ReadFileResult=ReadFile(OpenFileHandle,pTextData,nFileSize,&nNumberOfBytesRead,NULL);

	if (ReadFileResult==0) {
		MessageBox(NULL,"ReadFile() failed!","Error",MB_OK);
		return;
	}

	if (nNumberOfBytesRead!=nFileSize) {
		char ErrorMessage[MAX_PATH*2];
		sprintf(&ErrorMessage[0],"Tried to read %d bytes, actually read %d!",
			nFileSize,
			nNumberOfBytesRead);
		MessageBox(NULL,&ErrorMessage[0],"Error",MB_OK);
		return;
	}

	pTextData[nFileSize]='\0';

	CloseHandle(OpenFileHandle);

	FREEPTR(m_hTextData);

}

BOOL	CTextOverlayDfx::Render(CScreen **ppInput, CScreen *pOutput)
{

	DWORD* pOutputMem = (DWORD*)pOutput->GetBuffer();
	DWORD* pInputMem = (DWORD*)ppInput[0]->GetBuffer();

	WriteOutputFrame(pInputMem,pOutputMem);

	return TRUE;

}

int CTextOverlayDfx::GetCharWidth(char Character) {

	if ((Character<cnStartingChar)||(Character>=cnEndChar)) {
		return 0;
	} else {
		return m_FontCharacterSizes[Character-cnStartingChar].abcB;
	}
}

char* CTextOverlayDfx::GetStringLineWidth(char* pString,int* poutWidth) {

	int nCurrentWidth=0;
	bool bHitNewLine=false;
	
	while ((*pString!='\0')&&(!bHitNewLine)) {

		const char Character=*pString;

		switch (Character) {

			case '\n':{

				bHitNewLine=true;

			}break;

			case ' ':{

				const char DefaultChar='i';
				const int nDefaultCharWidth=GetCharWidth(DefaultChar);
				nCurrentWidth+=nDefaultCharWidth;
				
			}

			default: {

				const int nFontCharWidth=GetCharWidth(Character);
				nCurrentWidth+=(nFontCharWidth+1);

			}break;

		}

		pString+=1;

	}

	*poutWidth=nCurrentWidth;

	if (bHitNewLine) {

		return pString;

	} else {
		
		return NULL;

	}

}

char* CTextOverlayDfx::GetStringLineWidthWithWrap(char* pString,int nWrapWidth,int* poutWidth) {

	int nCurrentWidth=0;
	bool bHitNewLine=false;
	bool bHitLineWrap=false;
	char* pLastSpace=NULL;
	int nLastSpaceWidth=0;
	bool bNonSpaceHit=false;

	
	while ((*pString!='\0')&&(!bHitNewLine)&&(!bHitLineWrap)) {

		const char Character=*pString;

		switch (Character) {

			case '\n':{

				bHitNewLine=true;

			}break;

			case '\t': // deliberate fall-through
			case ' ':{

				if (bNonSpaceHit) {
					pLastSpace=pString;
					nLastSpaceWidth=nCurrentWidth;
				}
				
				const char DefaultChar='i';
				const int nDefaultCharWidth=GetCharWidth(DefaultChar);
				nCurrentWidth+=nDefaultCharWidth;



			}

			default: {

				const int nFontCharWidth=GetCharWidth(Character);
				nCurrentWidth+=(nFontCharWidth+1);
				bNonSpaceHit=true;

			}break;

		}

		if (nCurrentWidth>nWrapWidth) {

			if (!bHitNewLine) {
				bHitLineWrap=true;
			}

		}

		pString+=1;

	}


	char* pResult;
	int nResultWidth;

	if (bHitNewLine) {

		pResult=pString;
		nResultWidth=nCurrentWidth;

	} else if (bHitLineWrap) {

		if (pLastSpace!=NULL) {

			pResult=pLastSpace;
			nResultWidth=nLastSpaceWidth;

		} else {

			pResult=pString-1;
			const int nFontCharWidth=GetCharWidth((*pString-1));
			nCurrentWidth-=(nFontCharWidth+1);
			nResultWidth=nCurrentWidth;

		}


	} else {
		
		pResult=NULL;
		nResultWidth=nCurrentWidth;

	}

	*poutWidth=nResultWidth;
	return pResult;

}


void CTextOverlayDfx::DrawString(DWORD* pOutput,int nXOrigin,int nYOrigin,char* pString,char* pStringEnd,DWORD dwColour) {

	int nCurrentX=nXOrigin;
	int nCurrentY=nYOrigin;

	while ((*pString!='\0')&&(pString!=pStringEnd)) {

		const char Character=*pString;

		bool bIsVisibleCharacter;
		switch (Character) {

			case '\n':{

				nCurrentY-=m_FontTextMetrics.tmHeight;
				nCurrentX=nXOrigin;

				bIsVisibleCharacter=false;

			}break;

			case ' ':{

				const char DefaultChar='i';
				const int nDefaultCharWidth=GetCharWidth(DefaultChar);
				nCurrentX+=nDefaultCharWidth;
				
				bIsVisibleCharacter=false;
			}

			default: {

				bIsVisibleCharacter=true;

			}break;

		}

		if (bIsVisibleCharacter) {
		
			const int nCharWidth=DrawCharacter(pOutput,nCurrentX,nCurrentY,Character,dwColour);

			nCurrentX+=nCharWidth;

		}

		pString+=1;

	}

}


int	CTextOverlayDfx::DrawCharacter(DWORD* pOutput,int nXOrigin,int nYOrigin,char Character,DWORD dwColour) {

	BYTE* pFontImageData=static_cast<BYTE*>(GETPTR(m_hFontImageData));
	if (pFontImageData==NULL) {
		return 0;
	}

	if ((Character<cnStartingChar)||(Character>=cnEndChar)) {
		return 0;
	}

	const int nWidth=m_nXRes;
	const int nHeight=m_nYRes;

	const int nFontImageWidth=m_nFontImageDataWidth;
	const int nFontImageHeight=m_nFontImageDataHeight;

	const int nFontCharWidth=GetCharWidth(Character);
	const int nFontCharMaxWidth=m_FontTextMetrics.tmMaxCharWidth;
	const int nFontCharHeight=m_FontTextMetrics.tmHeight;

	const int nStartX=nXOrigin;
	const int nGatedStartX=Gate(nStartX,0,(nWidth-1));
	
	const int nEndX=nXOrigin+nFontCharWidth;
	const int nGatedEndX=Gate(nEndX,0,(nWidth-1));

	const int nStartY=nYOrigin-nFontCharHeight;
	const int nGatedStartY=Gate(nStartY,0,(nHeight-1));

	const int nEndY=nYOrigin;
	const int nGatedEndY=Gate(nEndY,0,(nHeight-1));

	int nFontStartX=((Character-cnStartingChar)%16)*nFontCharMaxWidth;
	int nFontStartY=nFontImageHeight-(((Character-cnStartingChar)/16)*nFontCharHeight);
	nFontStartY-=nFontCharHeight;

	const int nDeltaStartX=nGatedStartX-nStartX;
	const int nDeltaStartY=nGatedStartY-nStartY;

	nFontStartX+=nDeltaStartX;
	nFontStartY+=nDeltaStartY;

	int nY;
	int nFontY=nFontStartY;
	for (nY=nGatedStartY; nY<nGatedEndY; ++nY,++nFontY) {

		DWORD* pLineStart=pOutput+(nY*nWidth);
		BYTE* pFontLineStart=(pFontImageData+(nFontY*(nFontImageWidth/8)));

		int nX;
		int nFontX=nFontStartX;
		for (nX=nGatedStartX; nX<nGatedEndX; ++nX,++nFontX) {

			DWORD*const pDest=pLineStart+nX;

			BYTE* pCurrentFontData=
				(pFontLineStart+(nFontX/8));

			BYTE FontData=*pCurrentFontData;

			int nFlag=(1<<(7-(nFontX&7)));

			bool bIsSet=((FontData&nFlag)!=0);

			if (bIsSet) {

				*pDest=dwColour;

			}

		}

	}

	return nFontCharWidth+1;

}

void	CTextOverlayDfx::WriteOutputFrameTest(DWORD* pSource,DWORD* pOutput) {

	const int nWidth=m_nXRes;
	const int nHeight=m_nYRes;
	const int nNumPixels = nWidth*nHeight;

	if (m_hFontImageData==NULL) {
		return;
	}

	BYTE* pFontImageData=static_cast<BYTE*>(GETPTR(m_hFontImageData));
	if (pFontImageData==NULL) {
		return;
	}
	
	const int nFontImageWidth=m_nFontImageDataWidth;
	const int nFontImageHeight=m_nFontImageDataHeight;

	DWORD* pCurrentSource=pSource;
	DWORD* pCurrentOutput=pOutput;
	const DWORD* pSourceEnd=(pSource+nNumPixels);

	int nX=0;
	int nY=0;

	while (pCurrentSource!=pSourceEnd) {

		const int nFontX=(nX%nFontImageWidth);
		const int nFontY=(nY%nFontImageHeight);

		BYTE* pCurrentFontData=
			(pFontImageData+(nFontY*(nFontImageWidth/8))+(nFontX/8));

		BYTE FontData=*pCurrentFontData;

		int nFlag=(1<<(7-(nFontX&7)));

		bool bIsSet=(FontData&nFlag)!=0;

		if (bIsSet) {

			*pCurrentOutput=0x00000000;

		} else {

			*pCurrentOutput=*pCurrentSource;

		}
		
		pCurrentSource+=1;
		pCurrentOutput+=1;

		nX+=1;
		if (nX>=nWidth) {
			nX=0;
			nY+=1;
		}

	}

	FREEPTR(m_hFontImageData);

}

void	CTextOverlayDfx::DrawTextCentreJustified(DWORD* pOutput,char* pStartData,char* pEndData,int nXOrigin,int nYOrigin,DWORD dwColour) {

	char* pCurrentString=pStartData;

	int nCurrentY=nYOrigin;

	do {

		int nLineWidth;
		char* pNextString=GetStringLineWidth(pCurrentString,&nLineWidth);

		int nCurrentX=nXOrigin-(nLineWidth/2);

		DrawString(pOutput,nCurrentX,nCurrentY,pCurrentString,pNextString,dwColour);

		pCurrentString=pNextString;

		nCurrentY-=m_FontTextMetrics.tmHeight;

	} while ((pCurrentString!=NULL)&&(pEndData==NULL));


}

void	CTextOverlayDfx::DrawTextLeftJustified(DWORD* pOutput,char* pStartData,char* pEndData,int nXOrigin,int nYOrigin,DWORD dwColour) {

	DrawString(pOutput,nXOrigin,nYOrigin,pStartData,pEndData,dwColour);

}

void	CTextOverlayDfx::DrawTextCentreJustifiedWrap(DWORD* pOutput,char* pStartData,char* pEndData,int nXOrigin,int nYOrigin,DWORD dwColour) {

	char* pCurrentString=pStartData;

	int nCurrentY=nYOrigin;
	bool bIsLineDone;

	do {

		int nLineWidth;
		char* pNextString=GetStringLineWidthWithWrap(pCurrentString,m_nXRes,&nLineWidth);

		int nCurrentX=nXOrigin-(nLineWidth/2);

		DrawString(pOutput,nCurrentX,nCurrentY,pCurrentString,pNextString,dwColour);

		pCurrentString=pNextString;

		nCurrentY-=m_FontTextMetrics.tmHeight;

		bIsLineDone=((pEndData!=NULL)&&(pCurrentString>=pEndData));

	} while ((pCurrentString!=NULL)&&(!bIsLineDone));


}

void	CTextOverlayDfx::DrawTextLeftJustifiedWrap(DWORD* pOutput,char* pStartData,char* pEndData,int nXOrigin,int nYOrigin,DWORD dwColour) {

	char* pCurrentString=pStartData;

	int nCurrentY=nYOrigin;
	bool bIsLineDone;

	do {

		int nLineWidth;
		char* pNextString=GetStringLineWidthWithWrap(pCurrentString,m_nXRes-nXOrigin,&nLineWidth);

		int nCurrentX=nXOrigin;

		DrawString(pOutput,nCurrentX,nCurrentY,pCurrentString,pNextString,dwColour);

		pCurrentString=pNextString;

		nCurrentY-=m_FontTextMetrics.tmHeight;

		bIsLineDone=((pEndData!=NULL)&&(pCurrentString>=pEndData));

	} while ((pCurrentString!=NULL)&&(!bIsLineDone));


}

char* CTextOverlayDfx::GetNextNewLine(char* pString) {

	while ((*pString!='\0')&&(*pString!='\n')) {
		pString+=1;
	}

	if (*pString!='\0') {
		return pString;
	} else {
		return NULL;
	}

}

int CTextOverlayDfx::GetNextLineOffset(char* pTextData) {

	char* pCurrentLine=pTextData+m_nCurrentLineOffset;

	char* pNextLine=GetNextNewLine(pCurrentLine);

	if (pNextLine==NULL) {
		return 0;
	} else {
		return (pNextLine-pTextData)+1;
	}

}

char* CTextOverlayDfx::GetPreviousNewLine(char* pString,char* pStringStart) {

	if (pString!=pStringStart) {
		pString-=1;
	}
	if (pString!=pStringStart) {
		pString-=1;
	}
	
	while ((pString!=pStringStart)&&(*pString!='\n')) {
		pString-=1;
	}

	if (pString!=pStringStart) {
		return pString;
	} else {
		return NULL;
	}

}

int CTextOverlayDfx::GetPreviousLineOffset(char* pTextData) {

	char* pCurrentLine=pTextData+m_nCurrentLineOffset;

	char* pPreviousLine=GetPreviousNewLine(pCurrentLine,pTextData);

	if (pPreviousLine==NULL) {

		pPreviousLine=GetPreviousNewLine(pTextData+strlen(pTextData),pTextData);

	}
		
	if (pPreviousLine==NULL) {
		return 0;
	} else {
		return (pPreviousLine-pTextData)+1;
	}

}

void	CTextOverlayDfx::WriteOutputFrame(DWORD* pSource,DWORD* pOutput) {

	const int nWidth=m_nXRes;
	const int nHeight=m_nYRes;
	const int nNumPixels = nWidth*nHeight;

	memcpy(pOutput,pSource,sizeof(DWORD)*nNumPixels);

	if (((m_hTextData!=NULL)||(m_bTypedTextMode))&&(m_bEnabled)) {

		char* pTextData;
		if (m_bTypedTextMode) {
			if (m_bInstantTyping) {
				pTextData=m_pInputText;
			} else {
				pTextData=(&m_pDisplayText[0]);
			}
		} else {
			pTextData=(char*)GETPTR(m_hTextData);
		}

		const int nCurrentX=(int)(m_TextPosX*nWidth);
		const int nCurrentY=(int)(m_TextPosY*nHeight);

		const int nRed=(int)(m_TextColourRed);
		const int nGreen=(int)(m_TextColourGreen);
		const int nBlue=(int)(m_TextColourBlue);
		const DWORD dwColour=((nRed<<SHIFT_RED)|(nGreen<<SHIFT_GREEN)|(nBlue<<SHIFT_BLUE));

		char* pStartData;
		char* pEndData;

		if (m_bShowSingleLine) {

			if (m_bGoToNextLine&&!m_bLastGoToNextLine) {

				m_nCurrentLineOffset=GetNextLineOffset(pTextData);

			}

			if (m_bGoToPreviousLine&&!m_bLastGoToPreviousLine) {

				m_nCurrentLineOffset=GetPreviousLineOffset(pTextData);

			}

			m_bLastGoToNextLine=m_bGoToNextLine;
			m_bLastGoToPreviousLine=m_bGoToPreviousLine;


			pStartData=pTextData+m_nCurrentLineOffset;
			pEndData=GetNextNewLine(pStartData);

		} else {

			pStartData=pTextData;
			pEndData=NULL;

		}


		if (m_bWordWrap) {

			if (m_bCentreJustify) {
				DrawTextCentreJustifiedWrap(pOutput,pStartData,pEndData,nCurrentX,nCurrentY,dwColour);
			} else {
				DrawTextLeftJustifiedWrap(pOutput,pStartData,pEndData,nCurrentX,nCurrentY,dwColour);
			}
		
		} else {

			if (m_bCentreJustify) {
				DrawTextCentreJustified(pOutput,pStartData,pEndData,nCurrentX,nCurrentY,dwColour);
			} else {
				DrawTextLeftJustified(pOutput,pStartData,pEndData,nCurrentX,nCurrentY,dwColour);
			}

		}

		
//		DrawString(pOutput,0,100,pTextData,0x00000000);

		if (!m_bTypedTextMode) {
			FREEPTR(m_hTextData);
		}

	}

	if (m_bTypedTextMode) {
		ReadTypedText();
	}

}

void	CTextOverlayDfx::ReadTypedText(void) {

	const bool bIsControlPressed=GetAsyncKeyState(VK_CONTROL);
	const bool bIsShiftPressed=GetAsyncKeyState(VK_SHIFT);
	bool bIsReturnPressed=GetAsyncKeyState(VK_MENU);
	bool bIsBackSpacePressed=GetAsyncKeyState(VK_BACK);
	bool bIsSpacePressed=GetAsyncKeyState(' ');

	if (bIsReturnPressed) {
		bIsReturnPressed=(m_nVirtualKeyStates[VK_MENU]==0);
		m_nVirtualKeyStates[VK_MENU]=1;
	} else {
		m_nVirtualKeyStates[VK_MENU]=0;
	}

	if (bIsBackSpacePressed) {
		bIsBackSpacePressed=(m_nVirtualKeyStates[VK_BACK]==0);
		m_nVirtualKeyStates[VK_BACK]=1;
	} else {
		m_nVirtualKeyStates[VK_BACK]=0;
	}

	if (bIsSpacePressed) {
		bIsSpacePressed=(m_nVirtualKeyStates[' ']==0);
		m_nVirtualKeyStates[' ']=1;
	} else {
		m_nVirtualKeyStates[' ']=0;
	}

	if (!bIsControlPressed) {
		return;
	}

	bool bIsKeyPressed=false;
	char cKeyPressed;

	int nCount;
	for (nCount='0'; nCount<='9'; ++nCount) {
		if ((GetAsyncKeyState(nCount)&(1<<15))!=0) {
			if (m_nVirtualKeyStates[nCount]==0) {
				bIsKeyPressed=true;
				cKeyPressed=nCount;
				m_nVirtualKeyStates[nCount]=1;
			}
		} else {
			m_nVirtualKeyStates[nCount]=0;
		}
	}

	for (nCount='A'; nCount<='Z'; ++nCount) {
		if ((GetAsyncKeyState(nCount)&(1<<15))!=0) {
			if (m_nVirtualKeyStates[nCount]==0) {
				bIsKeyPressed=true;
				if (bIsShiftPressed) {
					cKeyPressed=nCount;
				} else {
					cKeyPressed=(nCount-'A')+'a';
				}
				m_nVirtualKeyStates[nCount]=1;
			}
		} else {
			m_nVirtualKeyStates[nCount]=0;
		}
	}

	if (bIsSpacePressed) {
		bIsKeyPressed=true;
		cKeyPressed=' ';
	}

	if (bIsReturnPressed) {

		strcpy(m_pDisplayText,m_pInputText);
		strcpy(m_pInputText,"");

	} else if (bIsBackSpacePressed){

		const int nStringLength=strlen(m_pInputText);

		if (nStringLength>1) {
			m_pInputText[(nStringLength-1)]='\0';
		}

	} else if (bIsKeyPressed) {

		const int nStringLength=strlen(m_pInputText);

		if (nStringLength<(cnTextBufferSize-1)) {
			sprintf(m_pInputText,"%s%c",m_pInputText,cKeyPressed);			
		}

	}

}

DWORD	CTextOverlayDfx::GetMemoryUsage()
{
	return m_nMemUsage;
}

BOOL CTextOverlayDfx::ConfirmRenderSource(int i)
{
	return TRUE;
}

DWORD CTextOverlayDfx::GetInputState()
{
	return 0xffffffff;
}

