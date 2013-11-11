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



#include <cntdef.h>

#include "engine.h"
#include "enginewrapper.h"
#include "creator_log.h" 
#include "creator_traces.h"

static const TInt KLogsFieldLength = 256;

//_LIT(KCreatorLogsPrefixName, "CR_");

//----------------------------------------------------------------------------

CLogsParameters::CLogsParameters()
    {
    LOGSTRING("Creator: CLogsParameters::CLogsParameters");

    iPhoneNumber = HBufC::New(KLogsFieldLength);
    /*iMissedCallPhoneNumber = HBufC::New(KLogsFieldLength);
    iReceivedCallPhoneNumber = HBufC::New(KLogsFieldLength);
    iDialledNumberPhoneNumber = HBufC::New(KLogsFieldLength);*/
    }

CLogsParameters::~CLogsParameters()
    {
    LOGSTRING("Creator: CLogsParameters::~CLogsParameters");

    delete iPhoneNumber;
    /*delete iDialledNumberPhoneNumber;
    delete iReceivedCallPhoneNumber;
    delete iMissedCallPhoneNumber;*/
    }

//----------------------------------------------------------------------------

CCreatorLogs* CCreatorLogs::NewL(CCreatorEngine* aEngine)
    {
    CCreatorLogs* self = CCreatorLogs::NewLC(aEngine);
    CleanupStack::Pop(self);
    return self;
    }

CCreatorLogs* CCreatorLogs::NewLC(CCreatorEngine* aEngine)
    {
    CCreatorLogs* self = new (ELeave) CCreatorLogs;
    CleanupStack::PushL(self);
    self->ConstructL(aEngine);
    return self;
    }

CCreatorLogs::CCreatorLogs() : CActive(0)
    {
    iEntriesToBeCreated = 1;
    }

void CCreatorLogs::ConstructL(CCreatorEngine* aEngine)
    {
    LOGSTRING("Creator: CCreatorLogs::ConstructL");

    iEngine = aEngine;

    iLogClient = CLogClient::NewL(CEikonEnv::Static()->FsSession());
    iLogEvent = CLogEvent::NewL();

	CActiveScheduler::Add(this); // needed by CActive
    }

CCreatorLogs::~CCreatorLogs()
    {
    LOGSTRING("Creator: CCreatorLogs::~CCreatorLogs");

    Cancel(); //from CActive

    if ( iEngine && iEntryIds.Count() )
        {
        TRAP_IGNORE( iEngine->WriteEntryIdsToStoreL( iEntryIds, KUidDictionaryUidLogs ) );
        }
    iEntryIds.Reset();
    iEntryIds.Close();
    
    delete iLogEvent;
    delete iLogClient;

    if (iParameters)
        delete iParameters;
    }

//----------------------------------------------------------------------------

void CCreatorLogs::QueryDialogClosedL(TBool aPositiveAction, TInt aUserData)
    {
    LOGSTRING("Creator: CCreatorModuleBase::QueryDialogClosedL");
    
    if( aPositiveAction == EFalse )
        {
        iEngine->ShutDownEnginesL();
        return;
        }
    
    const TDesC* showText = &KSavingText;
    TBool finished(EFalse);
    TBool retval(ETrue);
    switch(aUserData)
        {
        case ECreatorLogsDelete:
            showText = &KDeletingText;
            iEntriesToBeCreated = 1;
            finished = ETrue;
            break;
        case ECreatorLogsStart:
            finished = ETrue;
            break;
        default:
            //some error
            retval = EFalse;
            break;
        }
    if( retval == EFalse )
        {
        iEngine->ShutDownEnginesL();
        }
    else if( finished )
        {
        // add this command to command array
        iEngine->AppendToCommandArrayL(iCommand, NULL, iEntriesToBeCreated);
        // started exucuting commands
        iEngine->ExecuteFirstCommandL( *showText );
        }
    }
    
//----------------------------------------------------------------------------

TBool CCreatorLogs::AskDataFromUserL(TInt aCommand)
    {
    LOGSTRING("Creator: CCreatorLogs::AskDataFromUserL");
    
    iCommand = aCommand;
    
    if ( aCommand == ECmdDeleteLogs )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog( _L("Delete all log entries?"), this, ECreatorLogsDelete );
        }
    else if ( aCommand ==  ECmdDeleteCreatorLogs )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog( _L("Delete all log entries created with Creator?"), this, ECreatorLogsDelete );
        }
    return iEngine->GetEngineWrapper()->EntriesQueryDialog( &iEntriesToBeCreated, _L("How many entries to create?"), EFalse, this, ECreatorLogsStart );
    }

