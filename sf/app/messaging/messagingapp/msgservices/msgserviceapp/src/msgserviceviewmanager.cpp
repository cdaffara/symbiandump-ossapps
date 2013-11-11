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
 * Description:  Manages differnt messaging views.
 *
 */

#include "msgserviceviewmanager.h"

#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QPixmap>
#include <HbMainWindow>
#include <HbAction>
#include <HbApplication>
#include <HbMessageBox>
#include <HbDeviceMessageBox>
#include <HbSplashScreen>

#include <xqserviceutil.h>
#include <xqappmgr.h>
#include <xqaiwdecl.h>

#include "msgunieditorview.h"
#include "unifiedviewer.h"
#include "msgstorehandler.h"
#include "msgsettingsview.h"
#include "convergedmessageid.h"
#include "ringbc.h"
#include "unidatamodelloader.h"
#include "unidatamodelplugininterface.h"

// CONSTANTS
static const char SEND_EFFECT[] = "sendeffect";
static const char SEND_EFFECT_FILE[] = ":/effects/sendeffect.fxml";

// LOCALIZATION
#define LOC_DELETED_MESSAGE hbTrId("txt_messages_dialog_message_has_been_deleted")
#define LOC_CANNOT_OPEN_MESSAGE hbTrId("txt_messages_dialog_message_is_outgoingcannot_be")
#define LOC_UNKNOWN_MSG_TYPE hbTrId("txt_messages_dialog_unsupported_message_type")
#define LOC_SAVE_RINGTONE hbTrId("txt_conversations_dialog_save_ringing_tone")

//----------------------------------------------------------------------------
// MsgViewInterface::MsgViewInterface
// @see header
//----------------------------------------------------------------------------
MsgServiceViewManager::MsgServiceViewManager(MsgStoreHandler* storeHandler,
        HbMainWindow* mainWindow, QObject* parent) :
        QObject(parent), mMainWindow(mainWindow), mUniEditor(NULL),
        mUniViewer(NULL), mSettingsView(NULL), mBackAction(NULL),
        mStoreHandler(storeHandler)
    {
    //creating back action.
    mBackAction = new HbAction(Hb::BackNaviAction, this);
    connect(mBackAction, SIGNAL(triggered()), this, SLOT(onBackAction()));
    
    // create a temp view : which is required for lazy loading of other views
    HbView* tempView = new HbView();
    mMainWindow->addView(tempView);
    mMainWindow->setCurrentView(tempView);
    }

//----------------------------------------------------------------------------
// MsgServiceViewManager::~MsgServiceViewManager
// @see header
//----------------------------------------------------------------------------
MsgServiceViewManager::~MsgServiceViewManager()
    {
  
    }

//----------------------------------------------------------------------------
// MsgServiceViewManager::onBackAction
// @see header
//----------------------------------------------------------------------------
void MsgServiceViewManager::onBackAction()
    {
    switch(mCurrentView)
        {
        case MsgBaseView::UNIEDITOR:
            {
            mUniEditor->saveContentToDrafts();
            HbApplication::quit();
            break;
            }
        case MsgBaseView::MSGSETTINGS:
            {
            //delete the settings instance
            if (mSettingsView)
            {
                mMainWindow->removeView(mSettingsView);
                delete mSettingsView;
                mSettingsView = NULL;
            }
            if(mUniEditor)
                {
                mMainWindow->setCurrentView(mUniEditor);
                mCurrentView = MsgBaseView::UNIEDITOR;
                }
            else 
                {
                ConvergedMessage message;
                QVariantList param;
                QByteArray dataArray;
                QDataStream messageStream(&dataArray, 
                        QIODevice::WriteOnly | QIODevice::Append);
                message.serialize(messageStream);
                param << dataArray;

                // switch to editor
                switchToUniEditor(param);
                }
            break;
            }
        case MsgBaseView::UNIVIEWER: 
        default:
            {
            HbApplication::quit();
            break;
            }
            
        }
    }

