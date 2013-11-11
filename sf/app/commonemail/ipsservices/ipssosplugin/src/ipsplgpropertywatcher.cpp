/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CIpsPlgPropertyWatcher.
*
*/


#include "emailtrace.h"
#include "ipsplgheaders.h"

// ----------------------------------------------------------------------------
// class CIpsPlgPropertyWatcher
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgPropertyWatcher* CIpsPlgPropertyWatcher::NewL( 
    TInt aPriority, 
    CIpsPlgEventHandler& aEventHandler )
    {
    FUNC_LOG;
    CIpsPlgPropertyWatcher* self = new( 
        ELeave ) CIpsPlgPropertyWatcher( aPriority, aEventHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgPropertyWatcher::CIpsPlgPropertyWatcher( 
    TInt aPriority, 
    CIpsPlgEventHandler& aEventHandler )
    : CActive (aPriority), iEventHandler( aEventHandler )
    {
    FUNC_LOG;
    }


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgPropertyWatcher::~CIpsPlgPropertyWatcher()
    {
    FUNC_LOG;
    Cancel(); 
    iProperty.Close();
    }


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgPropertyWatcher::ConstructL()
    {
    FUNC_LOG;
    CActiveScheduler::Add(this);

    User::LeaveIfError( iProperty.Attach( KIpsPlgPropertyCatUid,
            KIPSSosEventPropertyKey ) );
    StartWatch();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgPropertyWatcher::StartWatch()
    {
    FUNC_LOG;
    iProperty.Subscribe( iStatus );
    SetActive();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgPropertyWatcher::DoCancel()
    {
    FUNC_LOG;
    iProperty.Cancel();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgPropertyWatcher::RunL()
    {
    FUNC_LOG;
    TPckgBuf<TIpsPlgPropertyEvent> event;
    TInt error = iProperty.Get( KIpsPlgPropertyCatUid,
            KIPSSosEventPropertyKey, event );
    
    if ( error == KErrNone )
        {
        TRAP_IGNORE( iEventHandler.NotifyPropertyEventL( event() ) );
        }
    
    StartWatch();
    }

