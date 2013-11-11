/*
 * Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  CS Session class
 *
 */

// INCLUDE FILES
#include <ccsconversationentry.h>
#include <ccsclientconversation.h>

#include "ccsconversationevent.h"
#include "ccsdebug.h"
#include "ccsserver.h"
#include "ccssession.h"
#include "ccsplugininterface.h"
#include "ccsconversationcache.h"
#include "ccscontactsresolver.h"
#include "ccsconversationdeletehandler.h"
#include "ccsconversationmarkreadhandler.h"

// CONSTANTS
const TInt KTinyBuffer = 256; // 256 bytes
const TInt KBigBuffer = 2048; // 2K

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CCsSession::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CCsSession* CCsSession::NewL(CCsServer* aServer)
{
    PRINT ( _L("Enter CCsSession::NewL") );

    CCsSession* self = new (ELeave) CCsSession(aServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self); // self

    PRINT ( _L("End CCsSession::NewL") );

    return self;
}

// ----------------------------------------------------------------------------
// CCsSession::CCsSession
// Construtor
// ----------------------------------------------------------------------------
CCsSession::CCsSession(CCsServer* aServer) :
    iServer(aServer),
	    iDes(NULL),
	    iMonitoredConversation(NULL),
	    iBufferOverflow(EFalse),
	    iGetConversationBufferOverflow( EFalse),
	    iNotifyHandling(EFalse),
	    iConversationListChangeObserver(EFalse),
	    iConversationChangeObserver(EFalse),
	    iCachingChangeObserver(EFalse),
	    iReqCnt(1) //Let's start the event ID from 1    
{
}

// ----------------------------------------------------------------------------
// CCsSession::ConstructL
// Second phase constructor
// ----------------------------------------------------------------------------
void CCsSession::ConstructL()
{
    PRINT ( _L("Enter CCsSession::ConstructL") );


    // initialize the event List
    iEventList = new (ELeave) RPointerArray<CCsConversationEvent> ();

    PRINT ( _L("End CCsSession::ConstructL") );
}

// ----------------------------------------------------------------------------
// CCsSession::CCsSession
// Destructor
// ----------------------------------------------------------------------------
CCsSession::~CCsSession()
{
    PRINT ( _L("Enter CCsSession::~CCsSession") );

    if ( iDes )
        {
        delete iDes;
        iDes = NULL;
        }
		
    if ( iEventList )
        {
        iEventList->ResetAndDestroy();
        iEventList->Close();
        delete iEventList;
        iEventList = NULL;
    }

    if (iMonitoredConversation)
    {
        delete iMonitoredConversation;
        iMonitoredConversation = NULL;
    }

    PRINT ( _L("End CCsSession::~CCsSession") );
}

// ----------------------------------------------------------------------------
// CCsSession::ServiceL
//
// ----------------------------------------------------------------------------
void CCsSession::ServiceL(const RMessage2& aMessage)
{
    TInt errStatus = KErrNone;

    // Do the service
    TRAP ( errStatus, DoServiceL(aMessage) );

    // Check the error status returned
    if (errStatus != KErrNone)
    {
        // Free memory.
        if(iDes)
            {
            delete iDes;
            iDes=NULL;
            }
        aMessage.Complete(errStatus);
    }
}

// ----------------------------------------------------------------------------
// CCsSession::DoServiceL
//
// ----------------------------------------------------------------------------
void CCsSession::DoServiceL(const RMessage2& aMessage)
{
    switch (aMessage.Function())
    {
        case EGetConversationList:
            PRINT ( _L("Received function EGetConversationList") )
            GetConversationListL(aMessage);
            break;
            
        case EGetConversationUnreadList:
            PRINT ( _L("Received function EGetConversationUnreadList") )
            GetConversationUnreadListL(aMessage);
            break;

        case EGetConversations:
            PRINT ( _L("Received function EGetConversations") )
            GetConversationsL(aMessage);
            break;
            
        case EGetTotalUnreadCount:
            PRINT ( _L("Received function EGetTotalUnreadCount") )
            GetTotalUnreadCountL(aMessage);
            break;

        case ERequestChangeEvent:
            PRINT ( _L("Received function ERequestChangeEvent") )
            RequestChangeEventL(aMessage);
            break;

        case ERemoveChangeEvent:
            PRINT ( _L("Received function ERemoveChangeEvent") )
            RemoveChangeEventL(aMessage);
            break;

        case ESetConversationListChangeObserver:
            PRINT ( _L("Received function ESetConversationListChangeObserver") )
            SetConversationListChangeObserverL(aMessage);
            break;

        case EResetConversationListChangeObserver:
            PRINT ( _L("Received function EResetConversationListChangeObserver") )
            ResetConversationListChangeObserverL(aMessage);
            break;

        case ESetConversationChangeObserver:
            PRINT ( _L("Received function ESetConversationChangeObserver") )
            SetConversationChangeObserverL(aMessage);
            break;

        case EResetConversationChangeObserver:
            PRINT ( _L("Received function EResetConversationChangeObserver") )
            ResetConversationChangeObserverL(aMessage);
            break;

        case ESetCachingStatusObserver:
            PRINT ( _L("Received function ESetCachingStatusObserver") )
            SetCachingStatusObserverL(aMessage);
            break;

        case EResetCachingStatusObserver:
            PRINT ( _L("Received function EResetCachingStatusObserver") )
            ResetCachingStatusObserverL(aMessage);
            break;

        case EGetCachingStatus:
            GetCachingStatusL(aMessage);
            break;

        case EShutdown:
            PRINT ( _L("Received function EShutdown") )
            ShutdownServerL(aMessage);
            break;

        case EUserDeleteConversation:
            PRINT ( _L("Received function EDeleteConversation") )
            DeleteConversationL(aMessage);
            break;

        case EGetConversationId:
            PRINT ( _L("Received function EGetConversationId") )
            GetConversationIdL(aMessage);
            break;
            
        case EGetConversationIdFromAddress:
            PRINT ( _L("Received function EGetConversationIdFromAddress") )
            GetConversationIdfromAddressL(aMessage);
            break;

        case EGetConversationFromConversationId:
            PRINT ( _L("Received function EGetConversationFromConversationId") )
            GetConversationFromConversationIdL(aMessage);
            break;
			
		case EGetConversationFromMessageId:
            PRINT ( _L("Received function EGetConversationFromMessageId") )
            GetConversationFromMessageIdL(aMessage);
            break;

        case EUserMarkReadConversation:
            PRINT ( _L("Received function EUserMarkReadConversation") )
            MarkConversationReadL(aMessage);
            break;
    }

    return;
}

