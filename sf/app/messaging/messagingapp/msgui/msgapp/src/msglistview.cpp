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
 * Description:Message List View for the messaging application.
 *
 */

#include "msglistview.h"

// SYSTEM INCLUDES
#include <hbmenu.h>
#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbtoolbarextension.h>
#include <hblistview.h>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <hbgroupbox.h>
#include <hbmessagebox.h>
#include <hbframebackground.h>
#include <QSortFilterProxyModel>
#include <QGraphicsLinearLayout>
#include <hblistviewitem.h>
#include <HbStyleLoader>
#include <HbMainWindow>
#include <xqaiwrequest.h>
#include <xqappmgr.h>
#include <qtcontacts.h>

// USER INCLUDES
#include "msgconversationviewinterface.h"
#include "debugtraces.h"
#include "conversationsengine.h"
#include "conversationsenginedefines.h"
#include "convergedmessage.h"
#include "msglistviewitem.h"

QTM_USE_NAMESPACE

//Icons
const QString POPUP_LIST_FRAME("qtg_fr_popup_list_normal");
const QString NEW_MESSAGE_ICON("qtg_mono_create_message");
const QString SORT_ICON("qtg_mono_sort");

//Localized constants

#define LOC_DIALOG_DELETE_CONVERSATION hbTrId("txt_messaging_dialog_delete_conversation")
#define LOC_DIALOG_UNABLE_TO_DELETE_CONVERSATION hbTrId("txt_messaging_dialog_unable_to_delete_conversation")

//itemspecific menu
#define LOC_OPEN hbTrId("txt_common_menu_open")
#define LOC_SAVETO_CONTACTS hbTrId("txt_messaging_menu_save_to_contacts")
#define LOC_DELETE_CONVERSATION hbTrId("txt_messaging_menu_delete_conversation")
#define LOC_OPEN_CONTACT_INFO hbTrId("txt_messaging_menu_contact_info")

//main menu
#define LOC_SETTINGS    hbTrId("txt_messaging_opt_settings")

#define LOC_TB_VIEW_EXTN hbTrId("txt_messaging_button_view")
#define LOC_TB_EXTN_DRAFTS hbTrId("txt_messaging_button_drafts")
#define LOC_TB_EXTN_CONVERSATIONS hbTrId("txt_messaging_button_conversations")

#define LOC_VIEW_HEADING hbTrId("txt_messaging_title_conversations")

//---------------------------------------------------------------
// MsgListView::MsgListView
// @see header
//---------------------------------------------------------------
MsgListView::MsgListView(QGraphicsItem *parent) :
    MsgBaseView(parent)
    {
    //These changes are needed for splash implementation to avoid flicker
    setupToolBar();
    setupMenu();

    // Create parent layout.
    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mMainLayout->setContentsMargins(0, 0, 0, 0);
    mMainLayout->setSpacing(0);

    // Create view heading.
    HbGroupBox *viewHeading = new HbGroupBox();
    viewHeading->setHeading(LOC_VIEW_HEADING);

    // Add view heading widget to main layout.
    mMainLayout->addItem(viewHeading);

    this->setLayout(mMainLayout);

    connect(this->mainWindow(), SIGNAL(viewReady()), this,
            SLOT(doDelayedConstruction()));
    }

//---------------------------------------------------------------
// MsgListView::~MsgListView
// @see header
//---------------------------------------------------------------
MsgListView::~MsgListView()
{
    
}

//---------------------------------------------------------------
// MsgListView::longPressed
// @see header
//---------------------------------------------------------------
void MsgListView::longPressed(HbAbstractViewItem* viewItem, const QPointF& point)
{
    if (this->isVisible()) {
        // Set the current index as the present Item's index.
        // By default it will not be set.
        mMsgList->setCurrentIndex(viewItem->modelIndex(), QItemSelectionModel::Select);

        // Create new menu
        HbMenu *contextMenu = new HbMenu();
        contextMenu->setAttribute(Qt::WA_DeleteOnClose);
        //open menu option
        contextMenu->addAction(LOC_OPEN,this,SLOT(openConversation()));
        
        //save to contacts for unresolved.
        int msgType = viewItem->modelIndex().data(MessageType).toInt();
        if(msgType == ConvergedMessage::Sms || 
           msgType == ConvergedMessage::Mms || 
           msgType == ConvergedMessage::BioMsg)
            {
            qint64 contactId = mMsgList->currentIndex().data(ContactId).toLongLong();
            if(contactId < 0)
                {
                contextMenu->addAction(LOC_SAVETO_CONTACTS,this,SLOT(saveToContacts()));
                }
            else
                {
                contextMenu->addAction(LOC_OPEN_CONTACT_INFO,this,SLOT(contactInfo()));
                }
            }
        
        //delete conversation
        contextMenu->addAction(LOC_DELETE_CONVERSATION,this,SLOT(deleteItem()));

        contextMenu->setPreferredPos(point);    
        contextMenu->show();
    }
}

