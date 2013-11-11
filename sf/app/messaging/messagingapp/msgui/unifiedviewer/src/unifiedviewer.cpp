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

#include "unifiedviewer.h"

// SYSTEM INCLUDES
#include <QGraphicsLinearLayout>
#include <HbAction>
#include <HbToolBar>
#include <hbmessagebox.h>
#include <hbnotificationdialog.h>
#include <HbStyleLoader>
#include <centralrepository.h>
#include <MmsEngineDomainCRKeys.h>
#include <ccsdefs.h>
#include <xqappmgr.h>
#include <xqaiwrequest.h>
#include <xqserviceutil.h>
#include <sqldb.h>

// USER INCLUDES
#include "uniscrollarea.h"
#include "unicontentswidget.h"
#include "univiewerfeeder.h"
#include "convergedmessageid.h"
#include "convergedmessage.h"
#include "convergedmessageaddress.h"
#include "conversationsenginedefines.h"
#include "conversationsengine.h"
#include "debugtraces.h"
#include "nativemessageconsts.h"
#include "mmsconformancecheck.h"
#include "msgsendutil.h"

// LOCAL CONSTANTS
const QString REPLY_ICON("qtg_mono_reply");
const QString REPLY_ALL_ICON("qtg_mono_reply_all");
const QString FORWARD_ICON("qtg_mono_forward_msg");
const QString SEND_ICON("qtg_mono_send");
const QString DELETE_ICON("qtg_mono_delete");

//DB-file
_LIT(KDbFileName, "c:[2002A542]conversations.db");
// query to see if msg is forwardable
_LIT(KSelectMsgPropertyStmt, " SELECT message_id, msg_property, msg_processingstate FROM conversation_messages WHERE message_id=:message_id ");


//LOCALIZED CONSTANTS
#define LOC_DELETE_MESSAGE hbTrId("txt_messaging_dialog_delete_message")
#define LOC_DELETE_SHARED_MESSAGE hbTrId("txt_messaging_dialog_same_message_exists_in_multip")

//----------------------------------------------------------------------------
// UnifiedViewer::UnifiedViewer
// constructor
//----------------------------------------------------------------------------
UnifiedViewer::UnifiedViewer(const qint32 messageId, QGraphicsItem *parent) :
    MsgBaseView(parent), mConversationID(-1)
{
    QDEBUG_WRITE("UnifiedViewer contruction start");

    if (!HbStyleLoader::registerFilePath(":/layouts")) {
        QDEBUG_WRITE("ERROR: UnifiedViewer -> HbStyleLoader::registerFilePath");
    }

    mMessageId = messageId;
    mViewFeeder = new UniViewerFeeder(mMessageId, this);

    mScrollArea = new UniScrollArea(this);
    this->setWidget(mScrollArea);

    mContentsWidget = new UniContentsWidget(mViewFeeder, this);

    connect(mContentsWidget, SIGNAL(sendMessage(const QString&,const QString&)), this,
        SLOT(sendMessage(const QString&,const QString&)));

    connect(mScrollArea, SIGNAL(scrolledToNextSlide()), mContentsWidget, SLOT(populateNextSlide()));

    mScrollArea->setContentWidget(mContentsWidget);
    mScrollArea->setHorizontalScrollBarPolicy(HbScrollArea::ScrollBarAlwaysOff);
    mScrollArea->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAutoHide);

    QDEBUG_WRITE("UnifiedViewer contruction End");
}

//----------------------------------------------------------------------------
// UnifiedViewer::UnifiedViewer
// Destructor
//----------------------------------------------------------------------------
UnifiedViewer::~UnifiedViewer()
{
    HbStyleLoader::unregisterFilePath(":/layouts");
}

//----------------------------------------------------------------------------
// UnifiedViewer::createToolBar
// Creates tool bar actions
//----------------------------------------------------------------------------
void UnifiedViewer::createToolBar()
{
    HbToolBar* toolbar = this->toolBar();
    toolbar->setOrientation(Qt::Horizontal);

    int sendingState = mViewFeeder->sendingState();

    if(mViewFeeder->sendingState() != ConvergedMessage::Failed)
    {
        // do not show reply option for multi recipient outgoing message
        if(!(!mViewFeeder->isIncoming() && mViewFeeder->recipientCount()>1))
        {
            QString from, alias;
            mViewFeeder->fromAddressAndAlias(from, alias);
            QScopedPointer<MsgSendUtil> sendUtil(new MsgSendUtil());
            if (!(mViewFeeder->isIncoming() && !(sendUtil->isValidAddress(from)))) {
                toolbar->addAction(HbIcon(REPLY_ICON), "", this, SLOT(handleReplyAction()));
            }
        }

        if (mViewFeeder->recipientCount() > 1)
        {
            toolbar->addAction(HbIcon(REPLY_ALL_ICON), "", this, SLOT(handleReplyAllAction()));
        }
    }

    if (isForwardOk())
    {
        toolbar->addAction(HbIcon(FORWARD_ICON), "", this, SLOT(handleFwdAction()));
    }

    // No delete action for viewer when launched as a service
    if(!XQServiceUtil::isService())
    {
        toolbar->addAction(HbIcon(DELETE_ICON), "",
                this, SLOT(handleDeleteAction()));
    }
}