// ----------------------------------------------------------------------------
// CCsSession::ServiceError
//
// ----------------------------------------------------------------------------
void CCsSession::ServiceError(const RMessage2& aMessage, TInt aError)
{
    if(iDes)
        {
        delete iDes;
        iDes=NULL;
        }
    aMessage.Complete(aError);
}

// ----------------------------------------------------------------------------
// CCsSession::GetConversationEntryListL
// the function to handle the request of Listing
// of recent(latest) conversation entry and
// list of dispalyname for all stored conversation entry ID
// ----------------------------------------------------------------------------
void CCsSession::GetConversationListL(const RMessage2& aMessage)
{
    PRINT ( _L("Enter CCsSession::GetConversationListL") );
    PRINT_TIMESTAMP ("Enter CCsSession::GetConversationListL");

    if (iBufferOverflow == EFalse)
    {
        RPointerArray<CCsClientConversation>* ClientConversationList =
                new (ELeave) RPointerArray<CCsClientConversation> ();
        CleanupResetAndDestroyPushL(ClientConversationList);
        // get cache pointer
        CCsConversationCache* cache = iServer->ConversationCacheInterface();

        // Call cache function to get recent conversation entry list
        // with dispaly name for all stored conversation entry ID
        cache->GetConversationListL(ClientConversationList);

        //write all list data into stream
        // create a new buffer for writing into stream
        CBufFlat* buf = CBufFlat::NewL(KBigBuffer);
        CleanupStack::PushL(buf);

        RBufWriteStream writeStream(*buf);
        writeStream.PushL();

        TInt listCount = ClientConversationList->Count();

        if (listCount == 0)
        {
            iConversationListChangeObserver = ETrue;
        }

        // write the count first
        writeStream.WriteUint16L(listCount);

        // now go through the list and do externalize
        for (int iloop = 0; iloop < listCount; iloop++)
        {
            CCsClientConversation
                    * ClientConversation =
                            static_cast<CCsClientConversation*> ( (*ClientConversationList)[iloop]);
            //write list of ClientConversation
            ClientConversation->ExternalizeL(writeStream);
        }

        // Results are already packed in the stream
        writeStream.CommitL();

        // --------------------------------------------------------------
        // Create a heap descriptor from the buffer
        iDes = HBufC8::NewL(buf->Size());
        TPtr8 ptr(iDes->Des());
        buf->Read(0, ptr, buf->Size());

        // Cleanup
        CleanupStack::PopAndDestroy(2, buf); // writestream, buf
        
        // Cleanup ClientConversationList
        CleanupStack::PopAndDestroy(ClientConversationList);
        }

    TInt rcevdBufferSize = aMessage.GetDesMaxLength(1);
    TInt reqdBufferSize  = iDes->Size();

    PRINT1 ( _L("Received buffer size = %d"), rcevdBufferSize );
    PRINT1 ( _L("Required buffer size = %d"), reqdBufferSize );

    // If the received buffer size from Client API is less than
    // the required buffer size write the required buffer size
    // and return.
    if (rcevdBufferSize < reqdBufferSize)
    {
        PRINT ( _L("In-adequate buffer received") );
        PRINT ( _L("Packing the required buffer size in response") );

        TPckgC<TInt> bufferSizePackage(reqdBufferSize);
        aMessage.WriteL(1, bufferSizePackage);
        aMessage.Complete(EGetConversationListBufferOverflow);
        iBufferOverflow = ETrue;
    }
    else
    {
        PRINT ( _L("Adequate buffer received") );
        PRINT ( _L("Packing the results in response") )

        aMessage.Write(1, *iDes);
        aMessage.Complete(EGetConversationListOperationComplete);
        iBufferOverflow = EFalse;
        delete iDes;
        iDes = NULL;
        }
    
    PRINT_TIMESTAMP ("End CCsSession::GetConversationListL");
    PRINT ( _L("End CCsSession::GetConversationListL") );
}

// ----------------------------------------------------------------------------
// CCsSession::GetConversationUnreadListL
// the function to handle the request of Listing
// of recent(latest) unread conversation entry and
// list of dispalyname for all stored conversation entry ID
// ----------------------------------------------------------------------------
void CCsSession::GetConversationUnreadListL(const RMessage2& aMessage)
    {
    PRINT ( _L("Enter CCsSession::GetConversationUnreadListL") );
    PRINT_TIMESTAMP ("Enter CCsSession::GetConversationUnreadListL");

    if (iBufferOverflow == EFalse)
        {
        RPointerArray<CCsClientConversation>* ClientConversationList =
                new (ELeave) RPointerArray<CCsClientConversation> ();
        CleanupResetAndDestroyPushL(ClientConversationList);
        // get cache pointer
        CCsConversationCache* cache = iServer->ConversationCacheInterface();

        // Call cache function to get recent conversation entry list
        // with dispaly name for all stored conversation entry ID
        cache->GetConversationUnreadListL(ClientConversationList);

        //write all list data into stream
        // create a new buffer for writing into stream
        CBufFlat* buf = CBufFlat::NewL(KBigBuffer);
        CleanupStack::PushL(buf);

        RBufWriteStream writeStream(*buf);
        writeStream.PushL();

        TInt listCount  = ClientConversationList->Count();

        if (listCount == 0)
            {
            iConversationListChangeObserver = ETrue;
            }

        // write the count first
        writeStream.WriteUint16L(listCount);

        // now go through the list and do externalize
        for (int iloop=0 ; iloop < listCount ; iloop++)
            {
            CCsClientConversation* ClientConversation =
            static_cast<CCsClientConversation*>((*ClientConversationList)[iloop]);
            //write list of ClientConversation
            ClientConversation->ExternalizeL(writeStream);
            }

        // Results are already packed in the stream
        writeStream.CommitL();

        // --------------------------------------------------------------
        // Create a heap descriptor from the buffer
        iDes = HBufC8::NewL(buf->Size());
        TPtr8 ptr(iDes->Des());
        buf->Read(0, ptr, buf->Size());

        // cleanup
        CleanupStack::PopAndDestroy(2, buf); // writestream, buf
        CleanupStack::PopAndDestroy(ClientConversationList);

        }

    TInt rcevdBufferSize = aMessage.GetDesMaxLength(1);
    TInt reqdBufferSize  = iDes->Size();

    PRINT1 ( _L("Received buffer size = %d"), rcevdBufferSize );
    PRINT1 ( _L("Required buffer size = %d"), reqdBufferSize );

    if ( rcevdBufferSize < reqdBufferSize )
        {
        PRINT ( _L("In-adequate buffer received") );
        PRINT ( _L("Packing the required buffer size in response") );

        TPckgC<TInt> overflowPackage(ETrue);
        aMessage.WriteL(0, overflowPackage);
        TPckgC<TInt> bufferSizePackage(reqdBufferSize);
        aMessage.WriteL(1, bufferSizePackage);
        aMessage.Complete(KErrNone);
        iBufferOverflow = ETrue;
        }
    else
        {
        PRINT ( _L("Adequate buffer received") );
        PRINT ( _L("Packing the results in response") )

        TPckgC<TInt> overflowPackage(EFalse);
        aMessage.WriteL(0, overflowPackage);
        aMessage.Write(1, *iDes);
        aMessage.Complete(KErrNone);
        iBufferOverflow = EFalse;
        delete iDes;
        iDes = NULL;
        }

    PRINT_TIMESTAMP ("End CCsSession::GetConversationUnreadListL");
    PRINT ( _L("End CCsSession::GetConversationUnreadListL") );
    }

