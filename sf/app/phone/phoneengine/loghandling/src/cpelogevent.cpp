/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of CPELogEvent class
*
*/


//  INCLUDE FILES
#include "cpelogevent.h"
#include "cpeloginfo.h"
#include "cpeloghandling.h"
#include "cpeloghandlingcommand.h"
#include <talogger.h>
#include <logcli.h>
#include <LogsApiConsts.h>


// EXTERNAL DATA STRUCTURES
//None

// EXTERNAL FUNCTION PROTOTYPES  
//None

// CONSTANTS
const TInt KPhonebookTypeIdLength = 11;

// Defined in \s60\app\contacts\logsui\EngineIncLogsEngConsts.h as KLogEventALS
const TLogFlags KPELogEventALS = 0x4;

const TInt KOneChar = 1;

// MACROS
//None

// LOCAL CONSTANTS AND MACROS
//None

// MODULE DATA STRUCTURES
//None

// LOCAL FUNCTION PROTOTYPES
//None

// FORWARD DECLARATIONS
//None


// ==================== LOCAL FUNCTIONS =====================================

// -----------------------------------------------------------------------------
// CPELogEvent::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPELogEvent* CPELogEvent::NewL
        ( 
        CPELogHandling& aOwner,  // Owner of the object
        CPELogHandlingCommand& aLogHandlingCommand
        )
    {
    TEFLOGSTRING(KTAOBJECT, "LOG CPELogEvent::NewL start.");
    CPELogEvent* self = new ( ELeave ) CPELogEvent( aOwner, aLogHandlingCommand );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TEFLOGSTRING(KTAOBJECT, "LOG CPELogEvent::NewL complete.");
    return self;
    }
  
// Destructor
/*****************************************************
*    Series 60 Customer / LOGENG
*    Series 60  LOGENG API
*****************************************************/
CPELogEvent::~CPELogEvent
        (
        // None
        )
    {
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogEvent::~CPELogEvent()" );
    delete iEvent;
    delete iLogInfo;
    }


// ================= MEMBER FUNCTIONS =======================================
// -----------------------------------------------------------------------------
// CPELogEvent::CPELogEvent
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPELogEvent::CPELogEvent
        ( 
        CPELogHandling& aOwner,
        CPELogHandlingCommand& aLogHandlingCommand
        ) : iOwner( aOwner ),
            iLogHandlingCommand( aLogHandlingCommand ),
            iCompleted( EFalse ),
            iAdded( EFalse ),
            iSaveLeave( EFalse )
    {
    }
// -----------------------------------------------------------------------------
// CPELogEvent::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / LOGENG
*    Series 60  LOGENG API
*****************************************************/
void CPELogEvent::ConstructL
        (
        // None
        )
    {
    iEvent = CLogEvent::NewL();
    ResetEvent( );
    iLogInfo = CPELogInfo::NewL();
    }
    
// -----------------------------------------------------------------------------
// CPELogEvent::Save
// Saves the event to the log.
// -----------------------------------------------------------------------------
//
void CPELogEvent::SaveL()
    {
    iSaveLeave = ETrue;
    // iCopleted flag must be set before the function leave as it is used 
    // in log subsytem recovery procedure in case of leave.
    if ( EPEStateIdle == iLogInfo->CallState() || iLogInfo->ForcedCompletion() )
        {
        iCompleted = ETrue;
        }
        
    if ( !iAdded )
        {
        TEFLOGSTRING2( KTAINT, "LOG CPELogEvent::SaveL() > CPELogHandlingCommand::AddEvent(), callId=%d", iLogInfo->CallId() );  
        SetPhoneNumberId();
        SetEventTypeL();
        SetLogEvent();
        SetDuration();
        SetServiceIdL();
        
        if ( &iLogInfo->ContactLink() )
            {
            SetContactLinkL();
            }
        // event under processing will always be of index 0 in the queued array
        User::LeaveIfError( iLogHandlingCommand.AddEvent( *iEvent ) );
        }
    else
        { 
        TEFLOGSTRING2( KTAINT, "LOG CPELogEvent::SaveL() > CPELogHandlingCommand::ChangeEvent(), callId=%d", iLogInfo->CallId() );            
        SetLogEvent();
        SetDuration();
        User::LeaveIfError( iLogHandlingCommand.ChangeEvent( *iEvent ) );
        }
    // indicates no leave happened in Save function
    iSaveLeave = EFalse;
    }

