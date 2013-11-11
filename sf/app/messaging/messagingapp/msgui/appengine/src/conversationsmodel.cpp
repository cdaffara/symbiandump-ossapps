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
#include "unidatamodelloader.h"
#include "unidatamodelplugininterface.h"
#include "ringbc.h"
#include "msgcontacthandler.h"
#include "mmsconformancecheck.h"
#include <ccsconversationentry.h>
#include <fileprotectionresolver.h>

#include "debugtraces.h"

#include <QFile>
#include <QFileInfo>
#include <s32mem.h>
#include <s32strm.h>
#include <fbs.h>
#include <ccsdefs.h>

//CONSTANTS
_LIT(KDbFileName, "c:[2002A542]conversations.db");

// preview sql query
_LIT(KSelectConvMsgsStmt, "SELECT message_id, msg_processingstate, subject, body_text, preview_path, msg_property, preview_icon FROM conversation_messages WHERE message_id=:message_id ");

//selecet preview-icon query
_LIT(KSelectPreviewIconStmt,"SELECT  message_id, preview_icon FROM conversation_messages WHERE message_id = :message_id ");

//selecet vcard-path query
_LIT(KSelectVCardStmt,"SELECT  message_id, msg_processingstate, preview_path FROM conversation_messages WHERE message_id = :message_id ");

// preview-cache max cost (items)
const int CACHE_COST =  50;
//Preview thumbnail size
const int KWidth = 24 * 6.7;
const int KHeight = 24 * 6.7;
//---------------------------------------------------------------
// ConversationsModel::ConversationsModel
// Constructor
//---------------------------------------------------------------
ConversationsModel::ConversationsModel(ConversationMsgStoreHandler* msgStoreHandler,
    QObject* parent) :
    QStandardItemModel(parent), mMsgStoreHandler(msgStoreHandler), iSqlDbOpen(EFalse)
{
    //Open SQL DB
    if (KErrNone == iSqlDb.Open(KDbFileName))
    {
        iSqlDbOpen = ETrue;
    }
    previewIconCache.setMaxCost(CACHE_COST);

    int err = connect(this, SIGNAL(retrievePreviewIcon(int, QString&)), this,
        SLOT(updatePreviewIcon(int, QString&)));
    QCRITICAL_WRITE_FORMAT("Error from connect()", err)
    iDataModelPluginLoader = new UniDataModelLoader;
    iMmsDataPlugin = iDataModelPluginLoader->getDataModelPlugin(ConvergedMessage::Mms);
    iBioMsgPlugin = iDataModelPluginLoader->getDataModelPlugin(ConvergedMessage::BioMsg);
}

//---------------------------------------------------------------
// ConversationsModel::~ConversationsModel
// Destructor
//---------------------------------------------------------------
ConversationsModel::~ConversationsModel()
{
    //Close SQL-DB
    iSqlDb.Close();

	//clear preview-cache
    previewIconCache.clear();

    if (iDataModelPluginLoader) {
        delete iDataModelPluginLoader;
        iDataModelPluginLoader = NULL;
    }
}

