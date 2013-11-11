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
 * Description: SMS data model plugin.
 *
 */

#ifndef UNISMSDATAPLUGIN_H_
#define UNISMSDATAPLUGIN_H_


#include <QObject>
#include "unidatamodelplugininterface.h"

/**
 * SMS data model plugin.
 * Implements the processing of a SMS message in Message Store.
 */

class UniSMSDataPluginPrivate;

class UniSMSDataPlugin : public QObject, public UniDataModelPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(UniDataModelPluginInterface)

public:
    /**
     * constructor
     */
    UniSMSDataPlugin(QObject* parent = 0);

    /**
     * Destructor
     */
    ~UniSMSDataPlugin();
    
    /**
     * Creates the new instance of the plugin
     * @return plugin instance
     */    
    QObject* createInstance();


    /**
     *  Reset the datamodel
     */
    void reset();

    /**
     * sets the message id for which details needs to be fetched
     * @param message id
     */
    int setMessageId(int mId);

    /**
     * Get the body of the message
     * @param aBodyText, for message body
     */
    void body(QString& aBodyText);

    /**
     * size of the message
     * @return size
     */
    int messageSize();

    /**
     * List of the message TO recipients
     * @param list of the recipients
     */
    void toRecipientList(ConvergedMessageAddressList& mAddressList);
    /**
     * List of the message cc recipients
     * @param list of the recipients
     */
    void ccRecipientList(ConvergedMessageAddressList& mAddressList);

    /**
     * List of the message bCC recipients
     * @param list of the recipients
     */
    void bccRecipientList(ConvergedMessageAddressList& mAddressList);

    /**
     * Address from which the message was received
     * @param messageAddress
     */
    void fromAddress(QString& messageAddress);

    /**
     * No. of attachments with the message
     * @return attachment count 
     */
    int attachmentCount();

    /**
     *  To check if the message has attachments
     *  @return true if attachment is present
     */
    bool hasAttachment();

    /**
     * list of attachments for a message 
     * @return attachemnt list
     */
    UniMessageInfoList attachmentList();

    /**
     *  Message type (SMS/MMS etc..)
     *  @return type of message 
     */
    QString messageType();

    /**
     * Priority of the message (high/low/medium)
     */
    MsgPriority messagePriority();

    /**
     * Number of objects asociated with a MMS message
     * @return count of objects
     */
    int objectCount();

    /**
     * The MMS object list
     * @return object list 
     */
    UniMessageInfoList objectList();

    /**
     * Number of slides associated with a MMS message
     * @return count of slides
     */
    int slideCount();

    /**
     * The MMS slide content 
     * @return slide list
     */
    UniMessageInfoList slideContent(int slidenum);

    /**
     * Time stamp of the message
     * @return time stamp of the created/received message
     */
    QDateTime timeStamp();

    /**
     * @see UniDataModelPluginInterface
     */
    void restore(CBaseMtm& mtm);

    /**
     * @see UniDataModelPluginInterface
     */
    QString subject();
    
	  /**
     * @see UniDataModelPluginInterface
     */
    CMsvSession* session();
	

private:
    UniSMSDataPluginPrivate* d_ptr;
};

#endif
