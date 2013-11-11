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
 * Description: Message Indicator class
 *
 */

#ifndef MSGINDICATOR_H
#define MSGINDICATOR_H

#include <QObject>
#include <QRunnable>
#include <hbindicatorinterface.h>

#include "msginfodefs.h"
class MsgIndicatorPrivate;
class QTranslator;

class ServiceRequestSenderTask : public QRunnable
{
public:
    /**
     * Constructor
     */
    ServiceRequestSenderTask(qint64 conversationId);
    
    /**
     * Destructor
     */
    ~ServiceRequestSenderTask();
     
    /**
     * create and send service request
     */
     void run();

private: 
     qint64 mConvId;
};

/**
 * Message indicator class. 
 * Handles client request and showing the indications. 
 */
class MsgIndicator : public HbIndicatorInterface
{
public:
    /**
     * Constructor
     */
    MsgIndicator(const QString &indicatorType);
    
    /**
     * Destructor
     */
    ~MsgIndicator();
    
    /**
     * @see HbIndicatorInterface
     */
    bool handleInteraction(InteractionType type);
    
    /**
     * @see HbIndicatorInterface
     */
    QVariant indicatorData(int role) const;

    
protected:
    /**
     * @see HbIndicatorInterface
     */
    bool handleClientRequest(RequestType type, const QVariant &parameter);
    
private: 
    /**
     * Prepares the display name from stream.
     * @param IindicatorData inidcator data.
     */
    QString prepareDisplayName(MsgInfo& indicatorData) const;
    
    /**
     * Gets the primary text for an indicator type
     * @param data MsgInfo the message information.
     * @return QString the primary text
     */
    QString getPrimaryText(MsgInfo& data);

    /**
     * Gets the primary text for unread messages
     * @param data MsgInfo the message information.
     */
    QString getUnreadPrimaryText(MsgInfo& data);
    
    /**
     * Get the secondary Text for messages.
     * @param type The type of the indicator
     * @return QString the secondary text
     */
    QString getSecondaryText(MsgInfo& info);

private:
    
    /**
     * Body text
     */
    mutable QString mBodyText;
    
    /**
     * Conversation id
     */
    mutable qint64 mConversationId;
    
    /**
     * Message type
     */
    mutable int mMessageType;

    /**
     * Message from single or multiple contact
     */
    mutable bool mConversationFromSingleContact;
    
    /**
     * Private implementaion object.
     * Owned.
     */
    MsgIndicatorPrivate* d_ptr;
   
    /**
     * Translator member variable
     * Owned
     */     
    QTranslator* mTranslator;
    
    /**
     * Translator member variable
     * Owned
     */     
    QTranslator* mTranslator_comm;
   
    /**
     * Primary Text
     * 
     */
    QString mPrimaryText;
    
    /**
     * Secondary Text
     */
    QString mSecondaryText;
    
    /**
     * Number of messages
     */    
    int mCount;
    
    /**
     * Indicator type
     */
    int mIndicatorType;
};

#endif // MSGINDICATOR_H

