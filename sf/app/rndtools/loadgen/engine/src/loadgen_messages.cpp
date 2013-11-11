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
*
*/


// INCLUDE FILES
#include "loadgen_messages.h"
#include "loadgen_utils.h"
#include "loadgen.hrh"
#include <loadgen.rsg>
#include <e32math.h>

_LIT(KThreadName, "Messages %d");
_LIT(KMessageSMS, "A test message from LoadGen S60 RnD tool. ");
_LIT( KMessageSMSType, "SMS" );
_LIT( KMessageMMSType, "MMS" );

const TInt KTestMessageLength = 42;
const TInt KDefaultStart = 50;
const TInt KDefaultPeriod = 5000000;

// ===================================== MEMBER FUNCTIONS =====================================

CMessages* CMessages::NewL( TMessageAttributes& aAttributes, TInt aReferenceNumber )
    {
    CMessages* self = new(ELeave) CMessages( aAttributes, aReferenceNumber );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }

// --------------------------------------------------------------------------------------------

CMessages::~CMessages()
    {
    Close();
    }

// --------------------------------------------------------------------------------------------

CMessages::CMessages( TMessageAttributes& aAttributes, 
                       TInt aReferenceNumber ) 
                            : iAttributes( aAttributes )
    {
    iAttributes.iId = aReferenceNumber;
    }

// --------------------------------------------------------------------------------------------

void CMessages::ConstructL()
    {
    CLoadBase::ConstructL();
    
    iType = ELoadGenCmdNewLoadMessages;
    
    TBuf<64> threadName;
    threadName.Format( KThreadName, iAttributes.iId );
    
    // create a thread
    User::LeaveIfError( iThread.Create( threadName, 
                                        ThreadFunction, 
                                        KDefaultStackSize * 2, 
                                        KMinHeapSize, 
                                        1024 * KMinHeapSize, 
                                        (TAny*) &iAttributes ) );
    
    // set priority of the thread
    SetPriority();
    }

// --------------------------------------------------------------------------------------------

TInt CMessages::ThreadFunction(TAny* aThreadArg)
    {
    CTrapCleanup* pC = CTrapCleanup::New();
    CActiveScheduler* pS = new CActiveScheduler;
    CActiveScheduler::Install( pS );

    // start generating load, pass pointer to arguments
    GenerateLoad( *( ( TMessageAttributes* ) aThreadArg ) );

    delete pS;
    delete pC;
    
    return KErrNone;
    }

// --------------------------------------------------------------------------------------------

void CMessages::GenerateLoad( TMessageAttributes& aAttributes )
    {
    CMessageManager* messagesManager = NULL;
    TRAPD( err, messagesManager = CMessageManager::NewL( aAttributes ) );

    if ( err == KErrNone )
        {
        CActiveScheduler::Start();
        }
    delete messagesManager;
    }

// --------------------------------------------------------------------------------------------

void CMessages::Resume()
    {
    CLoadBase::Resume();
    
    iThread.Resume();
    }

// --------------------------------------------------------------------------------------------

void CMessages::Suspend()
    {
    CLoadBase::Suspend();
    
    iThread.Suspend();
    }

// --------------------------------------------------------------------------------------------

void CMessages::SetPriority()
    {
    CLoadBase::SetPriority();
    
    iThread.SetPriority( CLoadGenUtils::SettingItemToThreadPriority( iAttributes.iPriority ) );
    }
    
// --------------------------------------------------------------------------------------------

void CMessages::Close()
    {
    CLoadBase::Close();

    if ( iThread.ExitReason() == 0 ) // check if the thread is still alive
        {
        // signal the thread that it needs to close
        iThread.RequestComplete( iAttributes.iDeathStatus, KErrCancel );

        // wait the thread to die
        TRequestStatus waiter;
        iThread.Logon( waiter );
        User::WaitForRequest( waiter );
        iThread.Close();
        }
    }
    
// --------------------------------------------------------------------------------------------

