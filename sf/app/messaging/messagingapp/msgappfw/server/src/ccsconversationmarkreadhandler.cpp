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
* Description:  CS Server Mark read Handler
*
*/
// INCLUDES
#include <ccsclientconversation.h>
#include <ccsconversationentry.h>
#include "ccsconversationcache.h"
#include "ccsconversationmarkreadhandler.h"
#include <mmsconst.h>

// ----------------------------------------------------------------------------
// CCsConversationMarkReadHandler::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CCsConversationMarkReadHandler* CCsConversationMarkReadHandler::
NewL(CCsConversationCache* aCache)
    {
    CCsConversationMarkReadHandler* self = 
            new (ELeave) CCsConversationMarkReadHandler();
    CleanupStack::PushL(self);
    self->ConstructL(aCache);
    CleanupStack::Pop(self); // self
    return self;
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
CCsConversationMarkReadHandler::CCsConversationMarkReadHandler():
        CActive(CActive::EPriorityLow)
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
void CCsConversationMarkReadHandler::ConstructL(CCsConversationCache* aCache)
    {
    iCache = aCache;
    iState = EMarkReadIdle;
       
    iConversationEntryList = new (ELeave)RPointerArray<CCsConversationEntry> ();  
    iSession = CMsvSession::OpenSyncL(*this);
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
CCsConversationMarkReadHandler::~CCsConversationMarkReadHandler()
{
    Cancel();
    if (iConversationEntryList)
    {
		iConversationEntryList->ResetAndDestroy();
        iConversationEntryList->Close();
        delete iConversationEntryList;
        iConversationEntryList = NULL;
        
    }

    if (iSession)
    {
        delete iSession;
        iSession = NULL;
        }
    }

// ----------------------------------------------------------------------------
// Mark set of messages as read
// ----------------------------------------------------------------------------
void CCsConversationMarkReadHandler::MarkReadL(TInt aConversationId)
    {
    // Temp client conversation object
    CCsClientConversation* clientConversation = CCsClientConversation::NewL();
    clientConversation->SetConversationEntryId(aConversationId);
    CleanupStack::PushL(clientConversation);

    // Get conversationlist for given client conversation
    iCache->GetConversationsL (clientConversation, iConversationEntryList);
    
    iMarkReadCount = 0;
    
    // Cleanup  
    CleanupStack::PopAndDestroy(clientConversation);
        
    iState = EMarkReadStart;
    IssueRequest();
    }

// ----------------------------------------------------------------------------
// Mark read a message
// ----------------------------------------------------------------------------
void CCsConversationMarkReadHandler::MarkReadOneMessageL()
    {
    CCsConversationEntry* entry = 
            static_cast<CCsConversationEntry*>((*iConversationEntryList)[iMarkReadCount]);    
  
    if ( entry->IsAttributeSet(ECsAttributeNew) ||
         entry->IsAttributeSet(ECsAttributeUnread) )
        {    
        TInt id = entry->EntryId();          
        
        CMsvEntry* cEntry = NULL;
        TRAPD(err, cEntry = iSession->GetEntryL(id));
        if ( err == KErrNotFound )
           return;
        CleanupStack::PushL(cEntry);
        	
        TMsvEntry entry = cEntry->Entry();
        if ( entry.Unread() ) 
           {
           // Mark the entry as read
            if(entry.iMtm != KUidMsgTypeMultimedia)
            {
                entry.SetUnread( EFalse );
                cEntry->ChangeL( entry );
            }
           }
        CleanupStack::PopAndDestroy(cEntry);
        }
    }

// ----------------------------------------------------------------------------
// RunL
// ----------------------------------------------------------------------------
void CCsConversationMarkReadHandler::RunL()
    {
    switch ( iState )
        {
        case EMarkReadStart:
            if ( iConversationEntryList->Count() > 0 )
                {
                iMarkReadCount = iConversationEntryList->Count();
                iState = EMarkReadNext;
                IssueRequest();
                }
            else
                {
                iState = EMarkReadComplete;
                IssueRequest();
                }
            break;
        
        case EMarkReadNext:
            iMarkReadCount--;
            MarkReadOneMessageL();
            if ( iMarkReadCount > 0 )
                {
                iState = EMarkReadNext;
                IssueRequest();
                }
            else
                {
                iState = EMarkReadComplete;
                IssueRequest();
                }
            break;
            
        case EMarkReadComplete:
            // Cleanup, this is the last call RunL is not activated again.
            delete this;
            break;
    }
}

TInt CCsConversationMarkReadHandler::RunError(TInt /*aError*/)
{
    // RunL left so stop processing the AO and clean it.
    delete this;                     
    return KErrNone;
}

// ----------------------------------------------------------------------------
// DoCancel
// ----------------------------------------------------------------------------
void CCsConversationMarkReadHandler::DoCancel()
    {
    // Not supported
    }

// ---------------------------------------------------------------------------
// Move to next state
// ---------------------------------------------------------------------------
//
void CCsConversationMarkReadHandler::IssueRequest()
    {
    if( !IsActive() )
        {
        iStatus = KRequestPending;
        TRequestStatus* status = &iStatus;
        SetActive();
        User::RequestComplete(status, KErrNone);
        }
    }

// EOF
