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
*     The source file of the file manager utilities
*/

#include "fmutils.h"

#include <QRegExp>
#include <QFileInfo>

#include <hbglobal.h>


/*!
    Used to get drive type for convenience.
*/
FmDriverInfo::DriveType FmDriverInfo::driveType()
{
    FmDriverInfo::DriveType driveType;
    if( mDriveState & FmDriverInfo::EDriveRemovable ) {
        // if drive is removable, it may be mass, usb, or remote drive
        if( mDriveState & FmDriverInfo::EDriveMassStorage ) {
            driveType = FmDriverInfo::EDriveTypeMassStorage;
        } else if( mDriveState & FmDriverInfo::EDriveUsbMemory ) {
            driveType = FmDriverInfo::EDriveTypeUsbMemory;
        } else if( mDriveState & FmDriverInfo::EDriveRemote ){
            driveType = FmDriverInfo::EDriveTypeRemote;
        } else {
            driveType = FmDriverInfo::EDriveTypeMemoryCard;
        }
    } else if( mDriveState & FmDriverInfo::EDriveRom ) {
        driveType = FmDriverInfo::EDriveTypeRom;
    } else if( mDriveState & FmDriverInfo::EDriveRam ) {
        driveType = FmDriverInfo::EDriveTypeRam;
    } else {
        // if drive type is not recognized in previous condition,
        // it should be an un-existed drive or phone memory
        if( mDriveState & EDriveNotPresent ){
            // if come here, means 2 conditions:
            //  1 this drive is not exist( FmUtils::queryDriverInfo will give EDriveNotPresent to mDriveState )
            //  2 un-mounted removable drive
            // removable drive filted in previous condifiton. So, give EDriveTypeNotExist 
            driveType = FmDriverInfo::EDriveTypeNotExist;
        } else {
            // drive is existed, only remain the type of EDriveTypePhoneMemory
            driveType = FmDriverInfo::EDriveTypePhoneMemory;
        }
    }
    
    return driveType;
}

/*!
    return drive name for \a path
*/
QString FmUtils::getDriveNameFromPath( const QString &path )
{
    // fillPathWithSplash make sure path length will be at least 3 if it is not empty.
    QString checkedPath( fillPathWithSplash( path ) );
    if( checkedPath.length() < 3 ) {
        return QString();
    }
    return checkedPath.left( 3 );
}

/*!
    return drive letter for \a path
*/
QString FmUtils::getDriveLetterFromPath( const QString &path )
{
	if( path.length() <2 ) {
        return QString();
    }
    return path.left( 1 );
}

/*!
    return formatted string for size.
*/
QString FmUtils::formatStorageSize( quint64 size )
{
	if ( size < 1000 ) {
		return QString::number( size ) + " B";
	} else if ( size < 1000 * 1000 ) {
		return QString::number( size / 1024.0, 'f', 2 ) + " KB";
	} else if ( size < 1000 * 1000 * 1000 ) {
		return QString::number( size / (1024.0 * 1024.0), 'f', 1 ) + " MB";
	} else {
	    return QString::number( size / ( 1024.0 * 1024.0 * 1024.0 ), 'f', 1 ) + " GB";	    
	}
}

/*!
    Judge if a path is drive path, not a folder or a file.
    For example, true for C:\ while false for C:\data
*/
bool FmUtils::isDrive( const QString &path )
{
   bool ret( false );
   if( path.length() <= 3 && path.length() >=2 ) {
       ret = true;
   }
   
   return ret;   
}

/*!
    All "/" and "\" in \a path will be changed to \a splitter
    QDir::separator is default value for splitter
    \sa fillPathWithSplash, fillPathWithSplash will append QDir::separator in the end
*/
QString FmUtils::formatPath( const QString &path, const QChar &splitter )
{
    QString formatPath;
    if( path.isEmpty() ) {
        return formatPath;
    }
    
    foreach( const QChar &ch, path ) {
        if( ch == QChar('\\') || ch == QChar('/') ) {
            formatPath.append( splitter );
        } else {
            formatPath.append( ch );
        }
    }

    return formatPath;
}