TPtrC CMessages::Description()
    {
    TBuf<256> buf;
    TBuf<16> prioBuf;
    TBuf<3> typeBuf;
    CLoadGenUtils::SettingItemToThreadDescription( iAttributes.iPriority, prioBuf );
    if ( iAttributes.iMessageType == EMessageTypeSMS )
        {
        typeBuf.Copy( KMessageSMSType );
        }
    else
        {
        typeBuf.Copy( KMessageMMSType );
        }
    _LIT(KMessagesEntry, "[%d] Type=%S prio=%S dest=%S idle=%dms random=%d%%");
    buf.Format( KMessagesEntry,  iAttributes.iId, &typeBuf, &prioBuf, &iAttributes.iDestination, 
                iAttributes.iIdle, iAttributes.iRandomVariance );
    
    return TPtrC( buf );
    }               
// --------------------------------------------------------------------------------------------

CMessageManager* CMessageManager::NewL( TMessageAttributes& aAttributes )
    {
    CMessageManager* self = new(ELeave) CMessageManager( aAttributes );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------------------------
CMessageManager::CMessageManager( TMessageAttributes& aAttributes ) :
    CActive( EPriorityStandard ), iAttributes( aAttributes ), iState( EStateIdle )
    {
    iMessageCounter = 0;
    }

// --------------------------------------------------------------------------------------------
    
CMessageManager::~CMessageManager()
    {
    Cancel();
    delete iMessage;
    if ( iPeriodicTimer )
        {
        iPeriodicTimer->Cancel();
        delete iPeriodicTimer;
        }
    if ( iAttributes.iMessageType == EMessageTypeSMS && iSmsHandler )
        {
        delete iSmsHandler;
        }
    else if ( iMmsHandler )
        {
        delete iMmsHandler;
        }
    }

// --------------------------------------------------------------------------------------------
 
void CMessageManager::ConstructL()
    {
    CActiveScheduler::Add( this );
    
    // set the status as pending
    iStatus = KRequestPending;
    SetActive();
    
    // set the death status pointer point to the request status of this ao
    iAttributes.iDeathStatus = &iStatus;
    if ( iAttributes.iAmount > 0 )
        {
        iState = EStateSend;
        }
    iMessage = HBufC::NewL( iAttributes.iLength );
    CreateMessage();

    // init SMS sender ao
    if ( iAttributes.iMessageType == EMessageTypeSMS )
        {
        iSmsHandler = CSmsHandler::NewL( *this );
        }
    else
        {
        iMmsHandler = CMmsHandler::NewL( *this );
        }
    // start timer    
    iPeriodicTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iPeriodicTimer->Start( KDefaultStart, KDefaultPeriod, 
                        TCallBack( PeriodicTimerCallBack, this ) );
    }

// --------------------------------------------------------------------------------------------
 
void CMessageManager::RunL()
    {
    // request status has completed by the main thread meaning that we need to stop now
    CActiveScheduler::Stop();
    }

// --------------------------------------------------------------------------------------------
 
void CMessageManager::DoCancel()
    {
    }
    
// --------------------------------------------------------------------------------------------

TInt CMessageManager::PeriodicTimerCallBack(TAny* aAny)
    {
    CMessageManager* self = static_cast<CMessageManager*>( aAny );

    self->iPeriodicTimer->Cancel();
    self->HandleMessageSending();

    return KErrNone;
    }

// --------------------------------------------------------------------------------------------

void CMessageManager::CreateMessage()
    {
    // Message body
    TBuf<KTestMessageLength> mToYou( KMessageSMS );
    TPtr ptr = iMessage->Des();

    // Take as many characters as user requested to create the message
    for ( TInt j = 0; j < iAttributes.iLength; j++ )
          {
           for ( TInt k = 0; k < KTestMessageLength;  k++ )
               {
               ptr.Append( mToYou[k] );
               j++;
               if ( j == iAttributes.iLength )
                   {
                   break;
                   }
               }        
           }
    }


// --------------------------------------------------------------------------------------------

void CMessageManager::HandleMessageSending()
    {
    if ( iState == EStateSend && iAttributes.iDestination.Length() <= KTelephoneNumberMaxLength )
        {
        TBool err( KErrNone );        

        // make a new call
        iState = EStateSending;
 
        iMessageCounter++;
        if ( iAttributes.iMessageType == EMessageTypeSMS )
            {
            TRAP( err, iSmsHandler->SendL( iAttributes.iDestination, *iMessage ) );
            if ( !err )
                {
                iState = EStateIdle;
                iPeriodicTimer->Start( CLoadGenUtils::MilliSecondsToMicroSeconds( iAttributes.iIdle,
                            iAttributes.iRandomVariance ), KDefaultPeriod, 
                            TCallBack( PeriodicTimerCallBack, this ) );

                }
            }
        else
            {
            TRAP( err, iMmsHandler->SendL( iAttributes.iDestination, *iMessage ) );            
            if ( err )
                {
                HandleStatus( EStateIdle );
                }
            }
        }
    else
        {
        iState = EStateIdle;
        iPeriodicTimer->Start( CLoadGenUtils::MilliSecondsToMicroSeconds( iAttributes.iIdle,
                    iAttributes.iRandomVariance ), KDefaultPeriod, 
                    TCallBack( PeriodicTimerCallBack, this ) );

        }
    }

// --------------------------------------------------------------------------------------------

void CMessageManager::HandleStatus( TInt /*aErr*/ )
    {
    // timer after wait
    if ( iMessageCounter < iAttributes.iAmount )
        {
        iState = EStateSend;
        iPeriodicTimer->Cancel();
        iPeriodicTimer->Start( CLoadGenUtils::MilliSecondsToMicroSeconds( iAttributes.iIdle,
                    iAttributes.iRandomVariance ), KDefaultPeriod, 
                    TCallBack( PeriodicTimerCallBack, this ) );
        }
    }

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------
// INCLUDE FILES
#include <eikenv.h>
#include <coemain.h>
#include <e32std.h>
#include <msvids.h>
#include <msvstd.h>
#include <smsclnt.h>
#include <smut.h>
#include <mtclreg.h>
#include <txtrich.h>
#include <smscmds.h>
#include <mtmuibas.h>
#include <mtmdef.h>
#include <StringLoader.h>
#include "smutset.h"
#include "smuthdr.h"
 
 
 
// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CSmsHandler::CSmsHandler()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CSmsHandler::CSmsHandler( CMessageManager& aManager )
    : CActive( CActive::EPriorityStandard ), iManager( aManager )
    {
    CActiveScheduler::Add( this );
    }
 
// -----------------------------------------------------------------------------
// CSmsHandler::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSmsHandler::ConstructL()
    {
    // Session to message server is opened asynchronously.
    iSession = CMsvSession::OpenAsyncL( *this );
 
    }
 
// -----------------------------------------------------------------------------
// CSmsHandler::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSmsHandler* CSmsHandler::NewL( CMessageManager& aManager  )
    {
    CSmsHandler* self = NewLC( aManager );
    CleanupStack::Pop( self );
    return self;
    }
 
// -----------------------------------------------------------------------------
// CSmsHandler::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSmsHandler* CSmsHandler::NewLC(  CMessageManager& aManager )
    {
    CSmsHandler* self = new ( ELeave ) CSmsHandler( aManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
 
// ----------------------------------------------------------
// CSmsHandler::~CSmsHandler()
// Destructor.
// ----------------------------------------------------------
//
CSmsHandler::~CSmsHandler()
    {
    Cancel();           // cancel any outstanding request
 
    delete iOperation;
    delete iMtmUiRegistry;
    delete iSmsMtm;
    delete iMtmRegistry;
    delete iSession;    // session must be deleted last
    }
 
// -----------------------------------------------------------------------------
// CSmsHandler::DoCancel()
// Cancels a request.
// -----------------------------------------------------------------------------
//
void CSmsHandler::DoCancel()
    {
    if ( iOperation )
        {
        iOperation->Cancel();
        }
    }
 
// -----------------------------------------------------------------------------
// CSmsHandler::RunL()
// Handles request completion events.
// -----------------------------------------------------------------------------
//
void CSmsHandler::RunL()
    {
    User::LeaveIfError( iStatus != KErrNone );
 
    // Determine the current operations progress.
    // ProgressL returns an 8 bit descriptor.
    TBufC8<KMsvProgressBufferLength> progress( iOperation->ProgressL() );
    _LIT8( KCompare, "KErrNone" );
    User::LeaveIfError( !progress.Compare( KCompare ) );
 
    // The pointer to the current CMsvOperation object is no longer needed.
    delete iOperation;
    iOperation = NULL;
 
    // Determine which request has finished.
    switch ( iState )
        {
        case EWaitingForMoving:
            // Once a message is moved to Outbox it is scheduled for sending.
            ScheduleL();
            break;
 
        case EWaitingForScheduling:
            {
            TMsvEntry entry( iSmsMtm->Entry().Entry() );
            TInt state( entry.SendingState() );
 
            if ( state == KMsvSendStateWaiting || state == KMsvSendStateScheduled)
                {
                // notify the observer that status has changed
                iManager.HandleStatus( iStatus.Int() );
                }
            break;
            }
 
        default:
            break;
        }
    }
 
// -----------------------------------------------------------------------------
// CSmsHandler::HandleSessionEventL()
// Handles notifications of events from the Message Server.
// -----------------------------------------------------------------------------
//
void CSmsHandler::HandleSessionEventL( TMsvSessionEvent aEvent,
                                      TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    switch ( aEvent )
        {
        // Session to server established
        case EMsvServerReady:
            {
            TMsvId serviceId( KUidMsgTypeSMS.iUid ); // SMS service id
 
            // Determine if the event was succesful.
            // ServiceProgress inserts TBuf8 value in progress.
            TBuf8<KBfrLength> progress;
            iSession->ServiceProgress( serviceId, progress );
            _LIT8( KCompare, "KErrNone" );
 
            if ( progress.Compare( KCompare ) )
                {
                // Check that MtmRegistry has not already been accessed.
                if ( !iMtmRegistry )
                    {
                    AccessMtmL();
                    }
                }
            break;
            }
 
        // All other events are ignored.
        default:
            break;
        }
    }
 
// -----------------------------------------------------------------------------
// CSmsHandler::AccessMtmL()
// Access the MTM Registry and create a SMS specific Client MTM instance.
// -----------------------------------------------------------------------------
//
void CSmsHandler::AccessMtmL()
    {
    // Create an MTM Registry object.
    iMtmRegistry = CClientMtmRegistry::NewL( *iSession );
 
    // Create an SMS Client MTM object.
    iSmsMtm = STATIC_CAST( CSmsClientMtm*, iMtmRegistry->NewMtmL( KUidMsgTypeSMS ) );
    }
 
// -----------------------------------------------------------------------------
// CSmsHandler::SendL()
// Starts the process of creating and sending an SMS message.
// -----------------------------------------------------------------------------
//
TBool CSmsHandler::SendL( const TDesC& aRecipientNumber,
                            const TDesC& aMessageText )
    {
    iRecipientNumber = aRecipientNumber;
    iMessageText = aMessageText;
 
    if ( CreateMsgL() )
        {
        return ETrue;
        }
 
    return EFalse;
    }
 
// -----------------------------------------------------------------------------
// CSmsHandler::CreateMsgL()
// Create an SMS message.
// -----------------------------------------------------------------------------
//
TBool CSmsHandler::CreateMsgL()
    {
	if(!iSmsMtm)
		{
		return EFalse;
		}
    // Current entry is the Draft folder.
    iSmsMtm->SwitchCurrentEntryL( KMsvDraftEntryId );
 
    // Create a new SMS message entry as a child of the current context.
    iSmsMtm->CreateMessageL( KUidMsgTypeSMS.iUid );
 
    CMsvEntry& serverEntry = iSmsMtm->Entry();
    TMsvEntry entry( serverEntry.Entry() );
 
    CRichText& body = iSmsMtm->Body();   // the body of the message
    body.Reset();
    // Insert the message text gotten as input from user.
    body.InsertL( 0, iMessageText );
 
    // Message will be sent immediately.
    entry.SetSendingState( KMsvSendStateWaiting );
 
    entry.iDate.UniversalTime(); // insert current time //Solution for HomeTime()

    // Set the SMS message settings for the message.
    CSmsHeader& header = iSmsMtm->SmsHeader();
    CSmsSettings* settings = CSmsSettings::NewL();
    CleanupStack::PushL( settings );
 
    settings->CopyL( iSmsMtm->ServiceSettings() );    // restore settings
    settings->SetDelivery( ESmsDeliveryImmediately ); // to be delivered immediately
    settings->SetDeliveryReport(ETrue);
    header.SetSmsSettingsL( *settings );              // new settings
 
    // Let's check if there is a service center address.
    if ( header.Message().ServiceCenterAddress().Length() == 0 )
        {
        // No, there isn't. We assume there is at least one service center
        // number set and use the default service center number.
        CSmsSettings* serviceSettings = &( iSmsMtm->ServiceSettings() );
 
        // Check if number of service center addresses in the list is null.
 
        //Changed for 3rd Edition specially
        if ( !serviceSettings->ServiceCenterCount() )
            {
            CleanupStack::PopAndDestroy( settings ); 
            return EFalse;     // quit creating the message
            }
 
        else
            {
            //Changed for 3rd Edition specially            
            CSmsNumber* smsCenter= CSmsNumber::NewL();
            CleanupStack::PushL(smsCenter);
            smsCenter->SetAddressL((serviceSettings->GetServiceCenter( 
                            serviceSettings->DefaultServiceCenter())).Address());
            header.Message().SetServiceCenterAddressL( smsCenter->Address() );
            CleanupStack::PopAndDestroy(smsCenter);
            }
        }
 
    CleanupStack::PopAndDestroy( settings );
 
    // Recipient number is displayed also as the recipient alias.
    entry.iDetails.Set( iRecipientNumber );
    // Add addressee.
    iSmsMtm->AddAddresseeL( iRecipientNumber, entry.iDetails );
 
    // Validate message.
    if ( !ValidateL() )
        {
        return EFalse;
        }
 
    entry.SetVisible( ETrue );          // set message as visible
    entry.SetInPreparation( EFalse );   // set together with the visibility flag
    serverEntry.ChangeL( entry );       // commit changes        
    iSmsMtm->SaveMessageL();            // save message
 
    TMsvSelectionOrdering selection;
    CMsvEntry* parentEntry = CMsvEntry::NewL( iSmsMtm->Session(), KMsvDraftEntryId, selection );
    CleanupStack::PushL( parentEntry );
 
    // Move message to Outbox.
    iOperation =parentEntry->MoveL( entry.Id(), KMsvGlobalOutBoxIndexEntryId, iStatus );
 
    CleanupStack::PopAndDestroy( parentEntry );
 
    iState = EWaitingForMoving;
    SetActive();
 
    return ETrue;
  }
 
// -----------------------------------------------------------------------------
// CSmsHandler::ValidateL()
// Validate an SMS message.
// -----------------------------------------------------------------------------
//
TBool CSmsHandler::ValidateL()
    {
	if( !iSmsMtm )
		{
		return EFalse;
		}
    // Empty part list to hold the result.
    TMsvPartList result( KMsvMessagePartNone );
 
    // Validate message body.
    result = iSmsMtm->ValidateMessage( KMsvMessagePartBody );
 
    if ( result != KMsvMessagePartNone )
        {
        return EFalse;
        }
 
    // Validate recipient.
    result = iSmsMtm->ValidateMessage( KMsvMessagePartRecipient );
 
    if ( result != KMsvMessagePartNone )
        {
        return EFalse;
        }
 
    return ETrue;
    }
 
// -----------------------------------------------------------------------------
// CSmsHandler::ScheduleL()
// Schedule an SMS message for sending.
// -----------------------------------------------------------------------------
//
void CSmsHandler::ScheduleL()
    {
	if( !iSmsMtm )
		{
		return;
		}
    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    selection->AppendL( iSmsMtm->Entry().EntryId() ); // add message to selection
 
    // Add entry to task scheduler.
    TBuf8<1> dummyParams;   // dummy parameters needed for InvokeAsyncFunctionL
    iOperation = iSmsMtm->InvokeAsyncFunctionL( ESmsMtmCommandScheduleCopy,
                          *selection, dummyParams, iStatus );
 
    CleanupStack::PopAndDestroy( selection );
 
    iState = EWaitingForScheduling;
    SetActive();
    }
 
// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------



// INCLUDE FILES
#include <mtclreg.h>                  // for CClientMtmRegistry 
#include <msvids.h>                   // for Message type IDs
#include <mmsclient.h>                // for CMmsClientMtm
#include <AknQueryDialog.h>           // for CAknTextQueryDialog
#include <f32file.h>                
#include <coeutils.h>                  // Check the file exist

#include <cmsvmimeheaders.h>        //Attachemt mimeheader
#include <mmsvattachmentmanager.h>    //Attachment manager

// -----------------------------------------------------------------------------
// CMmsHandler::CSmsHandler()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CMmsHandler::CMmsHandler( CMessageManager& aManager ) : iManager( aManager )
    {
    }
 
// -----------------------------------------------------------------------------
// CMmsHandler::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmsHandler::ConstructL()
    {
    // Session to message server is opened asynchronously.
    iSession = CMsvSession::OpenAsyncL( *this );
 
    }
 
// -----------------------------------------------------------------------------
// CMmsHandler::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmsHandler* CMmsHandler::NewL( CMessageManager& aManager  )
    {
    CMmsHandler* self = NewLC( aManager );
    CleanupStack::Pop( self );
    return self;
    }
 
// -----------------------------------------------------------------------------
// CMmsHandler::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmsHandler* CMmsHandler::NewLC(  CMessageManager& aManager )
    {
    CMmsHandler* self = new ( ELeave ) CMmsHandler( aManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
 
// ----------------------------------------------------------
// CMmsHandler::~CSmsHandler()
// Destructor.
// ----------------------------------------------------------
//
CMmsHandler::~CMmsHandler()
    {
    delete iMmsMtm;
    delete iMtmReg;
    delete iSession;    // session must be deleted last (and constructed first)
    }

/*
-----------------------------------------------------------------------------
    CMmsHandler::CompleteConstructL()
    Creates client MTM registry when session is ready for use. 
    This completes model construction and is called after 'server
    ready' event is received after async opening of CMsvSession.
-----------------------------------------------------------------------------
*/
void CMmsHandler::CompleteConstructL()
    {
    // We get a MtmClientRegistry from our session
    // this registry is used to instantiate new mtms.
    iMtmReg = CClientMtmRegistry::NewL( *iSession );
    iMmsMtm = (CMmsClientMtm*)iMtmReg->NewMtmL( KUidMsgTypeMultimedia );
    }
   
/*
-----------------------------------------------------------------------------
    CMmsHandler::HandleSessionEventL()

    Receives session events from observer and calls event handling functions. 
    Note that if additional session event handlers are defined 
    in the session, they are called before this function (as this is the
    main session observer).
    The type of event is indicated by the value of aEvent. The 
    interpretation of the TAny arguments depends on this type. 
-----------------------------------------------------------------------------
*/
void CMmsHandler::HandleSessionEventL(TMsvSessionEvent aEvent, 
                                        TAny* /*aArg1*/, 
                                        TAny* /*aArg2*/, 
                                        TAny* /*aArg3*/)
    {
    switch ( aEvent )
        {
        // This event tells us that the session has been opened
        case EMsvServerReady:
            {
            CompleteConstructL();       // Construct the mtm registry & MMS mtm
            break;
            }
        default:
            {
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CSmsHandler::SendL()
// Starts the process of creating and sending an SMS message.
// -----------------------------------------------------------------------------
//
TBool CMmsHandler::SendL( const TDesC& aRecipientNumber,
                            const TDesC& aMessageText )
    {
    TInt result( EFalse );
    
    iRecipientNumber = aRecipientNumber;
    iMessageText = aMessageText;
 
    if ( CreateMsgL() )
        {
        if ( SendMessageL() )
            {
            result = ETrue;
            }
        }
 
    return result;
    }

/*
-----------------------------------------------------------------------------
    CMmsHandler::CreateMsgL()
    Creates a new message server entry and set up default values.
    In case the attachment file does not found method return EFalse
    otherwise ETrue.
    There are differenses how to add attachment file between 2nd and 3rd edition. 
-----------------------------------------------------------------------------
*/
TBool CMmsHandler::CreateMsgL()
    {

    // - CMsvEntry accesses and acts upon a particular Message Server entry.
    // - NewL() does not create a new entry, but simply a new object to access an existing entry.
    // - It takes in as parameters the client's message server session,
    //   ID of the entry to access and initial sorting order of the children of the entry. 
    CMsvEntry* entry = CMsvEntry::NewL( *iSession, 
                                        KMsvGlobalOutBoxIndexEntryId, 
                                        TMsvSelectionOrdering() );
    CleanupStack::PushL( entry );

    // Set context to the parent folder (Outbox)
    iMmsMtm->SwitchCurrentEntryL( entry->EntryId() );
    
    // Create new message in the parent folder (Outbox) and set it as the current context.
    iMmsMtm->CreateMessageL( iMmsMtm->DefaultServiceL() );

    CleanupStack::PopAndDestroy( entry ); 
    
    // Setting recipients
    // use this to add the "To" recipients.
    iMmsMtm->AddAddresseeL( iRecipientNumber );
    
    //Setting message subject
    _LIT(KMessageSubject, "MMS Message");
    iMmsMtm->SetSubjectL( KMessageSubject );
    
    // add message text
    SetMessageBodyL();
   
    TMsvEntry ent = iMmsMtm->Entry().Entry();
    // Set InPreparation to false
    ent.SetInPreparation( EFalse );
    ent.SetVisible( ETrue );            // mark as visible, after this the message can be seen in Outbox and, after sending, in Sent folder.
  
    iMmsMtm->Entry().ChangeL( ent );    // Commit changes
    
    //Save the changes
    iMmsMtm->SaveMessageL();
    
    return ETrue;
    }

//---------------------------------------------------------------------------------
void CMmsHandler::SetMessageBodyL()
    {
    _LIT (KFilenameText, "msg.txt");
    CMsvStore* store = iMmsMtm->Entry().EditStoreL();
    CleanupStack::PushL( store );

    TMsvAttachmentId attachmentId = KMsvNullIndexEntryId;

    iMmsMtm->CreateTextAttachmentL ( *store,
                                     attachmentId,
                                     iMessageText,
                                     KFilenameText );

    store->CommitL();

    CleanupStack::PopAndDestroy( store );
    }

/* 
-----------------------------------------------------------------------------
    CMmsHandler::SendMessageL()
    Sends the message.
    Return values: ETrue or EFalse
-----------------------------------------------------------------------------
*/
TBool CMmsHandler::SendMessageL()
    {

    // Start sending the message via the Server MTM to the MMS server
    CMsvOperationWait* wait = CMsvOperationWait::NewLC();
    wait->iStatus = KRequestPending;
    CMsvOperation* op = NULL;
       op = iMmsMtm->SendL( wait->iStatus );
    wait->Start();
    CleanupStack::PushL( op );
    CActiveScheduler::Start();

    // The following is to ignore the completion of other active objects. It is not
    // needed if the app has a command absorbing control.
    while( wait->iStatus == KRequestPending )
        {
        CActiveScheduler::Start();
        }

    iManager.HandleStatus( wait->iStatus.Int() );
    CleanupStack::PopAndDestroy(2); // op, wait
    
    return ETrue;
    }
// End of File
