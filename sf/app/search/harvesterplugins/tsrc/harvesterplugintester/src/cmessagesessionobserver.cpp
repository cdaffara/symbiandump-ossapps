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

#include "cmessagesessionobserver.h"
#include <mtclreg.h>
#include <smsclnt.h>
#include <txtrich.h>
#include <mmsclient.h>
#include <cmsvmimeheaders.h>
#include <cmsvattachment.h>
#include <smtcmtm.h>


CMessageSessionObserver* CMessageSessionObserver::NewL()
    {
    CMessageSessionObserver* self = CMessageSessionObserver::NewLC();
    CleanupStack::Pop();
    return self;
    }

CMessageSessionObserver* CMessageSessionObserver::NewLC()
    {
    CMessageSessionObserver* self = new (ELeave) CMessageSessionObserver();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CMessageSessionObserver::ConstructL()
    {
    }

CMessageSessionObserver::CMessageSessionObserver()
    {
    
    }

CMessageSessionObserver::~CMessageSessionObserver()
    {
    
    }

void CMessageSessionObserver::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, 
                                                  TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    //Dummy implementation    
    }

CMessAsyncWaiter* CMessAsyncWaiter::NewL()
    {
    CMessAsyncWaiter* self = new(ELeave) CMessAsyncWaiter();
    return self;
    }

//Adds the active object to the scheduler
CMessAsyncWaiter::CMessAsyncWaiter() : CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

CMessAsyncWaiter::~CMessAsyncWaiter()
    {
    Cancel();
    }    

//Indicates that the active object has issued a request and that it is now outstanding
void CMessAsyncWaiter::StartAndWait()
    {
    SetActive();
    CActiveScheduler::Start();
    }  

TInt CMessAsyncWaiter::Result() const
    {
    return iError;
    }
    
/**
Handles an active object’s request completion event.
It sets to run the desired task.
*/ 
void CMessAsyncWaiter::RunL()
    {
    iError = iStatus.Int();
    CActiveScheduler::Stop();
    }

//Implements cancellation of an outstanding request,called as part of the active object's Cancel()
void CMessAsyncWaiter::DoCancel()
    {
    iError = KErrCancel;
    CActiveScheduler::Stop();
    }

TInt MessagingUtils::CreateMessageL(CMsvSession* aMsgSession, const TDesC& aMsgFrom,
                                    const TDesC& aMsgTo,const TDesC& aMsgBody)
    {
    CSmsClientMtm* smsMtm = NULL;
    TMsvId newMsgId = NULL;
    CClientMtmRegistry* mtmReg = CClientMtmRegistry::NewL( *aMsgSession );
    TRAPD(err,smsMtm = static_cast<CSmsClientMtm*>( mtmReg->NewMtmL( KUidMsgTypeSMS )));
    if( err == KErrNone)
        {
        smsMtm->SwitchCurrentEntryL( KMsvGlobalInBoxIndexEntryId );
        smsMtm->CreateMessageL( KUidMsgTypeSMS.iUid );
        newMsgId = smsMtm->Entry().EntryId();
        smsMtm->AddAddresseeL( aMsgTo ); //to address if need
        CMsvStore* messageStore = smsMtm->Entry().EditStoreL();
        CleanupStack::PushL( messageStore );
        CSmsHeader& header = smsMtm->SmsHeader();
        header.SetFromAddressL(aMsgFrom);
        header.StoreL( *messageStore );
        CleanupStack::PopAndDestroy( messageStore );
        CRichText& body = smsMtm->Body();
        body.Reset();
        // Put 16bits stream stored in 8bits buffer back to 16bits buffer.
        body.InsertL( 0, aMsgBody);
        TMsvEntry entry = smsMtm->Entry().Entry();
        entry.SetInPreparation( EFalse );
        entry.SetVisible( ETrue );
        entry.iDate.HomeTime();
        entry.iDetails.Set(aMsgFrom);
        entry.SetUnread( EFalse );
        smsMtm->Entry().ChangeL( entry );
        smsMtm->SaveMessageL();
        }
    delete smsMtm;
    delete mtmReg;
    return newMsgId;
    }

TInt MessagingUtils::RemoveEntryL(CMsvSession* aMsgSession,TMsvId aMsgId)
    {
    aMsgSession->RemoveEntry(aMsgId);
    return KErrNone;
    }

