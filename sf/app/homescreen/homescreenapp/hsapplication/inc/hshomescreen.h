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
* Description:  Homescreen application main class.
*
*/

#ifndef HSHOMESCREEN_H
#define HSHOMESCREEN_H

#include <QObject>
#include "hstest_global.h"
#ifdef Q_OS_SYMBIAN
#include "hshomescreenclientserviceprovider.h"
#endif

HOMESCREEN_TEST_CLASS(t_hsapplication)

class QStateMachine;

namespace QtMobility {
    class QServiceManager;
}
using QtMobility::QServiceManager;


class HsHomeScreen : public QObject
{
    Q_OBJECT

public:
    HsHomeScreen(QObject *parent = 0);
    ~HsHomeScreen();

signals:    
    void exit();

public slots:
    void start();
    void stop();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    void registerServicePlugins();
    void registerServicePlugins(const QString &root, QServiceManager &serviceManager);

private slots:
    void onRuntimeStarted();
    void onRuntimeStopped();    

private:
    Q_DISABLE_COPY(HsHomeScreen)

private:
    QStateMachine *mRuntime;
#ifdef Q_OS_SYMBIAN
    //Service provider for QtHighway 
    HsHomeScreenClientServiceProvider *mHomeScreenClientServiceProvider;
#endif
    HOMESCREEN_TEST_FRIEND_CLASS(t_hsapplication)
};

#endif
