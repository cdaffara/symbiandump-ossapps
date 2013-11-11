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
 * Description: Listen to conversation server events and parse the 
 * events. These parsed information passed to notifications and indications.
 *
 */

#ifndef MSGNOTIFIER_PRIVATE_H
#define MSGNOTIFIER_PRIVATE_H

#include <mcsconversationlistchangeobserver.h>
#include <QRunnable>

// CLASS DECLARATION
class MsgNotifier;
class CCSRequestHandler;
class MsgStoreHandler;
class XQSettingsManager;
class XQPublishAndSubscribeUtils;
class XQSystemToneService;
class CHWRMVibra;

/**
 * Structure to hold the details of a class 0 SMS
 */
struct Class0Info
{
    QString body;
    QString address;
    QString alias;
    QString time;
    int messageId;
};

/**
 * @class MsgNotifierPrivate
 */

class MsgNotifierPrivate : public CBase, public MCsConversationListChangeObserver
    {

public:
    /**
     * Constructor
     */
    MsgNotifierPrivate(MsgNotifier* msgNotifier);

    /**
     * Destructor
     */
    ~MsgNotifierPrivate();

public:
    /**  
     * AddConversationList
     * This is for handling new conversation event
     * asynchronously from the server 
     * @param aClientConversation CCsClientConversation The conversation object
     */
    void
            AddConversationList(
                    const CCsClientConversation& aClientConversation);

    /**  
     * DeleteConversationList
     * This is for handling delete conversation event
     * asynchronously from the server 
     * @param aClientConversation CCsClientConversation The conversation object
     */
    void DeleteConversationList(
            const CCsClientConversation& aClientConversation);
    
	/**
	 * PartialDeleteConversationList
	 * This is for handling partial delete of conversation event
	 * Asynchronous
	 * @param aClientConversation CCsClientConversation - The conversation object
	 */
    void PartialDeleteConversationList(
            const CCsClientConversation& aClientConversation);


    /**  
     * ModifyConversationList
     * This is for handling modify conversation asynchronously
     * from the server 
     * @param aClientConversation CCsClientConversation The conversation object
     * 
     */
    void ModifyConversationList(
            const CCsClientConversation& aClientConversation);

    /**
     * RefreshConversationList
     * This is for handling bulk changes. Message store change etc.
     */
    void RefreshConversationList();

public:

    /**
     * updateFailedIndications
     * Activate/Deactivate message indications based on pending message count
     */
    void updateOutboxIndications();

    /**
     * displayOutboxIndications
     * Display the outbox indications based on the data in MsgInfo
     */
    void displayOutboxIndications(MsgInfo data);

    /**
     * displayFailedNote
     * Popup a message box when a message is not successfully sent.
     * @param msgInfo MsgInfo The message information object
     */
    void displayFailedNote(MsgInfo msgInfo);

    /**
     * ShowClass0Message
     * Popups a Dialog with Class 0 SMS message...
     * @param msgInfo MsgInfo The message information object
     */
    void ShowClass0Message(Class0Info& aClass0Info);

private:
    /**
     * Does all the initializations. 
     */
    void initL();

    /**
     * Initializes the conversation store handler 
     */
    void initConvStoreHandler();

    /**
     * Process conversation entry for showing the notification.
     * @param aClientConversation conversation entry
     */
    void processListEntry(const CCsClientConversation& aClientConversation);

    /**
     * updateIndications
     * Activate/Deactivate message indications based on unread message count
     * @param bootup, true, if called on bootup else false
     */
    void updateUnreadIndications(bool bootup = false);
   
    /**
     * Show notification or not
     * @param receivedMsgConvId received message conversation id.
     * @return true if the received conversation id is not same as 
     * published conversation id ( opened conversation id) else false
     */
    bool showNotification(int receivedMsgConvId);
private:

    /**
     * Pointer to msgnotifier
     */
    MsgNotifier* q_ptr;

    /**
     * Conversation server client 
     */
    CCSRequestHandler* mCvServer;

    /**
     * Pointer to Conversation Msg Store Handler.
     */
    MsgStoreHandler* iMsgStoreHandler;
    
    /**
     * Settings manager 
     * Owned.
     */
    XQSettingsManager* mSettingsManager;

    /**
     * Publish and subscribe utils.
     * Owned.
     */
    XQPublishAndSubscribeUtils* mPSUtils;
    
    /**
     * Object to handle audio alert when new message is received
     */
    XQSystemToneService* mSts;
    
    /**
     * Vibra alert on receiving a new message
     */
    CHWRMVibra *mVibra; // Owned.
    };


/**
 * Class for launching the Class0 SMS dialog in a separate thread..
 */
class Class0SmsServiceTask : public QRunnable
{
public:
    /**
     * Constructor
     */
    Class0SmsServiceTask(Class0Info& class0info);
    
    /**
     * Destructor
     */
    ~Class0SmsServiceTask();
     
    /**
     * create and send service request
     */
     void run();

private: 
     Class0Info mClass0info;
};





#endif // MSGNOTIFIER_PRIVATE_H
//EOF
