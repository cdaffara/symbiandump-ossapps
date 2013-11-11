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
*       test the file manager internal apis.
*/

#include "ut_fminternaldrivemodel.h"
#include "testclassdatafmdrivelistprovider.h"
#include "fmdrivemodel.h"
#include <QtTest/QtTest>
#include <hbapplication.h>
#include "fmutils.h"
/*!
    \class TestFmInternalDriveModel
    \brief \n
      class name:  FmDriveModel \n
      class's description: Test the File Manager internal api functions. \n
      type of test case: unit test\n 
      test cases' number totally: 10 \n
 */

void TestFmInternalDriveModel::initTestCase()
{
    // translate the related text. 
    QString lang = QLocale::system().name(); 
    QString path = "z:/resource/qt/translations/"; 
    mTranslator.load( path + "filemanager_" + lang );
    qApp->installTranslator(&mTranslator);
    
    // get all the drives which can be shown, the parameters should be according to the parameter (Options) of FmDriveModel.
    FmUtils::getDriveList(mDriverList, false);
    qDebug() << mDriverList;        
}
/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        explicit FmDriveModel( QObject *parent = 0, Options options = 0,
            FmDriveListProvider *driveListProvider = 0 ); \n
     2. Case Descrition: test the constructor function. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> parent = new QObject(), 
            Options = FmDriveModel::FillWithVolume, 
            driveListProvider = new TestDataClassFmDriveListProvider();\n &nbsp;&nbsp;
        <2> parent = new QObject(),
            Options = FmDriveModel::FillWithVolume | FillWithDefaultVolume, 
            driveListProvider = 0 \n&nbsp;&nbsp;
        <3> parent = new QObject(); 
            Options = FmDriveModel::FillWithVolume | FillWithDefaultVolume |HideUnAvailableDrive, 
            driveListProvider = 0 \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> no crash \n &nbsp;&nbsp;
        <2> no crash \n &nbsp;&nbsp;
        <3> no crash \n
 */
