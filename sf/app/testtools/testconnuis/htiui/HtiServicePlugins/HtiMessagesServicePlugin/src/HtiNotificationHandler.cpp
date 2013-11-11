/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of creating notifications.
*
*/


// INCLUDE FILES
#include "HtiMessagesServicePlugin.h"
#include "HtiNotificationHandler.h"

#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>

#include <aknSDData.h>          //CAknSDData
#include <avkon.rsg>            //R_AVKON_SOFTKEYS_LISTEN_EXIT

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS


// MACROS
#define KSpeedDialViewId TVwsViewId( TUid::Uid( 0x1000590A ), TUid::Uid( 0x02 ) )
#define R_NCN_VOICE_MAIL_NOTE                     0x283d012
#define R_NCN_VOICE_MAILS_UNK_AMOUNT              0x283d01a
#define R_NCN_VOICE_MAIL_GROUPED                  0x283d017
#define R_NCN_VOICE_MAILS_UNK_AMOUNT_GROUPED      0x283d01b

// LOCAL CONSTANTS AND MACROS

_LIT8( KErrorInvalidParameters, "Invalid command parameters" );
_LIT8( KErrorCreateVoiceMessageNotificationFailed, "Creating voice message notification failed" );
_LIT8( KErrorClearNotificationFailed, "Clearing notification failed" );
_LIT8( KErrorUnrecognizedCommand, "Unrecognized command" );

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHtiNotificationHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CHtiNotificationHandler* CHtiNotificationHandler::NewL()
    {
    HTI_LOG_FUNC_IN( "CHtiNotificationHandler::NewL" );
    CHtiNotificationHandler* self = new (ELeave) CHtiNotificationHandler();
    CleanupStack::PushL ( self );
    self->ConstructL();
    CleanupStack::Pop();
    HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::NewL" );
    return self;
    }


// ----------------------------------------------------------------------------
// CHtiNotificationHandler::CHtiNotificationHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
CHtiNotificationHandler::CHtiNotificationHandler()
    : CActive( CActive::EPriorityStandard ),
      iMsgWaiting(),
      iMsgWaitingPckg( iMsgWaiting ),
      iVoiceMailsInLine1(0)
    {
    HTI_LOG_FUNC_IN( "CHtiNotificationHandler::CHtiNotificationHandler" );
    CActiveScheduler::Add( this );
    HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::CHtiNotificationHandler" );
    }


// -----------------------------------------------------------------------------
// CHtiNotificationHandler::~CHtiNotificationHandler
// Destructor.
// -----------------------------------------------------------------------------
CHtiNotificationHandler::~CHtiNotificationHandler()
    {
    HTI_LOG_FUNC_IN( "CHtiNotificationHandler::~CHtiNotificationHandler" );
    Cancel();
    iMobilePhone.Close();
    iTelServer.Close();
    iReqQueue.Close();
    HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::~CHtiNotificationHandler" );
    }


// -----------------------------------------------------------------------------
// CHtiNotificationHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CHtiNotificationHandler::ConstructL()
    {
    HTI_LOG_FUNC_IN( "CHtiNotificationHandler::ConstructL" );
    
    TInt err;       
    
    err = iTelServer.Connect();
    HTI_LOG_FORMAT("RTelServer::Connect() returns %d",err);
    User::LeaveIfError( err );

    err = iTelServer.GetPhoneInfo( 0, iPhoneInfo );
    HTI_LOG_FORMAT("RTelServer::GetPhoneInfo() returns %d",err);
    User::LeaveIfError( err ); 
        
    err = iMobilePhone.Open( iTelServer, iPhoneInfo.iName ); 
    HTI_LOG_FORMAT("RTelServer::Open() returns %d",err);
    User::LeaveIfError( err );

    HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::ConstructL" );
    }


// -----------------------------------------------------------------------------
// CHtiNotificationHandler::SetDispatcher
// Sets the dispatcher pointer.
// -----------------------------------------------------------------------------

void CHtiNotificationHandler::SetDispatcher( MHtiDispatcher* aDispatcher )
    {
    HTI_LOG_FUNC_IN( "CHtiNotificationHandler::SetDispatcher" );
    iDispatcher = aDispatcher;
    HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::SetDispatcher" );
    }


