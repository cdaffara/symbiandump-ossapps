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
 * Description:
 */

#include <mtmdef.h>
#include <CoreApplicationUIsSDKCRKeys.h>
#include <mmsheaders.h>
#include <mmscmds.h>
#include <mmsencode.h>
#include <mmscliententry.h>
#include <logwrap.h>
#include <logcli.h>
#include <logview.h>
#include "mmssettings.h"  //use mmssettings.h instead of cmmssettings.h
#include "mmstestbed.h"
#include "mmsreadfile.h"
#include "mmstestuitimer.h"
#include "mmsteststaticutils.h"
#include "mmstestbed.hrh"

//constants
_LIT( KMmsSender, "0601234567" );

MmsTestBed::MmsTestBed()
    {
    //start the timer
    iTimer = CTestUiTimer::NewL();
    iTimer->Cancel();

    iWait = CMsvOperationActiveSchedulerWait::NewLC();
    // don't leave iWait on cleanup stack
    CleanupStack::Pop();

    //open msvsession
    iSession = CMsvSession::OpenSyncL(*this);
    //create client registry
    iClientMtmRegistry = CClientMtmRegistry::NewL(*iSession);
    //create client mtm
    iMmsClient = (CMmsClientMtm *) iClientMtmRegistry->NewMtmL(
            KUidMsgTypeMultimedia);

    User::LeaveIfError( iFs.Connect() );
    iFs.SetSessionPath( KRootPath );
    iSettings = CMmsSettings::NewL();
    iMmsHeaders = CMmsHeaders::NewL(iSettings->MmsVersion());
    findDefaultL();
    iServiceId = iDefaultServiceId;

    //validate the settings
    iSettings->ValidateSettings();

    iLogEvent = CLogEvent::NewL();
    iLogEvent->SetEventType(KLogMmsEventTypeUid);
    iLogClient = NULL; // we test soon if this is available
    iLogView = NULL; // needs log client
    if ( checkLogClient() )
        {
        // first we generate a general view of all events
        // we'll set the filter when we update the view
        iLogView = CLogViewEvent::NewL( *iLogClient );
        }
    iLogFilter = CLogFilter::NewL();
    // we try to filter MMS events
    iLogFilter->SetEventType(KLogMmsEventTypeUid);
    }

MmsTestBed::~MmsTestBed()
    {
    delete iLogView;
    delete iLogFilter;
    delete iLogClient;
    delete iLogEvent;
    delete iSettings;
    delete iMmsHeaders;
    if(iTimer)
    {
        iTimer->Cancel();
        delete iTimer;
    }
    //delete iMsvEntrySelection;
    delete iMmsClient;
    delete iClientMtmRegistry;
    //delete iClientMtmRegistry;
    delete iSession;
    delete iWait;
    }

void MmsTestBed::setConnectionLocal(bool value)
    {
    //value = true for global off, local on
    //value = false for global on, local off
    iSettings->LoadSettingsL();
    iSettings->SetLocalMode( value );
    iSettings->SaveSettingsL();
    }

void MmsTestBed::fromOutboxToMmsc()
    {
    CMsvEntry* cEntry = NULL;

    // Get List of services
    cEntry = iSession->GetEntryL(KMsvGlobalOutBoxIndexEntryIdValue);
    CleanupStack::PushL(cEntry);
    // Get all mms messages of outbox
    CMsvEntrySelection* selection = cEntry->ChildrenWithMtmL(
            KUidMsgTypeMultimedia);
    CleanupStack::PushL(selection);

    // Change state to "KMsvSendStateUnknown" in case the entry has been suspended earlier
    for (TInt i = 0; i < selection->Count(); ++i)
        {
        cEntry->SetEntryL(selection->At(i));
        TMsvEntry entry = cEntry->Entry();
        entry.SetReadOnly(EFalse);
        entry.SetSendingState(KMsvSendStateUnknown);
        cEntry->ChangeL(entry);
        }

    selection->InsertL(0, iServiceId);

    CMsvOperation * op = NULL;
    TCommandParameters parameters; // initialized to zero
    TCommandParametersBuf paramPack(parameters);

    op = iSession->TransferCommandL(*selection, EMmsSend, paramPack,
            iWait->iStatus);

    CleanupStack::PushL(op);
    iWait->Start();

    while (iWait->iStatus.Int() == KRequestPending)
        {
        if (!iTimer->IsActive())
            {
            iTimer->IssueRequest();
            }
        CActiveScheduler::Start();
        }

    if (iWait->iStatus.Int() != KErrNone)
        {
        //DEBUG(_L("Testbed tried to send, return status %d"), iWait->iStatus.Int());
        }

    iTimer->Cancel();

    CleanupStack::PopAndDestroy(); // op
    CleanupStack::PopAndDestroy(); // selection
    CleanupStack::PopAndDestroy(); //cEntry
    }

void MmsTestBed::fromMmscToInbox()
    {
    CMsvEntrySelection* msvEntrySelection = new CMsvEntrySelection;
    CleanupStack::PushL(msvEntrySelection);

    // if we have a selected service, insert it into selection
    if (iServiceId != KMsvNullIndexEntryId)
        {
        msvEntrySelection->InsertL(0, iServiceId);
        }

    CMsvOperation * op = NULL;
    TCommandParameters parameters; // initialized to zero
    TCommandParametersBuf paramPack(parameters);

    op = iMmsClient->InvokeAsyncFunctionL(EMmsReceive, *msvEntrySelection,
            paramPack, iWait->iStatus);

    CleanupStack::PushL(op);
    iWait->Start();

    while (iWait->iStatus.Int() == KRequestPending)
        {
        if (!iTimer->IsActive())
            {
            iTimer->IssueRequest();
            }
        CActiveScheduler::Start();
        }

    if (iWait->iStatus.Int() != KErrNone)
        {
        //DEBUG(_L("Testbed tried to receive, return status %d"),iWait->iStatus.Int());
        }

    iTimer->Cancel();
    CleanupStack::PopAndDestroy(); // op
    CleanupStack::PopAndDestroy(); //msvEntrySelection
    }

void MmsTestBed::findDefaultL()
    {
    iSettings->LoadSettingsL();
    iDefaultServiceId = iSettings->Service();
    }

void MmsTestBed::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1,
        TAny* aArg2, TAny* /*aArg3*/)
    {
    iEvent = aEvent;
    if (aEvent == EMsvGeneralError)
        {
        return;
        }
    TMsvId parentId = KMsvNullIndexEntryId;
    if (aArg2 != NULL)
        {
        parentId = *(TMsvId*) aArg2;
        }

    CMsvEntrySelection* selection = (CMsvEntrySelection*) aArg1;
    TMsvEntry tEntry;
    TMsvId service;
    TInt error = KErrNone;
    error = iSession->GetEntry(selection->At(0), service, tEntry);

    CMsvEntry* cEntry = NULL;
    switch (aEvent)
        {
        case EMsvEntriesCreated:
            {
            if (parentId == KMsvGlobalInBoxIndexEntryIdValue)
                {
                // emit signal for new entry into INBOX
                emit entryCreatedInInbox(tEntry.Id());
                }
            else if (parentId == KMsvDraftEntryIdValue)
                {
                // emit signal for new entry into Draft
                emit entryCreatedInDraft(tEntry.Id());
                }
            else if (parentId == iServiceId)
                {
                // emit signal for new Entry into SERVICE
                }
            else
                {
                // do nothing
                }
            }
            break;
        case EMsvEntriesChanged:
            {
            TMsvId id;
            CMsvEntrySelection* selection = (CMsvEntrySelection*) aArg1;
            if (selection == NULL)
                {
                // no selection, cannot handle
                return;
                }
            id = selection->At(0);
            //DEBUG Entry changed"));
            if (parentId == KMsvGlobalInBoxIndexEntryIdValue)
                {
/*
                cEntry = iSession->GetEntryL( parentId );
                CleanupStack::PushL( cEntry );
                TRAPD (error, cEntry->SetEntryL( id ));
                if ( error != KErrNone )
                    {
                    CleanupStack::PopAndDestroy(); // cEntry
                    return;
                    }
                TMsvEntry tEntry = cEntry->Entry();
                if ( tEntry.Visible() )
                    {
                    // generate fake delivery report
                    CMsvStore* store = cEntry->ReadStoreL();
                    CleanupStack::PushL( store );
                    CMmsHeaders* mmsHeaders = CMmsHeaders::NewL( iSettings->MmsVersion() );
                    CleanupStack::PushL( mmsHeaders );
                    mmsHeaders->RestoreL( *store );
                    iEncodeBuffer->ResizeL( 0 );
                    generateDeliveryReport( mmsHeaders );
                    CleanupStack::PopAndDestroy( 2 ); // mmsHeaders, store
                    }
                CleanupStack::PopAndDestroy(); // cEntry
*/
                }
            }
            break;
        case EMsvEntriesDeleted:
            {
            //emit signal for entry deleted
            }
            break;
        case EMsvEntriesMoved:
            {
            if (parentId == KMsvGlobalOutBoxIndexEntryIdValue)
                {
                // entry moved to outbox
                emit entryMovedToOutbox(tEntry.Id());
                }
            else if (parentId == KMsvSentEntryIdValue)
                {
                // entry moved to sent folder
                emit entryMovedToSent(tEntry.Id());
                }
            else
                {
                // do nothing
                }
            }
            break;
        default:
            break;
        }
    }