// -----------------------------------------------------------------------------
// CPELogEvent::UpdateLogInfo
// Update log database entry.
// -----------------------------------------------------------------------------
//
void CPELogEvent::UpdateLogInfoL
        ( 
        const CPELogInfo& aLogInfo // Call information
        ) 
    {
    // CPELogHandling generic loginfo must be replicated to this event
    iLogInfo->CopyL( aLogInfo );
    }

// -----------------------------------------------------------------------------
// CPELogEvent::ResetLogInfo
// Reset log info.
// -----------------------------------------------------------------------------
//
void CPELogEvent::ResetLogInfo() 
    {
    TEFLOGSTRING( KTAINT, "LOG CPELogEvent::ResetLogInfo: start" );  
    iLogInfo->Reset();
    TEFLOGSTRING( KTAINT, "LOG CPELogEvent::ResetLogInfo: complete" ); 
    }
    
// -----------------------------------------------------------------------------
// CPELogEvent::ResetEvent
// Reset event on the idle state.
// -----------------------------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / LOGENG
*    Series 60  LOGENG API
*****************************************************/
void CPELogEvent::ResetEvent()
    {
    iAdded = EFalse;
    iCompleted = EFalse;
    iSaveLeave = EFalse;

    // CLogEvent -variables
    iEvent->SetId( KLogNullId );
    iEvent->SetEventType( KNullUid );
    iEvent->SetRemoteParty( KNullDesC );
    iEvent->SetDirection( KNullDesC );
    iEvent->SetTime( Time::NullTTime() );
    iEvent->SetDurationType( KLogNullDurationType );
    iEvent->SetDuration( KLogNullDuration );
    iEvent->SetStatus( KNullDesC );
    iEvent->SetSubject( KNullDesC );
    iEvent->SetNumber( KNullDesC );
    iEvent->SetContact( KNullContactId );
    iEvent->SetLink( KLogNullLink );
    iEvent->SetDescription( KNullDesC );
    iEvent->ClearFlags( KLogFlagsMask );
    TRAPD( error, iEvent->SetDataL( KNullDesC8 ) );
    if ( error != KErrNone )
        {
        // SetDataL leave should not cause log subsystem to fail. Just log the error.
        TEFLOGSTRING2( KTAERROR, "LOG CPELogEvent::Save()>CPELogHandlingCommand::ResetEvent(), error=%d", error );  
        }
    }

// -----------------------------------------------------------------------------
// CPELogEvent::CallId
// Get call id of the event.
// -----------------------------------------------------------------------------
//
TInt CPELogEvent::CallId
        (
        // None
        )
    {
    return iLogInfo->CallId();
    }

// -----------------------------------------------------------------------------
// CPELogEvent::IsCompleted
// Returns flag if the entry has been completed, ie. no further updates are expected
// -----------------------------------------------------------------------------
//
TBool CPELogEvent::IsCompleted
        (
        // None
        )
    {       
    return iCompleted;
    }

// -----------------------------------------------------------------------------
// CPELogEvent::Added
// iAdded flag is set to ETrue
// -----------------------------------------------------------------------------
//
void CPELogEvent::SetAdded
        (
        // None
        )
    {       
    iAdded = ETrue;
    }

// -----------------------------------------------------------------------------
// CPELogEvent::SaveLeave
// Return value of save leave flag
// -----------------------------------------------------------------------------
//
TBool CPELogEvent::SaveLeave
        (
        // None
        )
    {       
    return iSaveLeave;
    }

// -----------------------------------------------------------------------------
// CPELogEvent::SetPhoneNumberId
// Set event subject.
// -----------------------------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / LOGENG
*    Series 60  LOGENG API
*****************************************************/
void CPELogEvent::SetPhoneNumberId()
    {
    TBuf<KPhonebookTypeIdLength> subject;
    subject.Zero();

    switch ( iLogInfo->PhoneNumberId() )
        {
        case EPEMobileNumber:
            subject.AppendNum( EPEUnknownNumber );
            break;
        case EPETelephoneNumber:
            subject.AppendNum( EPEUnknownNumber );
            break;
        case EPEPager:
            subject.AppendNum( EPEUnknownNumber );
            break;
        case EPEFaxNumber:
            subject.AppendNum( EPEUnknownNumber );
            break; 
        case EPEAssistantNumber:
            subject.AppendNum( EPEUnknownNumber);
            break;
        case EPECarNumber:
            subject.AppendNum( EPEUnknownNumber);
            break;
        default:
            subject.AppendNum( EPEUnknownNumber );
            break;
        }

    if ( subject.Length() > 0 )
        {
        iEvent->SetSubject( subject );
        }
    }

