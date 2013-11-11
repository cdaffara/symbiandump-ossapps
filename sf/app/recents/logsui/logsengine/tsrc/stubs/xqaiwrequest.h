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


#ifndef XQAIWREQUEST_H_
#define XQAIWREQUEST_H_


#include <QObject>
#include <QList>
#include <QVariant>
#include <xqaiwinterfacedescriptor.h>


class XQAiwRequest : public QObject
    {
    Q_OBJECT

public:

    XQAiwRequest(const XQAiwInterfaceDescriptor &descriptor, const QString &operation, bool embedded = true);
    virtual ~XQAiwRequest();

public slots:
        
    void setArguments(const QList<QVariant> &arguments);
    const XQAiwInterfaceDescriptor &descriptor() const;

    bool send();

    const QString &operation() const;

    void setSynchronous(bool synchronous);
    
    
signals:

    void requestOk(const QVariant& result);
    void requestError(int errorCode, const QString& errorMessage);

    };

#endif /* XQAIWREQUEST_H_ */ 
