/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Client side for blacklist clearing plugin.
*
*/


// INCLUDE FILES
#include "rphoneanimclearblacklist.h"   // This
#include <coecntrl.h>                   // CCoeControl
#include <phoneanimdllstd.h>            // Command

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RPhoneAnimClearBlacklist::RPhoneAnimClearBlacklist
// ---------------------------------------------------------
//
RPhoneAnimClearBlacklist::RPhoneAnimClearBlacklist( RAnimDll& aAnimDll )
    :RAnim( aAnimDll )
    {
    }

// ---------------------------------------------------------
// RPhoneAnimClearBlacklist::ConstructL
// ---------------------------------------------------------
//
void RPhoneAnimClearBlacklist::ConstructL( const RWindowBase& aDevice )
    {
    // Creates a server side anim class
    RAnim::Construct( aDevice, EAnimClearBlackList, TPtrC8() );
    }

// ---------------------------------------------------------
// RPhoneAnimClearBlacklist::ClearBlackListOnNextKey()
//
//  Tells to plugin to clear list on next key event.
// ---------------------------------------------------------
//
void RPhoneAnimClearBlacklist::ClearBlackListOnNextKey()
    {
    Command( ECancelBlackList );
    }

// ---------------------------------------------------------
// RPhoneAnimClearBlacklist::ClearBlackList()
//
//  Tells to plugin to clear list.
// ---------------------------------------------------------
//
void RPhoneAnimClearBlacklist::ClearBlackListNow()
    {
    Command( EClearBlackListNow );
    }
//  End of File  
