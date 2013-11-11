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

#include "draftslistview.h"

// SYSTEM INCLUDES
#include <HbMenu>
#include <HbAction>
#include <HbToolBar>
#include <HbToolBarExtension>
#include <HbListView>
#include <HbListViewItem>
#include <HbListWidget>
#include <HbListWidgetItem>
#include <HbStyleLoader>
#include <HbGroupBox>
#include <HbFrameBackground>
#include <HbMessageBox>
#include <HbMainWindow>

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QGraphicsLinearLayout>

// USER INCLUDES
#include "conversationsengine.h"
#include "conversationsenginedefines.h"
#include "convergedmessage.h"
#include "convergedmessageid.h"

// LOCAL CONSTANTS
const QString POPUP_LIST_FRAME("qtg_fr_popup_list_normal");
const QString NEW_MESSAGE_ICON("qtg_mono_create_message");
const QString SORT_ICON("qtg_mono_sort");

// LOCALIZATION CONSTANTS

// Long Tap
#define LOC_COMMON_OPEN           hbTrId("txt_common_menu_open")
#define LOC_COMMON_DELETE         hbTrId("txt_common_menu_delete")

// View heading
#define LOC_DLV_HEADING           hbTrId("txt_messaging_title_drafts")

// Menu items
#define LOC_MENU_DELETE_ALL       hbTrId("txt_messaging_opt_delete_all")

// Toolbar & toolbar exension
#define LOC_TB_EXTN_DRAFTS        hbTrId("txt_messaging_button_drafts")
#define LOC_TB_EXTN_CONVERSATIONS hbTrId("txt_messaging_button_conversations")

// Confirmation note
#define LOC_DELETE_MESSAGE        hbTrId("txt_messaging_dialog_delete_message")
#define LOC_DELETE_ALL_DRAFTS     hbTrId("txt_messaging_dialog_delete_all_drafts")

//---------------------------------------------------------------
// DraftsListView::DraftsListView
// @see header
//---------------------------------------------------------------
DraftsListView::DraftsListView(QGraphicsItem *parent) :
    MsgBaseView(parent),
    mListView(0), 
    mViewExtnList(0),
    mToolBar(0)
{
    // Delayed loading.
    connect(this->mainWindow(), SIGNAL(viewReady()), this, SLOT(doDelayedLoading()));
}

//---------------------------------------------------------------
// DraftsListView::~DraftsListView
// @see header
//---------------------------------------------------------------
DraftsListView::~DraftsListView()
{
}

//---------------------------------------------------------------
// DraftsListView::setupMenu
// @see header
//---------------------------------------------------------------
void DraftsListView::setupMenu()
{
    QAbstractItemModel *model = mListView->model();

    // Menu items are added/removed based on the item count.
    connect(model, SIGNAL(rowsInserted(QModelIndex, int, int)), this,
        SLOT(handleModelChanged()), Qt::UniqueConnection);
    connect(model, SIGNAL(rowsRemoved(QModelIndex, int, int)), this,
        SLOT(handleModelChanged()), Qt::UniqueConnection);
}

//---------------------------------------------------------------
// DraftsListView::setupToolbar
// @see header
//---------------------------------------------------------------
void DraftsListView::setupToolbar()
{
    if (!mToolBar) {
        mToolBar = this->toolBar();
        mToolBar->setOrientation(Qt::Horizontal);

        // Create & setup ToolBar Extension
        HbToolBarExtension *viewExtn = new HbToolBarExtension();
        HbAction *viewAction = mToolBar->addExtension(viewExtn);
        viewAction->setIcon(HbIcon(SORT_ICON));

        mViewExtnList = new HbListWidget();
        mViewExtnList->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
        mViewExtnList->addItem(LOC_TB_EXTN_DRAFTS);
        mViewExtnList->addItem(LOC_TB_EXTN_CONVERSATIONS);

        HbListViewItem *prototype = mViewExtnList->listItemPrototype();
        HbFrameBackground frame(POPUP_LIST_FRAME, HbFrameDrawer::NinePieces);
        prototype->setDefaultFrame(frame);

        connect(mViewExtnList, SIGNAL(activated(HbListWidgetItem*)), this,
            SLOT(handleViewExtnActivated(HbListWidgetItem*)));
        connect(mViewExtnList, SIGNAL(released(HbListWidgetItem*)), viewExtn, SLOT(close()));

        viewExtn->setContentWidget(mViewExtnList);

        // Create & setup 2nd ToolBar button.
        mToolBar->addAction(HbIcon(NEW_MESSAGE_ICON), "", this, SLOT(createNewMessage()));
    }
}

