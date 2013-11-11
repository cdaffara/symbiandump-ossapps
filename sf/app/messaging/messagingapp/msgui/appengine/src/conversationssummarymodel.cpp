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
#include "unidatamodelloader.h"
#include "unidatamodelplugininterface.h"
#include "ringbc.h"
#include "msgcontacthandler.h"
#include "debugtraces.h"

#include <ccsclientconversation.h>
#include <ccsconversationentry.h>
#include <QFile>
#include <QFileInfo>

//---------------------------------------------------------------
// ConversationsSummaryModel::ConversationsSummaryModel
// @see header
//---------------------------------------------------------------
ConversationsSummaryModel::ConversationsSummaryModel(QObject* parent)
:QStandardItemModel(parent)
    {
    QStandardItemModel::setSortRole(TimeStamp);
    QStandardItemModel::sort(0, Qt::DescendingOrder);
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
        case SendingState:
        {
            value = item->data(SendingState);
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
        case MessageSubType:
        {
            value = item->data(MessageSubType);
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
        case DisplayName:
        {
            value = item->data(DisplayName);
            break;
        }
        case Avatar:
        {
            value = item->data(Avatar);
            break;
        }
        case Direction:
        {
            value = item->data(Direction);
            break;
        }
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
        const CCsClientConversation& conversation, bool caching)
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
    // no need to sort if it is initial caching, as sorting is already done
    if (!caching)
        {
        QStandardItemModel::sort(0, Qt::DescendingOrder);
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
    QCRITICAL_WRITE("ConversationsSummaryModel::populateItem start.");
            
    //get entry
    CCsConversationEntry* conEntry = conversation.GetConversationEntry(); 
    //error scenario
    if(conEntry == NULL)
        return;
    
    // message details
    // conversation-id 
    item.setData(conversation.GetConversationEntryId(), ConversationId);
    // message-id
    item.setData(conEntry->EntryId(), ConvergedMsgId);	
    // send status
    item.setData(conEntry->GetSendState(),SendingState);
    // direction
    item.setData(conEntry->ConversationDir(), Direction);
    //msg-type
    int msgType = ConversationsEngineUtility::messageType(conEntry->GetType());
    item.setData(msgType, MessageType);
    //message sub-type   
    item.setData(ConversationsEngineUtility::messageSubType(conEntry->GetType()), MessageSubType);

    //handle BT messages, needs to be revisited again, once vcal/vcard over BT issue is fixed
    if(ConvergedMessage::BT == msgType)
    {
        handleBlueToothMessages(item, *conEntry);
    }
    else if(msgType == ConvergedMessage::BioMsg)
    {
        handleBioMessages(item, *conEntry);
    }
    else
    {       
        // description
        HBufC* body = conEntry->Description();
        if( body && body->Length())
        {     
            QString bodytext(XQConversions::s60DescToQString(*body));
            item.setData(bodytext, BodyText); 
            item.setData(bodytext, Subject );
        }
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
    HBufC* disName = conversation.GetDisplayName(); 
    QString displayName("");
    //display name
    if(disName && disName->Length())
        {
        displayName = XQConversions::s60DescToQString(*disName);
        item.setData(displayName,DisplayName); 
        }
    
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
    
    QCRITICAL_WRITE("ConversationsSummaryModel::populateItem start.");
    }


//---------------------------------------------------------------
// ConversationsModel::handleBlueToothMessages
// @see header
//---------------------------------------------------------------
void ConversationsSummaryModel::handleBlueToothMessages(QStandardItem& item,
    const CCsConversationEntry& entry)
{
    int msgSubType = ConversationsEngineUtility::messageSubType(entry.GetType());
    if (msgSubType == ConvergedMessage::VCard) 
    {
        UniDataModelLoader* pluginLoader = new UniDataModelLoader;
        UniDataModelPluginInterface* bioMsgPlugin = pluginLoader->getDataModelPlugin(
            ConvergedMessage::BioMsg);
        bioMsgPlugin->setMessageId(entry.EntryId());

        if (bioMsgPlugin->attachmentCount() > 0) 
        	{
            UniMessageInfoList attList = bioMsgPlugin->attachmentList();
            QString attachmentPath = attList[0]->path();

            //get display-name and set as bodytext
            QString displayName = MsgContactHandler::getVCardDisplayName(attachmentPath);
            item.setData(displayName, BodyText);
		
						// clear attachement list : its allocated at data model
            while (!attList.isEmpty()) 
						{
                delete attList.takeFirst();
            }

        }
        delete pluginLoader;
    }

    else 
    {
        QString description = XQConversions::s60DescToQString(*(entry.Description()));

        if (msgSubType == ConvergedMessage::VCal) // "vCalendar"
        {
            //message sub-type
            item.setData(ConvergedMessage::VCal, MessageSubType);
        }
        else 
				{
            //message sub-type
            item.setData(ConvergedMessage::None, MessageSubType);
        }
        //for BT messages we show filenames for all other (except vcard) messages
        //get filename and set as body
        QFileInfo fileinfo(description);
        QString filename = fileinfo.fileName();
        item.setData(filename, BodyText);
    }
}

//---------------------------------------------------------------
// ConversationsSummaryModel::handleBioMessages
// @see header
//---------------------------------------------------------------
void ConversationsSummaryModel::handleBioMessages(QStandardItem& item, const CCsConversationEntry& entry)
{
    UniDataModelLoader* pluginLoader = new UniDataModelLoader;
    UniDataModelPluginInterface* bioMsgPlugin = pluginLoader->getDataModelPlugin(ConvergedMessage::BioMsg);
    bioMsgPlugin->setMessageId(entry.EntryId());
    
    int msgSubType = ConversationsEngineUtility::messageSubType(entry.GetType());
    if (ConvergedMessage::VCard == msgSubType) {
        if (bioMsgPlugin->attachmentCount() > 0) {
            UniMessageInfoList attList = bioMsgPlugin->attachmentList();
            QString attachmentPath = attList[0]->path();

            //get display-name and set as bodytext
            QString displayName = MsgContactHandler::getVCardDisplayName(
                    attachmentPath);
            item.setData(displayName, BodyText);

            // clear attachement list : its allocated at data model
            while (!attList.isEmpty()) {
                delete attList.takeFirst();
            }
        }
    }
    else if (ConvergedMessage::VCal == msgSubType) {
        //not supported
    }
    else if (ConvergedMessage::RingingTone == msgSubType) {
        if (bioMsgPlugin->attachmentCount() > 0) {
            UniMessageInfoList attList = bioMsgPlugin->attachmentList();
            QString attachmentPath = attList[0]->path();
            	
            //get tone title, and set as bodytext
            RingBc ringBc;
            item.setData(ringBc.toneTitle(attachmentPath), BodyText);
            while (!attList.isEmpty()) {
                delete attList.takeFirst();
            }
        }
    }

   else if(ConvergedMessage::NokiaService == msgSubType){
        // This is a bio message so lets parse it and see if it has a associated attachment ..        
        if (bioMsgPlugin->attachmentCount() > 0) {
				// TODO : need to confirm if we need to read from attachment
        }
        else {// description
            HBufC* description = entry.Description();
            QString subject("");
            if (description && description->Length()) {
                subject = (XQConversions::s60DescToQString(*description));
                item.setData(subject, BodyText);
            }

        }

    }

   else {
        // description
        HBufC* description = entry.Description();
        QString subject("");
        if (description && description->Length()) {
            subject = (XQConversions::s60DescToQString(*description));
            item.setData(subject, BodyText);
        }
    }
    
    delete pluginLoader;
}
// EOF
