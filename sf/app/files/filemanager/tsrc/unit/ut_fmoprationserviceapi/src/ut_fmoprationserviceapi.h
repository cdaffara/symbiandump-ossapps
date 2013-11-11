/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*       test application for qt control panel public apis.
*/

#ifndef UT_FMOPRATIONSERVICEAPI_H_
#define UT_FMOPRATIONSERVICEAPI_H_

#include <QObject>
#include <QEventLoop>
#include <QString>

class HbMainWindow;
class FmOperationBase;
class FmOperationService;

class TestFmOperationService :public QObject
{
    Q_OBJECT
private slots:
    
    void initTestCase();
    
    void testConstructor(); //test the constructor function. 
    void testAsyncViewDriveDetails();
    
    void testAsyncCopySingleFile();
    void testAsyncCopyMultiFiles();
    void testAsyncCopyFolder();
    void testAsyncCopyEmpty();
    
    void testAsyncMoveSingleFile();
    void testAsyncMoveMultiFiles();
    void testAsyncMoveFolder();
    void testAsyncMoveEmpty();
    
    void testAsyncRemoveSingleFile();
    void testAsyncRemoveMultiFiles();
    void testAsyncRemoveFolder();
//    void testAsyncRemoveEmpty();    
    
    void testAsyncBackup();
    
    
//    void testAsyncFormat();        
    void cleanupTestCase();
    
    // handle operation service event
    void onNotifyFinish( FmOperationBase* operationBase ); 
    void onNotifyError( FmOperationBase* operationBase, int error, QString errString ); 
    void onNotifyCanceled( FmOperationBase* operationBase ); 
private:
    HbMainWindow *mMainWindow;
    QEventLoop mTestEventLoop;
    FmOperationService *mOperationService;
    QString mPath;
    QString mDataPath;
    QString mDataMultiCopyPath;
    QString mDataMultiMovePath;
};
#endif /* UT_FMOPRATIONSERVICEAPI_H_ */