// ----------------------------------------------------------------------------
// CCsSession::GetConversationsL
// the function to handle the request
// of conversation entry list for one conversation entry ID
// ----------------------------------------------------------------------------
void CCsSession::GetConversationsL(const RMessage2& aMessage)
{
    PRINT ( _L("Enter CCsSession::GetConversationsL") );

    if (iGetConversationBufferOverflow == EFalse)
    {
        // Read Contact from the message
        HBufC8* buffer = HBufC8::NewLC(KBigBuffer);

        TPtr8 bufferPtr(buffer->Des());
        aMessage.ReadL(0, bufferPtr);

        // Stream over the buffer
        RDesReadStream stream(bufferPtr);
        stream.PushL();
        
        //Read known index and page size
        TInt knownIndex = stream.ReadInt32L();
        TInt pageSize  = stream.ReadInt32L();
        
        // get cache pointer
        CCsConversationCache* cache = iServer->ConversationCacheInterface();

        // read the Client Conversation consist of Entry Id
        CCsClientConversation* ClientConversation =
                CCsClientConversation::NewL();
        CleanupStack::PushL(ClientConversation);
        ClientConversation->InternalizeL(stream);
        CleanupStack::Pop(ClientConversation);
        CleanupStack::PopAndDestroy(2, buffer);//stream, buffer

        CleanupStack::PushL(ClientConversation);
        RPointerArray<CCsConversationEntry>* conversationEntryList=
                new (ELeave) RPointerArray<CCsConversationEntry>(10);
        CleanupResetAndDestroyPushL(conversationEntryList);

        // get conversation entry  list for given ClientConversation 
        TInt totalCount(0);
        cache->GetConversationsL (ClientConversation,
                conversationEntryList,
                knownIndex,
                pageSize,
                totalCount);

        // create a new buffer for writing into stream
        // write all list data into stream
        CBufFlat* buf = CBufFlat::NewL(KBigBuffer);
        CleanupStack::PushL(buf);

        RBufWriteStream writeStream(*buf);
        writeStream.PushL();

        TInt ItemCount = conversationEntryList->Count();
        //write  recent conversation entry list
        writeStream.WriteInt32L(ItemCount);
        
        //Write total count in the stream to update UI
        writeStream.WriteInt32L(totalCount);
        
        // Write the conversation entry
        for (TInt iloop = 0; iloop < ItemCount; iloop++)
        {
            CCsConversationEntry
                    * entry =
                            static_cast<CCsConversationEntry*> ( (*conversationEntryList)[iloop]);
            entry->ExternalizeL(writeStream);
        }

        // Results are already packed in the stream
        writeStream.CommitL();

        // --------------------------------------------------------------
        // Create a heap descriptor from the buffer
        iDes = HBufC8::NewL(buf->Size());
        TPtr8 ptr(iDes->Des());
        buf->Read(0, ptr, buf->Size());

        CleanupStack::PopAndDestroy(2, buf); // writestream, buf
     
        // Cleanup
        CleanupStack::PopAndDestroy(conversationEntryList);
        CleanupStack::PopAndDestroy(ClientConversation);
    }

    TInt rcevdBufferSize = aMessage.GetDesMaxLength(1);
    TInt reqdBufferSize  = iDes->Size();

    PRINT1 ( _L("Received buffer size = %d"), rcevdBufferSize );
    PRINT1 ( _L("Required buffer size = %d"), reqdBufferSize );

    // If the received buffer size from Client API is less than
    // the required buffer size write the required buffer size
    if (rcevdBufferSize < reqdBufferSize)
    {
        PRINT ( _L("In-adequate buffer received") );
        PRINT ( _L("Packing the required buffer size in response") );

        TPckgC<TInt> bufferSizePackage(reqdBufferSize);
        aMessage.WriteL(1, bufferSizePackage);
        aMessage.Complete(EGetConversationBufferOverflow);
        iGetConversationBufferOverflow = ETrue;
    }
    else
    {
        PRINT ( _L("Adequate buffer received") );
        PRINT ( _L("Packing the results in response") )

        aMessage.Write(1, *iDes);
        aMessage.Complete(EGetConversationOperationComplete);
        iGetConversationBufferOverflow = EFalse;
        delete iDes;
        iDes = NULL;
        }

    PRINT ( _L("End CCsSession::GetConversationsL") );
}

// ----------------------------------------------------------------------------
// CCsSession::ShutdownServerL
// Stops the scheduler
// ----------------------------------------------------------------------------
void CCsSession::ShutdownServerL(const RMessage2& aMessage)
{
    aMessage.Complete(KErrNone);
    CActiveScheduler::Stop();

    PRINT ( _L("CCsSession::ShutdownServerL - Server ShutDown") );
}

// ----------------------------------------------------------------------------
// CCsSession::HandleNewConversationListEventL
// Notify client about new conversation event
// ----------------------------------------------------------------------------
void CCsSession::HandleNewConversationListEventL(
                                                 CCsClientConversation* aClientConversation)
{
    PRINT ( _L("Enter CCsSession::HandleNewConversationListEventL") );

    if (!iConversationListChangeObserver)
        return;

    if (! (iNotifyHandling))
    {
        //append in notify list
        CCsConversationEvent* conversationEvent = CCsConversationEvent::NewL();
        CleanupStack::PushL(conversationEvent);
        conversationEvent->SetClientConversationL(*aClientConversation);
        conversationEvent->SetEvent(KConversationListEventNew);
        iEventList->AppendL(conversationEvent);
        CleanupStack::Pop(conversationEvent);
    }
    else
    {
        // create a new buffer for writing into stream
        CBufFlat* buf = CBufFlat::NewL(KBigBuffer);
        CleanupStack::PushL(buf);

        RBufWriteStream writeStream(*buf);
        writeStream.PushL();

        //externalize ClientConversation
        aClientConversation->ExternalizeL(writeStream);

        // Results are already packed in the stream
        writeStream.CommitL();

        // --------------------------------------------------------------
        // Create a heap descriptor from the buffer
        HBufC8* notifyDes = HBufC8::NewLC(buf->Size());
        TPtr8 ptr(notifyDes->Des());
        buf->Read(0, ptr, buf->Size());

        iAsyncReqRMessage.Write(1, *notifyDes);
        iAsyncReqRMessage.Complete(EAddConversationListEvent);
        CleanupStack::PopAndDestroy(3, buf); // notifyDes, writestream, buf 
        iNotifyHandling = EFalse;
    }

    PRINT ( _L("End CCsSession::HandleNewConversationListEventL") );
}