void MmsTestBed::createMmsService()
    {
    CMmsSettings * settings = CMmsSettings::NewL();
    CleanupStack::PushL( settings );
    settings->CreateNewServiceL( *iSession );
    CleanupStack::PopAndDestroy(); // settings
    return;
    }

void MmsTestBed::cleanup()
    {
    TMsvId entryToBeKilled;
    // Get access to root index
    CMsvEntry* cEntry = iSession->GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(cEntry);

    entryToBeKilled = iSettings->Service();
    cEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
    while (entryToBeKilled != KMsvNullIndexEntryId)
        {
        // delete child of root entry
        deleteEntry(entryToBeKilled, *cEntry);
        entryToBeKilled = iSettings->Service();
        }

    CleanupStack::PopAndDestroy(); // cEntry
    // We deleted everything!
    iDefaultServiceId = KMsvNullIndexEntryId;
    iServiceId = KMsvNullIndexEntryId;
    }

void MmsTestBed::deleteEntry(TMsvId aEntryId, CMsvEntry& aClientEntry)
    {
    aClientEntry.DeleteL(aEntryId);
    }

void MmsTestBed::cleanupAndCreateNewService()
    {
    cleanup();
    // all old service entries have been destroyed, create a new one
    createMmsService();
    }

void MmsTestBed::testFile(TFileName& aFilePath, TInt aCommand /* = 0 */, TMsvId aBox /* = KMsvGlobalOutBoxIndexEntryId */ )
    {
    // update settings in mmsclient
    iMmsClient->RestoreSettingsL();
    iSettings->CopyL( iMmsClient->MmsSettings() );

    RFileReadStream readStream;
    readStream.PushL();
//    TMsvId id = KMsvNullIndexEntryId;
    CBufFlat* encodeBuffer = NULL;
    CMmsEncode* encoder = NULL;

    // Open the file
    TInt err = readStream.Open(iFs, aFilePath, EFileShareReadersOnly );
    if (err != KErrNone) User::Leave( err );

    TInt   retCode = 0;
    CMmsReadFile* readFile = NULL;
    readFile = CMmsReadFile::NewL( iFs, readStream );
    CleanupStack::PushL( readFile );

    TInt messageCounter = 0;
    CMsvEntry* cEntry = NULL;

    while(!retCode)
        {
        // READ MESSAGE TO BUFFERS
        iMmsHeaders->Reset(iSettings);
        // put in some message type just for fun (testing...)
        iMmsHeaders->SetMessageType( KMmsMessageTypeForwardReq );
        retCode = readFile->CompleteTestL( messageCounter++, *iMmsHeaders );
        if(readFile->iMessageType == ETestNewMessage)
            {
            // CREATE MESSAGE ENTRY
            switch ( aCommand )
                {
                case ECreateToInbox:
                case ECreateNotification:
                    cEntry = iSession->GetEntryL(KMsvGlobalInBoxIndexEntryId);
                    break;
                case ECreateToSentItems:
                    cEntry = iSession->GetEntryL(KMsvSentEntryId);
                    break;
                case ECreateToDrafts:
                    cEntry = iSession->GetEntryL(KMsvDraftEntryId);
                    break;
                case ECreateHeadersFromFile:
                    // here we just encode headers, no message entry
                    // the entry is fake.
                    break;
                case ECreateMMBoxViewConf:
                    if ( iMmsHeaders->MessageType() == KMmsMessageTypeMboxViewConf )
                        {
                        // entry is not created for the description items
                        // they become attachments
                        cEntry = iSession->GetEntryL(KMsvDraftEntryId);
                        }
                    break;
                default:
                    cEntry = iSession->GetEntryL(aBox);
                    break;
                }

            // if we are just playing with headers we have no entry
            if ( aCommand != ECreateHeadersFromFile && aCommand != ECreateMMBoxViewConf )
                {
                CleanupStack::PushL(cEntry);
                iMmsClient->SwitchCurrentEntryL(cEntry->EntryId());

                // CREATE MESSAGE
                iMmsClient->CreateMessageL(iServiceId);
                }
            else if ( aCommand == ECreateMMBoxViewConf )
                {
                if ( iMmsHeaders->MessageType() == KMmsMessageTypeMboxViewConf )
                    {
                    CleanupStack::PushL(cEntry);
                    iMmsClient->SwitchCurrentEntryL(cEntry->EntryId());

                    // CREATE MESSAGE
                    iMmsClient->CreateMessageL(iServiceId);
                    }
                else
                    {
                    encodeBuffer = CBufFlat::NewL( 4 * 1024 ); // should be plenty
                    CleanupStack::PushL( encodeBuffer );
                    encoder = CMmsEncode::NewL( iFs );
                    CleanupStack::PushL( encoder );

                    // encode headers to a binary file
                    encoder->EncodeHeadersL( *iMmsHeaders, *encodeBuffer );

                    iFilename = KMmsMMBoxDescriptionDirectory;
                    TMmsTestUtils::Dump( *encodeBuffer, iFilename, iParse, iFs );

                    CleanupStack::PopAndDestroy( 2 ); // encodeBuffer, encoder
                    encodeBuffer = NULL;
                    encoder = NULL;
                    }
                }
            else
                {
                encodeBuffer = CBufFlat::NewL( 4 * 1024 ); // should be plenty
                CleanupStack::PushL( encodeBuffer );
                encoder = CMmsEncode::NewL( iFs );
                CleanupStack::PushL( encoder );

                // encode headers to a binary file
                encoder->EncodeHeadersL( *iMmsHeaders, *encodeBuffer );

                iFilename = KMmsDumpDirectory;
                TMmsTestUtils::Dump( *encodeBuffer, iFilename, iParse, iFs );

                CleanupStack::PopAndDestroy( 2 ); // encodeBuffer, encoder
                encodeBuffer = NULL;
                encoder = NULL;
                }
            }

        if ( aCommand != ECreateHeadersFromFile &&
            ( aCommand != ECreateMMBoxViewConf || iMmsHeaders->MessageType() == KMmsMessageTypeMboxViewConf ) )
            {
            if(readFile->iMessageType == ETestSettings)
                {
                TMsvId ServiceId = iMmsClient->DefaultServiceL();
                iMmsClient->RestoreSettingsL();
                iSettings->CopyL( iMmsClient->MmsSettings() );
                }

            TMemoryInfoV1Buf memory;
            UserHal::MemoryInfo( memory );
            TInt available = memory().iFreeRamInBytes;
//            TMmsLogger::Log(_L("Free memory before CreateMessageL %d"), available );

            TRAP (err, readFile->CreateMessageL(iMmsClient, iMmsHeaders));

            available = memory().iFreeRamInBytes;
//            TMmsLogger::Log(_L("Free memory after CreateMessageL %d"), available );

            if(readFile->iMessageType == ETestNewMessage)
                {
                TMsvEntry tEntry = iMmsClient->Entry().Entry();
                TMsvId id = tEntry.Id();
                if ( err == KErrNone )
                    {
                    // SAVE MESSAGE
                    iMmsClient->SaveMessageL();

                    // If we are creating a MMBox View confirmation,
                    // we add all binary files from KMmsMMBoxDirectory
                    // as attachments.

                    if ( iMmsHeaders->MessageType() == KMmsMessageTypeMboxViewConf )
                        {
                        addMMBoxDescriptions();
                        }

                    // reload the entry in case mms client put something into it
                    // MESSAGE MUST BE SET VISIBLE
                    tEntry = iMmsClient->Entry().Entry();
                    if ( iMmsClient->MessageClass() == EMmsClassAdvertisement )
                        {
                        tEntry.iMtmData1 |= KMmsMessageAdvertisement;
                        }
                    else if ( iMmsClient->MessageClass() == EMmsClassInformational )
                        {
                        tEntry.iMtmData1 |= KMmsMessageInformational;
                        }
                    tEntry.iMtmData1 &= ~KMmsMessageMobileTerminated;

                    // Test: Set all as editor oriented - except notifications!
                    if ( aCommand == ECreateNotification )
                        {
                        tEntry.iMtm = KUidMsgMMSNotification;
                        }
                    else
                        {
                        tEntry.iMtmData1 |= KMmsMessageEditorOriented;
                        }
                    if ( aCommand == ECreateToInbox )
                        {
                        tEntry.iMtmData1 |= KMmsMessageMobileTerminated;
                        tEntry.SetReadOnly( ETrue );
                        tEntry.SetNew( ETrue );
                        tEntry.SetUnread( ETrue );
                        }
                    else if ( aCommand == ECreateToSentItems )
                        {
                        tEntry.SetReadOnly( ETrue );
                        }
                    tEntry.SetVisible( ETrue );
                    tEntry.SetInPreparation( EFalse );
                    TTime now;
                    now.UniversalTime();
                    tEntry.iDate = now;
                    TMsvId entryId = tEntry.Id();
                    iMmsClient->Entry().ChangeL( tEntry );
                    if ( iMmsHeaders->MessageType() == KMmsMessageTypeMboxViewConf )
                        {
                        // Encode to the directory that is used to fetch MMBox view
                        iFilename.Copy( KMmsMMBoxDirectory );
                        encodeMessageFromDrafts();
                        cEntry->SetEntryL( KMsvDraftEntryId );
                        cEntry->DeleteL( entryId );
                        }
                    }
                else
                    {
                    //TMmsLogger::Log(_L("CreateMessageL left with error %d"), err );
                    iSession->RemoveEntry(id);
                    err = KErrNone; // clear error
                    }
                CleanupStack::PopAndDestroy(); // cEntry
                cEntry = NULL;
                }
            if(readFile->iMessageType == ETestSettings)
                {
                iMmsClient->SetSettingsL( *iSettings );
                iMmsClient->StoreSettingsL();
                }
        }
    }

    CleanupStack::PopAndDestroy(); //readFile

    readStream.Close();
    readStream.Pop();

    /*
    iMmsClient->SwitchCurrentEntryL(id);
    */
    }

