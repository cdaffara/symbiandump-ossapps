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
* Description:  Main test class for hsutils library.
*
*/


#include <qservicemanager.h>
#include <hbmainwindow.h>
#include <hbinstance.h> 
#include "t_hsapplication.h"

QTM_USE_NAMESPACE

const QString xmlPath = "./hsresources/plugins/stateplugins/";
const QString mockedStatePluginsXml = "mockstateplugins.xml";


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplication::initTestCase()
{
    //Let's add mocked test service to default interface implementation 
    //before testcases are run. This trick is needed especially in Symbian side
    //since default runtime is also seen due to exported resources for hsapplication.
    QApplication::addLibraryPath(QDir(xmlPath).absolutePath());
    QServiceManager manager;
    qDebug() << "t_hsapplication::initTestCase addService:" << manager.addService(xmlPath + mockedStatePluginsXml);
    
    qDebug() << "setInterfaceDefault(HsLoadSceneState): " << manager.setInterfaceDefault("mockstateplugins", "com.nokia.homescreen.state.HsLoadSceneState");
    qDebug() << "setInterfaceDefault(HsIdleState): " << manager.setInterfaceDefault("mockstateplugins", "com.nokia.homescreen.state.HsIdleState");
    qDebug() << "setInterfaceDefault(HsAppLibraryState): " << manager.setInterfaceDefault("mockstateplugins", "com.nokia.homescreen.state.HsAppLibraryState");
    qDebug() << "setInterfaceDefault(HsMenuWorkerState): " << manager.setInterfaceDefault("mockstateplugins", "com.nokia.homescreen.state.HsMenuWorkerState");
    qDebug() << "setInterfaceDefault(HsRootState): " << manager.setInterfaceDefault("mockstateplugins", "com.nokia.homescreen.state.HsRootState");
    qDebug() << "setInterfaceDefault(HsBackupRestoreState): " << manager.setInterfaceDefault("mockstateplugins", "com.nokia.homescreen.state.HsBackupRestoreState");
    
    HbInstance::instance();
    mWindow = new HbMainWindow;
    #ifndef NFT
//        QDir::setCurrent("C:/hsresources/testresources");
    #else      
//        QDir::setCurrent("Z:/hsresources/testresources");
    #endif   
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplication::cleanupTestCase()
{
    QServiceManager manager;
    manager.removeService("mockstateplugins");
    QApplication::removeLibraryPath(xmlPath);

    delete mWindow;
        mWindow = 0;
}

QTEST_MAIN(t_hsapplication)
