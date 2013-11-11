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
#include "conversationsengine.h"

#include <ccsconversationentry.h>
#include <ccsclientconversation.h>

#include "conversationsenginedefines.h"
#include "convergedmessage.h"
#include "conversationmsgstorehandler.h"
#include "conversationssummarymodel.h"
#include "conversationsmodel.h"
#include "draftsmodel.h"
#include "conversationsengine_p.h"
#include "debugtraces.h"
#include <xqconversions.h>

//---------------------------------------------------------------
// ConversationsEngine::instance
// @see header
//---------------------------------------------------------------
ConversationsEngine* ConversationsEngine::instance()
{
    static ConversationsEngine* conversationsEngine =
        new ConversationsEngine();
    return conversationsEngine;
}

//---------------------------------------------------------------
// ConversationsEngine::ConversationsEngine
// @see header
//---------------------------------------------------------------
ConversationsEngine::ConversationsEngine(QObject* parent):
    QObject(parent), mDraftsModel(NULL)
{
    mConversationMsgStoreHandler = q_check_ptr(new ConversationMsgStoreHandler);
    
    mConversationsSummaryModel = new ConversationsSummaryModel(this);    
    mConversationsModel = new ConversationsModel(mConversationMsgStoreHandler,
        this);   

    d_ptr = q_check_ptr( new ConversationsEnginePrivate(mConversationMsgStoreHandler,
        mConversationsSummaryModel,
        mConversationsModel));
    
    connect (mConversationsModel,
            SIGNAL(conversationViewEmpty()),
            this,
            SIGNAL(conversationViewEmpty())); 
   
}

