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
* Description: Homescreen client api.
*
*/

#ifndef HSHOMESCREENCLIENT_H
#define HSHOMESCREENCLIENT_H

#include <QObject>
#include <QVariant>

#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(HsHomescreenClientPluginTest)

class XQServiceRequest;

class HsHomescreenClient : public QObject
{
    Q_OBJECT

public:
    HsHomescreenClient(QObject *parent = 0);
    virtual ~HsHomescreenClient();
 
public slots:
    bool addWidget(const QString &uri, const QVariantHash &preferences);
    bool setWallpaper(const QString &fileName);
    bool setWallpaper(const QString &portraitFileName, const QString &landscapeFileName);
private slots:
    void onRequestCompleted(const QVariant &result);
    void onRequestError(int error);
    void doAddWidget(const QString &uri, const QVariantHash &preferences);
    void doSetWallpaper(const QString &fileName);
    void doSetWallpaper2(const QString &portraitFileName, const QString &landscapeFileName);
signals:
    void requestFinished();
private:
    XQServiceRequest* mAsyncRequest;
    bool    mRequestResult;
    
    HOMESCREEN_TEST_FRIEND_CLASS(HsHomescreenClientPluginTest)
};

#endif