//----------------------------------------------------------------------------
// MsgServiceViewManager::switchView
// @see header
//----------------------------------------------------------------------------
void MsgServiceViewManager::switchView(const QVariantList& data)
    {
    int viewId = data.at(0).toInt();
    switch (viewId)
        {
        case MsgBaseView::UNIEDITOR:
            {
            // except first 2 parameters pass other parameters
            QVariantList editorData;
            for(int a = 2; a < data.length(); ++a)
                {
                editorData << data.at(a);
                }
            switchToUniEditor(editorData);
            break;
            }
        case MsgBaseView::MSGSETTINGS:
            {
            switchToMsgSettings(data);
            break;
            }
        default: 
            {
            // if send from editor is successful, then run effects
            int previousView = data.at(1).toInt();
            if(previousView == MsgBaseView::UNIEDITOR)
                {
                startAnimation(SEND_EFFECT);
                }
            else
                {
                HbApplication::quit();
                }
            }
        }
    }

//----------------------------------------------------------------------------
// MsgServiceViewManager::send
// @see header
//----------------------------------------------------------------------------
void MsgServiceViewManager::send(const QString phoneNumber, 
        const qint32 contactId, 
        const QString displayName)
    {
    Q_UNUSED(contactId);
    mMainWindow->show();
    ConvergedMessage message;
    ConvergedMessageAddress address;
    address.setAddress(phoneNumber);
    address.setAlias(displayName);
    message.addToRecipient(address);

    QVariantList param;
    QByteArray dataArray;
    QDataStream messageStream(&dataArray, 
            QIODevice::WriteOnly | QIODevice::Append);
    message.serialize(messageStream);
    param << dataArray;
    
    // switch to editor
    switchToUniEditor(param);
    
    XQServiceUtil::toBackground(false);
    }

//----------------------------------------------------------------------------
// MsgServiceViewManager::send
// @see header
//----------------------------------------------------------------------------
void MsgServiceViewManager::send(const QString phoneNumber, 
        const QString alias, 
        const QString bodyText)
    {
    mMainWindow->show();
    ConvergedMessage message;
    ConvergedMessageAddress address;
    address.setAddress(phoneNumber);
    address.setAlias(alias);
    message.addToRecipient(address);
    message.setBodyText(bodyText);

    QVariantList param;
    QByteArray dataArray;
    QDataStream messageStream(&dataArray, 
            QIODevice::WriteOnly | QIODevice::Append);
    message.serialize(messageStream);
    param << dataArray;

    // switch to editor
    switchToUniEditor(param);
    
    XQServiceUtil::toBackground(false);
    }

//----------------------------------------------------------------------------
// MsgServiceViewManager::send
// @see header
//----------------------------------------------------------------------------
void MsgServiceViewManager::send(const QVariantMap addressList, 
              const QString bodyText)
    {
    mMainWindow->show();
    QStringList phoneNumList = addressList.keys(); 
    ConvergedMessageAddressList addrList;
    int count = phoneNumList.count();
    for( int i = 0; i < count; ++ i )
        {
        QString phNum = phoneNumList[i];
        ConvergedMessageAddress* address = new ConvergedMessageAddress(phNum,
                addressList.value(phNum).toString());	
        addrList.append(address);
        }
    
    ConvergedMessage message;
    message.addToRecipients(addrList);
    message.setBodyText(bodyText);
    
    QVariantList param;
    QByteArray dataArray;
    QDataStream messageStream(&dataArray, 
            QIODevice::WriteOnly | QIODevice::Append);
    message.serialize(messageStream);
    param << dataArray;

    // switch to editor
    switchToUniEditor(param);

    XQServiceUtil::toBackground(false);
    }

