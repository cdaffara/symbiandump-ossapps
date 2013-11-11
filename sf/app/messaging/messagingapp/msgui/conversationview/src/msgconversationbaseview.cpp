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

#include "msgconversationbaseview.h"

// SYSTEM INCLUDES
#include <QGraphicsLinearLayout>
#include <HbMainWindow>
#include <HbDeviceNotificationDialog>

#include <xqaiwrequest.h>
#include <xqappmgr.h>

#include <cntservicescontact.h>
#include <qtcontactsglobal.h>
#include <qtcontacts.h>
#include <ccsdefs.h>
#include <XQSettingsManager>
#include <QTimer>

// USER INCLUDES
#include "msgconversationview.h"
#include "conversationsengine.h"
#include "msgviewdefines.h"
#include "conversationsenginedefines.h"
#include "msgcontactcardwidget.h"
#include "conversationidpsconsts.h"

QTM_USE_NAMESPACE

const int INVALID_MSGID = -1;
const int INVALID_CONVID = -1;

// LOCALIZATION
#define LOC_SAVED_TO_DRAFTS    hbTrId("txt_messaging_dpopinfo_saved_to_drafts")

//---------------------------------------------------------------
// MsgConversationBaseView::MsgConversationBaseView
// Constructor
//---------------------------------------------------------------
MsgConversationBaseView::MsgConversationBaseView(QGraphicsItem* parent) :
    MsgBaseView(parent), mConversationId(-1), mCVIdkey(XQSettingsKey::TargetPublishAndSubscribe,
        KMsgCVIdProperty, KMsgCVIdKey)
{
    connect(this->mainWindow(), SIGNAL(viewReady()), this, SLOT(doDelayedConstruction()));
    initView();
}

//---------------------------------------------------------------
// MsgConversationBaseView::~MsgConversationBaseView
// Destructor
//---------------------------------------------------------------
MsgConversationBaseView::~MsgConversationBaseView()
{
}

//---------------------------------------------------------------
// MsgConversationBaseView::openConversationView
// Launches conversation view for entry id and mode
//---------------------------------------------------------------
void MsgConversationBaseView::openConversation(qint64 convId)
{
    ConversationsEngine::instance()->getConversations(convId);
    mConversationId = convId;
    connect(this->mainWindow(), SIGNAL(viewReady()), this, SLOT(doDelayedConstruction()));

    // publsih conversation id
    mSettingsManager->writeItemValue(mCVIdkey, (int) mConversationId);

    if (mConversationView) {
        mConversationView->refreshView();
    }
}

//---------------------------------------------------------------
// MsgConversationBaseView::initView
// create and initialise the conversationview
//---------------------------------------------------------------
void MsgConversationBaseView::initView()
{

    // Create header widget
    mContactCard = new MsgContactCardWidget(this);

    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical);

    qreal spacing = HbDeviceProfile::profile(this).unitValue();
    mMainLayout->setSpacing(spacing);
    mMainLayout->setContentsMargins(CONTENT_MARGIN, CONTENT_MARGIN, CONTENT_MARGIN, CONTENT_MARGIN);

    mMainLayout->addItem(mContactCard);
    connect(mContactCard, SIGNAL(conversationIdChanged(qint64)), this,
        SLOT(handleConversationIdChange(qint64)));

    /**
     * Create conversation view and connect to proper signals.
     * NOTE: contactCardWidget is NOT parent of MsgConversationView.
     * Just passing reference to MsgConversationView.
     */
    mConversationView = new MsgConversationView(mContactCard);

    mConversationView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(mConversationView, SIGNAL(closeConversationView()), this, SLOT(closeConversationView()));

    connect(mConversationView, SIGNAL(replyStarted()), this, SLOT(markMessagesAsRead()));

    connect(mConversationView, SIGNAL(switchView(const QVariantList&)), this,
        SIGNAL(switchView(const QVariantList&)));

    connect(mConversationView, SIGNAL(vkbOpened(bool)), this, SLOT(hideChrome(bool)));

    this->setMenu(mConversationView->menu());

    mMainLayout->addItem(mConversationView);

    this->setLayout(mMainLayout);

    mSettingsManager = new XQSettingsManager(this);

}

//---------------------------------------------------------------
// MsgConversationBaseView::closeConversationView
// close conversation view
//---------------------------------------------------------------
void MsgConversationBaseView::closeConversationView()
{
    markMessagesAsRead();
}