void MmsTestBed::addMMBoxDescriptions()
    {
    // add the contents of KMmsMMBoxDescriptionDirectory as attachments
    CDir* fileList = NULL;
    TInt i = 0;  // general counter
    TInt error = KErrNone;
    iCurrentPath = KMmsMMBoxDescriptionDirectory;

    iFs.SetSessionPath(iCurrentPath);

    TFindFile finder( iFs );
    error = finder.FindWildByPath( KWild, NULL, fileList );
    CleanupStack::PushL( fileList );
    TInt fileCounter = 0;

    if ( error == KErrNone )
        {
        fileCounter = fileList->Count();
        }

    TEntry entry;

    if ( error == KErrNone )
        {
        for ( i = 0; i < fileCounter; ++i )
            {
            // Reset inactivity timer to keep viewServer from crashing
            User::ResetInactivityTime();
            entry = (*fileList)[i]; // name is entry.iName
            iFilename.Copy( iCurrentPath );
            iFilename.Append( entry.iName );
            TPtrC ptr;
            ptr.Set( iFilename );
            iWait->iStatus = KErrNone;
            iMmsClient->AddAttachmentL( ptr, KMmsMimeType, 0, iWait->iStatus );

            iWait->Start();
            // The descriptions are cleared after being used
            iFs.Delete( ptr);
            }
        }

    iMmsClient->SaveMessageL(); // just in case somthing must be updated
    CleanupStack::PopAndDestroy(); // fileList
    fileList = NULL;
    }

void MmsTestBed::encodeMessageFromDrafts()
    {
    CMmsEncode* encoder = CMmsEncode::NewL( iFs );
    CleanupStack::PushL( encoder );
    // encode a message iMmsClientPoints to
    iMmsClient->LoadMessageL();
    CMsvStore* store = iMmsClient->Entry().ReadStoreL();
    CleanupStack::PushL( store );
    iMmsHeaders->RestoreL( *store );
    CleanupStack::PopAndDestroy(); // store
    store = NULL;
    iWait->iStatus = KErrNone;

// caller sets the directory
//    iFilename = KMmsMessageDumpDirectory;

    CMmsClientEntry* entryWrapper = CMmsClientEntry::NewL( iFs, iMmsClient->Entry(), iServiceId );
    CleanupStack::PushL( entryWrapper );
    iEncodeBuffer->ResizeL(0);
    encoder->StartL( *entryWrapper, *iMmsHeaders, *iEncodeBuffer, iWait->iStatus );
    iWait->Start();
    if ( iWait->iStatus == KErrNone )
        {
        TMmsTestUtils::Dump( *iEncodeBuffer, iFilename, iParse, iFs );
        }
    iEncodeBuffer->ResizeL(0);
    CleanupStack::PopAndDestroy(); // entryWrapper
    CleanupStack::PopAndDestroy(); // encoder
    }

void MmsTestBed::deleteNotifications()
    {
    TMsvId mmsFolderId = KMsvNullIndexEntryId;
    mmsFolderId = findMMSFolder();

    CMsvEntry* cEntry = NULL;
    // delete all messages from the specified box
    cEntry = iSession->GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(cEntry);

    if ( mmsFolderId != KMsvNullIndexEntryId )
        {
        cEntry->SetEntryL(mmsFolderId);

        // show invisible entries
        cEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
        CMsvEntrySelection* msvEntrySelection = cEntry->ChildrenWithMtmL(KUidMsgTypeMultimedia);
        CleanupStack::PushL(msvEntrySelection);

        TCommandParameters parameters; // initialized to zero
        TCommandParametersBuf paramPack( parameters );

        if (msvEntrySelection->Count() > 0)
            {
            CMsvOperation* op = iSession->TransferCommandL(
                *msvEntrySelection,
                EMmsDeleteEntries,
                paramPack,
                iWait->iStatus);
            CleanupStack::PushL(op);
            iWait->Start();
            CleanupStack::PopAndDestroy(); // op
            }

        // These cannot be deleted unless we have the a server mtm
        // corresponding to this mtm type.
        cEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
        CleanupStack::PopAndDestroy(); //msvEntrySelection
        msvEntrySelection = cEntry->ChildrenWithMtmL( KUidMsgMMSNotification );
        CleanupStack::PushL(msvEntrySelection);

        if (msvEntrySelection->Count() > 0)
            {
            CMsvOperation* op = iSession->TransferCommandL(
                *msvEntrySelection,
                EMmsDeleteEntries,
                paramPack,
                iWait->iStatus);
            CleanupStack::PushL(op);
            iWait->Start();
            CleanupStack::PopAndDestroy(); // op
            }

        CleanupStack::PopAndDestroy(); //msvEntrySelection

        CleanupStack::PopAndDestroy(); //cEntry
        }
    }

TMsvId MmsTestBed::findMMSFolder()
    {
    return iSettings->NotificationFolder();
    }

void MmsTestBed::restoreFactorySettings()
    {
    iMmsClient->RestoreSettingsL();
    iSettings->CopyL( iMmsClient->MmsSettings() );
    // do not reset access point
    TInt accessPoint = iSettings->AccessPoint( 0 );
    iSettings->RestoreFactorySettingsL( iMmsClient->Session(), EMmsFactorySettingsLevelDeep );
    TInt count = iSettings->AccessPointCount();
    TInt i = 0;
    for ( i = count - 1; i >= 0; --i )
        {
        iSettings->DeleteAccessPointL( i );
        }
    // restore the original access point
    if ( accessPoint > 0 )
        {
        // a negative access point is an error (most likely "KErrNotFound")
        iSettings->AddAccessPointL( accessPoint, 0 );
        }
    iMmsClient->SetSettingsL( *iSettings );
    iMmsClient->StoreSettingsL();
    }

void MmsTestBed::setFetchingState( TMmsReceivingMode aState )
    {
    iMmsClient->RestoreSettingsL();
    iSettings->CopyL( iMmsClient->MmsSettings() );

    iSettings->SetReceivingModeHome( aState );

    iMmsClient->SetSettingsL( *iSettings );
    iMmsClient->StoreSettingsL();
    }

void MmsTestBed::sendFromFile()
    {
    CMsvOperation * op = NULL;

    CMsvEntry* cEntry = iSession->GetEntryL(KMsvGlobalOutBoxIndexEntryId);
    CleanupStack::PushL( cEntry );

    CMsvEntrySelection* selection = NULL;
    selection = cEntry->ChildrenWithMtmL(KUidMsgTypeMultimedia);
    CleanupStack::PopAndDestroy(); // cEntry
    CleanupStack::PushL( selection );

    TRAPD (error, op = iMmsClient->SendL(*selection, iWait->iStatus));
    if ( error != KErrNone )
        {
        CleanupStack::PopAndDestroy(); // selection
        delete op;
        return;
        }
    CleanupStack::PushL(op);
    iWait->Start();

    while (iWait->iStatus.Int() == KRequestPending )
        {
        if (!iTimer->IsActive())
            {
            iTimer->IssueRequest();
            }
        CActiveScheduler::Start();
        }

    if ( iWait->iStatus.Int() != KErrNone )
        {
        //TMmsLogger::Log(_L("Testbed tried to send, return status %d"), iWait->iStatus.Int());
        }

    iTimer->Cancel();
    CleanupStack::PopAndDestroy(2); // op, selection
    }

void MmsTestBed::sendOneByOne()
    {
    CMsvEntry* cEntry = iSession->GetEntryL(KMsvGlobalOutBoxIndexEntryId);
    CleanupStack::PushL( cEntry );

    CMsvEntrySelection* selection = NULL;
    selection = cEntry->ChildrenWithMtmL(KUidMsgTypeMultimedia);
    CleanupStack::PushL( selection );

    CMsvEntrySelection* shortSelection = new (ELeave) CMsvEntrySelection;
    CleanupStack::PushL( shortSelection );

    TInt i;

    for ( i = 0; i < selection->Count(); ++i )
        {
        shortSelection->Reset();
        shortSelection->AppendL( selection->At( i ) );

        CMsvOperation * op = NULL;

        TTime now;
        now.UniversalTime();

        TRAPD (error, op = iMmsClient->SendL(*shortSelection, iWait->iStatus, now ));
        if ( error != KErrNone )
            {
            delete op;
            CleanupStack::PopAndDestroy( 3 ); // entry, selection, shortSelection
            return;
            }
        CleanupStack::PushL(op);
        iWait->Start();

        while (iWait->iStatus.Int() == KRequestPending )
            {
            if (!iTimer->IsActive())
                {
                iTimer->IssueRequest();
                }
            CActiveScheduler::Start();
            }

        if ( iWait->iStatus.Int() != KErrNone )
            {
            //TMmsLogger::Log(_L("Testbed tried to send, return status %d"), iWait->iStatus.Int());
            }

        iTimer->Cancel();
        CleanupStack::PopAndDestroy(); // op

        }
    CleanupStack::PopAndDestroy( 3 ); // entry, selection, shortSelection
    }

