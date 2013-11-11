/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QTimer>
#include <QDir>
#include <qpluginloader.h>
#include <xqapplicationmanager.h>
#include <xqaiwdeclplat.h>
#include <email_services_api.h>
#include "nmcommon.h"
#include "nmmessageenvelope.h"
#include "nmhswidgetemailengine.h"
#include "nmdataplugininterface.h"
#include "nmmailbox.h"
#include "nmfolder.h"
#include "nmdatapluginfactory.h"
#include "nmhswidgetconsts.h"
#include "emailtrace.h"

/*!
 Constructor
 */
NmHsWidgetEmailEngine::NmHsWidgetEmailEngine(const NmId& monitoredMailboxId) :
    mMailboxId(monitoredMailboxId),
    mFolderId(0),
    mAccountName(),
    mUnreadCount(-1),
    mEmailInterface(0),
    mFactory(0),
    mAccountEventReceivedWhenSuspended(false),
    mMessageEventReceivedWhenSuspended(false),
    mSuspended(false),
    mUpdateTimer(0),
    mAiwRequest(0)
{
    NM_FUNCTION;
}

/*!
 initilaize - Initializes the engine
 Works as a second phase constructor. Must be called before any other operations.
 Client should connect to exceptionOccured signal before calling this method.
 /return true if initialize succeedes otherwise false. If false the object is unusable.
 */
bool NmHsWidgetEmailEngine::initialize()
{
    NM_FUNCTION;

    if (!constructNmPlugin()) {
        //if plugin connection fails, there's no reason to proceed
        return false;
    }
    updateData();
    updateAccount();

    mUpdateTimer = new QTimer(this);
    mUpdateTimer->setInterval(NmHsWidgetEmailEngineUpdateTimerValue);
    connect(mUpdateTimer, SIGNAL(timeout()), this, SLOT(handleUpdateTimeout()) );

    return true;
}

/*!
 constructNmPlugin
 /post Constructs a connection to an instance of NmDataPluginInterface
 /return true if construction succeeded, otherwise false.
 */
bool NmHsWidgetEmailEngine::constructNmPlugin()
{
    NM_FUNCTION;

    QObject* pluginInstance(0);
    //Get data plugin factory instance
    mFactory = NmDataPluginFactory::instance();
    if (!mFactory) {
        NM_ERROR(1,"NmHsWidgetEmailEngine::constructNmPlugin() -- mFactory FAILED");
        return false;
    }

    //Get plugin instance
    pluginInstance = mFactory->pluginInstance(mMailboxId);
    if (!pluginInstance) {
        NM_ERROR(1,"NmHsWidgetEmailEngine::constructNmPlugin() -- pluginInstance FAILED");
        return false;
    }

    //get interface instance
    mEmailInterface = mFactory->interfaceInstance(pluginInstance);

    if (!mEmailInterface) {
        NM_ERROR(1,"NmHsWidgetEmailEngine::constructNmPlugin() -- mEmailInterface FAILED");
        return false;
    }
    //Verify that the mailbox we are interested actually exists.
    //Otherwise emit account deleted event (instead of just returning)
    QList<NmId> ids;
    mEmailInterface->listMailboxIds(ids);
    if(!ids.contains(mMailboxId)){
        NM_ERROR(1,"NmHsWidgetEmailEngine::constructNmPlugin() -- !ids.contains(mMailboxId) FAILED");
        emit exceptionOccured(NmEngineExcAccountDeleted);
        return false;
    }

    //retrieve folderID for this mailbox's inbox
    //If NmId equals zero, we don't have the Initial sync done for the account
    //This is valid at least for IMAP accounts.
    //Folder ID is then retrieved later when first message event is received
    mFolderId = mEmailInterface->getStandardFolderId(mMailboxId, NmFolderInbox);

    //Subscription is needed - otherwise the signals will not be received
    mEmailInterface->subscribeMailboxEvents(mMailboxId);

    //Connect to events
    connect(pluginInstance, SIGNAL( messageEvent(NmMessageEvent, const NmId&,
            const QList<NmId>&, const NmId&) ), this,
        SLOT( handleMessageEvent(NmMessageEvent, const NmId&,
                const QList<NmId>&, const NmId&) ));

    connect( pluginInstance, SIGNAL( folderEvent(
             NmFolderEvent, const QList<NmId>&, const NmId&) ), this,
             SLOT( handleFolderEvent( NmFolderEvent, const QList<NmId>&, const NmId&)));

    connect(pluginInstance, SIGNAL( mailboxEvent(NmMailboxEvent, const QList<NmId>& ) ), this,
        SLOT( handleMailboxEvent(NmMailboxEvent, const QList<NmId>&) ));

    return true;
}

