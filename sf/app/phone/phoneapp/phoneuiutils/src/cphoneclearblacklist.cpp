/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Blacklist clearing.
*
*/


// INCLUDE FILES
#include "cphoneclearblacklist.h"
#include "phoneconstants.h"
#include "phonelogger.h"
#include "phoneui.pan"
#include "rphoneanimclearblacklist.h"
#include <data_caging_path_literals.hrh>

// CONSTANTS
#ifdef __WINS__
_LIT( KPhoneAnimDllFileName, "phoneanimdll.dll" );
#else
_LIT( KPhoneAnimDllFileName, "z:phoneanimdll.dll" );
#endif // __WINS__


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CPhoneClearBlacklist::CreateL
// ---------------------------------------------------------
//
EXPORT_C CPhoneClearBlacklist* CPhoneClearBlacklist::CreateL( RWsSession& aWsSession, 
     const RWindowBase& aDevice )
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CPhoneClearBlacklist::CreateL( ) ");
    CPhoneClearBlacklist* instance = static_cast<CPhoneClearBlacklist*>(
        CCoeEnv::Static( KUidClearBlackListSingleton ) );
    if ( !instance )
        {
        instance = new (ELeave) CPhoneClearBlacklist();
        CleanupStack::PushL( instance );
        instance->ConstructL( aWsSession, aDevice );
        CleanupStack::Pop( instance );
        }
    return instance;
    }

// ---------------------------------------------------------
// CPhoneClearBlacklist::Instance
// ---------------------------------------------------------
//
EXPORT_C CPhoneClearBlacklist* CPhoneClearBlacklist::Instance()
    {
    CPhoneClearBlacklist* instance = static_cast<CPhoneClearBlacklist*>(
        CCoeEnv::Static( KUidClearBlackListSingleton ) );
    __ASSERT_DEBUG( instance, Panic( EPhoneCtrlSingletonNotInitialized ) );
    return instance;
    }

// -----------------------------------------------------------------------------
//  CPhoneClearBlacklist::CPhoneClearBlacklist
// -----------------------------------------------------------------------------
//
CPhoneClearBlacklist::CPhoneClearBlacklist() : CCoeStatic( KUidClearBlackListSingleton )
    {
    }

// -----------------------------------------------------------------------------
//  CPhoneClearBlacklist::~CPhoneClearBlacklist
// -----------------------------------------------------------------------------
//
EXPORT_C CPhoneClearBlacklist::~CPhoneClearBlacklist()
    {
    if ( iAnim )
        {
        iAnim->Close();
        delete iAnim;
        }

    if ( iAnimDll )
        {
        iAnimDll->Close();
        delete iAnimDll;
        }   
    }

// -----------------------------------------------------------------------------
//  CPhoneClearBlacklist::ConstructL
// -----------------------------------------------------------------------------
//
void CPhoneClearBlacklist::ConstructL( RWsSession& aWsSession, 
     const RWindowBase& aDevice )
    {
    // Construct RAnim connection
    iAnimDll = new (ELeave) RAnimDll( aWsSession );

    TParse* fp = new (ELeave) TParse();
#ifdef __WINS__
    fp->Set( KPhoneAnimDllFileName, NULL, NULL ); 
#else
    fp->Set( KPhoneAnimDllFileName, &KDC_SHARED_LIB_DIR, NULL );     
#endif //__WINS__
    TFileName fileName( fp->FullName() );
    delete fp;

    User::LeaveIfError( iAnimDll->Load( fileName ) );

    iAnim = new (ELeave) RPhoneAnimClearBlacklist( *iAnimDll );
    iAnim->ConstructL( aDevice );
    }

// -----------------------------------------------------------------------------
//  CPhoneClearBlacklist::ClearBlackList
//
//  Clears blacklist.
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneClearBlacklist::ClearBlackListNow()
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CPhoneClearBlacklist::ClearBlackListNow( ) ");
    iAnim->ClearBlackListNow();
    }

// -----------------------------------------------------------------------------
//  CPhoneClearBlacklist::ClearBlackListOnNextKey
//
//  Clears blacklist.
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneClearBlacklist::ClearBlackListOnNextKey()
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CPhoneClearBlacklist::ClearBlackListOnNextKey( ) ");
    iAnim->ClearBlackListOnNextKey();
    }

//  End of File  
