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
 *       test application for qt file manager operation service public apis.
 */

#include "ut_fmoprationserviceapi.h"
#include "testviewmanager.h"
#include "fmoperationservice.h"
#include "fmviewmanager.h"
#include <QtTest/QtTest>
#include <hbmainwindow.h>
#include <QFile>
#include <QtAlgorithms>

/*!
 \class TestFmOperationService
 \brief describe the test case's goal, like: \n
 class name: FmOperationService \n
 class's description: The header file of the operation service of file manager \n
 type of test case: unit api test\n 
 test cases' number totally: \n
 */

/*!
 Description of test data 
 */
void TestFmOperationService::initTestCase()
{
    mMainWindow = new HbMainWindow();
    TestViewManager::CreateTestViewManager(mMainWindow);   
    mOperationService = TestViewManager::viewManager()->operationService();
    QVERIFY (mOperationService != 0);
    
    connect(mOperationService, SIGNAL(notifyFinish( FmOperationBase*)),
            this, SLOT(onNotifyFinish( FmOperationBase*)));
    connect(mOperationService, SIGNAL(notifyError( FmOperationBase*, int, QString)), 
            this, SLOT(onNotifyError( FmOperationBase*, int, QString)));
    connect(mOperationService, SIGNAL(notifyCanceled( FmOperationBase* )),
            this, SLOT(onNotifyCanceled( FmOperationBase* )));
    
    // initialize public test data here
    mPath = Drive_C + QString("data") + QDir::separator() 
            + QString( "dataforfmoperation" ) + QDir::separator();
    mDataPath = Drive_C + QString("data") + QDir::separator() + QString( "dataforfmoperation" )
            + QDir::separator() + QString( "data" ) + QDir::separator();
    mDataMultiCopyPath = mDataPath + QString("multifilesforcopy") + QDir::separator();
    mDataMultiMovePath = mDataPath + QString("multifilesformove") + QDir::separator();
}

/*!
 Descrition of what you will do in this function
 */
void TestFmOperationService::cleanupTestCase()
{
//  mOperationService and mMainWindow are deleted in RemoveViewManager() too.
    TestViewManager::RemoveViewManager();    
    // release all test data
}

/*!
 Test Case Description:\n 
 1. Fucntion Name: explicit FmOperationService( QObject *parent );  \n
 2. Case Descrition: test the constructor function. \n
 3. Input Parameters:  \n &nbsp;&nbsp;
 <1> parent = 0;\n &nbsp;&nbsp;
 <2> parent = new QObject(); \n
 4. Expected result: \n &nbsp;&nbsp;
 <1> no crash \n &nbsp;&nbsp;
 <2> no crash \n
 */
void TestFmOperationService::testConstructor()
{
    FmOperationService *testOperationService = new FmOperationService(0);
    QVERIFY (testOperationService != 0);
    delete testOperationService;
}

/*!
 Test Case Description:\n 
 1. Fucntion Name: int asyncViewDriveDetails( const QString driverName );  \n
 2. Case Descrition:  \n
 3. Input Parameters:  \n &nbsp;&nbsp;
 <1> driverName = QString(XX);\n &nbsp;&nbsp;
 <2> driverName = QString(); \n
 4. Expected result: \n &nbsp;&nbsp;
 <1> no crash \n &nbsp;&nbsp;
 <2> no crash \n
 */
void TestFmOperationService::testAsyncViewDriveDetails()
{
//blocked, because of the UI problem.
}

/*!
 Test Case Description:\n 
 1. Fucntion Name:  \n &nbsp;&nbsp;
    int asyncCopy( const QStringList &sourceList, const QString &targetPath ); \n
 2. Case Descrition:  \n
 3. Input Parameters:  \n &nbsp;&nbsp;
 <1> sourceList = QString(XX), targetPath = QString(XX)\n
 4. Expected result: \n &nbsp;&nbsp;
 <1> the file is copied to the target path. \n
 */
