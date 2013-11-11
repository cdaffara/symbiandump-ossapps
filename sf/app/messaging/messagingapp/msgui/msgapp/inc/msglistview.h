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

#ifndef MSG_LIST_VIEW_H
#define MSG_LIST_VIEW_H

// INCLUDES
#include "msgbaseview.h"

// FORWARD DECLARATIONS
class HbListView;
class HbAbstractViewItem;
class HbListWidget;
class HbListWidgetItem;
class HbAction;
class QGraphicsLinearLayout;

/**
 * This class provides the message list view for the messaging application.
 * Data source for this view is the conversation list model.
 */
class MsgListView: public MsgBaseView
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    explicit MsgListView(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    virtual ~MsgListView();

private slots:
    /*
     * Handler for long tap of a list item.
     * @param viewItem  Lon tapped view item
     * @param point X and Y co-ordinates of long tap
     */
    void longPressed(HbAbstractViewItem* viewItem, const QPointF& point);

    /*
     * Handler for short tap of a list item and Item specific menu
     * Opens the conversation view.
     * @param index Model index of the list item tapped.
     */
    void openConversation(const QModelIndex& index);

    /**
     * Handler for new message button trigger
     */
    void sendNewMessage();

    /**
     *Popup the message settings dialog
     */
    void settings();

    // -------------------------Item Specific Menu----------------------- //

    /**
     * Opens the conversation view.
     */
    void openConversation();

    /**
     * Opens the contact information
     */
    void showContact();

    /**
     * Deletes the seleted item from the list of Conversations
     */
    void deleteItem();

    /**
     * Handles toolbar extension item clicks.
     * @param item Activated toolbar extension item.
     */
    void handleViewExtnActivated(HbListWidgetItem *item);
    
    /**
     * Constructs view when viewReady is emitted.
     */
    void doDelayedConstruction();
    
    /**
     * Handles saving to contacts
     */
    void saveToContacts();
    
    /**
     * Opens contact card for resolved contact.
     */
    void contactInfo();

	/**
     * This slot is called when delete message dialog is launched.
     * @param action selected action (yes or no).
     */
    void onDialogDeleteMsg(HbAction* action);
    
	/**
	 * This slot is called when appengine emits the partialdeleteconversationlist event.
	 * This performs grab of the HbAbstractviewitem on which delete was called.
	 * @param conversation id of the item.
	 */
    void enableListitem( int conversationId );
	    
private:

    /**
     * Initialize the list view inside the main view
     */
    void setupListView();

    /**
     * Initialize the view specific menu
     */
    void setupMenu();

    /**
     * Initialize the tool bar of the view
     */
    void setupToolBar();

private:
    /**
     * List view
     * Own.
     */
    HbListView* mMsgList;

    /**
     * Container widget of toolbar extension.
     * ToolBar takes ownership.
     */
    HbListWidget *mViewExtnList;

    /**
     * Views in toolbar extension.
     */
    enum viewExtnIds
    {
        DRAFTS_EXTN = 0x00, CONVERSATIONS_EXTN = 0x01
    };

    /**
     * Main layout
     * Own
     */    
    QGraphicsLinearLayout *mMainLayout;
};

#endif // MSG_LIST_VIEW_H
