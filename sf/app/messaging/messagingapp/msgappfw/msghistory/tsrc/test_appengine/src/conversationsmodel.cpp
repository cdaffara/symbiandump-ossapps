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

#include "conversationsmodel.h"
#include "conversationsenginedefines.h"
#include "conversationmsgstorehandler.h"
#include "convergedmessage.h"
#include <xqconversions.h>
#include "conversationsengineutility.h"
#include <ccsconversationentry.h>

//---------------------------------------------------------------
// ConversationsModel::ConversationsModel
// Constructor
//---------------------------------------------------------------
ConversationsModel::ConversationsModel(
    ConversationMsgStoreHandler* msgStoreHandler,
    QObject* parent): QStandardItemModel(parent),
    mMsgStoreHandler(msgStoreHandler)
            {
            }


//---------------------------------------------------------------
// ConversationsModel::~ConversationsModel
// Destructor 
//---------------------------------------------------------------
ConversationsModel::~ConversationsModel()
    {    
    }

//---------------------------------------------------------------
// ConversationsModel::data
// @see header
//---------------------------------------------------------------
QVariant ConversationsModel::data(const QModelIndex & index , int role ) const
{
    QVariant value;
    QStandardItem* item = itemFromIndex(index);
    switch(role)
    {
        case ConversationId:
        {
            value = item->data(ConversationId);
            break;
        }
        case UnReadStatus:
        {
            value = item->data(UnReadStatus);
            break;
        }
        case ContactId:
        {
            value = item->data(ContactId);
            break;
        }
        case TimeStamp:
        {
            value = item->data(TimeStamp);
            break;
        }
        case ConvergedMsgId:
        {
            value = item->data(ConvergedMsgId);
            break;
        }
        case MessageProperty:
        {
            value = item->data(MessageProperty);
            break;
        }
        case MessageType:
        {
            value = item->data(MessageType);
            break;
        }
        case Subject:
        {
            value = item->data(Subject);
            break;
        }
        case BodyText:
        {
            value = item->data(BodyText);
            break;
        }
        case ConversationAddress:
        {
            value = item->data(ConversationAddress);
            break;
        }
        case Direction:
        {
            value = item->data(Direction);
            break;
        }
        case SendingState:
        {
            value = item->data(SendingState);
            break;
        }
        case MessagePriority:
        {
            value = item->data(MessagePriority);
            break;
        }
        case Attachments:
        {
            value = item->data(Attachments);
            break;
        }
        case ServiceId:
        {
            value = item->data(ServiceId);
            break;
        }
        case SessionId:
        {
            value = item->data(SessionId);
            break;
        }
        case SubsessionId:
        {
            value = item->data(SubsessionId);
            break;
        }
        case MessageLocation:
        {
            value = item->data(MessageLocation);
            break;
        }
        case MessageStore:
        {
            value = item->data(MessageStore);
            break;
        }
        case ConversationAlias:
        {
            value = item->data(ConversationAlias);
            break;
        }
        case UnreadCount:
        {

            value = item->data(UnreadCount);
            break;
        }
        case FirstName: // Fall through start
            value = item->data(FirstName);
            break;
        case LastName:
            value = item->data(LastName);
            break;
        case NickName:
        case Avatar:    // Fall througn end
        {
            break;
        }
        case PresenceUri:        // Fall through start
        case PresenceServiceName:
        case PresenceStatus:
        case PresenceStatusMsg: // Fall through end
        {
            break;
        }
        default:
        {
                //No matching role found, set invalid variant
                value = QVariant();
                break;
        }
      }
        return value;
}

//---------------------------------------------------------------
// ConversationsModel::addRow
// @see header
//---------------------------------------------------------------
void ConversationsModel::addRow(
        const CCsConversationEntry& entry)
    {
    int msgId = entry.EntryId();	
    //match, if found update else add item
    QModelIndexList indexList = this->match(index(0, 0), 
            ConvergedMsgId, 
            msgId, -1, Qt::MatchExactly);

    // if not found, add new item
    if ( indexList.count() == 0 )
        {    
        QStandardItem* item = new QStandardItem();
        populateItem(*item,entry);
        appendRow(item);
        }
    else        
        {
        // Update an existing item
        QModelIndex index = indexList[0];
        QStandardItem* item = this->item(index.row(), 0);
        populateItem(*item,entry);
        }        
    }