void TestFmOperationService::testAsyncCopySingleFile()
{
    QStringList sourceData;
    sourceData.append(mDataPath + "dataforcopy.txt");
    // make sure the copy file is not available in the target path.
    QFile checkFile(mPath + "dataforcopy.txt");
    if(checkFile.exists()){
        checkFile.remove();
    }   
    //test the asyncCopy() function.
    int retInfo = mOperationService->asyncCopy(sourceData, mPath);
    
    mTestEventLoop.exec();
    QVERIFY( checkFile.exists() );
}

/*!
 Test Case Description:\n 
 1. Fucntion Name:  \n &nbsp;&nbsp;
    int asyncCopy( const QStringList &sourceList, const QString &targetPath ); \n
 2. Case Descrition:  \n
 3. Input Parameters:  \n &nbsp;&nbsp;
 <1> sourceList = QString(XX), targetPath = QString(XX)\n
 4. Expected result: \n &nbsp;&nbsp;
 <1> All the files are copied to the target path. \n
 */
void TestFmOperationService::testAsyncCopyMultiFiles()
{
    QStringList sourceDataMultiCopy;
    sourceDataMultiCopy.append(mDataMultiCopyPath  + "copydata1.txt");
    sourceDataMultiCopy.append(mDataMultiCopyPath  + "copydata2.txt");
    sourceDataMultiCopy.append(mDataMultiCopyPath  + "copydata3.txt");
    // make sure the copy file is not available in the target path.
    QFile checkFile1(mPath + "copydata1.txt");
    QFile checkFile2(mPath + "copydata2.txt");
    QFile checkFile3(mPath + "copydata3.txt");
    if(checkFile1.exists()){
        checkFile1.remove();
    }   
    if(checkFile2.exists()){
        checkFile2.remove();
    } 
    if(checkFile3.exists()){
        checkFile3.remove();
    } 
    //test the asyncCopy() function.
    int retInfo = mOperationService->asyncCopy(sourceDataMultiCopy, mPath);
    
    mTestEventLoop.exec();
    QVERIFY( checkFile1.exists() );
    QVERIFY( checkFile2.exists() );
    QVERIFY( checkFile3.exists() );
}

/*!
 Test Case Description:\n 
 1. Fucntion Name:  \n &nbsp;&nbsp;
    int asyncCopy( const QStringList &sourceList, const QString &targetPath ); \n
 2. Case Descrition:  \n
 3. Input Parameters:  \n &nbsp;&nbsp;
 <1> sourceList = QString(XX), targetPath = QString(XX)\n
 4. Expected result: \n &nbsp;&nbsp;
 <1> the folder included its content is copied to the target path. \n
 */
void TestFmOperationService::testAsyncCopyFolder()
{
    QStringList sourceDataFolder;
    sourceDataFolder.append( mDataPath + "multifilesforcopy" );
    // make sure the copy file is not available in the target path.
    QStringList folderNeedRemove;
    folderNeedRemove.append(mPath + "multifilesforcopy");
    QDir checkDir(mPath + "multifilesforcopy");
    if(checkDir.exists()){
        mOperationService->asyncRemove(folderNeedRemove);
        mTestEventLoop.exec();
    }   
    //test the asyncCopy() function.
    int retInfo = mOperationService->asyncCopy(sourceDataFolder, mPath);
    
    mTestEventLoop.exec();
    QVERIFY( checkDir.exists() );
}

/*!
 Test Case Description:\n 
 1. Fucntion Name:  \n &nbsp;&nbsp;
    int asyncCopy( const QStringList &sourceList, const QString &targetPath ); \n
 2. Case Descrition:  \n
 3. Input Parameters:  \n &nbsp;&nbsp;
 <1> sourceList = QString(), targetPath = QString(XX)\n
 4. Expected result: \n &nbsp;&nbsp;
 <1> no crash \n
 */
void TestFmOperationService::testAsyncCopyEmpty()
{
    QStringList sourceDataEmpty;
    sourceDataEmpty.append(QString());
    int retInfo = mOperationService->asyncCopy(sourceDataEmpty, mPath);
    
    mTestEventLoop.exec();
}

/*!
 Test Case Description:\n 
 1. Fucntion Name:  \n &nbsp;&nbsp;
    int asyncMove( const QStringList &sourceList, const QString &targetPath ); \n
 2. Case Descrition:  \n
 3. Input Parameters:  \n &nbsp;&nbsp;
 <1> sourceList = QString(XX), targetPath = QString(XX)\n
 4. Expected result: \n &nbsp;&nbsp;
 <1> the file is moved to the target path. \n
 */