//---------------------------------------------------------------
// MsgListView::openConversation
// @see header
//---------------------------------------------------------------
void MsgListView::openConversation(const QModelIndex& index)
{
    //TODO: model populating possibilities.
    if (index.isValid()) {
        QVariant conversationId = index.data(ConversationId);

        // Launch conversation view
        QVariantList param;
        param << MsgBaseView::CV; // target view
        param << MsgBaseView::CLV; // source view

        param << conversationId;

        emit switchView(param);
    }
}

//---------------------------------------------------------------
// MsgListView::sendNewMessage
// @see header
//---------------------------------------------------------------
void MsgListView::sendNewMessage()
{
    QVariantList param;
    param << MsgBaseView::UNIEDITOR; // target view
    param << MsgBaseView::CLV; // source view

    emit switchView(param);
}

//---------------------------------------------------------------
// MsgListView::settings
// @see header
//---------------------------------------------------------------
void MsgListView::settings()
{
    // Launch message settings
    QVariantList param;
    param << MsgBaseView::MSGSETTINGS;
    param << MsgBaseView::CLV;
    emit switchView(param);
}

//---------------------------------------------------------------
// MsgListView::openConversation
// @see header
//---------------------------------------------------------------
void MsgListView::openConversation()
{
    QModelIndex modelIndex = mMsgList->currentIndex();
    openConversation(modelIndex);
}

//---------------------------------------------------------------
// MsgListView::showContact
// @see header
//---------------------------------------------------------------
void MsgListView::showContact()
{
}

//---------------------------------------------------------------
// MsgListView::deleteItem
// @see header
//---------------------------------------------------------------
void MsgListView::deleteItem()
{
#ifdef _DEBUG_TRACES_
    qDebug() << "Inside MsgListView::deleteItem";
#endif
    QModelIndex index = mMsgList->currentIndex();
    if (index.isValid())
    {
        int sendState = index.data(SendingState).toInt();
        if(ConvergedMessage::Sending == sendState)
        {
            //confirmation dialog.
            HbMessageBox::information(LOC_DIALOG_UNABLE_TO_DELETE_CONVERSATION,
                0,0,
                HbMessageBox::Ok);      
        }
        // not in sending state and hence can be deleted.
        else
        {
            //confirmation dialog.
            HbMessageBox::question(LOC_DIALOG_DELETE_CONVERSATION,
                this,SLOT(onDialogDeleteMsg(HbAction*)),
                HbMessageBox::Delete | HbMessageBox::Cancel);       
        }

    }
    
#ifdef _DEBUG_TRACES_	
    qDebug() << " Leaving MsgConversationView::deleteItem";
#endif
}

//---------------------------------------------------------------
// MsgListView::setupView
// @see header
//---------------------------------------------------------------
void MsgListView::setupListView()
    {
    // Register the custorm css path.
    HbStyleLoader::registerFilePath(":/clv");

    mMsgList = new HbListView(this);
    mMsgList->setClampingStyle(HbScrollArea::BounceBackClamping);

    mMsgList->setLayoutName("custom");
    mMsgList->setItemRecycling(true);
	mMsgList->setItemPixmapCacheEnabled(true);
    mMsgList->setUniformItemSizes(true);

    MsgListViewItem *prototype = new MsgListViewItem(this);
    mMsgList->setItemPrototype(prototype);
    mMsgList->setModel(ConversationsEngine::instance()->getConversationsSummaryModel());
    
    // Single tap list item
    connect(mMsgList, SIGNAL(activated(const QModelIndex&)), this,
            SLOT(openConversation(const QModelIndex&)));

    // Long tap list item
    connect(mMsgList,
            SIGNAL(longPressed(HbAbstractViewItem*, const QPointF&)), this,
            SLOT(longPressed(HbAbstractViewItem*, const QPointF&)));

    // enable tap after delete is completed
    connect(ConversationsEngine::instance(),
            SIGNAL(conversationListEntryDeleted( int )), this,
            SLOT(enableListitem( int )));

    // Add list view to main layout.
    mMainLayout->addItem(mMsgList);
    }

//---------------------------------------------------------------
// MsgListView::addMenu
// @see header
//---------------------------------------------------------------
void MsgListView::setupMenu()
{
    // Main menu
    HbMenu* mainMenu = this->menu();
    mainMenu->addAction(LOC_SETTINGS,this,SLOT(settings()));
}

