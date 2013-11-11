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

// INCLUDES
#include "conversationsengine.h"
#include "testconversationengine.h"
#include "testconversationenginestub.h"
#include "testconversationenginestub.h"
#include "testconversationupdatehandler.h"
#include <ccsclientconversation.h>
#include <ccsconversationentry.h>
#include <xqconversions.h>
#include <conversationsenginedefines.h>


void TConversationEngine::initTestCase()
{
    iConvUpdateHandler = new TConversationUpdateHandler();
    QVERIFY2(ConversationsEngine::instance() != NULL, "Appengine is not initialized.");
    QVERIFY2(TestConversationEngineStub::Instance() != NULL, "Appengine Stub is not initialized.");
    
}

void TConversationEngine::init()
{
    // allocate, setup the neccessary environment for 
    // current test case to execute 
}

// fetch the conversations from server
void TConversationEngine::GetConversationsFromServer()
{
    QVERIFY2(ConversationsEngine::instance()->
            getConversations(TestConversationEngineStub::Instance()->
                            GetConversationID()), "Get conversations failed");
}

//fetch the coversation and validate conversation view
void TConversationEngine::FetchConversationsAndUpdateCV()
{
    QVERIFY2(ConversationsEngine::instance()->
            getConversations(TestConversationEngineStub::Instance()->
                            GetConversationID()), "Get conversations failed");
    
    //should be used once, will work for all the test cases
    iConvUpdateHandler->SetConversationsViewUpdateHandler();
    
    //listen for emitted signal
    QSignalSpy convListUpdate(ConversationsEngine::instance(), 
                        SIGNAL(conversationModelPopulated()));
    
    // Now we check to make sure we don't have any signals already
    QCOMPARE( convListUpdate.count(), 0 );

    //update the coversation model and 
    //wait for active object to execute
    TestConversationEngineStub::Instance()->UpdateConversationList();
    QTest::qWait(2000);
    
    //conversation engine should have emitted signal
    QCOMPARE( convListUpdate.count(), 1 );        
}

//validate the current conversation id
void TConversationEngine::CheckCurrentConversationID()
{
    QVERIFY2((ConversationsEngine::instance()->getCurrentConversationId() 
            == TestConversationEngineStub::Instance()->GetConversationID()), 
            "Current conversation id mismatch");
}

//mark the conversation as read and validate the conversation view
void TConversationEngine::MarkConversationReadAndUpdateCV()
{
    //mark the conversation as read
    ConversationsEngine::instance()->
            markConversationRead(TestConversationEngineStub::Instance()->
                                GetConversationID());

    //listen for emitted signal
    QSignalSpy convModify(ConversationsEngine::instance(), 
                        SIGNAL(conversationModelUpdated()));
    
    // Now we check to make sure we don't have any signals already
    QCOMPARE( convModify.count(), 0 );

    // update the conversation view with modified conversation 
    TestConversationEngineStub::Instance()->
            UpdateConvEntry(TestConversationEngineStub::Instance()->
                                GetConversationID());
    
    ConversationsEngine::instance()->emitConversationModelUpdated();
    
    //conversation engine should have emitted signal
    QCOMPARE( convModify.count(), 1 );
}

//delete the conversation and validate updetad conversation view
void TConversationEngine::DeleteConversationAndUpdateCV()
{
    // delete the conversation entry
    ConversationsEngine::instance()->
            deleteConversations(TestConversationEngineStub::Instance()->
                                GetConversationID());
   
    //listen for emitted signal
    QSignalSpy convDelete(ConversationsEngine::instance(), 
                        SIGNAL(conversationModelUpdated()));
    
    // Now we check to make sure we don't have any signals already
    QCOMPARE( convDelete.count(), 0 );
    
    // update the conversation view with deleted entry  
    TestConversationEngineStub::Instance()->UpdateDeletedConvEntry();

    ConversationsEngine::instance()->emitConversationModelUpdated();
    
    //conversation engine should have emitted signal
    QCOMPARE( convDelete.count(), 1 );
}

//notify new recieved conversation and validate conversation view
void TConversationEngine::NotifyNewConversationEntryAndUpdateCV()
{
    //listen for emitted signal
    QSignalSpy convAdd(ConversationsEngine::instance(), 
                        SIGNAL(conversationModelUpdated()));
    
    // Now we check to make sure we don't have any signals already
    QCOMPARE( convAdd.count(), 0 );

    TestConversationEngineStub::Instance()->UpdateAddConvEntry();
    
    //conversation engine should have emitted signal
    QCOMPARE( convAdd.count(), 1 );
}

//update conversation client list and validate conversation summary model
void TConversationEngine::NotifyConversationClientListAndUpdateCLV()
{
    //should be used once, will work for all the test cases
    //set the conversation summary view update handler 
    iConvUpdateHandler->SetConversationsSummaryViewUpdateHandler();

    //listen for emitted signal
    QSignalSpy convClientList(ConversationsEngine::instance(), 
                        SIGNAL(conversationListModelPopulated()));
    
    // Now we check to make sure we don't have any signals already
    QCOMPARE( convClientList.count(), 0 );
    
    //update the conversation client list to summary model
    TestConversationEngineStub::Instance()->UpdateConversationClientList();
    QTest::qWait(2000);
    
    //conversation engine should have emitted signal
    QCOMPARE( convClientList.count(), 1 );
}