void TestFmOperationService::testAsyncMoveSingleFile()
{
    QStringList sourceDataSingleMove;
    sourceDataSingleMove.append(mDataPath + "dataformove.txt");
    // make sure the copy file is not available in the target path.
    QFile checkFile(mPath + "dataformove.txt");
    if(checkFile.exists()){
        checkFile.remove();
    }   
    //test the asyncCopy() function.
    int retInfo = mOperationService->asyncMove(sourceDataSingleMove, mPath);
    
    mTestEventLoop.exec();
    QVERIFY( checkFile.exists() );
    QFile checkSourceData(mDataPath + "dataformove.txt");
    QVERIFY( !checkSourceData.exists() );
}

/*!
 Test Case Description:\n 
 1. Fucntion Name:  \n &nbsp;&nbsp;
    int asyncMove( const QStringList &sourceList, const QString &targetPath ); \n
 2. Case Descrition:  \n
 3. Input Parameters:  \n &nbsp;&nbsp;
 <1> sourceList = QString(XX), targetPath = QString(XX)\n
 4. Expected result: \n &nbsp;&nbsp;
 <1> All the filea are moved to the target path. \n
 */
void TestFmOperationService::testAsyncMoveMultiFiles()
{
    QStringList sourceDataMultiMove;
    sourceDataMultiMove.append(mDataMultiMovePath + "movedata1.txt");
    sourceDataMultiMove.append(mDataMultiMovePath + "movedata2.txt");
    // make sure the copy file is not available in the target path.
    QFile checkFile1(mPath + "movedata1.txt");
    QFile checkFile2(mPath + "movedata2.txt");
    if(checkFile1.exists()){
        checkFile1.remove();
    }   
    if(checkFile2.exists()){
        checkFile2.remove();
    }   
    //test the asyncCopy() function.
    int retInfo = mOperationService->asyncMove(sourceDataMultiMove, mPath);    
    mTestEventLoop.exec();
    //verify the files are copied to the target path.
    QVERIFY( checkFile1.exists() );
    QVERIFY( checkFile2.exists() );
    //verify the files are not in the orignal path yet.
    QFile checkSourceData1(mDataMultiMovePath + "movedata1.txt");
    QFile checkSourceData2(mDataMultiMovePath + "movedata2.txt");
    QVERIFY( !checkSourceData1.exists() );
    QVERIFY( !checkSourceData2.exists() );
}

/*!
 Test Case Description:\n 
 1. Fucntion Name:  \n &nbsp;&nbsp;
    int asyncMove( const QStringList &sourceList, const QString &targetPath ); \n
 2. Case Descrition:  \n
 3. Input Parameters:  \n &nbsp;&nbsp;
 <1> sourceList = QString(XX), targetPath = QString(XX)\n
 4. Expected result: \n &nbsp;&nbsp;
 <1> The folder is moved to the target path. \n
 */
void TestFmOperationService::testAsyncMoveFolder()
{
    QStringList sourceDataFolder;
    sourceDataFolder.append( mDataPath + "multifilesforMove" );
    // make sure the copy file is not available in the target path.
    QStringList folderNeedRemove;
    folderNeedRemove.append(mPath + "multifilesforMove");
    QDir checkDir(mPath + "multifilesforMove");
    if(checkDir.exists()){
        mOperationService->asyncRemove(folderNeedRemove);
        mTestEventLoop.exec();
    }   
    //test the asyncMove() function.
    int retInfo = mOperationService->asyncMove(sourceDataFolder, mPath);    
    mTestEventLoop.exec();
    
    QVERIFY( checkDir.exists() );
}

/*!
 Test Case Description:\n 
 1. Fucntion Name:  \n &nbsp;&nbsp;
    int asyncMove( const QStringList &sourceList, const QString &targetPath ); \n
 2. Case Descrition:  \n
 3. Input Parameters:  \n &nbsp;&nbsp;
 <1> sourceList = QString(), targetPath = QString(XX)\n
 4. Expected result: \n &nbsp;&nbsp;
 <1> no crash \n
 */
