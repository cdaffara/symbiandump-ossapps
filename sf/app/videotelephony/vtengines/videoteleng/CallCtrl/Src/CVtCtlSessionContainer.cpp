/*
* Copyright (c) 2004 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container for ETel session and subsessions.
*
*/


#pragma CTC SKIP
// INCLUDE FILES
#include    "CVtCtlSessionContainer.h"
#include    "CVtCtlLineMonitor.h"
#include    "CVtCtlVideoCall.h"
#include    <mmtsy_names.h>
#include    <cvtlogger.h>

// CONSTANTS

// Retrials for connecting to ETel server
const TInt KVtCtlETelConnectRetrials = 5;

// Granularity
const TInt KVtCtlMaxCalls = 5;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtCtlSessionContainer::CVtCtlSessionContainer
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtCtlSessionContainer::CVtCtlSessionContainer(
    MVtCtlCallStatusObserver& aObserver ) :
    iCalls( KVtCtlMaxCalls ), 
    iObserver( aObserver )
    {
    __VTPRINTENTER( "CVtCtlSessionContainer.CVtCtlSessionContainer" )
    __VTPRINTEXIT( "CVtCtlSessionContainer.CVtCtlSessionContainer" )
    }

// -----------------------------------------------------------------------------
// CVtCtlSessionContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/
void CVtCtlSessionContainer::ConstructL()
    {
    __VTPRINTENTER( "CVtCtlSessionContainer.ConstructL" )
    TInt retries( KVtCtlETelConnectRetrials );
    TInt err( KErrNone );
    while ( retries-- && 
            iServer.Handle() == KNullHandle )
        {
        err = iServer.Connect();        
        }
    User::LeaveIfError( err );
    User::LeaveIfError( iServer.LoadPhoneModule( KMmTsyModuleName() ) );
    User::LeaveIfError( iPhone.Open( iServer, KMmTsyPhoneName() ) );
    User::LeaveIfError( iLine.Open( iPhone, KMmTsyDataLineName() ) );

    iLineMonitor = new ( ELeave ) CVtCtlLineMonitor( iLine, *this );
    iLineMonitor->Start();

    RMobileCall::TMobileCallStatus lineStatus;
    User::LeaveIfError( iLine.GetMobileLineStatus( lineStatus ) );
    if ( lineStatus != RMobileCall::EStatusIdle )
        {
        __VTPRINT( DEBUG_SESSION | DEBUG_CONSTRUCT, 
            "VtCtl:SesConL line active" )
        TInt calls( 0 );
        User::LeaveIfError( iLine.EnumerateCall( calls ) );
        while ( calls-- )
            {
            RLine::TCallInfo callInfo;
            iLine.GetCallInfo( calls, callInfo );
            if ( InitCallL( callInfo.iCallName ) )
                {
                __VTPRINT( DEBUG_SESSION | DEBUG_CONSTRUCT, 
                    "VtCtl:SesConL call found and created" )
                calls = 0;
                }
            }
        }

    __VTPRINTEXIT( "CVtCtlSessionContainer.ConstructL" )
    }