/*!
    Fill splash in the end of \a filePath. And all "/" and "\" will be changed to \a splitter
    QDir::separator is default value for splitter
    Please do not call this function if path is a file.
    Use \a formatPath instead, \a formatPath will not append QDir::separator in the end.
    \sa formatPath only changed "/" and "\" to QDir::separator
*/
QString FmUtils::fillPathWithSplash( const QString &filePath, const QChar &splitter )
{
    QString newFilePath;
    if( filePath.isEmpty() ) {
        return newFilePath;
    }

    newFilePath = formatPath( filePath, splitter );
    
    if( newFilePath.right( 1 )!= splitter ){
        newFilePath.append( splitter );
    }
    return newFilePath;
}

/*!
    Remove splash in the end of \a filePath.
*/
QString FmUtils::removePathSplash( const QString &filePath )
{
    QString newFilePath( filePath );
    if( filePath.right( 1 ) == QChar( '/' ) || filePath.right(1) == QString( "\\" ) ) {
        newFilePath = filePath.left( filePath.length() - 1 );
    }
    return newFilePath;
}

/*!
    fill \a driveList of drives can be shown in driveListView
*/
void FmUtils::getDriveList( QStringList &driveList, bool isHideUnAvailableDrive )
{
    if( isHideUnAvailableDrive ) {
        FM_LOG( QString( "getDriveList HideUnAvailableDrive_true" ) );
    } else {
        FM_LOG( QString( "getDriveList HideUnAvailableDrive_false" ) );
    }
    QFileInfoList infoList = QDir::drives();

    foreach( const QFileInfo &fileInfo, infoList ) {
        QString driveName = fileInfo.absolutePath();
        if( checkDriveAccessFilter( driveName ) ) {
            if( !isHideUnAvailableDrive ) {
                driveList.append( driveName );
            }
            else if ( isDriveAvailable( driveName ) ) {
                driveList.append( driveName );
            }
        }
    }
    return;
}

/*!
    return the combined string of drive name and volume( e.g. C: Phone memory for C drive)
    \a driveName the drive name.
    If set \a isFillWithDefaultVolume true, fill result with default volume if drive volume is empty.
*/
QString FmUtils::fillDriveVolume( QString driveName, bool isFillWithDefaultVolume )
{
    QString ret;
    if( driveName.isEmpty() ) {
        return ret;
    }
    QString tempDriveName = fillPathWithSplash( driveName );

    QString checkedDriveName( removePathSplash( driveName ) );
    
    FmDriverInfo driverInfo = FmUtils::queryDriverInfo( tempDriveName );
    QString volumeName = driverInfo.volumeName();
    
    if( volumeName.isEmpty() && isFillWithDefaultVolume ){
        switch ( driverInfo.driveType() )
            {
            case FmDriverInfo::EDriveTypeMassStorage:
                ret = hbTrId( "txt_fmgr_dblist_1_mass_storage" ).arg( checkedDriveName );
                break;
            case FmDriverInfo::EDriveTypeUsbMemory:
                ret = hbTrId( "txt_fmgr_dblist_1_usb_memory" ).arg( checkedDriveName );
                break;
            case FmDriverInfo::EDriveTypeMemoryCard:
                ret = hbTrId( "txt_fmgr_dblist_1_memory_card" ).arg( checkedDriveName );
                break;
            case FmDriverInfo::EDriveTypePhoneMemory:
                ret = hbTrId( "txt_fmgr_dblist_1_device_memory" ).arg( checkedDriveName );
                break;
            case FmDriverInfo::EDriveTypeNotExist:// no such drive
            default:// do not need get volume for other drive types, e.g. Ram, Rom
                break;
            }    
    }
    
    if( ret.isEmpty() ) {
        // ret is not got. fill ret as default method
        // txt_fmgr_dblist_1_2 is not correct, can not use.
        ret = hbTrId( "%1 %2" ).arg( checkedDriveName ).arg( volumeName );
    }
    return ret;
}