// -----------------------------------------------------------------------------
// CPEGsmLogsEntry::SetEventType
// Set event type.
// -----------------------------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / LOGENG
*    Series 60  LOGENG API
*****************************************************/
void CPELogEvent::SetEventTypeL()
    {
    switch ( iLogInfo->EventType() )
        {
        case CPELogInfo::EPEVoiceEvent:
            
            {
            iEvent->SetEventType( KLogCallEventTypeUid );
            }
            break;
        case CPELogInfo::EPEVideoCallEvent:
            {           
            iEvent->SetEventType( KLogCallEventTypeUid );
            iEvent->SetDataL( KLogsDataFldTag_VT );
            TEFLOGSTRING( KTAINT, "LOG CPELogEvent::SetEventTypeL() video" );                
            }
            break;
        case CPELogInfo::EPEEmergecyEvent:
            {           
            iEvent->SetEventType( KLogCallEventTypeUid );
            iEvent->SetDataL( KLogsDataFldTag_Emergency );
            TEFLOGSTRING( KTAINT, "LOG CPELogEvent::SetEventTypeK() Emergecy" );                
            }
            break;
        case CPELogInfo::EPEVoIPEvent:
            {
            iEvent->SetEventType( KLogCallEventTypeUid );
            iEvent->SetDataL( KLogsDataFldTag_IP );
            TEFLOGSTRING( KTAINT, "LOG CPELogEvent::SetEventTypeL() VoIP" );                
            }
            break;
        case CPELogInfo::EPEUnknownEvent:
        default:
            {
            iEvent->SetEventType( KLogCallEventTypeUid );
            }
            break;
        }
    }

// -----------------------------------------------------------------------------
// CPELogEvent::SetLogEvent
// Set the rest of event information.
// -----------------------------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / LOGENG
*    Series 60  LOGENG API
*****************************************************/
void CPELogEvent::SetLogEvent()
    {
    if( CCCECallParameters::ECCELineTypeAux == iLogInfo->CurrentLine() )
        {
        iEvent->SetFlags( KPELogEventALS );             
        }
    if ( RMobileCall::EMobileOriginated == iLogInfo->CallDirection() )
        {     
        if ( CCCECallParameters::ECCELineTypeAux == iLogInfo->CurrentLine() )
            {
            iEvent->SetDirection( iOwner.LogStringOutAlt( ) );
            }
        else
            {
            iEvent->SetDirection( iOwner.LogStringOut( ) );    
            }
        }
    else if ( RMobileCall::EMobileTerminated == iLogInfo->CallDirection() )
        {
        if ( iLogInfo->MissedCall() && EPEStateIdle == iLogInfo->CallState() )
            {
            iEvent->SetDirection( iOwner.LogStringMissed( ) );
            }
        else
            //Logging on states Connected and Idle
            {
            if ( CCCECallParameters::ECCELineTypeAux == iLogInfo->CurrentLine() )
                {
                iEvent->SetDirection( iOwner.LogStringInAlt( ) );
                }
            else
                {
                iEvent->SetDirection( iOwner.LogStringIn( ) );
                }
            }
        }

    SetRemoteParty( *iEvent, *iLogInfo );
    iEvent->SetStatus( iOwner.LogStringDelivery( ) );
    
    SetRemoteContact( *iEvent, *iLogInfo );
    
    if ( KNullDesC() != iLogInfo->MyAddress() )
        {
        TRAPD( error, SetDataFieldL( KLogsDataFldTag_MA, iLogInfo->MyAddress() ) );
        if ( error )
            {
            TEFLOGSTRING2( KTAERROR, 
                "LOG CPELogEvent::SetLogEvent()>SetDataFieldL(), error=%d", 
                    error )
            }
        }
    }


