/* main.c */

#include <Carbon/Carbon.h>
#include <mach-o/dyld.h>
#include <dirent.h>
#include <CFString.h>

#include "main.h"
#include "ResourceHeader.h"
#include "PeteHelpers.h"
#include "FreeFrame.h"

// Globals
//------------------------------------------------------------------------------
	WindowRef	gWindow = NULL;
	GWorldPtr	gSrcGWorld = NULL;
        GWorldPtr	gDestGWorld = NULL;
	Movie		gMovie = NULL;
	TimeValue 	gMovieTime = 0;			// set current time value to begining of the Movie
	int		gFrameCount = -1;
	int		gFrameNumber = 0;
	Rect 		gMovieBox;


struct SFreeFrameHost_EffectData {
        NSObjectFileImage m_ObjectFileImage;
        NSModule m_Module;
	FF_Main_FuncPtr m_pMainFunc;
};

struct SFreeFrameHost_InstanceData;
struct SFreeFrameHost_SliderContext {
    SFreeFrameHost_InstanceData* pInstanceData;
    int nParameterIndex;
};

struct SFreeFrameHost_ParameterData {
    float m_Value;
    ControlRef m_SliderControl;
    ControlRef m_ValueControl;
    ControlRef m_NameControl;
    SFreeFrameHost_SliderContext m_SliderContext;
};


const int nMaxEffectCount=256;
const int nMaxParamCount=256;

struct SFreeFrameHost_InstanceData {
	SFreeFrameHost_EffectData m_Effects[nMaxEffectCount];
        SFreeFrameHost_ParameterData m_Params[nMaxParamCount];
	int nEffectsCount;
	int nSelectedEffect;
	VideoInfoStruct VideoInfo;
	int nSourceWidth;
	int nSourceHeight;
        WindowRef ParamsWindow;
};

const int nParameterBorder=10;
const int nParameterSliderWidth=150;
const int nParameterValueWidth=75;
const int nParameterNameWidth=200;
const int nParameterOverallWidth=(nParameterBorder+nParameterNameWidth+nParameterSliderWidth+nParameterValueWidth+nParameterBorder);
const int nParameterHeight=20;
const int nParameterOverallHeight=(nParameterHeight+nParameterBorder);

const int nParameterRange=10000;

void FreeFrame_LoadAllPlugins(SFreeFrameHost_InstanceData* pInstanceData);
bool FreeFrame_LoadPlugin(SFreeFrameHost_EffectData* pEffectData,char* pFileName);
void FreeFrame_InitEffect(SFreeFrameHost_InstanceData* pInstanceData);
void FreeFrame_DeInitEffect(SFreeFrameHost_InstanceData* pInstanceData);
void FreeFrame_ApplyEffect(SFreeFrameHost_InstanceData* pInstanceData);
char* FreeFrame_GetEffectName(SFreeFrameHost_EffectData* pEffectData);
void FreeFrame_AddEffectsToMenu(SFreeFrameHost_InstanceData* pInstanceData);

void MakeMenu(SFreeFrameHost_InstanceData* pInstanceData);
void DoMenuCommand(long menuResult,SFreeFrameHost_InstanceData* pInstanceData);
void DoAboutBox(void);
pascal void ParameterSliderCallback(ControlHandle control,SInt16 part);
void MakeParamsDialog(SFreeFrameHost_InstanceData* pInstanceData);
void DestroyParamsDialog(SFreeFrameHost_InstanceData* pInstanceData);
void NextFrame(SFreeFrameHost_InstanceData* pInstanceData);
void DrawMovie(void); 
 
void MakeMenu(SFreeFrameHost_InstanceData* pInstanceData)		/* Put up a menu */
{
    Handle	menuBar;
    MenuRef	menu;
    long	response;
    OSErr	err;
	
    menuBar = GetNewMBar(rMenuBar);	/* read menus into menu bar */
    if ( menuBar != nil )
    {
        SetMenuBar(menuBar);	/* install menus */
        
        err = Gestalt(gestaltMenuMgrAttr, &response);
	if ((err == noErr) && (response & gestaltMenuMgrAquaLayoutMask))
        {
            menu = GetMenuHandle( mFile );
            DeleteMenuItem( menu, iQuit );
            DeleteMenuItem( menu, iQuitSeparator );
        }
        
        FreeFrame_AddEffectsToMenu(pInstanceData);
        
        DrawMenuBar();
    }
    else
    	ExitToShell();
}

