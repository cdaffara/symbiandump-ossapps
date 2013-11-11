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
* Description:  First Time Use application main class.
*
*/


#include "ftufirsttimeuse.h"

#include <QStateMachine>
#include <qservicemanager.h>
#include <qservicefilter.h>
#include <qserviceinterfacedescriptor.h>
#include <QFileInfo>
#include <QDir>
#include <QApplication>


QTM_USE_NAMESPACE

const char* ftuRuntimeUri = "com.nokia.ftu.runtime.FtuRuntime";

// ---------------------------------------------------------------------------
// FtuFirstTimeUse::FtuFirstTimeUse
// ---------------------------------------------------------------------------
//


#include <QList>

FtuFirstTimeUse::FtuFirstTimeUse(QObject* aParent) : 
    QObject(aParent),
    mRuntime(NULL)
{  
    QDEBUG("FTU::FtuFirstTimeUse::FtuFirstTimeUse");
    
    registerServicePlugins();

    QServiceManager manager;
    QServiceFilter filter(ftuRuntimeUri);
    QList<QServiceInterfaceDescriptor> interfaces = manager.findInterfaces(
                                                                    filter);
    
    QDEBUG("ftu: found " << interfaces.count() 
             << " instances of runtime";)
    
    if(interfaces.count() > 0){
        mRuntime = (QStateMachine*)(manager.loadInterface(
                                        interfaces.first().interfaceName()));
    }

        
    
    if(mRuntime) {
        mRuntime->setParent(this);
        connect(mRuntime, SIGNAL(started()), SLOT(handleRuntimeStarted()));
        connect(mRuntime, SIGNAL(stopped()), SLOT(handleRuntimeStopped()));
        connect(mRuntime, SIGNAL(faulted()), SLOT(handleRuntimeFaulted()));    
    }
    
    QDEBUG("FTU::FtuFirstTimeUse::FtuFirstTimeUse");
     
}

// ---------------------------------------------------------------------------
// FtuFirstTimeUse::~FtuFirstTimeUse()
// ---------------------------------------------------------------------------
//
FtuFirstTimeUse::~FtuFirstTimeUse()
{
	if(mRuntime)
	{
		disconnect(mRuntime, SIGNAL(started()), this, SLOT(handleRuntimeStarted()));
        disconnect(mRuntime, SIGNAL(stopped()), this, SLOT(handleRuntimeStopped()));
        disconnect(mRuntime, SIGNAL(faulted()), this, SLOT(handleRuntimeFaulted()));

		delete mRuntime;
	}
}

// ---------------------------------------------------------------------------
// FtuFirstTimeUse::start()
// ---------------------------------------------------------------------------
//
void FtuFirstTimeUse::start()
{
    QDEBUG("FTU::FtuFirstTimeUse::start");
    
    if(mRuntime) {
        mRuntime->start();
    } else {
        emit exit();
    }
    
    QDEBUG("FTU::FtuFirstTimeUse::start");
}

// ---------------------------------------------------------------------------
// FtuFirstTimeUse::stop()
// ---------------------------------------------------------------------------
//
void FtuFirstTimeUse::stop()
{
    QDEBUG("FTU::FtuFirstTimeUse::stop");
    
    
    if (mRuntime && mRuntime->isRunning()) {
            QMetaObject::invokeMethod(mRuntime, "event_exit");
        }

    
    QDEBUG("FTU::FtuFirstTimeUse::stop");
}

// ---------------------------------------------------------------------------
// FtuFirstTimeUse::handleRuntimeStarted()
// ---------------------------------------------------------------------------
//
void FtuFirstTimeUse::handleRuntimeStarted()
{
}
 
// ---------------------------------------------------------------------------
// FtuFirstTimeUse::handleRuntimeStopped()
// ---------------------------------------------------------------------------
//
void FtuFirstTimeUse::handleRuntimeStopped()
{
    emit exit();
}
 
// ---------------------------------------------------------------------------
// FtuFirstTimeUse::handleRuntimeFaulted()
// ---------------------------------------------------------------------------
//
void FtuFirstTimeUse::handleRuntimeFaulted()
{
    emit exit();
}
// ---------------------------------------------------------------------------
// FtuFirstTimeUse::registerServicePlugins()
// ---------------------------------------------------------------------------
//
void FtuFirstTimeUse::registerServicePlugins()
{
    QDEBUG("FTU::FtuFirstTimeUse::registerServicePlugins()");
    QStringList pluginPaths;

    pluginPaths << "fturesources/plugins";

    QFileInfoList drives = QDir::drives();
    foreach(const QString pluginPath, pluginPaths) {
#ifdef Q_OS_SYMBIAN
        //Check plugin dirs from root of different drives
        foreach(QFileInfo drive, drives) {
            QString driveLetter = drive.absolutePath();
            QString path = driveLetter + pluginPath;
            if(QDir(path).exists()) {
                registerServicePlugins(path);
            }
        }
#endif
        //Check plugin path relative to current dir
        if(QDir(pluginPath).exists()) {
            registerServicePlugins(pluginPath);
        }
    }
    QDEBUG("FTU::FtuFirstTimeUse::registerServicePlugins()");
}

// ---------------------------------------------------------------------------
// FtuFirstTimeUse::registerServicePlugins()
// ---------------------------------------------------------------------------
//
void FtuFirstTimeUse::registerServicePlugins(const QString &root)
{
    QDEBUG("FTU::FtuFirstTimeUse::registerServicePlugins(const QString &)");
    QDEBUG("FTU: root: " << root;)
    QDir dir = QDir(root);
    QFileInfoList fileInfos = dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);

    foreach (QFileInfo fileInfo, fileInfos) {
        registerServicePlugins(fileInfo.absoluteFilePath());
    }

    fileInfos = dir.entryInfoList(QStringList("*.xml"));

    if (!fileInfos.isEmpty()) {
        //Plugin dll and xml are in the same directory
        QApplication::addLibraryPath(root);
        QDEBUG("FTU::FtuFirstTimeUse::registerServicePlugins - Directory added to application's library paths: "
                 << root;)
        QServiceManager manager;
        foreach(QFileInfo fileInfo, fileInfos) {
            manager.addService(fileInfo.absoluteFilePath());
            QDEBUG("FTU::FtuFirstTimeUse::registerServicePlugins - Plugin registered: " << fileInfo.fileName();)
        }
    }
    QDEBUG("FTU::FtuFirstTimeUse::registerServicePlugins(const QString &)");
}