// -----------------------------------------------------------------------------
// CPELogEvent::SetDuration
// Set the event duration
// -----------------------------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / LOGENG
*    Series 60  LOGENG API
*****************************************************/
void CPELogEvent::SetDuration()
    {
    iEvent->SetDurationType( KLogDurationValid );
    iEvent->SetDuration( static_cast <TLogDuration> ( iLogInfo->Duration().Int() ) );

#ifdef _DEBUG
    if ( EPEStateIdle == iLogInfo->CallState() )
        {
        // debug logging of the event start time.
        TBuf<KPEESDWFormattedTimeLength> formattedTime;
        formattedTime.Zero();
        TRAPD( error, iEvent->Time().FormatL( formattedTime, KPEESDWTimeFormat ) );    
        if( error == KErrNone )
            {
            TEFLOGSTRING2( KTAINT, "LOG CPELogEvent::SetDuration() Idle state > formattedTime: %S", &formattedTime );
            }
        else
            {
            TEFLOGSTRING( KTAERROR, "LOG CPELogEvent::SetDuration() Idle state > time conversion failed!");
            }        
        }
#endif // _DEBUG
    }

// -----------------------------------------------------------------------------
// CPELogEvent::SetServiceIdL
// Set callers service id to log event.
// -----------------------------------------------------------------------------
//
void CPELogEvent::SetServiceIdL()
    {
    HBufC8* eventData = NULL; 
    if ( iEvent->Data().Length() )
        {
        // There are existing data logged.
        eventData = HBufC8::NewLC( iEvent->Data().Length() 
                   + KLogsDataFldNameDelimiter().Length() // separator
                   + KLogsDataFldTag_ServiceId().Length()
                   + KLogsDataFldValueDelimiter().Length()
                   + KTUintCharLength );    
                   
        // Copy existing data to temp buffer.
        eventData->Des().Copy( iEvent->Data() );
   
        // Append tag separator.
        eventData->Des().Append( KLogsDataFldNameDelimiter );    
        }
    else
        {
        // No existing data logged. 
        eventData = HBufC8::NewLC( 
            KLogsDataFldTag_ServiceId().Length()
            + KLogsDataFldValueDelimiter().Length()
            + KTUintCharLength ); 
        }
    
    // Append service id
    eventData->Des().Append( KLogsDataFldTag_ServiceId );
    eventData->Des().Append( KLogsDataFldValueDelimiter );
    eventData->Des().AppendNum( (TInt)iLogInfo->ServiceId() );
    
    iEvent->SetDataL( *eventData );
    TEFLOGSTRING( KTAINT, "LOG CPELogEvent::SetServiceIdL() Service id set ok" );  
    CleanupStack::PopAndDestroy( eventData );
    }

// -----------------------------------------------------------------------------
// Sets callers contact link to log event.
// Contact link data might accidentally contain "\t"(KLogsDataFldNameDelimiter),
// because of this, those "accidents" are stuffed with another "\t". They are
// removed in logs handling side when data is read in.
// -----------------------------------------------------------------------------
//
void CPELogEvent::SetContactLinkL()
    {
    HBufC8* eventData = NULL;
    
    TPtrC8 pDelimiter( iLogInfo->ContactLink() );
    TInt delimiterCount = 0;
    // Check how many "delimiters" data contains so that we can
    // reserve space for data duplication
    while ( KErrNotFound != pDelimiter.Find( KLogsDataFldNameDelimiter ) )
        {
        // Skip first \t to see if there exists another one after that 
        pDelimiter.Set( pDelimiter.Mid( pDelimiter.Find( KLogsDataFldNameDelimiter ) + KOneChar ) );
        ++delimiterCount;
        }
    if ( iEvent->Data().Length() )
        {
        // There is previous data logged
        eventData = HBufC8::NewLC( 
            iEvent->Data().Length() 
            + KLogsDataFldNameDelimiter().Length() // separator
            + KLogsDataFldTag_ContactLink().Length() 
            + KLogsDataFldValueDelimiter().Length()
            + iLogInfo->ContactLink().Length() 
            + delimiterCount );
                   
        // Copy previous data to temp buffer
        eventData->Des().Copy( iEvent->Data() );
   
        // Append tag separator
        eventData->Des().Append( KLogsDataFldNameDelimiter );    
        }
    else
        {
        // No previous data logged. 
        eventData = HBufC8::NewLC( 
                KLogsDataFldTag_ContactLink().Length() 
            + KLogsDataFldValueDelimiter().Length()
            + iLogInfo->ContactLink().Length() 
            + delimiterCount ); 
        }
    
    TPtr8 eventDataPtr( eventData->Des() );
    eventDataPtr.Append( KLogsDataFldTag_ContactLink );
    eventDataPtr.Append( KLogsDataFldValueDelimiter );
    
    if ( delimiterCount )
        {
        // Link data contained unintended \t chars
        TPtrC8 linkPtr( iLogInfo->ContactLink() );
        TInt dataLength = 0;
        // Copy link data between \t chars
        for ( TInt i = 0; i < delimiterCount; i++ )
            {
            // Plus one because we need length, not offset
            dataLength = linkPtr.Find( KLogsDataFldNameDelimiter ) + KOneChar;
            // Append from beginning of data, including \t
            eventDataPtr.Append( linkPtr.Left( dataLength ) );
            // Add another \t
            eventDataPtr.Append( KLogsDataFldNameDelimiter );
            linkPtr.Set( linkPtr.Mid( dataLength ) );
            }
        // Copy rest of link data
        eventDataPtr.Append( linkPtr );
        }
    else
        {
        // Link data didn't contain \t, so normal copy is possible
        eventDataPtr.Append( iLogInfo->ContactLink() );
        }
    
    iEvent->SetDataL( *eventData );
    TEFLOGSTRING( KTAINT, "LOG CPELogEvent::ContactLinkL() Contact link set ok" );
    CleanupStack::PopAndDestroy( eventData );
    }