void MakeParamsDialog(SFreeFrameHost_InstanceData* pInstanceData) {

    pInstanceData->ParamsWindow=GetNewWindow(129,NULL,(WindowPtr)-1L);
    
    if (pInstanceData->ParamsWindow!=NULL) {
 
	const int nSelectedEffect=pInstanceData->nSelectedEffect;
	SFreeFrameHost_EffectData* pEffectData=
		&pInstanceData->m_Effects[nSelectedEffect];

	FF_Main_FuncPtr pFreeFrameMain=pEffectData->m_pMainFunc;
        
        int nNumParameters;
        if (pFreeFrameMain!=NULL) {
            nNumParameters=(int)pFreeFrameMain(FF_GETNUMPARAMETERS,NULL,0);
        } else {
            nNumParameters=0;
        }

        const int nWindowSizeX=nParameterOverallWidth;
        const int nWindowSizeY=(nParameterOverallHeight*nNumParameters)+nParameterBorder;

        Rect MainWindowPos;
        GetWindowBounds(gWindow,kWindowContentRgn,&MainWindowPos);
        
        MoveWindow(pInstanceData->ParamsWindow,MainWindowPos.right,MainWindowPos.top,true);

	SizeWindow(pInstanceData->ParamsWindow, nWindowSizeX, nWindowSizeY, true);

        char* pEffectName=FreeFrame_GetEffectName(pEffectData);            

        char pTitleString[256];
        strcpy(pTitleString,pEffectName);
        strcat(pTitleString," Parameters");

        CFStringRef TitleStringInMacFormat=CFStringCreateWithCString(NULL,pTitleString,GetApplicationTextEncoding());

        SetWindowTitleWithCFString(pInstanceData->ParamsWindow,TitleStringInMacFormat);
        
        ControlActionUPP pfnSliderCallback=NewControlActionUPP( ParameterSliderCallback );
        
        int nCount;
        for (nCount=0; nCount<nNumParameters; nCount+=1) {
 
            static void* pCastCount;
            pCastCount=reinterpret_cast<void*>(nCount);
			
            char* pOriginalName=(char*)(pEffectData->m_pMainFunc(FF_GETPARAMETERNAME,pCastCount,0));

            if (pOriginalName==NULL) {
                    pOriginalName="<NULL>";
            }

            char pName[16];
                        
            bool bNullCharFound=false;
            int nCharCount;
            for (nCharCount=0; ((nCharCount<15)&&(!bNullCharFound)); nCharCount+=1) {
                    bNullCharFound=(pOriginalName[nCharCount]=='\0');
                    pName[nCharCount]=pOriginalName[nCharCount];
            }
            if (!bNullCharFound) {
                    pName[15]='\0'; // Pete- Getting unterminated strings returned
            }

            CFStringRef NameInMacFormat=CFStringCreateWithCString(NULL,pName,GetApplicationTextEncoding());

            static U32 ResultAsU32;
            ResultAsU32=(U32)pEffectData->m_pMainFunc(FF_GETPARAMETERDEFAULT,pCastCount,0);
            const float DefaultValue=*((float*)&ResultAsU32);

            SetParameterStruct ArgStruct;
            ArgStruct.index=nCount;
            ArgStruct.value=DefaultValue;

            pFreeFrameMain(FF_SETPARAMETER,&ArgStruct,0);
            
            char* pValueDisplay=(char*)(pEffectData->m_pMainFunc(FF_GETPARAMETERDISPLAY,pCastCount,0));
			
            bNullCharFound=false;
            for (nCharCount=0; ((nCharCount<15)&&(!bNullCharFound)); nCharCount+=1) {
                    bNullCharFound=(pValueDisplay[nCharCount]=='\0');
            }
            if (!bNullCharFound) {
                    pValueDisplay[15]='\0'; // Pete- Getting unterminated strings returned
            }

            if (pValueDisplay==NULL) {
                    pValueDisplay="<NULL>";
            }

            CFStringRef ValueDisplayInMacFormat=CFStringCreateWithCString(NULL,pValueDisplay,GetApplicationTextEncoding());

            SFreeFrameHost_ParameterData* pCurrentParam=&pInstanceData->m_Params[nCount];
 
            const int nOffsetY=(nCount*nParameterOverallHeight);
 
            Rect BoundsRect;

            BoundsRect.left=nParameterBorder;
            BoundsRect.top=nParameterBorder+nOffsetY;
            BoundsRect.right=nParameterSliderWidth;
            BoundsRect.bottom=nParameterOverallHeight+nOffsetY;
 
            OSStatus CreateNameResult=CreateStaticTextControl (
                pInstanceData->ParamsWindow,
                &BoundsRect,
                NameInMacFormat, 
                NULL, 
                &pCurrentParam->m_NameControl);

            BoundsRect.left=BoundsRect.right;
            BoundsRect.right=BoundsRect.left+nParameterSliderWidth;

            OSStatus CreateSliderResult=CreateSliderControl(
                pInstanceData->ParamsWindow,
                &BoundsRect,
                (DefaultValue*nParameterRange),
                0,
                nParameterRange,
                1,
                5,
                true,
                pfnSliderCallback,
                &pCurrentParam->m_SliderControl);             

            BoundsRect.left=BoundsRect.right;
            BoundsRect.right=BoundsRect.left+nParameterValueWidth;
            OSStatus CreateValueResult=CreateStaticTextControl (
                pInstanceData->ParamsWindow,
                &BoundsRect,
                ValueDisplayInMacFormat, 
                NULL, 
                &pCurrentParam->m_ValueControl);
                
            pCurrentParam->m_SliderContext.pInstanceData=pInstanceData;
            pCurrentParam->m_SliderContext.nParameterIndex=nCount;
                
            SetControlReference(pCurrentParam->m_SliderControl,(long)(&pCurrentParam->m_SliderContext));
                            
        }
 
    } else {
    
        printf("Dialog not OK\n");
        
    }
    
}

void DestroyParamsDialog(SFreeFrameHost_InstanceData* pInstanceData) {

    DisposeWindow(pInstanceData->ParamsWindow);
    pInstanceData->ParamsWindow=NULL;
    
}

