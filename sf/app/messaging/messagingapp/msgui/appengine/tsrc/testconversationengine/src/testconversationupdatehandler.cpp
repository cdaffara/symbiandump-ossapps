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

#include "testconversationupdatehandler.h"
#include "testconversationenginestub.h"
#include "conversationsengine.h"
#include <conversationsenginedefines.h>
#include <QStandardItemModel>
#include <xqconversions.h>
#include <ccsdefs.h>
#include <ccsclientconversation.h>


TConversationUpdateHandler::TConversationUpdateHandler(QObject* parent)
: QObject(parent)
{
    
}
    

void TConversationUpdateHandler::SetConversationsViewUpdateHandler()
{
    connect(ConversationsEngine::instance(), 
                SIGNAL(conversationModelPopulated()), 
                this, 
                SLOT(ConversationsViewUpdated()));

    connect(ConversationsEngine::instance(), 
                SIGNAL(conversationModelUpdated()), 
                this, 
                SLOT(ConversationsViewUpdated()));
}

void TConversationUpdateHandler::SetConversationsSummaryViewUpdateHandler()
{
    connect(ConversationsEngine::instance(), 
                SIGNAL(conversationListModelPopulated()), 
                this, 
                SLOT(ConversationsSummaryViewUpdated()));
}

void TConversationUpdateHandler::ConversationsViewUpdated()
{
    int loop, entryCount, unRead = 0;
    
    QWARN("List is updated to ..Conversation Model");

    //Need to compare the Msglist with stub engine
    QStandardItemModel* convModel = ConversationsEngine::instance()->getConversationsModel();
    
    //get the list with conversation address, 
    //as this is common for all the conversation entries 
    QModelIndexList indexList = convModel->match(convModel->index(0, 0), 
                                                ConversationAddress, 
                                                XQConversions::s60DescToQString(TestConversationEngineStub::Instance()->GetContactID()), 
                                                -1, // One match 
                                                Qt::MatchExactly);
    entryCount = indexList.count();
    
    qDebug() << "entry count " << entryCount;
    qDebug() << "row count " << convModel->rowCount();
    
    QCOMPARE(entryCount, 
            TestConversationEngineStub::Instance()->GetConvListSize());

    //match all the entries with stub conversation list
    for(loop = 0; loop < entryCount; loop++)
    {
        //check for bunch of conversation fields and attributes
        QCOMPARE(convModel->data(indexList[loop], MessageType).toInt(),
                TestConversationEngineStub::Instance()->GetMsgType());

        //compare the message description
        QCOMPARE(convModel->data(indexList[loop], BodyText).toString(),
                XQConversions::s60DescToQString(
                        TestConversationEngineStub::Instance()->
                         GetDescription()));
        
        //check the unread message status 
        if (convModel->data(indexList[loop], UnReadStatus).toBool())
        {
           unRead++;
        }
        
        qDebug() << "Conversation IDs " << convModel->data(indexList[loop], ConvergedMsgId).toInt();
     }
    
    //check the unread messages count
    QCOMPARE(unRead, TestConversationEngineStub::Instance()->GetUnreadCount());
}

void TConversationUpdateHandler::ConversationsSummaryViewUpdated()
{
    QWARN("List is updated to ..Conversation Summary Model");

    //get the conversation client list
    RPointerArray<CCsClientConversation>&  clientList 
        = TestConversationEngineStub::Instance()->GetConversationClientList();

    //get the converation summary model
    QStandardItemModel* convModel = ConversationsEngine::instance()->getConversationsSummaryModel();
    
    //list size and rows in conversation model must be the same
    QCOMPARE(convModel->rowCount(), clientList.Count());
    

    //match all the client entries with stub conversation client list
    for (int loop = 0; loop < clientList.Count(); loop++)
    {
        CCsClientConversation* clientConv = clientList[loop];
        
        qint64 msgId = clientConv->GetConversationEntryId();
        
        //match convId in model, if not found raise error
        QModelIndexList indexList = convModel->match(convModel->index(0, 0), 
                 ConversationId, msgId, 1, Qt::MatchExactly);
        
         qDebug() << "msgId " << msgId;
         qDebug() << "index list " << indexList.count();
         
         if (indexList.count() == 0)
         {
            QFAIL("Conv client not found");
         }
     
         //check the bunch of converation client details 
         
         int msgCnt = clientConv->GetUnreadMessageCount();
         QCOMPARE(convModel->data(indexList[0], UnreadCount).toInt(), msgCnt);
         
         TDesC* dispName = clientConv->GetDisplayName();
         QCOMPARE(convModel->data(indexList[0], DisplayName).toString(),
                 XQConversions::s60DescToQString(*dispName));
         
         int contactId = clientConv->GetContactId();
         QCOMPARE(convModel->data(indexList[0], ContactId).toInt(), contactId);

         //need to check the conversation entry
    }
 }

