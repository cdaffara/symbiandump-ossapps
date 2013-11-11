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
 *     Email message related operations
 */

#ifndef NMAPIMESSAGEMANAGER_H_
#define NMAPIMESSAGEMANAGER_H_

#include <QObject>
#include <QPointer>
#include <QList>
#include <QVariant>

#include <nmapidef.h>
#include <nmapioperation.h>
#include <nmapimessage.h>

namespace EmailClientApi {

class NmApiMessageManagerPrivate;

class NMAPI_EXPORT NmApiMessageManager : public QObject
{
    Q_OBJECT
public:
    NmApiMessageManager(QObject *parent,quint64 mailboxId);

    virtual ~NmApiMessageManager();

public slots: 
    QPointer<NmApiOperation> createDraftMessage(const QVariant *initData);
    
    QPointer<NmApiOperation> createForwardMessage(NmApiMessage *orig,const QVariant *initData);
    
    QPointer<NmApiOperation> createReplyMessage(const NmApiMessage *orig,const QVariant *initData,bool replyAll);
    
    QPointer<NmApiOperation> moveMessages(const QList<quint64> messageIds,quint64 sourceFolderId,quint64 targetFolderId);
    
    QPointer<NmApiOperation> saveMessage(const NmApiMessage &message);
    
    QPointer<NmApiOperation> deleteMessages(const QList<quint64> messageIds);
    
    QPointer<NmApiOperation> fetchMessage(quint64 messageId);
    
    QPointer<NmApiOperation> sendMessage(const NmApiMessage &message);
    
    QPointer<NmApiOperation> createAttachment(NmApiMessage &message,const QVariant &attachmenSpec);
    
    QPointer<NmApiOperation> removeAttachment(NmApiMessage &message,quint64 attachmentId);
    
    QPointer<NmApiOperation> fetchAttachment(const NmApiMessage &relatedMessage,quint64 attachmentId);

private:
    NmApiMessageManagerPrivate* d;
};

} // namespace EmailClientApi
#endif /*NMAPIMESSAGEMANAGER_H_ */