pascal void ParameterSliderCallback(ControlHandle control,SInt16 part) {

    SFreeFrameHost_SliderContext* pSliderContext=
        (SFreeFrameHost_SliderContext*)GetControlReference( control );

    SFreeFrameHost_InstanceData* pInstanceData=pSliderContext->pInstanceData;
    const int nParameterIndex=pSliderContext->nParameterIndex;

    static void* pCastCount;
    pCastCount=reinterpret_cast<void*>(nParameterIndex);

    const int nSelectedEffect=pInstanceData->nSelectedEffect;
    SFreeFrameHost_EffectData* pEffectData=
            &pInstanceData->m_Effects[nSelectedEffect];

    FF_Main_FuncPtr pFreeFrameMain=pEffectData->m_pMainFunc;

    const int nSliderValue=GetControlValue(control);
    
    SetParameterStruct ArgStruct;
    ArgStruct.index=nParameterIndex;
    ArgStruct.value=(nSliderValue/(float)nParameterRange);

    pFreeFrameMain(FF_SETPARAMETER,&ArgStruct,0);
            
    char* pValueDisplay=(char*)(pEffectData->m_pMainFunc(FF_GETPARAMETERDISPLAY,pCastCount,0));
			
    bool bNullCharFound=false;
    int nCharCount;
    for (nCharCount=0; ((nCharCount<15)&&(!bNullCharFound)); nCharCount+=1) {
        bNullCharFound=(pValueDisplay[nCharCount]=='\0');
    }
    if (!bNullCharFound) {
        pValueDisplay[15]='\0'; // Pete- Getting unterminated strings returned
    }

    if (pValueDisplay==NULL) {
        pValueDisplay="<NULL>";
    }

    SFreeFrameHost_ParameterData* pCurrentParam=&pInstanceData->m_Params[nParameterIndex];

    SetControlData( pCurrentParam->m_ValueControl, 0, kControlStaticTextTextTag, strlen(pValueDisplay), pValueDisplay );
    DrawOneControl( pCurrentParam->m_ValueControl );
    
    NextFrame(pInstanceData);
    DrawMovie();
    
}

void DoMenuCommand(long menuResult,SFreeFrameHost_InstanceData* pInstanceData)
{
    short	menuID;		/* the resource ID of the selected menu */
    short	menuItem;	/* the item number of the selected menu */
	
    menuID = HiWord(menuResult);    /* use macros to get item & menu number */
    menuItem = LoWord(menuResult);
	
    switch (menuID) 
    {
        case mApple:
            switch (menuItem) 
            {
                case iAbout:
                    DoAboutBox();
                    break;
                    
                case iQuit:
                    ExitToShell();
                    break;
				
                default:
                    break;
            }
            break;
        
        case mFile:
            break;
		
        case mEdit:
            break;
            
        case mEffects: {
            FreeFrame_DeInitEffect(pInstanceData);
            pInstanceData->nSelectedEffect=(menuItem-1);
            FreeFrame_InitEffect(pInstanceData);
        }break;
        
        default: {
            // do nothing
        }break;
    }
    HiliteMenu(0);	/* unhighlight what MenuSelect (or MenuKey) hilited */
}

void DoAboutBox(void)
{
   (void) Alert(kAboutBox, nil);  // simple alert dialog box
}

//------------------------------------------------------------------------------
//	Open up the Movie file and get a Movie from it.
//------------------------------------------------------------------------------
void OpenMovie( void )
{
	FSSpec	theFSSpec;
	short	refnum = 0;
	OSType  openTypeList[] = {kQTFileTypeMovie};
	short 	numTypes = 1;
	
	OSErr err = noErr;

	err = GetOneFileWithPreview(numTypes, openTypeList, &theFSSpec, NULL);
	BailError(err);

	// open a Movie file using the FSSpec and create a Movie from that file.	
	err = OpenMovieFile(&theFSSpec, &refnum, fsRdPerm);
	BailError(err);

	NewMovieFromFile(&gMovie, refnum, NULL, NULL, newMovieActive, NULL);	

bail:	
	//	we're done with the file.
	if (refnum)
		CloseMovieFile(refnum);
}

//------------------------------------------------------------------------------
//	Count the number of video "frames" in the Movie by stepping through
//  all of the video "interesting times", or in other words, the places where the
//	movie displays a new video sample. The time between these interesting times
//  is not necessarily constant.
//------------------------------------------------------------------------------
void CountThemFrames( void )
{
	OSType		whichMediaType = VIDEO_TYPE;
	short		flags = nextTimeMediaSample + nextTimeEdgeOK;
	TimeValue	duration;
	TimeValue	theTime = 0;
	
	while (theTime >= 0) {
		gFrameCount++;
		GetMovieNextInterestingTime(gMovie,
									flags,
									1,
									&whichMediaType,
									theTime,
									0,
									&theTime,
									&duration);

		//	after the first interesting time, don't include the time we
		//  are currently at.

		flags = nextTimeMediaSample;
	} // while
}

