/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This header file contains constants definitions and type
*                definition
*
*/


#ifndef MPXDBCOMMONSTD_H
#define MPXDBCOMMONSTD_H

//  INCLUDES
#include <e32std.h>
#include <f32file.h>

// MACROS
#define SET_BIT(x,y) (x |= y)
#define GET_BIT(x,y) (x & y)
#define RESET_BIT(x,y) (x &= ~y)

#define MPX_ITEM_CATEGORY(x) static_cast<TMPXGeneralCategory>((x >> 28) & KMCCategoryMask)

// CONSTANTS
const TInt KMCIntegerLen          = 11; //-2147483648
const TInt KMCInt64Len            = 20; //-9223372036854775808

const TInt KMCPathStartWithColon  = 1;  // Look for ":\\"
const TInt KMCPathStartPos        = 2;  // e.g. "C:\\Path\\Name.Ext"
const TInt KMCCategoryMask        = 0x0F;

// Maximum number of deleted records kept in DB
const TInt KMCMaxSavedDeletedRecords = 2000;

// DRM types used
enum TMCDrmType
    {
    EMCDrmNone,
    EMCDrmOmaDrm,
    EMCDrmWmdrm
    };

// Reasons for collection server Panic
enum TMCDbPanic
    {
    };

// FUNCTION PROTOTYPES
GLREF_C void MCDbPanic(TMCDbPanic aPanic);

#endif // MPXDBCOMMONSTD_H

// End Of File