// ----------------------------------------------------------------------------
// CCsSession::HandleDeleteConversationListEventL
// Notify client about delete conversation event
// ----------------------------------------------------------------------------
void CCsSession::HandleDeleteConversationListEventL(
                                                    CCsClientConversation* aClientConversation)
{
    PRINT ( _L("Enter CCsSession::HandleDeleteConversationListEventL") );

    if (!iConversationListChangeObserver)
        return;

    if (! (iNotifyHandling))
    {
        //append in notify list
        CCsConversationEvent* conversationEvent = CCsConversationEvent::NewL();
        conversationEvent->SetClientConversationL(*aClientConversation);
        CleanupStack::PushL(conversationEvent);
        conversationEvent->SetEvent(KConversationListEventDelete);
        iEventList->AppendL(conversationEvent);
        CleanupStack::Pop(conversationEvent);
    }
    else
    {
        // create a new buffer for writing into stream
        CBufFlat* buf = CBufFlat::NewL(KBigBuffer);
        CleanupStack::PushL(buf);

        RBufWriteStream writeStream(*buf);
        writeStream.PushL();

        //externalize ClientConversation
        aClientConversation->ExternalizeL(writeStream);

        // Results are already packed in the stream
        writeStream.CommitL();

        // --------------------------------------------------------------
        // Create a heap descriptor from the buffer
        HBufC8* notifyDes = HBufC8::NewLC(buf->Size());
        TPtr8 ptr(notifyDes->Des());
        buf->Read(0, ptr, buf->Size());

        iAsyncReqRMessage.Write(1, *notifyDes);
        iAsyncReqRMessage.Complete(EDeleteConversationListEvent);
        CleanupStack::PopAndDestroy(3, buf); // notifyDes, writestream, buf
        iNotifyHandling = EFalse;
    }

    PRINT ( _L("End CCsSession::HandleDeleteConversationListEventL") );
}

// ----------------------------------------------------------------------------
// CCsSession::HandlePartialDeleteConversationListEventL
// Notify client about partial delete conversation event
// ----------------------------------------------------------------------------

void CCsSession::HandlePartialDeleteConversationListEvent(
                                                    CCsClientConversation* aClientConversation)
{
    PRINT ( _L("Enter CCsSession::HandlePartialDeleteConversationListEvent") );

    if (!iConversationListChangeObserver)
        return;
    
    if (! (iNotifyHandling))
    {
        //append in notify list
        CCsConversationEvent* conversationEvent = CCsConversationEvent::NewL();
        conversationEvent->SetClientConversationL(*aClientConversation);
        CleanupStack::PushL(conversationEvent);
        conversationEvent->SetEvent(KConversationListEventPartialDelete);
        iEventList->AppendL(conversationEvent);
        CleanupStack::Pop(conversationEvent);
    }
    else
    {
        // create a new buffer for writing into stream
        CBufFlat* buf = CBufFlat::NewL(KBigBuffer);
        CleanupStack::PushL(buf);

        RBufWriteStream writeStream(*buf);
        writeStream.PushL();

        //externalize ClientConversation
        aClientConversation->ExternalizeL(writeStream);

        // Results are already packed in the stream
        writeStream.CommitL();

        // --------------------------------------------------------------
        // Create a heap descriptor from the buffer
        HBufC8* notifyDes = HBufC8::NewLC(buf->Size());
        TPtr8 ptr(notifyDes->Des());
        buf->Read(0, ptr, buf->Size());

        iAsyncReqRMessage.Write(1, *notifyDes);
        iAsyncReqRMessage.Complete(EPartialDeleteConversationListEvent);
        CleanupStack::PopAndDestroy(3, buf); // notifyDes, writestream, buf
        iNotifyHandling = EFalse;
    }
}

// ----------------------------------------------------------------------------
// CCsSession::HandleModifyConversationListEventL
// Notify client about update conversation event
// ----------------------------------------------------------------------------
void CCsSession::HandleModifyConversationListEventL(
                                                    CCsClientConversation* aClientConversation)
{
    PRINT ( _L("Enter CCsSession::HandleModifyConversationListEventL") );

    if (!iConversationListChangeObserver)
        return;

    if (! (iNotifyHandling))
    {
        //append in notify list
        CCsConversationEvent* conversationEvent = CCsConversationEvent::NewL();
        conversationEvent->SetClientConversationL(*aClientConversation);
        CleanupStack::PushL(conversationEvent);
        conversationEvent->SetEvent(KConversationListEventUpdate);
        iEventList->AppendL(conversationEvent);
        CleanupStack::Pop(conversationEvent);
    }
    else
    {
        // create a new buffer for writing into stream
        CBufFlat* buf = CBufFlat::NewL(KBigBuffer);
        CleanupStack::PushL(buf);

        RBufWriteStream writeStream(*buf);
        writeStream.PushL();

        //externalize ClientConversation
        aClientConversation->ExternalizeL(writeStream);

        // Results are already packed in the stream
        writeStream.CommitL();
        // --------------------------------------------------------------

        // Create a heap descriptor from the buffer
        HBufC8* notifyDes = HBufC8::NewLC(buf->Size());
        TPtr8 ptr(notifyDes->Des());
        buf->Read(0, ptr, buf->Size());

        iAsyncReqRMessage.Write(1, *notifyDes);
        iAsyncReqRMessage.Complete(EModifyConversationListEvent);
        CleanupStack::PopAndDestroy(3, buf); // notifyDes, writestream, buf
        iNotifyHandling = EFalse;
    }

    PRINT ( _L("End CCsSession::HandleModifyConversationListEventL") );
}

