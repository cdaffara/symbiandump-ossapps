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

#include "conversationssummarymodel.h"
#include "conversationsenginedefines.h"
#include "conversationsengineutility.h"
#include <xqconversions.h>
#include "convergedmessage.h"

#include <ccsclientconversation.h>
#include <ccsconversationentry.h>

//---------------------------------------------------------------
// ConversationsSummaryModel::ConversationsSummaryModel
// @see header
//---------------------------------------------------------------
ConversationsSummaryModel::ConversationsSummaryModel(QObject* parent)
:QStandardItemModel(parent)
    {
    }

//---------------------------------------------------------------
// ConversationsSummaryModel::~ConversationsSummaryModel
// @see header
//---------------------------------------------------------------
ConversationsSummaryModel::~ConversationsSummaryModel()
{
    
}

//---------------------------------------------------------------
// ConversationsSummaryModel::data
// @see header
//---------------------------------------------------------------
QVariant ConversationsSummaryModel::data(const QModelIndex & index , 
    int role ) const
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
        case ConvergedMsgId:
        {
            value = item->data(ConvergedMsgId);
            break;
        }
        case UnreadCount:
        {
            value = item->data(UnreadCount);
            break;
        }
        case TimeStamp:
        {
            value = item->data(TimeStamp);   
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
        case FirstName:
        {
            value = item->data(FirstName);
            break;
        }
        case LastName:
        {
            value = item->data(LastName);
            break;
        }
        case NickName:
        {
            value = item->data(NickName);
            break;
        }
        case Avatar:
        {
            value = item->data(Avatar);
            break;
        }
        case PresenceUri:             // Fall through start.
        case PresenceServiceName:
        case PresenceStatus:
        case PresenceStatusMsg: // Fall through end.
        default:
        {
            //No matching role found, set invalid variant.
            value = QVariant();
            break;
        }
    }
    return value;
}


//---------------------------------------------------------------
// ConversationsSummaryModel::addRow
// @see header
//---------------------------------------------------------------
void ConversationsSummaryModel::addRow(
        const CCsClientConversation& conversation)
    {  
    int convId = conversation.GetConversationEntryId();
    
    //match convId in model, if found update else add item
    QModelIndexList indexList = this->match(index(0, 0), 
            ConversationId, 
            convId, 1, Qt::MatchExactly);
    
    // if not found, add new item
    if ( indexList.count() == 0 )
        {    
        QStandardItem* item = new QStandardItem();         
        populateItem(*item,conversation);
        appendRow(item);
        }
    else        
        {
        // Update an existing item
        QModelIndex index = indexList[0];
        QStandardItem* item = this->item(index.row(), 0);
        populateItem(*item,conversation);
        }        
    }

//---------------------------------------------------------------
// ConversationsSummaryModel::deleteRow
// @see header
//---------------------------------------------------------------
void ConversationsSummaryModel::deleteRow(qint64 convId)
    {
    //match, if found remove item
    QModelIndexList indexList = this->match(index(0, 0), 
            ConversationId, 
            convId, 1, Qt::MatchExactly);

    if( indexList.count() == 1 )
        {
        QModelIndex index = indexList[0];
        this->removeRow(index.row());
        }        
    }

//---------------------------------------------------------------
// ConversationsSummaryModel::populateItem
// @see header
//---------------------------------------------------------------
void ConversationsSummaryModel::populateItem(QStandardItem& item, 
        const CCsClientConversation& conversation)
    {
    //get entry
    CCsConversationEntry* conEntry = conversation.GetConversationEntry();   
    
    // id 
    item.setData(conversation.GetConversationEntryId(), ConversationId);

    // message details
    item.setData(conEntry->EntryId(), ConvergedMsgId);
    item.setData(ConversationsEngineUtility::messageType(conEntry->GetType()), MessageType);
    
    // description
    HBufC* body = conEntry->Description();
    if( body && body->Length())
        {     
        QString bodytext(XQConversions::s60DescToQString(*body));
        item.setData(bodytext, BodyText); 
        item.setData(bodytext, Subject );
        }

    // time stamp
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    TTime timeStamp(conEntry->TimeStamp() );
    timeStamp.SecondsFrom(unixEpoch, seconds);       
    item.setData(seconds.Int(), TimeStamp); 

    // unread count 
    item.setData(conversation.GetUnreadMessageCount(), UnreadCount);

    // contact details
    HBufC* firstname = conversation.GetFirstName(); 
    QString displayName("");
    //first name
    if(firstname && firstname->Length())
        {
    displayName = XQConversions::s60DescToQString(*firstname);
    item.setData(displayName,FirstName); 
        }
    //last name
    HBufC* lastname = conversation.GetLastName();
    if( lastname &&  lastname->Length())
        {          
    item.setData(XQConversions::s60DescToQString(*lastname),LastName);
        }
    //nick name
    HBufC* nickname = conversation.GetNickName();
    if (nickname && nickname->Length())
    {
        item.setData(XQConversions::s60DescToQString(*nickname), NickName);
    }

    //item.setData(displayName, NickName);

    // contact number
    HBufC* contactno = conEntry->Contact();
    QString contactNumber("");
    if ( contactno && contactno->Length() )
        {
        contactNumber = XQConversions::s60DescToQString(*contactno);                  
        }        
    item.setData(contactNumber, ConversationAddress);

    //set the contactId
    int contactId = conversation.GetContactId();
    item.setData(contactId, ContactId);
    
    // unread status        
    item.setData(conEntry->IsAttributeSet(ECsAttributeUnread),UnReadStatus);      

    //presence 
    //TODO
    }

// EOF