//---------------------------------------------------------------
// MsgConversationBaseView::markMessagesAsRead
// Mark unread msgs in cv as read
//---------------------------------------------------------------
void MsgConversationBaseView::markMessagesAsRead()
{
    if (mConversationId >= 0) {
        ConversationsEngine::instance()->markConversationRead(mConversationId);
    }
}

//---------------------------------------------------------------
// MsgConversationBaseView::saveContentToDrafts
// saves the editors content to drafts
//---------------------------------------------------------------
int MsgConversationBaseView::saveContentToDrafts()
{
    int msgId = INVALID_MSGID;
    if (mConversationId >= 0) {
    msgId = mConversationView->saveContentToDrafts();
    }

    if (msgId != INVALID_MSGID) {
        HbDeviceNotificationDialog::notification("", LOC_SAVED_TO_DRAFTS);
    }
    return msgId;
}

//--------------------------------------------------------------- 
// MsgConversationBaseView::conversationId 
// get the conversation ID 
//---------------------------------------------------------------     
qint64 MsgConversationBaseView::conversationId()
{
    return mConversationId;
}

//---------------------------------------------------------------
// MsgConversationBaseView::clearContent
// clears conversation view content
//---------------------------------------------------------------
void MsgConversationBaseView::clearContent()
{
    ConversationsEngine::instance()->clearConversations();
    mConversationView->clearEditors();
}

//---------------------------------------------------------------
// MsgConversationBaseView::handleOk
//
//---------------------------------------------------------------
void MsgConversationBaseView::handleOk(const QVariant& result)
{
    Q_UNUSED(result)
}

//---------------------------------------------------------------
// MsgConversationBaseView::handleError
//
//---------------------------------------------------------------
void MsgConversationBaseView::handleError(int errorCode, const QString& errorMessage)
{
    Q_UNUSED(errorMessage)
    Q_UNUSED(errorCode)
}

//---------------------------------------------------------------
// MsgConversationBaseView::doDelayedConstruction
//
//---------------------------------------------------------------	
void MsgConversationBaseView::doDelayedConstruction()
{
    disconnect(this->mainWindow(), SIGNAL(viewReady()), this, SLOT(doDelayedConstruction()));
    QTimer::singleShot(50, this, SLOT(handleViewReady()));
}

//---------------------------------------------------------------
// MsgConversationBaseView::handleViewReady
//
//---------------------------------------------------------------	
void MsgConversationBaseView::handleViewReady()
{
    mConversationView->onViewReady();
}

//---------------------------------------------------------------
// MsgConversationBaseView::handleConversationIdChange
//
//---------------------------------------------------------------
void MsgConversationBaseView::handleConversationIdChange(qint64 convId)
{
    if (INVALID_CONVID != convId && mConversationId != convId) {
	
        mConversationId = convId;        
        // publsih conversation id
        mSettingsManager->writeItemValue(mCVIdkey, (int) mConversationId);       
    }
}

//---------------------------------------------------------------
// MsgConversationBaseView::hideChrome
//
//---------------------------------------------------------------
void MsgConversationBaseView::hideChrome(bool hide)
{
    if (hide) {
        this->hideItems(Hb::StatusBarItem | Hb::TitleBarItem);
        this->setContentFullScreen(true);

        if (this->mainWindow()->orientation() == Qt::Horizontal) {
            mMainLayout->removeItem(mContactCard);
            mContactCard->hide();
        }
    }
    else {
        this->showItems(Hb::StatusBarItem | Hb::TitleBarItem);
        this->setContentFullScreen(false);

        if (!mContactCard->isVisible()) {
            mMainLayout->insertItem(0, mContactCard);
            mContactCard->show();
        }
    }
    
    //forcing relayouting
    mMainLayout->activate();
}

//---------------------------------------------------------------
// MsgConversationBaseView::setPSCVId
//
//---------------------------------------------------------------
void MsgConversationBaseView::setPSCVId(bool setId)
{
    if (setId) {
        mSettingsManager->writeItemValue(mCVIdkey, (int) mConversationId);
    }
    else {
        mSettingsManager->writeItemValue(mCVIdkey, -1);
    }
}

//---------------------------------------------------------------
// MsgConversationBaseView::handleKeyEvent
//
//---------------------------------------------------------------
bool MsgConversationBaseView::handleKeyEvent(int key)
{
    return mConversationView->handleKeyEvent(key);
}
// EOF