void MmsTestBed::sendNotifications()
    {
    // Only send one notification!!

    TInt error = KErrNone;

    if (! TMmsTestUtils::IsFile(iCurrentFile, iFs) )
        {
        //Not a file
        return;
        }
    TMsvId mmsFolder = findMMSFolder();

    TEntry entry;


    // now I think we have a filename
    TEntry orgEntry;
    TUint size = 0;
    error = iFs.Entry( iCurrentFile, orgEntry );
    size = orgEntry.iSize;

    //TMmsLogger::Log(_L("- notification %S "), &iCurrentFile);
    if ( size == 0 )
        {
        //empty file
        //TMmsLogger::Log(_L("- empty file"));
        return;
        }

    if ( iEncodeBuffer == NULL )
        {
        iEncodeBuffer = CBufFlat::NewL( size );
        }
    else
        {
        iEncodeBuffer->ResizeL( 0 );
        iEncodeBuffer->ResizeL( size );
        }

    RFile inFile;
    error = inFile.Open( iFs, iCurrentFile, EFileShareReadersOnly );
    TPtr8 ptr = iEncodeBuffer->Ptr( 0 );
    if ( error == KErrNone )
        {
        error = inFile.Read( ptr, size );
        inFile.Close();
        }
    else
       {
       //Error
       //TMmsLogger::Log(_L("- can't read file"));
       return;
       }

    TUint8 byte;
    TUint position = 0;
    TUint32 uintvar = 0;

    if ( size > 2 )
        {
        iEncodeBuffer->Read( 1, &byte, 1 );
        if ( byte == 6 ) // PUSH PDU
            {
            // try to find out length of header
            position = 2;
            iEncodeBuffer->Read( position, &byte, 1);

            while ( byte & 0x80  && position < size )
                {
                uintvar += ( byte & 0x7f );
                uintvar <<= 7;
                position++;
                iEncodeBuffer->Read( position, &byte, 1 );
                }

            // add last byte without shift
            uintvar += byte;
            position++;
           }
        }

    position += uintvar;

    if ( position < size )
        {
        ptr = iEncodeBuffer->Ptr( position );
        size = ptr.Length();
        }

    if ( size == 0 )
        {
        //no MMS stuff
        //TMmsLogger::Log(_L("- no MMS stuff"));
        return;
        }

    TMsvId entryId = TMmsTestUtils::CreateNotificationEntryL( mmsFolder, iServiceId, iEncodeBuffer, *iSession );

    // Now we have streamed our data into this entry.
    // Now we have an entry that says: local service, MMS MTM
    CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    if ( entryId != KMsvNullIndexEntryId )
        {
        selection->AppendL( entryId );
        }
    else
        {
        selection->AppendL( iDefaultServiceId );
        }

    TWatcherParameters parameters; // initialized to zero
    parameters.iWatcherId = RThread().Id();
    parameters.iDataPointer = &ptr;
    TWatcherParametersBuf paramPack( parameters );

    CMsvOperation * op = NULL;

    op = iSession->TransferCommandL(
        *selection, EMmsDecodePushedMessage, paramPack, iWait->iStatus );
    CleanupStack::PushL(op);
    iWait->Start();

    while (iWait->iStatus.Int() == KRequestPending )
        {
        if (!iTimer->IsActive())
            {
            iTimer->IssueRequest();
            }
        CActiveScheduler::Start();
        }

    iTimer->Cancel();
    User::After(1000000);

    CleanupStack::PopAndDestroy(); // op
    CleanupStack::PopAndDestroy(); // selection
    }

void MmsTestBed::copyDrafts()
    {
    CMsvEntry* cEntry = NULL;
// Copies contents of sent folder to drafts for retrying sending.

    cEntry = iSession->GetEntryL(KMsvSentEntryId);
    CleanupStack::PushL(cEntry);
    // Get all mms messages in drafts
    CMsvEntrySelection* selection = cEntry->ChildrenWithMtmL( KUidMsgTypeMultimedia );
    CleanupStack::PushL( selection );

    TMsvLocalOperationProgress progress;
    cEntry->CopyL( *selection, KMsvDraftEntryId, progress );

    TInt i = 0;
    for ( i = 0; i < selection->Count(); ++i )
        {
        cEntry->SetEntryL( selection->At( i ) );
        TMsvEntry entry = cEntry->Entry();
        entry.SetReadOnly( EFalse );
        cEntry->ChangeL( entry );
        }

    CleanupStack::PopAndDestroy(2); // selection, cEntry
    }

void MmsTestBed::garbageCollection(TUint32 aReason)
    {
    CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    selection->InsertL(0, iDefaultServiceId);

    CMsvOperation * op = NULL;

    TMMSGarbageCollectionParameters parameters; // initialized to zero
    parameters.iReasonFlags = aReason;
    TMMSGarbageCollectionParametersBuf paramPack( parameters );
    op = iSession->TransferCommandL(
        *selection, EMmsGarbageCollection, paramPack, iWait->iStatus );
    CleanupStack::PushL(op);
    iWait->Start();

    while (iWait->iStatus.Int() == KRequestPending )
        {
        if (!iTimer->IsActive())
            {
            iTimer->IssueRequest();
            }
        CActiveScheduler::Start();
        }

    iTimer->Cancel();

    CleanupStack::PopAndDestroy(); // op
    CleanupStack::PopAndDestroy(); // selection
    }

void MmsTestBed::messageVariation()
    {
    CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    selection->InsertL(0, iDefaultServiceId);

    CMsvOperation * op = NULL;

    op = iSession->TransferCommandL(
        *selection, EMmsMessageGeneration, TPtrC8(), iWait->iStatus );
    CleanupStack::PushL(op);
    iWait->Start();

    while (iWait->iStatus.Int() == KRequestPending )
        {
        if (!iTimer->IsActive())
            {
            iTimer->IssueRequest();
            }
        CActiveScheduler::Start();
        }

    iTimer->Cancel();

    CleanupStack::PopAndDestroy(); // op
    CleanupStack::PopAndDestroy(); // selection
    }

void MmsTestBed::cancelSendScheduling()
    {
    CMsvEntry* cEntry = NULL;
    setFetchingState( EMmsReceivingPostpone );
    testFile( iCurrentFile );
    scheduledSend(KMsvGlobalOutBoxIndexEntryIdValue, 10000); // long delay so that we have time to cancel
    deleteSendSchedule();

    cEntry = iSession->GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(cEntry);
    cEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryIdValue);
    if (iMsvEntrySelection != NULL)
        {
        delete iMsvEntrySelection;
        iMsvEntrySelection = NULL;
        }
    iMsvEntrySelection = cEntry->ChildrenWithMtmL(KUidMsgTypeMultimedia);
    CleanupStack::PopAndDestroy();
    }

void MmsTestBed::scheduledSend(TMsvId aBoxId, TInt aDelay /* = 5 */)
    {
    CMsvEntry* cEntry = NULL;
    TCommandParameters parameters;
    parameters.iInitialDelay = aDelay;
    TCommandParametersBuf paramPack( parameters );

    cEntry = iSession->GetEntryL(aBoxId);
    CleanupStack::PushL(cEntry);
    // Get all mms messages of outbox
    CMsvEntrySelection* selection = cEntry->ChildrenWithMtmL( KUidMsgTypeMultimedia );
    CleanupStack::PushL( selection );
//    selection->InsertL(0, iDefaultServiceId);

    CMsvOperation * op = NULL;
    op = iMmsClient->InvokeAsyncFunctionL(
        EMmsScheduledSend,
        *selection,
        paramPack,
        iWait->iStatus);
    CleanupStack::PushL(op);
    iWait->Start();

    while (iWait->iStatus.Int() == KRequestPending )
        {
        if (!iTimer->IsActive())
            {
            iTimer->IssueRequest();
            }
        CActiveScheduler::Start();
        }

    iTimer->Cancel();

    CleanupStack::PopAndDestroy(); // op
    CleanupStack::PopAndDestroy(2); // cEntry, selection
    }

void MmsTestBed::deleteSendSchedule()
    {
    CMsvEntry* cEntry = NULL;

    TCommandParameters parameters; // initialized to zero
    TCommandParametersBuf paramPack( parameters );

    cEntry = iSession->GetEntryL(KMsvGlobalOutBoxIndexEntryIdValue);
    CleanupStack::PushL(cEntry);
    // Get all mms messages of outbox
    CMsvEntrySelection* selection = cEntry->ChildrenWithMtmL( KUidMsgTypeMultimedia );
    CleanupStack::PushL( selection );
    selection->InsertL(0, iDefaultServiceId);

    CMsvOperation * op = NULL;
    op = iMmsClient->InvokeAsyncFunctionL(
        EMmsDeleteSchedule,
        *selection,
        paramPack,
        iWait->iStatus);
    CleanupStack::PushL(op);
    iWait->Start();

    while (iWait->iStatus.Int() == KRequestPending )
        {
        if (!iTimer->IsActive())
            {
            iTimer->IssueRequest();
            }
        CActiveScheduler::Start();
        }

    iTimer->Cancel();

    CleanupStack::PopAndDestroy(); // op
    CleanupStack::PopAndDestroy(2); // cEntry, selection
    }