// -----------------------------------------------------------------------------
// CPELogEvent::SetDataFieldL
// -----------------------------------------------------------------------------
//
void CPELogEvent::SetDataFieldL( const TDesC8& aTag, const TDesC& aFieldData )
    {
    __ASSERT_ALWAYS( aTag.Length() != 0 && aFieldData.Length() != 0 ,
        User::Leave( KErrArgument ) );
    
    if ( KErrNotFound == iEvent->Data().Find( aTag ) )
        {
        HBufC8* eventData = NULL;
        const TInt fieldLength = aTag.Length() 
            + KLogsDataFldValueDelimiter().Length()
            + aFieldData.Length();
        const TInt oldDataFieldLength = iEvent->Data().Length();
        
        if ( oldDataFieldLength != 0 )
            {
            const TInt newDataFieldLength = oldDataFieldLength 
                + KLogsDataFldNameDelimiter().Length()
                + fieldLength;
            
            eventData = HBufC8::NewLC( newDataFieldLength );
            
            // Copy previous data
            eventData->Des().Copy( iEvent->Data() );
            eventData->Des().Append( KLogsDataFldNameDelimiter );
            }
        else
            {
            eventData = HBufC8::NewLC( fieldLength );
            }
        
        // Add a new data field.
        eventData->Des().Append( aTag );
        eventData->Des().Append( KLogsDataFldValueDelimiter );
        eventData->Des().Append( aFieldData );     
        iEvent->SetDataL( *eventData );
        
        CleanupStack::PopAndDestroy( eventData );
        }
    }

// -----------------------------------------------------------------------------
// CPELogEvent::SetRemoteParty
// -----------------------------------------------------------------------------
//
void CPELogEvent::SetRemoteParty( CLogEvent& aEvent, 
        const CPELogInfo& aLogInfo )
    {
    if ( KNullDesC() != aLogInfo.Name() )
        {
        aEvent.SetRemoteParty( aLogInfo.Name() );
        }
    }


// -----------------------------------------------------------------------------
// CPELogEvent::SetRemoteContact
// -----------------------------------------------------------------------------
//
void CPELogEvent::SetRemoteContact( CLogEvent& aEvent, 
        const CPELogInfo& aLogInfo )
    {
    if ( KNullDesC() != aLogInfo.PhoneNumber() )
        {
        aEvent.SetNumber( aLogInfo.PhoneNumber() );
        }
    
    if ( KNullDesC() != aLogInfo.VoipAddress() )
        {
        TRAPD( error, SetDataFieldL( KLogsDataFldTag_URL, aLogInfo.VoipAddress() ) );
        if ( error )
            {
            TEFLOGSTRING2( KTAERROR, 
                "LOG CPELogEvent::SetLogEvent()>SetDataFieldL(), error=%d", error )
            }
        }
    }


// ================= OTHER EXPORTED FUNCTIONS ===============================

//  End of File 