//----------------------------------------------------------------------------
// MsgServiceViewManager::send
// @see header
//----------------------------------------------------------------------------
void MsgServiceViewManager::send(QVariant data)
    {
    mMainWindow->show();
    ConvergedMessage message;
    ConvergedMessageAttachmentList attachmentList;
    // handle multiple files from sendUI
    // e.g. contacts can send multiple vcards
    QStringList receivedFiles = data.toStringList();
    int recFileCount = receivedFiles.count();
    for (int i = 0; i < recFileCount; i++) {
    ConvergedMessageAttachment *attachment =
    new ConvergedMessageAttachment(receivedFiles.at(i));
    attachmentList.append(attachment);
    }
    message.addAttachments(attachmentList);

    QVariantList param;
    QByteArray dataArray;
    QDataStream messageStream(&dataArray, 
            QIODevice::WriteOnly | QIODevice::Append);
    message.serialize(messageStream);
    param << dataArray;

    // switch to editor
    switchToUniEditor(param);
    
    XQServiceUtil::toBackground(false);
    }

//----------------------------------------------------------------------------
// MsgServiceViewManager::switchToUniEditor
// @see header
//----------------------------------------------------------------------------
void MsgServiceViewManager::switchToUniEditor(const QVariantList& editorData)
{
    // construct
    if (!mUniEditor)
    {
        mUniEditor = new MsgUnifiedEditorView();
        mMainWindow->addView(mUniEditor);
        mUniEditor->setNavigationAction(mBackAction);
        connect(mUniEditor, SIGNAL(switchView(const QVariantList&)), this,
                SLOT(switchView(const QVariantList&)));
    }
    
    // populate
    mUniEditor->populateContent(editorData);
    
    // set current view as editor
    mMainWindow->setCurrentView(mUniEditor);
    mCurrentView = MsgBaseView::UNIEDITOR;
}

//----------------------------------------------------------------------------
// MsgServiceViewManager::switchToMsgSettings
// @see header
//----------------------------------------------------------------------------
void MsgServiceViewManager::switchToMsgSettings(const QVariantList& data)
{
    MsgSettingsView::SettingsView view = MsgSettingsView::DefaultView;
    
    if (mCurrentView == MsgBaseView::UNIEDITOR)
    {
        view = (MsgSettingsView::SettingsView)data.at(2).toInt();
    }

    mCurrentView = MsgBaseView::MSGSETTINGS;

    if (!mSettingsView)
    {        
        mSettingsView = new MsgSettingsView(view);
        mSettingsView->setNavigationAction(mBackAction);
        mMainWindow->addView(mSettingsView);
    }
    mMainWindow->setCurrentView(mSettingsView);
}

//----------------------------------------------------------------------------
// MsgServiceViewManager::showPopup
// @see header
//----------------------------------------------------------------------------
void MsgServiceViewManager::showPopup(const QString& dispText)
{
    HbDeviceMessageBox msgbox;
    msgbox.setMessageBoxType(HbMessageBox::MessageTypeInformation);
    msgbox.setText(dispText);
    msgbox.setAction(NULL, HbDeviceMessageBox::AcceptButtonRole);
    msgbox.exec();
    msgbox.close();
}

//----------------------------------------------------------------------------
// MsgServiceViewManager::view
// @see header
//----------------------------------------------------------------------------
void MsgServiceViewManager::view(int msgId)
{
    // Check if the message is present in store
    if(!mStoreHandler->exists(msgId))
    {
        showPopup(LOC_DELETED_MESSAGE);
        HbApplication::quit();
        return;
    }

    // Check if the given message is locked for viewing
    if(mStoreHandler->locked(msgId))
    {
        // if locked, then show info-note and exit
        showPopup(LOC_CANNOT_OPEN_MESSAGE);
        HbApplication::quit();
        return;
    }

    // Mark as read and get message type
    int msgType;
    int msgSubType;   
    mStoreHandler->markAsRead(msgId);
    mStoreHandler->getMsgTypeInfo(msgId,msgType,msgSubType);

    // Open draft message in editor
    if(mStoreHandler->isDraftMessage(msgId))
    {
        handleDraftMsg(msgId,msgType);
        return;
    }

    switch (msgType) 
    {
        case ConvergedMessage::Sms:
        case ConvergedMessage::Mms:
        {
            if (msgSubType == ConvergedMessage::VCard)
            {
                handleVCardMsg(msgId);
            }
            else
            {
                handleSmsMmsMsg(msgId);    
            }
            break;
        }
        case ConvergedMessage::MmsNotification:
        {
            handleMmsNotification(msgId);
            break;
        }
        case ConvergedMessage::BioMsg:
        {
            if (msgSubType == ConvergedMessage::RingingTone)
            {
                handleRingtoneMsg(msgId);
            }
            else if (msgSubType == ConvergedMessage::Provisioning)
            {
                handleProvisoningMsg(msgId);
            }
            else if (msgSubType == ConvergedMessage::VCard)
            {
                handleVCardMsg(msgId);
            }
            break;
        }
        case ConvergedMessage::BT:
        {
            handleBTMessage(msgId);
            break;
        }
        default:
        {
            // For all remaining unsupported messages, show delete option
            showPopup(LOC_UNKNOWN_MSG_TYPE);
            HbApplication::quit();
        }
    }
}

