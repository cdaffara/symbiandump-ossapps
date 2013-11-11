/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
* 
* Description: This module contains the implementation of 
* CAknUiEnvProxy class member functions.
*
*/

// INCLUDE FILES
#include "EventUtil.h"
#include <e32def.h>
#include <e32keys.h>
#include <w32std.h>

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS

/**
 * Key code names definition macro
 */
#define DefineKeyCodeNames( varName ) 			\
	static TText* const varName[] =				\
        {										\
        (TText*)L"ekeynull",			\
        (TText*)L"ekeybell",			\
        (TText*)L"ekeybackspace",		\
        (TText*)L"ekeytab",				\
        (TText*)L"ekeylinefeed",		\
        (TText*)L"ekeyverticaltab",		\
        (TText*)L"ekeyformfeed",		\
        (TText*)L"ekeyenter",			\
        (TText*)L"ekeyescape",			\
        (TText*)L"ekeyspace",			\
        (TText*)L"ekeydelete",			\
        (TText*)L"ekeyprintscreen",		\
        (TText*)L"ekeypause",			\
        (TText*)L"ekeyhome",			\
        (TText*)L"ekeyend",				\
        (TText*)L"ekeypageup",			\
        (TText*)L"ekeypagedown",		\
        (TText*)L"ekeyinsert",			\
        (TText*)L"ekeyleftarrow",		\
        (TText*)L"ekeyrightarrow",		\
        (TText*)L"ekeyuparrow",			\
        (TText*)L"ekeydownarrow",		\
        (TText*)L"ekeyleftshift",		\
        (TText*)L"ekeyrightshift",		\
        (TText*)L"ekeyleftalt",			\
        (TText*)L"ekeyrightalt",		\
        (TText*)L"ekeyleftctrl",		\
        (TText*)L"ekeyrightctrl",		\
        (TText*)L"ekeyleftfunc",		\
        (TText*)L"ekeyrightfunc",		\
        (TText*)L"ekeycapslock",		\
        (TText*)L"ekeynumlock",			\
        (TText*)L"ekeyscrolllock",		\
        (TText*)L"ekeyf1",				\
        (TText*)L"ekeyf2",				\
        (TText*)L"ekeyf3",				\
        (TText*)L"ekeyf4",				\
        (TText*)L"ekeyf5",				\
        (TText*)L"ekeyf6",				\
        (TText*)L"ekeyf7",				\
        (TText*)L"ekeyf8",				\
        (TText*)L"ekeyf9",				\
        (TText*)L"ekeyf10",				\
        (TText*)L"ekeyf11",				\
        (TText*)L"ekeyf12",				\
        (TText*)L"ekeyf13",				\
        (TText*)L"ekeyf14",				\
        (TText*)L"ekeyf15",				\
        (TText*)L"ekeyf16",				\
        (TText*)L"ekeyf17",				\
        (TText*)L"ekeyf18",				\
        (TText*)L"ekeyf19",				\
        (TText*)L"ekeyf20",				\
        (TText*)L"ekeyf21",				\
        (TText*)L"ekeyf22",				\
        (TText*)L"ekeyf23",				\
        (TText*)L"ekeyf24",				\
        (TText*)L"ekeyoff",				\
        (TText*)L"ekeyinccontrast",		\
        (TText*)L"ekeydeccontrast",		\
        (TText*)L"ekeybacklighton",		\
        (TText*)L"ekeybacklightoff",	\
        (TText*)L"ekeybacklighttoggle",	\
        (TText*)L"ekeysliderdown",		\
        (TText*)L"ekeysliderup",		\
        (TText*)L"ekeymenu",			\
        (TText*)L"ekeydictaphoneplay",	\
        (TText*)L"ekeydictaphonestop",	\
        (TText*)L"ekeydictaphonerecord",\
        (TText*)L"ekeyhelp",			\
        (TText*)L"ekeydial",			\
        (TText*)L"ekeyscreendimension0",\
        (TText*)L"ekeyscreendimension1",\
        (TText*)L"ekeyscreendimension2",\
        (TText*)L"ekeyscreendimension3",\
        (TText*)L"ekeyincvolume",		\
        (TText*)L"ekeydecvolume",		\
        (TText*)L"ekeydevice0",			\
        (TText*)L"ekeydevice1",			\
        (TText*)L"ekeydevice2",			\
        (TText*)L"ekeydevice3",			\
        (TText*)L"ekeydevice4",			\
        (TText*)L"ekeydevice5",			\
        (TText*)L"ekeydevice6",			\
        (TText*)L"ekeydevice7",			\
        (TText*)L"ekeydevice8",			\
        (TText*)L"ekeydevice9",			\
        (TText*)L"ekeydevicea",			\
        (TText*)L"ekeydeviceb",			\
        (TText*)L"ekeydevicec",			\
        (TText*)L"ekeydeviced",			\
        (TText*)L"ekeydevicee",			\
        (TText*)L"ekeydevicef",			\
        (TText*)L"ekeyapplication0",	\
        (TText*)L"ekeyapplication1",	\
        (TText*)L"ekeyapplication2",	\
        (TText*)L"ekeyapplication3",	\
        (TText*)L"ekeyapplication4",	\
        (TText*)L"ekeyapplication5",	\
        (TText*)L"ekeyapplication6",	\
        (TText*)L"ekeyapplication7",	\
        (TText*)L"ekeyapplication8",	\
        (TText*)L"ekeyapplication9",	\
        (TText*)L"ekeyapplicationa",	\
        (TText*)L"ekeyapplicationb",	\
        (TText*)L"ekeyapplicationc",	\
        (TText*)L"ekeyapplicationd",	\
        (TText*)L"ekeyapplicatione",	\
        (TText*)L"ekeyapplicationf",	\
        (TText*)L"ekeyyes",				\
        (TText*)L"ekeyno",				\
        (TText*)L"ekeyincbrightness",	\
        (TText*)L"ekeydecbrightness",	\
        (TText*)L"ekeykeyboardextend",	\
        (TText*)L"ekeydevice10",		\
        (TText*)L"ekeydevice11",		\
        (TText*)L"ekeydevice12",		\
        (TText*)L"ekeydevice13",		\
        (TText*)L"ekeydevice14",		\
        (TText*)L"ekeydevice15",		\
        (TText*)L"ekeydevice16",		\
        (TText*)L"ekeydevice17",		\
        (TText*)L"ekeydevice18",		\
        (TText*)L"ekeydevice19",		\
        (TText*)L"ekeydevice1a",		\
        (TText*)L"ekeydevice1b",		\
        (TText*)L"ekeydevice1c",		\
        (TText*)L"ekeydevice1d",		\
        (TText*)L"ekeydevice1e",		\
        (TText*)L"ekeydevice1f",		\
        (TText*)L"ekeyapplication10",	\
        (TText*)L"ekeyapplication11",	\
        (TText*)L"ekeyapplication12",	\
        (TText*)L"ekeyapplication13",	\
        (TText*)L"ekeyapplication14",	\
        (TText*)L"ekeyapplication15",	\
        (TText*)L"ekeyapplication16",	\
        (TText*)L"ekeyapplication17",	\
        (TText*)L"ekeyapplication18",	\
        (TText*)L"ekeyapplication19",	\
        (TText*)L"ekeyapplication1a",	\
        (TText*)L"ekeyapplication1b",	\
        (TText*)L"ekeyapplication1c",	\
        (TText*)L"ekeyapplication1d",	\
        (TText*)L"ekeyapplication1e",	\
        (TText*)L"ekeyapplication1f",	\
        }

