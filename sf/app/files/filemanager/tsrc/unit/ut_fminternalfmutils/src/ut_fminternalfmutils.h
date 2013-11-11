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
*       test application for File manager internal apis.
*/

#ifndef UT_FMINTERNALFMUTILS_H_
#define UT_FMINTERNALFMUTILS_H_

#include <QObject>
#include <QStringList>
#include <hbtranslator.h>
#include <QTranslator>
class TestFmInternalFmUtils :public QObject
{
    Q_OBJECT
private slots:    
    void initTestCase();    
    
    void testGetDriveNameFromPath(); //test the getDriveNameFromPath() and fillPathWithSplash() functions.
    
    void testGetDriveLetterFromPath(); // test the getDriveLetterFromPath() function.
    
    void testFormatStorageSize(); // test the formatStorageSize() function.
    
    void testIsDriveYes(); // test isDrive() function with normal drive path
    void testIsDriveNo(); // test isDrive() function with an unnormal drive path
    
    void testFormatPathWithEmptyPath(); // test formatPath() with empty path
    void testFormatPath(); // test formatPath() using path with "/" or "\"
    
    void testFillPathWithSplashUseEmptyPath(); // fillPathWithSplash()
    void testFillPathWithSplashHasSplash(); // fillPathWithSplash()
    void testFillPathWithSplashWithoutSplash(); // fillPathWithSplash()
    
    void testRemovePathSplashUseEmptyPath(); // removePathSplash()
    void testRemovePathSplashHasSplash(); // removePathSplash()
    void testRemovePathSplashWithoutSplash(); // removePathSplash()
    
    void testGetDriveListAll();//getDriveList()
    void testGetDriveListPart();//getDriveList()
    
    void testFillDriveVolumeEmptyName(); // fillDriveVolume()
    void testFillDriveVolumeDefaultVolume(); // fillDriveVolume()
    void testFillDriveVolume(); // fillDriveVolume()
    
    void testGetDefaultVolumeName(); // test getDefaultVolumeName() funtion
    
    void testGetVolumeNameWithDefaultNameIfNull();
    
    void testisPathEqualWithSamePath(); // test isPathEqual() with same path parameter
    void testisPathEqualWithDiffPath(); // test isPathEqual() with different path parameter
    
    void testcheckFolderFileNameNormal(); // test checkFolderFileName() with valid name
    void testcheckFolderFileNameUnNormal(); //test checkFolderFileName() with invalid name
    
    void testCheckNewFolderOrFileUnNormal(); // test checkNewFolderOrFile() function
    void testCheckNewFolderOrFileIlegel();
    void testCheckNewFolderOrFileNo();
    
    void testIsSubLevelPathYes(); // test isSubLevelPath() function
    void testIsSubLevelPathNo();
    
    void testRemoveDrivePwdNormal(); // test the removeDrivePwd() function
    void testRemoveDrivePwdUnNormal(); 
    
    void testSetDrivePwdNormal(); // test the setDrivePwd() function.
    void testSetDrivePwdUnNormal(); 
    
    void testCheckDrivePwd(); // test the checkDrivePwd() function.
    void testEmptyPwd(); 
    
    void testRenameDriveWithBadName(); // test renameDrive() function with bad names.
    void testRenameDriveWithNormalName(); // test renameDrive() function with normal names.
    
    void testCheckDriveAccessFilter(); // test the checkDriveAccessFilter() function.
    
    void testCheckDriveToFolderFilterC(); // test the checkDriveToFolderFilter() function.
    void testCheckDriveToFolderFilterNormal();
    
    void testCheckFolderToDriveFilterC(); // test CheckFolderToDriveFilter() function.
    void testCheckFolderToDriveFilterNormal();
    
    void testIsPathAccessableEmpty();
    void testIsPathAccessableOK();
    void testIsPathAccessableNo();
    
    void testIsDriveAvailable(); // test the isDriveAvailable() function.
    
    void testIsDefaultFolder(); // test the isDefaultFolder() function.
    
    void testCreateDefaultFolders(); // test the createDefaultFolders() function.
    
    void testSetFileAttributes(); // test teh setFileAttributes() function.
    
    void testHasEnoughSpaceYes(); // test the hasEnoughSpace() function.
    void testHasEnoughSpaceNo();
    
    void testMoveInsideDrive(); // test the moveInsideDrive() function.
    
    void testLaunchFile(); // test the launchFile() funtion.
    
    void testSendFiles();
    
    void testGetBurConfigPath();
    
    void testGetFileTypeWithSupportType(); // test the GetFileType() function using file manager supported types. 
    void testGetFileTypeWithUnSupportType();
    
    void testIsDriveCYes();
    void testIsDriveCNo();
    
    void testGetMaxFileNameLength();
    
    void testCheckMaxPathLength();
    
    void cleanupTestCase();    
private:
    QString mFilePath;
    QStringList mDrives; // all drives
    QStringList mDrives1;// drives which type is USB or MemCard
    QStringList mDrives2;// drives except USB and MemCard
    
    QTranslator mTranslator;
};
#endif /* UT_FMINTERNALFMUTILS_H_ */