// ----------------------------------------------------------------------------
// MsgServiceViewManager::handleKeyEvent
// @see header
// ----------------------------------------------------------------------------
bool MsgServiceViewManager::handleKeyEvent(int key)
{
    MsgBaseView *baseView = static_cast<MsgBaseView *>(mMainWindow->currentView());
    bool eventHandled = false;
    if (baseView) {
        eventHandled = baseView->handleKeyEvent(key);
    }
    return eventHandled;
}

// ----------------------------------------------------------------------------
// MsgServiceViewManager::saveContentToDraft
// @see header
// ----------------------------------------------------------------------------
void MsgServiceViewManager::saveContentToDraft()
{
    if(mUniEditor)
    {
      mUniEditor->saveContentToDrafts();
    }
}

// ----------------------------------------------------------------------------
// MsgServiceViewManager::handleDraftMsg
// @see header
// ----------------------------------------------------------------------------
void MsgServiceViewManager::handleDraftMsg(int msgId, int msgType)
{
    // show the splash-screen
    // Note: This causes cancellation of view switching effects
//    HbSplashScreen::start();
    connect(mMainWindow, SIGNAL(viewReady()), this, SLOT(showOnViewReady()));
    mCurrentView = MsgBaseView::UNIEDITOR;

    // construct editor instance (keep it minimal)
    if (!mUniEditor)
    {
        mUniEditor = new MsgUnifiedEditorView();
        mMainWindow->addView(mUniEditor);
        mUniEditor->setNavigationAction(mBackAction);
        connect(mUniEditor, SIGNAL(switchView(const QVariantList&)), this,
                SLOT(switchView(const QVariantList&)));
    }

    // prepare data for unieditor
    ConvergedMessage message;
    ConvergedMessageId convergedMsgId = ConvergedMessageId(msgId);
    message.setMessageType((ConvergedMessage::MessageType) msgType);
    message.setMessageId(convergedMsgId);
    QByteArray dataArray;
    QDataStream messageStream(&dataArray, QIODevice::WriteOnly | QIODevice::Append);
    message.serialize(messageStream);
    QVariantList editorData;
    editorData << dataArray;

    mMainWindow->show();
    // populate data into editor
    mUniEditor->openDraftsMessage(editorData);
}