/**
 * Key codes definition macro
 */
#define DefineKeyCodes( varName )		\
	static TUint const varName[] = 		\
		{								\
        (TUint)EKeyNull,				\
        (TUint)EKeyBell,				\
        (TUint)EKeyBackspace,			\
        (TUint)EKeyTab,					\
        (TUint)EKeyLineFeed,			\
        (TUint)EKeyVerticalTab,			\
        (TUint)EKeyFormFeed,			\
        (TUint)EKeyEnter,				\
        (TUint)EKeyEscape,				\
        (TUint)EKeySpace,				\
        (TUint)EKeyDelete,				\
        (TUint)EKeyPrintScreen,			\
        (TUint)EKeyPause,				\
        (TUint)EKeyHome,				\
        (TUint)EKeyEnd,					\
        (TUint)EKeyPageUp,				\
        (TUint)EKeyPageDown,			\
        (TUint)EKeyInsert,				\
        (TUint)EKeyLeftArrow,			\
        (TUint)EKeyRightArrow,			\
        (TUint)EKeyUpArrow,				\
        (TUint)EKeyDownArrow,			\
        (TUint)EKeyLeftShift,			\
        (TUint)EKeyRightShift,			\
        (TUint)EKeyLeftAlt,				\
        (TUint)EKeyRightAlt,			\
        (TUint)EKeyLeftCtrl,			\
        (TUint)EKeyRightCtrl,			\
        (TUint)EKeyLeftFunc,			\
        (TUint)EKeyRightFunc,			\
        (TUint)EKeyCapsLock,			\
        (TUint)EKeyNumLock,				\
        (TUint)EKeyScrollLock,			\
        (TUint)EKeyF1,					\
        (TUint)EKeyF2,					\
        (TUint)EKeyF3,					\
        (TUint)EKeyF4,					\
        (TUint)EKeyF5,					\
        (TUint)EKeyF6,					\
        (TUint)EKeyF7,					\
        (TUint)EKeyF8,					\
        (TUint)EKeyF9,					\
        (TUint)EKeyF10,					\
        (TUint)EKeyF11,					\
        (TUint)EKeyF12,					\
        (TUint)EKeyF13,					\
        (TUint)EKeyF14,					\
        (TUint)EKeyF15,					\
        (TUint)EKeyF16,					\
        (TUint)EKeyF17,					\
        (TUint)EKeyF18,					\
        (TUint)EKeyF19,					\
        (TUint)EKeyF20,					\
        (TUint)EKeyF21,					\
        (TUint)EKeyF22,					\
        (TUint)EKeyF23,					\
        (TUint)EKeyF24,					\
        (TUint)EKeyOff,					\
        (TUint)EKeyIncContrast,			\
        (TUint)EKeyDecContrast,			\
        (TUint)EKeyBacklightOn,			\
        (TUint)EKeyBacklightOff,		\
        (TUint)EKeyBacklightToggle,		\
        (TUint)EKeySliderDown,			\
        (TUint)EKeySliderUp,			\
        (TUint)EKeyMenu,				\
        (TUint)EKeyDictaphonePlay,		\
        (TUint)EKeyDictaphoneStop,		\
        (TUint)EKeyDictaphoneRecord,	\
        (TUint)EKeyHelp,				\
        (TUint)EKeyDial,				\
        (TUint)EKeyScreenDimension0,	\
        (TUint)EKeyScreenDimension1,	\
        (TUint)EKeyScreenDimension2,	\
        (TUint)EKeyScreenDimension3,	\
        (TUint)EKeyIncVolume,			\
        (TUint)EKeyDecVolume,			\
        (TUint)EKeyDevice0,				\
        (TUint)EKeyDevice1,				\
        (TUint)EKeyDevice2,				\
        (TUint)EKeyDevice3,				\
        (TUint)EKeyDevice4,				\
        (TUint)EKeyDevice5,				\
        (TUint)EKeyDevice6,				\
        (TUint)EKeyDevice7,				\
        (TUint)EKeyDevice8,				\
        (TUint)EKeyDevice9,				\
        (TUint)EKeyDeviceA,				\
        (TUint)EKeyDeviceB,				\
        (TUint)EKeyDeviceC,				\
        (TUint)EKeyDeviceD,				\
        (TUint)EKeyDeviceE,				\
        (TUint)EKeyDeviceF,				\
        (TUint)EKeyApplication0,		\
        (TUint)EKeyApplication1,		\
        (TUint)EKeyApplication2,		\
        (TUint)EKeyApplication3,		\
        (TUint)EKeyApplication4,		\
        (TUint)EKeyApplication5,		\
        (TUint)EKeyApplication6,		\
        (TUint)EKeyApplication7,		\
        (TUint)EKeyApplication8,		\
        (TUint)EKeyApplication9,		\
        (TUint)EKeyApplicationA,		\
        (TUint)EKeyApplicationB,		\
        (TUint)EKeyApplicationC,		\
        (TUint)EKeyApplicationD,		\
        (TUint)EKeyApplicationE,		\
        (TUint)EKeyApplicationF,		\
        (TUint)EKeyYes,					\
        (TUint)EKeyNo,					\
        (TUint)EKeyIncBrightness,		\
        (TUint)EKeyDecBrightness,		\
        (TUint)EKeyKeyboardExtend,		\
        (TUint)EKeyDevice10,			\
        (TUint)EKeyDevice11,			\
        (TUint)EKeyDevice12,			\
        (TUint)EKeyDevice13,			\
        (TUint)EKeyDevice14,			\
        (TUint)EKeyDevice15,			\
        (TUint)EKeyDevice16,			\
        (TUint)EKeyDevice17,			\
        (TUint)EKeyDevice18,			\
        (TUint)EKeyDevice19,			\
        (TUint)EKeyDevice1A,			\
        (TUint)EKeyDevice1B,			\
        (TUint)EKeyDevice1C,			\
        (TUint)EKeyDevice1D,			\
        (TUint)EKeyDevice1E,			\
        (TUint)EKeyDevice1F,			\
        (TUint)EKeyApplication10,		\
        (TUint)EKeyApplication11,		\
        (TUint)EKeyApplication12,		\
        (TUint)EKeyApplication13,		\
        (TUint)EKeyApplication14,		\
        (TUint)EKeyApplication15,		\
        (TUint)EKeyApplication16,		\
        (TUint)EKeyApplication17,		\
        (TUint)EKeyApplication18,		\
        (TUint)EKeyApplication19,		\
        (TUint)EKeyApplication1A,		\
        (TUint)EKeyApplication1B,		\
        (TUint)EKeyApplication1C,		\
        (TUint)EKeyApplication1D,		\
        (TUint)EKeyApplication1E,		\
        (TUint)EKeyApplication1F,		\
		}

