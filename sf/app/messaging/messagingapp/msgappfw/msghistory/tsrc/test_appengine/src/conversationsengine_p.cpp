/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include "conversationsengine_p.h"

//SYSTEM INCLUDES
#include <ccsconversationentry.h>
#include <ccsclientconversation.h>
//USER INCLUDES
#include "conversationsenginedefines.h"
#include "conversationmsgstorehandler.h"
#include "conversationssummarymodel.h"
#include "conversationsmodel.h"

//---------------------------------------------------------------
// ConversationsEnginePrivate::ConversationsEnginePrivate
// @see header
//---------------------------------------------------------------
ConversationsEnginePrivate::ConversationsEnginePrivate(
    ConversationMsgStoreHandler* conversationMsgStoreHandler,
    ConversationsSummaryModel *conversationsSummaryModel,
    ConversationsModel *conversationsModel
    ):mServer(NULL),
    mClientConv(NULL),
    mConversationMsgStoreHandler(conversationMsgStoreHandler),
    mConversationsSummaryModel(conversationsSummaryModel),
    mConversationsModel(conversationsModel)
{
        TRAP_IGNORE(initL());
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::~ConversationsEnginePrivate
// @see header
//---------------------------------------------------------------
ConversationsEnginePrivate::~ConversationsEnginePrivate()
{
    if( mClientConv )
    {        
        if(mServer)
        {    
        mServer->RemoveConversationChangeEventL (this ,mClientConv);
        }
        delete mClientConv;
        mClientConv = NULL;
    }
    if(mServer)
    {
        mServer->RemoveConversationListChangeEventL (this);
        mServer->RemoveResultsEventL ( this );
        delete mServer;
        mServer = NULL;
    }
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::initL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::initL()
{
    mServer = CCSRequestHandler::NewL();
    
    //Add results observer to the server
    mServer->RequestResultsEventL(this);
    
    //Fetch the conversation list from server
    mServer->GetConversationListL();
    
    //Add ConversationListChange Observer
    mServer->RequestConversationListChangeEventL (this);
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::getConversationsL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::getConversationsL( TInt aConversationId)
{
    /*if(mClientConv && mClientConv->GetConversationEntryId()!=aConversationId )
    {    
        clearConversationsL();
        fetchConversationsL(aConversationId);
    }
    else */if(!mClientConv)
    {
        fetchConversationsL(aConversationId);
      }
    //}
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::deleteConversationL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::deleteConversationL(TInt aConversationId)
{
    mServer->DeleteConversationL(aConversationId);
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::deleteMessages
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::deleteMessages(RArray<TInt>& aIdArray)
{
    mConversationMsgStoreHandler->DeleteMessages(aIdArray);
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::markMessagesReadL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::markMessagesReadL(RArray<TInt>& aIdArray)
{
    mConversationMsgStoreHandler->MarkMessagesReadL(aIdArray);
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::getConversationCurrentId
// @see header
//---------------------------------------------------------------
TInt ConversationsEnginePrivate::getConversationCurrentId()
{
    TInt convId = -1;
    if(mClientConv)
    {
        convId = mClientConv->GetConversationEntryId();    
    }
    return convId;
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::getConversationIdFromContactId
// @see header
//---------------------------------------------------------------
TInt ConversationsEnginePrivate::getConversationIdFromContactIdL(TInt contactId)
{
    TInt convId = -1;
    
    convId = mServer->GetConversationIdL(contactId);
    
    return convId;
}


//---------------------------------------------------------------
// ConversationsEnginePrivate::clearConversationsL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::clearConversationsL()
{
    // Delete old CCsClientConversation object 
    // Remove the old Conversation change observer
    mServer->RemoveConversationChangeEventL (this ,mClientConv);
    delete mClientConv;
    mClientConv = NULL;
    
    //Clear conversations model before populating with new data 
    mConversationsModel->clear();  
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::fetchConversationsL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::fetchConversationsL(TInt aConversationId)
{
    //create a client conversation
    mClientConv = CCsClientConversation::NewL();
    mClientConv->SetConversationEntryId(aConversationId);
    //set dummy entry
    CCsConversationEntry *entry = CCsConversationEntry::NewL();
    mClientConv->SetConversationEntryL(entry);

    //Get the conversations for new conversationId 
    mServer->GetConversationsL( mClientConv );

    //Add the Conversation Change for new  conversationId
    mServer->RequestConversationChangeEventL (this ,mClientConv);    
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::ConversationList
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::ConversationList(
    RPointerArray<CCsClientConversation>& aClientConversationList)
{
    for(TInt i=0; i < aClientConversationList.Count(); ++i )
    {
        mConversationsSummaryModel->addRow(*(aClientConversationList[i]));
    }
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::Conversations
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::Conversations(
    RPointerArray<CCsConversationEntry>& aConversationEntryList)
{
    for(TInt i=aConversationEntryList.Count()-1; i >= 0; --i )
    {
        mConversationsModel->addRow(*(aConversationEntryList[i]));        
    }  
    /*mConversationsModel->setSortRole(ConvergedMsgId);
    mConversationsModel->sort(0,Qt::AscendingOrder);*/ 
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::AddConversationList
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::AddConversationList(
    const CCsClientConversation& aClientConversation)
{
    mConversationsSummaryModel->addRow(aClientConversation);
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::DeleteConversationList
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::DeleteConversationList(
    const CCsClientConversation& aClientConversation)
{
    mConversationsSummaryModel->deleteRow(
        aClientConversation.GetConversationEntryId());
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::ModifyConversationList
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::ModifyConversationList(
    const CCsClientConversation& aClientConversation)
{
    mConversationsSummaryModel->addRow(aClientConversation);
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::RefreshConversationList
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::RefreshConversationList()
{
    
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::AddConversation
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::AddConversation(
    const CCsConversationEntry& aConversationEntry)
{
    mConversationsModel->addRow(aConversationEntry);   
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::ModifyConversation
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::ModifyConversation(
   const CCsConversationEntry& aConversationEntry)
{
    mConversationsModel->addRow(aConversationEntry);   
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::DeleteConversation
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::DeleteConversation(
    const CCsConversationEntry& aConversationEntry)
{
    mConversationsModel->deleteRow(aConversationEntry.EntryId());      
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::RefreshConversation
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::RefreshConversation()
{
    
}


