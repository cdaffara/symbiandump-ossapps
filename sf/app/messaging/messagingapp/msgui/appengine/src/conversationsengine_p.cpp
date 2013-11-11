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
#include "conversationchangehandler.h"
#include "conversationlistchangehandler.h"
#include "debugtraces.h"


//CONSTANTS
/**
 *Max number of conversation that can be exchanged in IPC call
 */
const TInt KMaxConversationIPCLimit =  250;
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
        initL();
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::~ConversationsEnginePrivate
// @see header
//---------------------------------------------------------------
ConversationsEnginePrivate::~ConversationsEnginePrivate()
{

    if( mClientConv )
    {        
        if(mServer && mConvChangeHandler)
        {    
        TRAP_IGNORE(mServer->RemoveConversationChangeEventL(
                mConvChangeHandler,mClientConv));
        }
        delete mClientConv;
        mClientConv = NULL;
    }
    if(mServer && mConvListChangeHandler)
    {
        TRAP_IGNORE(mServer->RemoveConversationListChangeEventL(
                mConvListChangeHandler));
        TRAP_IGNORE(mServer->RemoveResultsEventL ( this ));
        mServer->Cancel();
        delete mServer;
        mServer = NULL;
    }
    
    delete mConvChangeHandler;
    
    delete mConvListChangeHandler;
    
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
    
    mConvChangeHandler = new ConversationsChangeHandler(this,
        mConversationsModel);
    
    mConvListChangeHandler = new ConversationsListChangeHandler(this,
        mConversationsSummaryModel);
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::registerForConversationListUpdatesL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::registerForConversationListUpdatesL()
{
    //Add ConversationListChange Observer
    mServer->RequestConversationListChangeEventL (mConvListChangeHandler);
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::getConversationsL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::getConversationsL( TInt aConversationId)
{	
    if(!mClientConv)
    {
        QCRITICAL_WRITE("ConversationsEnginePrivate::getConversationsL start.");
        //Clear the model before issueing fetch
        mConversationsModel->clear();
        //create a client conversation
        mClientConv = CCsClientConversation::NewL();
        mClientConv->SetConversationEntryId(aConversationId);
        //set dummy entry
        CCsConversationEntry *entry = CCsConversationEntry::NewL();
        CleanupStack::PushL(entry);
        mClientConv->SetConversationEntryL(entry);
        CleanupStack::PopAndDestroy(entry);
        // Reset the values in change handler before initiating a request
        mConvChangeHandler->ResetValuesForNewConversation();
        
        //Get the conversations for new conversationId 
        mServer->GetConversationsL(mClientConv,0,KMaxConversationIPCLimit);

        QCRITICAL_WRITE("ConversationsEnginePrivate::getConversationsL end.");
        }   
    
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
// ConversationsEnginePrivate::deleteallDraftMessages
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::deleteAllDraftMessagesL()
{
    mConversationMsgStoreHandler->DeleteAllDraftMessagesL();
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::markConversationReadL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::markConversationReadL(TInt aConversationId)
{
    mServer->MarkConversationReadL(aConversationId);
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
// ConversationsEnginePrivate::getConversationIdFromAddress
// @see header
//---------------------------------------------------------------
TInt ConversationsEnginePrivate::getConversationIdFromAddressL(TDesC& contactAddress)
{
    TInt convId = -1;
    
    convId = mServer->GetConversationIdFromAddressL(contactAddress);
    
    return convId;
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::getConversationFromConversationId
// @see header
//---------------------------------------------------------------
CCsClientConversation* ConversationsEnginePrivate::getConversationFromConversationIdL(TInt aConversationId)
{
     return mServer->GetConversationFromConversationIdL(aConversationId);
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::clearConversationsL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::clearConversationsL()
{    
    QCRITICAL_WRITE("ConversationsEnginePrivate::clearConversationsL start.");
    
    mConvChangeHandler->Cancel();
    //Clear conversations model before populating with new data 
    mConversationsModel->clearModel();
    
    // Delete old CCsClientConversation object 
    // Remove the old Conversation change observer
    if(mClientConv)
    {   
        int error = KErrNone;
        TRAP(error, mServer->RemoveConversationChangeEventL (mConvChangeHandler ,mClientConv));
        delete mClientConv;
        mClientConv = NULL;
        User::LeaveIfError(error);
    }
    
    QCRITICAL_WRITE("ConversationsEnginePrivate::clearConversationsL end.");
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::registerForConversationUpdatesL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::registerForConversationUpdatesL()
{
    //Add the Conversation Change for new  conversationId
    if(mClientConv)
    {    
    mServer->RequestConversationChangeEventL (mConvChangeHandler ,mClientConv);
    }
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::deRegisterCVUpdatesTemporary
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::deRegisterCVUpdatesTemporary()
{
    mServer->RemoveConversationChangeEventL (mConvChangeHandler ,mClientConv);
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::registerAgainForConversationUpdatesL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::registerAgainForConversationUpdatesL(
        int newConversationId)
{
    //Add the Conversation Change for new  conversationId
    if(mClientConv)
    {    
    mClientConv->SetConversationEntryId(newConversationId);
    mServer->RequestConversationChangeEventL (mConvChangeHandler ,mClientConv);
    }
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::ConversationList
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::ConversationList(
    RPointerArray<CCsClientConversation>& aClientConversationList)
{
    int error;
    TRAP(error,
        mConvListChangeHandler->ConversationListL(aClientConversationList));
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::Conversations
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::Conversations(
    RPointerArray<CCsConversationEntry>& aConversationEntryList,
    TInt& aTotalCount)
{
    int error;
    if (mClientConv)
        {
        QCRITICAL_WRITE("ConversationsEnginePrivate::Conversations start.");

        TRAP(error,mConvChangeHandler->ConversationsL(aConversationEntryList,aTotalCount));

        QCRITICAL_WRITE("ConversationsEnginePrivate::Conversations end.");
        }
}


//---------------------------------------------------------------
// ConversationsEngine::fetchMoreConversations
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::fetchMoreConversations()
{
    if (mClientConv)
        {
        mConvChangeHandler->restartHandleConversations();
        }
}

//---------------------------------------------------------------
// ConversationsEngine::fetchRemainingConversations
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::fetchRemainingConversations(TInt& aCount)
    {
    if ( mServer && mClientConv )
            {
                    
            // Get conversations from server
             mServer->GetConversationsL(mClientConv, 
                     (aCount - 1),
                    KMaxConversationIPCLimit);
            }
    }
//---------------------------------------------------------------
// ConversationsEnginePrivate::resendMessage()
// @see header
//---------------------------------------------------------------
bool ConversationsEnginePrivate::resendMessage(TMsvId messageId)
{
    TInt err = KErrNone;
    bool retval = true;
    TRAP(err, retval = mConversationMsgStoreHandler->ResendMessageL(messageId));
    return ((err == KErrNone) && (retval == true))?true:false;
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::downloadMessage
// @see header
//---------------------------------------------------------------
TInt ConversationsEnginePrivate::downloadMessage(TMsvId messageId)
{
    TInt err = KErrNone;
    TInt retval = KErrNone;
    TRAP(err, retval = 
            mConversationMsgStoreHandler->DownloadMessageL(messageId));
            
    if( err!=KErrNone)
    {
        retval = KErrGeneral;
    }
    
    return retval;
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::downloadOperationSupported()
// @see header
//---------------------------------------------------------------
bool ConversationsEnginePrivate::downloadOperationSupported(TMsvId messageId)
{
  return mConversationMsgStoreHandler->DownloadOperationSupported(messageId);   
}
//EOF