/**
 * Key scan code names definition macro.
 */
#define DefineKeyScanCodeNames( varName ) 			\
	static TText* const varName[] =					\
        {											\
        (TText*)L"estdkeynull",						\
        (TText*)L"estdkeybackspace",				\
        (TText*)L"estdkeytab",						\
        (TText*)L"estdkeyenter",					\
        (TText*)L"estdkeyescape",					\
        (TText*)L"estdkeyspace",					\
        (TText*)L"estdkeyprintscreen",				\
        (TText*)L"estdkeypause",					\
        (TText*)L"estdkeyhome",						\
        (TText*)L"estdkeyend",						\
        (TText*)L"estdkeypageup",					\
        (TText*)L"estdkeypagedown",					\
        (TText*)L"estdkeyinsert",					\
        (TText*)L"estdkeydelete",					\
        (TText*)L"estdkeyleftarrow",				\
        (TText*)L"estdkeyrightarrow",				\
        (TText*)L"estdkeyuparrow",					\
        (TText*)L"estdkeydownarrow",				\
        (TText*)L"estdkeyleftshift",				\
        (TText*)L"estdkeyrightshift",				\
        (TText*)L"estdkeyleftalt",					\
        (TText*)L"estdkeyrightalt",					\
        (TText*)L"estdkeyleftctrl",					\
        (TText*)L"estdkeyrightctrl",				\
        (TText*)L"estdkeyleftfunc",					\
        (TText*)L"estdkeyrightfunc",				\
        (TText*)L"estdkeycapslock",					\
        (TText*)L"estdkeynumlock",					\
        (TText*)L"estdkeyscrolllock",				\
        (TText*)L"estdkeyf1",						\
        (TText*)L"estdkeyf2",						\
        (TText*)L"estdkeyf3",						\
        (TText*)L"estdkeyf4",						\
        (TText*)L"estdkeyf5",						\
        (TText*)L"estdkeyf6",						\
        (TText*)L"estdkeyf7",						\
        (TText*)L"estdkeyf8",						\
        (TText*)L"estdkeyf9",						\
        (TText*)L"estdkeyf10",						\
        (TText*)L"estdkeyf11",						\
        (TText*)L"estdkeyf12",						\
        (TText*)L"estdkeyf13",						\
        (TText*)L"estdkeyf14",						\
        (TText*)L"estdkeyf15",						\
        (TText*)L"estdkeyf16",						\
        (TText*)L"estdkeyf17",						\
        (TText*)L"estdkeyf18",						\
        (TText*)L"estdkeyf19",						\
        (TText*)L"estdkeyf20",						\
        (TText*)L"estdkeyf21",						\
        (TText*)L"estdkeyf22",						\
        (TText*)L"estdkeyf23",						\
        (TText*)L"estdkeyf24",						\
        (TText*)L"estdkeyxxx",						\
        (TText*)L"estdkeycomma",					\
        (TText*)L"estdkeyfullstop",					\
        (TText*)L"estdkeyforwardslash",				\
        (TText*)L"estdkeybackslash",				\
        (TText*)L"estdkeysemicolon",				\
        (TText*)L"estdkeysinglequote",				\
        (TText*)L"estdkeyhash",						\
        (TText*)L"estdkeysquarebracketleft",		\
        (TText*)L"estdkeysquarebracketright",		\
        (TText*)L"estdkeyminus",					\
        (TText*)L"estdkeyequals",					\
        (TText*)L"estdkeynkpforwardslash",			\
        (TText*)L"estdkeynkpasterisk",				\
        (TText*)L"estdkeynkpminus",					\
        (TText*)L"estdkeynkpplus",					\
        (TText*)L"estdkeynkpenter",					\
        (TText*)L"estdkeynkp1",						\
        (TText*)L"estdkeynkp2",						\
        (TText*)L"estdkeynkp3",						\
        (TText*)L"estdkeynkp4",						\
        (TText*)L"estdkeynkp5",						\
        (TText*)L"estdkeynkp6",						\
        (TText*)L"estdkeynkp7",						\
        (TText*)L"estdkeynkp8",						\
        (TText*)L"estdkeynkp9",						\
        (TText*)L"estdkeynkp0",						\
        (TText*)L"estdkeynkpfullstop",				\
        (TText*)L"estdkeymenu",						\
        (TText*)L"estdkeybacklighton",				\
        (TText*)L"estdkeybacklightoff",				\
        (TText*)L"estdkeybacklighttoggle",			\
        (TText*)L"estdkeyinccontrast",				\
        (TText*)L"estdkeydeccontrast",				\
        (TText*)L"estdkeysliderdown",				\
        (TText*)L"estdkeysliderup",					\
        (TText*)L"estdkeydictaphoneplay",			\
        (TText*)L"estdkeydictaphonestop",			\
        (TText*)L"estdkeydictaphonerecord",			\
        (TText*)L"estdkeyhelp",						\
        (TText*)L"estdkeyoff",						\
        (TText*)L"estdkeydial",						\
        (TText*)L"estdkeyincvolume",				\
        (TText*)L"estdkeydecvolume",				\
        (TText*)L"estdkeydevice0",					\
        (TText*)L"estdkeydevice1",					\
        (TText*)L"estdkeydevice2",					\
        (TText*)L"estdkeydevice3",					\
        (TText*)L"estdkeydevice4",					\
        (TText*)L"estdkeydevice5",					\
        (TText*)L"estdkeydevice6",					\
        (TText*)L"estdkeydevice7",					\
        (TText*)L"estdkeydevice8",					\
        (TText*)L"estdkeydevice9",					\
        (TText*)L"estdkeydevicea",					\
        (TText*)L"estdkeydeviceb",					\
        (TText*)L"estdkeydevicec",					\
        (TText*)L"estdkeydeviced",					\
        (TText*)L"estdkeydevicee",					\
        (TText*)L"estdkeydevicef",					\
        (TText*)L"estdkeyapplication0",				\
        (TText*)L"estdkeyapplication1",				\
        (TText*)L"estdkeyapplication2",				\
        (TText*)L"estdkeyapplication3",				\
        (TText*)L"estdkeyapplication4",				\
        (TText*)L"estdkeyapplication5",				\
        (TText*)L"estdkeyapplication6",				\
        (TText*)L"estdkeyapplication7",				\
        (TText*)L"estdkeyapplication8",				\
        (TText*)L"estdkeyapplication9",				\
        (TText*)L"estdkeyapplicationa",				\
        (TText*)L"estdkeyapplicationb",				\
        (TText*)L"estdkeyapplicationc",				\
        (TText*)L"estdkeyapplicationd",				\
        (TText*)L"estdkeyapplicatione",				\
        (TText*)L"estdkeyapplicationf",				\
        (TText*)L"estdkeyyes",						\
        (TText*)L"estdkeyno",						\
        (TText*)L"estdkeyincbrightness",			\
        (TText*)L"estdkeydecbrightness",			\
        (TText*)L"estdkeykeyboardextend",			\
        (TText*)L"estdkeydevice10",					\
        (TText*)L"estdkeydevice11",					\
        (TText*)L"estdkeydevice12",					\
        (TText*)L"estdkeydevice13",					\
        (TText*)L"estdkeydevice14",					\
        (TText*)L"estdkeydevice15",					\
        (TText*)L"estdkeydevice16",					\
        (TText*)L"estdkeydevice17",					\
        (TText*)L"estdkeydevice18",					\
        (TText*)L"estdkeydevice19",					\
        (TText*)L"estdkeydevice1a",					\
        (TText*)L"estdkeydevice1b",					\
        (TText*)L"estdkeydevice1c",					\
        (TText*)L"estdkeydevice1d",					\
        (TText*)L"estdkeydevice1e",					\
        (TText*)L"estdkeydevice1f",					\
        (TText*)L"estdkeyapplication10",			\
        (TText*)L"estdkeyapplication11",			\
        (TText*)L"estdkeyapplication12",			\
        (TText*)L"estdkeyapplication13",			\
        (TText*)L"estdkeyapplication14",			\
        (TText*)L"estdkeyapplication15",			\
        (TText*)L"estdkeyapplication16",			\
        (TText*)L"estdkeyapplication17",			\
        (TText*)L"estdkeyapplication18",			\
        (TText*)L"estdkeyapplication19",			\
        (TText*)L"estdkeyapplication1a",			\
        (TText*)L"estdkeyapplication1b",			\
        (TText*)L"estdkeyapplication1c",			\
        (TText*)L"estdkeyapplication1d",			\
        (TText*)L"estdkeyapplication1e",			\
        (TText*)L"estdkeyapplication1f",			\
        }

