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
 *
 */

//SYSTEM INCLUDES
#include <ccsrequesthandler.h>
#include <ccsconversationentry.h>
#include <ccsclientconversation.h>
#include <xqconversions.h>

//USER INCLUDES
#include "msghistoryprivate.h"
#include "msghistoryimpl.h"
#include <msgitem.h>

// CONSTANTS
_LIT(KUnixEpoch, "19700000:000000.000000");

const TInt KMaxConversationIPCLimit = 250;


//---------------------------------------------------------------
// MsgHistoryPrivate::MsgHistoryPrivate
// @see header
//---------------------------------------------------------------
MsgHistoryPrivate::MsgHistoryPrivate( MsgHistoryImpl* ptr )
:CActive(EPriorityStandard),
currentState(EInit),
q_ptr(ptr) 

    { 
    CActiveScheduler::Add(this);
    handler = CCSRequestHandler::NewL();
    handler->RequestResultsEventL(this);
    }

//---------------------------------------------------------------
// MsgHistoryPrivate::MsgHistoryPrivate
// @see header
//---------------------------------------------------------------
MsgHistoryPrivate::~MsgHistoryPrivate()
    {
    if ( handler )
        delete handler;
     if(msgs.count())
        {
         msgs.clear();
        }
    // Cancel the active Object 
    Cancel();
    }

// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void MsgHistoryPrivate::RunL()
{
    if (iStatus != KErrNone)
    {
        return;
    }
    //process
    switch (currentState)
    {
       case EFetchMoreConversations:
           GetMessagingHistory(contactId, msgs.count());
            break;
    }
}

// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void MsgHistoryPrivate::DoCancel()
{
    currentState = EInit;
    contactId = -1;
   
}

// ---------------------------------------------------------------------------
// Make the active object alive.
// ---------------------------------------------------------------------------
//
void MsgHistoryPrivate::IssueRequest()
{
    if (!IsActive())
    {
        iStatus = KRequestPending;
        TRequestStatus* status = &iStatus;
        SetActive();
        User::RequestComplete(status, KErrNone);
    }
}
//---------------------------------------------------------------
// MsgHistoryPrivate::GetMessagingHistory
// @see header
//---------------------------------------------------------------
TBool MsgHistoryPrivate::GetMessagingHistory( const TInt aContactId ,
                                              TInt aKnownIndex)
    {
    contactId = aContactId;
    TRAPD(err, handler->GetMessagingHistoryL(contactId,
                                             aKnownIndex,
                                             KMaxConversationIPCLimit));
    if ( err == KErrNone )
        return ETrue;

    return EFalse;
    }

//---------------------------------------------------------------
// MsgHistoryPrivate::ClearMessagingHistory
// @see header
//---------------------------------------------------------------
TBool MsgHistoryPrivate::ClearMessagingHistory( const TInt aContactId )
    {
    TRAPD(err, handler->ClearMessagingHistoryL(aContactId));
    if ( err == KErrNone )
        return ETrue;

    return EFalse;
    }

//---------------------------------------------------------------
// MsgHistoryPrivate::MarkReadMessagingHistory
// @see header
//---------------------------------------------------------------
TBool MsgHistoryPrivate::MarkReadMessagingHistory( const TInt aContactId )
    {
    TRAPD(err, handler->MarkMessagingHistoryReadL(aContactId));
    if ( err == KErrNone )
        return ETrue;

    return EFalse;
    }

//---------------------------------------------------------------
// MsgHistoryPrivate::Subscribe
// @see header
//---------------------------------------------------------------
TBool MsgHistoryPrivate::Subscribe( const TInt aContactId )
    {
    TInt conversationId = -1;
    TRAPD(err, 
            conversationId = handler->GetConversationIdL(aContactId));
    CCsClientConversation* clientConversation = CCsClientConversation::NewL();
    clientConversation->SetConversationEntryId(conversationId);
    // set the contact id, which will be used in case when the 
    // conversation entry id is -1.
    clientConversation->SetContactId(aContactId);
    //set dummy entry
    CCsConversationEntry *entry = CCsConversationEntry::NewL();
    clientConversation->SetConversationEntryL(entry);

    handler->RequestConversationChangeEventL(this, clientConversation);
    delete clientConversation;
    delete entry;        
    if ( err == KErrNone )
        return ETrue;

    return EFalse;
    }

//---------------------------------------------------------------
// MsgHistoryPrivate::UnSubscribe
// @see header
//---------------------------------------------------------------
TBool MsgHistoryPrivate::UnSubscribe( const TInt aContactId )
    {
    TInt conversationId = -1;
    TRAPD(err, 
            conversationId = handler->GetConversationIdL(aContactId));
    CCsClientConversation* clientConversation = CCsClientConversation::NewL();
    clientConversation->SetConversationEntryId(conversationId);
    // set the contact id, which will be used in case when the 
    // conversation entry id is -1.
    clientConversation->SetContactId(aContactId);
    //set dummy entry
    CCsConversationEntry *entry = CCsConversationEntry::NewL();
    clientConversation->SetConversationEntryL(entry);

    handler->RemoveConversationChangeEventL(this, clientConversation);
    delete clientConversation;
		
	delete entry;
    if ( err == KErrNone )
        return ETrue;

    return EFalse;
    }