//---------------------------------------------------------------
// ConversationsEngine::~ConversationsEngine
// @see header
//---------------------------------------------------------------
ConversationsEngine::~ConversationsEngine()
{
    if(mConversationMsgStoreHandler)
    {
        delete mConversationMsgStoreHandler;
        mConversationMsgStoreHandler = NULL;
    }
    if(d_ptr)
    {
        delete d_ptr;
        d_ptr = NULL;
    }
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
// ConversationsEngine::getDraftsModel
// @see header
//---------------------------------------------------------------
QStandardItemModel* ConversationsEngine::getDraftsModel()
    {
    if(!mDraftsModel)
        {
        mDraftsModel = new DraftsModel(this);
        mConversationMsgStoreHandler->FetchDraftMessages(mDraftsModel);
        }
    return mDraftsModel;
    }
//---------------------------------------------------------------
// ConversationsEngine::getConversations
// @see header
//---------------------------------------------------------------
bool ConversationsEngine::getConversations(qint64 conversationId)
{
    int error;
    TRAP(error,d_ptr->getConversationsL(conversationId));
    return (error!=KErrNone) ? false : true; 
}

//---------------------------------------------------------------
// ConversationsEngine::clearConversations
// @see header
//---------------------------------------------------------------
bool ConversationsEngine::clearConversations()
{
    int error;
    TRAP(error,d_ptr->clearConversationsL());
    return (error!=KErrNone) ? false : true; 
}

//---------------------------------------------------------------
// ConversationsEngine::deleteConversations
// @see header
//---------------------------------------------------------------
bool ConversationsEngine::deleteConversations(qint64 conversationId)
{
    int error;
    TRAP(error,d_ptr->deleteConversationL(conversationId));
    return (error!=KErrNone) ? false : true; 
}

//---------------------------------------------------------------
// ConversationsEngine::deleteMessages
// @see header
//---------------------------------------------------------------
void ConversationsEngine::deleteMessages(QList<int>& msgIdList)
{
    RArray<TInt> idArray;
    for(int i=0; i < msgIdList.count(); ++i)
    {
        idArray.Append(msgIdList[i]);
    }
    if(msgIdList.count()>0)
    {
        d_ptr->deleteMessages(idArray);
    }
}

//---------------------------------------------------------------
// ConversationsEngine::deleteAllDraftMessages
// @see header
//---------------------------------------------------------------
void ConversationsEngine::deleteAllDraftMessages()
{
    int error;
    TRAP(error, d_ptr->deleteAllDraftMessagesL());
    qt_symbian_throwIfError(error);
}

//---------------------------------------------------------------
// ConversationsEngine::markConversationRead
// @see header
//---------------------------------------------------------------
bool ConversationsEngine::markConversationRead(qint64 conversationId)
{
    int error = 0;
    TRAP(error,d_ptr->markConversationReadL(conversationId));
    return (error!=KErrNone) ? false : true; 
}

//---------------------------------------------------------------
// ConversationsEngine::markMessagesRead
// @see header
//---------------------------------------------------------------
bool ConversationsEngine::markMessagesRead(QList<int>& msgIdList)
{
    RArray<TInt> idArray;
    int error;
    for(int i=0; i < msgIdList.count(); ++i)
    {
        idArray.Append(msgIdList[i]);
    }
    if(msgIdList.count()>0)
    {
        TRAP(error,d_ptr->markMessagesReadL(idArray));
    }
    return (error!=KErrNone) ? false : true; 
}

//---------------------------------------------------------------
// ConversationsEngine::getContactDetails
// @see header
//---------------------------------------------------------------
void ConversationsEngine::getContactDetails(qint64 conversationId,
    QString& displayName,
    QString& address)
{
    QModelIndexList indexList = mConversationsSummaryModel->match(
            mConversationsSummaryModel->index(0, 0), 
            ConversationId, 
            conversationId, 
            1, // One match 
            Qt::MatchExactly);

    if(indexList.count() > 0)
    {
        displayName = indexList[0].data(DisplayName).toString();
        address = indexList[0].data(ConversationAddress).toString();
    }
    else
    {
        int error;
        CCsClientConversation* clientConv = NULL;
        TRAP(error, clientConv = d_ptr->getConversationFromConversationIdL(conversationId));
       
        qt_symbian_throwIfError(error);
        
        HBufC *name = clientConv->GetDisplayName();
        if (name && name->Length())
        {
            displayName = XQConversions::s60DescToQString(*name);
        }

        HBufC *addr = clientConv->GetConversationEntry()->Contact();
        if (addr && addr->Length())
        {
            address = XQConversions::s60DescToQString(*addr);
        }

        delete clientConv;
    }
}

//---------------------------------------------------------------
// ConversationsEngine::getConversationIdFromAddress
// @see header
//---------------------------------------------------------------
qint64 ConversationsEngine::getConversationIdFromAddress(QString address)
{
    qint64 conversationId = -1;

    int error;
    HBufC* number = XQConversions::qStringToS60Desc(address);
    TRAP(error,conversationId = 
        d_ptr->getConversationIdFromAddressL(*number));

    delete number;
    
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
    qint64 conversationId = -1;
    int error;
    TRAP(error,conversationId = 
        d_ptr->getConversationIdFromContactIdL(contactId));
    
    return conversationId;    
}

//---------------------------------------------------------------
// ConversationsEngine::emitConversationModelPopulated
// @see header
//---------------------------------------------------------------
void ConversationsEngine::emitConversationModelPopulated()
{
    emit conversationModelPopulated();
}

//---------------------------------------------------------------
// ConversationsEngine::emitConversationModelUpdated
// @see header
//---------------------------------------------------------------
void ConversationsEngine::emitConversationModelUpdated()
{
    emit conversationModelUpdated();
}

//---------------------------------------------------------------
// ConversationsEngine::emitConversationListModelPopulated
// @see header
//---------------------------------------------------------------
void ConversationsEngine::emitConversationListModelPopulated()
{
    emit conversationListModelPopulated();
}

//---------------------------------------------------------------
// ConversationsEngine::emitConversationListModelEntryDeleted
// @see header
//---------------------------------------------------------------
void ConversationsEngine::emitConversationListModelEntryDeleted( int conversationId )
{
    emit conversationListEntryDeleted( conversationId );
}

//---------------------------------------------------------------
// ConversationsEngine::emitOpenConversationViewIdUpdate
// @see header
//---------------------------------------------------------------
void ConversationsEngine::emitOpenConversationViewIdUpdate(
        int newConversationId)
{
    //also register for subscription now
    QT_TRAP_THROWING(d_ptr->registerAgainForConversationUpdatesL(newConversationId));   
}

//---------------------------------------------------------------
// ConversationsEngine::disableRegisterationForCVEvents
// @see header
//---------------------------------------------------------------
void ConversationsEngine::disableRegisterationForCVEvents()
{
    d_ptr->deRegisterCVUpdatesTemporary();
}

//---------------------------------------------------------------
// ConversationsEngine::fetchMoreConversations
// @see header
//---------------------------------------------------------------
void ConversationsEngine::fetchMoreConversations()
{
    d_ptr->fetchMoreConversations();
}

//---------------------------------------------------------------
// ConversationsEngine::resendMessage()
// @see header
//---------------------------------------------------------------

bool ConversationsEngine::resendMessage(qint32 messageId)
{
    return d_ptr->resendMessage(messageId);
}

//---------------------------------------------------------------
// ConversationsEngine::downloadMessage()
// @see header
//---------------------------------------------------------------

int ConversationsEngine::downloadMessage(qint32 messageId)
{
    return d_ptr->downloadMessage(messageId);
}

//---------------------------------------------------------------
// ConversationsEngine::downloadOperationSupported()
// @see header
//---------------------------------------------------------------
bool ConversationsEngine::downloadOperationSupported(qint32 messageId)
{
    return d_ptr->downloadOperationSupported(messageId);
}

//---------------------------------------------------------------
// ConversationsEngine::markAsReadAndGetType()
// @see header
//---------------------------------------------------------------

void ConversationsEngine::markAsReadAndGetType(qint32 messageId,
                                               int& msgType,
                                               int& msgSubType)
{
mConversationMsgStoreHandler->markAsReadAndGetType(messageId,
                                                    msgType,
                                                    msgSubType);
}

//---------------------------------------------------------------
// ConversationsEngine::getDBHandle()
// @see header
//---------------------------------------------------------------
RSqlDatabase& ConversationsEngine::getDBHandle(TBool& isOpen)
    {
    return mConversationsModel->getDBHandle(isOpen);
    }


//---------------------------------------------------------------
// ConversationsEngine::getMsgSubType()
// @see header
//---------------------------------------------------------------
int ConversationsEngine::getMsgSubType(int messageId)
{
 return mConversationMsgStoreHandler->getMsgSubType(messageId);
}


//EOF