// ----------------------------------------------------------------------------
// MsgServiceViewManager::handleVCardMsg
// @see header
// ----------------------------------------------------------------------------
void MsgServiceViewManager::handleVCardMsg(int msgId)
{
    // Extract vCard filepath
    QString filepath;
    UniDataModelLoader* pluginLoader = new UniDataModelLoader();
    UniDataModelPluginInterface* pluginInterface = 
                    pluginLoader->getDataModelPlugin(ConvergedMessage::BioMsg);
    pluginInterface->setMessageId(msgId);

    // Get attachment list
    UniMessageInfoList attachments = pluginInterface->attachmentList();
    if(attachments.count() > 0)
    {
        filepath = attachments.at(0)->path();    
    }

    // Cleanup
    foreach(UniMessageInfo* attachmentInfo,attachments)
    {
        delete attachmentInfo;
    }
    delete pluginLoader;

    // copy private-vCard file to public location for contacts access
    QDir tempDir;
    QString sharedFilePath(QDir::toNativeSeparators(tempDir.tempPath()));
    sharedFilePath.append(QDir::separator());
    QFileInfo fInfo(filepath);
    sharedFilePath.append(fInfo.fileName());
    QFile::copy(filepath, sharedFilePath);

    // Launch vCard viewer service
    XQApplicationManager appManager;
    XQAiwRequest* request = appManager.create(XQI_CONTACTS_EDIT,
                                XQOP_CONTACTS_EDIT_CREATE_NEW_VCARD, 
                                true); //embedded
    if(request)
    {
        QList<QVariant> args;
        args << sharedFilePath;
        request->setArguments(args);
        QVariant retValue;
        bool res = request->send(retValue);
        delete request;
    }

    // Delete shared file now
    QFile::remove(sharedFilePath);
    HbApplication::quit();
}

// ----------------------------------------------------------------------------
// MsgServiceViewManager::handleSmsMmsMsg
// @see header
// ----------------------------------------------------------------------------
void MsgServiceViewManager::handleSmsMmsMsg(int msgId)
{
    // show the splash-screen
    // Note: This causes cancellation of view switching effects
//    HbSplashScreen::start();
    connect(mMainWindow, SIGNAL(viewReady()), this, SLOT(showOnViewReady()));
    mCurrentView = MsgBaseView::UNIVIEWER;

    // construct viewer instance (keep it minimal)
    if(!mUniViewer)
    {
        mUniViewer = new UnifiedViewer(msgId);
        mMainWindow->addView(mUniViewer);
        mUniViewer->setNavigationAction(mBackAction);
        connect(mUniViewer, SIGNAL(switchView(const QVariantList&)),
                        this, SLOT(switchView(const QVariantList&)));
    }

    // populate the viewer
    mUniViewer->populateContent(msgId, true, 1);
    mMainWindow->show();
}

// ----------------------------------------------------------------------------
// MsgServiceViewManager::handleMmsNotification
// @see header
// ----------------------------------------------------------------------------
void MsgServiceViewManager::handleMmsNotification(int msgId)
{
    // set context to current entry
    if( KErrNone != mStoreHandler->setNotificationMessageId(msgId))
    {
        return;
    }

    // notification state e.g. waiting, retrieving etc
    QString displayTxt;
    QString statusStr;
    int status;
    mStoreHandler->notificationStatus(status, statusStr);
    if(!statusStr.isEmpty())
    {
        displayTxt.append(statusStr).append(QChar::LineSeparator);
    }
    
    QString sender = mStoreHandler->notificationSender();
    if(!sender.isEmpty())
    {
        displayTxt.append(sender).append(QChar::LineSeparator);
    }
    
    QString timestamp = mStoreHandler->notificationTimeStamp();
    if(!timestamp.isEmpty())
    {
        displayTxt.append(timestamp).append(QChar::LineSeparator);
    }

    QString subject = mStoreHandler->notificationSubject();
    if(!subject.isEmpty())
    {
        displayTxt.append(subject).append(QChar::LineSeparator);
    }

    QString msgSize = mStoreHandler->notificationMsgSize();
    if(!msgSize.isEmpty())
    {
        displayTxt.append(msgSize).append(QChar::LineSeparator);
    }

    QString msgClass = mStoreHandler->notificationClass();
    if(!msgClass.isEmpty())
    {
        displayTxt.append(msgClass).append(QChar::LineSeparator);
    }

    QString exprDate = mStoreHandler->notificationExpiryDate();
    if(!exprDate.isEmpty())
    {
        displayTxt.append(exprDate);
    }

    HbDeviceMessageBox msgbox;
    msgbox.setText(displayTxt);
    msgbox.setDismissPolicy(HbPopup::NoDismiss);
    msgbox.setTimeout(HbPopup::NoTimeout);
    msgbox.exec();
    msgbox.close();
    HbApplication::quit();
}