/*!
    return the default volume for drive
    \a driveName the drive.
    \sa getVolumeNameWithDefaultNameIfNull
*/
QString FmUtils::getDefaultVolumeName( const QString &driveName )
{
    QString defaultVolumeName;
    FmDriverInfo driverInfo = FmUtils::queryDriverInfo( driveName );
          
    switch ( driverInfo.driveType() )
        {
        case FmDriverInfo::EDriveTypeMassStorage:
            defaultVolumeName = hbTrId("Mass storage"); 
            break;
        case FmDriverInfo::EDriveTypeUsbMemory:
            defaultVolumeName = hbTrId("USB memory"); 
            break;
        case FmDriverInfo::EDriveTypeMemoryCard:
            defaultVolumeName = hbTrId("Memory card");
            break;
        case FmDriverInfo::EDriveTypePhoneMemory:
            defaultVolumeName = hbTrId("Device memory");
            break;
        case FmDriverInfo::EDriveTypeNotExist:// no such drive, break and return empty string
        default:// do not have default volume for other drive types, e.g. Ram, Rom
            break;
        }   
    return defaultVolumeName;
}

/*!
    get the volume name of the drive, if it is null, then return the default name.
    \a driveName the drive letter.
    \a defaultName whether it is the default name.
    return the volume name.
*/
QString FmUtils::getVolumeNameWithDefaultNameIfNull( const QString &driveName, bool &defaultName )
{
    // do not re-use FmUtils::getDefaultVolumeName to save one time for call FmUtils::queryDriverInfo
    FmDriverInfo driverInfo = FmUtils::queryDriverInfo( driveName );
          
    QString volumeName = driverInfo.volumeName();    
    //save the volume status, whether it is default name
    defaultName = false;
    //volume name may be null if not set, it will be set at least for one time in the following while cycling.
    if ( volumeName.isEmpty() ) {
        switch ( driverInfo.driveType() )
            {
            case FmDriverInfo::EDriveTypeMassStorage:
                volumeName = hbTrId("Mass storage"); 
                break;
            case FmDriverInfo::EDriveTypeUsbMemory:
                volumeName = hbTrId("USB memory"); 
                break;
            case FmDriverInfo::EDriveTypeMemoryCard:
                volumeName = hbTrId("Memory card");
                break;
            case FmDriverInfo::EDriveTypePhoneMemory:
                volumeName = hbTrId("Device memory");
                break;
            case FmDriverInfo::EDriveTypeNotExist:// no such drive
            default:// do not need get volume for other drive types, e.g. Ram, Rom
                break;
            }   
        if( !volumeName.isEmpty() ) {
             defaultName = true;
        }
    }
    return volumeName;
}

/*!
    Check if two path are the same.
*/
bool FmUtils::isPathEqual( const QString &pathFst, const QString &pathLast )
{
    QString fst( fillPathWithSplash( pathFst ) );
    QString last( fillPathWithSplash( pathLast ) );
    if( fst.compare( last, Qt::CaseInsensitive ) == 0 ) {
        return true;
    }
    return false;
}