/**
 * Key scan codes definition macro.
 */
#define DefineKeyScanCodes( varName )		\
	static TUint const varName[] = 			\
		{									\
        (TUint)EStdKeyNull,					\
        (TUint)EStdKeyBackspace,			\
        (TUint)EStdKeyTab,					\
        (TUint)EStdKeyEnter,				\
        (TUint)EStdKeyEscape,				\
        (TUint)EStdKeySpace,				\
        (TUint)EStdKeyPrintScreen,			\
        (TUint)EStdKeyPause,				\
        (TUint)EStdKeyHome,					\
        (TUint)EStdKeyEnd,					\
        (TUint)EStdKeyPageUp,				\
        (TUint)EStdKeyPageDown,				\
        (TUint)EStdKeyInsert,				\
        (TUint)EStdKeyDelete,				\
        (TUint)EStdKeyLeftArrow,			\
        (TUint)EStdKeyRightArrow,			\
        (TUint)EStdKeyUpArrow,				\
        (TUint)EStdKeyDownArrow,			\
        (TUint)EStdKeyLeftShift,			\
        (TUint)EStdKeyRightShift,			\
        (TUint)EStdKeyLeftAlt,				\
        (TUint)EStdKeyRightAlt,				\
        (TUint)EStdKeyLeftCtrl,				\
        (TUint)EStdKeyRightCtrl,			\
        (TUint)EStdKeyLeftFunc,				\
        (TUint)EStdKeyRightFunc,			\
        (TUint)EStdKeyCapsLock,				\
        (TUint)EStdKeyNumLock,				\
        (TUint)EStdKeyScrollLock,			\
        (TUint)EStdKeyF1,					\
        (TUint)EStdKeyF2,					\
        (TUint)EStdKeyF3,					\
        (TUint)EStdKeyF4,					\
        (TUint)EStdKeyF5,					\
        (TUint)EStdKeyF6,					\
        (TUint)EStdKeyF7,					\
        (TUint)EStdKeyF8,					\
        (TUint)EStdKeyF9,					\
        (TUint)EStdKeyF10,					\
        (TUint)EStdKeyF11,					\
        (TUint)EStdKeyF12,					\
        (TUint)EStdKeyF13,					\
        (TUint)EStdKeyF14,					\
        (TUint)EStdKeyF15,					\
        (TUint)EStdKeyF16,					\
        (TUint)EStdKeyF17,					\
        (TUint)EStdKeyF18,					\
        (TUint)EStdKeyF19,					\
        (TUint)EStdKeyF20,					\
        (TUint)EStdKeyF21,					\
        (TUint)EStdKeyF22,					\
        (TUint)EStdKeyF23,					\
        (TUint)EStdKeyF24,					\
        (TUint)EStdKeyXXX,					\
        (TUint)EStdKeyComma,				\
        (TUint)EStdKeyFullStop,				\
        (TUint)EStdKeyForwardSlash,			\
        (TUint)EStdKeyBackSlash,			\
        (TUint)EStdKeySemiColon,			\
        (TUint)EStdKeySingleQuote,			\
        (TUint)EStdKeyHash,					\
        (TUint)EStdKeySquareBracketLeft,	\
        (TUint)EStdKeySquareBracketRight,	\
        (TUint)EStdKeyMinus,				\
        (TUint)EStdKeyEquals,				\
        (TUint)EStdKeyNkpForwardSlash,		\
        (TUint)EStdKeyNkpAsterisk,			\
        (TUint)EStdKeyNkpMinus,				\
        (TUint)EStdKeyNkpPlus,				\
        (TUint)EStdKeyNkpEnter,				\
        (TUint)EStdKeyNkp1,					\
        (TUint)EStdKeyNkp2,					\
        (TUint)EStdKeyNkp3,					\
        (TUint)EStdKeyNkp4,					\
        (TUint)EStdKeyNkp5,					\
        (TUint)EStdKeyNkp6,					\
        (TUint)EStdKeyNkp7,					\
        (TUint)EStdKeyNkp8,					\
        (TUint)EStdKeyNkp9,					\
        (TUint)EStdKeyNkp0,					\
        (TUint)EStdKeyNkpFullStop,			\
        (TUint)EStdKeyMenu,					\
        (TUint)EStdKeyBacklightOn,			\
        (TUint)EStdKeyBacklightOff,			\
        (TUint)EStdKeyBacklightToggle,		\
        (TUint)EStdKeyIncContrast,			\
        (TUint)EStdKeyDecContrast,			\
        (TUint)EStdKeySliderDown,			\
        (TUint)EStdKeySliderUp,				\
        (TUint)EStdKeyDictaphonePlay,		\
        (TUint)EStdKeyDictaphoneStop,		\
        (TUint)EStdKeyDictaphoneRecord,		\
        (TUint)EStdKeyHelp,					\
        (TUint)EStdKeyOff,					\
        (TUint)EStdKeyDial,					\
        (TUint)EStdKeyIncVolume,			\
        (TUint)EStdKeyDecVolume,			\
        (TUint)EStdKeyDevice0,				\
        (TUint)EStdKeyDevice1,				\
        (TUint)EStdKeyDevice2,				\
        (TUint)EStdKeyDevice3,				\
        (TUint)EStdKeyDevice4,				\
        (TUint)EStdKeyDevice5,				\
        (TUint)EStdKeyDevice6,				\
        (TUint)EStdKeyDevice7,				\
        (TUint)EStdKeyDevice8,				\
        (TUint)EStdKeyDevice9,				\
        (TUint)EStdKeyDeviceA,				\
        (TUint)EStdKeyDeviceB,				\
        (TUint)EStdKeyDeviceC,				\
        (TUint)EStdKeyDeviceD,				\
        (TUint)EStdKeyDeviceE,				\
        (TUint)EStdKeyDeviceF,				\
        (TUint)EStdKeyApplication0,			\
        (TUint)EStdKeyApplication1,			\
        (TUint)EStdKeyApplication2,			\
        (TUint)EStdKeyApplication3,			\
        (TUint)EStdKeyApplication4,			\
        (TUint)EStdKeyApplication5,			\
        (TUint)EStdKeyApplication6,			\
        (TUint)EStdKeyApplication7,			\
        (TUint)EStdKeyApplication8,			\
        (TUint)EStdKeyApplication9,			\
        (TUint)EStdKeyApplicationA,			\
        (TUint)EStdKeyApplicationB,			\
        (TUint)EStdKeyApplicationC,			\
        (TUint)EStdKeyApplicationD,			\
        (TUint)EStdKeyApplicationE,			\
        (TUint)EStdKeyApplicationF,			\
        (TUint)EStdKeyYes,					\
        (TUint)EStdKeyNo,					\
        (TUint)EStdKeyIncBrightness,		\
        (TUint)EStdKeyDecBrightness,		\
        (TUint)EStdKeyKeyboardExtend,		\
        (TUint)EStdKeyDevice10,				\
        (TUint)EStdKeyDevice11,				\
        (TUint)EStdKeyDevice12,				\
        (TUint)EStdKeyDevice13,				\
        (TUint)EStdKeyDevice14,				\
        (TUint)EStdKeyDevice15,				\
        (TUint)EStdKeyDevice16,				\
        (TUint)EStdKeyDevice17,				\
        (TUint)EStdKeyDevice18,				\
        (TUint)EStdKeyDevice19,				\
        (TUint)EStdKeyDevice1A,				\
        (TUint)EStdKeyDevice1B,				\
        (TUint)EStdKeyDevice1C,				\
        (TUint)EStdKeyDevice1D,				\
        (TUint)EStdKeyDevice1E,				\
        (TUint)EStdKeyDevice1F,				\
        (TUint)EStdKeyApplication10,		\
        (TUint)EStdKeyApplication11,		\
        (TUint)EStdKeyApplication12,		\
        (TUint)EStdKeyApplication13,		\
        (TUint)EStdKeyApplication14,		\
        (TUint)EStdKeyApplication15,		\
        (TUint)EStdKeyApplication16,		\
        (TUint)EStdKeyApplication17,		\
        (TUint)EStdKeyApplication18,		\
        (TUint)EStdKeyApplication19,		\
        (TUint)EStdKeyApplication1A,		\
        (TUint)EStdKeyApplication1B,		\
        (TUint)EStdKeyApplication1C,		\
        (TUint)EStdKeyApplication1D,		\
        (TUint)EStdKeyApplication1E,		\
        (TUint)EStdKeyApplication1F,		\
        }