// ----------------------------------------------------------------------------
// MsgServiceViewManager::showOnViewReady
// @see header
// ----------------------------------------------------------------------------
void MsgServiceViewManager::showOnViewReady()
{
    switch(mCurrentView)
    {
        case MsgBaseView::UNIVIEWER:
        {
            mMainWindow->setCurrentView(mUniViewer);
            break;
        }
        case MsgBaseView::UNIEDITOR:
        {
            mMainWindow->setCurrentView(mUniEditor);
            break;
        }
        default:
            break;
    }
    disconnect(mMainWindow, SIGNAL(viewReady()), this, SLOT(showOnViewReady()));
}

// ----------------------------------------------------------------------------
// MsgServiceViewManager::handleRingtoneMsg
// @see header
// ----------------------------------------------------------------------------
void MsgServiceViewManager::handleRingtoneMsg(int msgId)
{
    // Extract rintone filepath
    UniDataModelLoader* pluginLoader = new UniDataModelLoader();
    UniDataModelPluginInterface* pluginInterface = 
                        pluginLoader->getDataModelPlugin(ConvergedMessage::BioMsg);
    pluginInterface->setMessageId(msgId);

    // Get attachment list
    UniMessageInfoList attachments = pluginInterface->attachmentList();
    QString filepath;
    if(attachments.count() > 0)
    {
        filepath = attachments.at(0)->path();
    }

    // Cleanup
    foreach(UniMessageInfo* attachmentInfo,attachments)
    {
        delete attachmentInfo;
    }
    delete pluginLoader;

    RingBc* ringBc = new RingBc();
    QString filename = ringBc->toneTitle(filepath);
    QString loc_str = LOC_SAVE_RINGTONE.arg(filename);
    bool save = HbDeviceMessageBox::question(loc_str,
                    HbMessageBox::Save | HbMessageBox::Cancel);
    if(save)
    {
        ringBc->saveTone(filepath);
    }
    delete ringBc;
    HbApplication::quit();
}

// ----------------------------------------------------------------------------
// MsgServiceViewManager::handleProvisoningMsg
// @see header
// ----------------------------------------------------------------------------
void MsgServiceViewManager::handleProvisoningMsg(int msgId)
    {
    QString messageId;
    messageId.setNum(msgId);

    XQApplicationManager* aiwMgr = new XQApplicationManager();

    XQAiwRequest* request = aiwMgr->create("com.nokia.services.MDM", 
            "Provisioning",
            "ProcessMessage(QString)", true); // embedded

    if (request) {
    QList<QVariant> args;
    args << QVariant(messageId);
    request->setArguments(args);

    // Send the request
    bool res = request->send();

    // Cleanup
    delete request;
    }

    delete aiwMgr;

    // close the application once its handled
    HbApplication::quit();
    }

//-----------------------------------------------------------------------------
//MsgServiceViewManager::handleBTMessage()
//@see header
//-----------------------------------------------------------------------------
void MsgServiceViewManager::handleBTMessage(int msgId)
    {
    XQApplicationManager* aiwMgr = new XQApplicationManager();
    XQAiwRequest* request = 
    aiwMgr->create("com.nokia.services.btmsgdispservices", "displaymsg",
            "displaymsg(int)", true); // embedded

    if (request) {
    QList<QVariant> args;
    args << QVariant(msgId);
    request->setArguments(args);

    // Send the request
    bool res = request->send();

    // Cleanup
    delete request;
    }

    delete aiwMgr;

    // close the application once its handled
    HbApplication::quit();
    }

