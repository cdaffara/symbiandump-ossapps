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
*   Stub used by CxuiServiceProvider
*/

#ifndef XQSERVICEPROVIDER_H
#define XQSERVICEPROVIDER_H

#include <QObject>
#include <QString>
#include "xqrequestinfo.h"

class XQServiceProvider : public QObject
{
    Q_OBJECT
public:
    XQServiceProvider(const QString &service, QObject *parent = 0) : QObject(parent) {Q_UNUSED(service)}
    virtual ~XQServiceProvider() {}
protected:
    virtual void publishAll(){}
    bool completeRequest(int index, const QVariant &retValue) {return true;}
    int setCurrentRequestAsync() {return 0;}
    XQRequestInfo requestInfo() const {return XQRequestInfo();}
};

#endif // XQSERVICEPROVIDER_H
