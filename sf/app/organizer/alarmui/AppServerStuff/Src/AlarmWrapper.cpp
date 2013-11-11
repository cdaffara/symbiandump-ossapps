/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This is the MAlarmObserver I/F implementation instantiated and used directly by EIkAlert.
*
*/



// INCLUDES
#include "pim_trace.h"
#include "AlarmWrapper.h"

#include <asshdalarm.h>
#include <uikon/eikalsup.h>
#include <AknSgcc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <uiklaf/private/pluginuid.hrh>
#endif

// ---------------------------------------------------------------------------
// DLL entry point
// ---------------------------------------------------------------------------
EXPORT_C MAlarmObserver* NewAlarm()
    {
    TRACE_ENTRY_POINT;
    CAknAlarmWrapper* alarmWrapper = new CAknAlarmWrapper;
    TRACE_EXIT_POINT;
    return alarmWrapper;
    }


// ---------------------------------------------------------------------------
// RAknAlarmClient
// ---------------------------------------------------------------------------

// Client for AknCapServer
RAknAlarmClient::RAknAlarmClient(CAknDataFetcher** aFetcher)
: iFetcher( aFetcher )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// In fact we don't need this, next method with dummy parameter is enough
TInt RAknAlarmClient::SendSynch(TInt aOpcode)
    {
    TRACE_ENTRY_POINT;
    TInt ret = CheckConnection();

    if (ret == KErrNone)
        {
        (*iFetcher)->Start(); // does nothing if already active
        ret = SendReceive( aOpcode );
        }
    TRACE_EXIT_POINT;
    return ret;
    }

// ?description
TInt RAknAlarmClient::SendSynch(TInt aOpcode, TInt& aParam)
    {
    TRACE_ENTRY_POINT;
    TInt ret = CheckConnection();

    if( ret == KErrNone )
        {
        (*iFetcher)->Start(); // does nothing if already active

        TPckg<TInt> pckg( aParam );
        ret = SendReceive( aOpcode, TIpcArgs( &pckg ) );
        }
    TRACE_EXIT_POINT;
    return ret;
    }

// Used currently only by cmd fetcher
void RAknAlarmClient::SendAsync(TInt aOpcode, TRequestStatus& aStatus, TIpcArgs& aArgs)
    {
    TRACE_ENTRY_POINT;
    // we currently assume that only this client will initiate asynch messages, and 
    // does it only when connection exists -> we don't check our connection before trying to send data
    SendReceive( aOpcode, aArgs, aStatus );
    TRACE_EXIT_POINT;
    }

// ?description
TInt RAknAlarmClient::SetAlarm(const TASShdAlarm& aAlarm, const TFullName& aOwner, const TDesC8& aAlarmData)
    {
    TRACE_ENTRY_POINT;
    TInt ret = CheckConnection();

    if( ret == KErrNone )
        {
        (*iFetcher)->Start(); // does nothing if already active
        TPckgC<TASShdAlarm> pAlarm( aAlarm );
        TIpcArgs args( &pAlarm, &aOwner, &aAlarmData );
        ret = SendReceive( EASAltOpCodeSetAlarm, args );
        }
    TRACE_EXIT_POINT;
    return ret;
    }

// connect on demand, returns KErrNotReady if akncapserver is not available yet
TInt RAknAlarmClient::CheckConnection()
    {
    TRACE_ENTRY_POINT;
    if( !Handle() )
        {
        RAknUiServer& client = *(CAknSgcClient::AknSrv());
        TInt err = KErrNotReady;

        if( client.Handle() != 0 ) // akncapserver running
            {
            _LIT( KServerNameFormat, "%08x_%08x_AppServer" );
            TFullName serverName;
            serverName.Format( KServerNameFormat, KUikonUidPluginInterfaceNotifiers, KAknCapServerUid.iUid );
            TRAP( err, ConnectExistingByNameL( serverName ); )
            }
        TRACE_EXIT_POINT;
        return err;
        }
    TRACE_EXIT_POINT;
    return KErrNone;
    }

// Just to prevent direct calling Connect, private
TInt RAknAlarmClient::Connect()
    {
    TRACE_ENTRY_POINT;
    TInt retVal = RAknUiServer::Connect();
    TRACE_EXIT_POINT;
    return retVal;
    }


// ---------------------------------------------------------------------------
// CAknDataFetcher
// ---------------------------------------------------------------------------

// Active object used as callback channel from akncapserver (Stop, Snooze), 
// basically always active after connection to akncapserver has been formed
CAknDataFetcher::CAknDataFetcher( TInt aPriority, 
                                  CEikAlmControlSupervisor* aSupervisor, 
                                  RAknAlarmClient& aClient )
: CActive( aPriority ), 
  iSupervisor( aSupervisor ),
  iClient( aClient ),
  iPckg( 0 ),
  iArgs( &iPckg )
    {
    TRACE_ENTRY_POINT;
    CActiveScheduler::Add( this );
    TRACE_EXIT_POINT;
    }

// ?description
void CAknDataFetcher::Start()
    {
    TRACE_ENTRY_POINT;
    if( !IsActive() )
        {
        SetActive();
        iClient.SendAsync( EASAltOpCodeNotify, iStatus, iArgs );
        }
    TRACE_EXIT_POINT;
    }