void TestFmInternalDriveModel::testConstructor()
{
    QObject *pObject = new QObject();
    // the class TestDataClassFmDriveListProvider is a sub class from FmDriveListProvider.
    TestDataClassFmDriveListProvider *driveListProvider = new TestDataClassFmDriveListProvider();    
    FmDriveModel *fmDriveModel = new FmDriveModel( pObject,FmDriveModel::FillWithVolume,driveListProvider );
    QVERIFY ( fmDriveModel != 0 );

    // the drive number in class TestDataClassFmDriveListProvider is "3".
    QVERIFY ( fmDriveModel->rowCount() == 3 );
    delete fmDriveModel;
    fmDriveModel = 0;
    
    fmDriveModel = new FmDriveModel( pObject,FmDriveModel::FillWithVolume | FmDriveModel::FillWithDefaultVolume ,0 );
    QVERIFY ( fmDriveModel != 0 );
    QVERIFY ( fmDriveModel->rowCount() > 0 );
    delete fmDriveModel;
    fmDriveModel = 0;
    
    fmDriveModel = new FmDriveModel( pObject, FmDriveModel::FillWithVolume | FmDriveModel::FillWithDefaultVolume | FmDriveModel::HideUnAvailableDrive ,0 );
    QVERIFY ( fmDriveModel != 0 );
    QVERIFY ( fmDriveModel->rowCount() > 0 );
    
    delete fmDriveModel;
    delete pObject;
    delete driveListProvider;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: int rowCount( const QModelIndex &parent = QModelIndex() ) const; \n
     2. Case Descrition: Verify it return the right row count. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> parent = QModelIndex() \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retColumnCount = 1 \n
 */
void TestFmInternalDriveModel::testRowCount()
{
    FmDriveModel *fmDriveModel = new FmDriveModel(0);
    QModelIndex testIndex1 = fmDriveModel->index(1,0);

    int retRowCount1 = fmDriveModel->rowCount(testIndex1);
    QVERIFY(retRowCount1 == 0);    
    int retRowCount2 = fmDriveModel->rowCount();
    delete fmDriveModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: int columnCount( const QModelIndex &parent = QModelIndex() ) const; \n
     2. Case Descrition:  \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> parent = QModelIndex() \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retColumnCount = 1 \n
 */
void TestFmInternalDriveModel::testColumnCount()
{
    FmDriveModel *fmDriveModel = new FmDriveModel(0);
    QModelIndex testIndex = QModelIndex();
    int retColumnCount = fmDriveModel->columnCount(testIndex);
    QVERIFY(retColumnCount == 1);    
    delete fmDriveModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name:  \n &nbsp;&nbsp;
        QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const; \n
     2. Case Descrition:  \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> index = QModelIndex(), role = Qt::DisplayRole \n &nbsp;&nbsp;
        <2> index = QModelIndex(), role = Qt::UserRole \n &nbsp;&nbsp;
        <3> index = QModelIndex(), role = Qt::DecorationRole \n &nbsp;&nbsp;
        <4> index = QModelIndex(), role = Qt::TextAlignmentRole \n &nbsp;&nbsp;
     4. Expected result: \n &nbsp;&nbsp;
        <1> retData1 = "C:" \n &nbsp;&nbsp;
        <2> retData2 = "C:\"  \n &nbsp;&nbsp;
        <3> return the right icon
        <4> to be delete...
 */
void TestFmInternalDriveModel::testData()
{
    QObject *pObject = new QObject();  
    TestDataClassFmDriveListProvider *driveListProvider = new TestDataClassFmDriveListProvider();
    
    FmDriveModel *fmDriveModel = new FmDriveModel( 0 );
    QModelIndex index = fmDriveModel->index(0,0);
    
    QVariant retData1 = fmDriveModel->data(index, Qt::DisplayRole);
    QString strTemp1 = retData1.toString();
    QVERIFY( strTemp1.trimmed() == "C:" );

//  unable to verify the return icon is the right one.
    QVariant retData3 = fmDriveModel->data(index, Qt::DecorationRole); 
    
// need to delete the  "if (index.column() == 1 && role == Qt::TextAlignmentRole)", because only one column is defined.
    QVariant retData4 = fmDriveModel->data(index, Qt::TextAlignmentRole);
    
    delete driveListProvider;
    driveListProvider = 0;
    delete fmDriveModel;
    fmDriveModel = 0; 
}

/*!
     Test Case Description:\n 
     1. Fucntion Name:  \n &nbsp;&nbsp;
        bool indexValid( const QModelIndex &index ) const; \n
     2. Case Descrition:  \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> index = QModelIndex() \n &nbsp;&nbsp;     
     4. Expected result: \n &nbsp;&nbsp;
        <1> retBool = true \n
 */
void TestFmInternalDriveModel::testIndexValid()
{
    FmDriveModel *fmDriveModel = new FmDriveModel(0);
    QModelIndex aIndex = fmDriveModel->index(0,0);
    QModelIndex bIndex = fmDriveModel->index(0,1); 
    bool a = fmDriveModel->indexValid(aIndex);
    QVERIFY(a == true);
    bool b = fmDriveModel->indexValid(bIndex);
    QVERIFY(b == true);
    delete fmDriveModel;
    fmDriveModel = 0;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name:  \n &nbsp;&nbsp;
        QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const; \n
     2. Case Descrition:  \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> section = 0, orientation =  Qt::Horizontal, role = Qt::DisplayRole \n &nbsp;&nbsp;
        <2> section = 1, orientation =  Qt::Horizontal, role = Qt::DisplayRole \n &nbsp;&nbsp;
        <3> section = 2, orientation =  Qt::Horizontal, role = Qt::DisplayRole \n &nbsp;&nbsp;
        <4> section = 3, orientation =  Qt::Horizontal, role = Qt::DisplayRole \n &nbsp;&nbsp;
        <5> section = 4, orientation =  Qt::Horizontal, role = Qt::DisplayRole \n &nbsp;&nbsp; 
        <6> section = 1, orientation =  Qt::Vertical, role = Qt::DisplayRole \n &nbsp;&nbsp;
        <7> section = 1, orientation =  Qt::Horizontal, role = Qt::UserRole \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retHeaderData.toString() = "Name" \n &nbsp;&nbsp;
        <2> retHeaderData.toString() = "Size" \n &nbsp;&nbsp;
        <3> retHeaderData.toString() = "Type" \n &nbsp;&nbsp;
        <4> retHeaderData.toString() = "Date Modified" \n &nbsp;&nbsp;
        <5> no crash \n &nbsp;&nbsp;
        <6> no crash \n &nbsp;&nbsp;
        <7> no crash \n 
 */
void TestFmInternalDriveModel::testHeaderData()
{
    FmDriveModel *fmDriveModel = new FmDriveModel(0);
    // 
    QVariant retHeaderData1 = fmDriveModel->headerData(0, Qt::Horizontal, Qt::DisplayRole);
    QVERIFY(retHeaderData1.toString() == "Name");
    
    QVariant retHeaderData2 = fmDriveModel->headerData(1, Qt::Horizontal, Qt::DisplayRole);
    QVERIFY(retHeaderData2.toString() == "Size");
    
    QVariant retHeaderData3 = fmDriveModel->headerData(2, Qt::Horizontal, Qt::DisplayRole);
    QVERIFY(retHeaderData3.toString() == "Type");
    
    QVariant retHeaderData4 = fmDriveModel->headerData(3, Qt::Horizontal, Qt::DisplayRole);
    QVERIFY(retHeaderData4.toString() == "Date Modified");
    
    QVariant retHeaderData5 = fmDriveModel->headerData(4, Qt::Horizontal, Qt::DisplayRole);    
    QVERIFY(retHeaderData5.toString().isEmpty());// "");
    
#ifdef TO_BE_INVESTIGATE    
    QVariant retHeaderData6 = fmDriveModel->headerData(0, Qt::Vertical, Qt::DisplayRole);  
    QString a = retHeaderData6.toString(); // to be investigated.
//    QVERIFY(retHeaderData6.toString() == ""); 
#endif
    
    QVariant retHeaderData7 = fmDriveModel->headerData(1, Qt::Horizontal, Qt::UserRole);  
    QVERIFY(retHeaderData7.toString().isEmpty()); //== "");
    
    delete fmDriveModel;
    fmDriveModel = 0;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name:  \n &nbsp;&nbsp;
        QString displayString( const QModelIndex &index ) const; \n
     2. Case Descrition: Verify the correct display string is shown. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> index = QModelIndex() \n &nbsp;&nbsp;     
     4. Expected result: \n &nbsp;&nbsp;
        <1> retBool = true \n
 */
void TestFmInternalDriveModel::testDisplayStringWithVolume()
{
    // option = FillWithVolume | FillWithDefaultVolume
    FmDriveModel *fmDriveModel = new FmDriveModel( 0,FmDriveModel::FillWithVolume | FmDriveModel::FillWithDefaultVolume, 0);
    
    for(int i=0; i < mDriverList.count(); i++){
        QModelIndex indexValid = fmDriveModel->index(i,0);
        // get the display string.
        QVariant retData = fmDriveModel->displayString(indexValid); 
        QVERIFY(retData.toString() == FmUtils::fillDriveVolume(mDriverList[i], true));
    }
    delete fmDriveModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name:  \n &nbsp;&nbsp;
        QString displayString( const QModelIndex &index ) const; \n
     2. Case Descrition: Verify the correct display string is shown. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> index = QModelIndex() \n &nbsp;&nbsp;     
     4. Expected result: \n &nbsp;&nbsp;
        <1> retBool = true \n
 */
void TestFmInternalDriveModel::testDisplayStringWithoutVolume()
{ 
    FmDriveModel *fmDriveModel = new FmDriveModel();
    
    for(int i=0; i < mDriverList.count(); i++){
        QModelIndex indexValid = fmDriveModel->index(i,0);
        // get the display string.
        QVariant retData = fmDriveModel->displayString(indexValid); 
//        QString retString = retData.toString();
//        qDebug()<<retString;
        QVERIFY(retData.toString() == FmUtils::removePathSplash( mDriverList[i] ));
    }
    delete fmDriveModel;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name:  \n &nbsp;&nbsp;
        QString displayString( const QModelIndex &index ) const; \n
     2. Case Descrition: Verify it doesn't crash when using an invalid index. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> index = QModelIndex() \n &nbsp;&nbsp;     
     4. Expected result: \n &nbsp;&nbsp;
        <1> retString = QString() \n
 */
void TestFmInternalDriveModel::testDisplayStringEmpty()
{ 
    FmDriveModel *fmDriveModel = new FmDriveModel();
    QModelIndex indexInValid = fmDriveModel->index(-1,0);
    QVariant retData = fmDriveModel->displayString(indexInValid); 
    QVERIFY(retData.toString() == QString());
}
/*!
     Test Case Description:\n 
     1. Fucntion Name:  \n &nbsp;&nbsp;
        QString driveName( const QModelIndex &index ) const; \n
     2. Case Descrition:  \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> indexValid = QModelIndex() \n &nbsp;&nbsp;
        <2> indexInValid = QModelIndex() \n     
     4. Expected result: \n &nbsp;&nbsp;
        <1> return the rignt drive name. \n &nbsp;&nbsp;
        <2> no crash. \n
 */
void TestFmInternalDriveModel::testDriveName()
{
    FmDriveModel *fmDriveModel = new FmDriveModel();
    QModelIndex indexValid = fmDriveModel->index(0,0);    
    QModelIndex indexInValid1 = fmDriveModel->index(0,2);
    QModelIndex indexInValid2 = fmDriveModel->index(18);
    QModelIndex indexInValid3 = fmDriveModel->index(5,0);
    QString retDriveName1 = fmDriveModel->driveName(indexValid);
    QVERIFY(retDriveName1 == "C:/");
    QString retDriveName2 = fmDriveModel->driveName(indexInValid1);
    QVERIFY(retDriveName2.isEmpty());
    QString retDriveName3 = fmDriveModel->driveName(indexInValid2);
    QVERIFY(retDriveName3.isEmpty());
    QString retDriveName4 = fmDriveModel->driveName(indexInValid3);
    QVERIFY(retDriveName4.isEmpty());
    delete fmDriveModel;
    fmDriveModel = 0;
}

void TestFmInternalDriveModel::cleanupTestCase()
{
}

QTEST_MAIN(TestFmInternalDriveModel)