//---------------------------------------------------------------
// MsgHistoryPrivate::ConversationList
// @see header
//---------------------------------------------------------------
void MsgHistoryPrivate::ConversationList
( RPointerArray<CCsClientConversation>& /*aClientConversationList*/ )
    {
    // No implementation required.
    }

//---------------------------------------------------------------
// MsgHistoryPrivate::Conversations
// @see header
//---------------------------------------------------------------
void MsgHistoryPrivate::Conversations
( RPointerArray<CCsConversationEntry>& aConversationEntryList,
        TInt& aTotalCount)
    {
 
    for(TInt i = 0 ; i < aConversationEntryList.Count(); i++ )
        {
        MsgItem item;
        PopulateMsgItem(item,*(aConversationEntryList[i]));
        msgs.append(item);
        }  
    //emit signal, when all the messages in the conversation are fetched.
    if(msgs.count()== aTotalCount)
        {
            currentState = EInit;
            q_ptr->messagesReadyEvent(msgs);
            msgs.clear();

        }
        else
        {
            currentState = EFetchMoreConversations;
            IssueRequest();
        }
    }

//---------------------------------------------------------------
// MsgHistoryPrivate::AddConversation
// @see header
//---------------------------------------------------------------
void MsgHistoryPrivate::AddConversation 
( const CCsConversationEntry& aConversationEntry )
    {
    MsgItem item;
    PopulateMsgItem(item, aConversationEntry);
    q_ptr->messageAddedEvent(item);
    }

//---------------------------------------------------------------
// MsgHistoryPrivate::ModifyConversation
// @see header
//---------------------------------------------------------------
void MsgHistoryPrivate::ModifyConversation
( const CCsConversationEntry& aConversationEntry )
    {
    MsgItem item;
    PopulateMsgItem(item, aConversationEntry);
    q_ptr->messageChangedEvent(item);
    }

//---------------------------------------------------------------
// MsgHistoryPrivate::DeleteConversation
// @see header
//---------------------------------------------------------------
void MsgHistoryPrivate::DeleteConversation
( const CCsConversationEntry& aConversationEntry )
    {
    MsgItem item;
    PopulateMsgItem(item, aConversationEntry);
    q_ptr->messageDeletedEvent(item);
    }

//---------------------------------------------------------------
// MsgHistoryPrivate::RefreshConversation
// @see header
//---------------------------------------------------------------
void MsgHistoryPrivate::RefreshConversation()
    {
    // No implementation required.
    }

//---------------------------------------------------------------
// MsgHistoryPrivate::populateMsgItem
// @see header
//---------------------------------------------------------------
void MsgHistoryPrivate::PopulateMsgItem
(MsgItem& item, const CCsConversationEntry& entry)
    {
    // id             
    item.setId(entry.EntryId());
    
    // message type.        
    item.setType((MsgItem::MsgType)entry.GetType());

    // description
    HBufC* description = entry.Description();    
    if( description && description->Length())
        {
        item.setBody(XQConversions::s60DescToQString(*description)); 
        }

    // time stamp 
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    TTime timeStamp(entry.TimeStamp() );
    timeStamp.SecondsFrom(unixEpoch, seconds);    
    item.setTimeStamp(seconds.Int());

    //contact details
    HBufC* contact = entry.Contact();
    if( contact && contact->Length())
        {
        item.setPhoneNumber(XQConversions::s60DescToQString(*contact));      
        }

    //sending state
    item.setState((MsgItem::MsgState)entry.GetSendState());    
    
    // direction
    item.setDirection((MsgItem::MsgDirection)entry.ConversationDir());  
    
    //attributes
    SetMsgAttributes(item, entry);    
    }

//---------------------------------------------------------------
// MsgHistoryPrivate::SetMsgAttributes
// @see header
//---------------------------------------------------------------
void MsgHistoryPrivate::SetMsgAttributes
(MsgItem& item, const CCsConversationEntry& entry)
    {
    //draft
    if(entry.IsAttributeSet(ECsAttributeDraft))
        {
        item.changeAttributes(MsgItem::MsgAttributeDraft); 
        }
    //sent
    if(entry.IsAttributeSet(ECsAttributeSent))
        {
        item.changeAttributes(MsgItem::MsgAttributeSent);  
        }
    //new
    if(entry.IsAttributeSet(ECsAttributeNew))
        {
        item.changeAttributes(MsgItem::MsgAttributeNew); 
        }
    //unread
    if(entry.IsAttributeSet(ECsAttributeUnread))
        {
        item.changeAttributes(MsgItem::MsgAttributeUnread); 
        }
    //High priority
    if(entry.IsAttributeSet(ECsAttributeHighPriority))
        {
        item.changeAttributes(MsgItem::MsgAttributeHighPriority); 
        }
    //low priority
    if(entry.IsAttributeSet(ECsAttributeLowPriority))
        {
        item.changeAttributes(MsgItem::MsgAttributeLowPriority); 
        }
    //attachment
    if(entry.IsAttributeSet(ECsAttributeAttachment))
        {
        item.changeAttributes(MsgItem::MsgAttributeAttachment); 
        }
    }

//EOF
