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
 * Description: Feeder Private class for unified viewer. Fetches data from the
 *              message store for the given message id.
 *
 */
#ifndef UNIVIEWER_FEEDER_PRIVATE_H
#define UNIVIEWER_FEEDER_PRIVATE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <msvapi.h>

#include "convergedmessage.h"
#include "unidatamodelplugininterface.h"

class QDateTime;
class UniViewerFeeder;
class CMsvSession;
class UniDataModelPluginInterface;
class UniDataModelLoader;

/**
 * UniViewerFeederPrivate
 * Feeder Private class for unified viewer. Fetches data from the
 * message store for the given message id.
 */
class UniViewerFeederPrivate
{
public:
    /**
     * Destructor.
     */
    ~UniViewerFeederPrivate();

    /**
     * Constructor
     * @param msgId message id.
     * @param feeder feeder object.
     */
    UniViewerFeederPrivate(qint32 msdId,
                           UniViewerFeeder* feeder);

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
     * Updates  the content by fetching new details
     * @param msgId message id
     * @param msgType message type
     */
    void updateContent(qint32 msgId);

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
     * Finds total recipient count
     * @return total recipient count
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
      */
    QString subject();
    
    /**
     * Returns the sending state of the message.
     */
    int sendingState();

private:
    /**
     * initialise the symbian constructions.
     */
    void initL(qint32 msgId);

private:
    /**
     * Ptr to QT class UniViewerFeeder.
     * Not Own.
     */
    UniViewerFeeder* q_ptr;

    /**
     * MessageType
     */
    int mMessageType;

    /**
     * Msv Entry
     */
    TMsvEntry mEntry;

    /**
     * Message Id
     */
    qint32 mMsgId;

    /**
     * Slide Count.
     */
    int mSlideCount;

    /**
     * interface to fetch data
     */
    UniDataModelPluginInterface* mPluginInterface;

    /**
     * plugin loader
     */
    UniDataModelLoader* mPluginLoader;
    /**
     * To address list
     * owned.
     */
    ConvergedMessageAddressList mToAddressList;

    /**
     * CC address list
     * owned.
     */
    ConvergedMessageAddressList mCcAddressList;

    /**
     * CC address list
     * owned.
     */
    ConvergedMessageAddressList mBccAddressList;

    /**
     * Msv Session.
     * not Owned.
     */
    CMsvSession* mSession;
      
};

#endif // UNIVIEWER_FEEDER_PRIVATE_H
// EOF