// -----------------------------------------------------------------------------
// CHtiNotificationHandler::ProcessMessageL
// Parses the received message and calls handler functions.
// -----------------------------------------------------------------------------
void CHtiNotificationHandler::ProcessMessageL( const TDesC8& aMessage,
                                              THtiMessagePriority /*aPriority*/ )
    {
    HTI_LOG_FUNC_IN( "CHtiNotificationHandler::ProcessMessageL" );

    // Zero length message and command code validity already checked
    // in HtiMessagesServicePlugin.

    if ( aMessage[0] == CHtiMessagesServicePlugin::ECreateVoiceMessageNotification )
        {
        TRAPD(err, HandleCreateVoiceMessageNotificationL(aMessage));
        if ( err != KErrNone )
            {
            SendErrorMessageL( err, KErrorCreateVoiceMessageNotificationFailed );
            }
        }

    else if ( aMessage[0] == CHtiMessagesServicePlugin::EClearAllNotifications)
        {
        TRAPD(err, HandleClearAllNotificationsL(aMessage));
        if( err!= KErrNone )
            {
            SendErrorMessageL( err, KErrorClearNotificationFailed );
            }
        }
    else
        {
        SendErrorMessageL( KErrArgument, KErrorUnrecognizedCommand );
        }

    HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::ProcessMessageL" );
    }


void CHtiNotificationHandler::RunL()
    {
    HTI_LOG_FUNC_IN( "CHtiNotificationHandler::RunL" );
    if(iReqQueue.Count())
        {
        TNcnReqQueueElement elem = iReqQueue[0];
        iReqQueue.Remove(0);
        if(elem.iCmd == ENcnSetIndicator)
            {
            SetIndicator(elem.iIndicator, elem.iMsgCount, elem.iEnable);
            }
        else if(elem.iCmd == ENcnResetIndicator)
            {
            ResetIndicator();
            }
        }
    HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::RunL" );
    }

void CHtiNotificationHandler::DoCancel()
    {
    HTI_LOG_FUNC_IN( "CHtiNotificationHandler::DoCancel" );
    if( IsActive() )
        {
        iMobilePhone.CancelAsyncRequest( EMobilePhoneSetIccMessageWaitingIndicators );
        }
    HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::DoCancel" );
    }

TInt CHtiNotificationHandler::RunError(TInt aError)
    {
    HTI_LOG_FUNC_IN( "CHtiNotificationHandler::RunError" );
    HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::RunError" );
    return aError;
    }

CAknSoftNotificationParameters* CHtiNotificationHandler::CreateNotificationParametersLC(TBool aAmountKnown)
    {
        HTI_LOG_FUNC_IN( "CHtiNotificationHandler::CreateNotificationParametersLC" );
        const TUid KNcnDefaultMessageUid = { 1 };
        _LIT( KNcnResourceFile, "z:\\resource\\Ncnlist.rSC" );
        const TInt KNcnNotificationPriority = 2200;

        TInt noteId = aAmountKnown ? R_NCN_VOICE_MAIL_NOTE : R_NCN_VOICE_MAILS_UNK_AMOUNT;
        TInt groupId = aAmountKnown ? R_NCN_VOICE_MAIL_GROUPED : R_NCN_VOICE_MAILS_UNK_AMOUNT_GROUPED;
        TUid customMessageId = KNcnDefaultMessageUid; // default
        _LIT8( KNcnExternalLaunch, "outside" );
        CAknSoftNotificationParameters* ret = CAknSoftNotificationParameters::NewL(
                KNcnResourceFile,
                noteId,
                KNcnNotificationPriority,
                R_AVKON_SOFTKEYS_LISTEN_EXIT,
                CAknNoteDialog::ENoTone,
                KSpeedDialViewId,
                customMessageId,
                EAknSoftkeyListen,
                KNcnExternalLaunch );
        ret->SetGroupedTexts( groupId );
        CleanupStack::PushL( ret );
        HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::CreateNotificationParametersLC" );
        return ret;
    } 

