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

#ifndef NMMAILAGENT_H
#define NMMAILAGENT_H

#include <nmcommon.h>
#include <nmuieventsnotifier.h>

class CHWRMVibra;
class NmMailbox;
class NmDataPluginFactory;
class NmDataPluginInterface;
class HbIndicator;
class XQSettingsManager;
class XQSettingsKey;


class NmMailboxInfo
{
public:
    NmId mId;
    int mIndicatorIndex;
    QString mName;
    QString mIconName;
    NmId mInboxFolderId;
    NmId mOutboxFolderId;
    NmSyncState mSyncState;
    NmConnectState mConnectState;
    int mInboxCreatedMessages;
    int mInboxChangedMessages;
    int mInboxDeletedMessages;
    QList<NmId> mMailIdList; // all messages
    QList<NmId> mNewUnreadMailIdList; // new unread messages
    int mOutboxMails;
    bool mActive;

    NmMailboxInfo();
};


class NmMailAgent : public QObject
{
    Q_OBJECT

public:

    NmMailAgent();

    ~NmMailAgent();

    bool init();

public slots:

    void handleMailboxEvent(NmMailboxEvent event,
            const QList<NmId> &mailboxIds);

    void handleMessageEvent(
            NmMessageEvent event,
            const NmId &folderId,
            const QList<NmId> &messageIds,
            const NmId &mailboxId);

    void handleSyncStateEvent(
            NmSyncState state,
            const NmOperationCompletionEvent &event);

    void handleConnectionEvent(NmConnectState state, const NmId mailboxId, int errorcode);

    void delayedStart();

    void enableAlertTone();

    void indicatorActivated(const QString &type, const QVariantMap &data);

    void valueChanged(const XQSettingsKey &key, const QVariant &value);

    void delayedMailboxCreated();
    
    void handleViewStateChangedEvent(const NmUiEventsNotifier::NmUiEventType eventType,
                                     const NmUiViewId viewId,
                                     const NmId mailboxId);

private:

    void initMailboxStatus();

    bool updateUnreadCount(const NmId &mailboxId, NmMailboxInfo &mailboxInfo);

    int getOutboxCount(const NmId &mailboxId, const NmId &outboxId);

    NmMailboxInfo *getMailboxByType(const QString &type);

    int getFreeIndicatorIndex();

    int getTotalUnreadCount() const;

    bool updateUnreadIndicator();

    bool updateUnreadIndicator(bool active);

    bool updateIndicator(bool active,
        const NmMailboxInfo& mailboxInfo);

    NmMailboxInfo *getMailboxInfo(const NmId &id);

    NmMailboxInfo *createMailboxInfo(const NmId &id);

    NmMailboxInfo *createMailboxInfo(const NmMailbox &mailbox,
        NmDataPluginInterface *plugin);

    bool removeMailboxInfo(const NmId &id);

    bool updateMailboxState(const NmId &mailboxId);
    
    bool updateMailboxState(const NmId &mailboxId,
        bool active, bool refreshAlways);

    static QStringList pluginFolders();

    bool getMessageUnreadInfo(const NmId &folderId,
        const NmId &messageId, const NmId &mailboxId, bool &unreadMessage);

    bool playAlertTone();

    void updateSendIndicator();
    
    void resetMailboxState(NmMailboxInfo *info); 

    void storeMailboxActive(const NmId &mailboxId, bool active);

    bool isMailboxActive(const NmId &mailboxId);

    void deleteStoredMailboxActivity(const NmId &mailboxId);

    bool launchMailbox(quint64 mailboxId);

    void handleMessageCreatedEvent(const NmId &folderId, const QList<NmId> &messageIds,
        const NmId &mailboxId);

    void handleMessageChangedEvent(const NmId &folderId, const QList<NmId> &messageIds,
        const NmId &mailboxId);

    void handleMessageDeletedEvent(const NmId &folderId, const QList<NmId> &messageIds,
        const NmId &mailboxId);

private: // data

    HbIndicator *mIndicator; // Owned;
    NmDataPluginFactory *mPluginFactory; // Not owned (singleton).
    QList<NmMailboxInfo*> mMailboxes;
    CHWRMVibra *mVibra; // Owned.
    XQSettingsManager *mSettingManager; // Owned.
    NmUiEventsNotifier *mUiEventsNotifier; // Owned.
    bool mAlertToneAllowed;
    int mLastOutboxCount;
    bool mUnreadIndicatorActive;
    int mSilenceMode;
    NmId mNewMailboxId;
};


#endif // NMMAILAGENT_H