//------------------------------------------------------------------------------
//	Get the bounding rectangle of the Movie the create a 32-bit GWorld
//  with those dimensions.
//	This GWorld will be used for rendering Movie frames into.
//------------------------------------------------------------------------------
void MakeGWorld( void )
{
	Rect srcRect;
	Rect portRect;
	RGBColor theBlackColor = { 0, 0, 0 };
	RGBColor theWhiteColor = { 65535, 65535, 65535 };
	
	OSErr err = noErr;
	
	GetMovieBox(gMovie,&srcRect);

	err = NewGWorld(&gDestGWorld,
					k32ARGBPixelFormat,
					&srcRect,
					NULL,
					NULL,
					0);
	BailError(err);
	
	LockPixels(GetGWorldPixMap(gDestGWorld));

	SetGWorld(gDestGWorld,NULL);
	GetPortBounds(gDestGWorld, &portRect);
	RGBBackColor(&theBlackColor);
	RGBForeColor(&theWhiteColor);
	EraseRect(&portRect);		
	
	err = NewGWorld(&gSrcGWorld,
					k32ARGBPixelFormat,
					&srcRect,
					NULL,
					NULL,
					0);
	BailError(err);
	
	LockPixels(GetGWorldPixMap(gSrcGWorld));

	SetGWorld(gSrcGWorld,NULL);
	GetPortBounds(gSrcGWorld, &portRect);
	RGBBackColor(&theBlackColor);
	RGBForeColor(&theWhiteColor);
	EraseRect(&portRect);
        		
bail:
	return;
}

//------------------------------------------------------------------------------
//	Blit the contents of the GWorld to the window
//------------------------------------------------------------------------------
void BlitToScreen( void )
{
	Rect srcRect;
	Rect destRect;
	
	GetPortBounds(gSrcGWorld, &srcRect);
	GetPortBounds(GetWindowPort(gWindow), &destRect);
	CopyBits(GetPortBitMapForCopyBits(gSrcGWorld),
			 GetPortBitMapForCopyBits(GetWindowPort(gWindow)),
			 &srcRect,
			 &destRect,
			 srcCopy,
			 NULL);
}


//------------------------------------------------------------------------------
//	First get the next frame of the movie, set the movie time for that frame,
//  then task the movie which will draw the frame to the GWorld
//  Modify the movie matrix so the next frame will be rotated just for fun and
//  finally draw the frame number on top of the image and inval the window rect
//------------------------------------------------------------------------------
void NextFrame(SFreeFrameHost_InstanceData* pInstanceData)
{
	CGrafPtr	 	savedPort;
 	GDHandle     	savedDevice;
 	Rect 		 	theRect;
	char 		 	frame[32];
	Str255		 	theString;
	Rect 		 	invalRect;
		
	if ( gFrameNumber < gFrameCount ) {

		TimeValue duration;
		
		GetGWorld(&savedPort, &savedDevice);
   		SetGWorld(gSrcGWorld, NULL);
		
		// get the next frame of the source movie
		short 	flags = nextTimeMediaSample;
		OSType	whichMediaType = VIDEO_TYPE;

		// if this is the first frame, include the frame we are currently on
		if (gFrameNumber == 0)
			flags |= nextTimeEdgeOK;

		// skip to the next interesting time and get the duration for that frame
		GetMovieNextInterestingTime(gMovie,
									flags,
									1,
									&whichMediaType,
									gMovieTime,
									0,
									&gMovieTime,
									&duration);

		// set the time for the frame and give time to the movie toolbox
		SetMovieTimeValue(gMovie,gMovieTime);
		GetPortBounds(gSrcGWorld, &theRect);
		EraseRect(&theRect);	// paint the background black
		
		// *** this does the actual drawing into the GWorld ***
		MoviesTask(gMovie,0);

//------------------------------------------------------------------------------
// You now have pixels you can play with in the GWorld!
// This sample simply blits the contents of the GWorld back to
// a window with the frame number drawn on top of the image
//------------------------------------------------------------------------------

                FreeFrame_ApplyEffect(pInstanceData);
		
		// have some fun and rotate the movie frame
//		GetMovieMatrix(gMovie, &theMovieMatrix);
//		RotateMatrix(&theMovieMatrix, degrees, x, y);
//		SetMovieMatrix(gMovie, &theMovieMatrix);		

		gFrameNumber++;

		// draw the frame number on top of the image        
 //       TextSize(12);
//        TextMode(srcCopy);
//        MoveTo( theRect.left + 5, theRect.bottom - 15 );
//        sprintf(frame,"Frame #%d",gFrameNumber);
//        CopyCStringToPascal(frame, theString);
//        DrawString(theString);
        
        SetGWorld(savedPort, savedDevice);
		
		// inval the window to generate an update event
		GetPort(&savedPort);
		SetPortWindowPort(gWindow);
		
		GetWindowBounds(gWindow, kWindowContentRgn, &invalRect);
		GlobalToLocal( (Point *)&invalRect.top);
		InvalWindowRect(gWindow, &invalRect);
		
		SetPort(savedPort);
		
	} else {
	 	// reset the movie time back to the beginning
	 	// then do it all over again
		gMovieTime = 0;
		gFrameNumber = 0;
		NextFrame(pInstanceData);
	}
}