//get contactt details associated with conversation id
void TConversationEngine::GetContactDetailsFromConversationID()
{
    //get the conversation client list
    RPointerArray<CCsClientConversation>&  clientList 
            = TestConversationEngineStub::Instance()->GetConversationClientList();
    
    //match the client list with summary model list
    for (int loop = 0; loop < clientList.Count(); loop++)
    {
        CCsClientConversation* clientConv = clientList[loop];
         
        qint64 msgId = clientConv->GetConversationEntryId();
         
        QString fname;
        QString address;
         
        ConversationsEngine::instance()->
                getContactDetails(msgId, fname, address);
         
        qDebug() << "msgId " << msgId;
          
        //check the bunch of converation client details 
        QCOMPARE(fname,
               XQConversions::s60DescToQString(
                       *(clientConv->GetDisplayName())));
          
        QCOMPARE(address,
                XQConversions::s60DescToQString(
                        *(clientConv->GetConversationEntry()->Contact())));
    }
}

// get conversation id associated with contact id
void TConversationEngine::GetConversationIDFromContatcID()
{
    //get the conversation client list
    RPointerArray<CCsClientConversation>&  clientList 
            = TestConversationEngineStub::Instance()->GetConversationClientList();

    //match the client list with summary model list
    for (int loop = 0; loop < clientList.Count(); loop++)
     {
         CCsClientConversation* clientConv = clientList[loop];
         
         qint64 msgId = clientConv->GetConversationEntryId();
         
         qint64 convID = ConversationsEngine::instance()->
                 getConversationIdFromContactId(clientConv->GetContactId());
         QCOMPARE(convID, msgId);
     }
}

// update the new conversation client to summary view 
void TConversationEngine::NotifyNewConversationClientAndUpdateCLV()
{
    //Add and update new conversation client to summary model
    TestConversationEngineStub::Instance()->UpdateAddConvClient();

    //emit the signal for validating the summary model
    ConversationsEngine::instance()->emitConversationListModelPopulated();
}

//update the modified conversation client
void TConversationEngine::NotifyModifyConversationClientAndUpdateCLV()
{
    //update the modified conversation client to summary model
    TestConversationEngineStub::Instance()->
            UpdateConvClient(TestConversationEngineStub::Instance()->GetConversationID());
    
    //emit the signal for validating the summary model
    ConversationsEngine::instance()->emitConversationListModelPopulated();
}

//update the deleted conversation client
void TConversationEngine::NotifyDeleteConversationClientAndUpdateCLV()
{
    //update the deleted conversation client to summary model
    TestConversationEngineStub::Instance()->UpdateDeletedConvClient();
    
    //emit the signal for validating the summary model
    ConversationsEngine::instance()->emitConversationListModelPopulated();
}

//get the conversation id from address
void TConversationEngine::GetConversationIDFromAddress()
{
    //get the conversation client list
    RPointerArray<CCsClientConversation>&  clientList 
                = TestConversationEngineStub::Instance()->GetConversationClientList();

    //math the conversation id recieved from summary model
    if (clientList.Count())
    {
         qint64 msgId = clientList[0]->GetConversationEntryId();
         
         QStandardItemModel* convModel = ConversationsEngine::instance()->
                                                 getConversationsSummaryModel();
  
         //match convid in model, if not found raise error
         QModelIndexList indexList = convModel->match(convModel->index(0, 0), 
                  ConversationId, msgId, 1, Qt::MatchExactly);

         QVERIFY2(indexList.count(), "No item found in summary model");
         
         qint64 convID = ConversationsEngine::instance()->
                         getConversationIdFromAddress(
                         convModel->data(indexList[0], ConversationAddress).toString());
         QCOMPARE(convID, msgId);
    }
}

//fetch more conversations from server
void TConversationEngine::FetchMoreConversations()
{
    ConversationsEngine::instance()->fetchMoreConversations();
}

//clear conversations from conversation view
void TConversationEngine::ClearConversations()
{
    //clear the conversations
    ConversationsEngine::instance()->clearConversations();
    
    //validate the conversation view, and raise the error if entry found
    QStandardItemModel* convModel 
                        =  ConversationsEngine::instance()->getConversationsModel();
    QVERIFY2(convModel->rowCount() == 0, "conversation view not cleared");     
}

//get the drafts model
void TConversationEngine::GetDraftsModelFromConversationEngine()
{
    QStandardItemModel* convModel 
                =  ConversationsEngine::instance()->getDraftsModel();
    QVERIFY2(convModel, "Get Drafts Model Failed");
}

void TConversationEngine::cleanup()
{
    //clean the resources for the current test case
}

void TConversationEngine::cleanupTestCase()
{
    //delete the update handler
    delete iConvUpdateHandler;
    iConvUpdateHandler = NULL;
    //delete the conversation engine variable
    delete ConversationsEngine::instance();
    //delete the stub data
    delete TestConversationEngineStub::Instance();
}


//main entry point
int main(int argc, char *argv[])
    { 
    int ret = -1;
    QCoreApplication app(argc, argv);
    QObject* tc = new TConversationEngine();
    ret =  QTest::qExec(tc, argc, argv);
	delete tc;
    return ret;
    }