void CHtiNotificationHandler::ShowNewItemsL( CAknSoftNotifier* aNotifier, 
        const TAknSoftNotificationType aNotificationType, const TInt aAmount )
    {
    HTI_LOG_FUNC_IN( "CHtiNotificationHandler::ShowNewItemsL" );
    TInt KVMMaximumNumber = 0xfe;
    TBool amountKnown = ( aAmount <= KVMMaximumNumber );

    // fuzzy voicemail note and normal voice mail note should never exist at the same time. 
    // Always cancel the other before adding new note.
    if ( aNotificationType == EVoiceMailNotification
        || aNotificationType == ESeparateVoiceMailOnLine1Notification
        || aNotificationType == ESeparateVoiceMailOnLine2Notification )
        {
        CAknSoftNotificationParameters* params =
            CreateNotificationParametersLC(!amountKnown);
  
        aNotifier->CancelCustomSoftNotificationL( *params );
        CleanupStack::PopAndDestroy( params );
        }

    // create notification parameters
    CAknSoftNotificationParameters* params =
        CreateNotificationParametersLC(amountKnown);
        
    if( aAmount <= 0 )
        {
        aNotifier->CancelCustomSoftNotificationL( *params );
        }
    else
        {
        aNotifier->SetCustomNotificationCountL( *params, aAmount );
        }

    // destroy parameters
    CleanupStack::PopAndDestroy( params );
    HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::ShowNewItemsL" );
    }

void CHtiNotificationHandler::SetNotificationL(TUint aAmount)
    {
    HTI_LOG_FUNC_IN( "CHtiNotificationHandler::SetNotificationL" );
    
    //Convert to avkon type
    TAknSoftNotificationType aknType = EVoiceMailNotification;

    //Get the secondary display id for this note
    TInt dialogID = 5; //SecondaryDisplay::ECmdVoiceMailNotification in NcnSecondaryDisplayAPI.h

    // The category UID for the messages in this header file.
    //
    const TUid KCatNcnList = {0x100058F1};
    const TUint KNcnExternalizedNumberMaxLength = 8;
    
    // Amount needs to be stored to "additional parameter"
    TBuf8<KNcnExternalizedNumberMaxLength> additionalData;

    additionalData.Num(aAmount);
    
    // Encapsulate the sent data
    CAknSDData* sdData = NULL;
    TRAPD( status, sdData = CAknSDData::NewL(
            KCatNcnList, 
            dialogID, 
            additionalData) );
    
    // Create notifier instance
    CAknSoftNotifier* notifier = CAknSoftNotifier::NewL();
    CleanupStack::PushL(notifier);
    
    // Send data to cover ui    
    if ( status == KErrNone && sdData != NULL)
        {
        notifier->SetSecondaryDisplayData( sdData ); //Takes ownership 
        }
    
    // If there are problems with notifier, just catch the error and 
    // try to finish the other processes.
    TRAP( status, ShowNewItemsL( notifier, aknType, aAmount ) );
    
    CleanupStack::PopAndDestroy(notifier);
    HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::SetNotificationL" );
    }

void CHtiNotificationHandler::SetIndicator(const TNcnIndicator aIndicator, TUint aMsgCount, TBool aEnable)
    {
    HTI_LOG_FUNC_IN( "CHtiNotificationHandler::SetIndicator" );
       
    TInt aId = RMobilePhone::KDisplayVoicemailActive;
    if( aEnable )       
        {
        iMsgWaiting.iDisplayStatus |= aId;          
        }
    else
        {
        iMsgWaiting.iDisplayStatus &= ~ aId;                    
        } 
    iMsgWaiting.iVoiceMsgs = aMsgCount;
    if(IsActive())
        {
        TNcnReqQueueElement elem(ENcnSetIndicator, aIndicator, aMsgCount, aEnable );
        iReqQueue.Append( elem );
        }
    else
        {
        iStatus = KRequestPending;
        iMobilePhone.SetIccMessageWaitingIndicators( iStatus, iMsgWaitingPckg );
        HTI_LOG_FORMAT("iStatus: %d", iStatus.Int());
        SetActive();
        }
    HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::SetIndicator" );
    }

