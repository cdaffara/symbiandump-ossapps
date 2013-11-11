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
#include <ccsconversationentry.h>
#include "conversationsengine.h"

#include "conversationsenginedefines.h"
#include "convergedmessage.h"
#include "conversationmsgstorehandler.h"
#include "conversationssummarymodel.h"
#include "conversationsmodel.h"
#include "conversationsengine_p.h"
#include "debugtraces.h"
#include <msghistory.h>
#include <msgitem.h>
#include <xqconversions.h>

const int ViewableLimit = 160;
//---------------------------------------------------------------
// ConversationsEngine::instance
// @see header
//---------------------------------------------------------------
ConversationsEngine* ConversationsEngine::instance()
{
    static ConversationsEngine* conversationsEngine = new ConversationsEngine();
    return conversationsEngine;
}

//---------------------------------------------------------------
// ConversationsEngine::ConversationsEngine
// @see header
//---------------------------------------------------------------
ConversationsEngine::ConversationsEngine(QObject* parent) :
    QObject(parent)
{
    mConversationMsgStoreHandler = new ConversationMsgStoreHandler;
    mConversationsSummaryModel = new ConversationsSummaryModel(this);
    mConversationsModel = new ConversationsModel(mConversationMsgStoreHandler,
                                                 this);

    d_ptr = new ConversationsEnginePrivate(mConversationMsgStoreHandler,
                                           mConversationsSummaryModel,
                                           mConversationsModel);

    mMsgHistory = new MsgHistory(this);

}

//---------------------------------------------------------------
// ConversationsEngine::~ConversationsEngine
// @see header
//---------------------------------------------------------------
ConversationsEngine::~ConversationsEngine()
{
    delete mConversationMsgStoreHandler;
    delete d_ptr;
}

//---------------------------------------------------------------
// ConversationsEngine::getConversationsSummaryModel
// @see header
//---------------------------------------------------------------
QStandardItemModel* ConversationsEngine::getConversationsSummaryModel()
{
    return mConversationsSummaryModel;
}

//---------------------------------------------------------------
// ConversationsEngine::getConversationsModel
// @see header
//---------------------------------------------------------------
QStandardItemModel* ConversationsEngine::getConversationsModel()
{
    return mConversationsModel;
}

//---------------------------------------------------------------
// ConversationsEngine::getConversations
// @see header
//---------------------------------------------------------------
void ConversationsEngine::getConversations(qint64 conversationId)
{
    int error;
    mContactId = conversationId;

    mMsgHistory->getMessages(conversationId);
    mMsgHistory->subscribe(conversationId);

    //connect to the signals
    connect(mMsgHistory, SIGNAL(messagesReady(QList<MsgItem>&)),
    this, SLOT(onMessagesReady(QList<MsgItem>&)));
    connect(mMsgHistory, SIGNAL(messageAdded(MsgItem&)), this, SLOT(onMessageAdded(MsgItem&)));

    connect(mMsgHistory, SIGNAL(messageChanged(MsgItem&)), this, SLOT(onMessageChanged(MsgItem&)));

    connect(mMsgHistory, SIGNAL(messageDeleted(MsgItem&)), this, SLOT(onMessageDeleted(MsgItem&)));    
    

    //TRAP(error,d_ptr->getConversationsL(conversationId));   
}

void ConversationsEngine::onMessagesReady(QList<MsgItem>& msgs)
{
    for (int i = 0; i < msgs.count(); i++)
    {
        MsgItem item = msgs.at(i);
        CCsConversationEntry* entry = CCsConversationEntry::NewL();
        populateEntry(item, entry);
        mConversationsModel->addRow(*entry);
        delete entry;
    }
    
    mMsgHistory->markRead(mContactId);
}

void ConversationsEngine::populateEntry(MsgItem& item,
                                        CCsConversationEntry* entry)
{
    // id             
    entry->SetEntryId(item.id());

    // message type. 
    entry->SetType(item.type());

    // description
    QString descr = item.body();
    HBufC* description = XQConversions::qStringToS60Desc(descr);
    entry->SetDescriptionL(description->Des());
    delete description;

    // time stamp 
    QString tt = item.timeStamp().toString();
    entry->SetTimeStampL(tt.toInt());

    //contact details
    QString details = item.phoneNumber();
    HBufC* d_details = XQConversions::qStringToS60Desc(details);
    entry->SetContactL(d_details->Des());
    delete d_details;

    //sending state
    entry->SetSendState(item.state());

    // direction
    entry->SetConversationDir(item.direction());

    //attributes
    if (item.isAttributeSet(MsgItem::MsgAttributeUnread))
    {
        entry->ChangeAttributes(ECsAttributeUnread, ECsAttributeNone);
    }
    if (item.isAttributeSet(MsgItem::MsgAttributeSent))
    {
        entry->ChangeAttributes(ECsAttributeNew, ECsAttributeNone);
    }
    if (item.isAttributeSet(MsgItem::MsgAttributeNew))
    {
        entry->ChangeAttributes(ECsAttributeNew, ECsAttributeNone);
    }
    if (item.isAttributeSet(MsgItem::MsgAttributeAttachment))
    {
        entry->ChangeAttributes(ECsAttributeAttachment, ECsAttributeNone);
    }
    if (item.isAttributeSet(MsgItem::MsgAttributeHighPriority))
    {
        entry->ChangeAttributes(ECsAttributeHighPriority, ECsAttributeNone);
    }
}