void MmsTestBed::cancelFetchScheduling()
    {
    TMsvId mmsFolder = findMMSFolder();
    CMsvEntry* cEntry = NULL;
    setFetchingState( EMmsReceivingPostpone );
    testFile( iCurrentFile );
    fromOutboxToMmsc(); // immediate send (EMmsSend)

    // Wait until notification has arrived - may take a while in global mode
    TInt i = 0;
    while ( TMmsTestUtils::CountChildrenL( mmsFolder, iMsvEntrySelection, *iSession ) == 0
        && i < 2000 )
        {
        if (!iTimer->IsActive())
            {
            iTimer->IssueRequest();
            }
        i++;
        if ( (i/100) * 100 == i )
            {
            //TMmsLogger::Log(_L("%d:th wait cycle"), i );
            }
        CActiveScheduler::Start();
        }
    iTimer->Cancel();

    fetchForced( 10000 ); // delay to allow cancelling
    deleteFetchSchedule();

    cEntry = iSession->GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(cEntry);
    cEntry->SetEntryL(mmsFolder);
    if (iMsvEntrySelection != NULL)
        {
        delete iMsvEntrySelection;
        iMsvEntrySelection = NULL;
        }
    iMsvEntrySelection = cEntry->ChildrenWithMtmL(KUidMsgTypeMultimedia);
    CleanupStack::PopAndDestroy(); // cEntry
    }

void MmsTestBed::fetchForced(TInt aDelay /* = 5 */)
    {
    TCommandParameters parameters;
    parameters.iInitialDelay = aDelay;
    TCommandParametersBuf paramPack( parameters );

    CMsvEntrySelection* selection = new CMsvEntrySelection;
    CleanupStack::PushL( selection );
    // if we have a selected service, insert it into selection
    if (iServiceId != KMsvNullIndexEntryId)
        {
        selection->InsertL(0, iServiceId);
        }
    else
        {
        selection->InsertL(0, iDefaultServiceId);
        }


    CMsvOperation * op = NULL;
    op = iMmsClient->InvokeAsyncFunctionL(
        EMmsScheduledReceiveForced,
        *selection,
        paramPack,
        iWait->iStatus);

    CleanupStack::PushL(op);
    iWait->Start();

    while (iWait->iStatus.Int() == KRequestPending )
        {
        if (!iTimer->IsActive())
            {
            iTimer->IssueRequest();
            }
        CActiveScheduler::Start();
        }

    iTimer->Cancel();

    CleanupStack::PopAndDestroy(); // op
    CleanupStack::PopAndDestroy(); // selection
    }

void MmsTestBed::deleteFetchSchedule()
    {
    CMsvEntry* cEntry = NULL;

    TCommandParameters parameters; // initialized to zero
    TCommandParametersBuf paramPack( parameters );

    TMsvId mmsFolderId = KMsvNullIndexEntryId;
    mmsFolderId = findMMSFolder();
    cEntry = iSession->GetEntryL( mmsFolderId );
    CleanupStack::PushL(cEntry);
    // Get all notifications
    CMsvEntrySelection* selection = cEntry->ChildrenWithMtmL( KUidMsgTypeMultimedia );
    CleanupStack::PushL( selection );
    selection->InsertL(0, iDefaultServiceId);

    CMsvOperation * op = NULL;
    op = iMmsClient->InvokeAsyncFunctionL(
        EMmsDeleteSchedule,
        *selection,
        paramPack,
        iWait->iStatus);
    CleanupStack::PushL(op);
    iWait->Start();

    while (iWait->iStatus.Int() == KRequestPending )
        {
        if (!iTimer->IsActive())
            {
            iTimer->IssueRequest();
            }
        CActiveScheduler::Start();
        }

    iTimer->Cancel();

    CleanupStack::PopAndDestroy(); // op
    CleanupStack::PopAndDestroy(2); // cEntry, selection
    }

void MmsTestBed::doAFetchCycle()
    {
    // Sends a message from iCurrentFile and lets it come back
    TMsvId mmsFolder = findMMSFolder();
    testFile( iCurrentFile );
    fromOutboxToMmsc(); // immediate send (EMmsSend)
    // fetching will start automatically

    TInt i = 0;
    while ( TMmsTestUtils::CountChildrenL( KMsvGlobalInBoxIndexEntryId, iMsvEntrySelection, *iSession ) == 0
         && i < 2000
         && TMmsTestUtils::CountChildrenL( mmsFolder, iMsvEntrySelection, *iSession ) > 0 )
        {
        if (!iTimer->IsActive())
            {
            iTimer->IssueRequest();
            }
        i++;
        if ( (i/100) * 100 == i )
            {
            //TMmsLogger::Log(_L("%d:th wait cycle"), i );
            }
        CActiveScheduler::Start();
        }
    iTimer->Cancel();
    }

bool MmsTestBed::checkLogClient()
    {
    if ( iLogClient == NULL )
        {
        TRAP_IGNORE( iLogClient = CLogClient::NewL( iFs ) );
        }
    return ( iLogClient != NULL );
    }

int MmsTestBed::getLogEntries()
    {
    TInt count = 0;
    if ( !iLogView )
        {
        return 0;
        }

    if ( iLogView->SetFilterL( *iLogFilter, iWait->iStatus ) )
        {
        // should complete with KErrNone
        iWait->Start();
        if ( iWait->iStatus.Int() == KErrNone )
            {
            count = iLogView->CountL();
            }
        }
    return count;
    }

void MmsTestBed::createEntry(TMsvEntry& aNewEntry, CMsvEntry& aClientEntry)
    {
    CMsvOperation*  opert = aClientEntry.CreateL(aNewEntry, iWait->iStatus);
    iWait->Start();
    if (!opert->iStatus.Int()==KErrNone)
        {
        // what should we do? panic?
        }

    TPckgBuf<TMsvLocalOperationProgress> package;
    package.Copy(opert->ProgressL());
    *(TMsvId*)&aNewEntry = package().iId;

    delete opert; opert=NULL;
    }

void MmsTestBed::cleanOutbox()
    {
    TMmsTestUtils::CleanBoxL(KMsvGlobalOutBoxIndexEntryId, *iSession);
    }

void MmsTestBed::cleanInbox()
    {
    TMmsTestUtils::CleanBoxL(KMsvGlobalInBoxIndexEntryId, *iSession);
    }

void MmsTestBed::cleanSent()
    {
    TMmsTestUtils::CleanBoxL(KMsvSentEntryId, *iSession);
    }

void MmsTestBed::cleanDrafts()
    {
    TMmsTestUtils::CleanBoxL(KMsvDraftEntryId, *iSession);
    }

void MmsTestBed::cleanAll()
    {
    cleanInbox();
    cleanOutbox();
    cleanSent();
    cleanDrafts();
    TMmsTestUtils::CleanBoxL( iSettings->MMBoxFolder(), *iSession );
    deleteNotifications();
    }

void MmsTestBed::reply()
    {
    // The first message from inbox is replied to
    CMsvEntry* cEntry = iSession->GetEntryL(KMsvGlobalInBoxIndexEntryId);
    CleanupStack::PushL(cEntry);
    delete iMsvEntrySelection;
    iMsvEntrySelection = NULL;
    iMsvEntrySelection = cEntry->ChildrenWithMtmL( KUidMsgTypeMultimedia );
    if ( iMsvEntrySelection->Count() > 0 )
        {
        TMsvId originalEntry = iMsvEntrySelection->At(0);
        iMmsClient->SwitchCurrentEntryL( originalEntry );
        TMsvPartList partList = KMsvMessagePartOriginator | KMsvMessagePartDescription;
        CMsvOperation * op = NULL;
        op = iMmsClient->ReplyL( KMsvGlobalOutBoxIndexEntryId, partList, iWait->iStatus);
        CleanupStack::PushL(op);
        iWait->Start();
        while (iWait->iStatus.Int() == KRequestPending )
            {
            if (!iTimer->IsActive())
                {
                iTimer->IssueRequest();
                }
            CActiveScheduler::Start();
            }

        iTimer->Cancel();
        if ( iWait->iStatus.Int() != KErrNone )
            {
            //TMmsLogger::Log(_L("Create reply, return status %d"), iWait->iStatus.Int());
            }
        else
            {
            TMsvId newEntry = KMsvNullIndexEntryId;
            TPckgBuf<TMsvId> package;
            package.Copy(op->ProgressL());
            newEntry = package();
            if ( newEntry != KMsvNullIndexEntryId )
                {
                cEntry->SetEntryL( newEntry );
                TMsvEntry entry = cEntry->Entry();
                entry.SetInPreparation( EFalse );
                entry.SetVisible( ETrue );
                cEntry->ChangeL(entry);
                }
            }
        CleanupStack::PopAndDestroy(); // op
        }
    CleanupStack::PopAndDestroy(); // cEntry
    }

