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
#ifndef HSTITLERESOLVER_H
#define HSTITLERESOLVER_H

#include <QObject>
#include "hsdeviceinfolistener.h"
#include "hsutils_global.h"
#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(t_hsUtils)

class HSUTILS_EXPORT HsTitleResolver : public QObject
{
    Q_OBJECT

public:
    HsTitleResolver(QObject *parent = 0);
    ~HsTitleResolver();
public:
    QString title();
signals:
    void titleChanged(const QString &name);
private slots:
    void onNetworkNameChanged(const QString &name);
    void onStatusChanged(HsDeviceInfoListener::HsDeviceInfoStatus status);
private:
    QString mOperatorName;
    QString mOfflineText;
    QString mNoServiceText;
    HsDeviceInfoListener *mDeviceInfoListener;
    HsDeviceInfoListener::HsDeviceInfoStatus mCurrentStatus;
    HOMESCREEN_TEST_FRIEND_CLASS(t_hsUtils)
};

#endif // HSTITLERESOLVER_H