void TestFmOperationService::testAsyncMoveEmpty()
{
    QStringList sourceDataEmpty;
    sourceDataEmpty.append(QString());
    int retInfo = mOperationService->asyncMove(sourceDataEmpty, mPath);
    
    mTestEventLoop.exec();
}

/*!
 Test Case Description:\n 
 1. Fucntion Name:  \n &nbsp;&nbsp;
    int asyncRemove( const QStringList &pathList ); \n
 2. Case Descrition:  \n
 3. Input Parameters:  \n &nbsp;&nbsp;
    <1> pathList = QStringList(XX);\n 
 4. Expected result: \n &nbsp;&nbsp;
    <1> The file is removed. \n 
 */
void TestFmOperationService::testAsyncRemoveSingleFile()
{
    QStringList sourceData;
    sourceData.append(mPath + "dataforcopy.txt");
    // make sure the copy file is available in the target path.
    QFile checkFile(mPath + "dataforcopy.txt");
    if(checkFile.exists()){
    //test the asyncRemove() function.
        int retInfo = mOperationService->asyncRemove(sourceData);    
        mTestEventLoop.exec();
    }   
    QVERIFY( !checkFile.exists() );
}

/*!
 Test Case Description:\n 
 1. Fucntion Name:  \n &nbsp;&nbsp;
    int asyncRemove( const QStringList &pathList ); \n
 2. Case Descrition:  \n
 3. Input Parameters:  \n &nbsp;&nbsp;
    <1> pathList = QStringList(XX);\n 
 4. Expected result: \n &nbsp;&nbsp;
    <1> All the files are removed. \n 
 */
void TestFmOperationService::testAsyncRemoveMultiFiles()
{
    QStringList multiRemoveData;
    multiRemoveData.append(mPath + "copydata1.txt");
    multiRemoveData.append(mPath + "copydata2.txt");
    multiRemoveData.append(mPath + "movedata1.txt");
    // make sure the copy file is available in the target path.
    QFile checkFile1(mPath + "copydata1.txt");
    QFile checkFile2(mPath + "copydata2.txt");
    QFile checkFile3(mPath + "movedata1.txt");
    if(checkFile1.exists() && checkFile2.exists() && checkFile3.exists()){
    //test the asyncRemove() function.
        int retInfo = mOperationService->asyncRemove(multiRemoveData);    
        mTestEventLoop.exec();
    }   
    QVERIFY( !checkFile1.exists() );
    QVERIFY( !checkFile2.exists() );
    QVERIFY( !checkFile3.exists() );
}

/*!
 Test Case Description:\n 
 1. Fucntion Name:  \n &nbsp;&nbsp;
    int asyncRemove( const QStringList &pathList ); \n
 2. Case Descrition:  \n
 3. Input Parameters:  \n &nbsp;&nbsp;
    <1> pathList = QStringList(XX);\n 
 4. Expected result: \n &nbsp;&nbsp;
    <1> The specified folder is removed. \n 
 */
void TestFmOperationService::testAsyncRemoveFolder()
{
    QStringList sourceDataFolder;
    sourceDataFolder.append( mDataPath + "multifilesforMove" );
    // make sure the copy file is not available in the target path.
    QStringList folderRemoveData;
    folderRemoveData.append(mPath + "multifilesforMove");
    QDir checkDir(mPath + "multifilesforMove");
    
    if(checkDir.exists()){
        mOperationService->asyncRemove(folderRemoveData);
        mTestEventLoop.exec();
    }
    //verify the folder is removed.
    QVERIFY( !checkDir.exists() );
}

void TestFmOperationService::testAsyncBackup()
{
    // not implemented yet.
}

void TestFmOperationService::onNotifyFinish(FmOperationBase* operationBase)
{
    mTestEventLoop.exit();
}
void TestFmOperationService::onNotifyError(FmOperationBase* operationBase,
        int error, QString errString)
{
    mTestEventLoop.exit();
}
void TestFmOperationService::onNotifyCanceled(FmOperationBase* operationBase)
{
    mTestEventLoop.exit();
}

QTEST_MAIN(TestFmOperationService)
