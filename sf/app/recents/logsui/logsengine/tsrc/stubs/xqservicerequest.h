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

#ifndef XQSERVICEREQUEST_H
#define XQSERVICEREQUEST_H

#include <QVariant>
#include <xqrequestinfo.h>
#include <QObject>

class XQServiceRequest : public QObject
{
    Q_OBJECT
public:
    
    XQServiceRequest();
    XQServiceRequest(const QString& fullServiceName, const QString& message, const bool& synchronous = true);    
    virtual ~XQServiceRequest();

    bool send(QVariant& retValue) ;

    void setSynchronous(const bool &synchronous);
    bool isSynchronous() const;

    QString service() const;
    QString message() const;

    void setArguments(const QList<QVariant> &arguments);
    
    template<typename T>
    inline XQServiceRequest &operator<< (const T &var)
    {
        QVariant v = qVariantFromValue(var);
        addArg(v);
        return *this;
    }

    void setInfo(const XQRequestInfo &info);
    XQRequestInfo info() const;
    
signals:
    void requestCompleted(const QVariant& value) ;
    void requestError(int err);

private:
    void addArg(const QVariant& v);
};


#endif