/**
 * Key modifier names definition macro.
 */
#define DefineModifiers( varName )			\
	static TUint const varName[] = 			\
		{									\
        (TUint)EModifierAutorepeatable,		\
        (TUint)EModifierKeypad,				\
        (TUint)EModifierLeftAlt,			\
        (TUint)EModifierRightAlt,			\
        (TUint)EModifierAlt,				\
        (TUint)EModifierLeftCtrl,			\
        (TUint)EModifierRightCtrl,			\
        (TUint)EModifierCtrl,				\
        (TUint)EModifierLeftShift,			\
        (TUint)EModifierRightShift,			\
        (TUint)EModifierShift,				\
        (TUint)EModifierLeftFunc,			\
        (TUint)EModifierRightFunc,			\
        (TUint)EModifierFunc,				\
        (TUint)EModifierCapsLock,			\
        (TUint)EModifierNumLock,			\
        (TUint)EModifierScrollLock,			\
        (TUint)EModifierKeyUp,				\
        (TUint)EModifierSpecial,			\
        (TUint)EModifierDoubleClick,		\
        (TUint)EModifierPureKeycode,		\
        (TUint)EModifierKeyboardExtend,		\
        (TUint)EModifierCancelRotation,		\
        (TUint)EModifierRotateBy90,			\
        (TUint)EModifierRotateBy180,		\
        (TUint)EModifierRotateBy270,		\
        (TUint)EModifierPointer3DButton1,	\
        (TUint)EModifierPointer3DButton2,	\
        (TUint)EModifierPointer3DButton3,	\
        (TUint)EAllModifiers,				\
    }