//---------------------------------------------------------------
// UnifiedViewer::clearContent
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::clearContent()
{
    QDEBUG_WRITE("UnifiedViewer clearContent start");

    mContentsWidget->clearContent();
    mContentsWidget->resize(mContentsWidget->rect().width(), -1);
    mScrollArea->setPosToStart();
    mViewFeeder->clearContent();

    QDEBUG_WRITE("UnifiedViewer clearContent End");
}

//---------------------------------------------------------------
// UnifiedViewer::populateContent
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::populateContent(const qint32 messageId, bool update, int msgCount, qint64 conversationId)
{
    QDEBUG_WRITE("UnifiedViewer populateContent Start");

    mMsgCount = msgCount;
    mMessageId = messageId;
    mConversationID = conversationId;

    QDEBUG_WRITE("UnifiedViewer feeder->updateContent START");

    if (update)
    {
        mViewFeeder->updateContent(messageId);
    }
    QDEBUG_WRITE("UnifiedViewer feeder->updateContent END");

    // Dont show the scroll bar.
    mScrollArea->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAlwaysOff);

    if ( (mViewFeeder->msgType() == KSenduiMtmMmsUidValue) &&
         (mViewFeeder->slideCount() > 0) )
    {
        mScrollArea->setTotalSlides(mViewFeeder->slideCount());
    }
    else
    {
        mScrollArea->setTotalSlides(1);
    }
    mScrollArea->resetCurrentSlide();

    mContentsWidget->populateContent();

    //Creation of toolbar now depends on content
    createToolBar();

    QDEBUG_WRITE("UnifiedViewer populateContent END");
}

//---------------------------------------------------------------
// UnifiedViewer::handleKeyEvent
// @see header file
//---------------------------------------------------------------
bool UnifiedViewer::handleKeyEvent(int key)
{
    bool eventHandled = false;
    if (Qt::Key_Yes == key) {
        bool incoming = mViewFeeder->isIncoming();
        QString number;
        if (incoming) {
            QString alias;
            mViewFeeder->fromAddressAndAlias(number, alias);
        }
        else if (!incoming && 1 == mViewFeeder->recipientCount()) {
            ConvergedMessageAddressList addrList;
            if ((addrList = mViewFeeder->toAddressList()).count()) {
                number = addrList[0]->address();
            }
            else if((addrList = mViewFeeder->ccAddressList()).count()) {
                number = addrList[0]->address();
            }
            else if ((addrList = mViewFeeder->bccAddressList()).count()) {
                number = addrList[0]->address();
            }
        }

        // Call
        if (!number.isEmpty()) {
            eventHandled = true;
            call(number);
        }
    }
    return eventHandled;
}

//---------------------------------------------------------------
// UnifiedViewer::handleFwdAction
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::handleFwdAction()
{
    launchEditor(MsgBaseView::FORWARD_MSG);
}

//---------------------------------------------------------------
// UnifiedViewer::handleReplyAction
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::handleReplyAction()
{
    launchEditor(MsgBaseView::REPLY_MSG);
}

//---------------------------------------------------------------
// UnifiedViewer::handleReplyAllAction
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::handleReplyAllAction()
{
    launchEditor(MsgBaseView::REPLY_ALL_MSG);
}

//---------------------------------------------------------------
// UnifiedViewer::resizeEvent
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::resizeEvent(QGraphicsSceneResizeEvent * event)
{
    Q_UNUSED(event)
    mContentsWidget->resize(this->rect().width(), this->rect().height()+1);
}

//---------------------------------------------------------------
// UnifiedViewer::handleDeleteAction
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::handleDeleteAction()
{
    QString txt = LOC_DELETE_MESSAGE;

    //if mms and out going. check for sharing
    if((mViewFeeder->msgType() == KSenduiMtmMmsUidValue) && (!mViewFeeder->isIncoming()))
    {
        if(mViewFeeder->recipientCount() > 1 )
        {
            txt =  LOC_DELETE_SHARED_MESSAGE;
        }
    }

    HbMessageBox::question(txt,this,SLOT(onDialogDeleteMsg(int)),
                           HbMessageBox::Delete | HbMessageBox::Cancel);
}

//---------------------------------------------------------------
// UnifiedViewer::sendMessage
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::sendMessage(const QString& phoneNumber,const QString& alias)
    {
    ConvergedMessage message;
    message.setBodyText(QString());

    ConvergedMessageAddress address;
    address.setAlias(alias);
    address.setAddress(phoneNumber);
    message.addToRecipient(address);

    QByteArray dataArray;
    QDataStream messageStream
    (&dataArray, QIODevice::WriteOnly | QIODevice::Append);
    message.serialize(messageStream);

    QVariantList params;
    params << MsgBaseView::UNIEDITOR;
    params << MsgBaseView::UNIVIEWER;
    params << dataArray;

    emit switchView(params);
    }