void DrawMovie(void) {
    
    WindowRef pWhichWindow=gWindow;
    
    GrafPtr savedPort;
    CGrafPtr windowPort = GetWindowPort(pWhichWindow);
    RgnHandle vizRgn = NewRgn();
    RgnHandle savedClip = NewRgn();

    GetPort(&savedPort);
    SetPort(windowPort);
    GetClip(savedClip);
					
    BeginUpdate(pWhichWindow);
    GetPortVisibleRegion(windowPort, vizRgn);
    SetClip(vizRgn);
    
    BlitToScreen();

    EndUpdate(pWhichWindow);
					
    SetClip(savedClip);
    SetPort(savedPort);
    DisposeRgn(savedClip);
    DisposeRgn(vizRgn);
}


int main()
{
	Rect rectWnd;
	Rect rectMovie;
	WindowRef pWhichWindow;
	short nPart;
	EventRecord theEvent;
	Boolean bDone = false;
	
	OSErr result = noErr;
	
	// initialize for Carbon and QuickTime		
	InitCursor();
	result = EnterMovies();
	BailError(result);

	// create the window
	SetRect(&rectWnd, 100, 100, 200, 200);
	gWindow = NewCWindow(NULL, &rectWnd, "\pFreeFrame Mac TestBed", false, kWindowDocumentProc, (WindowPtr)-1, true, NULL);
	BailNULL(gWindow);
	SetPortWindowPort(gWindow);
	
	// get the movie
	OpenMovie();
	BailNULL(gMovie);
	
	// normalize the movie rect
	GetMovieBox(gMovie, &rectMovie);
	OffsetRect(&rectMovie, -rectMovie.left, -rectMovie.top);
	SetMovieBox(gMovie, &rectMovie);
	gMovieBox = rectMovie;
	
	SizeWindow(gWindow, rectMovie.right, rectMovie.bottom, true);
	ShowWindow(gWindow);
        
	CountThemFrames();
	
	// create the offscreen to draw into
	MakeGWorld();
	BailNULL(gSrcGWorld);
	
	// *** set the graphics world for displaying the movie ***
	SetMovieGWorld(gMovie, gSrcGWorld, GetGWorldDevice(gSrcGWorld));
	
        SFreeFrameHost_InstanceData FFInstanceData;
        FFInstanceData.nEffectsCount=0;
        FFInstanceData.nSelectedEffect=0;

        FreeFrame_LoadAllPlugins(&FFInstanceData);

        MakeMenu(&FFInstanceData);
                          
        FFInstanceData.nSourceWidth=rectMovie.right;
	FFInstanceData.nSourceHeight=rectMovie.bottom;

	FreeFrame_InitEffect(&FFInstanceData);

	NextFrame(&FFInstanceData);
		
	while (!bDone) {
		if (GetNextEvent(everyEvent, &theEvent)) {
			switch (theEvent.what) {
			case updateEvt:
				pWhichWindow = (WindowPtr)theEvent.message;
				if (pWhichWindow == gWindow) {

                                    DrawMovie();

                                } else if (pWhichWindow==FFInstanceData.ParamsWindow) {
	
                                    GrafPtr savedPort;
                                    CGrafPtr windowPort = GetWindowPort(pWhichWindow);
                                    RgnHandle vizRgn = NewRgn();
                                    RgnHandle savedClip = NewRgn();

                                    GetPort(&savedPort);
                                    SetPort(windowPort);
                                    GetClip(savedClip);
					
                                    BeginUpdate(pWhichWindow);
                                    GetPortVisibleRegion(windowPort, vizRgn);
                                    SetClip(vizRgn);

                                    Rect tempRect;
                                    EraseRect(GetWindowPortBounds(pWhichWindow, &tempRect));
                                    DrawControls(pWhichWindow);
                                    DrawGrowIcon(pWhichWindow);

                                    EndUpdate(pWhichWindow);
					
                                    SetClip(savedClip);
                                    SetPort(savedPort);
                                    DisposeRgn(savedClip);
                                    DisposeRgn(vizRgn);
                                    
				}
				break;

			case mouseDown:
				nPart = FindWindow(theEvent.where, &pWhichWindow);
                                
                                if (nPart==inMenuBar) {
                                    DoMenuCommand(MenuSelect(theEvent.where),&FFInstanceData);
                                }
                                
				if ((pWhichWindow == gWindow)||(pWhichWindow == FFInstanceData.ParamsWindow)) {
					switch (nPart) {
                                        
					case inGoAway:
						// we're done
						bDone = TrackGoAway(pWhichWindow, theEvent.where);
						break;
						
					case inContent:
						// get the next frame
						//NextFrame(&FFInstanceData);
						break;
											
					case inDrag: {
						BitMap screenBits;
						GetQDGlobalsScreenBits(&screenBits);
						DragWindow(pWhichWindow, theEvent.where, &screenBits.bounds);
						} break;
					} // switch
                                        
                                        default: {
                                        
                                            if (pWhichWindow==FFInstanceData.ParamsWindow) {
                                            
                                                GrafPtr savedPort;
                                                CGrafPtr windowPort = GetWindowPort(pWhichWindow);
                                                GetPort(&savedPort);
                                                SetPort(windowPort);

                                                GlobalToLocal(&theEvent.where);
                                        
                                                ControlPartCode WhichPart;
                                                ControlRef pWhichControl=FindControlUnderMouse (
                                                    theEvent.where, 
                                                    pWhichWindow, 
                                                    &WhichPart);
                                                    
                                                if (pWhichControl!=NULL) {
                                                    
                                                    ControlPartCode AnotherPart=HandleControlClick (
                                                        pWhichControl, 
                                                        theEvent.where, 
                                                        theEvent.modifiers, 
                                                        -1);
                                                        
                                                }
                                            
                                                SetPort(savedPort);
                                            }
                                        
                                        }break;
                                
                                }
				break;
			} // switch
		}
            
            NextFrame(&FFInstanceData);

	} // while
	
bail:
	if ( gMovie )
		DisposeMovie(gMovie);
		
	if ( gWindow )
		DisposeWindow(gWindow);
                
        return 0;
        
}