/*!
 Reset envelope list
 \post mEnvelopeList.isEmpty() == true && all contained objects are deleted
 */
void NmHsWidgetEmailEngine::resetEnvelopeList()
{
    NM_FUNCTION;

    while (!mEnvelopeList.isEmpty()) {
        delete mEnvelopeList.takeFirst();
    }
}

/*!
 Destructor
 */
NmHsWidgetEmailEngine::~NmHsWidgetEmailEngine()
{
    NM_FUNCTION;

    resetEnvelopeList();
    if (mFactory) {
        NmDataPluginFactory::releaseInstance(mFactory);
    }

    if (mUpdateTimer){
        mUpdateTimer->stop();
        delete mUpdateTimer;
    }
}

/*!
 getEnvelopes() provides message envelopes as a list of stack objects
 Amount of message envelopes in the list parameter is the smallest of the following factors:
 'KMaxNumberOfEnvelopesProvided', 'maxEnvelopeAmount', 'amount of available envelopes'.

 \param list list to be filled with message envelopes
 \param maxEnvelopeAmount Client side limit for amount of message envelope count.
 \return count of envelopes added to list
 */
int NmHsWidgetEmailEngine::getEnvelopes(QList<NmMessageEnvelope*> &list, int maxEnvelopeAmount)
{
    NM_FUNCTION;
    list.clear(); //Reset the parameter list to avoid side effects
    list.append(mEnvelopeList.mid(0, maxEnvelopeAmount));
    return list.count();
}

/*!
 UnreadCount

 \return count of unread mails, or -1 if there are no mails at all in the mailbox.
 */
int NmHsWidgetEmailEngine::unreadCount()
{
    NM_FUNCTION;
    return mUnreadCount;
}

/*!
 accountName

 \return name of the monitored account
 */
QString NmHsWidgetEmailEngine::accountName()
{
    NM_FUNCTION;
    return mAccountName;
}

/*!
 Refresh email data.
 \post mEnvelopeList is refreshed with valid content so that it has
       valid data with maximum of KMaxNumberOfEnvelopesProvided envelopes.

 - emits exceptionOccured(NmEngineExcFailure) if fatal error occurs.
 - emits mailDataChanged() if new mail data is set into mEnvelopeList
 - emits unreadCountChanged(mUnreadCount), if mUnreadCount is updated

 \return true if everything succeeded, otherwise false
 */
bool NmHsWidgetEmailEngine::updateData()
{
    NM_FUNCTION;
    if (!mEmailInterface) {
        NM_ERROR(1,"NmHsWidgetEmailEngine::updateData() -- Interface missing");
        emit exceptionOccured(NmEngineExcFailure); //fatal error
        return false; //if interface is missing there's nothing to do
    }

    //reset envelope list before retrieving new items
    resetEnvelopeList();

    //get messages from inbox
    int msgErr = mEmailInterface->listMessages(mMailboxId, mFolderId, mEnvelopeList,
        KMaxNumberOfEnvelopesProvided);
    if (msgErr) {
        //retrieval of messages failed.
        return false;
    }
    //emit signal about new message data right away
    emit mailDataChanged();
    //retrieve new unread count to mUnreadCount
    NmFolder* folder = NULL;
    int folderErr = mEmailInterface->getFolderById(mMailboxId, mFolderId, folder);
    if (folderErr) {
        //retrieval of valid folder failed.
        return false;
    }
    if (folder) {
        //If messageCount in the folder is zero we must indicate unread count to be -1
        if (mEnvelopeList.count() == 0) {
            mUnreadCount = -1;
        }
        else {
            mUnreadCount = folder->unreadMessageCount();
        }
        delete folder;
        folder = NULL;
        //limit the unread count to KMaxUnreadCount
        if (mUnreadCount > KMaxUnreadCount) {
            mUnreadCount = KMaxUnreadCount;
        }
        //emit signal about changed unread count
        emit unreadCountChanged(mUnreadCount);
    }else{
        return false;
    }
    return true;
}