//---------------------------------------------------------------
// MsgListView::setupToolBar
// @see header
//---------------------------------------------------------------
void MsgListView::setupToolBar()
{
    // ToolBar
    HbToolBar *toolBar = this->toolBar();
    toolBar->setOrientation(Qt::Horizontal);

    // Create & setup ToolBar Extension
    HbToolBarExtension *viewExtn = new HbToolBarExtension();
    HbAction *viewAction = toolBar->addExtension(viewExtn);
    viewAction->setIcon(HbIcon(SORT_ICON));

    mViewExtnList = new HbListWidget();
    mViewExtnList->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    mViewExtnList->addItem(LOC_TB_EXTN_DRAFTS);
    mViewExtnList->addItem(LOC_TB_EXTN_CONVERSATIONS);

    HbListViewItem *prototype = mViewExtnList->listItemPrototype();
    HbFrameBackground frame(POPUP_LIST_FRAME, HbFrameDrawer::NinePieces);
    prototype->setDefaultFrame(frame);

    connect(mViewExtnList, SIGNAL(activated(HbListWidgetItem*)),
            this,SLOT(handleViewExtnActivated(HbListWidgetItem*)));
    connect(mViewExtnList, SIGNAL(released(HbListWidgetItem*)), viewExtn, SLOT(close()));

    viewExtn->setContentWidget(mViewExtnList);
    
    // Create & setup 2nd ToolBar button.
    toolBar->addAction(HbIcon(NEW_MESSAGE_ICON),"",this,SLOT(sendNewMessage()));
}

//---------------------------------------------------------------
// MsgListView::handleViewExtnActivated
// @see header
//---------------------------------------------------------------
void MsgListView::handleViewExtnActivated(HbListWidgetItem *item)
{
    int row = mViewExtnList->row(item);
    if (DRAFTS_EXTN == row) {
        QVariantList param;
        param << MsgBaseView::DLV; // target view
        param << MsgBaseView::CLV; // source view

        emit switchView(param);
    }
}

//---------------------------------------------------------------
// MsgListView::doDelayedConstruction
// @see header
//---------------------------------------------------------------
void MsgListView::doDelayedConstruction()
    {    
    setupListView();
    
    disconnect(this->mainWindow(),SIGNAL(viewReady()),this,SLOT(doDelayedConstruction()));
    }

//---------------------------------------------------------------
// MsgListView::saveToContacts
// @see header
//---------------------------------------------------------------
void MsgListView::saveToContacts()
    {
    //save to contacts with phone number field prefilled.
    QList<QVariant> args;  

    QString data = mMsgList->currentIndex().data(DisplayName).toString();

    QString type = QContactPhoneNumber::DefinitionName;

    args << type;
    args << data;

    //service stuff.
    QString service("phonebookservices");
    QString interface("com.nokia.symbian.IContactsEdit");
    QString operation("editCreateNew(QString,QString)");

    XQAiwRequest* request;
    XQApplicationManager appManager;
    request = appManager.create(service, interface, operation, true); // embedded
    if ( request == NULL )
        {
        return;       
        }

    request->setArguments(args);
    request->send();
    
    delete request;
    }

//---------------------------------------------------------------
// MsgListView::contactInfo
// @see header
//---------------------------------------------------------------
void MsgListView::contactInfo()
    {
    //open contact info.
    QList<QVariant> args;  

    int contactId = mMsgList->currentIndex().data(ContactId).toInt();

    args << contactId;

    //service stuff.
    QString service("phonebookservices");
    QString interface("com.nokia.symbian.IContactsView");
    QString operation("openContactCard(int)");

    XQAiwRequest* request;
    XQApplicationManager appManager;
    request = appManager.create(service, interface, operation, true); // embedded
    if ( request == NULL )
        {
        return;       
        }

    request->setArguments(args);
    request->send();
    
    delete request;
    }

//---------------------------------------------------------------
// MsgListView::onDialogDeleteMsg
// @see header
//---------------------------------------------------------------
void MsgListView::onDialogDeleteMsg(HbAction* action)
    {
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
    if (action == dlg->actions().at(0))
        {
        QModelIndex index = mMsgList->currentIndex();
        if (index.isValid())
            {
            HbAbstractViewItem *item = mMsgList->currentViewItem();
            item->ungrabGesture(Qt::TapGesture);
            qint64 conversationId = index.data(ConversationId).toLongLong();

            ConversationsEngine::instance()->deleteConversations(
                    conversationId);

            }

        }
    }

//---------------------------------------------------------------
// MsgListView::enableListitem
// @see header
//---------------------------------------------------------------
void MsgListView::enableListitem(int conversationId)
    {
    QAbstractItemModel* proxyModel = mMsgList->model();
    QModelIndexList indexList = proxyModel->match(proxyModel->index(0, 0),
            ConversationId, conversationId, 1, Qt::MatchExactly);
    HbAbstractViewItem* item = NULL;
    int count = indexList.count();
	// only item is expected to be the result of the above match function
    if (1 == count)
        {
        item = mMsgList->itemByIndex(indexList[0]);
        if (item)
            {
            item->grabGesture(Qt::TapGesture);
            }
        }

    }

//EOF
