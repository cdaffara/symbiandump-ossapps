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

#include <QStateMachine>
#include <QDir>

#include <qservicemanager.h>
#include <qservicefilter.h>
#include <qserviceinterfacedescriptor.h>

#include <HbInstance>

#include "hshomescreen.h"
#include "hstest_global.h"
#include "hsstatemachine.h"
#ifdef Q_OS_SYMBIAN
#include "hshomescreenclientserviceprovider.h"
#endif
QTM_USE_NAMESPACE

/*!
    \class HsHomeScreen
    \ingroup group_hsapplication
    \brief Homescreen application main class.
    Manages the state machine execution.
*/

/*!
    Constructor.
    \a parent Parent object.
*/
HsHomeScreen::HsHomeScreen(QObject *parent)
  : QObject(parent),
    mRuntime(0)
#ifdef Q_OS_SYMBIAN
    ,mHomeScreenClientServiceProvider(0)
#endif
{
    HSTEST_FUNC_ENTRY("HS::HsHomeScreen::HsHomeScreen");

    registerServicePlugins();
    
    mRuntime = new HsStateMachine(this);

    connect(mRuntime, SIGNAL(started()), SLOT(onRuntimeStarted()));
    connect(mRuntime, SIGNAL(stopped()), SLOT(onRuntimeStopped()));
    hbInstance->allMainWindows().first()->installEventFilter(this);
#ifdef Q_OS_SYMBIAN
    mHomeScreenClientServiceProvider = new HsHomeScreenClientServiceProvider;
    mHomeScreenClientServiceProvider->setParent(this);
#endif

    HSTEST_FUNC_EXIT("HS::HsHomeScreen::HsHomeScreen");
}

/*!
    Destructor.
*/
HsHomeScreen::~HsHomeScreen()
{
}

/*!
    \fn void HsHomeScreen::exit()
    Emitted when the home screen application needs to exit.
*/

/*!
    Starts the state machine.
*/
void HsHomeScreen::start()
{
    HSTEST_FUNC_ENTRY("HS::HsHomeScreen::start");
    mRuntime->start();
    HSTEST_FUNC_EXIT("HS::HsHomeScreen::start");
}

/*!
    Stops the state machine.
*/
void HsHomeScreen::stop()
{
	if (mRuntime->isRunning()) {
        QEventLoop eventLoop;
        connect(mRuntime, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        QMetaObject::invokeMethod(mRuntime, "event_exit", Qt::QueuedConnection);
        eventLoop.exec();
    }
}

/*!
    \copydoc QObject::eventFilter(QObject *watched, QEvent *event)
*/
bool HsHomeScreen::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);
    if (event->type() == QEvent::Close) {
        return true;
    }
    return false;
}

/*!
    Called after the state machine has started.
*/
void HsHomeScreen::onRuntimeStarted()
{
}

/*!
    Called after the state machine has stopped.
*/
void HsHomeScreen::onRuntimeStopped()
{
    emit exit();
}

/*!
    Registers service plugins pre-installed on the device.
*/
void HsHomeScreen::registerServicePlugins()
{    
    HSTEST_FUNC_ENTRY("HS::HsHomeScreen::registerServicePlugins()");

#ifdef Q_OS_SYMBIAN    
    foreach(QFileInfo drive, QDir::drives()) {
        QApplication::addLibraryPath(drive.absolutePath() 
            + QLatin1String("resource/qt/plugins/homescreen"));
    }
#else
    QApplication::addLibraryPath("resource/qt/plugins/homescreen");
#endif    
    
    QServiceManager serviceManager;
    
    QStringList pluginPaths;
    
    pluginPaths << "private/20022F35";
    
#ifndef Q_OS_SYMBIAN
    pluginPaths << "resource/qt/plugins/homescreen";
#endif

    QFileInfoList drives = QDir::drives();
    foreach(const QString pluginPath, pluginPaths) {
#ifdef Q_OS_SYMBIAN
        //Check plugin dirs from root of different drives
        foreach(QFileInfo drive, drives) {
            QString driveLetter = drive.absolutePath();
            QString path = driveLetter + pluginPath;
            if(QDir(path).exists()) {
                registerServicePlugins(path, serviceManager);
            }
        }
#endif
        //Check plugin path relative to current dir
        if(QDir(pluginPath).exists()) {
            registerServicePlugins(pluginPath, serviceManager);
        }
    }
    HSTEST_FUNC_EXIT("HS::HsHomeScreen::registerServicePlugins()");
}

#ifdef COVERAGE_MEASUREMENT 
#ifndef Q_OS_SYMBIAN 
#pragma CTC SKIP
#endif // Q_OS_SYMBIAN
#endif //COVERAGE_MEASUREMENT

/*!
    Recursively registers service plugins starting from given /a root
    directory. All directories containing plugins are added to
    application's library paths at the same time.
*/
void HsHomeScreen::registerServicePlugins(const QString &root, QServiceManager &serviceManager)
{
    HSTEST_FUNC_ENTRY("HS::HsHomeScreen::registerServicePlugins(const QString &)");
    QDir dir(root);
    QFileInfoList fileInfos = dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);

    foreach (QFileInfo fileInfo, fileInfos) {
        registerServicePlugins(fileInfo.absoluteFilePath(), serviceManager);
    }

    fileInfos = dir.entryInfoList(QStringList("*.xml"));

    if (!fileInfos.isEmpty()) {
        //Plugin dll and xml are in the same directory
        QApplication::addLibraryPath(root);
        qDebug() << QString("HS::HsHomeScreen::registerServicePlugins - Directory added to application's library paths: ")
                 << root;        
        foreach(QFileInfo fileInfo, fileInfos) {
            if (serviceManager.addService(fileInfo.absoluteFilePath())) {
                qDebug() << QString("HS::HsHomeScreen::registerServicePlugins - Plugin registered: ") + fileInfo.fileName();
            } else {
                qDebug() << QString("HS::HsHomeScreen::registerServicePlugins - Plugin FAILED to register: ") + fileInfo.fileName();
            }
        }
    }
    HSTEST_FUNC_EXIT("HS::HsHomeScreen::registerServicePlugins(const QString &)");
}

#ifdef COVERAGE_MEASUREMENT
#ifndef Q_OS_SYMBIAN 
#pragma CTC ENDSKIP
#endif // Q_OS_SYMBIAN
#endif //COVERAGE_MEASUREMENT