//---------------------------------------------------------------
// UnifiedViewer::onDialogDeleteMsg
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::onDialogDeleteMsg(int val)
{
    if (val == HbMessageBox::Delete) {
        QList<int> msgIdList;
        msgIdList << mMessageId;

        ConversationsEngine::instance()->deleteMessages(msgIdList);

        QVariantList param;
        if (mMsgCount > 1) {
            param << MsgBaseView::CV;
            param << MsgBaseView::UNIVIEWER;
            param << mConversationID;
        }
        else {
            param << MsgBaseView::CLV;
            param << MsgBaseView::UNIVIEWER;
        }

        QVariant dummy(QVariant::Invalid);
        param << dummy;
        emit switchView(param);
    }
}

//---------------------------------------------------------------
// UnifiedViewer::launchEditor
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::launchEditor(
        MsgBaseView::UniEditorOperation operation)
{
    ConvergedMessage message;
    ConvergedMessageId id(mMessageId);
    message.setMessageId(id);
    if(mViewFeeder->msgType() == KSenduiMtmMmsUidValue)
    {
        message.setMessageType(ConvergedMessage::Mms);
    }
    else
    {
        message.setMessageType(ConvergedMessage::Sms);
    }

    QByteArray dataArray;
    QDataStream messageStream
    (&dataArray, QIODevice::WriteOnly | QIODevice::Append);
    message.serialize(messageStream);

    QVariantList params;
    params << MsgBaseView::UNIEDITOR; // target view
    params << MsgBaseView::UNIVIEWER; // source view

    params << dataArray;
    params << operation;

    emit switchView(params);
}

//---------------------------------------------------------------
// UnifiedViewer::call
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::call(const QString &number)
{
    QString service("phoneui");
    QString interface("com.nokia.symbian.ICallDial");
    QString operation("dial(QString)");

    XQApplicationManager appManager;
    QScopedPointer<XQAiwRequest> request(appManager.create(service, interface, operation, false));
    if (request) {
        QList<QVariant> args;
        args << number;
        request->setArguments(args);
        request->send();
    }
}

//---------------------------------------------------------------
// UnifiedViewer::isForwardOk
// @see header file
//---------------------------------------------------------------
bool UnifiedViewer::isForwardOk()
{
    bool canForwardMsg = true;
    if(mViewFeeder->msgType() == KSenduiMtmMmsUidValue)
    {
        // open DB
        RSqlDatabase sqlDb;
        TInt error = sqlDb.Open(KDbFileName);
        if(error == KErrNone)
        {
            RSqlStatement sqlSelectStmt;
            CleanupClosePushL(sqlSelectStmt);
            sqlSelectStmt.PrepareL(sqlDb,KSelectMsgPropertyStmt);
            TInt msgIdIndex = sqlSelectStmt.ParameterIndex(_L(":message_id"));
            sqlSelectStmt.BindInt(msgIdIndex, mMessageId);
            // read the flag
            TInt msgPropertyIndex =
                    sqlSelectStmt.ColumnIndex(_L("msg_property"));
            
            TInt msgProcessingStateIndex = sqlSelectStmt.ColumnIndex(
                       _L("msg_processingstate"));
            
            TInt retValue = 0;
            TInt processingStateVal = 0;
            if (sqlSelectStmt.Next() == KSqlAtRow)
            {
                retValue = static_cast<TInt>
                    (sqlSelectStmt.ColumnInt(msgPropertyIndex));
                processingStateVal = static_cast<TInt>
                    (sqlSelectStmt.ColumnInt(msgProcessingStateIndex));              
            }        
           
            CleanupStack::PopAndDestroy(&sqlSelectStmt);
            sqlDb.Close();
            
            bool processing =  (processingStateVal & EPreviewMsgProcessed)? true:false;
            if(processing)
            {
                canForwardMsg = (retValue & EPreviewForward)? true:false;
            }
            else
            {
               	// If message is still getting processed, forward field may not be updated in
               	// Preview DB, so get the forwardable state from MMS conformance checks...
                MmsConformanceCheck* mmsConformanceCheck = new MmsConformanceCheck;
                canForwardMsg = mmsConformanceCheck->validateMsgForForward(mMessageId);
                delete mmsConformanceCheck;  
            }
        }
        else
        {
            // fall-back plan
            MmsConformanceCheck* mmsConformanceCheck = new MmsConformanceCheck;
            canForwardMsg = mmsConformanceCheck->validateMsgForForward(mMessageId);
            delete mmsConformanceCheck;
        }
    }
    return canForwardMsg;
}

// EOF
