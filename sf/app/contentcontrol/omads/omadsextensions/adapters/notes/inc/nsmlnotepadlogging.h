/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  calss which directly deal with notepad database.
*
*/


#ifndef __NSMLNOTEPADLOGGING_H__
#define __NSMLNOTEPADLOGGING_H__

#define _NOTEPAD_DBG_FILE(p)

#ifdef _DEBUG_
#include "nsmldebug.h"

#define _NOTEPAD_DBG_FILE(p) _DBG_FILE(p)

#endif


#endif //__NSMLNOTEPADLOGGING_H__