void MmsTestBed::replyToAll()
    {
    // The first message from inbox is replied to
    CMsvEntry* cEntry = iSession->GetEntryL(KMsvGlobalInBoxIndexEntryId);
    CleanupStack::PushL(cEntry);
    delete iMsvEntrySelection;
    iMsvEntrySelection = NULL;
    iMsvEntrySelection = cEntry->ChildrenWithMtmL(KUidMsgTypeMultimedia);

    if ( iMsvEntrySelection->Count() > 0 )
        {
        TMsvId originalEntry = iMsvEntrySelection->At(0);
        iMmsClient->SwitchCurrentEntryL( originalEntry );
        TMsvPartList partList = KMsvMessagePartOriginator | KMsvMessagePartDescription |
            KMsvMessagePartRecipient;
        CMsvOperation * op = NULL;
        op = iMmsClient->ReplyL( KMsvGlobalOutBoxIndexEntryId, partList, iWait->iStatus);
        CleanupStack::PushL(op);
        iWait->Start();
        while (iWait->iStatus.Int() == KRequestPending )
            {
            if (!iTimer->IsActive())
                {
                iTimer->IssueRequest();
                }
            CActiveScheduler::Start();
            }

        iTimer->Cancel();
        if ( iWait->iStatus.Int() != KErrNone )
            {
            //TMmsLogger::Log(_L("Create reply to all, return status %d"), iWait->iStatus.Int());
            }
        else
            {
            TMsvId newEntry = KMsvNullIndexEntryId;
            TPckgBuf<TMsvId> package;
            package.Copy(op->ProgressL());
            newEntry = package();
            if ( newEntry != KMsvNullIndexEntryId )
                {
                cEntry->SetEntryL( newEntry );
                TMsvEntry entry = cEntry->Entry();
                entry.SetInPreparation( EFalse );
                entry.SetVisible( ETrue );
                cEntry->ChangeL(entry);
                }
            }
        CleanupStack::PopAndDestroy(); // op
        }
    CleanupStack::PopAndDestroy(); // cEntry
    }

void MmsTestBed::forward()
    {
    // The first message from inbox is forwarded
    CMsvEntry* cEntry = iSession->GetEntryL(KMsvGlobalInBoxIndexEntryId);
    CleanupStack::PushL(cEntry);
    delete iMsvEntrySelection;
    iMsvEntrySelection = NULL;
    iMsvEntrySelection = cEntry->ChildrenWithMtmL(KUidMsgTypeMultimedia);

    if ( iMsvEntrySelection->Count() > 0 )
        {
        TMsvId originalEntry = iMsvEntrySelection->At(0);
        iMmsClient->SwitchCurrentEntryL( originalEntry );
        TMsvPartList partList = KMsvMessagePartOriginator | KMsvMessagePartDescription |
            KMsvMessagePartRecipient | KMsvMessagePartAttachments;
        CMsvOperation * op = NULL;
        op = iMmsClient->ForwardL( KMsvGlobalOutBoxIndexEntryId, partList, iWait->iStatus);
        CleanupStack::PushL(op);
        iWait->Start();
        while (iWait->iStatus.Int() == KRequestPending )
            {
            if (!iTimer->IsActive())
                {
                iTimer->IssueRequest();
                }
            CActiveScheduler::Start();
            }

        iTimer->Cancel();
        if ( iWait->iStatus.Int() != KErrNone )
            {
            //TMmsLogger::Log(_L("Create forward, return status %d"), iWait->iStatus.Int());
            }
        else
            {
            TMsvId newEntry = KMsvNullIndexEntryId;
            TPckgBuf<TMsvId> package;
            package.Copy(op->ProgressL());
            newEntry = package();
            if ( newEntry != KMsvNullIndexEntryId )
                {
                iMmsClient->SwitchCurrentEntryL( newEntry );
                // Add a sender: 0601234567
                iMmsClient->LoadMessageL();
                iMmsClient->SetSenderL( KMmsSender );
                iMmsClient->SaveMessageL();
                cEntry->SetEntryL( newEntry );
                TMsvEntry entry = cEntry->Entry();
                entry.SetInPreparation( EFalse );
                entry.SetVisible( ETrue );
                cEntry->ChangeL(entry);
                }
            }
        CleanupStack::PopAndDestroy(); // op
        }
    CleanupStack::PopAndDestroy(); // cEntry
    }

void MmsTestBed::sendReadReport()
    {
    // TURN READ REPORTS ON (until available from menu)
    iSettings->LoadSettingsL();
    iSettings->SetReadReplyReportSendingAllowed( ETrue );
    iSettings->SaveSettingsL();
    iMmsClient->RestoreSettingsL();


    // Read report is sent for the first message in inbox
    CMsvEntry* cEntry = iSession->GetEntryL( KMsvGlobalInBoxIndexEntryId );
    CleanupStack::PushL(cEntry);
    delete iMsvEntrySelection;
    iMsvEntrySelection = NULL;
    iMsvEntrySelection = cEntry->ChildrenWithMtmL( KUidMsgTypeMultimedia );

    if ( iMsvEntrySelection->Count() == 0 )
        {
        return;
        }
    TMsvId originalEntry = iMsvEntrySelection->At( 0 );

    // new test using Client MTM
    CMsvOperation * op = NULL;
    op = iMmsClient->SendReadReportL( originalEntry, iWait->iStatus, EMmsReadStatusRead );
    CleanupStack::PushL(op);
    iWait->Start();

    while (iWait->iStatus.Int() == KRequestPending )
        {
        if (!iTimer->IsActive())
            {
            iTimer->IssueRequest();
            }
        CActiveScheduler::Start();
        }

    if ( iWait->iStatus.Int() != KErrNone )
        {
        //TMmsLogger::Log(_L("Testbed tried to send read report, return status %d"), iWait->iStatus.Int());
        }

    iTimer->Cancel();

    CleanupStack::PopAndDestroy( op );
    op = NULL;

    // Try sending the read report to current entry

    iMmsClient->SwitchCurrentEntryL( originalEntry );
    op = iMmsClient->SendReadReportL( originalEntry, iWait->iStatus, EMmsReadStatusRead );
    CleanupStack::PushL(op);
    iWait->Start();

    while (iWait->iStatus.Int() == KRequestPending )
        {
        if (!iTimer->IsActive())
            {
            iTimer->IssueRequest();
            }
        CActiveScheduler::Start();
        }

    if ( iWait->iStatus.Int() != KErrNone )
        {
        //TMmsLogger::Log(_L("Testbed tried to send read report, return status %d"), iWait->iStatus.Int());
        }

    iTimer->Cancel();
    CleanupStack::PopAndDestroy( op );
    op = NULL;

/*
    cEntry->SetEntryL( originalEntry );
    iMmsHeaders->Reset();
    CMsvStore* store = cEntry->ReadStoreL();
    CleanupStack::PushL( store );
    iMmsHeaders->RestoreL( *store );
    CleanupStack::PopAndDestroy( store );
    store = NULL;
    HBufC8* messageId = HBufC8::NewL( iMmsHeaders->MessageId().Length() );
    CleanupStack::PushL( messageId );
    messageId->Des().Copy( iMmsHeaders->MessageId() );

    iMmsClient->SwitchCurrentEntryL( originalEntry );
    iMmsClient->LoadMessageL();

    iMmsHeaders->Reset();
    iMmsHeaders->SetMessageType( KMmsMessageTypeReadRecInd );
    // Message id cannot be accessed via MMS Client MTM!

    iMmsHeaders->SetMessageIdL( messageId->Des() );
    CleanupStack::PopAndDestroy( messageId );

    iMmsHeaders->AddTypedAddresseeL( iMmsClient->Sender(), EMsvRecipientTo );
    // sender must be insert-address-token because we don't know our number
    TTime now;
    now.UniversalTime();

    _LIT( K1970, "19700000:000000.000000" );    // 1-Jan 1970 0:00:00

    TTime y1970( K1970 );
    TTimeIntervalMicroSeconds interval;
    // we can't use "seconds from" as it only returns a
    // 32 bit signed integer. If fails in 2038.
    // "microseconds from" returns a 64 bit signed integer
    interval = now.MicroSecondsFrom( y1970 );
    // date in iMmsHeaders() in seconds from 1.1.1970.
    iMmsHeaders->SetDate( (interval.Int64() ) / KMmsMillion );
    iMmsHeaders->SetReadStatus( KMmsReadStatusRead );

    TMsvId mmsFolderId = KMsvNullIndexEntryId;
    mmsFolderId = FindMMSFolderL();

    cEntry->SetEntryL( mmsFolderId );

    TMsvEntry entry;
    entry.iType = KUidMsvMessageEntry;
    entry.iMtm = KUidMsgTypeMultimedia;
    entry.SetVisible( ETrue );
    entry.SetInPreparation( EFalse );
    entry.iServiceId = KMsvLocalServiceIndexEntryId;
    entry.iRelatedId = iDefaultServiceId;
    entry.iMtmData1 = KMmsMessageReadRecInd;
    cEntry->CreateL( entry );
    TMsvId entryId = entry.Id();

    cEntry->SetEntryL( entryId );

    store = cEntry->EditStoreL();
    CleanupStack::PushL( store );
    iMmsHeaders->StoreL( *store );
    store->CommitL();
    CleanupStack::PopAndDestroy( store );
    store = NULL;

    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    selection->InsertL(0, entryId);

    CMsvOperation * op = NULL;
    TCommandParameters parameters; // initialized to zero
    TCommandParametersBuf paramPack( parameters );

    op = iSession->TransferCommandL(*selection,EMmsScheduledReadReport,paramPack,iWait->iStatus);

    CleanupStack::PushL(op);
    iWait->Start();

    while (iWait->iStatus.Int() == KRequestPending )
        {
        if (!iTimer->IsActive())
            {
            iTimer->IssueRequest();
            }
        CActiveScheduler::Start();
        }

    if ( iWait->iStatus.Int() != KErrNone )
        {
        //TMmsLogger::Log(_L("Testbed tried to send read report, return status %d"), iWait->iStatus.Int());
        }

    iTimer->Cancel();

    CleanupStack::PopAndDestroy( op );
    CleanupStack::PopAndDestroy( selection );
*/
    CleanupStack::PopAndDestroy( cEntry );
    }

