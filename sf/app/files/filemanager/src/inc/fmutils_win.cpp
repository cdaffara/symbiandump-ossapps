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
*     The source file of the file manager utilities on windows
*/

#include "fmutils.h"

#include <windows.h>

#include <QDir>
#include <QUrl>
#include <QVariant>
#include <QFileInfo>
#include <QDesktopServices>

#include <hbglobal.h>

#define BURCONFIGFILE  "burconfig.xml"
const int KMaxFileName=0x100;
const int KMaxPath=0x100;

// EDriveA means number of A drive
const int EDriveA = 0;

FmDriverInfo FmUtils::queryDriverInfo( const QString &driveName )
{
    quint64 size = 0;
    quint64 freeSize = 0;
    QString drive = driveName;
    
    drive.replace( '/', "\\" );
    if ( drive.right(1) != "\\" ) {
        drive.append( "\\" );
    }
    GetDiskFreeSpaceEx( (LPCWSTR)drive.constData(),
                        (PULARGE_INTEGER)&freeSize,
                        (PULARGE_INTEGER)&size,
                        0 );

    TCHAR volumeName[MAX_PATH + 1] = { 0 };
    GetVolumeInformation( (LPCWSTR)drive.constData(),
                          &volumeName[0],
                          MAX_PATH + 1,
                          0,
                          0,
                          0,
                          0,
                          0 );

    quint32 state( 0 );
    quint32 drvStatus = GetDriveType( (LPCWSTR)drive.constData() );
    if ( drvStatus == DRIVE_REMOVABLE  ) {
        state |= FmDriverInfo::EDriveRemovable;
    }

	if( !(state&FmDriverInfo::EDriveNotPresent) && !(state&FmDriverInfo::EDriveLocked) &&
		!(state&FmDriverInfo::EDriveCorrupted) ) {
		state |= FmDriverInfo::EDriveAvailable;
	}
    return FmDriverInfo( size, freeSize, driveName, QString::fromWCharArray( &volumeName[0] ), state );
}

int FmUtils::removeDrivePwd( const QString &driveName,  const QString &Pwd )
{
    Q_UNUSED( driveName );
    Q_UNUSED( Pwd );
    return 0;
}

int FmUtils::unlockDrive( const QString &driveName,  const QString &Pwd )
{
    Q_UNUSED( driveName );
    Q_UNUSED( Pwd );
    return 0;
}

int FmUtils::checkDrivePwd( const QString &driveName, const QString &pwd)
{
    Q_UNUSED( driveName );
    Q_UNUSED( pwd );
    return 0;
}

int FmUtils::setDrivePwd( const QString &driveName, const QString &oldPwd, const QString &newPwd)
{
    Q_UNUSED( driveName );
    Q_UNUSED( oldPwd );
    Q_UNUSED( newPwd );
    return 0;
}

void FmUtils::emptyPwd( QString &pwd )
{
    Q_UNUSED( pwd );
}

int FmUtils::renameDrive( const QString &driveName, const QString &newVolumeName)
{
    Q_UNUSED( driveName );
    foreach( const QChar &ch, newVolumeName )
    {
        // If not alphadigit or space, return error
        if( !ch.isLetterOrNumber() && !ch.isSpace() )
        {
            return FmErrBadName;
        }   
    }
    return 0;
}

int FmUtils::ejectDrive( const QString &driveName )
{
    Q_UNUSED( driveName );
	return FmErrNone; 
}

bool FmUtils::checkDriveAccessFilter( const QString &driveName )
{
    Q_UNUSED( driveName );
    return true;
}

QString FmUtils::checkDriveToFolderFilter( const QString &path )
{
    Q_UNUSED( path );
    return path;

}

QString FmUtils::checkFolderToDriveFilter( const QString &path )
{
    Q_UNUSED( path );
    return path;

}

int FmUtils::isPathAccessabel( const QString &path )
{
    Q_UNUSED( path );
    return FmErrNone;
}

bool FmUtils::isDriveAvailable( const QString &path )
{
    Q_UNUSED( path );
	return true;
}

bool FmUtils::isDefaultFolder( const QString &folderPath  )
{
    Q_UNUSED( folderPath );
    return false;
}

void FmUtils::createDefaultFolders( const QString &driveName )
{
    Q_UNUSED( driveName );
}

/*!
    return localized file name by \a path, empty string returned for non-localized path.
    In Symbian system, default folders will be localized.
    So localize also can be used to check if a path is a default folder
    Do not need localize on windows.
    \sa isDefaultFolder
*/
QString FmUtils::localize( const QString &path )
{
    Q_UNUSED( path );
    return QString();
}

/*!
    set the \a desFile attributes as the same with \a srcFile
*/
int FmUtils::setFileAttributes( const QString &srcFile, const QString &desFile )
{
    Q_UNUSED( srcFile );
    Q_UNUSED( desFile );
    return FmErrNone;
}

/*!
    judge whether there is enough space on \a targetDrive for \a size.
    return true if has, false if not.
*/
bool FmUtils::hasEnoughSpace( const QString &targetDrive, qint64 size )
{
    Q_UNUSED( targetDrive );
    Q_UNUSED( size );
    return true;
}

/*!
    move one file insice the same drive, from \a source to \a target.
    return KErrNone if successful, otherwise one of the other system-wide error codes.
*/
int FmUtils::moveInsideDrive( const QString &source, const QString &target )
{
    Q_UNUSED( source );
    Q_UNUSED( target );
    return FmErrNone;
}

int FmUtils::launchFile( const QString &filePath )
{
    if( QDesktopServices::openUrl( QUrl::fromLocalFile( filePath ) ) ) {
        return FmErrNone;
    } else {
        return FmErrGeneral;
    }
}

QString FmUtils::getBurConfigPath( QString appPath )
{
    QFileInfo fileInfo( appPath );
    QString testString = fileInfo.absolutePath();
    QString path = fillPathWithSplash( fileInfo.absolutePath() );
    path = path + QString( "src/filemanager/" );
    path = path + QString( BURCONFIGFILE );
    return path;
}

QString FmUtils::getFileType( const QString &filePath  )
{
    Q_UNUSED( filePath );
    return QString( "" );
}

bool FmUtils::isDriveC( const QString &driveName )
{
	if( driveName.contains(Drive_C,Qt::CaseInsensitive) ){
        return true;
    }
    else{
        return false;
    }
}

int FmUtils::getMaxFileNameLength()
{
    return KMaxFileName;
}

bool FmUtils::checkMaxPathLength( const QString& path )
{
    if( path.length() > KMaxPath ) {
        return false;
    }
    return true;
}

void FmUtils::setSystem( bool isSystem )
{
    Q_UNUSED( isSystem );
}

QString FmUtils::numberToDriveName( int drive )
{
    QChar driveChar( drive - EDriveA + 'A' );
    QString driveName = QString( driveChar ) + ':';
    return driveName;
}