OSErr GetOneFileWithPreview (short theNumTypes, TypeListPtr theTypeList, FSSpecPtr theFSSpecPtr, void *theFilterProc)
{
	NavReplyRecord		myReply;
	NavDialogOptions	myDialogOptions;
	NavTypeListHandle	myOpenList = NULL;
	NavEventUPP			myEventUPP = NewNavEventUPP(HandleNavEvent);
	OSErr				myErr = noErr;
	
	if (theFSSpecPtr == NULL)
		return(paramErr);

	// specify the options for the dialog box
	NavGetDefaultDialogOptions(&myDialogOptions);
	myDialogOptions.dialogOptionFlags -= kNavNoTypePopup;
	myDialogOptions.dialogOptionFlags -= kNavAllowMultipleFiles;
	BlockMoveData(kApplicationName, myDialogOptions.clientName, kApplicationName[0] + 1);
	
	// create a handle to an 'open' resource
	myOpenList = (NavTypeListHandle)CreateOpenHandle(kApplicationSignature, theNumTypes, theTypeList);
	if (myOpenList != NULL)
		HLock((Handle)myOpenList);
	
	// prompt the user for a file
	myErr = NavGetFile(NULL, &myReply, &myDialogOptions, myEventUPP, NULL, (NavObjectFilterUPP)theFilterProc, myOpenList, NULL);
	if ((myErr == noErr) && myReply.validRecord) {
		AEKeyword		myKeyword;
		DescType		myActualType;
		Size			myActualSize = 0;
		
		// get the FSSpec for the selected file
		if (theFSSpecPtr != NULL)
			myErr = AEGetNthPtr(&(myReply.selection), 1, typeFSS, &myKeyword, &myActualType, theFSSpecPtr, sizeof(FSSpec), &myActualSize);

		NavDisposeReply(&myReply);
	}
	
	if (myOpenList != NULL) {
		HUnlock((Handle)myOpenList);
		DisposeHandle((Handle)myOpenList);
	}
	
	DisposeNavEventUPP(myEventUPP);
 
	return(myErr);
}

//////////
//
// CreateOpenHandle
// Get the 'open' resource or dynamically create a NavTypeListHandle.
//
//////////
Handle CreateOpenHandle (OSType theApplicationSignature, short theNumTypes, TypeListPtr theTypeList)
{
	Handle			myHandle = NULL;
	
	// see if we have an 'open' resource...
	myHandle = Get1Resource('open', 128);
	if ( myHandle != NULL && ResError() == noErr ) {
		DetachResource( myHandle );
		return myHandle;
	} else {
		myHandle = NULL;
	}
	
	// nope, use the passed in types and dynamically create the NavTypeList
	if (theTypeList == NULL)
		return myHandle;
	
	if (theNumTypes > 0) {
		myHandle = NewHandle(sizeof(NavTypeList) + (theNumTypes * sizeof(OSType)));
		if (myHandle != NULL) {
			NavTypeListHandle 	myOpenResHandle	= (NavTypeListHandle)myHandle;
			
			(*myOpenResHandle)->componentSignature = theApplicationSignature;
			(*myOpenResHandle)->osTypeCount = theNumTypes;
			BlockMoveData(theTypeList, (*myOpenResHandle)->osType, theNumTypes * sizeof(OSType));
		}
	}
	
	return myHandle;
}

//////////
//
// HandleNavEvent
// A callback procedure that handles events while a Navigation Service dialog box is displayed.
//
//////////
PASCAL_RTN void HandleNavEvent(NavEventCallbackMessage theCallBackSelector, NavCBRecPtr theCallBackParms, void *theCallBackUD)
{
#pragma unused(theCallBackUD)
	
	if (theCallBackSelector == kNavCBEvent) {
		switch (theCallBackParms->eventData.eventDataParms.event->what) {
			case updateEvt:
#if TARGET_OS_MAC
				// Handle Update Event
#endif
				break;
			case nullEvent:
				// Handle Null Event
				break;
		}
	}
}