// ----------------------------------------------------------------------------
// CCsSession::RequestChangeEventL
// the function to register for cache change event notification
// ----------------------------------------------------------------------------
void CCsSession::RequestChangeEventL(const RMessage2& aMessage)
{
    iAsyncReqRMessage = aMessage;
    iNotifyHandling = ETrue;

    if (iEventList->Count() > 0)
    {
        CCsConversationEvent* conversationEvent = (*iEventList)[0];

        // check if the reqCnt matches with the latest arrived count
        // then its not a duplicate, delete the top event
        if (iReqCnt == aMessage.Int2())
        {
            iEventList->Remove(0);
            delete conversationEvent;

            if (iEventList->Count() > 0)
            {
                // increment the count
                iReqCnt++;
                conversationEvent = (*iEventList)[0];
            }
            else
            {
                // no more pending events, simply return
                return;
            }
        }

		// coverity[size_error][buffer_alloc]
        HBufC8* buffer = HBufC8::NewLC(4);
        TPtr8 cntPtr(buffer->Des());
        _LIT8(KFormat,"%d");
        cntPtr.Format(KFormat, iReqCnt);
        iAsyncReqRMessage.Write(0, *buffer);

        // create a new buffer for writing into stream
        CBufFlat* buf = CBufFlat::NewL(KBigBuffer);
        CleanupStack::PushL(buf);

        RBufWriteStream writeStream(*buf);
        writeStream.PushL();

        // Externalize ClientConversation
        if (conversationEvent->ClientConversation())
            conversationEvent->ClientConversation()->ExternalizeL(writeStream);

        // Results are already packed in the stream
        writeStream.CommitL();

        // --------------------------------------------------------------
        // Create a heap descriptor from the buffer
        HBufC8* notifyDes = HBufC8::NewLC(buf->Size());
        TPtr8 ptr(notifyDes->Des());
        buf->Read(0, ptr, buf->Size());

        iAsyncReqRMessage.Write(1, *notifyDes);

        NotifyClient(conversationEvent);

        CleanupStack::PopAndDestroy(4, buffer);
        // notifyDes, writestream, buf, buffer
        iNotifyHandling = EFalse;
    }
}

// ----------------------------------------------------------------------------
// CCsSession::RemoveChangeEventL
// Deregister the cache change event notification
// ----------------------------------------------------------------------------
void CCsSession::RemoveChangeEventL(const RMessage2& aMessage)
{
    if (! (iNotifyHandling))
    {
        // complete message with aMessage
        aMessage.Complete(KErrNone);
    }
    else
    {
        iAsyncReqRMessage.Complete(KErrCancel);
        iNotifyHandling = EFalse;
        // complete message with aMessage
        aMessage.Complete(KErrNone);
    }
}

// ----------------------------------------------------------------------------
// CCsSession::GetCachingStatusL
// the function to request conversation server
// to get caching status.
// ----------------------------------------------------------------------------
void CCsSession::GetCachingStatusL(const RMessage2& aMessage)
{
    PRINT ( _L("Enter CCsSession::GetCachingStatusL") );

    // create a new buffer for writing into stream
    CBufFlat* buf = CBufFlat::NewL(KTinyBuffer);

    CleanupStack::PushL(buf);
    RBufWriteStream writeStream(*buf);
    writeStream.PushL();

    // Externalize caching status
    writeStream.WriteUint8L(iServer->GetCachingStatus());

    // Results are already packed in the stream
    writeStream.CommitL();
    // --------------------------------------------------------------

    // Create a heap descriptor from the buffer
    HBufC8* des = HBufC8::NewL(buf->Size());
    TPtr8 ptr(des->Des());
    buf->Read(0, ptr, buf->Size());

    CleanupStack::PopAndDestroy(2, buf); // writestream, buf

    aMessage.Write(1, *des);
    aMessage.Complete(KErrNone);
    delete des;

    PRINT ( _L("End CCsSession::GetCachingStatusL") );
}

// ----------------------------------------------------------------------------
// CCsSession::GetTotalUnreadCountL
// Gets total unread conversation entries.
// ----------------------------------------------------------------------------
void CCsSession::GetTotalUnreadCountL(const RMessage2& aMessage)
    {
    PRINT ( _L("Enter CCsSession::GetTotalUnreadCountL") );

    // create a new buffer for writing into stream
    CBufFlat* buf = CBufFlat::NewL(KTinyBuffer);

    CleanupStack::PushL(buf);
    RBufWriteStream writeStream(*buf);
    writeStream.PushL();

    CCsConversationCache* cache = iServer->ConversationCacheInterface();

    // Externalize caching status
    writeStream.WriteUint32L(cache->GetTotalUnreadCount());

    // Results are already packed in the stream
    writeStream.CommitL();
    // --------------------------------------------------------------

    // Create a heap descriptor from the buffer
    HBufC8* des = HBufC8::NewL(buf->Size());
    TPtr8 ptr(des->Des());
    buf->Read(0, ptr, buf->Size());

    CleanupStack::PopAndDestroy(2, buf); // writestream, buf

    aMessage.Write(1, *des);
    aMessage.Complete(KErrNone);
    delete des;

    PRINT ( _L("End CCsSession::GetTotalUnreadCountL") );
    }

// ----------------------------------------------------------------------------
// CCsSession::SetConversationListChangeObserverL
// the function to request conversation server
// to set ConversationListChangeObserver.
// ----------------------------------------------------------------------------
void CCsSession::SetConversationListChangeObserverL(const RMessage2& aMessage)
{
    iConversationListChangeObserver = ETrue;
    aMessage.Complete(KErrNone);
}

// ----------------------------------------------------------------------------
// CCsSession::ResetConversationListChangeObserverL
// the function to request conversation server
// to reset ConversationListChangeObserver.
// ----------------------------------------------------------------------------
void CCsSession::ResetConversationListChangeObserverL(const RMessage2& aMessage)
{
    iConversationListChangeObserver = EFalse;
    aMessage.Complete(KErrNone);
}

// ----------------------------------------------------------------------------
// CCsSession::SetConversationChangeObserverL
// the function to request conversation server
// to set ConversationChangeObserver.
// ----------------------------------------------------------------------------
void CCsSession::SetConversationChangeObserverL(const RMessage2& aMessage)
{
    // Read Contact from the message
    HBufC8* buffer = HBufC8::NewLC(KBigBuffer);

    TPtr8 bufferPtr(buffer->Des());
    aMessage.ReadL(0, bufferPtr);

    // Stream over the buffer
    RDesReadStream stream(bufferPtr);
    stream.PushL();

    // Get the client conversation
    CCsClientConversation* clientConversation = CCsClientConversation::NewL();
    CleanupStack::PushL(clientConversation);
    clientConversation->InternalizeL(stream);
    CleanupStack::Pop(clientConversation);

    CleanupStack::PopAndDestroy(2, buffer);//stream, buffer

    iMonitoredConversation = clientConversation;
    iConversationChangeObserver = ETrue;

    aMessage.Complete(KErrNone);
}

