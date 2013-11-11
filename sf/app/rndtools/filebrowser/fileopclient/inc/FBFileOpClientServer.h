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
* Description:  
*
*/


#ifndef __FBCLIENTSERVER_H__
#define __FBCLIENTSERVER_H__

#include <e32std.h>

const TUint KCopyServMajorVersionNumber = 1;
const TUint KCopyServMinorVersionNumber = 0;
const TUint KCopyServBuildVersionNumber = 0;

_LIT(KMyServerName, "FileBrowserFileOpServer");
_LIT(KMyServerImg, "FileBrowserFileOpServer");		// EXE name
const TUid KServerUid3 = {0x102828D8};

enum TFileOpMessages
	{
	EFileOpCopy,
	EFileOpRename,
	EFileOpAttribs,
	EFileOpRmDir,
	EFileOpDelete,
	EFileOpMkDirAll,
	EFileOpCreateEmptyFile,
	EFileOpEraseMBR,
	EFileOpPartitionDrive,
	EFileOpCancel
	};

class TFileOpArgs
    {
public:
    TFileName   iBuf1;
    TFileName   iBuf2;
    TUint       iUint1;
    TUint       iUint2;
    TUint       iUint3;
    TTime       iTime1;
    };

#endif