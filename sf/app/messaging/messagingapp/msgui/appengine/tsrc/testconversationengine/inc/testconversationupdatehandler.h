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
 * Description:
 *
 */

#ifndef T_CONVERSATIONUPDATEHANDLER_H_
#define T_CONVERSATIONUPDATEHANDLER_H_

// INCLUDES
#include <QtTest/QtTest>


class TConversationUpdateHandler : public QObject
    {
    Q_OBJECT
public: 

    /**
     * Constructor
     */
    explicit TConversationUpdateHandler(QObject* parent = 0);    

    /**
     * Set the conversation view update handler 
     */
    void SetConversationsViewUpdateHandler();

    /**
     * Set the conversation summary view update handler 
     */
    void SetConversationsSummaryViewUpdateHandler();
   
private slots:

    /**
     * Validates Conversation view on updation 
     */
    void ConversationsViewUpdated();

    /**
     * Validates Conversation summary view on updation 
     */
    void ConversationsSummaryViewUpdated();
    
    };
#endif /* T_CONVERSATIONUPDATEHANDLER_H_ */