//-----------------------------------------------------------------------------
//MsgServiceViewManager::startAnimation
//@see header
//-----------------------------------------------------------------------------
void MsgServiceViewManager::startAnimation(QString effectEvent)
    {
    // take screen shot
    QGraphicsPixmapItem *animationScreenShot = screenShot();
    if (animationScreenShot)
        {
        // but don't show it yet.
        animationScreenShot->hide();
        animationScreenShot->setPos(0,0);
        animationScreenShot->setZValue(0);

        // hide items, so that background app's items are visible immediately
        mMainWindow->currentView()->hideItems(Hb::AllItems);
        
        // reset background & set the base transparent
        mMainWindow->setBackgroundImageName(
                mMainWindow->orientation(), QString("dummy_blank"));
        QPalette p = mMainWindow->viewport()->palette();
        p.setColor(QPalette::Base, Qt::transparent);
        mMainWindow->viewport()->setPalette(p);

        // add animating item directly to the scene
        mMainWindow->scene()->addItem(animationScreenShot);
        
        // hide other views
        QList<HbView*> vws = mMainWindow->views();
        while (!vws.isEmpty())
            {
            HbView* view = vws.takeLast();
            view->hide();
            }
        
        // now show the animating item, and start animation on it
        animationScreenShot->show();
        QString effectFile = getAnimationFile(effectEvent);
        HbEffect::add(animationScreenShot, effectFile, effectEvent);
        HbEffect::start(animationScreenShot, effectEvent, this,
                "onAnimationComplete");
        }
    }

//-----------------------------------------------------------------------------
//MsgServiceViewManager::resetAnimation
//@see header
//-----------------------------------------------------------------------------
void MsgServiceViewManager::resetAnimation(
        QString effectEvent,
        QGraphicsItem* item)
    {
    if (item) 
        {
        QString effectFile = getAnimationFile(effectEvent);
        HbEffect::remove(item, effectFile, effectEvent);
        mMainWindow->scene()->removeItem(item);
        delete item;
        item = NULL;
        }
    }

//-----------------------------------------------------------------------------
//MsgServiceViewManager::onAnimationComplete
//@see header
//-----------------------------------------------------------------------------
void MsgServiceViewManager::onAnimationComplete(
        const HbEffect::EffectStatus &status)
    {
    QGraphicsItem* item = status.item;
    QString effectEvent = status.effectEvent;
    resetAnimation(effectEvent, item);
    HbApplication::quit();
    }

//-----------------------------------------------------------------------------
//MsgServiceViewManager::screenShot
//@see header
//-----------------------------------------------------------------------------
QGraphicsPixmapItem* MsgServiceViewManager::screenShot()
    {
    // set fullscreen and hide unwanted items
    mMainWindow->currentView()->hideItems(Hb::ToolBarItem | Hb::DockWidgetItem | Hb::StatusBarItem);
    mMainWindow->currentView()->setContentFullScreen(true);

    // grab whole view into pixmap image
    QPixmap screenCapture = QPixmap::grabWindow(mMainWindow->internalWinId());

    // create an QGraphicsItem to do animation
    QGraphicsPixmapItem *ret(NULL);

    // for landscape, the screenshot must be rotated
    if(mMainWindow->orientation() == Qt::Horizontal)
        {
        QMatrix mat;
        mat.rotate(-90); // rotate 90 degrees counter-clockwise
        ret = new QGraphicsPixmapItem(screenCapture.transformed(mat));
        }
    else
        {
        ret = new QGraphicsPixmapItem(screenCapture);
        }
    return ret;
    }

//-----------------------------------------------------------------------------
//MsgServiceViewManager::getAnimationFile
//@see header
//-----------------------------------------------------------------------------
QString MsgServiceViewManager::getAnimationFile(QString effectEvent)
    {
    QString animFile;
    if(effectEvent == SEND_EFFECT)
        {
        animFile.append(SEND_EFFECT_FILE);
        }

    return animFile;
    }

//-----------------------------------------------------------------------------
//MsgServiceViewManager::send
//@see header
//-----------------------------------------------------------------------------

void MsgServiceViewManager::send(ConvergedMessage message)
    {
    mMainWindow->show();    
    QVariantList param;
    QByteArray dataArray;
    QDataStream messageStream(&dataArray, QIODevice::WriteOnly | QIODevice::Append);
    message.serialize(messageStream);
    param << dataArray;

    // switch to editor
    switchToUniEditor(param);
    
    XQServiceUtil::toBackground(false);    
    }
	
	
	