// -----------------------------------------------------------------------------
// CVtCtlSessionContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtCtlSessionContainer* CVtCtlSessionContainer::NewL( 
    MVtCtlCallStatusObserver& aObserver )
    {
    __VTPRINTENTER( "CVtCtlSessionContainer.NewL" )
    CVtCtlSessionContainer* self = 
        new ( ELeave ) CVtCtlSessionContainer( aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    __VTPRINTEXIT( "CVtCtlSessionContainer.NewL" )
    return self;
    }

    
// Destructor
CVtCtlSessionContainer::~CVtCtlSessionContainer()
    {
    __VTPRINTENTER( "CVtCtlSessionContainer.~" )

    TInt count( iCalls.Count() );
    CVtCtlVideoCall* call = NULL;
    while ( count-- )
        {
        call = iCalls[count];
        delete call;
        }
    iCalls.Close();
    delete iLineMonitor;
    iLine.Close();
    iPhone.Close();    
    iServer.UnloadPhoneModule( KMmTsyModuleName() );
    iServer.Close();
    __VTPRINTEXIT( "CVtCtlSessionContainer.~" )
    }

// -----------------------------------------------------------------------------
// CVtCtlSessionContainer::HandleLineEventL
// Handle new call on data line
// -----------------------------------------------------------------------------
//
void CVtCtlSessionContainer::HandleLineEventL(
    const TLineEvent aEvent, 
    TDesC& aCallName )
    {
    __VTPRINTENTER( "CVtCtlSessionContainer.HandleLineEventL" )
    if ( aEvent == ELineEventCallAdded )
        {
        CVtCtlVideoCall* call = FindCall( aCallName );
        if ( call == NULL )
            {
            InitCallL( aCallName );
            }
        }
    __VTPRINTEXITR( "CVtCtlSessionContainer.HandleLineEventL event=%d", 
        aEvent )
    }

// -----------------------------------------------------------------------------
// CVtCtlSessionContainer::HandleLineEventL
// 
// -----------------------------------------------------------------------------
//
/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/
void CVtCtlSessionContainer::HandleLineEventL( 
    const TLineEvent aEvent, 
    const RMobileCall::TMobileCallStatus aStatus )
    {
    __VTPRINTENTER( "CVtCtlSessionContainer.HandleLineEventL" )
    __VTPRINT3( DEBUG_SESSION, "SesCont.LineEvent2 event=%d,status=%d",
        (TInt) aEvent,
        (TInt) aStatus )
    if ( aEvent == ELineEventStatusChange && 
         ( aStatus == RMobileCall::EStatusRinging ||
           aStatus == RMobileCall::EStatusDialling ) )
        {
        TInt calls( 0 );
        iLine.EnumerateCall( calls );
        while ( calls-- )
            {
            RLine::TCallInfo callInfo;
            TInt err( iLine.GetCallInfo( calls, callInfo ) );
            // ensure we're opening correct call
            if ( err == KErrNone &&
               ( callInfo.iStatus == RCall::EStatusDialling ||
                 callInfo.iStatus == RCall::EStatusRinging ) )
                {
                calls = 0; // break
                CVtCtlVideoCall* call = FindCall( callInfo.iCallName );
                if ( call == NULL )
                    {
                    InitCallL( callInfo.iCallName );
                    }
                }
            }        
        }
    __VTPRINTEXIT( "CVtCtlSessionContainer.HandleLineEventL" )
    }

// -----------------------------------------------------------------------------
// CVtCtlSessionContainer::HandleCallStatusChangeL
// passes call status change event to observer
// -----------------------------------------------------------------------------
//
void CVtCtlSessionContainer::HandleCallStatusChangeL( 
    TVtCtlSessionId aId,
    MVtCtlCallControl::TVtCtlState aStatus,
    MVtCtlCallControl::TVtCtlState aPreviousStatus )
    {
    __VTPRINTENTER( "CVtCtlSessionContainer.HandleCallStatusChangeL" )
    iObserver.HandleCallStatusChangeL( aId, aStatus, aPreviousStatus );
    if ( aStatus == MVtCtlCallControl::EIdle )
        {
        DestructCall( aId );
        }
    __VTPRINTEXIT( "CVtCtlSessionContainer.HandleCallStatusChangeL" )
    }

// -----------------------------------------------------------------------------
// CVtCtlSessionContainer::FindCall
// Finds call by name
// -----------------------------------------------------------------------------
//
/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/
CVtCtlVideoCall* CVtCtlSessionContainer::FindCall( TDesC& aCallName ) const
    {
    __VTPRINTENTER( "CVtCtlSessionContainer.FindCall" )
    TInt count( iCalls.Count() );
    CVtCtlVideoCall* call = NULL;
    while ( count-- )
        {
        call = iCalls[count];
        const RMobileCall::TMobileCallInfoV1& mobileInfo = call->MobileInfo();
        if ( aCallName == mobileInfo.iCallName )
            {
            count = 0; // break
            }
        else
            {
            call = NULL;
            }
        }
    __VTPRINT2( DEBUG_SESSION | DEBUG_DETAIL, 
        "SesCont.FindCall by name --> %d",(TInt)call )
    __VTPRINTEXIT( "CVtCtlSessionContainer.FindCall" )
    return call;
    }


// -----------------------------------------------------------------------------
// CVtCtlSessionContainer::FindCall
// Finds call by id.
// -----------------------------------------------------------------------------
//
CVtCtlVideoCall* CVtCtlSessionContainer::FindCall( TVtCtlSessionId aId ) const
    {    
    __VTPRINTENTER( "CVtCtlSessionContainer.FindCall" )
    TInt count( iCalls.Count() );
    CVtCtlVideoCall* call = NULL;
    if ( aId == KActiveSessionId && count == 1 )
        {
        call = iCalls[0];
        count = 0;
        }
    while ( count-- )
        {
        call = iCalls[count];
        const CVtCtlVideoCall::TVtCtlCallInfo& info =
            call->CallInfo();
        if ( aId == info.iId )
            {
            count = 0; // break
            }
        else
            {
            call = NULL;
            }
        }
    __VTPRINT2( DEBUG_SESSION | DEBUG_DETAIL, 
        "SesCont.FindCall by id --> %d",(TInt)call )
    __VTPRINTEXIT( "CVtCtlSessionContainer.FindCall" )
    return call;
    }

// -----------------------------------------------------------------------------
// CVtCtlSessionContainer::Sessions
// Returns sessions.
// -----------------------------------------------------------------------------
//
TInt CVtCtlSessionContainer::GetSessions( RVtCtlSessionIdArray& aArray ) const
    {
    __VTPRINTENTER( "CVtCtlSessionContainer.GetSessions" )
    const TInt count( iCalls.Count() );
    for ( TInt ii = 0; ii < count; ii++ )
        {
        TInt err( aArray.Append( iCalls[ii]->CallInfo().iId ) );
        if ( err != KErrNone )
            {
            __VTPRINTEXIT( "CVtCtlSessionContainer.GetSessions" )
            return err;
            }
        }
    __VTPRINTEXIT( "CVtCtlSessionContainer.GetSessions" )
    return count;
    }

// -----------------------------------------------------------------------------
// CVtCtlSessionContainer::DestructCall
// Destructs call
// -----------------------------------------------------------------------------
//
void CVtCtlSessionContainer::DestructCall( TVtCtlSessionId aId )
    {
    __VTPRINTENTER( "CVtCtlSessionContainer.DestructCall" )
    TInt count( iCalls.Count() );
    CVtCtlVideoCall* call = NULL;
    while ( count-- )
        {
        call = iCalls[count];
        const CVtCtlVideoCall::TVtCtlCallInfo& info =
            call->CallInfo();
        if ( aId == info.iId )
            {
            delete call;
            iCalls.Remove( count );
            count = 0; // break
            }
        }
    __VTPRINTEXIT( "CVtCtlSessionContainer.DestructCall" )
    }

// -----------------------------------------------------------------------------
// CVtCtlSessionContainer::CheckCallTypeL
// Checks call type
// -----------------------------------------------------------------------------
//
/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/
TBool CVtCtlSessionContainer::CheckCallTypeL( 
    RMobileCall& aCall, 
    const TName& aName )
    {
    __VTPRINTENTER( "CVtCtlSessionContainer.CheckCallTypeL" )
    User::LeaveIfError( aCall.OpenExistingCall( iLine, aName ) );
    TBool typeOk( EFalse );
    RMobileCall::TMobileCallDataCapsV1 caps;
    RMobileCall::TMobileCallDataCapsV1Pckg pckg( caps );
    User::LeaveIfError( aCall.GetMobileDataCallCaps( pckg ) );
    __VTPRINT3( DEBUG_SESSION | DEBUG_DETAIL,
        "SesCont.ChkCall speedCaps=%d,protCaps=%d", 
        caps.iSpeedCaps, caps.iProtocolCaps )    
    if ( caps.iProtocolCaps & RMobileCall::KCapsIsdnMultimedia )
        {
        typeOk = ETrue;
        }
    else
        {        
        aCall.Close();
        }
    __VTPRINTEXIT( "CVtCtlSessionContainer.CheckCallTypeL" )
    return typeOk;
    }

// -----------------------------------------------------------------------------
// CVtCtlSessionContainer::InitCallL
// Initializes call if it is video call.
// -----------------------------------------------------------------------------
//
/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/
TBool CVtCtlSessionContainer::InitCallL( const TName& aCallName )
    {
    __VTPRINTENTER( "CVtCtlSessionContainer.InitCallL" )
    RMobileCall* mobilecall = new ( ELeave ) RMobileCall;
    CleanupStack::PushL( mobilecall );

    // check that type is multimedia call
    const TBool callTypeOk = CheckCallTypeL( *mobilecall, aCallName );
    if ( callTypeOk )
        {
        __VTPRINT( DEBUG_SESSION | DEBUG_DETAIL,
            "SesCont.InitCall ok" )
        TVtCtlSessionId id = iSessionIds.Create();
        CVtCtlVideoCall* call = CVtCtlVideoCall::NewL( 
            iLine, id, *this );
        CleanupStack::PushL( call );
        User::LeaveIfError( iCalls.Append( call ) );
        call->InitializeCallL( *mobilecall ); // takes ownership
        const CVtCtlVideoCall::TVtCtlCallInfo& callInfo = call->CallInfo();
        iObserver.HandleCallStatusChangeL( 
            id, callInfo.iStatus, callInfo.iPreviousStatus );
        CleanupStack::Pop(2); // mobilecall, call
        }
    else
        {
        CleanupStack::PopAndDestroy(); // mobilecall
        }
    __VTPRINTEXIT( "CVtCtlSessionContainer.InitCallL" )
    return callTypeOk;
    }

// -----------------------------------------------------------------------------
// CVtCtlSessionContainer::TVtCtlSessionIdentities::TVtCtlSessionIdentities
// C++ constructor
// -----------------------------------------------------------------------------
//
CVtCtlSessionContainer::TVtCtlSessionIdentities::TVtCtlSessionIdentities() 
    : iCurrentId( 0 )
    {
    __VTPRINTENTER( "TVtCtlSessionIdentities.TVtCtlSessionIdentities" )
    __VTPRINTEXIT( "TVtCtlSessionIdentities.TVtCtlSessionIdentities" )
    }

// -----------------------------------------------------------------------------
// CVtCtlSessionContainer::TVtCtlSessionIdentities::Create
// Increment session id.
// -----------------------------------------------------------------------------
//
TVtCtlSessionId CVtCtlSessionContainer::TVtCtlSessionIdentities::Create()
    {
    __VTPRINTENTER( "TVtCtlSessionIdentities.Create" )
    __VTPRINTEXIT( "TVtCtlSessionIdentities.Create" )
    return ++iCurrentId;
    }

// -----------------------------------------------------------------------------
// CVtCtlSessionContainer::TVtCtlSessionIdentities::Current
// Return current session id.
// -----------------------------------------------------------------------------
//
TVtCtlSessionId CVtCtlSessionContainer::TVtCtlSessionIdentities::Current() const
    {
    __VTPRINTENTER( "TVtCtlSessionIdentities.Current" )
    __VTPRINTEXIT( "TVtCtlSessionIdentities.Current" )
    return iCurrentId;
    }
#pragma CTC ENDSKIP
//  End of File  