// ----------------------------------------------------------------------------
// CCsSession::ResetConversationChangeObserverL
// the function to request conversation server
// to reset ConversationChangeObserver.
// ----------------------------------------------------------------------------
void CCsSession::ResetConversationChangeObserverL(const RMessage2& aMessage)
{
    // Read Contact from the message
    HBufC8* buffer = HBufC8::NewLC(KBigBuffer);

    TPtr8 bufferPtr(buffer->Des());
    aMessage.ReadL(0, bufferPtr);

    // Stream over the buffer
    RDesReadStream stream(bufferPtr);
    stream.PushL();

    // Read the client conversation
    CCsClientConversation* clientConversation = CCsClientConversation::NewL();
    CleanupStack::PushL(clientConversation);
    clientConversation->InternalizeL(stream);
    CleanupStack::Pop(clientConversation);

    CleanupStack::PopAndDestroy(2, buffer); //stream, buffer

    if (iMonitoredConversation)
    {
        delete iMonitoredConversation;
        iMonitoredConversation = NULL;
        iConversationChangeObserver = EFalse;
    }

    delete clientConversation;

    aMessage.Complete(KErrNone);
}

// ----------------------------------------------------------------------------
// CCsSession::SetCachingStatusObserverL
// the function to request conversation server
// to set caching status observer flag.
// ----------------------------------------------------------------------------
void CCsSession::SetCachingStatusObserverL(const RMessage2& aMessage)
{
    iCachingChangeObserver = ETrue;
    aMessage.Complete(KErrNone);
}

// ----------------------------------------------------------------------------
// CCsSession::ResetCachingStatusObserverL
// the function to request conversation server
// to reset caching status observer flag.
// ----------------------------------------------------------------------------
void CCsSession::ResetCachingStatusObserverL(const RMessage2& aMessage)
{
    iCachingChangeObserver = EFalse;
    aMessage.Complete(KErrNone);
}

// ----------------------------------------------------------------------------
// CCsSession::HandleNewConversationEventL
// the function to handles the new conversation event received from cache
// asynchronously
// ----------------------------------------------------------------------------
void CCsSession::HandleNewConversationEventL(
                                             CCsClientConversation* aClientConversation)
{
    PRINT ( _L("Enter CCsSession::HandleNewConversationEventL") );

    if (!iConversationChangeObserver)
        return;

    //this is check to send notif to clients for a new message
    //1. if the client is subscribed with contact id ==> then send
    //2. if the client is subscribed with conv id ---> then send
    // else dont send
    
    if ((aClientConversation->GetContactId() == 
            iMonitoredConversation->GetContactId() && 
            aClientConversation->GetContactId() != -1)
            ||(aClientConversation->GetConversationEntryId() == 
                    iMonitoredConversation->GetConversationEntryId()))
    {

        if (! (iNotifyHandling))
        {
            //append in notify list
            CCsConversationEvent* conversationEvent = CCsConversationEvent::NewL();
            CleanupStack::PushL(conversationEvent);
            conversationEvent->SetClientConversationL(*aClientConversation);
            conversationEvent->SetEvent(KConversationEventNew);
            iEventList->AppendL(conversationEvent);
            CleanupStack::Pop(conversationEvent);
        }
        else
        {
            // create a new buffer for writing into stream
            CBufFlat* buf = CBufFlat::NewL(KBigBuffer);
            CleanupStack::PushL(buf);
    
            RBufWriteStream writeStream(*buf);
            writeStream.PushL();
    
            //externalize ClientConversation
            aClientConversation->ExternalizeL(writeStream);
    
            // Results are already packed in the stream
            writeStream.CommitL();
            // --------------------------------------------------------------
    
            // Create a heap descriptor from the buffer
            HBufC8* des = HBufC8::NewLC(buf->Size());
            CleanupStack::Pop(des);
            TPtr8 ptr(des->Des());
            buf->Read(0, ptr, buf->Size());
    
            CleanupStack::PopAndDestroy(2, buf); // writestream, buf
    
            iAsyncReqRMessage.Write(1, *des);
            iAsyncReqRMessage.Complete(EAddConversationEvent);
            delete des;
            iNotifyHandling = EFalse;
        }
    }
    PRINT ( _L("End CCsSession::HandleNewConversationEventL") );
}

// ----------------------------------------------------------------------------
// CCsSession::HandleDeleteConversationEventL
// the function to handles the delete conversation event received from cache
// asynchronously
// ----------------------------------------------------------------------------
void CCsSession::HandleDeleteConversationEventL(
                                                CCsClientConversation* aClientConversation)
{
    PRINT ( _L("Enter CCsSession::HandleDeleteConversationEventL") );

    if (!iConversationChangeObserver)
        return;

    if ((aClientConversation->GetContactId()
               != iMonitoredConversation->GetContactId()) &&
               (aClientConversation->GetConversationEntryId()
               != iMonitoredConversation->GetConversationEntryId())
                 )
        return;

    if (! (iNotifyHandling))
    {
        //append in notify list
        CCsConversationEvent* conversationEvent = CCsConversationEvent::NewL();
        CleanupStack::PushL(conversationEvent);
        conversationEvent->SetClientConversationL(*aClientConversation);
        conversationEvent->SetEvent(KConversationEventDelete);
        iEventList->AppendL(conversationEvent);
        CleanupStack::Pop(conversationEvent);
    }
    else
    {
        // create a new buffer for writing into stream
        CBufFlat* buf = CBufFlat::NewL(KBigBuffer);
        CleanupStack::PushL(buf);

        RBufWriteStream writeStream(*buf);
        writeStream.PushL();

        //externalize ClientConversation
        aClientConversation->ExternalizeL(writeStream);

        // Results are already packed in the stream
        writeStream.CommitL();
        // --------------------------------------------------------------

        // Create a heap descriptor from the buffer
        HBufC8* des = HBufC8::NewL(buf->Size());

        TPtr8 ptr(des->Des());
        buf->Read(0, ptr, buf->Size());

        CleanupStack::PopAndDestroy(2, buf); // writestream, buf

        iAsyncReqRMessage.Write(1, *des);
        iAsyncReqRMessage.Complete(EDeleteConversationEvent);
        delete des;
        iNotifyHandling = EFalse;
    }

    PRINT ( _L("End CCsSession::HandleDeleteConversationEventL") );
}