/*!
 handleMessageEvent slot.
 */
void NmHsWidgetEmailEngine::handleMessageEvent(
    NmMessageEvent event,
    const NmId &folderId,
    const QList<NmId> &messageIds,
    const NmId& mailboxId)
{
    NM_FUNCTION;
    Q_UNUSED(event);
    Q_UNUSED(messageIds);

    if (!mEmailInterface) {
        NM_ERROR(1,"NmHsWidgetEmailEngine::handleMessageEvent() -- Interface missing");
        emit exceptionOccured(NmEngineExcFailure); //fatal error
        return; //if interface is missing there's nothing to do
    }

    if (mFolderId == NmId(0)) {
        // inbox was not created in construction phase or the folder was deleted in some point,
        //so let's get the id now as we received first mail event
        mFolderId = mEmailInterface->getStandardFolderId(mMailboxId, NmFolderInbox);
    }
    if ((folderId == mFolderId) && (mailboxId == mMailboxId)) {
        //Data is updated only if the engine is not suspended
        if (mSuspended) {
            mMessageEventReceivedWhenSuspended = true;
        }
        else {
            //start or restart the timer. Update is started when timer expires
            mUpdateTimer->start();
        }
    }
}

/*!
 handleFolderEvent slot.
 If the monitored folder is deleted, clear the envelope list and unread count.
 Also the mFolderId is set to 0 to indicate the current state so that in device boot up
 we don't query any messages. When the first message event arrives, the folder ID must be
 retrieved again.
 */
void NmHsWidgetEmailEngine::handleFolderEvent( NmFolderEvent event,
    const QList<NmId> &folderIds, const NmId& mailboxId)
{
    NM_FUNCTION;
    //react only if the monitored folder for monitored account is deleted
    if (event == NmFolderIsDeleted && mailboxId == mMailboxId && folderIds.contains(mFolderId) )
    {
        resetEnvelopeList();    //cached envelopes to be cleared
        mUnreadCount = 0;       //unread count to 0
        mFolderId = NmId(0);    //folder id to zero (indicates the situation where folder is not available)
        emit mailDataChanged(); //emit data change for UI
        emit unreadCountChanged(mUnreadCount); // emit unread count change to UI
    }
}


/*!
 handleMailboxEvent slot.
 */
void NmHsWidgetEmailEngine::handleMailboxEvent(NmMailboxEvent event, const QList<NmId> &mailboxIds)
{
    NM_FUNCTION;
    if (mailboxIds.contains(mMailboxId)) {
        switch (event) {
            case (NmMailboxChanged): {
                if (mSuspended) {
                    mAccountEventReceivedWhenSuspended = true;
                }
                else {
                    updateAccount();
                }
                break;
            }
            case (NmMailboxDeleted): {
                emit exceptionOccured(NmEngineExcAccountDeleted);
                break;
            }
            default:
                //ignored
                break;
        }
    }
}

/*!
 * handleUpdateTimeout slot
 */
void NmHsWidgetEmailEngine::handleUpdateTimeout()
{
    NM_FUNCTION;
    if (mUpdateTimer){
        mUpdateTimer->stop();
    }
    updateData();
}

/*!
 Update Account data
 \post if mEmailInterface exists, the mAccountName is refreshed from adapter
 and accountNameChanged signal is emitted.
 */
bool NmHsWidgetEmailEngine::updateAccount()
{
    NM_FUNCTION;

    NmMailbox* box = NULL;
    if (mEmailInterface) {
        int err = mEmailInterface->getMailboxById(mMailboxId, box);
        if (err) {
            //By specification return and wait another event
            return false;
        }
    }
    if (box) {
        mAccountName = box->name();
        delete box;
        box = NULL;
        emit accountNameChanged(mAccountName);
    }
    return true;
}

/*!
 suspend slot.
 \post engine will not emit signals or refresh its data during suspension.
 */
void NmHsWidgetEmailEngine::suspend()
{
    NM_FUNCTION;
    mSuspended = true;
}

