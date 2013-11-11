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
 * Description:Conversation Base view.
 *
 */

#ifndef MSG_CONVERSATION_VIEW_INTERFACE_H
#define MSG_CONVERSATION_VIEW_INTERFACE_H

#ifdef MSGUI_UNIT_TEST
 #define CONVERSATION_VIEW_EXPORT
#else
 #ifdef  CONVERSATIONVIEW_DLL
  #define CONVERSATION_VIEW_EXPORT Q_DECL_EXPORT
 #else
  #define CONVERSATION_VIEW_EXPORT Q_DECL_IMPORT
 #endif
#endif

#include <xqsettingskey.h>
#include "msgbaseview.h"

// FORWARD DECLARATIONS
class MsgConversationView;
class HbListWidgetItem;
class MsgContactCardWidget;
class QGraphicsLinearLayout;
class XQSettingsManager;
class XQPublishAndSubscribeUtils;

class CONVERSATION_VIEW_EXPORT MsgConversationBaseView: public MsgBaseView
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    MsgConversationBaseView(QGraphicsItem* parent = 0);

    /**
     * Destructor
     */
    ~MsgConversationBaseView();

public:
    /**
     * openConversationView
     * Launches conversation view
     * @param entryId, the conversation index to be open
     *
     */
    void openConversation(qint64 entryId);

    /**
     * clearContent
     * Clear conversation view content
     */
    void clearContent();

    /**
     * saveContentToDrafts
     * Saves the editors content in cv to drafts
     * @return valid message id if save is success
     * else invalid message id ( i.e. -1 )
     */
    int saveContentToDrafts();

    /**
     * conversationId
     * Get the conversationId
     */
    qint64 conversationId();

    /**
     * Event handler
     * @param key Key code
     * @return true if key event handled else false.
     * @see MsgBaseView
     */
    bool handleKeyEvent(int key);

public slots:
    /**
     * Close the conversation view 
     * This slot is triggered when user presses back(seconday sofykey)
     */
    void closeConversationView();

    /**
     * Mark messages as read
     * Slot is triiggered when replying started
     */
    void markMessagesAsRead();

    /**
     * Publish the conversation id based on the flag.
     * @param setId if true previous conversation id 
     * will be published else -1 will be published.
     */
    void setPSCVId(bool setId);

private slots:
    /**
     * Slot for delayed construction
     */
    void doDelayedConstruction();

    /**
     * Hides chrome when itu-t is up
     */
    void hideChrome(bool hide);

    /**
     * Slot for handling valid returns from the framework.
     * 
     * @param result const QVariant&
     */
    void handleOk(const QVariant& result);

    /**
     * Slot for handling errors. Error ids are provided as 
     * 32-bit integers.
     * @param errorCode qint32
     */
    void handleError(int errorCode, const QString& errorMessage);

    /**
     * Slot for handling the timer expiry event fired from view reay indication
     */
    void handleViewReady();

    /**
     *
     */
    void handleConversationIdChange(qint64 convId);

signals:
    /**
     * Signal emitted when the conversation view is closed.
     */
    void conversationViewClosed();

private:
    /**
     * Create and initialise the view
     */
    void initView();

private:

    /**
     * Conversation ID
     */
    qint64 mConversationId;

    /**
     * MsgConversationView, main view class
     * Owned
     */
    MsgConversationView *mConversationView;

    /**
     * Contact card widget.
     */
    MsgContactCardWidget* mContactCard;

    /**
     * Main layout.
     */
    QGraphicsLinearLayout* mMainLayout;

    /**
     * Object of XQSettingsKey. 
     */
    XQSettingsKey mCVIdkey;

    /**
     * mSettingsManager
     * Instance of the XQSettingsManager
     * Own.
     */
    XQSettingsManager* mSettingsManager;
};

#endif // MSG_CONVERSATION_VIEW_INTERFACE_H
// EOF
