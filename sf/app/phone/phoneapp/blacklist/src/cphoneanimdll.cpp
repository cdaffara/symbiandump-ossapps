/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  It provides implementation for the CPhoneAnimDll class. Only
*                one animation service is provided, CPhoneBlacklistAnim, to
*                clear blacklist upon next keypress.
*
*/



// INCLUDE FILES
#include    "phoneanimdllstd.h"         
#include    "cphoneanimdll.h"
#include    "cphoneblacklistanim.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPhoneAnimDll::CreateInstanceL
// 
// Create blacklist clearing plug-in. Only one type provided.
// ---------------------------------------------------------
//
CAnim* CPhoneAnimDll::CreateInstanceL( TInt aType )
    {
    switch ( aType )
        {
        case EAnimClearBlackList:
            return new ( ELeave ) CPhoneBlacklistAnim();
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CAnimDll::CreateCAnimDllL
// 
// Create plug-in. Factory function.
// ---------------------------------------------------------
//
EXPORT_C CAnimDll* CreateCAnimDllL()
    {
    return new ( ELeave ) CPhoneAnimDll();
    }

// End of File
