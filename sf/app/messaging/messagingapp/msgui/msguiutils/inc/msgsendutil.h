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
 * Description:Message send utility.
 *
 */

#ifndef MSG_SEND_UTIL_H
#define MSG_SEND_UTIL_H

#include <msvstd.h>
#include <QObject>
#include "convergedmessage.h"

#ifdef MSGUIUTILS_UNIT_TEST
 #define MSGUI_UTILS_DLL_EXPORT
#else
#ifdef BUILD_MSGUI_UTILS_DLL
 #define MSGUI_UTILS_DLL_EXPORT Q_DECL_EXPORT
#else
 #define MSGUI_UTILS_DLL_EXPORT Q_DECL_IMPORT
#endif
#endif


class UniEditorPluginLoader;
class UniEditorGenUtils;
class UniEditorPluginInterface;

/**
 * This class manages the message send functionality.
 * 
 */
class MSGUI_UTILS_DLL_EXPORT MsgSendUtil : public QObject
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    MsgSendUtil(QObject* parent=0);

    /**
     * Destructor
     */
    ~MsgSendUtil();

    /**
     * Save message to drafts
     * @param msg Message to be sent 
     * @return true if message is saved to drafts
     */
    TMsvId saveToDrafts(ConvergedMessage& msg);

    /**
     * Send message
     * @param msg Message to be sent 
     * @return KErrNone if send is successful, 
     *         KErrNotFound if settings are not available,
     *         KErrGeneral for other cases
     */
    int send(ConvergedMessage& msg);

    /**
     * Checks for valid phone number and email address.
     * @param address Address to be checked.
     * @return true if valid address else false.
     */
    bool isValidAddress(const QString &address);

private:
    /**
     * Check if email over sms is supported and change msgtype appropriately.
     * @param msg Message to be sent
     * @param emailOverSmsSupported OUT true if email over sms is supported
     * @return true if ok to send message
     */
    bool checkEmailOverSms(ConvergedMessage& msg, bool& emailOverSmsSupported);
    
    /**
     * Check max recipient count and change msg type appropriately.
     * @param msg Message to be sent
     * @return true if ok to send message
     */
    bool checkMaxRecipientCount(ConvergedMessage& msg);
    
    /**
     * Check max message size limit and change msg type appropriately.
     * @param msg Message to be sent
     * @return true if ok to send message
     */
    bool checkMaxMsgSizeLimit(ConvergedMessage& msg);
    
    /**
     * Calculate sms message size
     * @param msg Message to be sent
     * @return size of the message
     */
    int getSmsMsgSize(ConvergedMessage& msg);
    
    /**
     * Get the longest email address's size
     * @return size
     */
    int longestEmailAddressSize(ConvergedMessageAddressList addrList);

    /**
     * Calculate mms message size
     * @param msg Message to be sent
     * @return size of the message
     */
    int getMmsMsgSize(ConvergedMessage& msg);
    
    /**
     * Validate service and check for the send settings
     * @param pluginInterface plugin to be used for msg-send
     * @param emailOverSmsSupported true if email over sms is supported
     * @return true if ok to send message
     */
    bool validateService(UniEditorPluginInterface* pluginInterface,
                         bool emailOverSmsSupported);

private:
    
    /**
     * Sms/Mms send plugin loader
     * Own
     */
    UniEditorPluginLoader* mPluginLoader;
    
    /**
     * General utils for send-time checks
     * Own
     */
    UniEditorGenUtils* mUniEditorGenUtils;

#ifdef MSGUIUTILS_UNIT_TEST
    /**
     * Unit Testing
     */
    friend class TestMsgSendUtil;
#endif    
};

#endif // MSG_SEND_UTIL_H

// EOF
