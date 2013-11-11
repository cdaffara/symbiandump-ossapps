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
*     Zhiqiang Yang <zhiqiang.yang@nokia.com>
* 
* Description:
*     The header file of the file manager utilities
*/

#ifndef FMUTILS_H
#define FMUTILS_H

#include "fmcommon.h"

#include <QDir>
#include <QString>

/*!
    \class FmDriverInfo
    \brief The class FmDriverInfo provide drive info data
 */
class FmDriverInfo
{
public:
    /*!
        Used to get drive status for convenience.
        /sa DriveType is used to get drive type.
    */
    enum driveState
    {
        EDriveNotPresent = 0x1, // true when Drive have not inserted, for example, MMC Card
        EDriveAvailable = 0x2,  // false when drive is locked or corrupted, for example MMC Card
        EDriveLocked = 0x4,
        EDriveCorrupted = 0x8,
        EDriveWriteProtected = 0x10,
        EDriveRemovable = 0x20,
        EDriveRom = 0x40,
        EDriveFormattable = 0x80,
        EDriveFormatted = 0x100,
        EDriveLockable = 0x200,
        EDrivePasswordProtected = 0x400,
        EDriveBackupped = 0x800,
        EDriveConnected = 0x1000,
        EDriveEjectable = 0x2000,
        EDriveInUse = 0x4000,
        EDriveMassStorage = 0x8000,
        EDriveRam = 0x10000,
        EDriveUsbMemory = 0x20000,
        EDriveRemote = 0x40000,
     };
    
    /*!
        Used to get drive type for convenience.
    */
    enum DriveType
    {
        EDriveTypeNotExist = -1, // specified drive is not existed in device
        EDriveTypeRom,
        EDriveTypeRam,
        EDriveTypePhoneMemory,
        EDriveTypeMassStorage,
        EDriveTypeMemoryCard,
        EDriveTypeUsbMemory,
        EDriveTypeRemote,
    };
    
    Q_DECLARE_FLAGS( DriveState, driveState )
    
    FmDriverInfo( quint64 s, quint64 f, const QString &n, const QString &vN, const quint32 driveState ) :
        mSize( s ), mFreeSize( f ), mName( n ), mVolumeName( vN ), mDriveState( driveState ) {}
    FmDriverInfo( const FmDriverInfo &other )
    {
        *this = other;
    }

    FmDriverInfo &operator= ( const FmDriverInfo &rhs )
    {
        this->mSize = rhs.mSize;
        this->mFreeSize = rhs.mFreeSize;
        this->mName = rhs.mName;
        this->mVolumeName = rhs.mVolumeName;
        this->mDriveState = rhs.mDriveState;
        return *this;
    }
    
    /*!
        Total size for drive
    */
    quint64 size() const { return mSize; }
    
    /*!
        Free size for drive
    */
    quint64 freeSize() const { return mFreeSize; }
    
    /*!
        path for drive
    */
    QString name() const { return mName; }
    
    /*!
        volume for drive
    */
    QString volumeName() const { return mVolumeName; }
    
    /*!
        status for drive
    */
    DriveState driveState() const { return mDriveState; }
    
    /*!
        drive type
    */
    DriveType driveType();
    
private:
    quint64 mSize;
    quint64 mFreeSize;
    QString mName;
    QString mVolumeName;
    DriveState mDriveState;
    DriveType mDriveType;
};
Q_DECLARE_OPERATORS_FOR_FLAGS( FmDriverInfo::DriveState )

/*!
    \class FmUtils
    \brief The class FmUtils provide util functions
    This class is implemented by three parts:
    fmutils.cpp for Qt Based functions
    fmutils_s60.cpp is symbian implement version for platform based functions
    fmutils_win.cpp is win32 implement version for platform based functions( a simple wrapper )
 */
class FmUtils
{
public:
    //////////////////////////////////////////////////////////////////
    // <QtBasedFunctions>
    static QString  getDriveNameFromPath( const QString &path );
    static QString  getDriveLetterFromPath( const QString &path );
    static QString  formatStorageSize( quint64 size );
    static bool     isDrive( const QString &path );
    static QString  formatPath( const QString &path, const QChar &splitter = QDir::separator() );
    static QString  fillPathWithSplash( const QString &filePath, const QChar &splitter = QDir::separator() );
    static QString  removePathSplash( const QString &filePath );
    static void     getDriveList( QStringList &driveList, bool isHideUnAvailableDrive );
    static QString  fillDriveVolume( QString driveName, bool isFillWithDefaultVolume );
    static QString  getDefaultVolumeName( const QString &driveName );
    static QString  getVolumeNameWithDefaultNameIfNull( const QString &diskName, bool &defaultName );
    static bool     isPathEqual( const QString &pathFst, const QString &pathLast );
    static bool     checkFolderFileName( const QString& name );
    static bool     checkNewFolderOrFile( const QString& fileName, const QString &path, QString &errString );
    static bool     isSubLevelPath( const QString &src, const QString &dest );
    static bool     isSystemFolder( const QString &path );
    // </QtBasedFunctions>
    //////////////////////////////////////////////////////////////////
    
    
    //////////////////////////////////////////////////////////////////
    // <platformBasedFunctions>
    static FmDriverInfo queryDriverInfo( const QString &driverName );
    
    // password related functions
    static int      removeDrivePwd( const QString &driverName, const QString &Pwd );
    static int      unlockDrive( const QString &driverName, const QString &Pwd );
    static int      checkDrivePwd( const QString &driverName, const QString &pwd);
    static int      setDrivePwd( const QString &driverName, const QString &oldPwd, const QString &newPwd);
    static void     emptyPwd( QString &pwd );
    
    // external drive related functions
    static int      renameDrive( const QString &driverName, const QString &newVolumeName);
    static int      ejectDrive( const QString &driverName );
    
    // drive security related functions
    static bool     checkDriveAccessFilter( const QString &driveName );
    static QString  checkDriveToFolderFilter( const QString &path );
    static QString  checkFolderToDriveFilter( const QString &path );
    static int      isPathAccessabel( const QString &path );
    static bool     isDriveAvailable( const QString &path );
    
    // system default folder related functions
    static bool     isDefaultFolder( const QString &folderPath  );
    static void     createDefaultFolders( const QString &driverName );
    static QString  localize( const QString &path );
        
    // copy move related functions
    static int      setFileAttributes( const QString &srcFile, const QString &desFile );
    static bool     hasEnoughSpace( const QString &targetDrive, qint64 size );
    static int      moveInsideDrive( const QString &source, const QString &target );
	
    // other fucntions
    static int      launchFile( const QString &filePath );
    static QString  getBurConfigPath( QString appPath );
    static QString  getFileType( const QString &filePath  );
    static bool     isDriveC( const QString &driverName );
    static int      getMaxFileNameLength();
    static bool     checkMaxPathLength( const QString& path );
    static void     setSystem( bool isSystem );
    static QString  numberToDriveName( int drive );
    // </platformBasedFunctions>
    //////////////////////////////////////////////////////////////////
};

#endif

