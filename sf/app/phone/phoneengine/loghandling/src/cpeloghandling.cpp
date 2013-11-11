/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the implementation of CPELogHandling class 
*                member functions.
*
*/



//  INCLUDE FILES
#include <spsettings.h>
#include <spproperty.h>
#include <telloggingextension.h>
#include <ecom.h> 
#include "cpeloghandling.h"
#include "cpelogevent.h"
#include "cpeloghandlingcommand.h"
#include "cpelogexternaldata.h"
#include <talogger.h>
#include <logcli.h>
#include <pepanic.pan>
#include <mpephonemodelinternal.h>
#include <mpedatastore.h>
#include <hbtextresolversymbian.h>
#include "cpeloginfo.h"
#include "cpelogextensionwrapper.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
/// None

// CONSTANTS
_LIT(KEmergencyText, "txt_phone_other_emergency_call");
_LIT(KPhoneLocalisationFile, "telephone_");
_LIT(KTsFilePath, "z:/resource/qt/translations/");

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// -----------------------------------------------------------------------------
// CPELogHandling::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MPELogHandling* CPELogHandling::NewL
        ( 
        MPEPhoneModelInternal& aModel,        // Owner of the object
        RFs& aFsSession                       // Handle to a file server session
        )  
    {
    TEFLOGSTRING(KTAOBJECT, "LOG CPELogHandling::NewL start.");
    CPELogHandling* self = 
        new ( ELeave ) CPELogHandling( aModel, aFsSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TEFLOGSTRING(KTAOBJECT, "LOG CPELogHandling::NewL complete.");
    return self;
    }
 
// -----------------------------------------------------------------------------
// CPELogHandling::~CPELogHandling
// Destructor.
// -----------------------------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / LOGENG
*    Series 60  LOGENG API
*****************************************************/
EXPORT_C CPELogHandling::~CPELogHandling()
    {
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogHandling::~CPELogHandling" );
    
    delete iLogExternalData;

    iFreeLogEventArray.ResetAndDestroy();
    iActiveLogEventArray.ResetAndDestroy();
    // All created CPELogEvent objects must be either in Free or Active array
    // That is, it would be error to destroy objects in queued array.
    iQueuedLogEventArray.Reset();
    iActiveCallIds.Close();

    delete iLogHandlingCommand;
    delete iLogClient;
    iPlugins.ResetAndDestroy();
    REComSession::FinalClose();
    }
    
// -----------------------------------------------------------------------------
// CPELogHandling::CPELogHandling
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPELogHandling::CPELogHandling
        ( 
        MPEPhoneModelInternal& aModel,
        RFs& aFsSession        
        ) : iModel( aModel ),
            iDataStore( *( aModel.DataStore( ) ) ),
            iFsSession( aFsSession )
    {
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogHandling::CPELogHandling" );
    }

// -----------------------------------------------------------------------------
// CPELogHandling::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / LOGENG
*    Series 60  LOGENG API
*****************************************************/
void CPELogHandling::ConstructL()
    {
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogHandling::ConstructL" );
    // Create new instace of the Log Client
    iLogClient = CLogClient::NewL( iFsSession ); 

    // Enumeration for incoming call, get from database 
    User::LeaveIfError( iLogClient->GetString( iLogStringIn, R_LOG_DIR_IN ) ); 
    // Enumeration for outgoing call, get from database 
    User::LeaveIfError( iLogClient->GetString( iLogStringOut, R_LOG_DIR_OUT ) );   
    // Enumeration for missed call, get from database
    User::LeaveIfError( iLogClient->GetString( iLogStringMissed, R_LOG_DIR_MISSED ) );   
    // Enumeration for delivery call, get from database  
    User::LeaveIfError( iLogClient->GetString( iLogStringDelivery, R_LOG_DEL_NONE ) ); 
    // Enumeration for incoming auxiliary line, get from database 
    User::LeaveIfError( iLogClient->GetString( iLogStringInAlt, R_LOG_DIR_IN_ALT ) );   
    // Enumeration for outgoing auxiliary line, get from database  
    User::LeaveIfError( iLogClient->GetString( iLogStringOutAlt, R_LOG_DIR_OUT_ALT ) );
    // Enumeration for unknown name, get from database  
    User::LeaveIfError( iLogClient->GetString( iLogStringUnknown, R_LOG_REMOTE_UNKNOWN ) );
      
    iLogHandlingCommand = new (ELeave) CPELogHandlingCommand( *this, *iLogClient );
 
    // Reserve some log events here. We delete these log event objects only on the destructor.
    for( TInt entryIndex = 0; entryIndex < KPEMaximumNumberOfLogEvents; entryIndex++ )
        {
        CPELogEvent* logEvent = CPELogEvent::NewL( *this, *iLogHandlingCommand );
        CleanupStack::PushL( logEvent );
        iFreeLogEventArray.AppendL( logEvent );
        CleanupStack::Pop( logEvent );
        }
    // Compress free log event array as it would never contain more objects than create here
    iFreeLogEventArray.Compress( );

    iLogExternalData = CPELogExternalData::NewL( *this );
    }

// -----------------------------------------------------------------------------
// CPELogHandling::SaveCallEntry
// Creates log info and initiates saving call log info by calling SaveCallInfoL.
// Do log handling cleanup in case of a leave.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPELogHandling::SaveCallEntry
        ( 
        const TInt aCallId
        )
    {
    TInt errorCode( KErrArgument );
    if ( CallIdCheck::IsVoice( aCallId ) || 
         CallIdCheck::IsVideo( aCallId )
       ) 
        {
        CPELogInfo* logInfo( NULL );
        TRAP( errorCode, logInfo = CPELogInfo::NewL(); );
            
        if ( logInfo )
            {
            // continue gathering log data
            
            // set phonenumber and/or voip address
            SetRemoteContact( aCallId, *logInfo );
            logInfo->SetEventData( aCallId, iDataStore ); 
            TRAP_IGNORE( 
                // Contact link can be big, not critical for basic functionality.
                if ( &iDataStore.ContactId( aCallId ) )
                    {
                    // Pack and add link
                    HBufC8* buf( iDataStore.ContactId( aCallId ).PackLC() );
                    TEFLOGSTRING( KTAINT, "Contact id packed" );
                    logInfo->SetContactLink( buf ); 
                    CleanupStack::Pop( buf );
                    }
                // default logging strategy is used if error happens while using extension
                UpdateLogInfoWithExtensionDataL( aCallId, *logInfo ) );
            
            if ( CPELogInfo::EPEEmergecyEvent == logInfo->EventType() )
                {
                SetEmergencyCallName( *logInfo );
                }
            
            TRAP( errorCode, SaveCallInfoL( *logInfo ) );
            delete logInfo;
            }
        
        if( errorCode != KErrNone )
            {
            DoCleanup();
            }
        }
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPELogHandling::SendMessage
// Method reroutes messages from other modules to the CPEPho-object
// Note that it does not use base class SendMessage.
// -----------------------------------------------------------------------------
//
void CPELogHandling::SendMessage
        ( 
        const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage, // The message id number of the message.
        TInt aErrorCode
        )
    {
    if ( aMessage == MEngineMonitor::EPEMessageLogEventSaved )
        {
        TEFLOGSTRING( KTAINT, "LOG CPELogHandling::SendMessage(), Log event saved" );                        
        
        if (aErrorCode == KErrNone )
            {
            // flag event entry as added; next event is to be just entry update.
            iLogEventUnderProcessing->SetAdded( );
            }
        
        if ( iLogEventUnderProcessing->IsCompleted( ) )
            {
            TEFLOGSTRING( KTAINT, "LOG CPELogHandling::SendMessage(), Log entry completed" );                       
                       
            DeleteArrayEntry( iLogEventUnderProcessing );
            }

        // Indicate that no processing of log event is in progress
        iLogEventUnderProcessing = NULL;            
            
        if ( iQueuedLogEventArray.Count( ) > 0 )
            {
            TEFLOGSTRING( KTAINT, "LOG CPELogHandling::SendMessage(), Executing queued request" );
            
            CPELogEvent* logEvent = iQueuedLogEventArray[0];
            iQueuedLogEventArray.Remove( 0 );
            iLogEventUnderProcessing = logEvent;
            TRAPD( error, logEvent->SaveL( ) );
            if ( error != KErrNone )
                {
                DoCleanup();
                }
            }
        }
    else
        {
        TEFLOGSTRING2( KTAINT, "LOG CPELogHandling::SendMessage(), Message %d not supported", aMessage );
        }   
    }
        
// -----------------------------------------------------------------------------
// CPELogHandling::LogStringIn
// Get log string.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TLogString& CPELogHandling::LogStringIn
        (
        // None
        )
    {
    return iLogStringIn;
    }

// -----------------------------------------------------------------------------
// CPELogHandling::LogStringOut
// Get log string.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TLogString& CPELogHandling::LogStringOut
        (
        // None
        )
    {
    return iLogStringOut;
    }
    
// -----------------------------------------------------------------------------
// CPELogHandling::LogStringMissed
// Get log string.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TLogString& CPELogHandling::LogStringMissed
        (
        // None
        )
    {
    return iLogStringMissed;
    }
    
// -----------------------------------------------------------------------------
// CPELogHandling::LogStringIn
// Get log string.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TLogString& CPELogHandling::LogStringDelivery
        (
        // None
        )
    {
    return iLogStringDelivery;
    }        

// -----------------------------------------------------------------------------
// CPELogHandling::LogStringIn
// Get log string.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TLogString& CPELogHandling::LogStringInAlt
        (
        // None
        )
    {
    return iLogStringInAlt;
    }

// -----------------------------------------------------------------------------
// CPELogHandling::LogStringOutAlt
// Get log string.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TLogString& CPELogHandling::LogStringOutAlt
        (
        // None
        )
    {
    return iLogStringOutAlt;
    }

// -----------------------------------------------------------------------------
// CPELogHandling::LogStringUnknown
// Get log string.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TLogString& CPELogHandling::LogStringUnknown
        (
        // None
        )
    {
    return iLogStringUnknown;
    }

// -----------------------------------------------------------------------------
// CPELogHandling::SetRemoteContact
// -----------------------------------------------------------------------------
//
void CPELogHandling::SetRemoteContact( TInt aCallId, CPELogInfo& aLogInfo )
    {
    aLogInfo.SetVoipAddress( KNullDesC() );
    
    if ( iDataStore.CallDirection( aCallId ) 
            == RMobileCall::EMobileOriginated )
        {
        TEFLOGSTRING2(KTAINT, 
            "LOG CPELogHandling::CreateLogInfo, WholeOutgoingPhoneNumber, aCallId: %d"
            , aCallId )
        aLogInfo.SetPhoneNumber( iDataStore.WholeOutgoingPhoneNumber( aCallId ) );
        }
    else
        {
        aLogInfo.SetPhoneNumber( iDataStore.RemotePhoneNumber( aCallId ) );
        }
    
    if ( EPECallTypeVoIP == iDataStore.CallType( aCallId ) )
        {
        // voip address field must be used for voip calls
        aLogInfo.SetVoipAddress( aLogInfo.PhoneNumber() );
        aLogInfo.SetPhoneNumber( KNullDesC() );
        }
    }


// -----------------------------------------------------------------------------
// CPELogHandling::SaveCallInfoL
// Update log external data.
// Call SaveEventL if event should be saved to log db.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPELogHandling::SaveCallInfoL
        ( 
        const CPELogInfo& aLogInfo // Call information
        )
    {
    TEFLOGSTRING3( KTAINT, "LOG CPELogHandling::SaveCallInfoL(), [CallID: %d], [CallState: %d]", aLogInfo.CallId(), aLogInfo.CallState() );
    
    if ( EPEStateConnected == aLogInfo.CallState() )
        {
        // Insert the call ID to the active call array when the call state is connected.
        TInt err = iActiveCallIds.InsertInOrder( aLogInfo.CallId() );
        if ( err != KErrAlreadyExists )
            {
            User::LeaveIfError( err );
            }
        
        // Only for the first call set the first call start time
        if ( iActiveCallIds.Count() == 1 )
            {
            // For Last active call timer. See end part of UpdateLastCallTimer.
            iLogExternalData->SetFirstCallStartTime( aLogInfo.CallStartTime() );
            }
  
        // We have already store the start time to log database on dialling/ringing state,
        // but this is the real start time of call.
        TBuf<KPEESDWFormattedTimeLength> formattedTime;
        formattedTime.Zero();
        aLogInfo.CallStartTime().FormatL( formattedTime, KPEESDWTimeFormat ); 
        TEFLOGSTRING2( KTAINT, "LOG CPELogHandling::SaveCallInfoL() Connected state iCallStartTime > formattedTime: %S", &formattedTime );
        }
    else
        {
        // logsEntry handling only on Dialling/Ringing and Idle states.
        if ( EPEStateIdle == aLogInfo.CallState() )
            {          
            iLogExternalData->UpdateCallTimers( aLogInfo );
            if ( aLogInfo.MissedCall() )  
                {
                iLogExternalData->IncreaseMissedCallValue( aLogInfo.CallId() );
                }

            TInt index = iActiveCallIds.FindInOrder( aLogInfo.CallId() );
            if ( index >= 0 )
                {
                // When a call is disconnected, its ID is removed from the active call array.
                iActiveCallIds.Remove( index );
                iLogExternalData->UpdateLastCallTimerByLine( aLogInfo );
                if ( iActiveCallIds.Count() == 0 )
                    {
                    TInt duration = iLogExternalData->UpdateLastCallTimer( aLogInfo );
                    iModel.DataStore()->SetCallDuration( duration );
                    }
                }
            }
        if ( aLogInfo.LoggingEnabled() ) 
            {
            // We log all voice calls and the following data/fax calls to Log database
            // MO external Data calls,
            // MO external Fax calls,
            // MT external Data calls
            // MT external Fax calls,
            // MT internal Data calls,
            // We don't log the following data/fax calls to Log database
            // MO internal Data calls
            // MO internal Fax calls,
            // MT internal Fax calls, 
            // these are logged by CSD agent.
            // But we have to update timers for these calls.            
            
            SaveEventL( aLogInfo );
            }
        }
    }


// -----------------------------------------------------------------------------
// CPELogHandling::SaveEventL
// Update log database entry for an event. Manages event queueing.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPELogHandling::SaveEventL
        ( 
        const CPELogInfo& aLogInfo  // Log information
        ) 
    {
    TInt index;
    CPELogEvent* logEvent;
    
    
    index = FindEventIndexById( aLogInfo.CallId() ); 
    if ( index == KErrNotFound ) 
        {
        // a log event object can't be reused;  
        TEFLOGSTRING2( KTAINT, "LOG CPELogHandling::SaveEventL(), [CallID: %d]",aLogInfo.CallId() );
        TEFLOGSTRING2( KTAINT, "LOG CPELogHandling::SaveEventL(), [CallDirection: %d]", aLogInfo.CallDirection() );

        if ( iFreeLogEventArray.Count() > 0 )
            {
            // reuse an already created object from the array of the freed log event objects
            logEvent = iFreeLogEventArray[ 0 ];
            iFreeLogEventArray.Remove( 0 );
            }
        else 
            {
            // create a new object; none available to be reused
            logEvent = CPELogEvent::NewL( *this, *iLogHandlingCommand );
            }
            
        CleanupStack::PushL( logEvent );
        iActiveLogEventArray.AppendL( logEvent );
        CleanupStack::Pop( logEvent );            
        }
    else
        {
        // the log event object is already active but not completed. reuse it.
        logEvent = iActiveLogEventArray[ index ];
        }
        
    __ASSERT_ALWAYS( logEvent, Panic(EPEPanicNullPointer) );
        

    // Save logInfo for possible queueing.
    logEvent->UpdateLogInfoL( aLogInfo );
       
    if ( iLogEventUnderProcessing )
        {
        // queueing can not be done per event because log client
        // would fail if new request is sent before the previous one
        // has been processed even if they are for two different events.
        // See Symbian doc for more details.
        if ( iQueuedLogEventArray.Find( logEvent ) == KErrNotFound )  
            {
            TEFLOGSTRING( KTAINT, "LOG CPELogHandling::SaveEventL(), AO busy, request queued" );                
            // queue the updated event only if event is not already queued
            TInt error = iQueuedLogEventArray.Append( logEvent );
            if ( error != KErrNone )
                {
                TEFLOGSTRING( KTAERROR, "LOG CPELogHandling::SaveEventL(), Append to QueuedArray failed!" );
                TEFLOGSTRING( KTAERROR, "LOG CPELogHandling::SaveEventL(), WARNING: Array Entry will be deleted to prevent memory leak." );
                       
                DeleteArrayEntry( logEvent );
                
                User::LeaveIfError( error );
                }
            }
        else // the existing queued request will be executed with updated info
            {
            TEFLOGSTRING( KTAINT, "LOG CPELogHandling::SaveEvent(), AO busy, queued request already exists" );
            }
        }   
    else
        {
        iLogEventUnderProcessing = logEvent;
        logEvent->SaveL( );            
        }
    }
    
// -----------------------------------------------------------------------------
// CPELogsHandling::FindEventIndexById
// Find index number from array with given callid.
// -----------------------------------------------------------------------------
//
TInt CPELogHandling::FindEventIndexById
        ( 
        const TInt aCallId // Call identification number
        )
    {
    TEFLOGSTRING( KTAINT, 
        "Log Handling: CPELogHandling::FindEventIndexById() - 1" );
    for( TInt index = 0; index < iActiveLogEventArray.Count(); index++ )
        { 
        TEFLOGSTRING( KTAINT, 
            "Log Handling: CPELogHandling::FindEventIndexById() - 2" );        
        if( (iActiveLogEventArray[ index ]->CallId() == aCallId) &&
            !(iActiveLogEventArray[ index ]->IsCompleted()) )
            {
            TEFLOGSTRING( KTAINT, 
                "LOG CPELogHandling::FindEventIndexById() - 3" );
            return index;
            }
        }
    TEFLOGSTRING( KTAINT, 
        "LOG CPELogHandling::FindEventIndexById() - 4" );
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CPELogsHandling::DeleteArrayEntry
// Delete array's enty by given callid.
// -----------------------------------------------------------------------------
//
void CPELogHandling::DeleteArrayEntry
        ( 
        CPELogEvent* aLogEvent
        )
    {
    TInt index = iActiveLogEventArray.Find( aLogEvent );
    TInt queuedIndex = iQueuedLogEventArray.Find( aLogEvent );
    TInt freeIndex = iFreeLogEventArray.Find( aLogEvent );
    
    // Reset event before deletion
    aLogEvent->ResetEvent( );
    aLogEvent->ResetLogInfo();
    
    // Remove event from queued array as it is there by error and should not be processed after deletion
    if ( queuedIndex != KErrNotFound )
        {
        TEFLOGSTRING( KTAERROR, "LOG CPELogHandling::DeleteArrayEntry: WARNING Log event removed from queued event array. This should not happen." )
        iQueuedLogEventArray.Remove( queuedIndex );
        }
    
    if ( index != KErrNotFound )
        {
        iActiveLogEventArray.Remove( index );
        }
    else
        {
        TEFLOGSTRING( KTAERROR, "LOG CPELogHandling::DeleteArrayEntry: WARNING Log event NOT in active event array. This should not happen." )
        }
        
    if ( freeIndex == KErrNotFound )
        {   
        if ( iFreeLogEventArray.Count( ) < KPEMaximumNumberOfLogEvents )
            {
            if (iFreeLogEventArray.Append( aLogEvent ) != KErrNone)
                {
                // prevent memory leak
                delete aLogEvent;
                }
            }
        else
            {
            // prevent memory leak
            delete aLogEvent;
            }
        }
    }

// -----------------------------------------------------------------------------
// CPELogsHandling::DoCleanup
// Do log handling cleanup in case of leave during handling of a log event.
// -----------------------------------------------------------------------------
//
void CPELogHandling::DoCleanup()
    {
    if ( iLogEventUnderProcessing ) 
        {
        // delete log event entry only if save leaves and event is completed
        // otherwise we expect more log events for the same entry that is no need to delete
        if ( iLogEventUnderProcessing->SaveLeave( ) && 
             iLogEventUnderProcessing->IsCompleted( ) )
            {
            TEFLOGSTRING( KTAERROR, 
                "LOG CPELogHandling::DoCleanup(), WARNING: Array Entry will be deleted to prevent memory leak." );
            
            DeleteArrayEntry( iLogEventUnderProcessing );
            }
        }
    }


// -----------------------------------------------------------------------------
// CPELogsHandling::ResetMissedCalls()
// Reset missed call.
// -----------------------------------------------------------------------------
//
void CPELogHandling::ResetMissedCalls( TInt aCallId )
    {
    TEFLOGSTRING( KTAINT, 
        "LOG CPELogsHandling::ResetMissedCalls > SetMissedCall" );
    
    TEFLOGSTRING2(KTAINT, 
            "LOG CPELogHandling::ResetMissedCalls, aCallId: %d"
            , iDataStore.CallId() );
    iDataStore.SetMissedCall( EFalse, aCallId );
    }


// -----------------------------------------------------------------------------
// CPELogHandling::UpdateLogInfoWithExtensionDataL
// Log information is not changed if some error happens with extension usage. 
// -----------------------------------------------------------------------------
//
void CPELogHandling::UpdateLogInfoWithExtensionDataL( TInt aCallId, 
        CPELogInfo& aLogInfo )
    {
    TUint serviceId( iDataStore.ServiceId( aCallId ) );
    CTelLoggingExtension& extension = LoggingExtensionL( serviceId );
    
    // initialize extension with original remote contact number/address
    const TPEPhoneNumber* origRemoteContact = NULL;
    RMobileCall::EMobileOriginated == iDataStore.CallDirection( aCallId )
        ? origRemoteContact = &iDataStore.WholeOutgoingPhoneNumber( aCallId )
        : origRemoteContact = &iDataStore.RemotePhoneNumber( aCallId );
    __ASSERT_ALWAYS( NULL != origRemoteContact, User::Leave( KErrNotFound ) );
    extension.InitializeL( serviceId, *origRemoteContact );
    
    RBuf phoneNumber;
    CleanupClosePushL( phoneNumber );
    User::LeaveIfError( extension.GetPhoneNumber( phoneNumber ) );
    
    RBuf voipAddress;
    CleanupClosePushL( voipAddress );
    User::LeaveIfError( extension.GetVoipAddress( voipAddress ) );
    
    RBuf myAddress;
    CleanupClosePushL( myAddress );
    User::LeaveIfError( extension.GetMyAddress( myAddress ) );
    
    RBuf remotePartyName;
    CleanupClosePushL( remotePartyName );
    User::LeaveIfError( extension.GetRemotePartyName( remotePartyName ) );
    
    // update log info with successfully queried extension data
    SetExtensionData( aLogInfo, phoneNumber, voipAddress, 
        myAddress, remotePartyName );
    
    CleanupStack::PopAndDestroy( 4, &phoneNumber );
    }


// -----------------------------------------------------------------------------
// CPELogHandling::LoggingExtensionL
// -----------------------------------------------------------------------------
//
CTelLoggingExtension& CPELogHandling::LoggingExtensionL( TUint aServiceId )
    {
    TUid pluginUid = LoggingPluginIdentifierL( aServiceId );
    
    CPELogExtensionWrapper* wrapper = NULL;
    TInt pluginInd = 
        iPlugins.Find( pluginUid, CPELogExtensionWrapper::MatchByUid );
    if ( KErrNotFound == pluginInd )
        {
        wrapper = CreateExtensionWrapperLC( pluginUid );
        iPlugins.AppendL( wrapper );
        CleanupStack::Pop( wrapper );
        }
    else
        {
        wrapper = iPlugins[pluginInd];
        }
    
    __ASSERT_ALWAYS( NULL != wrapper, User::Leave( KErrNotFound ) );
    return *wrapper;
    }


// -----------------------------------------------------------------------------
// CPELogHandling::LoggingPluginIdentifierL
// Resolves plugin identifier for the given service.
// -----------------------------------------------------------------------------
//
TUid CPELogHandling::LoggingPluginIdentifierL( TUint aServiceId ) const
    {
    TInt pluginUid( 0 );
    CSPSettings* settings = CSPSettings::NewLC();
    CSPProperty* property = CSPProperty::NewLC();
    
    TInt result = settings->FindPropertyL( aServiceId, 
        EPropertyCallLoggingPluginId, *property );
    User::LeaveIfError( result );
    User::LeaveIfError( property->GetValue( pluginUid ) );
    CleanupStack::PopAndDestroy( property );
    CleanupStack::PopAndDestroy( settings );
    
    return TUid::Uid( pluginUid );
    }


// -----------------------------------------------------------------------------
// CPELogHandling::SetExtensionData
// Lengths of the descriptors gotten from plugin must be checked, because
// extension API does not set limits for data length. In case that phone number
// is available, voip address is not saved to logs. That enables user to select
// call type s/he wants when calling from logs.
// -----------------------------------------------------------------------------
//
void CPELogHandling::SetExtensionData( CPELogInfo& aLogInfo, 
        const TDesC& aPhoneNumber, const TDesC& aVoipAddress, 
        const TDesC& aMyAddress, const TDesC& aRemotePartyName )
    {
    if ( aPhoneNumber.Length() <= aLogInfo.PhoneNumber().MaxLength() )
        {
        aLogInfo.SetPhoneNumber( aPhoneNumber );
        aLogInfo.SetVoipAddress( KNullDesC() );
        }
    
    if ( aVoipAddress.Length() <= aLogInfo.VoipAddress().MaxLength() )
        {
        if ( KNullDesC() == aLogInfo.PhoneNumber() )
            {
            aLogInfo.SetVoipAddress( aVoipAddress );
            }
        }
    
    if ( aMyAddress.Length() <= aLogInfo.MyAddress().MaxLength() )
        {
        aLogInfo.SetMyAddress( aMyAddress );
        }
    
    // Try to use user defined contact name from contacts, remote party name
    // from extension or voip address from extension as a contact name to be
    // saved in logs, in that order.
    TBool noContactMatch = ( KNullDesC() == aLogInfo.Name() ); 
    if ( noContactMatch )
        {
        if ( aRemotePartyName.Length() != 0 )
            {
            aLogInfo.SetName( aRemotePartyName.Left( aLogInfo.Name().MaxSize() ) );
            }
        else if ( KNullDesC() != aLogInfo.VoipAddress() )
            {
            aLogInfo.SetName( aLogInfo.VoipAddress() );
            }
        else
            {
            aLogInfo.SetName( aLogInfo.PhoneNumber() );
            }
        }
    }

// -----------------------------------------------------------------------------
// CPELogHandling::CreateExtensionWrapperLC
// -----------------------------------------------------------------------------
//
CPELogExtensionWrapper* CPELogHandling::CreateExtensionWrapperLC( 
        const TUid& aPluginUid ) const
    {
    return CPELogExtensionWrapper::NewLC( aPluginUid );
    }

// -----------------------------------------------------------------------------
// CPELogHandling::SetEmergencyCallName
// -----------------------------------------------------------------------------
//
void CPELogHandling::SetEmergencyCallName( CPELogInfo& aLogInfo )
    {
    HBufC* emergencyText = NULL;

    if ( HbTextResolverSymbian::Init( KPhoneLocalisationFile, KTsFilePath) )
        {
        TRAP_IGNORE( emergencyText = 
                HbTextResolverSymbian::LoadL(KEmergencyText) );
        }
    
    if ( emergencyText )
        {
        aLogInfo.SetName( emergencyText->Des().
                Left( Min(emergencyText->Length(), KCntMaxTextFieldLength) ) );
        }
    }

// End of File
