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
* Description: 
*     Timer for smtp plugin
*
*/

#include "ipssosaopluginheaders.h"

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOSmtpSendWatcher* CIpsSosAOSmtpSendWatcher::NewL( 
    TInt aPriority, 
    CMsvSession& aSession )
    {
    FUNC_LOG;
    CIpsSosAOSmtpSendWatcher* self = new( 
        ELeave ) CIpsSosAOSmtpSendWatcher( aPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aSession );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOSmtpSendWatcher::CIpsSosAOSmtpSendWatcher( 
    TInt aPriority )
    : CActive (aPriority)
    {
    FUNC_LOG;
    }


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOSmtpSendWatcher::~CIpsSosAOSmtpSendWatcher()
    {
    FUNC_LOG;
    Cancel(); 
    iProperty.Close();
    delete iAgent;
    }


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSmtpSendWatcher::ConstructL( CMsvSession& aSession )
    {
    FUNC_LOG;
    CActiveScheduler::Add(this);

    User::LeaveIfError( iProperty.Attach( KIpsPlgPropertyCatUid,
            KIPSSosEventPropertyKey ) );
    
    iAgent = CIpsSosAOSmtpAgent::NewL( aSession );
    StartWatch();
    }



// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSmtpSendWatcher::StartWatch()
    {
    FUNC_LOG;
    iProperty.Subscribe( iStatus );
    SetActive();
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSmtpSendWatcher::DoCancel()
    {
    FUNC_LOG;
    iProperty.Cancel();
    }


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSmtpSendWatcher::RunL()
    {
    FUNC_LOG;
    TPckgBuf<TIpsPlgPropertyEvent> ebuf;
    TInt error = iProperty.Get( KIpsPlgPropertyCatUid,
            KIPSSosEventPropertyKey, ebuf );
    
    TIpsPlgPropertyEvent event = ebuf();
    
    if ( event.iEvent == KIPSSosSmtpEmptyOutboxNow )
        {
        iAgent->EmptyOutboxNowL( event.iMailbox );
        }
    StartWatch();
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TInt CIpsSosAOSmtpSendWatcher::RunError( TInt /* aError */ )
    {
    FUNC_LOG;
    // Just ignore any error and continue without
    // any handling to allow smooth execution. 
    StartWatch();
    return KErrNone;            
    }


//EOF