int MmsTestBed::fromOutboxToMmscWithMemoryFailure()
    {
    TInt error = KErrNone;
    TInt messageCount = 0;
    TInt failureCount = 0;
    do {
        CMsvEntry* cEntry = NULL;

        // Get List of services
        cEntry = iSession->GetEntryL(KMsvGlobalOutBoxIndexEntryIdValue);
        CleanupStack::PushL(cEntry);
        // Get all mms messages of outbox
        CMsvEntrySelection* selection = cEntry->ChildrenWithMtmL( KUidMsgTypeMultimedia );
        CleanupStack::PushL( selection );
        failureCount++;
        // These are failures that fail in Message server.
        if ( failureCount >= 63 && failureCount <= 64 )
            {
            failureCount = 65;
            }
        cEntry->SetEntryL( iServiceId );
        TMsvEntry entry = cEntry->Entry();
        entry.iMtmData3 &= 0x0000000FF;
        entry.iMtmData3 |= failureCount << 8;
        cEntry->ChangeL( entry );

        selection->InsertL(0, iServiceId);

        CMsvOperation * op = NULL;
        TCommandParameters parameters; // initialized to zero
        TCommandParametersBuf paramPack( parameters );

        TRAP (error, op = iSession->TransferCommandL(*selection,EMmsSend,paramPack,iWait->iStatus));

        if ( error == KErrNone )
            {
            CleanupStack::PushL(op);
            iWait->Start();

            while (iWait->iStatus.Int() == KRequestPending )
                {
                if (!iTimer->IsActive())
                    {
                    iTimer->IssueRequest();
                    }
                CActiveScheduler::Start();
                }

            if ( iWait->iStatus.Int() != KErrNone )
                {
                error = iWait->iStatus.Int();
                if ( iWait->iStatus.Int() != KErrNoMemory )
                    {
                    //TMmsLogger::Log(_L("Testbed tried to send, return status %d"), iWait->iStatus.Int());
                    }
                }

            CleanupStack::PopAndDestroy(); // op
            }
        iTimer->Cancel();
        CleanupStack::PopAndDestroy(); // selection
        CleanupStack::PopAndDestroy(); // cEntry
        selection = NULL;
        cEntry = NULL;
        cEntry = iSession->GetEntryL( KMsvGlobalOutBoxIndexEntryIdValue );
        CleanupStack::PushL(cEntry);
        selection = cEntry->ChildrenWithMtmL( KUidMsgTypeMultimedia );
        messageCount = selection->Count();
        delete selection;
        selection = 0;
        CleanupStack::PopAndDestroy(); //cEntry
        }
    while ( error == KErrNoMemory || messageCount > 0 );
    return failureCount;
    }

int MmsTestBed::fromMmscToInboxWithMemoryFailure()
    {
    TInt error = KErrNone;
    TInt failureCount = 0;
    TInt messageCount = 0;
    TMsvId mmsFolderId = KMsvNullIndexEntryId;
    mmsFolderId = findMMSFolder();

    do {
        // Inbox must be cleaned if failure makes message to be fetched more than once.
        // should not happen, but this test is quite stressful, and perfect result
        // cannot be guaranteed.
        // What is expected:
        // 1. Program does not crash
        // 2. After sufficient number of retries the message is fetched and notification deleted.
        TMmsTestUtils::CleanBoxL(KMsvGlobalInBoxIndexEntryId, *iSession);
        CMsvEntry* cEntry = NULL;
        CMsvEntrySelection* msvEntrySelection = new CMsvEntrySelection;
        CleanupStack::PushL(msvEntrySelection);

        cEntry = iSession->GetEntryL(iServiceId);
        CleanupStack::PushL(cEntry);

        failureCount++;
        // These are failures that fail in Message server.
        if ( failureCount >= 63 && failureCount <= 64 )
            {
            failureCount = 65;
            }
        cEntry->SetEntryL( iServiceId );

        TMsvEntry entry = cEntry->Entry();
        entry.iMtmData3 &= 0x0000000FF;
        entry.iMtmData3 |= failureCount << 8;
        cEntry->ChangeL( entry );

        // if we have a selected service, insert it into selection
        if (iServiceId != KMsvNullIndexEntryId)
            {
            msvEntrySelection->InsertL(0, iServiceId);
            }

        CMsvOperation * op = NULL;
        TCommandParameters parameters; // initialized to zero
        TCommandParametersBuf paramPack( parameters );

        TRAP (error, op = iMmsClient->InvokeAsyncFunctionL(
            EMmsReceiveForced,
            *msvEntrySelection,
            paramPack,
            iWait->iStatus) );

        if ( error == KErrNone )
            {
            CleanupStack::PushL(op);
            iWait->Start();

            while (iWait->iStatus.Int() == KRequestPending )
                {
                if (!iTimer->IsActive())
                    {
                    iTimer->IssueRequest();
                    }
                CActiveScheduler::Start();
                }

            if ( iWait->iStatus.Int() != KErrNone )
                {
                error = iWait->iStatus.Int();
                if ( iWait->iStatus.Int() != KErrNoMemory )
                    {
                    //TMmsLogger::Log(_L("Testbed tried to receive, return status %d"), iWait->iStatus.Int());
                    }
                }

            CleanupStack::PopAndDestroy(); // op
            }

        iTimer->Cancel();
        CleanupStack::PopAndDestroy(); // msvEntrySelection
        CleanupStack::PopAndDestroy(); // cEntry
        msvEntrySelection = NULL;
        cEntry = NULL;
        cEntry = iSession->GetEntryL( mmsFolderId );
        CleanupStack::PushL(cEntry);
        msvEntrySelection = cEntry->ChildrenWithMtmL( KUidMsgTypeMultimedia );
        messageCount = msvEntrySelection->Count();
        delete msvEntrySelection;
        msvEntrySelection = 0;
        CleanupStack::PopAndDestroy(); //cEntry
        }
    while ( error == KErrNoMemory || messageCount > 0 );
    if ( error != KErrNoMemory && error != KErrNone )
        {
        //TMmsLogger::Log(_L("Memory failure loop ended with %d"), error);
        }
    return failureCount;
    }

void MmsTestBed::sendViaClient()
    {
    CMsvEntry* cEntry = NULL;

    cEntry = iSession->GetEntryL(KMsvDraftEntryId);
    CleanupStack::PushL(cEntry);
    // Get all mms messages in drafts
    CMsvEntrySelection* selection = cEntry->ChildrenWithMtmL( KUidMsgTypeMultimedia );
    CleanupStack::PushL( selection );

    CMsvOperation * op = NULL;
    TTime now;
    now.UniversalTime();
    op = iMmsClient->SendL( *selection, iWait->iStatus, now );

    CleanupStack::PushL(op);
    iWait->Start();

    while (iWait->iStatus.Int() == KRequestPending )
        {
        if (!iTimer->IsActive())
            {
            iTimer->IssueRequest();
            }
        CActiveScheduler::Start();
        }

    iTimer->Cancel();
    CleanupStack::PopAndDestroy(); // op
    CleanupStack::PopAndDestroy(2); // cEntry, selection
    }

void MmsTestBed::scheduledFetch(TInt aDelay /* = 5 */)
    {
    TCommandParameters parameters;
    parameters.iInitialDelay = aDelay;
    TCommandParametersBuf paramPack( parameters );

    CMsvEntrySelection* selection = new CMsvEntrySelection;
    CleanupStack::PushL( selection );
    // if we have a selected service, insert it into selection
    if (iServiceId != KMsvNullIndexEntryId)
        {
        selection->InsertL(0, iServiceId);
        }
    else
        {
        selection->InsertL(0, iDefaultServiceId);
        }

    CMsvOperation * op = NULL;
    op = iMmsClient->InvokeAsyncFunctionL(
        EMmsScheduledReceive,
        *selection,
        paramPack,
        iWait->iStatus);

    CleanupStack::PushL(op);
    iWait->Start();

    while (iWait->iStatus.Int() == KRequestPending )
        {
        if (!iTimer->IsActive())
            {
            iTimer->IssueRequest();
            }
        CActiveScheduler::Start();
        }

    iTimer->Cancel();
    CleanupStack::PopAndDestroy(); // op
    CleanupStack::PopAndDestroy(); // selection
    }

void MmsTestBed::fetchForcedImmediate()
    {
    TCommandParameters parameters;
    TCommandParametersBuf paramPack( parameters );

    CMsvEntrySelection* selection = new CMsvEntrySelection;
    CleanupStack::PushL( selection );
    // if we have a selected service, insert it into selection
    if (iServiceId != KMsvNullIndexEntryId)
        {
        selection->InsertL(0, iServiceId);
        }
    else
        {
        selection->InsertL(0, iDefaultServiceId);
        }


    CMsvOperation * op = NULL;
    op = iMmsClient->InvokeAsyncFunctionL(
        EMmsReceiveForced,
        *selection,
        paramPack,
        iWait->iStatus);

    CleanupStack::PushL(op);
    iWait->Start();

    while (iWait->iStatus.Int() == KRequestPending )
        {
        if (!iTimer->IsActive())
            {
            iTimer->IssueRequest();
            }
        CActiveScheduler::Start();
        }

    iTimer->Cancel();
    CleanupStack::PopAndDestroy(); // op
    CleanupStack::PopAndDestroy(); // selection
    }