/**
 * Key modifiers definition macro.
 */
#define DefineModifierNames( varName ) 				\
	static TText* const varName[] =					\
        {											\
        (TText*)L"estdkeynull",						\
        (TText*)L"emodifierautorepeatable",			\
        (TText*)L"emodifierkeypad",					\
        (TText*)L"emodifierleftalt",				\
        (TText*)L"emodifierrightalt",				\
        (TText*)L"emodifieralt",					\
        (TText*)L"emodifierleftctrl",				\
        (TText*)L"emodifierrightctrl",				\
        (TText*)L"emodifierctrl",					\
        (TText*)L"emodifierleftshift",				\
        (TText*)L"emodifierrightshift",				\
        (TText*)L"emodifiershift",					\
        (TText*)L"emodifierleftfunc",				\
        (TText*)L"emodifierrightfunc",				\
        (TText*)L"emodifierfunc",					\
        (TText*)L"emodifiercapslock",				\
        (TText*)L"emodifiernumlock",				\
        (TText*)L"emodifierscrolllock",				\
        (TText*)L"emodifierkeyup",					\
        (TText*)L"emodifierspecial",				\
        (TText*)L"emodifierdoubleclick",			\
        (TText*)L"emodifierpurekeycode",			\
        (TText*)L"emodifierkeyboardextend",			\
        (TText*)L"emodifiercancelrotation",			\
        (TText*)L"emodifierrotateby90",				\
        (TText*)L"emodifierrotateby180",			\
        (TText*)L"emodifierrotateby270",			\
        (TText*)L"emodifierpointer3dbutton1",		\
        (TText*)L"emodifierpointer3dbutton2",		\
        (TText*)L"emodifierpointer3dbutton3",		\
        (TText*)L"eallmodifiers",					\
        }
        