//----------------------------------------------------------------------------

void CCreatorLogs::RunL()
	{
    LOGSTRING("Creator: CCreatorLogs::RunL");

    // log writing finished, stop the async waiting
    iLogWriteWait.AsyncStop();
    }

//----------------------------------------------------------------------------

void CCreatorLogs::DoCancel()
	{
    }

//----------------------------------------------------------------------------

TInt CCreatorLogs::CreateMissedCallEntryL(CLogsParameters *aParameters)
    {
    LOGSTRING("Creator: CCreatorLogs::CreateMissedCallEntryL");

    // clear any existing parameter definations
    delete iParameters;
    iParameters = NULL;
    
    CLogsParameters* parameters = aParameters;
    
    // random data needed if no predefined data available
    if (!parameters)
        {
        iParameters = new(ELeave) CLogsParameters;
        parameters = iParameters;

        /*iParameters->iMissedCallPhoneNumber->Des().Zero();
        iParameters->iMissedCallPhoneNumber->Des().AppendNum( iEngine->RandomNumber(1000000, 9999999) );

        iParameters->iMissedCallEventTime.UniversalTime();
        iParameters->iMissedCallEventTime -= TTimeIntervalMinutes( iEngine->RandomNumber(12000) );*/
        parameters->iPhoneNumber->Des().Zero();
        parameters->iPhoneNumber->Des().AppendNum( iEngine->RandomNumber(1000000, 9999999) );
        
        parameters->iEventTime.UniversalTime();
        parameters->iEventTime -= TTimeIntervalMinutes( iEngine->RandomNumber(12000) );

        }
    
    // defaults
    iLogEvent->SetId( KLogNullId );
    iLogEvent->SetEventType( KNullUid );
    iLogEvent->SetRemoteParty( KNullDesC );
    iLogEvent->SetDirection( KNullDesC );
    iLogEvent->SetTime( Time::NullTTime() );
    iLogEvent->SetDurationType( KLogNullDurationType );
    iLogEvent->SetDuration( KLogNullDuration );
    iLogEvent->SetStatus( KNullDesC );
    iLogEvent->SetSubject( KNullDesC );
    iLogEvent->SetNumber( KNullDesC );
    iLogEvent->SetContact( KNullContactId );
    iLogEvent->SetLink( KLogNullLink );
    iLogEvent->SetDescription( KNullDesC );
    iLogEvent->SetFlags( KLogNullFlags );
    iLogEvent->SetEventType(KLogCallEventTypeUid);
    iLogEvent->SetSubject(_L("A"));

    // set direction
    TLogString missed;
    iLogClient->GetString(missed, R_LOG_DIR_MISSED);
    iLogEvent->SetDirection(missed);

    // set duration
    iLogEvent->SetDurationType( KLogDurationNone );
    TLogDuration duration = 0;
    iLogEvent->SetDuration(duration);

    // set the phone number
    //iLogEvent->SetNumber( iParameters->iMissedCallPhoneNumber->Des() );
    iLogEvent->SetNumber( parameters->iPhoneNumber->Des() );

    // set event time
    //iLogEvent->SetTime( iParameters->iMissedCallEventTime );
    iLogEvent->SetTime( parameters->iEventTime );

    // start writing
    __ASSERT_ALWAYS(!IsActive(), User::Panic(_L("LogWriter"), 150));
	iLogClient->AddEvent(*iLogEvent, iStatus);
	SetActive();

    // wait till adding has finished
    iLogWriteWait.Start();

    // id has been generated, store it for being able to delete
    // entries created only with Creator
    TRAP_IGNORE( iEntryIds.AppendL( iLogEvent->Id() ) );

    return iStatus.Int();  // should be KErrNone if writing was succesful
    }

//----------------------------------------------------------------------------