// Handle ie. deliver commands to real alarm supervisor (ie. Alarm Session)
void CAknDataFetcher::RunL()
    {
    TRACE_ENTRY_POINT;
    __ASSERT_DEBUG( iSupervisor != 0, User::Invariant() );

    switch( iStatus.Int() )
        {
        case ECmdAcknowledgedAlarm:
            iSupervisor->CmdAcknowledgeAlarm();
            break;

        case ECmdTaskAwayFromAlarm:
            {
            PIM_TRAPD_ASSERT( iSupervisor->CmdTaskAwayFromAlarmL(); )
            }
            break;

        case ECmdTaskAwayFromAlarmWTime:
            {
            PIM_TRAPD_ASSERT( iSupervisor->CmdTaskAwayFromAlarmL( iPckg() ); )
            }
            break;

        default:
            // just omit error, dont set us active until next real message is being sent
            TRACE_EXIT_POINT;
            return;
        }

    Start(); // restart
    TRACE_EXIT_POINT;
    }

// ?description
void CAknDataFetcher::DoCancel()
    {
    TRACE_ENTRY_POINT;
    // we cannot be active unless connetion is valid
    iClient.SendSynch( EASAltOpCodeNotifyCancel );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CAknAlarmWrapper
// ---------------------------------------------------------------------------

// ?description
CAknAlarmWrapper::CAknAlarmWrapper()
: iClient( &iActive )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// Main class, owns client and command fetcher, implements entrypoint and methods of MAlarmObserver
void CAknAlarmWrapper::ConstructL( CEikAlmControlSupervisor* aSupervisor,
                                   CEikServAppUiBase* aAppUi )
    {
    TRACE_ENTRY_POINT;
    iAppUi = aAppUi;
    iActive = new( ELeave )CAknDataFetcher( CActive::EPriorityHigh, aSupervisor, iClient );
    TRACE_EXIT_POINT;
    }

// ?description
CAknAlarmWrapper::~CAknAlarmWrapper()
    {
    TRACE_ENTRY_POINT;
    delete iActive;
    iClient.Close(); // if iActive was active, it could need the connection for cancellation
    TRACE_EXIT_POINT;
    }

// ?description
void CAknAlarmWrapper::Release()
    {
    TRACE_ENTRY_POINT;
    delete this;
    TRACE_EXIT_POINT;
    }

// ?description
void CAknAlarmWrapper::ShowAlarm()
    {
    TRACE_ENTRY_POINT;
    // ignore possible error
    TInt dummy = ETrue;
    iClient.SendSynch( EASAltOpCodeVisible, dummy );
    TRACE_EXIT_POINT;
    }

// ?description
void CAknAlarmWrapper::HideAlarm()
    {
    TRACE_ENTRY_POINT;
    // ignore possible error
    TInt dummy = EFalse;
    iClient.SendSynch( EASAltOpCodeVisible, dummy );
    TRACE_EXIT_POINT;
    }

// ?description
TInt CAknAlarmWrapper::CurrentServerState() const
    {
    TRACE_ENTRY_POINT;
    RAknAlarmClient& client =  MUTABLE_CAST( RAknAlarmClient&, iClient );
    TInt ret( 0 ), err( 0 );
    err = client.SendSynch( EAknSAltOpCodeAskServerState, ret );

    if( err == KErrNotReady ) // special case
        {
        TRACE_EXIT_POINT;
        return 0; // as alarm control is CBase derived, state will be null until updated otherwise
        }

    TRACE_EXIT_POINT;
    return ( err != KErrNone ? err : ret );
    }

// ?description
void CAknAlarmWrapper::UpdateSoundPauseTimeInterval(TInt /*aMinutes*/)
    {
    TRACE_ENTRY_POINT;
    // empty
    TRACE_EXIT_POINT;
    }

// ?description
void CAknAlarmWrapper::UpdateForAlarmServerState(TInt aNewAlarmServerState)
    {
    TRACE_ENTRY_POINT;
    // ignore possible error
    iClient.SendSynch( EASAltOpCodeSetState, aNewAlarmServerState );
    TRACE_EXIT_POINT;
    }

// ?description
void CAknAlarmWrapper::UpdateAlarmInfo(const TASShdAlarm& aAlarm,const TFullName& aOwner)
    {
    TRACE_ENTRY_POINT;
    TInt err = iClient.SetAlarm( aAlarm, aOwner, KNullDesC8 );

    if( err == KErrNotReady )
        {
        TRAP( err, iActive->iSupervisor->CmdTaskAwayFromAlarmL( 1 ); ) // snooze to one minute
        }
    ASSERT( !err );
    TRACE_EXIT_POINT;
    }

// ?description
void CAknAlarmWrapper::StartPlayAlarmL(const TDesC& /*aAlarmName*/)
    {
    TRACE_ENTRY_POINT;
    // empty
    TRACE_EXIT_POINT;
    }

// ?description
void CAknAlarmWrapper::StopPlayAlarm()
    {
    TRACE_ENTRY_POINT;
    // empty
    TRACE_EXIT_POINT;
    }


// End of File
