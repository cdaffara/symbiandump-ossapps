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
 * Description: Drafts List View for displaying drafts messages.
 *
 */

#ifndef DRAFTS_LISTVIEW_H
#define DRAFTS_LISTVIEW_H

#include <msgbaseview.h>

// FORWARD DECLARATIONS
class HbToolBar;
class HbListView;
class HbListWidget;
class HbListWidgetItem;
class HbAbstractViewItem;
class HbAction;
/**
 * List view implementation for showing Draft messages.
 */
class DraftsListView: public MsgBaseView
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    explicit DraftsListView(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    virtual ~DraftsListView();

private:

    /**
     * Creates the menu items.
     */
    void setupMenu();

    /**
     * Creates the toolbar & toolbar extension.
     */
    void setupToolbar();

    /**
     * Creates the list view.
     */
    void setupListView();

private slots:

    /**
     * Do delayed loading.
     */
    void doDelayedLoading();

    /**
     * Opens a Draft message.
     */
    void openDraftMessage();

    /**
     * Deletes a Draft message.
     */
    void deleteDraftMessage();

    /**
     * Deletes all Draft messages.
     */
    void deleteAllDraftMessage();

    /**
     * Invokes editor for creating new message.
     */
    void createNewMessage();

    /**
     * Opens a Draft message.
     * @param index Index of the list item activated.
     */
    void openDraftMessage(const QModelIndex &index);

    /**
     * Item specific long press handler.
     * @param item Item which is long pressed.
     * @param coords Screen coordinates of the click.
     */
    void handleLongPressed(HbAbstractViewItem *item, const QPointF &coords);

    /**
     * Handles toolbar extension item clicks.
     * @param item Activated toolbar extension item.
     */
    void handleViewExtnActivated(HbListWidgetItem *item);

    /**
     * This slot is hit when rows are inserted/removed in model.
     */
    void handleModelChanged();

	/**
     * This slot is called when delete message dialog is launched.
     * @param action selected action (yes or no).
     */
    void onDialogDeleteMsg(HbAction* action);
    
    /**
     * This slot is called when delete message dialog is launched.
     * @param action selected action (yes or no).
     */
    void onDialogDeleteAllMessages(HbAction* action);
    
private:

    /**
     * List view for Draft messages.
     * Own.
     */
    HbListView *mListView;

    /**
     * Container widget of toolbar extension.
     * ToolBar takes ownership.
     */
    HbListWidget *mViewExtnList;

    /**
     * Pointer to views toolbar.
     * Not Own.
     */
    HbToolBar *mToolBar;

    /**
     * Views in toolbar extension.
     */
    enum viewExtnIds
    {
        DRAFTS_EXTN = 0x00, CONVERSATIONS_EXTN = 0x01
    };
    
};

#endif /* DRAFTS_LISTVIEW_H */

// EOF