/**
 * Pointer event type names definition macro
 */
#define DefinePointerEventTypeNames( varName ) 			\
	static TText* const varName[] =						\
        {												\
        (TText*)L"epointermove",						\
        (TText*)L"epointerswitchon",							\
        (TText*)L"ebutton1down",						\
        (TText*)L"ebutton1up",							\
        (TText*)L"ebutton2down",						\
        (TText*)L"ebutton2up",							\
        (TText*)L"ebutton3down",						\
        (TText*)L"ebutton3up",							\
        (TText*)L"ebutton1",							\
        (TText*)L"ebutton2",							\
        (TText*)L"ebutton3",							\
        }

/**
 * Pointer event type codes definition macro.
 */
#define DefinePointerEventTypes( varName )					\
	static TInt const varName[] = 							\
		{													\
        (TUint)TRawEvent::EPointerMove,						\
        (TUint)TRawEvent::EPointerSwitchOn,					\
        (TUint)TRawEvent::EButton1Down,						\
        (TUint)TRawEvent::EButton1Up,						\
        (TUint)TRawEvent::EButton2Down,						\
        (TUint)TRawEvent::EButton2Up,						\
        (TUint)TRawEvent::EButton3Down,						\
        (TUint)TRawEvent::EButton3Up,						\
        (TUint)TEventUtil::EButton1,						\
        (TUint)TEventUtil::EButton2,						\
        (TUint)TEventUtil::EButton3,						\
		}

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: TEventUtil

     Method: GetKeyCodeName

     Description: Gets key name.

     Returns selected key name. 

     Parameters: TUint aKeyCode: out: Key code.
     
     Return Values: Key name.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TPtrC TEventUtil::GetKeyCodeName( TUint aKeyCode )
	{
	DefineKeyCodeNames( keyCodeNames );
	DefineKeyCodes( keyCodes );
	int keyCodeNamesCount = (sizeof( keyCodeNames )/sizeof(TText*));
	
	for ( int i = 0; i < keyCodeNamesCount; i++ ) {
		if ( keyCodes[ i ] == aKeyCode ) 
			{
			return keyCodeNames[ i ];
			}
	}

	return NULL;
	}