void FreeFrame_LoadAllPlugins(SFreeFrameHost_InstanceData* pInstanceData) {

    char* pPluginDirName="./plugins";

    DIR* pPluginDir=opendir(pPluginDirName);
    if (pPluginDir==NULL) {
        printf("Couldn't find plugin directory '%s'!\n",pPluginDirName);
        return;
    }
    
    dirent* pCurrentEntry=readdir(pPluginDir);
    while (pCurrentEntry!=NULL) {
    
        printf("Plugin: '%s'\n",pCurrentEntry->d_name);
        
        int nEffectsLoaded=pInstanceData->nEffectsCount;

        SFreeFrameHost_EffectData* pCurrentEffect=
            &pInstanceData->m_Effects[nEffectsLoaded];
                    
        char pFullFileName[4096];
        strcpy(pFullFileName,pPluginDirName);
        strcat(pFullFileName,"/");
        strcat(pFullFileName,pCurrentEntry->d_name);

        const bool bEffectLoaded=
            FreeFrame_LoadPlugin(pCurrentEffect,pFullFileName);

        if (bEffectLoaded) {
            nEffectsLoaded+=1;
        }

        pInstanceData->nEffectsCount=nEffectsLoaded;
        
        pCurrentEntry=readdir(pPluginDir);
        
    }
    
}

bool FreeFrame_LoadPlugin(SFreeFrameHost_EffectData* pEffectData,char* pFileName) {
	
    bool bLoadSucceeded=false;

    printf("Attempting to load '%s'\n",pFileName);

    NSObjectFileImageReturnCode CreateObjectFileImageResult=
    NSCreateObjectFileImageFromFile(
        pFileName,
        &pEffectData->m_ObjectFileImage);

    if (CreateObjectFileImageResult==NSObjectFileImageSuccess) {
        
        pEffectData->m_Module=NSLinkModule(
            pEffectData->m_ObjectFileImage,
            pFileName,
            NSLINKMODULE_OPTION_BINDNOW|
            NSLINKMODULE_OPTION_PRIVATE|
            NSLINKMODULE_OPTION_RETURN_ON_ERROR);

        if (pEffectData->m_Module!=NULL) {
        
            printf("Module OK\n");
        
            NSSymbol FFMainSymbol=
                NSLookupSymbolInModule(pEffectData->m_Module,"_plugMain");
                
            if (FFMainSymbol!=NULL) {

                printf("Symbol OK\n");

                FF_Main_FuncPtr pFreeFrameMain=
                    (FF_Main_FuncPtr)NSAddressOfSymbol(FFMainSymbol);

                if (pFreeFrameMain!=NULL) {
                
                    printf("Address OK\n");
                
                    pEffectData->m_pMainFunc=pFreeFrameMain;
                    bLoadSucceeded=true;

                }
                
            }
            
        }

    } else {
    
        switch (CreateObjectFileImageResult) {
        
            case NSObjectFileImageFailure:
                printf("NSObjectFileImageFailure\n");
            break;

            case NSObjectFileImageInappropriateFile:
                 printf("NSObjectFileImageInappropriateFile\n");
            break;
            
            case NSObjectFileImageArch:
                printf("NSObjectFileImageArch\n");            
            break;

            case NSObjectFileImageFormat:
                printf("NSObjectFileImageFormat\n");
            break;

            case NSObjectFileImageAccess:
                printf("NSObjectFileImageAccess\n");            
            break;
            
            default:
                printf("NSObjectFileImage:Unknown\n");
            break;

        }

    }
    
    return bLoadSucceeded;

}

void FreeFrame_InitEffect(SFreeFrameHost_InstanceData* pInstanceData) {

    const int nSelectedEffect=pInstanceData->nSelectedEffect;
    SFreeFrameHost_EffectData* pEffectData=
        &pInstanceData->m_Effects[nSelectedEffect];

    FF_Main_FuncPtr pFreeFrameMain=pEffectData->m_pMainFunc;

    VideoInfoStruct* pVideoInfo=&pInstanceData->VideoInfo;

    const int nSourceWidth=pInstanceData->nSourceWidth;
    const int nSourceHeight=pInstanceData->nSourceHeight;

    pVideoInfo->frameWidth=nSourceWidth;
    pVideoInfo->frameHeight=nSourceHeight;

    if (pFreeFrameMain!=NULL) {

        pFreeFrameMain(FF_DEINITIALISE,NULL,0);

        if ((DWORD)pFreeFrameMain(FF_GETPLUGINCAPS,(void*)2,0)==FF_TRUE) {
                pVideoInfo->bitDepth=2;
        } else if ((DWORD)pFreeFrameMain(FF_GETPLUGINCAPS,(void*)1,0)==FF_TRUE) {
                pVideoInfo->bitDepth=1;
        } else if ((DWORD)pFreeFrameMain(FF_GETPLUGINCAPS,(void*)0,0)==FF_TRUE) {
                pVideoInfo->bitDepth=0;
        } else {
                assert(FALSE);
                pVideoInfo->bitDepth=2;
        }	

        pFreeFrameMain(FF_INITIALISE,pVideoInfo,0);
	
    }
 
    MakeParamsDialog(pInstanceData);

}

void FreeFrame_DeInitEffect(SFreeFrameHost_InstanceData* pInstanceData) {

    DestroyParamsDialog(pInstanceData);

    const int nSelectedEffect=pInstanceData->nSelectedEffect;
    SFreeFrameHost_EffectData* pEffectData=
            &pInstanceData->m_Effects[nSelectedEffect];

    FF_Main_FuncPtr pFreeFrameMain=pEffectData->m_pMainFunc;

    if (pFreeFrameMain!=NULL) {

            pFreeFrameMain(FF_DEINITIALISE,NULL,0);
	
    }

}