/*!
    Check if a name is illegal for folder/file naming rule.
*/
bool FmUtils::checkFolderFileName( const QString& name )
{
    // trim space firest, because there may be some spaces after "." ,  it is also not valid
    QString trimmedName( name.trimmed() );
	if( trimmedName.isEmpty() ) {
		return false;
	}
    if( trimmedName.endsWith( QChar('.'),  Qt::CaseInsensitive ) ) {
        return false;
    }
    if( trimmedName.contains( QChar('\\'), Qt::CaseInsensitive ) ||
        trimmedName.contains( QChar('/'),  Qt::CaseInsensitive ) ||
        trimmedName.contains( QChar(':'),  Qt::CaseInsensitive ) ||
        trimmedName.contains( QChar('*'),  Qt::CaseInsensitive ) ||
        trimmedName.contains( QChar('?'),  Qt::CaseInsensitive ) ||
        trimmedName.contains( QChar('\"'), Qt::CaseInsensitive ) ||
        trimmedName.contains( QChar('<'),  Qt::CaseInsensitive ) ||
        trimmedName.contains( QChar('>'),  Qt::CaseInsensitive ) ||
        trimmedName.contains( QChar('|'),  Qt::CaseInsensitive ) ){
        return false;
    }
    // use original name to exam max size of file name
    if( name.length() > getMaxFileNameLength() ) {
        return false;
    }
    return true;
}

/*!
    check file or folder path is illegal or not.
    \a fileName file/folder name, used to check illegal characters
    \a path file/folder path, used to check if path is available to use.
    \a errString if return false, errString will be set for error note.
    return true for not illegal and false for illegal path.
*/
bool FmUtils::checkNewFolderOrFile( const QString &fileName, const QString &path, QString &errString )
{
    // first check if fileName is valid, then check if path length is valid, and check if file/foler is existed at last
    QFileInfo fileInfo( path );
    bool ret( true );   
    if (!FmUtils::checkFolderFileName( fileName ) ) {
        errString = hbTrId( "Invalid file or folder name!" );
        ret = false;
    } else if( !FmUtils::checkMaxPathLength( path ) ) {
        errString = hbTrId( "the path you specified is too long!" );
        ret = false;
    } else if (fileInfo.exists()) {
        errString = hbTrId( "%1 already exist!" ).arg( fileInfo.fileName() );
        ret = false;
    }
    return ret;
}

/*!
    Check if \a dest is sub level path of \a src
    Used to check True/False when copy a folder to itself or its subfolder
    For example, c:\data\test is sub path of c:\data.
    But c:\data123\test is not sub path of c:\data.
    So after got right part of path, the first char must be \ or /
*/
bool FmUtils::isSubLevelPath( const QString &src, const QString &dest )
{
    FM_LOG("FmUtils::isSubFolder: src=" + src + " dest=" + dest);
    QString checkedSrc( FmUtils::fillPathWithSplash( src ) );
    QString checkedDest( FmUtils::fillPathWithSplash( dest ) );
    
    if( checkedDest.contains( checkedSrc, Qt::CaseInsensitive) &&
            checkedDest.length() > checkedSrc.length() ) {
        // for example c:\data\ vs c:\data\123\ 
        FM_LOG("FmUtils::isSubFolder: true");
        return true;
    }
    // for example c:\data\ vs c:\data\ 
    // for example c:\data\ vs c:\data123\ 

    FM_LOG("FmUtils::isSubFolder: false");
    return false;
}

/*!
    Check if \a path is system path.
*/
bool FmUtils::isSystemFolder( const QString &path )
{
    QFileInfo fileInfo( path );
    if( fileInfo.isDir() ) {
        QString checkedPath( fillPathWithSplash( path, Char_Slash ) );

        QRegExp systemFolder  ( RegexWidecard_SystemFolder,   Qt::CaseInsensitive, QRegExp::Wildcard );
        QRegExp sysFolder     ( RegexWidecard_SysFolder,      Qt::CaseInsensitive, QRegExp::Wildcard );
        QRegExp privateFolder ( RegexWidecard_PrivateFolder,  Qt::CaseInsensitive, QRegExp::Wildcard );
        QRegExp resourceFolder( RegexWidecard_ResourceFolder, Qt::CaseInsensitive, QRegExp::Wildcard );

        if( systemFolder.exactMatch( checkedPath ) ||
            sysFolder.exactMatch( checkedPath ) ||
            privateFolder.exactMatch( checkedPath ) ||
            resourceFolder.exactMatch( checkedPath ) ) {
            return true;
        }
    }
    return false;
}
