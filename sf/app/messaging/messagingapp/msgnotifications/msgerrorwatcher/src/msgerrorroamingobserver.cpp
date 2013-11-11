/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*       CMsgErrorRoamingObserver implementation file
*
*/



// INCLUDE FILES
#include    <e32base.h>
#include    <etel.h>
#include    <etelmm.h>
#include    <mmtsy_names.h>

#include    "msgerrorwatcher_p.h"
#include    "msgerrorroamingobserver.h"
#include    "debugtraces.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/

// ---------------------------------------------------------
// CMsgErrorRoamingObserver::CMsgErrorRoamingObserver
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgErrorRoamingObserver::CMsgErrorRoamingObserver( CMsgErrorWatcherPrivate&  aWatcher )
    : CActive( CActive::EPriorityStandard ),
    iWatcher( aWatcher ),
    iRoaming( EFalse ),
    iRegStatus( RMobilePhone::ERegistrationUnknown ),
    iRequestId( 0 )
    {
    QDEBUG_WRITE("CMsgErrorRoamingObserver:CMsgErrorRoamingObserver : Enter");
    CActiveScheduler::Add( this );
    QDEBUG_WRITE("CMsgErrorRoamingObserver:CMsgErrorRoamingObserver : Exit");
    }

/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/

/*****************************************************
*   Series 60 Customer / TSY
*   Needs customer TSY implementation
*****************************************************/

// ---------------------------------------------------------
// CMsgErrorRoamingObserver::ConstructL()
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMsgErrorRoamingObserver::ConstructL()
    {
    QDEBUG_WRITE("CMsgErrorRoamingObserver:ConstructL : Enter");
    User::LeaveIfError( iETelServer.Connect() );
    User::LeaveIfError( iETelServer.LoadPhoneModule( KMmTsyModuleName ) );
    User::LeaveIfError( iMobilePhone.Open( iETelServer, KMmTsyPhoneName ) );
    iMobilePhone.GetNetworkRegistrationStatus( iStatus, iRegStatus );
    iRequestId = EMobilePhoneGetNetworkRegistrationStatus;
    SetActive();
    QDEBUG_WRITE("CMsgErrorRoamingObserver:ConstructL : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorRoamingObserver::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgErrorRoamingObserver* CMsgErrorRoamingObserver::NewL( CMsgErrorWatcherPrivate& aWatcher )
    {
    QDEBUG_WRITE("CMsgErrorRoamingObserver:NewL : Enter");
    CMsgErrorRoamingObserver* self = new ( ELeave )
        CMsgErrorRoamingObserver( aWatcher );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    QDEBUG_WRITE("CMsgErrorRoamingObserver:NewL : Exit");

    return self;
    }

    
/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/

/*****************************************************
*   Series 60 Customer / TSY
*   Needs customer TSY implementation
*****************************************************/

// ---------------------------------------------------------
// CMsgErrorRoamingObserver::~CMsgErrorRoamingObserver
//
// Destructor
// ---------------------------------------------------------
//
CMsgErrorRoamingObserver::~CMsgErrorRoamingObserver()
    {
    QDEBUG_WRITE("CMsgErrorRoamingObserver:~CMsgErrorRoamingObserver : Enter");
    Cancel();
    iMobilePhone.Close();
    iETelServer.UnloadPhoneModule( KMmTsyModuleName );
    iETelServer.Close();    
    QDEBUG_WRITE("CMsgErrorRoamingObserver:~CMsgErrorRoamingObserver : Exit");
    }

/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/

// ---------------------------------------------------------
// CMsgErrorRoamingObserver::UpdateRoamingStatusL
//
// Checks whether roaming status has actually changed
// ---------------------------------------------------------
//
void CMsgErrorRoamingObserver::UpdateRoamingStatusL()
    {
    QDEBUG_WRITE("CMsgErrorRoamingObserver:UpdateRoamingStatusL : Enter");
    QDEBUG_WRITE_FORMAT("iRoaming: ", iRoaming);
    QDEBUG_WRITE_FORMAT("iRegStatus: ", iRegStatus);
    
    TBool changed = EFalse;
    if ( iRoaming && iRegStatus == RMobilePhone::ERegisteredOnHomeNetwork )
        {
        iRoaming = EFalse;
        changed = ETrue;
        }
    else if ( !iRoaming && iRegStatus == RMobilePhone::ERegisteredRoaming )
        {
        iRoaming = ETrue;
        changed = ETrue;
        }
    else
        {
        //Nothing to do
        }  
    QDEBUG_WRITE_FORMAT("changed: ", changed);
    if ( changed )
        {
        //Inform watcher only when roaming status has changed
        iWatcher.HandleRoamingEventL( iRoaming );
        }
    QDEBUG_WRITE("CMsgErrorRoamingObserver:UpdateRoamingStatusL : Exit");
    }

/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/

// ---------------------------------------------------------
// CMsgErrorRoamingObserver::Start
//
// Starts the active object
// ---------------------------------------------------------
//
void CMsgErrorRoamingObserver::Start()
    {
    QDEBUG_WRITE("CMsgErrorRoamingObserver:Start : Enter");
    if ( !IsActive() ) 
        {
        iMobilePhone.NotifyNetworkRegistrationStatusChange( iStatus, iRegStatus );
        iRequestId = EMobilePhoneNotifyNetworkRegistrationStatusChange;
        SetActive();
        }
    QDEBUG_WRITE("CMsgErrorRoamingObserver:Start : Exit");
    }

/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/

// ---------------------------------------------------------
// CMsgErrorRoamingObserver::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorRoamingObserver::DoCancel()
    {
    QDEBUG_WRITE("CMsgErrorRoamingObserver:DoCancel : Enter");
    if ( iRequestId )
        {
        iMobilePhone.CancelAsyncRequest( iRequestId );
        iRequestId = 0;
        }
    QDEBUG_WRITE("CMsgErrorRoamingObserver:DoCancel : Exit");
    }

/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/

// ---------------------------------------------------------
// CMsgErrorRoamingObserver::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorRoamingObserver::RunL()
    {
    QDEBUG_WRITE("CMsgErrorRoamingObserver:RunL : Enter");
    iRequestId = 0;
    TInt status = iStatus.Int();
    if ( status < 0 )
        {
        iRegStatus = RMobilePhone::ERegistrationUnknown;
        }
    TRAP_IGNORE( UpdateRoamingStatusL() );
    Start();
    QDEBUG_WRITE("CMsgErrorRoamingObserver:RunL : Exit");
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