//---------------------------------------------------------------
// ConversationsModel::deleteRow
// @see header
//---------------------------------------------------------------
void ConversationsModel::deleteRow(int msgId)
    {
    //match, if found remove item
    QModelIndexList indexList = this->match(index(0, 0), 
            ConvergedMsgId, 
            msgId, 1, Qt::MatchExactly);

    if( indexList.count() == 1 )
        {
        QModelIndex index = indexList[0];
        this->removeRow(index.row());
        }        
    }


//---------------------------------------------------------------
// ConversationsModel::populateItem
// @see header
//---------------------------------------------------------------
void ConversationsModel::populateItem(QStandardItem& item, 
        const CCsConversationEntry& entry)
    {
    int msgId = entry.EntryId();		
    // id             
    item.setData(msgId, ConvergedMsgId);

    // description
    HBufC* description = entry.Description();
    QString subject("");
    if( description && description->Length())
        {
        subject = (XQConversions::s60DescToQString(*description));
        item.setData(subject, Subject); 
        }
    
    // time stamp 
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    TTime timeStamp(entry.TimeStamp() );
    timeStamp.SecondsFrom(unixEpoch, seconds);
    item.setData(seconds.Int(), TimeStamp);

    //contact details
    HBufC* contact = entry.Contact();
    if( contact && contact->Length())
        {
        item.setData(XQConversions::s60DescToQString(*contact), ConversationAddress);      
        }

    // message type.        
    item.setData(ConversationsEngineUtility::messageType(entry.GetType()), MessageType);
    // unread status
    if(entry.IsAttributeSet(ECsAttributeUnread))
    {    
    item.setData(true,UnReadStatus);
    }
    else
    {
    item.setData(false,UnReadStatus);    
    }
    
    //sending state
    item.setData(entry.GetSendState(), SendingState);        
    // direction
    item.setData(entry.ConversationDir(), Direction);        

    //location
    if(entry.ConversationDir() == ECsDirectionIncoming)
    {
        item.setData(ConvergedMessage::Inbox, MessageLocation);   
    }
    else if(entry.IsAttributeSet(ECsAttributeDraft))
    {
        item.setData(ConvergedMessage::Draft, MessageLocation);   
    }
    else if(entry.IsAttributeSet(ECsAttributeSent))
    {
        item.setData(ConvergedMessage::Sent, MessageLocation);  
    }
    else
    {
        item.setData(ConvergedMessage::Outbox, MessageLocation);  
    }
    
    //message specific handling
    TCsType msgType = entry.GetType();
    if(msgType == ECsMMS)
    {    
        //retrieve bodytext and attachment info of mms mesages
        CNativeMmsUtility* mmsHandlerUtil = CNativeMmsUtility::NewL(entry.EntryId(),
            mMsgStoreHandler->GetMsvSession());
        CleanupStack::PushL(mmsHandlerUtil);

        //bodyText
        QString bodytext;
        mmsHandlerUtil->getBodyTextL(bodytext);
        item.setData(bodytext, BodyText);

        //get attachemnt paths and set to item
        ConvergedMessageAttachmentList attachmentlist;
        mmsHandlerUtil->getAttachmentListL(attachmentlist);
        QStringList attachmentPaths;
        for (int a = 0; a < attachmentlist.count(); ++a)
        {
            attachmentPaths << attachmentlist.at(a)->filePath();
        }
        //populate item  with the attachment list
        item.setData(attachmentPaths.join("|"),Attachments);
        if(entry.IsAttributeSet(ECsAttributeAttachment))
        {    
        item.setData(ConvergedMessage::Attachment,MessageProperty);
        }
        //priority
        if(entry.IsAttributeSet(ECsAttributeHighPriority))
        {    
            item.setData(ConvergedMessage::High,MessagePriority);
        }
        else if(entry.IsAttributeSet(ECsAttributeLowPriority))
        {
            item.setData(ConvergedMessage::Low,MessagePriority);    
        }
        //cleanup
        CleanupStack::PopAndDestroy(mmsHandlerUtil);
    }
    else
    {
        // sms bodytext                   
        item.setData(subject, BodyText); 
    }

    //presence 
    //TODO
    }

//EOF