void FreeFrame_ApplyEffect(SFreeFrameHost_InstanceData* pInstanceData) {

    Rect SourceRect;
    GetPortBounds(gSrcGWorld, &SourceRect);

    const int nWidth=(SourceRect.right-SourceRect.left);
    const int nHeight=(SourceRect.bottom-SourceRect.top);

    PixMapHandle hSourcePixMap=GetGWorldPixMap(gSrcGWorld);

    Ptr pSourceData=GetPixBaseAddr(hSourcePixMap);
    const int nSourceRowBytes=GetPixRowBytes(hSourcePixMap);
 
    PixMapHandle hDestPixMap=GetGWorldPixMap(gDestGWorld);

    Ptr pDestData=GetPixBaseAddr(hDestPixMap);
    const int nDestRowBytes=(nWidth*sizeof(U32));

    const int nSelectedEffect=pInstanceData->nSelectedEffect;
    SFreeFrameHost_EffectData* pEffectData=
        &pInstanceData->m_Effects[nSelectedEffect];

    FF_Main_FuncPtr pFreeFrameMain=pEffectData->m_pMainFunc;

    VideoInfoStruct* pVideoInfo=&pInstanceData->VideoInfo;

    if (pFreeFrameMain==NULL) {
        return;
    }

    const int nBitDepth=pVideoInfo->bitDepth;

    int nY;
    for (nY=0; nY<nHeight; nY+=1) {

        U32* pSourceRowStart=(U32*)
            (((char*)pSourceData)+(nY*nSourceRowBytes));
        U32* pSourceRowEnd=(pSourceRowStart+nWidth);

        U32* pDestRowStart=(U32*)
            (((char*)pDestData)+(nY*nDestRowBytes));

        U32* pCurrentSource=pSourceRowStart;
        U32* pCurrentDest=pDestRowStart;

        while (pCurrentSource<pSourceRowEnd) {

            U32 SourceColour=*pCurrentSource;

            U32 OutputColour=SourceColour;

            *pCurrentDest=OutputColour;

            pCurrentDest+=1;
            pCurrentSource+=1;
					
        }

        switch (nBitDepth) {

            case 2: { // 32 bit
                // do nothing
            }break;
			
            case 1: { // 24 bit
                Pete_CopyAndConvert32BitTo24Bit(pDestRowStart,pDestRowStart,nWidth);
            }break;
			
            case 0: { // 16 bit
                Pete_CopyAndConvert32BitTo16Bit565(pDestRowStart,(U16*)pDestRowStart,nWidth);
            }break;
			
            default: {
                assert(FALSE);
            }break;

        }

    }
		
    pFreeFrameMain(FF_PROCESSFRAME,pDestData,0);

    for (nY=0; nY<nHeight; nY+=1) {

        U32* pSourceRowStart=(U32*)
            (((char*)pSourceData)+(nY*nSourceRowBytes));

        U32* pDestRowStart=(U32*)
            (((char*)pDestData)+(nY*nDestRowBytes));

        switch (nBitDepth) {

            case 2: { // 32 bit
                memcpy(pSourceRowStart,pDestRowStart,(nWidth*sizeof(U32)));
            }break;
			
            case 1: { // 24 bit
                Pete_CopyAndConvert24BitTo32Bit(pDestRowStart,pSourceRowStart,nWidth);
            }break;
			
            case 0: { // 16 bit
                Pete_CopyAndConvert16Bit565To32Bit((U16*)pDestRowStart,pSourceRowStart,nWidth);
            }break;
			
            default: {
                assert(FALSE);
            }break;

        }

    }
    
}

char* FreeFrame_GetEffectName(SFreeFrameHost_EffectData* pEffectData) {

	FF_Main_FuncPtr pFreeFrameMain=pEffectData->m_pMainFunc;

	if (pFreeFrameMain==NULL) {
		return "<NULL>";
	}

	PlugInfoStruct* pInfoStruct=(PlugInfoStruct*)pEffectData->m_pMainFunc(FF_GETINFO,NULL,0);
	
	if (pInfoStruct==NULL) {
		return "<PNULL>";
	}

	char* pPluginName=(char*)(&pInfoStruct->pluginName[0]);
	static char TerminatedName[256];
	
	int nCharCount=0;
	while ((pPluginName[nCharCount]!='\0')&&(nCharCount<16)) {
		TerminatedName[nCharCount]=pPluginName[nCharCount];
		nCharCount+=1;
	}
	TerminatedName[nCharCount]='\0';

	return TerminatedName;
}

void FreeFrame_AddEffectsToMenu(SFreeFrameHost_InstanceData* pInstanceData) {

    MenuRef EffectsMenu=GetMenuHandle( mEffects );

    const int nEffectsCount=pInstanceData->nEffectsCount;

    int nCount;
    for (nCount=0; nCount<nEffectsCount; nCount+=1) {

        SFreeFrameHost_EffectData* pEffectData=
            &pInstanceData->m_Effects[nCount];
        
        char* pEffectName=FreeFrame_GetEffectName(pEffectData);            

        char pMenuString[256];
        pMenuString[0]=strlen(pEffectName);
        strcpy(&(pMenuString[1]),pEffectName);
        
        AppendMenu(EffectsMenu,pMenuString);
            
    }
    
}
