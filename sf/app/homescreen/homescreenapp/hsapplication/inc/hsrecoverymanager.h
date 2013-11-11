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

#ifndef HSRECOVERYMANAGER_H
#define HSRECOVERYMANAGER_H

#include <QObject>
#include <QTimer>

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(t_hsapplication)

class HsRecoveryManager : public QObject
{
    Q_OBJECT

public:
    HsRecoveryManager(QObject *parent = 0);
    ~HsRecoveryManager();

    void execute();
    
private slots:
    void restoreRomDatabase();
    void resetCrashCount();
           
private:
    Q_DISABLE_COPY(HsRecoveryManager)
    int crashCount();
    
private:
    QTimer mTimer;

    HOMESCREEN_TEST_FRIEND_CLASS(t_hsapplication)
};

#endif // HSRECOVERYMANAGER_H