//---------------------------------------------------------------
// DraftsListView::setupListView
// @see header
//---------------------------------------------------------------
void DraftsListView::setupListView()
{
    if (!mListView) {
        // Create parent layout.
        QGraphicsLinearLayout *mainLayout = new QGraphicsLinearLayout(Qt::Vertical);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        // Create view heading.
        HbGroupBox *viewHeading = new HbGroupBox();
        viewHeading->setHeading(LOC_DLV_HEADING);

        // Create List View.
        mListView = new HbListView(this);

        mListView->setItemRecycling(true);
        mListView->setUniformItemSizes(true);
        mListView->setClampingStyle(HbScrollArea::BounceBackClamping);

        // Register the custorm css path.
        HbStyleLoader::registerFilePath(":/dlv");
        //    mListView->setLayoutName("custom");

        // Set list item properties.
        HbListViewItem *prototype = mListView->listItemPrototype();
        prototype->setGraphicsSize(HbListViewItem::SmallIcon);
        prototype->setStretchingStyle(HbListViewItem::StretchLandscape);
        prototype->setSecondaryTextRowCount(1, 1);

        // Create and set model
        QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
        proxyModel->setDynamicSortFilter(true);
        proxyModel->setSourceModel(ConversationsEngine::instance()->getDraftsModel());
        proxyModel->setSortRole(TimeStamp);
        proxyModel->sort(0, Qt::DescendingOrder);
        mListView->setModel(proxyModel);

        // Short & Long Taps
        connect(mListView, SIGNAL(activated(QModelIndex)), this,
            SLOT(openDraftMessage(QModelIndex)));
        connect(mListView, SIGNAL(longPressed(HbAbstractViewItem*,QPointF)), this,
            SLOT(handleLongPressed(HbAbstractViewItem*,QPointF)));

        // Add all widgets to main layout.
        mainLayout->addItem(viewHeading);
        mainLayout->addItem(mListView);

        this->setLayout(mainLayout);
    }
}

//------------------------------------------------------------------------------
// DraftsListView::doDelayedLoading
// @see header
//------------------------------------------------------------------------------
void DraftsListView::doDelayedLoading()
{
    setupToolbar();
    setupListView();
    setupMenu();
    disconnect(this->mainWindow(), SIGNAL(viewReady()), this, SLOT(doDelayedLoading()));
}

//------------------------------------------------------------------------------
// DraftsListView::openDraftMessage
// @see header
//------------------------------------------------------------------------------
void DraftsListView::openDraftMessage()
{
    openDraftMessage(mListView->currentIndex());
}

//------------------------------------------------------------------------------
// DraftsListView::deleteDraftMessage
// @see header
//------------------------------------------------------------------------------
void DraftsListView::deleteDraftMessage()
{
    QModelIndex index = mListView->currentIndex();

    if (!index.isValid()) {
        return;
    }

    HbMessageBox::question(LOC_DELETE_MESSAGE,
                           this,SLOT(onDialogDeleteMsg(HbAction*)),
                           HbMessageBox::Delete | HbMessageBox::Cancel);

}

//------------------------------------------------------------------------------
// DraftsListView::deleteAllDraftMessage
// @see header
//------------------------------------------------------------------------------
void DraftsListView::deleteAllDraftMessage()
{
    HbMessageBox::question(LOC_DELETE_ALL_DRAFTS,
                           this,SLOT(onDialogDeleteAllMessages(HbAction*)),
                           HbMessageBox::Delete | HbMessageBox::Cancel);
}