void CHtiNotificationHandler::ResetIndicator()
    {
    HTI_LOG_FUNC_IN( "CHtiNotificationHandler::ResetIndicator" );
    if( IsActive() )
        {
        TNcnReqQueueElement elem(ENcnResetIndicator, ENcnIndicatorVMLine1, 0, EFalse );
        iReqQueue.Append( elem );
        return;
        }
    iMsgWaiting.iVoiceMsgs = 0;
    iMsgWaiting.iDisplayStatus = 0;
    iMobilePhone.SetIccMessageWaitingIndicators( iStatus, iMsgWaitingPckg );
    SetActive();
    HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::ResetIndicator" );
    }

// ----------------------------------------------------------------------------
// CHtiNotificationHandler::HandleCreateVoiceMessageNotificationL
// Creates new voice message notification.
// ----------------------------------------------------------------------------
void CHtiNotificationHandler::HandleCreateVoiceMessageNotificationL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiNotificationHandler::HandleCreateVoiceMessageNotificationL" );
    
    if ( aData.Length() == 1 )
        {
        HTI_LOG_TEXT( "no command parameter, using default value 1");
        iVoiceMailsInLine1 = 1;
        }    
    else if(aData.Length() == 2) //1-byte command parameter
        {
        iVoiceMailsInLine1 = aData[1];
        HTI_LOG_FORMAT("%d new voice messages", iVoiceMailsInLine1);
        }
    else
        {
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::HandleCreateVoiceMessageNotificationL" );
        return;
        }
    
    ResetIndicator();
    SetNotificationL(iVoiceMailsInLine1);    
    SetIndicator(ENcnIndicatorVMLine1, iVoiceMailsInLine1, iVoiceMailsInLine1 ? ETrue : EFalse);    
    SendOkMsgL( KNullDesC8 );    
    HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::HandleCreateVoiceMessageNotificationL" );
    }

// ----------------------------------------------------------------------------
// CHtiNotificationHandler::HandleClearAllNotificationsL
// Clears all message notifications.
// ----------------------------------------------------------------------------
void CHtiNotificationHandler::HandleClearAllNotificationsL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiNotificationHandler::HandleClearAllNotificationsL" );
    if ( aData.Length() != 1 )
        {
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::HandleClearAllNotificationsL" );
        return;
        }

    CAknSoftNotifier* notifier = CAknSoftNotifier::NewL();
    CleanupStack::PushL(notifier);
    CAknSoftNotificationParameters* params = CreateNotificationParametersLC( ETrue );  
    notifier->CancelCustomSoftNotificationL( *params ); 
    CleanupStack::PopAndDestroy(2);//param, notifier

    ResetIndicator();
    
    SendOkMsgL( KNullDesC8 ); 
    HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::HandleClearAllNotificationsL" );
    }

// ----------------------------------------------------------------------------
// CHtiNotificationHandler::SendOkMsgL
// Helper function for sending response messages.
// ----------------------------------------------------------------------------
void CHtiNotificationHandler::SendOkMsgL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiNotificationHandler::SendOkMsgL" );
    User::LeaveIfNull( iDispatcher );
    HBufC8* temp = HBufC8::NewL( aData.Length() + 1 );
    TPtr8 response = temp->Des();
    response.Append( ( TChar ) CHtiMessagesServicePlugin::EResultOk );
    response.Append( aData );
    User::LeaveIfError( iDispatcher->DispatchOutgoingMessage(
        temp, KHtiMessagesServiceUid ) );
    HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::SendOkMsgL" );
    }


// ----------------------------------------------------------------------------
// CHtiNotificationHandler::SendErrorMessageL
// Helper function for sending error response messages.
// ----------------------------------------------------------------------------
void CHtiNotificationHandler::SendErrorMessageL( TInt aError, const TDesC8& aDescription )
    {
    HTI_LOG_FUNC_IN( "CHtiNotificationHandler::SendErrorMessageL" );
    User::LeaveIfNull( iDispatcher );
    User::LeaveIfError( iDispatcher->DispatchOutgoingErrorMessage(
        aError, aDescription, KHtiMessagesServiceUid ) );
    HTI_LOG_FUNC_OUT( "CHtiNotificationHandler::SendErrorMessageL" );
    }

// End of file