/*!
 activate slot.
 \post Engine will immediately refresh all the data that has been announced to
 have changed during the suspension. Events are enabled.
 */
void NmHsWidgetEmailEngine::activate()
{
    NM_FUNCTION;
    mSuspended = false;
    if (mAccountEventReceivedWhenSuspended) {
        mAccountEventReceivedWhenSuspended = false;
        updateAccount();
    }
    if (mMessageEventReceivedWhenSuspended) {
        mMessageEventReceivedWhenSuspended = false;
        updateData();
    }
}

/*!
 launchMailAppInboxView slot.
 \post Mail application is launched to inbox view corresponding widget's mailbox id
 */
void NmHsWidgetEmailEngine::launchMailAppInboxView()
{
    NM_FUNCTION;
    NM_TIMESTAMP("Launch mail application from widget.");

    QT_TRY{
        if (!mAiwRequest) {
            XQApplicationManager appManager;
            mAiwRequest = appManager.create(
                    XQI_EMAIL_INBOX_VIEW, XQOP_EMAIL_INBOX_VIEW,
                    false);

            if (mAiwRequest) {
                connect(mAiwRequest, SIGNAL( requestError(int, const QString&) ),
                        this, SLOT( aiwRequestError(int, const QString&) ));
                connect(mAiwRequest, SIGNAL( requestOk(const QVariant&) ),
                        this, SLOT( aiwRequestOk(const QVariant&) ));
                QList<QVariant> list;
                list.append(QVariant(mMailboxId.id()));

                mAiwRequest->setSynchronous(false);
                mAiwRequest->setArguments(list);
                mAiwRequest->send();
            }
        }
    }
    QT_CATCH(...){
        // no actions taken.
        // try-catch mechanism added to avoid crashing widget, in case XQAiwRequest
        // creation raise exception.
    }
}

/*!
 launchMailAppMailViewer slot.
 \param messageId Defines the message opened to viewer
 \post Mail application is launched and viewing mail specified by
 */
void NmHsWidgetEmailEngine::launchMailAppMailViewer(const NmId &messageId)
{
    NM_FUNCTION;
    NM_TIMESTAMP("Launch mail viewer from widget.");

    QT_TRY{
        if (!mAiwRequest) {
            XQApplicationManager appManager;
            mAiwRequest = appManager.create(
                    XQI_EMAIL_MESSAGE_VIEW, "viewMessage(QVariant,QVariant)",
					//XQOP_EMAIL_MESSAGE_VIEW,
                    false);

            if (mAiwRequest) {
                connect(mAiwRequest, SIGNAL( requestError(int, const QString&) ),
                        this, SLOT( aiwRequestError(int, const QString&) ));
                connect(mAiwRequest, SIGNAL( requestOk(const QVariant&) ),
                        this, SLOT( aiwRequestOk(const QVariant&) ));

                QVariantList messageIdList;
                messageIdList.append(mMailboxId.id());
                messageIdList.append(mFolderId.id());
                messageIdList.append(messageId.id());

                QList<QVariant> argList;
                argList.append(QVariant(messageIdList));
                argList.append(quint64(EmailBackReturnsToMessageList));

                mAiwRequest->setSynchronous(false);
                mAiwRequest->setArguments(argList);
                mAiwRequest->send();
            }
        }
    }
    QT_CATCH(...){
        // no actions taken.
        // try-catch mechanism added to avoid crashing widget, in case XQAiwRequest
        // creation raise exception.
    }
}

/*!
 aiwRequestOk slot.
 */
void NmHsWidgetEmailEngine::aiwRequestOk(const QVariant& result)
{
    NM_FUNCTION;

    Q_UNUSED(result);

    deleteAiwRequest();
}

/*!
 aiwRequestError slot.
 */
void NmHsWidgetEmailEngine::aiwRequestError(int errorCode, const QString& errorMessage)
{
    NM_FUNCTION;

    Q_UNUSED(errorCode);
    Q_UNUSED(errorMessage);

    deleteAiwRequest();
}

/*!
 deleteAiwRequest function.
 */
void NmHsWidgetEmailEngine::deleteAiwRequest()
{
    NM_FUNCTION;

    delete mAiwRequest;
    mAiwRequest = NULL;
}