//------------------------------------------------------------------------------
// DraftsListView::createNewMessage
// @see header
//------------------------------------------------------------------------------
void DraftsListView::createNewMessage()
{
    QVariantList param;
    param << MsgBaseView::UNIEDITOR; // target view
    param << MsgBaseView::DLV; // source view

    emit switchView(param);
}

//------------------------------------------------------------------------------
// DraftsListView::openDraftMessage
// @see header
//------------------------------------------------------------------------------
void DraftsListView::openDraftMessage(const QModelIndex &index)
{
    QVariant msgId = index.data(ConvergedMsgId);
    QVariant msgType = index.data(MessageType);
    ConvergedMessageId convergedMsgId = ConvergedMessageId(msgId.toInt());
    ConvergedMessage message;
    message.setMessageType((ConvergedMessage::MessageType) msgType.toInt());
    message.setMessageId(convergedMsgId);

    // Launch uni-editor view
    QByteArray dataArray;
    QDataStream messageStream(&dataArray, QIODevice::WriteOnly | QIODevice::Append);
    message.serialize(messageStream);

    QVariantList params;
    params << MsgBaseView::UNIEDITOR; // target view
    params << MsgBaseView::DLV; // source view

    params << dataArray;
    emit switchView(params);
}

//------------------------------------------------------------------------------
// DraftsListView::handleLongPressed
// @see header
//------------------------------------------------------------------------------
void DraftsListView::handleLongPressed(HbAbstractViewItem *item, const QPointF &coords)
{
    if (this->isVisible()) {

        // Set the current index as tapped items index.
        mListView->setCurrentIndex(item->modelIndex(), QItemSelectionModel::Select);

        HbMenu *contextMenu = new HbMenu();
        contextMenu->setAttribute(Qt::WA_DeleteOnClose);
        // Open
        HbAction* openAction = contextMenu->addAction(LOC_COMMON_OPEN);
        connect(openAction, SIGNAL(triggered()), this, SLOT(openDraftMessage()));

        // Delete
        HbAction *deletAction = contextMenu->addAction(LOC_COMMON_DELETE);
        connect(deletAction, SIGNAL(triggered()), this, SLOT(deleteDraftMessage()));

        contextMenu->setPreferredPos(coords);
        contextMenu->show();
    }
}

//------------------------------------------------------------------------------
// DraftsListView::handleViewExtnActivated
// @see header
//------------------------------------------------------------------------------
void DraftsListView::handleViewExtnActivated(HbListWidgetItem *item)
{
    int row = mViewExtnList->row(item);
    if (CONVERSATIONS_EXTN == row) {
        QVariantList param;
        param << MsgBaseView::CLV; // target view
        param << MsgBaseView::DLV; // source view

        emit switchView(param);
    }
}

//------------------------------------------------------------------------------
// DraftsListView::handleModelChanged
// @see header
//------------------------------------------------------------------------------
void DraftsListView::handleModelChanged()
{
    // If there are no items in list view, delete the menu item.
    HbMenu *mainMenu = this->menu();
    if (!mListView->model()->rowCount()) {
        mainMenu->clearActions();
    }
    else {
        if (this->menu()->isEmpty()) {
            mainMenu->addAction(LOC_MENU_DELETE_ALL, this, SLOT(deleteAllDraftMessage()));
        }
    }
}

//------------------------------------------------------------------------------
// DraftsListView::onDialogDeleteMsg
// @see header
//------------------------------------------------------------------------------
void DraftsListView::onDialogDeleteMsg(HbAction* action)
{
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
    if (action == dlg->actions().at(0)) {
        QModelIndex index = mListView->currentIndex();
        if (index.isValid()) {
            int msgId = index.data(ConvergedMsgId).toInt();
            QList<int> msgIdList;
            msgIdList.append(msgId);
            ConversationsEngine::instance()->deleteMessages(msgIdList);
        }

    }
}

//------------------------------------------------------------------------------
// DraftsListView::onDialogDeleteMsg
// @see header
//------------------------------------------------------------------------------
void DraftsListView::onDialogDeleteAllMessages(HbAction* action)
{
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
    if (action == dlg->actions().at(0)) {
        ConversationsEngine::instance()->deleteAllDraftMessages();
    }
}

// EOF
