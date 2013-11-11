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
* Description:  Stub for FM Radio home screen widget unit testing.
*
*/

#ifndef XQAIWREQUEST_H
#define XQAIWREQUEST_H

// System includes
#include <QObject>
#include <QVariant>
#include <QStringList>

// User includes
#include <xqrequestinfo.h>

class XQAiwRequest : public QObject
{
    Q_OBJECT

public:
    XQAiwRequest(const QString &interface, const QString &operation, bool isEmbedded);
    ~XQAiwRequest();

    void replyRequestOk(const QVariantList &reply);

public slots:
    void setOperation(const QString &operation);
    void setSynchronous(bool synchronous);
    void setEmbedded(bool embedded);
    bool send();
    void setInfo(const XQRequestInfo &info);

signals:
    void requestOk(const QVariant& result);
    void requestError(int errorCode, const QString& errorMessage);
    
private:
    bool replyAfterDelay();
    
    
private: // data
    QString mOperation;
    QStringList pendingRequests;
    int mRequestCount;

};

#endif // XQAIWREQUEST_H
