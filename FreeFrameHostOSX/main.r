/* main.r */

#include <Carbon/Carbon.r>
#include "ResourceHeader.h"

resource 'MBAR' (rMenuBar, preload) {
        {       /* array MenuArray: 3 elements */
                /* [1] */
                128,
                /* [2] */
                129,
                /* [3] */
                130,
                /* [4] */
                131,
        }
};

resource 'MENU' (mApple, preload) {
        128,
        textMenuProc,
        0x7FFFFFFD,
        enabled,
        apple,
        {       /* array: 2 elements */
                /* [1] */
                "About TestBed", noIcon, noKey, noMark, plain,
                /* [2] */
                "-", noIcon, noKey, noMark, plain
        }
};

resource 'MENU' (mFile, preload) {
        129,
        textMenuProc,
        0x1400,
        enabled,
        "File",
        {       /* array: 11 elements */
                /* [1] */
                "New", noIcon, "N", noMark, plain,
                /* [2] */
                "Open", noIcon, "O", noMark, plain,
                /* [3] */
                "-", noIcon, noKey, noMark, plain,
                /* [4] */
                "Close", noIcon, "W", noMark, plain,
                /* [5] */
                "Save", noIcon, "S", noMark, plain,
                /* [6] */
                "Save As…", noIcon, noKey, noMark, plain,
                /* [7] */
                "-", noIcon, noKey, noMark, plain,
                /* [8] */
                "Page Setup…", noIcon, noKey, noMark, plain,
                /* [9] */
                "Print…", noIcon, noKey, noMark, plain,
                /* [10] */
                "-", noIcon, noKey, noMark, plain,
                /* [11] */
                "Quit", noIcon, "Q", noMark, plain
        }
};

resource 'MENU' (mEdit, preload) {
        130,
        textMenuProc,
        0x0,
        enabled,
        "Edit",
        {       /* array: 6 elements */
                /* [1] */
                "Undo", noIcon, "Z", noMark, plain,
                /* [2] */
                "-", noIcon, noKey, noMark, plain,
                /* [3] */
                "Cut", noIcon, "X", noMark, plain,
                /* [4] */
                "Copy", noIcon, "C", noMark, plain,
                /* [5] */
                "Paste", noIcon, "V", noMark, plain,
                /* [6] */
                "Clear", noIcon, noKey, noMark, plain
        }
};

resource 'MENU' (mEffects, preload) {
        131,
        textMenuProc,
        0x0,
        enabled,
        "Effects",
        {       /* array: 0 elements */
        }
};

resource 'DITL' (kAboutBox) {
        {       /* array DITLarray: 3 elements */
                /* [1] */
                {16, 21, 38, 208},
                StaticText {
                        disabled,
                        "FreeFrame Mac Test"
                },
                /* [2] */
                {116, 287, 136, 345},
                Button {
                        enabled,
                        "OK"
                },
                /* [3] */
                {54, 139, 74, 197},
                Button {
                        enabled,
                        "OK"
                },
        }
};

resource 'ALRT' (kAboutBox) {
/*        {40, 40, 139, 280},*/
        {40,40,640,640},
        200,
        {       /* array: 4 elements */
                /* [1] */
                OK, visible, sound1,
                /* [2] */
                OK, visible, sound1,
                /* [3] */
                OK, visible, sound1,
                /* [4] */
                OK, visible, sound1
        },
	noAutoCenter
};

resource 'WIND' (129) {
        {137, 122, 283, 458},
        1024,
        visible,
        noGoAway,
        0x0,
        "Effect Parameters",
        staggerParentWindow
};

resource 'DITL' (129) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{8, 74, 61, 356},
		StaticText {
			disabled,
			"^0"
		},
		/* [2] */
		{70, 299, 90, 357},
		Button {
			enabled,
			"OK"
		}
	}
};

resource 'DLOG' (1000, "AboutBox") {
	{0, 0, 150, 400},
	kWindowModalDialogProc, visible, goAway, 0x0, 1000, "", centerMainScreen
};

resource 'dlgx' (1000) {
	versionZero {
            kDialogFlagsUseThemeBackground|
            kDialogFlagsUseControlHierarchy|
            kDialogFlagsHandleMovableModal|
            kDialogFlagsUseThemeControls
	}
};


resource 'DITL' (3000) {
	 {
/* 1 */ {118, 330, 138, 388},
		button {
			enabled,
			"OK"
		};
/* 2 */ {12, 12, 110, 388},				/* SourceLanguage Item */
		staticText {
			disabled,
			"Sample Carbon Application using Speech Recognition.\n\n"
			"Version 1.1"
		}
	}
};


resource 'DITL' (1000, "Sliders")
{
	{	/* array DITLarray: 9 elements */
		/* [1] */
		{30+15, 22, 30+26+15, 22+143},
		Control {
			enabled,
			6201
		},
		/* [2] */
		{60+15, 22, 60+26+15, 22+143},
		Control {
			enabled,
			6202
		},
		/* [3] */
		{90+15, 22, 90+26+15, 22+143},
		Control {
			enabled,
			6203
		},
		/* [4] */
		{124+15, 23, 124+100+15, 23+26},
		Control {
			enabled,
			6204
		},
		/* [5] */
		{124+15, 87, 124+100+15, 87+41},
		Control {
			enabled,
			6205
		},
		/* [6] */
		{124+15, 143, 124+100+15, 143+26},
		Control {
			enabled,
			6206
		},
		/* [7] */
		{171+15, 224, 193+15, 345},
		CheckBox {
			enabled,
			"Live feedback"
		},
		/* [8] */
		{139+15, 265, 156+15, 309},
		StaticText {
			disabled,
			"0"
		},
		/* [9] */
		{38+15, 224, 133+15, 351},
		Control {
			enabled,
			6207
		}
	}
};

resource 'CNTL' (6201, "Mega Dialog - Down pointing slider")
{
	{0, 0, 26, 143},
	0,
	visible,
	255,
	0,
	49,
	0,
	""
};

resource 'CNTL' (6202, "Mega Dialog - Up pointing slider")
{
	{0, 0, 26, 143},
	0,
	visible,
	255,
	0,
	53,
	0,
	""
};

resource 'CNTL' (6203, "Mega Dialog - Non-directional slider (horiz)")
{
	{0, 0, 26, 143},
	0,
	visible,
	255,
	0,
	57,
	0,
	""
};

resource 'CNTL' (6204, "Mega Dialog - Left pointing slider")
{
	{0, 0, 100, 26},
	0,
	visible,
	255,
	0,
	53,
	0,
	""
};

resource 'CNTL' (6205, "Mega Dialog - Right pointing slider w/Tick marks")
{
	{0, 0, 100, 41},
	5,
	visible,
	255,
	0,
	51,
	0,
	""
};

resource 'CNTL' (6206, "Mega Dialog - non-directional slider (vert)")
{
	{0, 0, 100, 26},
	0,
	visible,
	255,
	0,
	57,
	0,
	""
};
