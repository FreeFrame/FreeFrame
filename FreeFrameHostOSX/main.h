
#ifndef __VIDEO_FRAME_TO_GWORLD__
	#define __VIDEO_FRAME_TO_GWORLD__

#ifndef TARGET_API_MAC_CARBON
	#define TARGET_API_MAC_CARBON 1
#endif

#ifndef DONT_INCLUDE_NONSENSE

#include <ConditionalMacros.h>
#include <Movies.h>
#include <MoviesFormat.h>
#include <QuickTimeComponents.h>
#include <TextUtils.h>
#include <FixMath.h>
#include <Resources.h>
#include <Navigation.h>
#include <stdio.h>

// Typedefs
//------------------------------------------------------------------------------
typedef const OSTypePtr TypeListPtr;

#if TARGET_OS_MAC
	typedef MenuHandle			MenuReference;
	typedef WindowPtr			WindowReference;
	typedef NavObjectFilterUPP	QTFrameFileFilterUPP;
#endif

#if TARGET_OS_WIN32
	typedef HMENU				MenuReference;
	typedef HWND				WindowReference;
	typedef FileFilterUPP		QTFrameFileFilterUPP;
#endif

// Defines & Constants
//------------------------------------------------------------------------------
#define	BailNULL(n)  if (!n) goto bail;
#define	BailError(n) if (n) goto bail;
#if TARGET_OS_MAC
	#define PASCAL_RTN pascal
#endif

#endif // DONT_INCLUDE_NONSENSE

const OSType    kApplicationSignature  = FOUR_CHAR_CODE('vfgw');
const ResType   kOpenResourceType = FOUR_CHAR_CODE('open');
const StringPtr kApplicationName = "\pVideoFrameToGWorld";

// Prototypes
//------------------------------------------------------------------------------
void OpenMovie( void );
void CountThemFrames( void );
void MakeGWorld( void );
void BlitToScreen( void );
void NextFrame( void );
OSErr GetOneFileWithPreview (short theNumTypes, TypeListPtr theTypeList, FSSpecPtr theFSSpecPtr, void *theFilterProc);
Handle CreateOpenHandle (OSType theApplicationSignature, short theNumTypes, TypeListPtr theTypeList);
PASCAL_RTN void HandleNavEvent(NavEventCallbackMessage theCallBackSelector, NavCBRecPtr theCallBackParms, void *theCallBackUD);
#endif // __VIDEO_FRAME_TO_GWORLD__