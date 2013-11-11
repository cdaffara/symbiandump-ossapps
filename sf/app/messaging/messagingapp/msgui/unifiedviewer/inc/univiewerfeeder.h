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
 * Description: Feeder class for unified viewer. Fetches data from the
 *              message store for the given message store id.
 *
 */
#ifndef UNIVIEWER_FEEDER_H
#define UNIVIEWER_FEEDER_H

#include <QObject>
#include <QDateTime>

#include "convergedmessage.h"
#include "unidatamodelplugininterface.h"

const qint32 KMtmSmsUidValue = 0x1000102C;

class UniViewerFeederPrivate;
/**
 * Feeder class for unified viewer. Fetches data from the
 * message store for the given message store id.
 */
class UniViewerFeeder : public QObject
{
Q_OBJECT

public:
    /**
     * Default Constructor.
     * @param msgId message id
     * @param parent parent object.
     */
    UniViewerFeeder(qint32 msgId, QObject *parent = 0);

    /**
     * Destructor.
     */
    ~UniViewerFeeder();

    /**
     * Updates  the content by fetching new details
     * @param msgId message id
     * @param msgType message type
     */
    void updateContent(qint32 msgId);

    /**
     * Emits msgBody signal with msgBody as argument.
     * Called from private class to emit signal.
     * @param  msgBody message body text.
     */
    void emitMsgBody(QString msgBody);

    /**
     * Fetches message details from the store.
     */
    void fetchDetails();

    /**
     * Returns the message type.
     * @return  message type.
     */
    qint32 msgType();

    /**
     * Returns the time stamp.
     * @return  time stamp of the message.
     */
    QDateTime timeStamp();

    /**
     * hasAttachments
     * @return true/false based on if message has attachments
     */
    bool hasAttachments();

    /**
     * attachmentList
     * @return list of current message's attachments, with size and mimetype info
     */
    UniMessageInfoList attachmentsList();

    /**
     * attachmentCount
     * @return number of attachments in the message
     */
    int attachmentCount();

    /**
     * objectsList
     * @return list of current message's objects, with size and mimetype info
     */
    UniMessageInfoList objectsList();

    /**
     * objectCount
     * @return count of media objects in SMIL presentation.
     * NOTE: Smil attachment is _not_ counted as object.
     */
    int objectCount();

    /**
     * slideCount
     * @return count of slides in SMIL presentation.
     */
    int slideCount();

    /**
     * slideContent
     * @return list of media objects in given slide
     */
    UniMessageInfoList slideContent(int slidenum);

    /**
     * toAddressList
     * @return list of to address list
     */
    ConvergedMessageAddressList toAddressList();

    /**
     * ccAddressList
     * @return list of cc address list
     */
    ConvergedMessageAddressList ccAddressList();

    /**
     * bccAddressList
     * @return list of Bcc address list
     */
    ConvergedMessageAddressList bccAddressList();

    /**
     * recipientCount
     * @return Total recipient count
     */
    int recipientCount();

    /**
     * messageSize
     * @return int 
     */
    int messageSize();

    /**
     * clearContent
     * @void
     */
    void clearContent();

    /**
     * get the from address and alias of a message
     * @param from address
     * @param alias
     * @void 
     */
    void fromAddressAndAlias(QString& from, QString& alias);
    
    /**
     * whether the message is incoming Or outgoing.
     * @return bool
     */
    bool isIncoming();
    
    /**
     * returns the priority of the message.
     * @return int
     */
    int priority();
    
    /**
     * gets the subject of the message
     * @return QString
     */
    QString subject();

    /**
     * if the send state of message is failed, then return true.
     * @return int The sending state 
     */
    int sendingState();
    
signals:
    /**
     * Signal emitted after fetching message body.
     * @param msgBody Message body content.
     */
    void msgBody(QString msgBody);

private:

    /**
     * Pointer to S60 class UniViewerFeederPrivate.
     * Own.
     */
    UniViewerFeederPrivate *d_ptr;

};

#endif // UNIVIEWER_FEEDER_H
// EOF
