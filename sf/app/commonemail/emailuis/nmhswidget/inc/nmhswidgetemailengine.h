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

#ifndef NMHSWIDGETEMAILENGINE_H_
#define NMHSWIDGETEMAILENGINE_H_

#include <QObject>
#include "nmcommon.h"

class NmMessageEnvelope;
class NmDataPluginFactory;
class NmDataPluginInterface;
class QPluginLoader;
class QTimer;
class XQAiwRequest;

//Three seconds
const int NmHsWidgetEmailEngineUpdateTimerValue = 3000;

enum NmHsWidgetEmailEngineExceptionCode
    {
    NmEngineExcFailure,
    NmEngineExcAccountDeleted
    };

class NmHsWidgetEmailEngine : public QObject
    {
    Q_OBJECT
    
public:
    NmHsWidgetEmailEngine( const NmId& monitoredMailboxId );
    bool initialize(); 
    ~NmHsWidgetEmailEngine();

    int getEnvelopes(QList<NmMessageEnvelope*> &list, int maxEnvelopeAmount);
    int unreadCount();
    QString accountName();
    void deleteAiwRequest();
        
public slots:
    void handleMessageEvent( 
            NmMessageEvent event,
            const NmId &folderId,
            const QList<NmId> &messageIds,
            const NmId& mailboxId);

    void handleFolderEvent(
                NmFolderEvent event,
                const QList<NmId> &folderIds,
                const NmId& mailboxId);

    
    void handleMailboxEvent(NmMailboxEvent event, const QList<NmId> &mailboxIds);
   
    //Activity control
    void suspend();
    void activate();
    void launchMailAppInboxView();
    void launchMailAppMailViewer(const NmId &messageId);
    void handleUpdateTimeout();
    
    void aiwRequestOk(const QVariant& result);
    void aiwRequestError(int errorCode, const QString& errorMessage);
    
signals:
    void mailDataChanged();    
    void accountNameChanged(const QString& accountName);
    void unreadCountChanged(const int& unreadCount);
    void exceptionOccured(const int& err);
    
private:
    bool constructNmPlugin();
    bool updateData(); 
    bool updateAccount();
    void resetEnvelopeList();
    
private:
    NmId mMailboxId;
    NmId mFolderId;
    QString mAccountName;
    int mUnreadCount;
    QList<NmMessageEnvelope*> mEnvelopeList;
    NmDataPluginInterface *mEmailInterface;
    NmDataPluginFactory* mFactory;
    //suspension variables
    bool mAccountEventReceivedWhenSuspended;
    bool mMessageEventReceivedWhenSuspended;
    bool mSuspended; 
    QTimer* mUpdateTimer;
    XQAiwRequest* mAiwRequest;
    };

#endif /* NMHSWIDGETEMAILENGINE_H_ */