void ConversationsEngine::onMessageAdded(MsgItem& item)
{

    CCsConversationEntry* entry = CCsConversationEntry::NewL();
    populateEntry(item, entry);
    mConversationsModel->addRow(*entry);
    delete entry;
}
void ConversationsEngine::onMessageChanged(MsgItem& item)
{
    CCsConversationEntry* entry = CCsConversationEntry::NewL();
    populateEntry(item, entry);
    mConversationsModel->addRow(*entry);
    delete entry;
}
void ConversationsEngine::onMessageDeleted(MsgItem& item)
{
    mConversationsModel->deleteRow(item.id());
}
//---------------------------------------------------------------
// ConversationsEngine::clearConversations
// @see header
//---------------------------------------------------------------
void ConversationsEngine::clearConversations()
{
    int error;
    //TRAP(error,d_ptr->clearConversationsL());

    //clear model
    mConversationsModel->clear();
    mMsgHistory->unSubscribe(mContactId);
    mContactId = -1;
}

//---------------------------------------------------------------
// ConversationsEngine::deleteConversations
// @see header
//---------------------------------------------------------------
void ConversationsEngine::deleteConversations(qint64 conversationId)
{
    int error;
    //TRAP(error,d_ptr->deleteConversationL(conversationId));  
    mMsgHistory->clearMessages(conversationId);
}

//---------------------------------------------------------------
// ConversationsEngine::deleteMessages
// @see header
//---------------------------------------------------------------
void ConversationsEngine::deleteMessages(QList<int>& msgIdList)
{
    RArray<TInt> idArray;
    for (int i = 0; i < msgIdList.count(); ++i)
    {
        idArray.Append(msgIdList[i]);
    }
    if (msgIdList.count() > 0)
    {
        d_ptr->deleteMessages(idArray);
    }
}

//---------------------------------------------------------------
// ConversationsEngine::getUnReadMessages
// @see header
//---------------------------------------------------------------
void ConversationsEngine::getUnReadMessages(QList<int>& msgIdList)
{
    QModelIndexList indexList =
            mConversationsModel->match(mConversationsModel->index(0, 0),
                                       UnReadStatus,
                                       true,
                                       -1, // all matching hits 
                                       Qt::MatchExactly);

    int count = indexList.count();
    for (int i = 0; i < count; ++i)
    {
        if ( (indexList[i].data(MessageType).toInt() == ConvergedMessage::Sms)
                && (indexList[i].data(BodyText).toString().count()
                        < ViewableLimit))
        {
            msgIdList.append(indexList[i].data(ConvergedMsgId).toInt());
        }
    }
}

//---------------------------------------------------------------
// ConversationsEngine::markMessagesRead
// @see header
//---------------------------------------------------------------
void ConversationsEngine::markMessagesRead(QList<int>& msgIdList)
{
    RArray<TInt> idArray;
    int error;
    for (int i = 0; i < msgIdList.count(); ++i)
    {
        idArray.Append(msgIdList[i]);
    }
    if (msgIdList.count() > 0)
    {
        TRAP(error,d_ptr->markMessagesReadL(idArray));
    }
}

//---------------------------------------------------------------
// ConversationsEngine::getContactDetails
// @see header
//---------------------------------------------------------------
void ConversationsEngine::getContactDetails(qint64 conversationId,
                                            QString& firstName,
                                            QString& lastName)
{
    QModelIndexList
            indexList =
                    mConversationsSummaryModel->match(mConversationsSummaryModel->index(0,
                                                                                        0),
                                                      ConversationId,
                                                      conversationId,
                                                      1, // One match 
                                                      Qt::MatchExactly);

    if (indexList.count() > 0)
    {
        firstName = indexList[0].data(FirstName).toString();
        lastName = indexList[0].data(LastName).toString();
    }
}

//---------------------------------------------------------------
// ConversationsEngine::getConversationIdFromAddress
// @see header
//---------------------------------------------------------------
qint64 ConversationsEngine::getConversationIdFromAddress(QString address)
{
    QModelIndexList
            indexList =
                    mConversationsSummaryModel->match(mConversationsSummaryModel->index(0,
                                                                                        0),
                                                      ConversationAddress,
                                                      address,
                                                      1, // One match 
                                                      Qt::MatchExactly); //TODO phonebook matching logic to be taken care

    qint64 conversationId = -1;
    if (indexList.count() > 0)
    {
        conversationId = indexList[0].data(ConversationId).toLongLong();
    }
    return conversationId;
}

//---------------------------------------------------------------
// ConversationsEngine::getCurrentConversationId
// @see header
//---------------------------------------------------------------
qint64 ConversationsEngine::getCurrentConversationId()
{
    return d_ptr->getConversationCurrentId();
}

//---------------------------------------------------------------
// ConversationsEngine::getConversationIdFromContactId
// @see header
//---------------------------------------------------------------
qint64 ConversationsEngine::getConversationIdFromContactId(qint32 contactId)
{
    /*QModelIndexList indexList = mConversationsSummaryModel->match(
     mConversationsSummaryModel->index(0, 0), 
     ContactId, 
     contactId, 
     1, // One match 
     Qt::MatchExactly); //TODO 

     qint64 conversationId = -1;
     if(indexList.count() > 0)
     {    
     conversationId = indexList[0].data(ConversationId).toLongLong();
     }         
     return conversationId;*/

    qint64 conversationId = -1;
    int error;
    TRAP(error,conversationId =
            d_ptr->getConversationIdFromContactIdL(contactId));

    return conversationId;
}
