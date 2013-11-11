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
 * Description: MMS data model plugin.
 *
 */

#ifndef UNIMMSDATAPLUGIN_H_
#define UNIMMSDATAPLUGIN_H_

#include <QObject>
#include "unidatamodelplugininterface.h"

class UniMMSDataPluginPrivate;

/**
 * MMS data model plugin.
 * Implements the processing of an MMS message in Message Store.
 */
class UniMMSDataPlugin : public QObject, public UniDataModelPluginInterface
{
Q_OBJECT 
Q_INTERFACES(UniDataModelPluginInterface)

public:
    /**
     * Constructor
     */
    UniMMSDataPlugin(QObject* parent = 0);

    /**
     * Destructor
     */
    ~UniMMSDataPlugin();
    
    /**
     * Creates the new instance of the plugin
     * @return plugin instance
     */    
    QObject* createInstance();

    /**
     * Sets the id of the message whose details needs to be extacted.
     * @param TMsvId of the message.
     */
    int setMessageId(int messageId);

    /** Reset the datamodel
     * Resets the data model to be used again
     */
    void reset();

    /**
     * Get the body of the message
     * @param aBodyText , a QString to hold the message body
     */
    void body(QString& aBodyText);

    /**
     * Size of the message
     * @return message size
     */
    int messageSize();

    /**
     * List of the message To recipients 
     * @param  mAddressList, list of all addresses in the message
     */
    void toRecipientList(ConvergedMessageAddressList& mAddressList);

    /**
     * List of the message CC recipients 
     * @param  mAddressList, list of all addresses in the message
     */
    void ccRecipientList(ConvergedMessageAddressList& mAddressList);

    /**
     * List of the message  bCC recipients 
     * @param  mAddressList, list of all addresses in the message
     */
    void bccRecipientList(ConvergedMessageAddressList& mAddressList);

    /**
     * The from address of a message
     * @param messageAddress
     */
    void fromAddress(QString& messageAddress);

    /**
     * List of attachments in the message.
     * @return the attachment list for the message.
     */
    UniMessageInfoList attachmentList();

    /**
     * Type of the message (SMS/MMS etc..)
     * @return message type
     */
    QString messageType();

    /**
     * Priority of the message (low/medium/high)
     * @return MsgPriority.
     */
    MsgPriority messagePriority();

    /**
     * Number of attachments with the message
     * @return count of attachment
     */
    int attachmentCount();

    /**
     * To check if the message has attachments
     * @return true/false
     */
    bool hasAttachment();

    /**
     * Number of objects asociated with a MMS message
     * @return object count
     */
    int objectCount();

    /**
     * The MMS object list asociated with a MMS message
     * @return object list 
     */
    UniMessageInfoList objectList();

    /**
     * Number of slides asociated with a MMS message
     * @return slide count
     */
    int slideCount();

    /**
     * The content of a Slide in case of a MMS message
     * @param list of messageinfo objects.
     */
    UniMessageInfoList slideContent(int slidenum);

    /**
     * Time stamp of the message when it was created/recieved.
     * @param mesage time
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
    UniMMSDataPluginPrivate* d_ptr;
};

#endif /* UNIMMSDATAPLUGIN_H_ */
