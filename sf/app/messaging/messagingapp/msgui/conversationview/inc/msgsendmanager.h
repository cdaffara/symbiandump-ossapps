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
 * Description:Message send functionality handler.
 *
 */

#ifndef MSG_SEND_MANAGER_H
#define MSG_SEND_MANAGER_H

#include <QObject>

#include "servicediscoveryaction.h"

class ConvergedMessage;
class MessageServiceAction;

/**
 * This class manages the message send functionality.
 * It loads service plugins and handles send functionality.
 */
class MsgSendManager : public QObject
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    MsgSendManager();

    /**
     * Destructor
     */
    ~MsgSendManager();
    /**
     * Send message
     * @param msg Message to be sent 
     * @param serviceId service id of the service
     */
    void send(const ConvergedMessage& msg, const QString serviceId);

    signals:
    /**
     * Signal emitted when an error is generated.
     * @param error The error as a string to be displayed on the screen.
     */

    void serviceError(const QString& error);

private:
    // List of service plugins 
    MessageServiceAction* msgAction;

};

#endif // MSG_SEND_MANAGER_H

// EOF
