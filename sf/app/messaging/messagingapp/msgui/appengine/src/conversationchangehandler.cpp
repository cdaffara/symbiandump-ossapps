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
 * Description:  ?Description
 *
 */

#include "conversationchangehandler.h"
// SYSTEM INCLUDES
#include <ccsconversationentry.h>

// USER INCLUDES
#include "conversationsengine.h"
#include "conversationsengine_p.h"
#include "conversationsmodel.h"

const TInt WindowSize = 7;

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
ConversationsChangeHandler::ConversationsChangeHandler(
    ConversationsEnginePrivate* convEnginePrivate,
    ConversationsModel* conversationsModel) :
    CActive(EPriorityStandard), 
    mCurrentState(EInit),
    mFirstWindowCached(EFalse),
    mCurrentIndex(-1), 
    mConversationsModel(conversationsModel), 
    mConvEnginePrivate(convEnginePrivate)

{
    CActiveScheduler::Add(this);
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
ConversationsChangeHandler::~ConversationsChangeHandler()
{
    mConversationEntryList.ResetAndDestroy();
    // Cancel the active object
    Cancel();
}

// ---------------------------------------------------------------------------
// This is for handling GetConversation results asynchronusly
// from the server.
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::ConversationsL(RPointerArray<
        CCsConversationEntry>& aConversationEntryList, TInt& aTotalCount)
{
    
    mTotalCount = aTotalCount;

    for (TInt i = 0; i < aConversationEntryList.Count(); ++i)
    {
        CCsConversationEntry *entry = aConversationEntryList[i]->CloneL();
        mConversationEntryList.AppendL(entry);
    }
    if (aConversationEntryList.Count() > 0)
    {
        mCurrentState = EInitialCache;
        IssueRequest();
    }
    else
    {
        //This is done for safety
        mConvEnginePrivate->registerForConversationUpdatesL();
        mCurrentState = EListenToEvents;
    }
}
// ---------------------------------------------------------------------------
// This is for resetting the values before initiating a request 
// for fetching entries for a new conversation
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::ResetValuesForNewConversation()
{
	mConvEnginePrivate->registerForConversationUpdatesL();
    mCurrentIndex = 0;
    mFirstWindowCached = EFalse;
    mConversationEntryList.ResetAndDestroy();
    

}

// ---------------------------------------------------------------------------
// Handling addition of new conversation entry from the server
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::AddConversation(
    const CCsConversationEntry& aConvEntry)
{
    mConversationsModel->addRow(aConvEntry, true);
    ConversationsEngine::instance()->emitConversationModelUpdated();
}

// ---------------------------------------------------------------------------
// Handling modification of existing conversation entry from the server
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::ModifyConversation(
    const CCsConversationEntry& aConvEntry)
{
    mConversationsModel->addRow(aConvEntry, true);
}

// ---------------------------------------------------------------------------
// Handling deletion of conversation entry from the server
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::DeleteConversation(
    const CCsConversationEntry& aConvEntry)
{
    mConversationsModel->deleteRow(aConvEntry.EntryId());
    
    //check if the CV model row count has become zero
    // then needs to emit signal
    if (mConversationsModel->rowCount() == 0)
    {
        mConversationsModel->emitConversationViewEmpty();
    }
}

//-----------------------------------------------------------------------
// This is for handling modify conversation event asynchronusly from the server
//-----------------------------------------------------------------------
//
void ConversationsChangeHandler::RefreshConversation()
{
}

// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::RunL()
{
    if (iStatus != KErrNone)
    {
        return;
    }
    //process
    switch (mCurrentState)
    {
        case EInitialCache:
            HandleConversationsL();
            break;
        case EFetchMoreConversations:
            FetchRemainingConversations(mConversationEntryList.Count());
            break;
    }
}

// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::DoCancel()
{
    mCurrentState = EListenToEvents;
}

// ---------------------------------------------------------------------------
// Make the active object alive.
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::IssueRequest()
{
    if (!IsActive())
    {
        iStatus = KRequestPending;
        TRequestStatus* status = &iStatus;
        SetActive();
        User::RequestComplete(status, KErrNone);
    }
}

// ---------------------------------------------------------------------------
//  Handles Conversations received from server and updates into model
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::HandleConversationsL()
{
    for (int i = 0; i < WindowSize; ++i)
    {
        if (mCurrentIndex < mConversationEntryList.Count())
        {
            mConversationsModel->addRow(
                * (mConversationEntryList[mCurrentIndex]));
            mCurrentIndex++;
        }
        else
        {
            break;
        }
    }
    if (mCurrentIndex < mConversationEntryList.Count())
    {
        if (!mFirstWindowCached)
        {
            ConversationsEngine::instance()->emitConversationModelPopulated();
            mFirstWindowCached = ETrue;
            return;
        }
        IssueRequest();
        return;
    }
    //if more entries have to be fetched , issue a request,
    // else listen for events.
    if(mCurrentIndex < mTotalCount )
    {
        //fetch more
        mCurrentState = EFetchMoreConversations;
        IssueRequest();
    }
    else
    {
        if (!mFirstWindowCached)
        {
            ConversationsEngine::instance()->emitConversationModelPopulated();
            mFirstWindowCached = ETrue;
        }
        mConversationEntryList.ResetAndDestroy();
        mCurrentState = EListenToEvents;
    }
}

// ---------------------------------------------------------------------------
//  Fetches remaining conversations from the server 
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::FetchRemainingConversations(TInt aTotalCount)
{
    mConvEnginePrivate->fetchRemainingConversations(aTotalCount);
    
}
// ---------------------------------------------------------------------------
//  Starts fetching remaining conversations
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::restartHandleConversations()
{
    IssueRequest();
}

//EOF