TInt CCreatorLogs::CreateReceivedCallEntryL(CLogsParameters *aParameters)
    {
    LOGSTRING("Creator: CCreatorLogs::CreateReceivedCallEntryL");

    // clear any existing parameter definations
    delete iParameters;
    iParameters = NULL;
    
    CLogsParameters* parameters = aParameters;
        
    // random data needed if no predefined data available
    if (!parameters)
        {
        iParameters = new(ELeave) CLogsParameters;
        parameters = iParameters;

        /*iParameters->iReceivedCallPhoneNumber->Des().Zero();
        iParameters->iReceivedCallPhoneNumber->Des().AppendNum( iEngine->RandomNumber(1000000, 9999999) );

        iParameters->iReceivedCallEventTime.UniversalTime();
        iParameters->iReceivedCallEventTime -= TTimeIntervalMinutes( iEngine->RandomNumber(12000) );

        iParameters->iReceivedCallDuration = iEngine->RandomNumber(7200);*/
        parameters->iPhoneNumber->Des().Zero();
        parameters->iPhoneNumber->Des().AppendNum( iEngine->RandomNumber(1000000, 9999999) );
        
        parameters->iEventTime.UniversalTime();
        parameters->iEventTime -= TTimeIntervalMinutes( iEngine->RandomNumber(12000) );
        
        parameters->iDuration = iEngine->RandomNumber(7200);
        }
   
    // defaults
    iLogEvent->SetId( KLogNullId );
    iLogEvent->SetEventType( KNullUid );
    iLogEvent->SetRemoteParty( KNullDesC );
    iLogEvent->SetDirection( KNullDesC );
    iLogEvent->SetTime( Time::NullTTime() );
    iLogEvent->SetDurationType( KLogNullDurationType );
    iLogEvent->SetDuration( KLogNullDuration );
    iLogEvent->SetStatus( KNullDesC );
    iLogEvent->SetSubject( KNullDesC );
    iLogEvent->SetNumber( KNullDesC );
    iLogEvent->SetContact( KNullContactId );
    iLogEvent->SetLink( KLogNullLink );
    iLogEvent->SetDescription( KNullDesC );
    iLogEvent->SetFlags( KLogNullFlags );
    iLogEvent->SetEventType(KLogCallEventTypeUid);
    iLogEvent->SetSubject(_L("A"));

    // set direction
    TLogString incoming;
    iLogClient->GetString(incoming, R_LOG_DIR_IN);
    iLogEvent->SetDirection(incoming);

    // set duration
    iLogEvent->SetDurationType( KLogDurationValid );
    //iLogEvent->SetDuration( iParameters->iReceivedCallDuration );
    iLogEvent->SetDuration( parameters->iDuration );

    // set the phone number
    //iLogEvent->SetNumber( iParameters->iReceivedCallPhoneNumber->Des() );
    iLogEvent->SetNumber( parameters->iPhoneNumber->Des() );

    // set event time
    //iLogEvent->SetTime( iParameters->iReceivedCallEventTime );
    iLogEvent->SetTime( parameters->iEventTime );

    // start writing
    __ASSERT_ALWAYS(!IsActive(), User::Panic(_L("LogWriter"), 151));
	iLogClient->AddEvent(*iLogEvent, iStatus);
	SetActive();

    // wait till adding has finished
    iLogWriteWait.Start();

    // id has been generated, store it for being able to delete
    // entries created only with Creator
    TRAP_IGNORE( iEntryIds.AppendL( iLogEvent->Id() ) );

    return iStatus.Int();  // should be KErrNone if writing was succesful
    }

//----------------------------------------------------------------------------