// ----------------------------------------------------------------------------
// CCsSession::HandleModifyConversationEventL
// the function to handles the modify conversation event received from cache
// asynchronously
// ----------------------------------------------------------------------------
void CCsSession::HandleModifyConversationEventL(
                                                CCsClientConversation* aClientConversation)
{
    PRINT ( _L("Enter CCsSession::HandleModifyConversationEventL") );

    if (!iConversationChangeObserver)
        return;

    //this is check to send notif to clients for a new message
    //1. if the client is subscribed with contact id ==> then send
    //2. if the client is subscribed with conv id ---> then send
    // else dont send
    
    if ((aClientConversation->GetContactId() == 
                iMonitoredConversation->GetContactId() && 
                aClientConversation->GetContactId() != -1)
                ||(aClientConversation->GetConversationEntryId() == 
                        iMonitoredConversation->GetConversationEntryId()))
    {
    
        if (! (iNotifyHandling))
        {
            //append in notify list
            CCsConversationEvent* conversationEvent = CCsConversationEvent::NewL();
            CleanupStack::PushL(conversationEvent);
            conversationEvent->SetClientConversationL(*aClientConversation);
            conversationEvent->SetEvent(KConversationEventUpdate);
            iEventList->AppendL(conversationEvent);
            CleanupStack::Pop(conversationEvent);
        }
        else
        {
            // create a new buffer for writing into stream
            CBufFlat* buf = CBufFlat::NewL(KBigBuffer);
            CleanupStack::PushL(buf);
    
            RBufWriteStream writeStream(*buf);
            writeStream.PushL();
    
            //externalize ClientConversation
            aClientConversation->ExternalizeL(writeStream);
    
            // Results are already packed in the stream
            writeStream.CommitL();
            // --------------------------------------------------------------
    
            // Create a heap descriptor from the buffer
            HBufC8* des = HBufC8::NewL(buf->Size());
            TPtr8 ptr(des->Des());
            buf->Read(0, ptr, buf->Size());
    
            CleanupStack::PopAndDestroy(2, buf); // writestream, buf
    
            iAsyncReqRMessage.Write(1, *des);
            iAsyncReqRMessage.Complete(EModifyConversationEvent);
            delete des;
            iNotifyHandling = EFalse;
        }
    }
    PRINT ( _L("End CCsSession::HandleModifyConversationEventL") );
}

// ----------------------------------------------------------------------------
// CCsSession::NotifyClient
// The function to notify client for cache change event
// ----------------------------------------------------------------------------
void CCsSession::NotifyClient(CCsConversationEvent* aConversationEvent)
{
    if (aConversationEvent->IsNewConversationListEventSet())
    {
        iAsyncReqRMessage.Complete(EAddConversationListEvent);
    }
    else if (aConversationEvent->IsDeleteConversationListEventSet())
    {
        iAsyncReqRMessage.Complete(EDeleteConversationListEvent);
    }
    else if (aConversationEvent->IsUpdateConversationListEventSet())
    {
        iAsyncReqRMessage.Complete(EModifyConversationListEvent);
    }
    else if(aConversationEvent->IsPartialDeleteConversationListEventSet())
    {
        iAsyncReqRMessage.Complete(EPartialDeleteConversationListEvent);
    }
    else if (aConversationEvent->IsNewConversationEventSet())
    {
        iAsyncReqRMessage.Complete(EAddConversationEvent);
    }
    else if (aConversationEvent->IsDeleteConversationEventSet())
    {
        iAsyncReqRMessage.Complete(EDeleteConversationEvent);
    }
    else if (aConversationEvent->IsUpdateConversationEventSet())
    {
        iAsyncReqRMessage.Complete(EModifyConversationEvent);
    }
    else if (aConversationEvent->IsRefreshConversationListEventSet())
    {
        iAsyncReqRMessage.Complete(KConversationEventListRefresh);
    }
    else if (aConversationEvent->IsRefreshConversationEventSet())
    {
        iAsyncReqRMessage.Complete(KConversationEventRefresh);
    }
}

// ----------------------------------------------------------------------------
// CCsSession::HandleChangeEventL
// The function handles cache change events
// ----------------------------------------------------------------------------
void CCsSession::HandleChangeEventL(CCsClientConversation* aConversation,
                                    TUint32 aEvent)
{
    if (aEvent & KConversationListEventNew)
    {
        HandleNewConversationListEventL(aConversation);
    }
    else if (aEvent & KConversationListEventUpdate)
    {
        HandleModifyConversationListEventL(aConversation);
    }
    else if (aEvent & KConversationListEventDelete)
    {
        HandleDeleteConversationListEventL(aConversation);
    }
    else if(aEvent & KConversationListEventPartialDelete)
    {
        HandlePartialDeleteConversationListEvent(aConversation);
    }
    else if (aEvent & KConversationEventNew)
    {
        HandleNewConversationEventL(aConversation);
    }
    else if (aEvent & KConversationEventUpdate)
    {
        HandleModifyConversationEventL(aConversation);
    }
    else if (aEvent & KConversationEventDelete)
    {
        HandleDeleteConversationEventL(aConversation);
    }
    else if (aEvent & KConversationEventListRefresh)
    {
        HandleRefreshConversationListL();
    }
    else if (aEvent & KConversationEventRefresh)
    {
        HandleRefreshConversationL();
    }
}

// ----------------------------------------------------------------------------
// CCsSession::DeleteConversationL
// ----------------------------------------------------------------------------
void CCsSession::DeleteConversationL(const RMessage2& aMessage)
{
    PRINT ( _L("Enter CCsSession::DeleteConversationL") );

    TInt conversationId = aMessage.Int0();

    // Delete handler
    CCsConversationCache* cache = iServer->ConversationCacheInterface();
    CCsConversationDeleteHandler* deleteHandler =
            CCsConversationDeleteHandler::NewL(cache);

    deleteHandler->DeleteL(conversationId);
    aMessage.Complete(EUserDeleteConversationComplete);

    PRINT ( _L("End CCsSession::DeleteConversationL") );
}

// ----------------------------------------------------------------------------
// CCsSession::HandleRefreshConversationListL
// ----------------------------------------------------------------------------
void CCsSession::HandleRefreshConversationListL()
{
    if (!iConversationListChangeObserver)
        return;

    if (! (iNotifyHandling))
    {
        // Append in notify list
        CCsConversationEvent* conversationEvent = CCsConversationEvent::NewL();
        CleanupStack::PushL(conversationEvent);
        conversationEvent->SetEvent(KConversationEventListRefresh);
        iEventList->AppendL(conversationEvent);
        CleanupStack::Pop(conversationEvent);
    }
    else
    {
        iAsyncReqRMessage.Complete(ERefreshConversationListEvent);
        iNotifyHandling = EFalse;
    }
}

// ----------------------------------------------------------------------------
// CCsSession::HandleRefreshConversationL
// ----------------------------------------------------------------------------
void CCsSession::HandleRefreshConversationL()
{
    if (!iConversationChangeObserver)
        return;

    if (! (iNotifyHandling))
    {
        // Append in notify list
        CCsConversationEvent* conversationEvent = CCsConversationEvent::NewL();
        CleanupStack::PushL(conversationEvent);
        conversationEvent->SetEvent(KConversationEventRefresh);
        iEventList->AppendL(conversationEvent);
        CleanupStack::Pop(conversationEvent);
    }
    else
    {
        iAsyncReqRMessage.Complete(ERefreshConversationEvent);
        iNotifyHandling = EFalse;
    }
}