TInt MessagingUtils::CreateMmsMessageL(CMsvSession* aMsgSession, const TDesC& aMsgFrom,
                                        const TDesC& aMsgTo,const TDesC& afilename)
    {
    _LIT(KMessageSubject, "MMS Test");
    CClientMtmRegistry* mtmReg = CClientMtmRegistry::NewL( *aMsgSession );
    CMmsClientMtm* iMmsClient = static_cast<CMmsClientMtm*>( mtmReg->NewMtmL( KUidMsgTypeMultimedia ));
    
    iMmsClient->SwitchCurrentEntryL( KMsvGlobalInBoxIndexEntryId );
    iMmsClient->CreateMessageL( KUidMsgTypeMultimedia.iUid );
    TMsvId newMsgId = iMmsClient->Entry().EntryId();
    iMmsClient->AddAddresseeL( aMsgTo ); //to address if need    
    iMmsClient->SetSubjectL(KMessageSubject);

    CMsvStore* store = iMmsClient->Entry().EditStoreL();
    CleanupStack::PushL( store );    
    
    CMsvAttachment* attaInfo = NULL;
    TMsvAttachmentId attaId = KMsvNullIndexEntryId;    
    
    RFs iFSsession;
    User::LeaveIfError( iFSsession.Connect());
    RFile testfile;
    
    CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
    CleanupStack::PushL( mimeHeaders );    
   
    mimeHeaders->SetContentTypeL( _L8("text") );
    mimeHeaders->SetContentSubTypeL( _L8("plain") );
    mimeHeaders->SetMimeCharset( KMmsUtf8 );
    mimeHeaders->SetSuggestedFilenameL( _L("body.txt") );

    _LIT8(KMimeType, "text/plain");
    TBufC8<10> mimeType(KMimeType);
    // CreateAttachment2L will set the content type to attachment Info
    TInt error = testfile.Open(iFSsession, afilename ,EFileShareReadersOnly | EFileRead);
    
    CleanupClosePushL(testfile);
    
    attaInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
    // attaInfo ownerhip will be transferred to Attachment Manager
    // It must not be pushed onto the cleanupStack before calling CreateAttachment2L

    iMmsClient->CreateAttachment2L(
                *store,   // edit store
                testfile, // open file handle
                mimeType, 
                *mimeHeaders,
                attaInfo,
                attaId);
    // Now Attachment Manager owns the attaInfo
    attaInfo = NULL;

    CleanupStack::PopAndDestroy(); // testfile.Close()
    CleanupStack::PopAndDestroy(); // mimeHeaders
    
    store->CommitL();
    CleanupStack::PopAndDestroy(); // store
    
    TMsvEntry entry = iMmsClient->Entry().Entry();
    
    entry.SetInPreparation( EFalse );
    entry.SetVisible( ETrue );
    entry.iDate.HomeTime();
    entry.iDetails.Set(aMsgFrom);
    entry.SetUnread( EFalse );
    iMmsClient->Entry().ChangeL( entry );
    iMmsClient->SaveMessageL();
    delete iMmsClient;
    iMmsClient = NULL;
    delete mtmReg;
    return newMsgId;
    }

TInt MessagingUtils::RemoveMmsEntryL(CMsvSession* aMsgSession,TMsvId aMsgId)
    {
    CMsvEntry* cEntry = aMsgSession->GetEntryL( aMsgId );
    CleanupStack::PushL( cEntry );
    cEntry->SetEntryL( cEntry->Entry().Parent() );
    cEntry->DeleteL( aMsgId );
    CleanupStack::PopAndDestroy();
    return KErrNone;
    }

TInt MessagingUtils::CreateEmailEntryL(CMsvSession* aMsgSession)
    {
    _LIT8(KFrom, "test@email.in");
    _LIT(KSubject, "SampleEmail");
    _LIT(KBodyContents, "This is a Email entry");
    CClientMtmRegistry* mtmReg = CClientMtmRegistry::NewL( *aMsgSession );
    CSmtpClientMtm* iSmtpClient = static_cast<CSmtpClientMtm*>( mtmReg->NewMtmL( KUidMsgTypeSMTP ));
    
    TMsvId outboxId = KMsvGlobalOutBoxIndexEntryId; 
            
    // Set the context to the folder in which message has to be created
    CMsvEntry*  entry = CMsvEntry::NewL(*aMsgSession,outboxId,TMsvSelectionOrdering());
    CleanupStack::PushL(entry);
    entry->SetEntryL(outboxId);
    
    CMessAsyncWaiter* waiter = CMessAsyncWaiter::NewL();
    CleanupStack::PushL(waiter);
        
    TMsvEmailTypeList msvEmailTypeList = 0;
    TMsvPartList partList = (KMsvMessagePartBody | KMsvMessagePartAttachments);
        
    CImEmailOperation* emailOperation = CImEmailOperation::CreateNewL(waiter->iStatus, *aMsgSession, KMsvGlobalOutBoxIndexEntryId, partList, msvEmailTypeList, KUidMsgTypeSMTP);
    CleanupStack::PushL(emailOperation);
    waiter->StartAndWait();
        
    TMsvId temp;
    TPckgC<TMsvId> paramPack(temp);
    const TDesC8& progBuf = emailOperation->ProgressL();
    paramPack.Set(progBuf);
    TMsvId newMsgId;
    newMsgId = paramPack();    
    entry->SetEntryL(newMsgId);

    CMsvStore* store = entry->EditStoreL();
    CleanupStack::PushL(store);
    CImHeader* emailEntry = CImHeader::NewLC();
    //emailEntry->RestoreL(*store);
    emailEntry->SetFromL((TDesC8&)KFrom);
    emailEntry->SetSubjectL((TDesC&)KSubject);
        
    // Paragraph format layer for the rich text object 
    CParaFormatLayer* paraFormatLayer = CParaFormatLayer::NewL();
    CleanupStack::PushL(paraFormatLayer);
    // Character format layer for the rich text object
    CCharFormatLayer* charFormatLayer = CCharFormatLayer::NewL(); 
    CleanupStack::PushL(charFormatLayer);
    CRichText* bodyText = CRichText::NewL(paraFormatLayer, charFormatLayer, CEditableText::EFlatStorage, 256);
    CleanupStack::PushL(bodyText);
    // Inserts the contents of a buffer into the document at specified position
    bodyText->InsertL(0, KBodyContents);
    store->StoreBodyTextL(*bodyText);
    emailEntry->StoreL(*store);
    // Store the changes permanently
    store->CommitL();
    CleanupStack::PopAndDestroy(8,entry);
    
 
    delete iSmtpClient;
    iSmtpClient = NULL;
    delete mtmReg;
    return newMsgId;
    }

