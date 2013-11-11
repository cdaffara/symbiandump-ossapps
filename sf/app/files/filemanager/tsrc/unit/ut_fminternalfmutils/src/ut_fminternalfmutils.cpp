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

#include "ut_fminternalfmutils.h"
#include "fmutils.h"
#include "fmdrivemodel.h"
#include <QtTest/QtTest>
#include <qdir.h>
#include <qdebug.h>
#include <hbglobal.h>

/*!
    \class TestFmInternalFmUtils
    \brief \n
      class name:  FmDriveModel \n
      class's description: Test the File Manager internal api functions. \n
      type of test case: unit test\n 
      test cases' number totally: 63 \n
 */

/*!
     Description of test data 
 */
void TestFmInternalFmUtils::initTestCase()
{
    QString lang = QLocale::system().name(); 
    QString path = "z:/resource/qt/translations/"; 
    mTranslator.load( path + "filemanager_" + lang );
    qApp->installTranslator(&mTranslator);
    
    // init the path of test data.
    mFilePath = Drive_C + QString("data") + QDir::separator() + QString("resource") + QDir::separator() + QString( "fmtestdata" ) + QDir::separator();
    // get all the drives
    QFileInfoList fileInfoList = QDir::drives();
    foreach(const QFileInfo &fileInfo,fileInfoList) {
        QString str = fileInfo.filePath();
        if (str.length() > 2) {
            str = str.left(3);
        }
        // append the drives to mDrives1 which drive type is USB or Memory card.
        FmDriverInfo driverInfo = FmUtils::queryDriverInfo( str );
        FmDriverInfo::DriveType driveType = driverInfo.driveType();
        if( driveType == FmDriverInfo::EDriveTypeMemoryCard || 
            driveType == FmDriverInfo::EDriveTypeUsbMemory){ 
            mDrives1.append(str);
            mDrives.append(str);
        }else{
            mDrives2.append(str);
            mDrives.append(str);
        }
    }
    qDebug() << mDrives;
    qDebug() << mDrives1;
    qDebug() << mDrives2;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString getDriveNameFromPath( const QString &path ); \n &nbsp;&nbsp;
        static QString fillPathWithSplash( const QString &filePath ); \n
     2. Case Descrition: verify that it return the right drive name. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> strpath = QString() \n &nbsp;&nbsp;
        <2> strpath = QString(XX) [XX.length() < 3] \n &nbsp;&nbsp;
        <3> strpath = QString(XX) [XX.length() >= 3] \n &nbsp;&nbsp;
     4. Expected result: \n &nbsp;&nbsp;
        <1> retPath = QString() \n &nbsp;&nbsp;
        <2> retPath = QString() \n &nbsp;&nbsp;
        <3> retPath = checkpath.left(3) \n 
 */
void TestFmInternalFmUtils::testGetDriveNameFromPath()
{
    QString strPath1 = "C";
    QString strPath2 = "D:\\dir\\subdir";
    QString strPath3 = "T:/dir/subdir";
    QString strPath4 = "pathwithoutLetter";
    QString strPath5 = "C:";
    QString strPath6 = "C:\\";
    // path = QString().
    QString retDriveName1 = FmUtils::getDriveNameFromPath(QString());
    QVERIFY(retDriveName1.isEmpty());
    // path1.length() < 3
    QString retDriveName2 = FmUtils::getDriveNameFromPath(strPath1);
    QVERIFY(retDriveName2.isEmpty());
    // path1.length() >= 3
    QString retDriveName3 = FmUtils::getDriveNameFromPath(strPath2);
    QVERIFY(retDriveName3 == QString("D:") + QDir::separator());
    
    QString retDriveName4 = FmUtils::getDriveNameFromPath(strPath3);
    QVERIFY(retDriveName4 == QString("T:") + QDir::separator());
    
    QString retDriveName5 = FmUtils::getDriveNameFromPath(strPath5);
    QVERIFY(retDriveName5 == QString("C:") + QDir::separator());
    
    QString retDriveName6 = FmUtils::getDriveNameFromPath(strPath6);
    QVERIFY(retDriveName6 == QString("C:") + QDir::separator());
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString getDriveLetterFromPath( const QString &path ); \n &nbsp;&nbsp;
     2. Case Descrition: verify that it return the right first drive letter. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> strpath = QString(XX) [XX.length() < 2] \n &nbsp;&nbsp;
        <2> strpath = QString(XX) [XX.length() >= 2] \n 
     4. Expected result: \n &nbsp;&nbsp;
        <1> retPath = QString() \n &nbsp;&nbsp;
        <2> retPath = strPath.left(1) \n &nbsp;&nbsp;
 */
void TestFmInternalFmUtils::testGetDriveLetterFromPath()
{
    QString strPath1 = "C";
    QString strPath2 = "X:\\dir\\subdir";
    // path = QString().
    QString retDriveName1 = FmUtils::getDriveLetterFromPath(QString());
    QVERIFY(retDriveName1.isEmpty());
    // path1.length() < 2
    QString retDriveName2 = FmUtils::getDriveLetterFromPath(strPath1);
    QVERIFY(retDriveName2.isEmpty());
    // path1.length() >= 2
    QString retDriveName3 = FmUtils::getDriveLetterFromPath(strPath2);
    QVERIFY(retDriveName3 == QString("X"));
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString formatStorageSize( quint64 size ); \n &nbsp;&nbsp;
     2. Case Descrition: verify that it return the correct storage size. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> size = Q_UINT64_C( 555 ) \n &nbsp;&nbsp;
        <2> size = Q_UINT64_C( 1024 ) \n &nbsp;&nbsp;
        <3> size = Q_UINT64_C( 1200000 ) \n &nbsp;&nbsp;
        <4> size = Q_UINT64_C( 1000*1000*1000 + 1024 ) \n &nbsp;&nbsp;
        <5> size = Q_UINT64_C( -5 ) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retSize = QString( "555 B" ) \n &nbsp;&nbsp;
        <2> retSize = QString( "1.00 KB" ) \n &nbsp;&nbsp;
        <3> retSize = QString( "1.1 MB" ) \n &nbsp;&nbsp;
        <4> retSize = QString( "0.9 GB" ) \n &nbsp;&nbsp;
        <5> no crash. \n
 */
void TestFmInternalFmUtils::testFormatStorageSize()
{
    quint64 size1 = Q_UINT64_C(555);
//    quint64 sizetemp = Q_UINT64_C(1000);
    quint64 size2 = Q_UINT64_C( 1024 );
    quint64 size3 = Q_UINT64_C( 1200000 );
    quint64 size4 = Q_UINT64_C( 1000*1000*1000 + 1024 );
#ifdef BUG_TO_BE_FIX
    quint64 sizeInvalid = Q_UINT64_C( -5 );
#endif
//    X < 1000
    QString retSize1 = FmUtils::formatStorageSize(size1);
    QVERIFY( retSize1 == QString::number(size1) + QString(" B") );
//    1000 < X < 1000*1000 
    QString retSize2 = FmUtils::formatStorageSize(size2);
    QVERIFY( retSize2 == QString::number( size2 / 1024.0, 'f', 2 ) + " KB" );
//   1000*1000 < X < 1000*1000*1000 
     QString retSize3 = FmUtils::formatStorageSize(size3);
    QVERIFY( retSize3 == QString::number( size3 / (1024.0 * 1024.0), 'f', 1 ) + " MB" );
//    X > 1000*1000*1000
     QString retSize4 = FmUtils::formatStorageSize(size4);
    QVERIFY( retSize4 == QString::number( size4 / ( 1024.0 * 1024.0 * 1024.0 ), 'f', 1 ) + " GB" );     

#ifdef BUG_TO_BE_FIX
     QString retSize5 = FmUtils::formatStorageSize(sizeInvalid);
     qDebug()<<retSize5;
#endif
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static bool     isDrive( const QString &path ); \n &nbsp;&nbsp;
     2. Case Descrition: verify it is a drive with correct path. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driverName = "C:\" \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retResult = true \n
 */
void TestFmInternalFmUtils::testIsDriveYes()
{
    bool retResult = FmUtils::isDrive("C:/");
    QVERIFY( retResult == true );
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static bool     isDrive( const QString &path ); \n &nbsp;&nbsp;
     2. Case Descrition: verify it is not a drive with wrong path. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driverName = "C" \n &nbsp;&nbsp;
        <2> driverName = "D:\path" \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retResult = false \n &nbsp;&nbsp;
        <2> retResult = false \n
 */
void TestFmInternalFmUtils::testIsDriveNo()
{
    bool retResult1 = FmUtils::isDrive("C");
    QVERIFY( retResult1 == false );
    
    bool retResult2 = FmUtils::isDrive("D:\\path");
    QVERIFY( retResult2 == false );
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString  formatPath( const QString &path  ); \n &nbsp;&nbsp;
     2. Case Descrition: verify it doesn't crash with empty path. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = QString() \n 
     4. Expected result: \n &nbsp;&nbsp;
        <1> retPath = QString() \n 
 */
void TestFmInternalFmUtils::testFormatPathWithEmptyPath()
{
    QString retPath = FmUtils::formatPath(QString());
    QVERIFY( retPath.isEmpty() );
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString  formatPath( const QString &path  ); \n &nbsp;&nbsp;
     2. Case Descrition: verify it doesn't crash with empty path. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = QString("C:\\path1\\subpath") \n &nbsp;&nbsp;
        <2> path = QString("C:/path2/subpath") \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retPath = QString("C:")+QDir::QDir::separator()+QString("path1")+QDir::separator()+QString("subpath") \n &nbsp;&nbsp;
        <2> retPath = QString("C:")+QDir::QDir::separator()+QString("path2")+QDir::separator()+QString("subpath") \n
 */
void TestFmInternalFmUtils::testFormatPath()
{
    QString retPath1 = FmUtils::formatPath("C:\\path1\\subpath");
    QVERIFY( retPath1 == QString("C:") + QDir::separator() +QString("path1") +QDir::separator() + QString("subpath") );
    
    QString retPath2 = FmUtils::formatPath("C:/path2/subpath");
    QVERIFY( retPath2 == QString("C:")+ QDir::separator()+QString("path2")+ QDir::separator()+QString("subpath") );
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString  fillPathWithSplash( const QString &filePath ); \n &nbsp;&nbsp;
     2. Case Descrition: verify it doesn't crash with empty path. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = QString() \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retPath = QString() \n 
 */
void TestFmInternalFmUtils::testFillPathWithSplashUseEmptyPath()
{
    QString retPath = FmUtils::fillPathWithSplash(QString());
    QVERIFY( retPath.isEmpty() );
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString  fillPathWithSplash( const QString &filePath ); \n &nbsp;&nbsp;
     2. Case Descrition: verify it doesn't add splash again when the path has the splash in the end. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retPath = path \n 
 */
void TestFmInternalFmUtils::testFillPathWithSplashHasSplash()
{
    QString path1 = "C:\\hasSplash\\";
    QString path2 = "D:/hasSplash/";
    QString retPath1 = FmUtils::fillPathWithSplash(path1);
    QVERIFY(retPath1 == QString("C:") + QDir::separator() + QString("hasSplash") + QDir::separator());
    
    QString retPath2 = FmUtils::fillPathWithSplash(path2);
    QVERIFY(retPath2 == QString("D:") + QDir::separator() + QString("hasSplash") + QDir::separator());
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString  fillPathWithSplash( const QString &filePath ); \n &nbsp;&nbsp;
     2. Case Descrition: verify the splash is added at the end of the path. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retPath = QString(XX) + QDir::separator() \n 
 */
void TestFmInternalFmUtils::testFillPathWithSplashWithoutSplash()
{
    QString path1 = "C:\\noSplash";
    QString path2 = "D:/noSplash";
    QString retPath1 = FmUtils::fillPathWithSplash(path1);
    QVERIFY(retPath1 == QString("C:") + QDir::separator() + QString("noSplash") + QDir::separator());
    
    QString retPath2 = FmUtils::fillPathWithSplash(path2);
    QVERIFY(retPath2 == QString("D:") + QDir::separator() + QString("noSplash") + QDir::separator());
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString  removePathSplash( const QString &filePath ); \n &nbsp;&nbsp;
     2. Case Descrition: verify it doesn't crash with empty path. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = QString() \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retPath = QString() \n 
 */
void TestFmInternalFmUtils::testRemovePathSplashUseEmptyPath()
{
    QString retPath = FmUtils::removePathSplash(QString());
    QVERIFY( retPath.isEmpty());
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString  removePathSplash( const QString &filePath ); \n &nbsp;&nbsp;
     2. Case Descrition: verify the splash at the end is removed. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retPath = QString(XX) \n 
 */
void TestFmInternalFmUtils::testRemovePathSplashHasSplash()
{
    QString path1 = "C:\\hasSplash\\";
    QString path2 = "D:/hasSplash/";
    QString path3 = "/";
    QString path4 = "\\";
    
    QString retPath1 = FmUtils::removePathSplash(path1);
    QVERIFY(retPath1 == QString("C:") + QDir::separator() + QString("hasSplash"));
    
    QString retPath2 = FmUtils::removePathSplash(path2);
    QVERIFY(retPath2 == "D:/hasSplash");
    
    QString retPath3 = FmUtils::removePathSplash( path3 );
    QVERIFY(retPath3.isEmpty());
    
    QString retPath4 = FmUtils::removePathSplash( path4 );
    QVERIFY(retPath4.isEmpty());
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString  removePathSplash( const QString &filePath ); \n &nbsp;&nbsp;
     2. Case Descrition: verify it doesn't crash with empty path. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = QString() \n
     4. Expected result: \n 
        <1> retPath = QString() \n 
 */
void TestFmInternalFmUtils::testRemovePathSplashWithoutSplash()
{
    QString path1 = "C:\\withoutSplash";
    QString path2 = "D:/withoutSplash";
    QString retPath1 = FmUtils::removePathSplash(path1);
    QVERIFY(retPath1 == QString("C:") + QDir::separator() + QString("withoutSplash"));
    
    QString retPath2 = FmUtils::removePathSplash(path2);
    QVERIFY(retPath2 == "D:/withoutSplash");
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static void getDriveList( QStringList &driveList, bool isHideUnAvailableDrive ); \n &nbsp;&nbsp;
     2. Case Descrition: verify the correct drives are returned.  \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveList = QStringList, isHideUnAvailableDrive = false \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> driveList == driveListForVerify\n 
 */
void TestFmInternalFmUtils::testGetDriveListAll()
{
    QStringList driveList;
    QStringList driveListForVerify;
    
    FmUtils::getDriveList(driveList, false);
    qDebug()<<driveList;
    
    QFileInfoList fileInfoList = QDir::drives();
    foreach(const QFileInfo &fileInfo,fileInfoList) {
        QString str = fileInfo.filePath();
        if (str.length() > 2) {
            str = str.left(3);
        }
        // append the drives to driveListForVerify except the drive type are EDriveRam and EDriveRom.
        FmDriverInfo driverInfo = FmUtils::queryDriverInfo( str );
        FmDriverInfo::DriveType driveType = driverInfo.driveType();
        // fliter the drives which drive type are EDriveRam and EDriveRom.
        bool driveFilter = ( driverInfo.driveState()& FmDriverInfo::EDriveRam ) ||( driverInfo.driveState()& FmDriverInfo::EDriveRom );
        if( !driveFilter ){ 
        driveListForVerify.append(str);
        }
    }
    qDebug()<<driveListForVerify;
    // verify all the drives in driveListForVerify are get in driveList too.
    foreach(QString driveName, driveListForVerify){
        QVERIFY(driveList.contains(driveName, Qt::CaseSensitive));
    }
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static void getDriveList( QStringList &driveList, bool isHideUnAvailableDrive ); \n &nbsp;&nbsp;
     2. Case Descrition: verify the correct drives are returned. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveList = QStringList, isHideUnAvailableDrive = true \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> driveList == driveListForVerify \n 
 */
void TestFmInternalFmUtils::testGetDriveListPart()
{
    QStringList driveList;
    QStringList driveListForVerify;
    
    FmUtils::getDriveList(driveList, true);
    qDebug() << driveList;
    
    QFileInfoList fileInfoList = QDir::drives();
    foreach(const QFileInfo &fileInfo,fileInfoList) {
        QString str = fileInfo.filePath();
        if (str.length() > 2) {
            str = str.left(3);
        }
        FmDriverInfo driverInfo = FmUtils::queryDriverInfo( str );
        FmDriverInfo::DriveType driveType = driverInfo.driveType();
        // fliter the drives which drive type are EDriveRam and EDriveRom.
        bool driveFilter = ( driverInfo.driveState()& FmDriverInfo::EDriveRam ) ||( driverInfo.driveState()& FmDriverInfo::EDriveRom );
        // filter the unavailable drive
        if( !driveFilter && FmUtils::isDriveAvailable( str ) ){         
        driveListForVerify.append(str);
        }
    }
    qDebug()<<driveListForVerify;    

    // verify all the drives in driveListForVerify are get in driveList too.
    foreach(QString driveName, driveListForVerify){
        QVERIFY(driveList.contains(driveName, Qt::CaseSensitive));
    }
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString  fillDriveVolume( QString driveName, bool isFillWithDefaultVolume ); \n &nbsp;&nbsp;
     2. Case Descrition: verify it doesn't crash with empty path. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveName = QString(), isFillWithDefaultVolume = false \n &nbsp;&nbsp;
        <2> driveName = QString(), isFillWithDefaultVolume = true \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retDriveVolume2 == QString() \n
 */
void TestFmInternalFmUtils::testFillDriveVolumeEmptyName()
{
    QString retDriveVolume1 = FmUtils::fillDriveVolume( QString(), false );
    QVERIFY(retDriveVolume1.isEmpty());

    QString retDriveVolume2 = FmUtils::fillDriveVolume( QString(), false );
    QVERIFY(retDriveVolume2.isEmpty());
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString  fillDriveVolume( QString driveName, bool isFillWithDefaultVolume ); \n &nbsp;&nbsp;
     2. Case Descrition: verify the default volume is set when the isFillWithDefaultVolume is true. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveName = QString(XX), isFillWithDefaultVolume = true \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> return the right drive volume(e.g.: C:memory) \n 
 */
void TestFmInternalFmUtils::testFillDriveVolumeDefaultVolume()
{
    foreach(QString strDriveName, mDrives ){
        QString retDriveVolume = FmUtils::fillDriveVolume(strDriveName, true );
            
            QString tempDriveName = FmUtils::fillPathWithSplash( strDriveName );
            FmDriverInfo driverInfo = FmUtils::queryDriverInfo( tempDriveName );
            QString checkedDriveName( FmUtils::removePathSplash( strDriveName ) );
            QString volumeName = driverInfo.volumeName();
            if(volumeName.isEmpty()){
            switch ( driverInfo.driveType() )
                        {
                        case FmDriverInfo::EDriveTypeMassStorage:
                            QVERIFY(retDriveVolume == hbTrId("txt_fmgr_dblist_1_mass_storage").arg(checkedDriveName));
                            break;
                        case FmDriverInfo::EDriveTypeUsbMemory:
                            QVERIFY(retDriveVolume == hbTrId("txt_fmgr_dblist_1_usb_memory").arg(checkedDriveName));
                            break;
                        case FmDriverInfo::EDriveTypeMemoryCard:
                            QVERIFY(retDriveVolume == hbTrId("txt_fmgr_dblist_1_memory_card").arg(checkedDriveName));
                            break;
                        case FmDriverInfo::EDriveTypePhoneMemory:
                            QVERIFY(retDriveVolume == hbTrId("txt_fmgr_dblist_1_device_memory").arg(checkedDriveName));
                            break;
                        default:
                            break;
                        }
            }
    }        
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString  fillDriveVolume( QString driveName, bool isFillWithDefaultVolume ); \n &nbsp;&nbsp;
     2. Case Descrition: verify it doesn't crash with empty path. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveList = QStringList, isHideUnAvailableDrive = false \n
     4. Expected result: \n 
        <1>  \n 
 */
void TestFmInternalFmUtils::testFillDriveVolume()
{
    foreach(QString strDriveName, mDrives ){
        QString retDriveVolume = FmUtils::fillDriveVolume(strDriveName, false );
        
        FmDriverInfo driverInfo = FmUtils::queryDriverInfo( strDriveName );
        QString checkedDriveName( FmUtils::removePathSplash( strDriveName ) );
        QString volumeName = driverInfo.volumeName();
        QString volumeNameForVerify = hbTrId( "%1 %2" ).arg( checkedDriveName ).arg( volumeName );
        QVERIFY(retDriveVolume == volumeNameForVerify);
    }
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString  getDefaultVolumeName( const QString &driveName ); \n &nbsp;&nbsp;
     2. Case Descrition: verify the correct default volume is returned \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveName = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> return the right drive volume(e.g.: "Device memory" when crive name is "C:") \n 
 */
void TestFmInternalFmUtils::testGetDefaultVolumeName()
{  
    foreach(QString strDriveName, mDrives ){
        // get the volume using getDefaultVolumeName function.
        QString retDriveVolume = FmUtils::getDefaultVolumeName(strDriveName);
            
            FmDriverInfo driverInfo = FmUtils::queryDriverInfo( strDriveName );
            switch ( driverInfo.driveType() )
                        {
                        case FmDriverInfo::EDriveTypeMassStorage:
                            QVERIFY(retDriveVolume == hbTrId("Mass storage")); 
                            break;
                        case FmDriverInfo::EDriveTypeUsbMemory:
                            QVERIFY(retDriveVolume == hbTrId("USB memory")); 
                            break;
                        case FmDriverInfo::EDriveTypeMemoryCard:
                            QVERIFY(retDriveVolume == hbTrId("Memory card"));
                            break;
                        case FmDriverInfo::EDriveTypePhoneMemory:
                            QVERIFY(retDriveVolume == hbTrId("Device memory"));
                            break;
                        case FmDriverInfo::EDriveTypeNotExist:
                        default:
                            break;
                        }
            }
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString  getVolumeNameWithDefaultNameIfNull( const QString &diskName, bool &defaultName ); \n &nbsp;&nbsp;
     2. Case Descrition: verify the right volume name is returned. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveName = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> return the right drive volume(e.g.: "Device memory" when drive name is "C:" & is empty) \n 
 */
void TestFmInternalFmUtils::testGetVolumeNameWithDefaultNameIfNull()
{  
    foreach(QString strDriveName, mDrives ){
        // get the volume using getVolumeNameWithDefaultNameIfNull() function.
        bool isDefaultName = false;
        QString retDriveVolume = FmUtils::getVolumeNameWithDefaultNameIfNull(strDriveName, isDefaultName);
        
            FmDriverInfo driverInfo = FmUtils::queryDriverInfo( strDriveName );
            QString volumeName = driverInfo.volumeName(); 
            if ( volumeName.isEmpty() ){
                switch ( driverInfo.driveType() )
                        {
                        case FmDriverInfo::EDriveTypeMassStorage:
                            QVERIFY(retDriveVolume == hbTrId("Mass storage")); 
                            break;
                        case FmDriverInfo::EDriveTypeUsbMemory:
                            QVERIFY(retDriveVolume == hbTrId("USB memory")); 
                            break;
                        case FmDriverInfo::EDriveTypeMemoryCard:
                            QVERIFY(retDriveVolume == hbTrId("Memory card"));
                            break;
                        case FmDriverInfo::EDriveTypePhoneMemory:
                            QVERIFY(retDriveVolume == hbTrId("Device memory"));
                            break;
                        case FmDriverInfo::EDriveTypeNotExist:
                        default:
                            break;
                        }
            }
            }
}
/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static bool isPathEqual( const QString &pathFst, const QString &pathLast ); \n &nbsp;&nbsp;
     2. Case Descrition: Verify the two path are the same. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> pathFst = QString(X), pathLast = QString(X)\n
     4. Expected result: \n &nbsp;&nbsp;
        <1> isEqual = true \n 
 */
void TestFmInternalFmUtils::testisPathEqualWithSamePath()
{
    QString path1 = "C:\\the path is same";
    QString path2 = "C:\\the path is same";
    QString path3 = "C:\\ frontbland\\isSame";
    QString path4 = "C:\\ frontbland\\isSame";
    QString path5 = "C:\\endwithbland \\isSame";
    QString path6 = "C:\\endwithbland \\isSame";
    bool isEqual1 = FmUtils::isPathEqual(QString(), QString());
    QVERIFY(isEqual1 == true);
    
    bool isEqual2 = FmUtils::isPathEqual(path1, path2);
    QVERIFY(isEqual2 == true);
    
    bool isEqual3 = FmUtils::isPathEqual(path3, path4);
    QVERIFY(isEqual3 == true);
    
    bool isEqual4 = FmUtils::isPathEqual(path5, path6);
    QVERIFY(isEqual4 == true);
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static bool isPathEqual( const QString &pathFst, const QString &pathLast ); \n &nbsp;&nbsp;
     2. Case Descrition: Verify the two path are different. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> pathFst = QString(X), pathLast = QString(Y)\n
     4. Expected result: \n &nbsp;&nbsp;
        <1> isEqual = false \n 
 */
void TestFmInternalFmUtils::testisPathEqualWithDiffPath()
{
    QString path1 = "C:\\the path is different";
    QString path2 = "C:\\the path is not same\\subdir\\";
    bool isEqual = FmUtils::isPathEqual(path1, path2);
    QVERIFY(isEqual == false);
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static bool checkFolderFileName( const QString& name ); \n &nbsp;&nbsp;
     2. Case Descrition: Verify folder or file name is OK. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> pathFst = QString(XX)\n
     4. Expected result: \n &nbsp;&nbsp;
        <1> isValid= true \n 
 */
void TestFmInternalFmUtils::testcheckFolderFileNameNormal()
{
    QString name = "This is a valid name";
    bool isValid = FmUtils::checkFolderFileName(name);
    QVERIFY(isValid == true);
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static bool checkFolderFileName( const QString& name ); \n &nbsp;&nbsp;
     2. Case Descrition: Verify the folder or file name is invalid. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> pathFst = QString(XX)\n
     4. Expected result: \n &nbsp;&nbsp;
        <1> isValid= false \n 
 */
void TestFmInternalFmUtils::testcheckFolderFileNameUnNormal()
{
    QList<QChar> list;
    list << '\\' << '/' << ':' << '*' << '?' << '\"' << '<' << '>' <<'|';
    foreach(QChar ilegelChar, list){
        QString name = QString("This is an invalid name") + ilegelChar;
        bool isValid = FmUtils::checkFolderFileName(name);
        QVERIFY(isValid == false);
    }
    // end with "."
    QString nameEndWithDot = "End with dot.";
    bool isValid1 = FmUtils::checkFolderFileName(nameEndWithDot);
    QVERIFY(isValid1 == false);
    // more than max size
    QString nameMax;
    nameMax.fill('x', 300);
    bool isValid2 = FmUtils::checkFolderFileName(nameMax);
    QVERIFY(isValid2 == false);
    // name is empty
    bool isValid3 = FmUtils::checkFolderFileName(QString());
    QVERIFY(isValid3 == false);
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static bool     checkNewFolderOrFile( const QString& fileName, const QString &path, QString &errString ); \n &nbsp;&nbsp;
     2. Case Descrition: Verify it is a new folder or file.\n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> fileName = QString(XX), path = QString(XX), errString = QString(XX)\n
     4. Expected result: \n &nbsp;&nbsp;
        <1> isNew = true \n 
 */
void TestFmInternalFmUtils::testCheckNewFolderOrFileUnNormal()
{
    QString fileName = "newFolder";
    QString path = mFilePath + "newFolder";
    QString errStr;
    bool isNew = FmUtils::checkNewFolderOrFile(fileName, path, errStr);
    QVERIFY(isNew == true);
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static bool     checkNewFolderOrFile( const QString& fileName, const QString &path, QString &errString ); \n &nbsp;&nbsp;
     2. Case Descrition: Verify the function when the folder/file name with ilegel characters.\n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> fileName = QString(XX), path = QString(XX), errString = QString(XX)\n
     4. Expected result: \n &nbsp;&nbsp;
        <1> isNew = false \n 
 */
void TestFmInternalFmUtils::testCheckNewFolderOrFileIlegel()
{
    QString validName = "Image.jpg";
    QString ilegelFileName = "isInvalidName?";
    QString validPath = mFilePath + "newFolder";
    QString inValidPath;
    inValidPath.fill('x', 300);
    QString errStr;
    
    // file name is invalid
    bool isNew1 = FmUtils::checkNewFolderOrFile(ilegelFileName, validPath, errStr);
    QVERIFY(isNew1 == false);
    QVERIFY(errStr == hbTrId( "Invalid file or folder name!" ));
    // path is too long
    bool isNew2 = FmUtils::checkNewFolderOrFile(validName, inValidPath, errStr);
    QVERIFY(isNew2 == false);
    QVERIFY(errStr == hbTrId( "the path you specified is too long!" ));
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static bool     checkNewFolderOrFile( const QString& fileName, const QString &path, QString &errString ); \n &nbsp;&nbsp;
     2. Case Descrition: Verify it is not a new folder or file.\n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> fileName = QString(XX), path = QString(XX), errString = QString(XX)\n
     4. Expected result: \n &nbsp;&nbsp;
        <1> isNew = false \n 
 */
void TestFmInternalFmUtils::testCheckNewFolderOrFileNo()
{
    QString validName = "Image.jpg";
    QString exsitPath = mFilePath;
    QString errStr;
    // the file\folder has been exsit
    bool isNew3 = FmUtils::checkNewFolderOrFile(validName, exsitPath, errStr);
    QVERIFY(isNew3 == false);
    QFileInfo fileInfo( exsitPath );
    QVERIFY(errStr == hbTrId( "%1 already exist!" ).arg( fileInfo.fileName()));
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static bool     isSubLevelPath( const QString &src, const QString &dest ); \n &nbsp;&nbsp;
     2. Case Descrition: Verify the path is a sub level path.\n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> src = QString(XX), dest = QString(XX)\n
     4. Expected result: \n &nbsp;&nbsp;
        <1> isSubLevelPath = true \n 
 */
void TestFmInternalFmUtils::testIsSubLevelPathYes()
{
    QString src = "C:\\dir";
    QString dest = "C:\\dir\\subdir";
    bool isSubLevelPath = FmUtils::isSubLevelPath(src, dest);
    QVERIFY(isSubLevelPath == true);
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static bool     isSubLevelPath( const QString &src, const QString &dest ); \n &nbsp;&nbsp;
     2. Case Descrition: Verify the path is a sub level path.\n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> src = QString(XX), dest = QString(XX)\n
     4. Expected result: \n &nbsp;&nbsp;
        <1> isSubLevelPath = false \n 
 */
void TestFmInternalFmUtils::testIsSubLevelPathNo()
{
    QString src1 = "C:\\dir1";
    QString dest1 = "C:\\dir2\\subdir";
    QString src2 = "C:\\dir1\\subdir";
    QString dest2 = "C:\\dir1";
    // dest does not contain src.
    bool isSubLevelPath1 = FmUtils::isSubLevelPath(src1, dest1);
    QVERIFY(isSubLevelPath1 == false);
    // dest.length < src.length
    bool isSubLevelPath2 = FmUtils::isSubLevelPath(src2, dest2);
    QVERIFY(isSubLevelPath2 == false);
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static int removeDrivePwd( const QString &driverName, const QString &Pwd ); \n &nbsp;&nbsp;
     2. Case Descrition: verify this function can remove the password successfully. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driverName = QString(), password = QString(XX) \n &nbsp;&nbsp;
     4. Expected result: \n 
        <1> retErr = FmErrNone, \n 
 */
void TestFmInternalFmUtils::testRemoveDrivePwdNormal()
{
    if( ! mDrives1.empty() ){
    //  set the password.
    FmUtils::setDrivePwd( mDrives1[0], QString(), QString("123456") );
    FmDriverInfo driverInfo = FmUtils::queryDriverInfo( mDrives1[0] );
    int a = driverInfo.driveState()& FmDriverInfo::EDrivePasswordProtected;
    QVERIFY( a == FmDriverInfo::EDrivePasswordProtected );
    //remove the password and verify the drive state.
    int retRemovePwdErr = FmUtils::removeDrivePwd( mDrives1[0], QString("123456") );
    driverInfo = FmUtils::queryDriverInfo( mDrives1[0] );
//    int b = driverInfo.driveState()& FmDriverInfo::EDrivePasswordProtected;
    QVERIFY( !(driverInfo.driveState()& FmDriverInfo::EDrivePasswordProtected) );
     
    QVERIFY( retRemovePwdErr == FmErrNone );
   }
    else
        QWARN("No memory crad or USB device are available.");
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static int removeDrivePwd( const QString &driverName, const QString &Pwd ); \n &nbsp;&nbsp;
     2. Case Descrition: verify this function can remove the password successfully. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driverName = QString(), password = QString(XX) \n &nbsp;&nbsp;
        <2> driverName = QString(XX), password = QString(XX) \n &nbsp;&nbsp;
     4. Expected result: \n &nbsp;&nbsp;
        <1> retErr = FmErrWrongParam, \n &nbsp;&nbsp;
        <2> no crash. \n
 */
void TestFmInternalFmUtils::testRemoveDrivePwdUnNormal()
{
    int retErr1 = FmUtils::removeDrivePwd( QString(), QString() );
    QVERIFY( retErr1 == FmErrWrongParam );   
    
    int retErr2 = FmUtils::removeDrivePwd( QString("C:/"), QString("5t0Lfezb") );
    QVERIFY( retErr2 == FmErrUnKnown );

    int retErr3 = FmUtils::removeDrivePwd( QString("U:/"), QString("5t0Lfezb") );
    QVERIFY( retErr3 == FmErrUnKnown );
}
/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static int setDrivePwd( const QString &driverName, const QString &oldPwd, const QString &newPwd); \n &nbsp;&nbsp;
     2. Case Descrition: verify this function can set the password correctly. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driverName = QString(XX), oldPwd = QString(), newPwd = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retErr = FmErrNone
 */
void TestFmInternalFmUtils::testSetDrivePwdNormal()
{
    if( ! mDrives1.empty() ){
    //set the password
    int retErr = FmUtils::setDrivePwd( mDrives1[0], QString(), QString("123456") );
    // get the drive state and verify it.
    FmDriverInfo driverInfo = FmUtils::queryDriverInfo( mDrives1[0] );
    int a = driverInfo.driveState()& FmDriverInfo::EDrivePasswordProtected;
    QVERIFY( a == FmDriverInfo::EDrivePasswordProtected );  
    QVERIFY( retErr == FmErrNone );  
    
    FmUtils::removeDrivePwd( mDrives1[0], QString("123456") );
    }
    else
        QWARN("No memory crad or USB device are available.");
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static int setDrivePwd( const QString &driverName, const QString &oldPwd, const QString &newPwd); \n &nbsp;&nbsp;
     2. Case Descrition: verify this function can set the password correctly. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driverName = QString(), oldPwd = QString(), newPwd = QString(X) \n &nbsp;&nbsp;
        <2> driverName = QString(X), oldPwd = QString(), newPwd = QString(X.length()>8) \n &nbsp;&nbsp;
        <3> driverName = QString(C:), oldPwd = QString(), newPwd = QString(X) \n 
     4. Expected result: \n &nbsp;&nbsp;
        <1> retErr = FmErrWrongParam \n &nbsp;&nbsp;
        <2> retErr = FmErrWrongParam \n &nbsp;&nbsp;
        <3> retErr = FmErrNotSupported \n 
 */
void TestFmInternalFmUtils::testSetDrivePwdUnNormal()
{
    // The drive name is empty
    int retSetPwdErr1 = FmUtils::setDrivePwd( QString(""), QString(""), QString("123456") );
    QVERIFY( retSetPwdErr1 == FmErrWrongParam );
    
    if( !mDrives1.empty() ){
    int retSetPwdEr2 = FmUtils::setDrivePwd( mDrives1[0], QString(""), QString("1234567890123456") );
    FmDriverInfo driverInfo = FmUtils::queryDriverInfo( mDrives1[0] );
    QVERIFY( !(driverInfo.driveState()& FmDriverInfo::EDrivePasswordProtected) );
    QVERIFY( retSetPwdEr2 == FmErrWrongParam );
    }else
         QWARN("No memory crad or USB device are available.");
    
    int retSetPwdErr3 = FmUtils::setDrivePwd( mDrives2[0], QString(""), QString("123456") );
    QVERIFY( retSetPwdErr3 == FmErrNotSupported );
    
    // return 
    int retSetPwdErr4 = FmUtils::setDrivePwd( QString("9527"), QString("oldpwd"), QString("123456") );
    QVERIFY( retSetPwdErr4 == FmErrUnKnown );  
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static int checkDrivePwd( const QString &driverName, const QString &pwd); \n &nbsp;&nbsp;
     2. Case Descrition: verify this function can check whether the drive having password . \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driverName = QString(), password = QString(X.length()<8) \n &nbsp;&nbsp;
        <2> driverName = QString(X), password = QString(X.length()>8) \n &nbsp;&nbsp;
        <3> driverName = QString(X), password = QString(X.length()<8) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retErr = FmErrWrongParam \n &nbsp;&nbsp;
        <2> retErr = FmErrWrongParam \n &nbsp;&nbsp;
        <3> retErr = FmErrNone\n 
 */
void TestFmInternalFmUtils::testCheckDrivePwd()
{
    if( !mDrives1.empty() ){
    //the drive name is empty
    int retCheckStatus1 = FmUtils::checkDrivePwd( QString(), QString("password"));
    FmDriverInfo driverInfo = FmUtils::queryDriverInfo( mDrives1[0] );

    QVERIFY( !(driverInfo.driveState()& FmDriverInfo::EDrivePasswordProtected) );
    QVERIFY( retCheckStatus1 == FmErrWrongParam );
    
    // the length of the password is more than 8
    int retCheckStatus2 = FmUtils::checkDrivePwd( mDrives1[0], QString("oldpassword"));
    driverInfo = FmUtils::queryDriverInfo( mDrives1[0] );

    QVERIFY( !(driverInfo.driveState()& FmDriverInfo::EDrivePasswordProtected) );  
    QVERIFY( retCheckStatus2 == FmErrWrongParam );
    
    int retCheckStatus3 = FmUtils::checkDrivePwd( mDrives1[0], QString("password"));
    driverInfo = FmUtils::queryDriverInfo( mDrives1[0] );
#ifdef BUG_TO_FIX
    QVERIFY( !(driverInfo.driveState()& FmDriverInfo::EDrivePasswordProtected) );  //The drive state of FmDriverInfo::EDrivePasswordProtected should not be changed with a valid pwd parameter.
    QVERIFY( retCheckStatus3 == FmErrNone );
#endif
    }
    else
        QWARN("No memory crad or USB device are available.");
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static void emptyPwd( QString &pwd ); \n &nbsp;&nbsp;
     2. Case Descrition: verify this function can empty the password . \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> pwd = QString() \n &nbsp;&nbsp;
        <2> pwd = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> no crash \n &nbsp;&nbsp;
        <2> no crash. \n
 */
void TestFmInternalFmUtils::testEmptyPwd()
{
    QString strPassword= QString("password");
    QString pwdEmpty = QString();
    FmUtils::emptyPwd( pwdEmpty );
    FmUtils::emptyPwd( strPassword );
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static int renameDrive( const QString &driverName, const QString &newVolumeName); \n
     2. Case Descrition: verify that it does not crash with bad names. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveName = QString(), newVolumeName = QString(XX) \n &nbsp;&nbsp;
        <2> driveName = "C:", newVolumeName = QString() \n &nbsp;&nbsp;
        <3> driveName = "C:", newVolumeName = QString("!$@^#*#dsfljlke").  \n &nbsp;&nbsp;
        <4> driveName = "C:", newVolumeName = QString("more than maximum words").  \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> return FmErrWrongParam \n &nbsp;&nbsp;
        <2> return FmErrUnKnown \n &nbsp;&nbsp;
        <3> return FmErrBadName \n &nbsp;&nbsp;
        <4> return FmErrBadName \n
 */
void TestFmInternalFmUtils::testRenameDriveWithBadName()
{
    int retInfo1 = FmUtils::renameDrive( QString(),QString("new volume name") );
    QVERIFY( retInfo1 == FmErrWrongParam );
    
    if( !mDrives1.empty() ){
    int retInfo2 = FmUtils::renameDrive( mDrives1[0],QString("") );
    QVERIFY( retInfo2 == FmErrNone );
    
    int retInfo3 = FmUtils::renameDrive( mDrives1[0],QString("!$@^#*e") );
    QVERIFY( retInfo3 == FmErrBadName );
    
    int retInfo4 = FmUtils::renameDrive( mDrives1[0],QString("more than maximum words") );
    QVERIFY( retInfo4 == FmErrUnKnown );
    }else
        QWARN("No memory crad or USB device are available.");
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static int renameDrive( const QString &driverName, const QString &newVolumeName); \n
     2. Case Descrition: verify this function can empty the password . \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveName = QString(X), newVolumeName = QString(XX) \n &nbsp;&nbsp;
        <2> driveName = "C:", newVolumeName = QString() \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> return FmErrWrongParam \n &nbsp;&nbsp;
        <2> return FmErrBadName \n
 */
void TestFmInternalFmUtils::testRenameDriveWithNormalName()
{
    if( !mDrives1.empty() ){
    int retInfo1 = FmUtils::renameDrive(mDrives1[0],QString("newname") );
    bool defaultName = false;
    QString retVolumeName = FmUtils::getVolumeNameWithDefaultNameIfNull( mDrives1[0], defaultName );
    QVERIFY( retVolumeName == "newname" );
    QVERIFY( retInfo1 == FmErrNone );
    //set the name back
    FmUtils::renameDrive(mDrives1[0],QString());
    }else
        QWARN("No memory crad or USB device are available.");
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static bool     checkDriveAccessFilter( const QString &driveName ); \n
     2. Case Descrition: verify whether the drive can be accessable. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveName = QString() \n &nbsp;&nbsp;
        <2> driveName = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> isAccessable == false \n &nbsp;&nbsp;
        <2> return true if it's accessful, or return false \n 
 */
void TestFmInternalFmUtils::testCheckDriveAccessFilter()
{
    foreach(QString strDriveName, mDrives ){
        bool isAccessable1 = FmUtils::checkDriveAccessFilter(strDriveName);
        
        FmDriverInfo driveInfo = FmUtils::queryDriverInfo( strDriveName );
        if ((driveInfo.driveState()& FmDriverInfo::EDriveRam ) ||
            +( driveInfo.driveState()& FmDriverInfo::EDriveRom ) ) {
            QVERIFY(isAccessable1 == false);
        }else
        QVERIFY(isAccessable1 == true);
    }
    // set drive name as QString
    bool isAccessable2 = FmUtils::checkDriveAccessFilter(QString());
    QVERIFY(isAccessable2 == false);
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString  checkDriveToFolderFilter( const QString &path ); \n
     2. Case Descrition: Verify the correct path is returned. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = QString("C:") \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retPath == path + QDir::separator() + QString( "data" ) + QDir::separator() \n &nbsp;&nbsp;       
 */
void TestFmInternalFmUtils::testCheckDriveToFolderFilterC()
{
    QString path = Drive_C;
    QString retPath = FmUtils::checkDriveToFolderFilter(path);
    QVERIFY(retPath == path + QString( "data" ) + QDir::separator());
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString  checkDriveToFolderFilter( const QString &path ); \n
     2. Case Descrition: Verify the correct path is returned. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retPath == path \n &nbsp;&nbsp;       
 */
void TestFmInternalFmUtils::testCheckDriveToFolderFilterNormal()
{
    QString path = "D:";
    QString retPath1 = FmUtils::checkDriveToFolderFilter(path);
    QVERIFY(retPath1 == path);
    
    QString retPath2 = FmUtils::checkDriveToFolderFilter(QString());
    QVERIFY(retPath2.isEmpty());
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString  checkFolderToDriveFilter( const QString &path );\n
     2. Case Descrition:  \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = "C:\\data" \n 
     4. Expected result: \n &nbsp;&nbsp;
        <1> retPath == "C:" \n
 */
void TestFmInternalFmUtils::testCheckFolderToDriveFilterC()
{
    QString path = "C:\\data";
    QString retPath = FmUtils::checkFolderToDriveFilter(path);
    QVERIFY(retPath == Drive_C);
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString  checkFolderToDriveFilter( const QString &path );\n
     2. Case Descrition: Verify the correct path is returned. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = QString() \n &nbsp;&nbsp;
        <2> path = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retPath == QString() \n &nbsp;&nbsp;
        <2> retPath == path \n 
 */
void TestFmInternalFmUtils::testCheckFolderToDriveFilterNormal()
{
    QString path = "E:\\thisIsNotCDrive";
    QString retPath1 = FmUtils::checkDriveToFolderFilter(path);
    QVERIFY(retPath1 == path);
    
    QString retPath2 = FmUtils::checkDriveToFolderFilter(QString());
    QVERIFY(retPath2.isEmpty());
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static int      isPathAccessabel( const QString &path );\n
     2. Case Descrition: Verify the right info is returned when using empty path \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = QString() \n 
     4. Expected result: \n &nbsp;&nbsp;
        <1> retInfo == FmErrPathNotExist \n 
 */
void TestFmInternalFmUtils::testIsPathAccessableEmpty()
{
    int retInfo = FmUtils::isPathAccessabel(QString());
    QVERIFY(retInfo == FmErrPathNotExist);
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static int      isPathAccessabel( const QString &path );\n
     2. Case Descrition: Verify the right info is returned when using valid path \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = Folder_C_Data\n 
     4. Expected result: \n &nbsp;&nbsp;
        <1> retInfo == FmErrNone \n 
 */
void TestFmInternalFmUtils::testIsPathAccessableOK()
{
    QString path = Folder_C_Data;
    int retInfo = FmUtils::isPathAccessabel(path);
    QVERIFY(retInfo == FmErrNone);
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static int      isPathAccessabel( const QString &path );\n
     2. Case Descrition: Verify the right info is returned when using unAccessable path \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = Drive_C +QString(XX) \n &nbsp;&nbsp;
        <2> path = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retInfo == FmErrDriveDenied \n
        <2> retInfo == FmErrPathNotExist \n 
 */
void TestFmInternalFmUtils::testIsPathAccessableNo()
{
     QString path = "cannotaccesspath";
    int retInfo = FmUtils::isPathAccessabel(path);
    QVERIFY(retInfo == FmErrPathNotExist);
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static bool     isDriveAvailable( const QString &path );\n
     2. Case Descrition: Verify the drive is available of the path. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = QString(XX) \n &nbsp;&nbsp;
        <2> path = QString() \n
     4. Expected result: \n &nbsp;&nbsp;
        <1>  \n &nbsp;&nbsp;
        <2> isAvailable == fasle \n
 */
void TestFmInternalFmUtils::testIsDriveAvailable()
{
    foreach(QString strDriveName, mDrives ){
        bool isAvailable1 = FmUtils::isDriveAvailable(strDriveName + QString("availableOrNot"));
        
        FmDriverInfo::DriveState driveState = FmUtils::queryDriverInfo( strDriveName + QString("availableOrNot") ).driveState();
        if( ( driveState & FmDriverInfo::EDriveAvailable ) ) {
        QVERIFY(isAvailable1 == true);
        }else
            QVERIFY(isAvailable1 == false);
    }
    // set drive name as QString
    bool isAccessable2 = FmUtils::isDriveAvailable(QString());
    QVERIFY(isAccessable2 == false);
    
    QString pathNotAvailable = "A:\\notAvailabel";
    bool isAccessable3 = FmUtils::isDriveAvailable(pathNotAvailable);
    QVERIFY(isAccessable3 == false);
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static bool     isDefaultFolder( const QString &folderPath  );\n
     2. Case Descrition: Verify the folder is default folder. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> filePath = QString(XX) \n 
     4. Expected result: \n &nbsp;&nbsp;
        <1> isDefault == true \n  
 */
void TestFmInternalFmUtils::testIsDefaultFolder()
{
    QString filePath1 = "C:\\data\\Games";
    QString filePath2 = "C:\\data\\Images";
    QString filePath3 = "C:\\data\\Installs";
    bool isDefault1 = FmUtils::isDefaultFolder(filePath1);
    QVERIFY(isDefault1 == true);
    bool isDefault2 = FmUtils::isDefaultFolder(filePath2);
    QVERIFY(isDefault2 == true);
    bool isDefault3 = FmUtils::isDefaultFolder(filePath3);
    QVERIFY(isDefault3 == true);
    
    bool isDefaultPath1 = FmUtils::isDefaultFolder(QString());
    QVERIFY(isDefaultPath1 == false);
    bool isDefaultPath2 = FmUtils::isDefaultFolder(mFilePath);
    QVERIFY(isDefaultPath2 == false);
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static void     createDefaultFolders( const QString &driverName );\n
     2. Case Descrition:  \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveName = QString() \n &nbsp;&nbsp;
        <2> driveName = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> isAccessable == false \n &nbsp;&nbsp;
        <2> return true if it's accessful, or return false \n 
 */
void TestFmInternalFmUtils::testCreateDefaultFolders()
{
    FmUtils::createDefaultFolders( Folder_C_Data );
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static int      setFileAttributes( const QString &srcFile, const QString &desFile );\n
     2. Case Descrition:  \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveName = QString() \n &nbsp;&nbsp;
        <2> driveName = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> isAccessable == false \n &nbsp;&nbsp;
        <2> return true if it's accessful, or return false \n 
 */
void TestFmInternalFmUtils::testSetFileAttributes()
{
    QString srcFile = mFilePath +"Image.jpg";
    QString desFile = mFilePath +"Tone.mp3";
    int err = FmUtils::setFileAttributes( srcFile, desFile );
    QVERIFY(err == 0);
    //crash when run to these lines below.    
//    User::LeaveIfError( src.Open( fsSession, *buf1, EFileRead | EFileShareReadersOnly ) );
//    User::LeaveIfError( des.Open( fsSession, *buf2, EFileWrite | EFileShareExclusive ) );
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static bool     hasEnoughSpace( const QString &targetDrive, qint64 size );\n
     2. Case Descrition: Verify the Specified drive has enough space. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveName = QString(XX), size = qint64 \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> isHasSpace == true \n 
 */
void TestFmInternalFmUtils::testHasEnoughSpaceYes()
{
    qint64 size = 1024;
    bool isHasSpace = FmUtils::hasEnoughSpace(mDrives[0], size);
    QVERIFY(isHasSpace == true);
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static bool     hasEnoughSpace( const QString &targetDrive, qint64 size );\n
     2. Case Descrition: Verify the Specified drive does not has enough space when using large size. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveName = QString(XX), size = qint64 \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> isHasSpace == false \n 
 */
void TestFmInternalFmUtils::testHasEnoughSpaceNo()
{
    qint64 size = 999999999999999;
    bool isHasSpace = FmUtils::hasEnoughSpace(mDrives[0], size);
    QVERIFY(isHasSpace == false);
}
/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static int      moveInsideDrive( const QString &source, const QString &target );\n
     2. Case Descrition:  \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> source = QString(XX) \n &nbsp;&nbsp;
        <2> target = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> isAccessable == false \n 
 */
void TestFmInternalFmUtils::testMoveInsideDrive()
{
    QString source = mFilePath + "Imageformove.jpg";
    QString target = Drive_C + QString("data") + QDir::separator() + QString("resource") + QDir::separator() + "Imagenew.jpg";
    QString errStr1;
    QString errStr2;
    FmUtils::moveInsideDrive(source,target);
    FmUtils::checkNewFolderOrFile("Imagenew.jpg", target, errStr1);
    bool isNotExsit = FmUtils::checkNewFolderOrFile("Imageformove.jpg", source, errStr2);
    qDebug()<<errStr1;
    //verify the image.jpg is moved to the new path with the new name.
    QVERIFY(errStr1 == "Imagenew.jpg already exist!");
    qDebug()<<isNotExsit;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static int      launchFile( const QString &filePath );\n
     2. Case Descrition: Verify the file can be launched or not. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveName = QString() \n &nbsp;&nbsp;
        <2> driveName = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> isAccessable == false \n &nbsp;&nbsp;
        <2> return true if it's accessful, or return false \n 
 */
void TestFmInternalFmUtils::testLaunchFile()
{
//    QString filePath = mFilePath + "Image.jpg";
//    int retInfo = FmUtils::launchFile(filePath); // switch to other thread, test app will shut down.
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static void     sendFiles( QStringList &filePathList );\n
     2. Case Descrition:  \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveName = QString() \n &nbsp;&nbsp;
        <2> driveName = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> isAccessable == false \n &nbsp;&nbsp;
        <2> return true if it's accessful, or return false \n 
 */
void TestFmInternalFmUtils::testSendFiles()
{
    // will delete this case in future
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString  getBurConfigPath( QString appPath );\n
     2. Case Descrition: verify the right path for backup restore config file. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveName = QString() \n 
     4. Expected result: \n &nbsp;&nbsp;
        <1> isAccessable == false \n 
 */
void TestFmInternalFmUtils::testGetBurConfigPath()
{
    QString retPath = FmUtils::getBurConfigPath(QString());
    QVERIFY(retPath == "z:/private/2002BCC0/burconfig.xml");
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static bool     isDriveC( const QString &driverName );\n
     2. Case Descrition: Verify the drive is C. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveName = Drive_C + QString("ItisCDrive") \n 
     4. Expected result: \n &nbsp;&nbsp;
        <1> isDriveC == true \n 
 */
void TestFmInternalFmUtils::testIsDriveCYes()
{
    QString pathToVerify = Drive_C +"ItisCDrive";
    bool isDriveC = FmUtils::isDriveC(pathToVerify);
    QVERIFY(isDriveC == true);
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static bool     isDriveC( const QString &driverName );\n
     2. Case Descrition: Verify the drive is C. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveName = Drive_C + QString("ItisCDrive") \n 
     4. Expected result: \n &nbsp;&nbsp;
        <1> isDriveC == true \n 
 */
void TestFmInternalFmUtils::testIsDriveCNo()
{
    QString pathToVerify ="A:\\ItisCDrive";
    bool isDriveC1 = FmUtils::isDriveC(pathToVerify);
    QVERIFY(isDriveC1 == false);
    
    bool isDriveC2 = FmUtils::isDriveC(QString());
    QVERIFY(isDriveC2 == false);
}
/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static int      getMaxFileNameLength();\n
     2. Case Descrition:  \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveName = QString() \n &nbsp;&nbsp;
        <2> driveName = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> isAccessable == false \n &nbsp;&nbsp;
        <2> return true if it's accessful, or return false \n 
 */
void TestFmInternalFmUtils::testGetMaxFileNameLength()
{
    QVERIFY( FmUtils::getMaxFileNameLength() == KMaxFileName );
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static bool     checkMaxPathLength( const QString& path );\n
     2. Case Descrition: Verify whether the path length is exceed max path length.  \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> driveName = QString() \n &nbsp;&nbsp;
        <2> driveName = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> isAccessable == false \n &nbsp;&nbsp;
        <2> return true if it's accessful, or return false \n 
 */
void TestFmInternalFmUtils::testCheckMaxPathLength()
{
    QString nameMaxPath;
    nameMaxPath.fill('x',320);
    QString nameNormalPath = "D:\\ThisIsNormalName";
    
    bool isMax1 = FmUtils::checkMaxPathLength(nameMaxPath);
    QVERIFY(isMax1 == false);
    
    bool isMax2 = FmUtils::checkMaxPathLength(nameNormalPath);
    QVERIFY(isMax2 == true);
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString getFileType( const QString &filePath  ); \n
     2. Case Descrition: verify the right file type is returned. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path =QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> return the right file type(e.g.: the file is "*.jpg", then return "image/jpeg" \n 
 */
void TestFmInternalFmUtils::testGetFileTypeWithSupportType()
{
    QString path1 = mFilePath + QString("Image.jpg");
    QString path2 = mFilePath + QString("Tone.mp3");
    
    QString retType = FmUtils::getFileType(path2);   
    QVERIFY( FmUtils::getFileType(path1) == "image/jpeg");
    QVERIFY( FmUtils::getFileType(path2) == "audio/mpeg");    
}
/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        static QString getFileType( const QString &filePath  ); \n
     2. Case Descrition: verify the funtion when using unsupported file type \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path =QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1>  \n 
 */
void TestFmInternalFmUtils::testGetFileTypeWithUnSupportType()
{
    QString path1 = mFilePath + QString("unknow.unknow");
    
    QString retType = FmUtils::getFileType(path1);   
//    QVERIFY( FmUtils::getFileType(path1) == ""); 
}
/*!
    Descrition of what you will do in this function
 */
void TestFmInternalFmUtils::cleanupTestCase()
{
}

QTEST_MAIN(TestFmInternalFmUtils)