// ----------------------------------------------------------------------------
// Get conversation id
// ----------------------------------------------------------------------------
void CCsSession::GetConversationIdL(const RMessage2& aMessage)
{
    // create a new buffer for writing into stream
    CBufFlat* buf = CBufFlat::NewL(KTinyBuffer);

    CleanupStack::PushL(buf);
    RBufWriteStream writeStream(*buf);
    writeStream.PushL();

    // Get the contact id
    TInt contactId = aMessage.Int0();
    CCsConversationCache* cache = iServer->ConversationCacheInterface();
    TInt conversationId = cache->GetConversationIdL(contactId);

    // Externalize link
    writeStream.WriteInt32L(conversationId);

    // Results are already packed in the stream
    writeStream.CommitL();

    // Create a heap descriptor from the buffer
    HBufC8* des = HBufC8::NewL(buf->Size());
    TPtr8 ptr(des->Des());
    buf->Read(0, ptr, buf->Size());

    CleanupStack::PopAndDestroy(2, buf); // writestream, buf

    aMessage.Write(1, *des);
    aMessage.Complete(EGetConversationIdComplete);
    delete des;
}

void CCsSession::GetConversationFromConversationIdL(const RMessage2& aMessage)
{
    // create a new buffer for writing into stream
    CBufFlat* buf = CBufFlat::NewL(KBigBuffer);
    CleanupStack::PushL(buf);

    RBufWriteStream writeStream(*buf);
    writeStream.PushL();

    // Get the conversation id
    TInt conversationId = aMessage.Int0();
    CCsConversationCache* cache = iServer->ConversationCacheInterface();
    CCsClientConversation* clientConv = cache->GetConversationFromConversationIdL(conversationId);

    // if no conversation exists for given message-id,
    // create a dummy conversation and complete response
    if(clientConv == NULL)
    {
        //create dummy conversation
        clientConv = CCsClientConversation::NewL();
        CleanupStack::PushL(clientConv);
        clientConv->SetConversationEntryId(-1);
        CCsConversationEntry* entry = CCsConversationEntry::NewL();
        CleanupStack::PushL(entry);
        entry->SetEntryId(-1);
        clientConv->SetConversationEntryL(entry); // clone
        CleanupStack::PopAndDestroy(entry);
    }
    else
    {
        CleanupStack::PushL(clientConv);
    }

    // Externalize
    clientConv->ExternalizeL(writeStream);

    // Results are already packed in the stream
    writeStream.CommitL();

    // Create a heap descriptor from the buffer
    HBufC8* des = HBufC8::NewL(buf->Size());
    TPtr8 ptr(des->Des());
    buf->Read(0, ptr, buf->Size());

    CleanupStack::PopAndDestroy(3, buf); // clientConv, writestream, buf

    aMessage.Write(1, *des);
    aMessage.Complete(EGetConversationFromConversationIdComplete);
    delete des;
}

// ----------------------------------------------------------------------------
// GetConversationFromMessageIdL
// ----------------------------------------------------------------------------
void CCsSession::GetConversationFromMessageIdL(const RMessage2& aMessage)
    {
    // create a new buffer for writing into stream
    CBufFlat* buf = CBufFlat::NewL(KBigBuffer);
    CleanupStack::PushL(buf);

    RBufWriteStream writeStream(*buf);
    writeStream.PushL();

    // Get the message id
    TInt messageId = aMessage.Int0();
    CCsConversationCache* cache = iServer->ConversationCacheInterface();
    CCsClientConversation* conversation = cache->GetConversationFromMessageIdL(messageId);
    
    // if no conversation exists for given message-id, 
    // create a dummy conversation and complete response
    if(conversation == NULL)
    {
        //create dummy conversation
        conversation = CCsClientConversation::NewL();
        CleanupStack::PushL(conversation);
        conversation->SetConversationEntryId(-1);
        CCsConversationEntry* entry = CCsConversationEntry::NewL();
		CleanupStack::PushL(entry);
        entry->SetEntryId(-1);
        conversation->SetConversationEntryL(entry); // clone
		CleanupStack::PopAndDestroy(entry);
    }
    else
    {
        CleanupStack::PushL(conversation);
    }

    // Externalize 
    conversation->ExternalizeL(writeStream);
    
    // Results are already packed in the stream
    writeStream.CommitL();
    
    // Create a heap descriptor from the buffer
    HBufC8* des = HBufC8::NewL(buf->Size());
    TPtr8 ptr(des->Des());
    buf->Read(0, ptr, buf->Size());
    
    CleanupStack::PopAndDestroy(3, buf); // conversation, writestream, buf
    
    aMessage.Write(1, *des);
    aMessage.Complete(EGetConversationFromMessageIdComplete);
    delete des;
    }

// ----------------------------------------------------------------------------
// CCsSession::GetConversationIdfromAddressL
// ----------------------------------------------------------------------------
void CCsSession::GetConversationIdfromAddressL(const RMessage2& aMessage)
{
    TInt deslen = aMessage.GetDesLength(0);
    
    // Copy the data into a buffer
    RBuf buffer;
    buffer.CreateL(deslen);
    buffer.CleanupClosePushL();
    aMessage.ReadL(0,buffer,0);


    // Get the contact id
    CCsConversationCache* cache = iServer->ConversationCacheInterface();
    TInt conversationId = cache->GetConversationIdFromAddressL(buffer);

    // create a new buffer for writing into stream
    CBufFlat* buf = CBufFlat::NewL(KTinyBuffer);
    CleanupStack::PushL(buf);

    RBufWriteStream writeStream(*buf);
    writeStream.PushL();
    // Externalize link
    writeStream.WriteInt32L(conversationId);

    // Results are already packed in the stream
    writeStream.CommitL();

    // Create a heap descriptor from the buffer
    HBufC8* des = HBufC8::NewL(buf->Size());
    TPtr8 ptr(des->Des());
    buf->Read(0, ptr, buf->Size());
 
    CleanupStack::PopAndDestroy(2, buf); // writestream, buf
    CleanupStack::PopAndDestroy(&buffer);

    aMessage.Write(1, *des);
    aMessage.Complete(EGetConversationIdFromAddressComplete);
    delete des;
}

// ----------------------------------------------------------------------------
// CCsSession::MarkConversationReadL
// ----------------------------------------------------------------------------
void CCsSession::MarkConversationReadL(const RMessage2& aMessage)
{
    PRINT ( _L("Enter CCsSession::MarkConversationReadL") );

    TInt conversationId = aMessage.Int0();

    // Mark read handler
    CCsConversationCache* cache = iServer->ConversationCacheInterface();
    CCsConversationMarkReadHandler* markHandler = CCsConversationMarkReadHandler::NewL(cache);

    markHandler->MarkReadL(conversationId);
    
    aMessage.Complete(EUserMarkReadConversationComplete);

    PRINT ( _L("End CCsSession::MarkConversationReadL") );
}
//EOF