//---------------------------------------------------------------
// ConversationsModel::data
// @see header
//---------------------------------------------------------------
QVariant ConversationsModel::data(const QModelIndex & index, int role) const
{
    QVariant value;
    QStandardItem* item = itemFromIndex(index);
    switch (role) {
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
    case PreviewIcon:
    {
        QString filepath(item->data(Attachments).toString());
        int msgId = item->data(ConvergedMsgId).toInt();
        HbIcon *icon = getPreviewIconItem(msgId, filepath);
        return *icon;
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
    case DisplayName: // Fall through start
        value = item->data(DisplayName);
        break;
    case Avatar: // Fall througn end
        value = item->data(Avatar);
        break;
    case NotificationStatus:
        value = item->data(NotificationStatus);
        break;
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
void ConversationsModel::addRow(const CCsConversationEntry& entry, bool dynamicUpdate)
{
    int msgId = entry.EntryId();
    //match, if found update else add item
    QModelIndexList indexList = this->match(index(0, 0), ConvergedMsgId, msgId, -1,
        Qt::MatchExactly);

    // if not found, add new item
    if (indexList.count() == 0) {
        QStandardItem* item = new QStandardItem();
        populateItem(*item, entry);
        if (!dynamicUpdate) {
            insertRow(0, item);
        }
        else {
            int i;
            for (i = rowCount() - 1; i >= 0; --i) {
                QStandardItem* modelItem = this->item(i, 0);
                if (modelItem->data(ConvergedMsgId).toInt() < item->data(ConvergedMsgId).toInt()) {
                    if (i == rowCount() - 1) {
                        appendRow(item);
                    }
                    else {
                        insertRow(i + 1, item);
                    }
                    return;
                }
            }
            if (i == 0) {
                insertRow(0, item);
            }
        }
    }
    else {
        // Update an existing item
        QModelIndex index = indexList[0];
        QStandardItem* item = this->item(index.row(), 0);
        populateItem(*item, entry);
    }
}

//---------------------------------------------------------------
// ConversationsModel::deleteRow
// @see header
//---------------------------------------------------------------
void ConversationsModel::deleteRow(int msgId)
{
    //match, if found remove item
    QModelIndexList indexList =
        this->match(index(0, 0), ConvergedMsgId, msgId, 1, Qt::MatchExactly);

    if (indexList.count() == 1) {
        QModelIndex index = indexList[0];
        this->removeRow(index.row());
    }
}

//---------------------------------------------------------------
// ConversationsModel::populateItem
// @see header
//---------------------------------------------------------------
void ConversationsModel::populateItem(QStandardItem& item, const CCsConversationEntry& entry)
{
    QCRITICAL_WRITE("ConversationsModel::populateItem start.");

    int msgId = entry.EntryId();
    // id
    item.setData(msgId, ConvergedMsgId);

    // description
    HBufC* description = entry.Description();
    QString subject("");
    if (description && description->Length()) {
        subject = (XQConversions::s60DescToQString(*description));     
    }

    // time stamp
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    TTime timeStamp(entry.TimeStamp());
    timeStamp.SecondsFrom(unixEpoch, seconds);
    item.setData(seconds.Int(), TimeStamp);

    //contact details
    HBufC* contact = entry.Contact();
    if (contact && contact->Length()) {
        item.setData(XQConversions::s60DescToQString(*contact), ConversationAddress);
    }

    // message type.
    int msgType = ConversationsEngineUtility::messageType(entry.GetType());
    item.setData(msgType, MessageType);

    //message sub-type
    item.setData(ConversationsEngineUtility::messageSubType(entry.GetType()), MessageSubType);

    // unread status
    if (entry.IsAttributeSet(ECsAttributeUnread)) {
        item.setData(true, UnReadStatus);
    }
    else {
        item.setData(false, UnReadStatus);
    }

    //sending state
    item.setData(entry.GetSendState(), SendingState);
    // direction
    item.setData(entry.ConversationDir(), Direction);

    //location
    if (entry.ConversationDir() == ECsDirectionIncoming) {
        item.setData(ConvergedMessage::Inbox, MessageLocation);
    }
    else if (entry.IsAttributeSet(ECsAttributeDraft)) {
        item.setData(ConvergedMessage::Draft, MessageLocation);
    }
    else if (entry.IsAttributeSet(ECsAttributeSent)) {
        item.setData(ConvergedMessage::Sent, MessageLocation);
    }
    else {
        item.setData(ConvergedMessage::Outbox, MessageLocation);
    }

    //message specific handling
    if (msgType == ConvergedMessage::Mms) {
        QCRITICAL_WRITE("ConversationsModel::populateItem  MMS start.")
        handleMMS(item, entry);
        QCRITICAL_WRITE("ConversationsModel::populateItem MMS end.")
    }
    else if(msgType == ConvergedMessage::MmsNotification) {
        item.setData(subject, Subject);
        handleMMSNotification(item, entry);
    }
    else if (msgType == ConvergedMessage::BT) {
        handleBlueToothMessages(item, entry);
    }
    else if (msgType == ConvergedMessage::BioMsg) {
        handleBioMessages(item, entry);
    }
    else {
        // sms bodytext
        item.setData(subject, BodyText);
    }

    QCRITICAL_WRITE("ConversationsModel::populateItem end.");
}

//---------------------------------------------------------------
// ConversationsModel::validateMsgForForward
// @see header file
//---------------------------------------------------------------
bool ConversationsModel::validateMsgForForward(qint32 messageId)
{
    bool retValue = true;
    //Validate if the mms msg can be forwarded or not
    MmsConformanceCheck* mmsConformanceCheck = new MmsConformanceCheck;
    retValue = mmsConformanceCheck->validateMsgForForward(messageId);

    delete mmsConformanceCheck;
    return retValue;
}


//---------------------------------------------------------------
// ConversationsModel::handleMMS
// @see header
//---------------------------------------------------------------
void ConversationsModel::handleMMS(QStandardItem& item, const CCsConversationEntry& entry)
{
    //msg_id
    int msgId = entry.EntryId();

    bool isEntryInDb = false;
    TInt err = KErrNone;

    //check if db is open and query db
    if (iSqlDbOpen)
    {
        RSqlStatement sqlSelectStmt;
        err = sqlSelectStmt.Prepare(iSqlDb, KSelectConvMsgsStmt);

        // move to fallback option
        if (KErrNone == err)
            {
            TInt msgIdIndex = sqlSelectStmt.ParameterIndex(_L(":message_id"));
            TInt msgProcessingStateIndex = sqlSelectStmt.ColumnIndex(_L("msg_processingstate"));
            TInt subjectIndex = sqlSelectStmt.ColumnIndex(_L("subject"));
            TInt bodyIndex = sqlSelectStmt.ColumnIndex(_L("body_text"));
            TInt previewPathIndex = sqlSelectStmt.ColumnIndex(
                    _L("preview_path"));
            TInt msgpropertyIndex = sqlSelectStmt.ColumnIndex(
                _L("msg_property"));
            TInt previewIconIndex = sqlSelectStmt.ColumnIndex(
                _L("preview_icon"));

            err = sqlSelectStmt.BindInt(msgIdIndex, msgId);

            // populate item
            if ((KErrNone == err) && (sqlSelectStmt.Next() == KSqlAtRow))
                {
                int msgProcessingState = 0;
                msgProcessingState = sqlSelectStmt.ColumnInt(
                        msgProcessingStateIndex);
                if (msgProcessingState == EPreviewMsgProcessed)
                    {
                    // use entry to populate model only when,
                    // entry is present in DB and its processing is over.
                    RBuf subjectBuffer;
                    if( subjectBuffer.Create(
                            sqlSelectStmt.ColumnSize(
                                    subjectIndex)) == KErrNone)
                    {
                        sqlSelectStmt.ColumnText(subjectIndex, subjectBuffer);
                        item.setData(
                                XQConversions::s60DescToQString(
                                        subjectBuffer), Subject);
                        subjectBuffer.Close();
                    }
                    
                    RBuf bodyBuffer;
                    if (bodyBuffer.Create(
                            sqlSelectStmt.ColumnSize(
                                    bodyIndex)) == KErrNone)
                    {
                       sqlSelectStmt.ColumnText(bodyIndex, bodyBuffer);
                       item.setData(
                               XQConversions::s60DescToQString(
                                       bodyBuffer), BodyText);
                       bodyBuffer.Close();
                    }

                    RBuf previewPathBuffer;
                    QString attachmentPath;
                    if (previewPathBuffer.Create(
                            sqlSelectStmt.ColumnSize(
                                    previewPathIndex)) == KErrNone)
                    {
                        sqlSelectStmt.ColumnText(
                                previewPathIndex,
                                previewPathBuffer);

                        //Rightnow set inside attachments
                        attachmentPath = XQConversions::s60DescToQString(
                                previewPathBuffer);
                        item.setData(attachmentPath, Attachments);
                        previewPathBuffer.Close();
                    }
                    
                    int msgProperty = 0;
                    msgProperty = sqlSelectStmt.ColumnInt(msgpropertyIndex);
                    item.setData(msgProperty, MessageProperty);

                    RSqlColumnReadStream stream;
                    //Get data from binary column BLOB
                    TInt err = stream.ColumnBinary(sqlSelectStmt,
                            previewIconIndex);

                    QCRITICAL_WRITE_FORMAT("Error from ColumnBinary()", err)

                    if (err == KErrNone)
                        {
                        CFbsBitmap *bitmap = new CFbsBitmap;
                        TRAPD(err,bitmap->InternalizeL(stream));
                        QCRITICAL_WRITE_FORMAT("Error from bitmap InternalizeL()", err)

                        //convert bitmap to pixmap
                        if (err == KErrNone)
                            {
                            TSize size = bitmap->SizeInPixels();
                            int bytesPerLine = bitmap->ScanLineLength(
                                    size.iWidth, bitmap->DisplayMode());
                            const uchar* dataPtr =
                                    (const uchar*) bitmap->DataAddress();

                            QPixmap pixmap = QPixmap::fromImage(QImage(
                                    dataPtr, size.iWidth, size.iHeight,
                                    bytesPerLine, QImage::Format_RGB16));

                            setPreviewIcon(pixmap, attachmentPath, msgId,
                                    true);

                            }
                        else
                            {
                            QPixmap pixmap;
                            setPreviewIcon(pixmap, attachmentPath, msgId,
                                    false);
                            }
                        //remove bitmap
                        delete bitmap;
                        }

                    //set flag to disable fallback option
                    isEntryInDb = true;
                    }
                }
            }
        sqlSelectStmt.Close();
        }

    //fallback option incase of db operation failure or enry not found in DB
    //populate from data plugins
    if (!isEntryInDb || err != KErrNone)
    {
        int error = iMmsDataPlugin->setMessageId(entry.EntryId());
        if(error != KErrNone)
        {
            // skip all
            return;
        }
        int msgProperty = 0;

        if (iMmsDataPlugin->attachmentCount() > 0)
        {
            msgProperty |= EPreviewAttachment;
        }

        if(validateMsgForForward(entry.EntryId()))
        {
            msgProperty |= EPreviewForward;
        }

        //subject
        item.setData(iMmsDataPlugin->subject(), Subject);

        int slideCount = iMmsDataPlugin->slideCount();
        bool isBodyTextSet = false;
        bool isAudioSet = false;
        bool isImageSet = false;
        bool isVideoSet = false;
        QString textContent;
        QString videoPath;
        QString imagePath;

        for (int i = 0; i < slideCount; ++i)
        {
            UniMessageInfoList objectList = iMmsDataPlugin->slideContent(i);
            for (int index = 0; index < objectList.count(); ++index)
            {
                if (!isBodyTextSet && objectList[index]->mimetype().contains(
                    "text"))
                {
                    QFile file(objectList[index]->path());
                    file.open(QIODevice::ReadOnly);
                    QByteArray textArray;
                    textArray = file.readAll();
                    char *data = new char[textArray.size()+1];
                    strcpy(data,textArray.data());
                    //This is needed since MMS text content 
                    //is stored in UTF8 format
                    textContent = textContent.fromUtf8(data,strlen(data));
                    delete []data;
                    item.setData(textContent, BodyText);
                    isBodyTextSet = true;
                    file.close();
                }
                if (!isVideoSet && !isImageSet && objectList[index]->mimetype().contains(
                    "image"))
                {
                    isImageSet = true;
                    msgProperty |= EPreviewImage;
                    if (objectList[index]->isProtected())
                    {
                        msgProperty |= EPreviewProtectedImage;
                    }
                    if (objectList[index]->isCorrupted())
                    {
                        msgProperty |= EPreviewCorruptedImage;
                    }
                    imagePath = objectList[index]->path();
                }
                if (!isVideoSet && !isAudioSet && objectList[index]->mimetype().contains(
                    "audio"))
                {
                    msgProperty |= EPreviewAudio;
                    if (objectList[index]->isProtected())
                    {
                        msgProperty |= EPreviewProtectedAudio;
                    }
                    if (objectList[index]->isCorrupted())
                    {
                        msgProperty |= EPreviewCorruptedAudio;
                    }
                    isAudioSet = true;
                }
                if (!( isImageSet || isAudioSet) && !isVideoSet && objectList[index]->mimetype().contains(
                    "video"))
                {
                    isVideoSet = true;
                    msgProperty |= EPreviewVideo;
                    if (objectList[index]->isProtected())
                    {
                        msgProperty |= EPreviewProtectedVideo;
                    }
                    if (objectList[index]->isCorrupted())
                    {
                        msgProperty |= EPreviewCorruptedVideo;
                    }
                    videoPath = objectList[index]->path();
                }
            }
            foreach(UniMessageInfo* slide,objectList)
                {
                    delete slide;
                }
        }
        QPixmap pixmap;
        //populate item  with the attachment list
        //TODO: This code is not required bcoz video icon is show and not preview  
        if (isVideoSet)
        {
            item.setData(videoPath, Attachments);
            // Store thumbnail only for non protected, non corrupted content.
            if (!(EPreviewProtectedVideo & msgProperty) &&
                !(EPreviewCorruptedVideo & msgProperty))
            {
                setPreviewIcon(pixmap, videoPath, msgId, false);
            }
        }
        else if (isImageSet)
        {
            item.setData(imagePath, Attachments);
            // Store thumbnail only for non protected, non corrupted content.
            if (!(EPreviewProtectedImage & msgProperty) &&
                !(EPreviewCorruptedImage & msgProperty))
            {
                setPreviewIcon(pixmap, imagePath, msgId, false);
            }
        }
        //populate msgProperty
        item.setData(msgProperty, MessageProperty);
    }

    // fill other attributes
    if (entry.IsAttributeSet(ECsAttributeHighPriority))
    {
        item.setData(ConvergedMessage::High, MessagePriority);
    }
    else if (entry.IsAttributeSet(ECsAttributeLowPriority))
    {
        item.setData(ConvergedMessage::Low, MessagePriority);
    }
}

//---------------------------------------------------------------
// ConversationsModel::handleMMSNotification
// @see header
//---------------------------------------------------------------
void ConversationsModel::handleMMSNotification(QStandardItem& item,
    const CCsConversationEntry& entry)
{
    // set context to current entry
    TRAPD(err, mMsgStoreHandler->setNotificationMessageIdL(entry.EntryId()));
    if(err != KErrNone)
    {
        return;
    }

    // fetch relevent info to show in CV
    // notification state e.g. waiting, retrieving etc
    QString statusStr;
    int status;
    mMsgStoreHandler->NotificationStatus(status, statusStr);

    // create data for bodytext role
    QString dataText;
    dataText.append(mMsgStoreHandler->NotificationMsgSize());
    dataText.append(QChar::LineSeparator);
    dataText.append(mMsgStoreHandler->NotificationClass());
    dataText.append(QChar::LineSeparator);
    dataText.append(mMsgStoreHandler->NotificationExpiryDate());
    if(!statusStr.isEmpty())
    {
        dataText.append(QChar::LineSeparator);
        dataText.append(statusStr);
    }

    // set fetched data to roles
    item.setData(status, NotificationStatus);
    item.setData(dataText, BodyText);

    if (entry.IsAttributeSet(ECsAttributeHighPriority)) {
        item.setData(ConvergedMessage::High, MessagePriority);
    }
    else if (entry.IsAttributeSet(ECsAttributeLowPriority)) {
        item.setData(ConvergedMessage::Low, MessagePriority);
    }
}

//---------------------------------------------------------------
// ConversationsModel::handleBlueToothMessages
// @see header
//---------------------------------------------------------------
void ConversationsModel::handleBlueToothMessages(QStandardItem& item,
    const CCsConversationEntry& entry)
    {
    int msgSubType = ConversationsEngineUtility::messageSubType(
            entry.GetType());
    
    if (msgSubType == ConvergedMessage::VCard)
        {
        handleVCard(item, entry.EntryId());
        }
    else
        {
        QString description = XQConversions::s60DescToQString(
                *(entry.Description()));

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
// ConversationsModel::handleBioMessages
// @see header
//---------------------------------------------------------------
void ConversationsModel::handleBioMessages(QStandardItem& item, const CCsConversationEntry& entry)
{
    int msgSubType = ConversationsEngineUtility::messageSubType(entry.GetType());
    if (ConvergedMessage::VCard == msgSubType)
        {
        handleVCard(item, entry.EntryId());
        }
    else if (ConvergedMessage::VCal == msgSubType) {
        //not supported
    }
    else if (ConvergedMessage::RingingTone == msgSubType) {
        iBioMsgPlugin->setMessageId(entry.EntryId());
        if (iBioMsgPlugin->attachmentCount() > 0) {
            UniMessageInfoList attList = iBioMsgPlugin->attachmentList();
            QString attachmentPath = attList[0]->path();

            //get tone title, and set as bodytext
            RingBc ringBc;
            item.setData(ringBc.toneTitle(attachmentPath), BodyText);
            while (!attList.isEmpty()) {
                delete attList.takeFirst();
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
}

//---------------------------------------------------------------
// ConversationsModel::getDBHandle()
// @see header
//---------------------------------------------------------------
RSqlDatabase& ConversationsModel::getDBHandle(TBool& isOpen)
{
    isOpen = iSqlDbOpen;
    return iSqlDb;
}

//---------------------------------------------------------------
// ConversationsModel::setPreviewIcon()
// @see header
//---------------------------------------------------------------
void ConversationsModel::setPreviewIcon(QPixmap& pixmap, QString& filePath,
    int msgId, bool inDb)
{

    //Since the population happens in reverse this check is needed so that
    //most recent items have their icons present in cache
    if (previewIconCache.totalCost() >= previewIconCache.maxCost())
        return;

    // if not found in db, set from file path
    if (!inDb)
    {
        QPixmap pixmap(filePath);
        QSize originalIconSize = pixmap.size();
        QSize scaledIconSize;
        getScaledSize(originalIconSize,scaledIconSize);

        QPixmap scaledPixmap = pixmap.scaled(scaledIconSize.width(), 
                scaledIconSize.height(), Qt::KeepAspectRatio);
        HbIcon *previewIcon = new HbIcon(scaledPixmap);

        previewIconCache.insert(msgId, previewIcon);

    }
    else
    {
        HbIcon *previewIcon = new HbIcon(pixmap);
        previewIconCache.insert(msgId, previewIcon);
    }
}

//---------------------------------------------------------------
// ConversationsModel::getPreviewIconItem()
// @see header
//---------------------------------------------------------------
HbIcon* ConversationsModel::getPreviewIconItem(int msgId,
    QString& filepath) const
{
    QCRITICAL_WRITE("ConversationsModel::getPreviewIconItem start.")

    //Initialize icon from the Cache will be NULL if Item not present
    HbIcon* previewIcon = previewIconCache[msgId];
    if (!previewIcon)
    {
        //This is done in this way as non-const function call cant be done here
        emit retrievePreviewIcon(msgId, filepath);

        previewIcon = previewIconCache[msgId];
    }

    QCRITICAL_WRITE("ConversationsModel::getPreviewIconItem start.")

    return previewIcon;
}

//---------------------------------------------------------------
// ConversationsModel::updatePreviewIcon()
// @see header
//---------------------------------------------------------------
void ConversationsModel::updatePreviewIcon(int msgId, QString& filePath)
{
    QCRITICAL_WRITE("ConversationsModel::updatePreviewIcon start.")

    //sql query to get preview-icon from DB
    bool imagePreviewed = false;
    QPixmap pixmap;

    if (iSqlDbOpen)
    {
        RSqlStatement sqlSelectPreviewIconStmt;
        TInt err = sqlSelectPreviewIconStmt.Prepare(iSqlDb,
            KSelectPreviewIconStmt);

        QCRITICAL_WRITE_FORMAT("Error from Prepare()", err)

        if (err == KErrNone)
        {
            //msg_id
            TInt msgIdIndex = sqlSelectPreviewIconStmt.ParameterIndex(
                _L(":message_id"));
            sqlSelectPreviewIconStmt.BindInt(msgIdIndex, msgId);

            // get preview-icon from DB
            err = sqlSelectPreviewIconStmt.Next();
            QCRITICAL_WRITE_FORMAT("Error from Next()", err)

            if (err == KSqlAtRow)
            {
                TInt previewIconIndex = sqlSelectPreviewIconStmt.ColumnIndex(
                    _L("preview_icon"));

                RSqlColumnReadStream stream;

                //Get data from binary column BLOB
                err = stream.ColumnBinary(sqlSelectPreviewIconStmt,
                    previewIconIndex);

                QCRITICAL_WRITE_FORMAT("Error from ColumnBinary()", err)

                if (err == KErrNone)
                {
                    CFbsBitmap *bitmap = new CFbsBitmap;
                    TRAPD(err,bitmap->InternalizeL(stream));
                    QCRITICAL_WRITE_FORMAT("Error from bitmap InternalizeL()", err)

                    //convert bitmap to pixmap
                    if (err == KErrNone)
                    {
                        TSize size = bitmap->SizeInPixels();
                        int bytesPerLine = bitmap->ScanLineLength(size.iWidth,
                            bitmap->DisplayMode());
                        const uchar* dataPtr =
                                (const uchar*) bitmap->DataAddress();

                        pixmap = QPixmap::fromImage(QImage(dataPtr,
                            size.iWidth, size.iHeight, bytesPerLine,
                            QImage::Format_RGB16));

                        imagePreviewed = true;

                        QCRITICAL_WRITE("Bitmap Conversion completed")
                    }
                    //remove bitmap
                    delete bitmap;
                }
                //close stream
                stream.Close();
            }
        }
        sqlSelectPreviewIconStmt.Close();
    }

    // if not found in db, set from file path
    if (!imagePreviewed)
    {
        QPixmap orgPixmap(filePath);
        QSize originalIconSize = pixmap.size();
        QSize scaledIconSize;
        getScaledSize(originalIconSize,scaledIconSize);

        QPixmap scaledPixmap = orgPixmap.scaled(scaledIconSize.width(), 
                scaledIconSize.height(), Qt::KeepAspectRatio);
    }
    HbIcon * previewIcon = new HbIcon(pixmap);

    previewIconCache.insert(msgId, previewIcon);

    QCRITICAL_WRITE("ConversationsModel::updatePreviewIcon end.")

}

//---------------------------------------------------------------
// ConversationsModel::handleVCard()
// @see header
//---------------------------------------------------------------
void ConversationsModel::handleVCard(QStandardItem& item, int msgId)
    {
    //sql query to get vcard-path from DB
    bool vCardParsed = false;

    if (iSqlDbOpen)
        {
        RSqlStatement sqlSelectVcardStmt;
        TInt err = sqlSelectVcardStmt.Prepare(iSqlDb, KSelectVCardStmt);

        QCRITICAL_WRITE_FORMAT("Error from Prepare()", err)

        if (err == KErrNone)
            {
            //msg_id
            TInt msgIdIndex = sqlSelectVcardStmt.ParameterIndex(
                    _L(":message_id"));
            sqlSelectVcardStmt.BindInt(msgIdIndex, msgId);
            // state
            TInt msgProcessingStateIndex = sqlSelectVcardStmt.ColumnIndex(
                    _L("msg_processingstate"));
           
            // get vacrd-path from DB
            err = sqlSelectVcardStmt.Next();
            QCRITICAL_WRITE_FORMAT("Error from Next()", err)

            if (err == KSqlAtRow)
                {
                int msgProcessingState = 0;
                msgProcessingState = sqlSelectVcardStmt.ColumnInt(
                        msgProcessingStateIndex);
                if (msgProcessingState == EPreviewMsgProcessed)
                    {
                    //path-index
                    TInt previewPathIndex = sqlSelectVcardStmt.ColumnIndex(
                            _L("preview_path"));

                    //Get vcard-path data from path-index
                    RSqlColumnReadStream stream;
                    err = stream.ColumnBinary(sqlSelectVcardStmt,
                            previewPathIndex);

                    QCRITICAL_WRITE_FORMAT("Error from ColumnBinary()", err)

                    if (err == KErrNone)
                    {
                        RBuf vCardPathBuffer;
                        if (vCardPathBuffer.Create(
                                sqlSelectVcardStmt.ColumnSize(
                                previewPathIndex)) == KErrNone)
                        {
                            sqlSelectVcardStmt.ColumnText(
                                    previewPathIndex,
                                    vCardPathBuffer);

                            //set inside attachments
                            QString attachmentPath(
                                    XQConversions::s60DescToQString(
                                            vCardPathBuffer));
                            item.setData(attachmentPath, Attachments);

                            //get display-name and set as bodytext
                            QString displayName =
                                    MsgContactHandler::getVCardDisplayName(
                                            attachmentPath);
                            item.setData(displayName, BodyText);

                            vCardPathBuffer.Close();
                            vCardParsed = true;
                            
                            QCRITICAL_WRITE("vcard parsing complete.")
                        }
                    }
                    //close stream
                    stream.Close();
                    }
                }
            }
        sqlSelectVcardStmt.Close();
        }

    // fallback, if not parsed in DB, parse from store
    if (!vCardParsed)
        {
        iBioMsgPlugin->setMessageId(msgId);

        if (iBioMsgPlugin->attachmentCount() > 0)
            {
            UniMessageInfoList attList = iBioMsgPlugin->attachmentList();
            QString attachmentPath = attList[0]->path();

            //get display-name and set as bodytext
            QString displayName = MsgContactHandler::getVCardDisplayName(
                    attachmentPath);
            item.setData(displayName, BodyText);
            item.setData(attachmentPath, Attachments);

            // clear attachement list : its allocated at data model
            while (!attList.isEmpty())
                {
                delete attList.takeFirst();
                }
            }
        }
    }

//---------------------------------------------------------------
// ConversationsModel::clearModel()
// @see header
//---------------------------------------------------------------
void ConversationsModel::clearModel()
{
    clear();
    previewIconCache.clear();
}

//---------------------------------------------------------------
// ConversationsModel::emitConversationViewEmpty()
// @see header
//---------------------------------------------------------------
void ConversationsModel:: emitConversationViewEmpty()
{
    emit conversationViewEmpty();
}

//---------------------------------------------------------------
// ConversationsModel::getScaledSize()
// @see header
//---------------------------------------------------------------
void ConversationsModel::getScaledSize(const QSize &originalSize,
        QSize &scaledSize)
{
    qreal newLength =0;
    if(originalSize.width() >= originalSize.height())
        {
        if(originalSize.width() < KWidth)
            {
            scaledSize.setHeight(originalSize.height());
            scaledSize.setWidth(originalSize.width());
            }
        else
            {
            scaledSize.setWidth(KWidth);
            newLength = (KWidth * originalSize.height())/
                    originalSize.width();
            scaledSize.setHeight(newLength);
            }
        }
    else
        {
        if(originalSize.height() < KHeight)
            {
            scaledSize.setHeight(originalSize.height());
            scaledSize.setWidth(originalSize.width());
            }
        else
            {
            scaledSize.setHeight(KHeight);
            newLength = (KHeight * originalSize.width())/
                    originalSize.height();
            scaledSize.setWidth(newLength);
            }            
        }
}

//EOF
