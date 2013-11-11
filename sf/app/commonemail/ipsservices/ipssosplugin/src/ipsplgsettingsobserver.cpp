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
* Description: This file implements class CIpsPlgSettingsObserver.
*
*
*
*/


// INCLUDE FILES

#include "emailtrace.h"
#include "ipsplgheaders.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgSettingsObserver::CIpsPlgSettingsObserver( 
    TFSMailMsgId aAccount,
    CRepository& aCenRep,
    MIpsPlgSettingsObserverCallback& aIPSCallback )
    : 
    CActive( CActive::EPriorityStandard ),
    iCenRep( aCenRep ),
    iAccount( aAccount ),
    iFSObservers( 2 ),
    iIPSCallback( aIPSCallback )
    {
    FUNC_LOG;

    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgSettingsObserver* CIpsPlgSettingsObserver::NewL( 
    TFSMailMsgId aAccount,
    CRepository& aCenRep,
    MIpsPlgSettingsObserverCallback& aIPSCallback )
    {
    FUNC_LOG;
    CIpsPlgSettingsObserver* self = 
        new(ELeave) CIpsPlgSettingsObserver( aAccount, aCenRep, aIPSCallback );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSettingsObserver::ConstructL( )
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    }

    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgSettingsObserver::~CIpsPlgSettingsObserver()
    {
    FUNC_LOG;
    Cancel();
    iFSObservers.Close();
    } 

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSettingsObserver::DoCancel()
    {
    FUNC_LOG;
    iCenRep.NotifyCancel( iKey );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSettingsObserver::RunL()
    {
    FUNC_LOG;
    if ( iStatus.Int() == KErrNone )
        {
        for ( TInt i = 0; i < iFSObservers.Count(); i++ )
            {
            iFSObservers[i]->EventL( 
                TFSEventMailboxSettingsChanged, 
                iAccount,
                NULL,
                NULL,
                NULL );            
            iIPSCallback.SettingsChangedL( iAccount );
            }
        Activate();//we don't set a new key here
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TInt CIpsPlgSettingsObserver::RunError( TInt /* aError */ )
    {
    FUNC_LOG;
    // Just ignore any error and continue without
    // any handling to allow smooth execution. 
    return KErrNone;            
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSettingsObserver::SetKeyAndActivateL( 
    TUint32 aKey, 
    const RPointerArray<MFSMailEventObserver>& aObservers )
    {
    FUNC_LOG;
    if ( IsActive() && aKey != iKey )
        {
        Cancel();
        iKey = aKey;
        Activate();
        }
    else if ( !IsActive() )
        {
        iKey = aKey;
        Activate();
        }
    iFSObservers.Reset();
    for ( TInt i = 0; i < aObservers.Count(); i++ )
        {
        iFSObservers.AppendL(aObservers[i]);
        }
    }
// ----------------------------------------------------------------------------
// ---------------------------------------------------------------------------- 
void CIpsPlgSettingsObserver::Activate()
    {
    FUNC_LOG;
    TInt error = iCenRep.NotifyRequest( iKey, iStatus );
    if ( error == KErrNone )
        {
        SetActive();
        }
    }
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgSettingsObserver::UnActivate()
    {
    FUNC_LOG;
    // set null, not owned
    iFSObservers.Reset();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TFSMailMsgId CIpsPlgSettingsObserver::MailboxId()
    {
    FUNC_LOG;
    return iAccount;
    }
//  End of File  

