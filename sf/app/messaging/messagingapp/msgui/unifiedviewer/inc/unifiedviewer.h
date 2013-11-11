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
 * Description: Main view of unified viewer
 *
 */

#ifndef UNIFIED_VIEWER_H
#define UNIFIED_VIEWER_H

#ifdef MSGUI_UNIT_TEST
 #define UNI_VIEWER_DLL
#else
 #ifdef BUILD_UNI_VIEWER_DLL
  #define UNI_VIEWER_DLL Q_DECL_EXPORT
 #else
  #define UNI_VIEWER_DLL Q_DECL_IMPORT
 #endif
#endif

#include "msgbaseview.h"
#include "convergedmessage.h"

class UniViewerFeeder;
class ConvergedMessage;
class HbScrollArea;
class UniScrollArea;
class UniContentsWidget;
class MessageDeleteAction;
class HbAction;
/**
 * Main view of unified viewer.
 */
class UNI_VIEWER_DLL UnifiedViewer : public MsgBaseView
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    UnifiedViewer(const qint32 messageId,
                  QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~UnifiedViewer();

    /**
     * Populates the content on the widget
     */
    void populateContent(const qint32 messageId, bool update, int msgCount, qint64 conversationId = -1);

    /**
     * Event handler
     * @param key Key code
     * @return true if key event handled else false.
     * @see MsgBaseView
     */
    bool handleKeyEvent(int key);

public slots:

    /**
     * Clears the content on the widget
     */
    void clearContent();

    // Used for internal testing will be removed
    void handleFwdAction();

    /**
     * Handle delete
     */
    void handleDeleteAction();

protected:

    /**
     * reimplemented from base class to provide proper geometry for scrolling.
     * @see HbWidget
     */
    void resizeEvent(QGraphicsSceneResizeEvent * event);

private:
    /**
     * Check if this message can be forwarded
     * @return true, if possible to forward
     */
    bool isForwardOk();

    /**
     * Creates tool bar actions
     * @see HbView
     */
    void createToolBar();
    
    /**
     * Launch Editor for forward, reply, replyall actions
     * @param operation, Editor operation e.g. forward, reply etc
     */
    void launchEditor(MsgBaseView::UniEditorOperation operation);

    /**
     * Launches the dialer interface.
     * @args number Number to be called.
     */
    void call(const QString &number);

private slots:
    /**
     * This slot is called when sendMessage signal is emitted for a highlighted
     * phone number, from body widget.
     */
    void sendMessage(const QString& phoneNumber, const QString& alias = QString());

    /**
     * This slot is called when delete message dialog is launched.
     * @param val selected action value.
     */
    void onDialogDeleteMsg(int val);
    
    /**
     * Handle reply
     */
    void handleReplyAction();
    
    /**
     * Handle reply-all
     */
    void handleReplyAllAction();

private:

    /**
     * Feeder object
     * Owned
     */
    UniViewerFeeder *mViewFeeder;

    /**
     * HbScrollArea object
     * Owned
     */
    UniScrollArea* mScrollArea;

    /**
     * UniContentsWidget object
     * Owned
     */
    UniContentsWidget* mContentsWidget;

    /**
     * Conversation ID
     */
    qint64 mConversationID;

    /**
     * Contact id
     */
    qint32 mContactId;

    /**
     * Message id
     */
    qint32 mMessageId;

    /**
     * Message count
     */
    int mMsgCount;

#ifdef MSGUI_UNIT_TEST
    /**
     * Unit Testing
     */
    friend class TestUnifiedViewer;
#endif
};

#endif
// EOF
