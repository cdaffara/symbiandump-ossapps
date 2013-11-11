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
* Description:  Conversation view interface used for launching 
 *                conversation view.
 *
*/


#ifndef MSG_CONVERSATION_VIEW_INTERFACE_H
#define MSG_CONVERSATION_VIEW_INTERFACE_H

#include "msgconversationviewdefines.h"

// FORWARD DECLARATIONS
class HbMainWindow;
class MsgConversationView;
class HbView;

/**
 * Interface for launching the Conversation View.
 * The conversation view then
 * shall be used to show the conversations or used for editing message.
 * 
 * MsgConversationViewInterface emits 1 signal
 * 
 * conversationViewClosed() is emitted when the view is closed.
 * The view is closed , when the back key is pressed.
 * i.e. secondary softkey.
 */
class CONVERSATION_VIEW_EXPORT MsgConversationViewInterface : public QObject
    {
    Q_OBJECT

public:

    /**
     * Constructor
     */
    MsgConversationViewInterface(HbMainWindow* parent=0);

    /**
     * Destructor
     */
    ~MsgConversationViewInterface();

public:
    /**
     * openConversationView
     * Launches conversation view
     * @param entryId, the conversation index to be open
     *
     */
    void openConversationView(qint64 entryId);

    /**
     * openConversationView
     * Launches conversation view in send mode with pre-populated data.
     * @param dataArray, the data array from which conversation view is
     * populated.
     */
    void openConversationView(QByteArray dataArray);
    
    /**
     * openConversationView
     * Launches conversation view in send mode with pre-populated data.
     * @param number specifies phone number string
     * @param name specifies number alias name
     */
    void openConversationView(QString number, QString name);
    
public slots:   
    /**
     * Close the conversation view 
     * This slot is triggered when user presses back(seconday sofykey)
     */
    void closeConversationView();
 
 signals:
     /**
      * Signal emitted when the conversation view is closed.
      */
    void conversationViewClosed(); 
    
private :
    /**
     * Create and initialise the view
     */
    void initView();
    
private:
    /**
     * Reference of HbMainWindow, main window class
     * Not Owned
     */
    HbMainWindow *mMainWindow;
    
    /**
     * MsgConversationView, main view class
     * Owned
     */
    MsgConversationView *mConversationView;

    /**
     * Container widget holding conversation view &
     * header widget
     * Owned
     */
    HbView *mMainWidget;

    };

#endif // MSG_CONVERSATION_VIEW_INTERFACE_H
