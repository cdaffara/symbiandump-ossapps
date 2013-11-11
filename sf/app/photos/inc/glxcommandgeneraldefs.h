/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Definitions for command type
*
*/




#ifndef __T_GLXCOMMANDGENERALDEFS_H__
#define __T_GLXCOMMANDGENERALDEFS_H__

#include <e32base.h>

// Unique command ids
const TUint32 KGlxCommandIdAdd = 0x200071A0;
const TUint32 KGlxCommandIdRemove = 0x200071A1;
const TUint32 KGlxCommandIdSet = 0x200071A2;
const TUint32 KGlxCommandThumbnailCleanup = 0x200071A3;
/// @todo Should these UIDs be properly described in \doc\allocated_uid_list.txt?

const TUint32 KGlxContainerDevice = 0;

#endif // __T_GLXCOMMANDGENERALDEFS_H__