/*
-------------------------------------------------------------------------------

     Class: TEventUtil

     Method: GetKeyScanCodeName

     Description: Gets key scan code name.

     Returns selected key scan code name. 

     Parameters: TUint aKeyCode: out: Key scan code.
     
     Return Values: Key scan code name.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TPtrC TEventUtil::GetKeyScanCodeName( TInt aKeyScanCode )
	{	
	DefineKeyScanCodeNames( keyScanCodeNames );
	DefineKeyScanCodes( keyScanCodes );
	int keyScanCodeNamesCount = (sizeof( keyScanCodeNames )/sizeof(TText*));
	
	for ( int i = 0; i < keyScanCodeNamesCount; i++ ) {
		if ( keyScanCodes[ i ] == (TUint)aKeyScanCode ) 
			{
			return keyScanCodeNames[ i ];
			}
	}
	
	return NULL;
	}

/*
-------------------------------------------------------------------------------

     Class: TEventUtil

     Method: GetModifierName

     Description: Gets key modifier name.

     Returns selected key modifier name. 

     Parameters: TUint aModifier: out: Key modifier.
     
     Return Values: Key modifier name.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TPtrC TEventUtil::GetModifierName( TUint aModifier )
	{
	DefineModifierNames( modifierNames );
	DefineModifiers( modifiers );
	int modifierNamesCount = (sizeof( modifierNames )/sizeof(TText*));
	
	for ( int i = 0; i < modifierNamesCount; i++ ) {
		if ( modifiers[ i ] == aModifier ) 
			{
			return modifierNames[ i ];
			}
	}
	
	return NULL;	
	}

/*
-------------------------------------------------------------------------------

     Class: TEventUtil

     Method: ParseKeyCode

     Description: Parses key code.

     Parses key code. 

     Parameters: TDesC& aKeyCodeName: in: Key code name.
     			 TUint& aKeyCode: out: Parsed key code.
     
     Return Values: KErrNone if key code was parsed successfuly, 
     				KErrNotFound in other case.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt TEventUtil::ParseKeyCode( const TDesC& aKeyCodeName, TUint& aKeyCode )
	{
	DefineKeyCodeNames( keyCodeNames );
	DefineKeyCodes( keyCodes );
	int keyCodeNamesCount = (sizeof( keyCodeNames )/sizeof(TText*));
	
	TBuf<64> keyCodeNameLowerCase( aKeyCodeName );
	keyCodeNameLowerCase.LowerCase();
	for ( int i = 0; i < keyCodeNamesCount; i++ ) {
		if ( TPtrC( keyCodeNames[ i ] ).Compare( keyCodeNameLowerCase ) == 0 ) 
			{
			aKeyCode = keyCodes[ i ];
			return KErrNone;
			}
	}
	
	return KErrNotFound;
	}

/*
-------------------------------------------------------------------------------

     Class: TEventUtil

     Method: ParseKeyScanCode

     Description: Parses key scan code.

     Parses key scan code. 

     Parameters: TDesC& aKeyScanCodeName: in: Key scan code name.
     			 TUint& aKeyScanCode: out: Parsed key scan code.
     
     Return Values: KErrNone if key scan code was parsed successfuly, 
     				KErrNotFound in other case.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt TEventUtil::ParseKeyScanCode( const TDesC& aKeyScanCodeName, TInt& aKeyScanCode )
	{
	DefineKeyScanCodeNames( keyScanCodeNames );
	DefineKeyScanCodes( keyScanCodes );
	int keyScanCodeNamesCount = (sizeof( keyScanCodeNames )/sizeof(TText*));
	
	TBuf<64> keyScanCodeNameLowerCase( aKeyScanCodeName );
	keyScanCodeNameLowerCase.LowerCase();
	for ( int i = 0; i < keyScanCodeNamesCount; i++ ) {
		if ( TPtrC( keyScanCodeNames[ i ] ).Compare( keyScanCodeNameLowerCase ) == 0 )
			{
			aKeyScanCode = keyScanCodes[ i ];
			return KErrNone;
			}
	}
	
	return KErrNotFound;
	}

/*
-------------------------------------------------------------------------------

     Class: TEventUtil

     Method: ParseModifier

     Description: Parses key modifier.

     Parses key modifier. 

     Parameters: TDesC& aModifierName: in: Key modifier name.
     			 TUint& aModifier: out: Parsed key modifier.
     
     Return Values: KErrNone if key modifier was parsed successfuly, 
     				KErrNotFound in other case.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt TEventUtil::ParseModifier( const TDesC& aModifierName, TUint& aModifier )
	{
	DefineModifierNames( modifierNames );
	DefineModifiers( modifiers );
	int modifierNamesCount = (sizeof( modifierNames )/sizeof(TText*));
	
	TBuf<64> modifierNameLowerCase( aModifierName );
	modifierNameLowerCase.LowerCase();
	for ( int i = 0; i < modifierNamesCount; i++ ) {
		if ( TPtrC( modifierNames[ i ] ).Compare( modifierNameLowerCase ) == 0 )
			{
			aModifier = modifiers[ i ];
			return KErrNone;
			}
	}
	
	return KErrNotFound;	
	}

/*
-------------------------------------------------------------------------------

     Class: CAknUiEnvProxy

     Method: ParsePointerEventType

     Description: Parses pointer event type.

     Parses key modifier.

     Parameters: const TDesC& aPointerEventTypeName: in: Pointer event type.
     			 TUint& aModifier: out: Parsed pointer event type.
     
     Return Values: KErrNone if no error occures during parsing.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt TEventUtil::ParsePointerEventType( const TDesC& aPointerEventTypeName, TUint& aPointerEventType )
	{
	DefinePointerEventTypeNames( pointerEventTypeNames );
	DefinePointerEventTypes( pointerEventTypes );
	int pointerEventTypeNamesCount = (sizeof( pointerEventTypeNames )/sizeof(TText*));
	
	TBuf<64> pointerEventTypeNamesLowerCase( aPointerEventTypeName );
	pointerEventTypeNamesLowerCase.LowerCase();
	for ( int i = 0; i < pointerEventTypeNamesCount; i++ ) {
		if ( TPtrC( pointerEventTypeNames[ i ] ).Compare( pointerEventTypeNamesLowerCase ) == 0 )
			{
			aPointerEventType = pointerEventTypes[ i ];
			return KErrNone;
			}
	}
	
	return KErrNotFound;	
	}

// End of File
