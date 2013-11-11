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
 * Description: This is the domain header API for general message data model
 *              plugin behavior
 */

#ifndef UNIDATAMODELPLUGININTERFACE_H_
#define UNIDATAMODELPLUGININTERFACE_H_

#include <QtPlugin>
#include <msvstd.h>
#include <QDateTime>
#include <msvapi.h>
#include <cmsvattachment.h>
#include "convergedmessageaddress.h"

/**
 * Enum defining Message  priority
 * @attention This enum can have values from 0 to 2 only.
 */
enum MsgPriority
{
    Low = 0, Normal, High
};

/**
 * UniMessageInfo
 * Definition of Message's media objects
 */

class UniMessageInfo
{
public:
    /**
     * Constructor
     */
   inline  UniMessageInfo(const QString& filepath, const int size,
                   const QString& mimetype);

    /**
     * Destructor
     */
    inline ~UniMessageInfo();

    /**
     * setPath
     * @param file's path
     */
    inline void setPath(const QString& filepath);

    /**
     * setMimeType
     * @param file's mimetype
     */
   inline void setMimeType(const QString& mimetype);

    /**
     * path
     * @return file's path
     */
    inline const QString& path();

    /**
     * size
     * @return file's size
     */
    inline const int size();

    /**
     * mimetype
     * @return file's mimetype
     */
    inline const QString& mimetype();

    /**
     * isProtected
     * @return true if DRM protected object else false.
     */
    inline bool isProtected() const;

    /**
     * setProtected
     * @param isProtected true if DRM protected object else false.
     */
    inline void setProtected(bool isProtected);

    /**
     * isCorrupted
     * @return true if object is corrupted else false.
     */
    inline bool isCorrupted() const;

    /**
     * setCorrupted
     * @param corrupted true if object is corrupted else false.
     */
    inline void setCorrupted(bool corrupted);

    /**
     * Serialize the data memebers into the stream.
     * @param stream data stream to which data is serialized.
     */
    inline void serialize(QDataStream &stream) const;

    /**
     * Deserialize the stream to data members.
     * @param stream data stream from which data is deserialized.
     */
   inline void deserialize(QDataStream &stream);

private:
    QString mPath;
    QString mMimeType;
    int mSize;
    bool mProtected;
    bool mCorrupt;

};


typedef QList<UniMessageInfo*> UniMessageInfoList;

class UniDataModelPluginInterface
{
public:

    /**
     * Destructor
     */
    virtual ~UniDataModelPluginInterface()
    {
    }

    /**
     * Creates the new instance of the plugin
     * @return plugin instance
     */    
    virtual QObject* createInstance()=0;

    /**
     * Sets the id of the message whose details needs to be extacted.
     * @param TMsvId of the message.
     */

    virtual int setMessageId(int messageId)=0;

    /** Reset the datamodel
     * Resets the data model to be used again
     */
    virtual void reset() =0;

    /**
     * Get the body of the message
     * @param aBodyText , a QString to hold the message body
     */
    virtual void body(QString& aBodyText)=0;

    /**
     * Size of the message
     * @return message size
     */
    virtual int messageSize()=0;

    /**
     * List of the message To recipients 
     * @param  mAddressList, list of all addresses in the message
     */
    virtual void toRecipientList(ConvergedMessageAddressList& mAddressList)=0;

    /**
     * List of the message CC recipients 
     * @param  mAddressList, list of all addresses in the message
     */
    virtual void ccRecipientList(ConvergedMessageAddressList& mAddressList)=0;

    /**
     * List of the message  bCC recipients 
     * @param  mAddressList, list of all addresses in the message
     */
    virtual void bccRecipientList(ConvergedMessageAddressList& mAddressList)=0;

    /**
     * The from address of a message
     * @param messageAddress
     */
    virtual void fromAddress(QString& messageAddress)=0;

    /**
     * List of attachments in the message.
     * @return the attachment list for the message.
     */
    virtual UniMessageInfoList attachmentList()=0;

    /**
     * Type of the message (SMS/MMS etc..)
     * @return message type
     */
    virtual QString messageType()=0;

    /**
     * Priority of the message (low/medium/high)
     * @return MsgPriority.
     */
    virtual MsgPriority messagePriority()=0;

    /**
     * Number of attachments with the message
     * @return count of attachment
     */
    virtual int attachmentCount()=0;

    /**
     * To check if the message has attachments
     * @return true/false
     */

    virtual bool hasAttachment()=0;

    /**
     * Number of objects asociated with a MMS message
     * @return object count
     */
    virtual int objectCount()=0;

    /**
     * The MMS object list asociated with a MMS message
     * @return object list 
     */
    virtual UniMessageInfoList objectList()=0;

    /**
     * Number of slides asociated with a MMS message
     * @return slide count
     */
    virtual int slideCount()=0;

    /**
     * The content of a Slide in case of a MMS message
     * @param list of messageinfo objects.
     */
    virtual UniMessageInfoList slideContent(int count)=0;

    /**
     * Time stamp of the message when it was created/received.
     * @param message time
     */
    virtual QDateTime timeStamp()=0;

    /**
     *  Restores the model for editing purpose
     *  @param CBaseMtm
     */        
    virtual void restore(CBaseMtm& mtm)=0;
	
		/**
     * Subject associated with the message
     * @return QString
     */
		virtual QString subject()=0;
	
		/**
     * Session with the Messaging Server 
     * @return CMsvSession*
     */
		virtual CMsvSession* session() =0;
};

Q_DECLARE_INTERFACE(UniDataModelPluginInterface,
        "org.nokia.messaging.UniDataModelPluginInterface/1.0")

#include "unidatamodelplugininterface.inl"

#endif //UNIDATAMODELPLUGININTERFACE_H_