void MmsTestBed::getEventType()
    {
    if ( !checkLogClient() )
        {
        return; // not available
        }

    CLogEventType* eventType = CLogEventType::NewL();
    CleanupStack::PushL( eventType );
    eventType->SetUid(iLogEvent->EventType());
    iLogClient->GetEventType( *eventType, iWait->iStatus );

    iWait->Start();
    CleanupStack::PopAndDestroy(); // eventType
    }

void MmsTestBed::addEventType()
    {
    if ( !checkLogClient() )
        {
        return; // not available
        }

    CLogEventType* eventType = CLogEventType::NewL();
    CleanupStack::PushL( eventType );
    eventType->SetUid(iLogEvent->EventType());
    eventType->SetDescription(_L("Multimedia Message") );
    eventType->SetLoggingEnabled( ETrue );

//    iWait->iStatus = KRequestPending;
    iLogClient->AddEventType( *eventType, iWait->iStatus );
    iWait->Start();
    CleanupStack::PopAndDestroy(); // eventType
    }

void MmsTestBed::deleteEventType()
    {
    if ( !checkLogClient() )
        {
        return; // not available
        }
//    iWait->iStatus = KRequestPending;
    iLogClient->DeleteEventType( iLogEvent->EventType(), iWait->iStatus );
    iWait->Start();
    }

void MmsTestBed::cleanLog()
    {
    TInt count = 0;

    count = getLogEntries();

    TInt i;
    for ( i = 0; i < count; ++i )
        {
        // when a view is created, it will be positioned on the first event
//        iWait->iStatus = KRequestPending;
        iLogClient->DeleteEvent(iLogView->Event().Id(), iWait->iStatus);
        iWait->Start();

        if ( iLogView->NextL( iWait->iStatus ) )
            {
            // should complete with KErrNone
            iWait->Start();
            if ( iWait->iStatus.Int() != KErrNone )
                {
                // could not get next event!
                break;
                }
            }
        else
            {
            break; // no more events!
            }
        }
    }

void MmsTestBed::setOnline(bool value)
    {
    //value = true for online mode
    //value = false for offline mode
    CRepository* repository = NULL;
    TInt retval = KErrNone;
    TRAP( retval, repository = CRepository::NewL( KCRUidCoreApplicationUIs ) );
    if( retval == KErrNone )
        {
        repository->Set( KCoreAppUIsNetworkConnectionAllowed, value );
        delete repository;
        }
    }

void MmsTestBed::generateDeliveryReport( CMmsHeaders* aMmsHeaders )
    {
    iEncodeBuffer->ResizeL( 1024 );

    TInt position = 0;

    // encode message type
    iEncodeBuffer->Write( position, &KMmsAssignedMessageType, 1 );
    position++;
    iEncodeBuffer->Write( position, &KMmsMessageTypeDeliveryInd, 1 );
    position++;

    // version
    iEncodeBuffer->Write( position, &KMmsAssignedMmsVersion, 1 );
    position++;

    TUint8 version = aMmsHeaders->MmsVersion() | 0x80; // current version as short integer
    iEncodeBuffer->Write( position, &version, 1 );
    position++;

    // message id from the headers
    // At least once MMSC did not send us the message id!
    // if message id is missing, we cannot match the entry!
    // This only a fake. Real delivery reports should always
    // contain the message id.
    if ( aMmsHeaders->MessageId().Length() > 0 )
        {
        iEncodeBuffer->Write( position, &KMmsAssignedMessageId, 1 );
        position++;
        iEncodeBuffer->Write( position, &(aMmsHeaders->MessageId()[0]), aMmsHeaders->MessageId().Length() );
        position += aMmsHeaders->MessageId().Length();
        iEncodeBuffer->Write( position, &KMmsNull, 1 );
        position++;
        }

    // To is taken from the headers
    iEncodeBuffer->Write( position, &KMmsAssignedTo, 1 );
    position++;

    TPtrC recipient;
    if ( aMmsHeaders->ToRecipients().MdcaCount() > 0 &&
        aMmsHeaders->ToRecipients()[0].Length() > 0 )
        {
        recipient.Set( aMmsHeaders->ToRecipients()[0] );
        }
    else if( aMmsHeaders->CcRecipients().MdcaCount() > 0 &&
        aMmsHeaders->CcRecipients()[0].Length() > 0 )
        {
        recipient.Set( aMmsHeaders->CcRecipients()[0] );
        }
    else if( aMmsHeaders->BccRecipients().MdcaCount() > 0 &&
        aMmsHeaders->BccRecipients()[0].Length() > 0 )
        {
        recipient.Set( aMmsHeaders->BccRecipients()[0] );
        }
    else
        {
        recipient.Set( _L("Jasso-Kissa@jii.fi") );
        }

    TMmsAddressType addressType = EMmsAddressTypeUnknown;

    if ( recipient.Find( KMiuMau ) != KErrNotFound )
        {
        addressType = EMmsAddressTypeEmail;
        }
    else
        {
        addressType = EMmsAddressTypeMobile;
        }

    TUint8 character;
    TInt i;
    if ( addressType == EMmsAddressTypeEmail )
        {
        // email address - ASCII ONLY - THIS IS JUST A TEST!

        for ( i = 0; i < recipient.Length(); ++i )
            {
            character = TUint8( recipient[i] & 0xff );
            iEncodeBuffer->Write( position, &character, 1 );
            position++;
            }
        iEncodeBuffer->Write( position, &KMmsNull, 1 );
        position++;

        }
    else
        {
        // must be a phone number
        // We expect for now that the format is correct as is
        // All legal characters present in a phone number are ASCII

        TInt i;
        for ( i = 0; i < recipient.Length(); ++i )
            {
            character = TUint8( recipient[i] & 0xff );
            iEncodeBuffer->Write( position, &character, 1 );
            position++;
            }
        iEncodeBuffer->Write( position, KMmsPlmn, KMmsPlmnLength );
        position += KMmsPlmnLength;
        iEncodeBuffer->Write( position, &KMmsNull, 1 );
        position++;
        }

    // date
    iEncodeBuffer->Write( position, &KMmsAssignedDate, 1 );
    position++;

    TLocale locale;
    locale.Refresh();
    TInt64 UtcDate;
    TTimeIntervalSeconds universalTimeOffset( locale.UniversalTimeOffset() );

    TTime now;
    now.UniversalTime();
    UtcDate = ( now.MicroSecondsFrom( TTime( KMmsYear1970String ) ).Int64() ) / 1000000 ;

    UtcDate -= universalTimeOffset.Int();

    if ( locale.QueryHomeHasDaylightSavingOn() )
        {
        TTimeIntervalSeconds daylightSaving( 60 * 60 );
        UtcDate -= daylightSaving.Int();
        }

    TUint8 len; // number of bytes we will need
    len = 0;
    TUint8 array[8];

    TInt64 temp = UtcDate;

    for (i = 7; i >= 0; --i)
        {
        array[i] = TInt8( ( I64INT( temp ) ) & 0xFF );
        I64LSR( temp, 8 );
        }

    len = 8;
    i = 0;
    while( ( array[i]== 0 ) && ( i < 8 ) )
        {
        i++;
        len--;
        }

    // a zero should be coded as short integer.
    // However, if there is a valid reason to code a zero as a long integer,
    // we allow it. The caller should know what he is doing.
    if ( len == 0 )
        {
        len = 1;
        }
    // write short length
    iEncodeBuffer->Write( position, &len, 1 );
    position++;
    // write as many bytes as were non-zero
    iEncodeBuffer->Write( position, &(array[8 - len] ), len );
    position+= len;
    // status
    iEncodeBuffer->Write( position, &KMmsAssignedStatus, 1 );
    position++;
    iEncodeBuffer->Write( position, &KMmsMessageStatusRetrieved, 1 );
    position++;
    // DONE!!!
    iEncodeBuffer->ResizeL( position );
    return;
    }

void MmsTestBed::sendDeliveryReport()
    {
    if ( iEncodeBuffer->Size() == 0 )
        {
        //No delivery report
        return;
        }


    TMsvId mmsFolder = findMMSFolder();
    CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
    CleanupStack::PushL( selection );

    TMsvId entryId = TMmsTestUtils::CreateNotificationEntryL( mmsFolder, iServiceId, iEncodeBuffer, *iSession );
    TPtr8 ptr = iEncodeBuffer->Ptr( 0 );

    // Now we have streamed our data into this entry.
    // Now we have an entry that says: local service, MMS MTM

    if ( entryId != KMsvNullIndexEntryId )
        {
        selection->AppendL( entryId );
        }
    else
        {
        selection->AppendL( iDefaultServiceId );
        }

    TWatcherParameters parameters; // initialized to zero
    parameters.iWatcherId = RThread().Id();
    parameters.iDataPointer = &ptr;
    TWatcherParametersBuf paramPack( parameters );

    CMsvOperation * op = NULL;

//    iWait->iStatus = KRequestPending;
    op = iSession->TransferCommandL(
        *selection, EMmsDecodePushedMessage, paramPack, iWait->iStatus );
    CleanupStack::PushL(op);
    iWait->Start();

    while (iWait->iStatus.Int() == KRequestPending )
        {
        if (!iTimer->IsActive())
            {
            iTimer->IssueRequest();
            }
        CActiveScheduler::Start();
        }

    iTimer->Cancel();

    CleanupStack::PopAndDestroy(); // op
    CleanupStack::PopAndDestroy(); // selection
    }

