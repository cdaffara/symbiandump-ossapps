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
* Description:
*
*/


#ifndef NMVIEWERVIEWNETREPLY_H_
#define NMVIEWERVIEWNETREPLY_H_

#include <QNetworkReply>
#include "nmcommon.h"

class NmOperation;
class NmMessage;
class NmUiEngine;

class NmViewerViewNetReply : public QNetworkReply
{
    Q_OBJECT
public:
    NmViewerViewNetReply(QVariant data, NmUiEngine &uiEngine);
    NmViewerViewNetReply(QVariant data, NmUiEngine &uiEngine, 
            const NmId &mailboxId, const NmId &folderId, const NmId &messageId, 
            const NmId &messagePartId);
    ~NmViewerViewNetReply();
    void setOriginalRequest(const QNetworkRequest &request);
    qint64 readData(char *data, qint64 maxlen);
    qint64 readBufferSize () const;
    qint64 bytesAvailable () const;
    bool isSequential () const;
    void abort();

public slots:
    void signalReady();
    void fetchCompleted(int result);
    void fetchCancelled();

private:
    QByteArray mDataArray;
    NmUiEngine &mUiEngine;
    NmId mMailboxId;
    NmId mFolderId;
    NmId mMessageId;
    NmId mMessagePartId;
    NmOperation *mOperation;    // Not owned
    qint64 mReadIndex;
};

#endif /* NMVIEWERVIEWNETREPLY_H_ */