TInt CCreatorLogs::CreateDialledNumberEntryL(CLogsParameters *aParameters)
    {
    LOGSTRING("Creator: CCreatorLogs::CreateDialledNumberEntryL");

    // clear any existing parameter definations
    delete iParameters;
    iParameters = NULL;
    
    CLogsParameters* parameters = aParameters; 
        
    // random data needed if no predefined data available
    if (!parameters)
        {
        iParameters = new(ELeave) CLogsParameters;
        parameters = iParameters;

        /*iParameters->iDialledNumberPhoneNumber->Des().Zero();
        iParameters->iDialledNumberPhoneNumber->Des().AppendNum( iEngine->RandomNumber(1000000, 9999999) );

        iParameters->iDialledNumberEventTime.UniversalTime();
        iParameters->iDialledNumberEventTime -= TTimeIntervalMinutes( iEngine->RandomNumber(12000) );

        iParameters->iDialledNumberDuration = iEngine->RandomNumber(7200);*/
        parameters->iPhoneNumber->Des().Zero();
        parameters->iPhoneNumber->Des().AppendNum( iEngine->RandomNumber(1000000, 9999999) );

        parameters->iEventTime.UniversalTime();
        parameters->iEventTime -= TTimeIntervalMinutes( iEngine->RandomNumber(12000) );

        parameters->iDuration = iEngine->RandomNumber(7200);

        }
   
    // defaults
    iLogEvent->SetId( KLogNullId );
    iLogEvent->SetEventType( KNullUid );
    iLogEvent->SetRemoteParty( KNullDesC );
    iLogEvent->SetDirection( KNullDesC );
    iLogEvent->SetTime( Time::NullTTime() );
    iLogEvent->SetDurationType( KLogNullDurationType );
    iLogEvent->SetDuration( KLogNullDuration );
    iLogEvent->SetStatus( KNullDesC );
    iLogEvent->SetSubject( KNullDesC );
    iLogEvent->SetNumber( KNullDesC );
    iLogEvent->SetContact( KNullContactId );
    iLogEvent->SetLink( KLogNullLink );
    iLogEvent->SetDescription( KNullDesC );
    iLogEvent->SetFlags( KLogNullFlags );
    iLogEvent->SetEventType(KLogCallEventTypeUid);
    iLogEvent->SetSubject(_L("A"));

    // set direction
    TLogString outgoing;
    iLogClient->GetString(outgoing, R_LOG_DIR_OUT);
    iLogEvent->SetDirection(outgoing);

    // set duration
    iLogEvent->SetDurationType( KLogDurationValid );
    //iLogEvent->SetDuration( iParameters->iDialledNumberDuration );
    iLogEvent->SetDuration( parameters->iDuration );

    // set the phone number
    //iLogEvent->SetNumber( iParameters->iDialledNumberPhoneNumber->Des() );
    iLogEvent->SetNumber( parameters->iPhoneNumber->Des() );

    // set event time
    //iLogEvent->SetTime( iParameters->iDialledNumberEventTime );
    iLogEvent->SetTime( parameters->iEventTime );

    // start writing
    __ASSERT_ALWAYS(!IsActive(), User::Panic(_L("LogWriter"), 152));
	iLogClient->AddEvent(*iLogEvent, iStatus);
	SetActive();

    // wait till adding has finished
    iLogWriteWait.Start();

    // id has been generated, store it for being able to delete
    // only entries created with Creator
    TRAP_IGNORE( iEntryIds.AppendL( iLogEvent->Id() ) );
    
    return iStatus.Int();  // should be KErrNone if writing was succesful
    }

//----------------------------------------------------------------------------
void CCreatorLogs::DeleteAllL()
    {
    LOGSTRING("Creator: CCreatorLogs::DeleteAllL");
    iLogClient->ClearLog( TTime( Time::MaxTTime() ), iStatus ); 
    SetActive();
    
    // wait till delete has finished
    iLogWriteWait.Start();
    User::LeaveIfError( iStatus.Int() );

    // reset must be done here, because iEntryIds is stored in destructor
    iEntryIds.Reset();
    
    // all entries deleted, remove the Logs related registry
    iEngine->RemoveStoreL( KUidDictionaryUidLogs );
    }

//----------------------------------------------------------------------------
void CCreatorLogs::DeleteAllCreatedByCreatorL()
    {
    LOGSTRING("Creator: CCreatorLogs::DeleteAllCreatedByCreatorL");
    iEntryIds.Reset();
    
    // fetch ids of entries created by Creator
    iEngine->ReadEntryIdsFromStoreL( iEntryIds, KUidDictionaryUidLogs );
    
    // delete entries
    for ( TInt i = 0; i < iEntryIds.Count(); i++ )
        {
        // This could be implemented with while loop
        // with additional cancel flag. That would enable 
        // using CancelOp() and cancelling would be bit smoother.
        // With this for loop implementation all entries will be
        // deleted whether or not user cancels during logs operation.

        iLogClient->DeleteEvent( iEntryIds[i], iStatus );
        SetActive();        
        // wait till delete has finished
        iLogWriteWait.Start();
        }
    
    // reset must be done here, because iEntryIds is stored in destructor
    iEntryIds.Reset();
    
    // all entries deleted, remove the Logs related registry
    iEngine->RemoveStoreL( KUidDictionaryUidLogs );
    }
