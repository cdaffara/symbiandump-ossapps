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
*     The source file of the file manager utilities on Symbian
*/


#include "fmutils.h"
#include "fmcommon.h"
#include "fms60utils.h"

#include <COEMAIN.H>
#include <eikenv.H> 
#include <apgcli.h>
#include <sysutil.h>
#include <f32file.h>
#include <coemain.h>
#include <pathinfo.h>
#include <driveinfo.h>
#include <e32property.h>
#include <coreapplicationuisdomainpskeys.h>

#include <QDir>
#include <QFile>
#include <QRegExp>
#include <QStringList>
#include <QFileInfoList>

#include <xqappmgr.h>
#include <XQConversions>
#include <xqaiwrequest.h>

#include <hbglobal.h>
#include <hbdirectorynamelocalizer.h>

#define BURCONFIGFILE  "z:/private/2002BCC0/burconfig.xml"

/*!
    query drive info and status for \a driveName
    return \a FmDriverInfo
*/
FmDriverInfo FmUtils::queryDriverInfo( const QString &driveName )
{
    if( driveName.isEmpty() ) {
        return FmDriverInfo( 0, 0, driveName, QString(), FmDriverInfo::EDriveNotPresent );
    }
    //    CCoeEnv *env = CCoeEnv::Static();
    //    RFs& fs = env->FsSession();
    // make new fs so that this function can be called in thread
    RFs fs;
    fs.Connect();

    TVolumeInfo volumeInfo;
    TInt drive = 0;
    drive = driveName[0].toUpper().toAscii() - 'A' + EDriveA;

    quint32 state( 0 );
    
    int volumeInfoErr( KErrNone );
    int driveInfoErr( KErrNone );
    int errorCode( KErrNone );
    volumeInfoErr = fs.Volume( volumeInfo, drive );
    errorCode = volumeInfoErr;
    QString volumeName( (QChar*)( volumeInfo.iName.Des().Ptr() ), volumeInfo.iName.Length() );
    
    TDriveInfo driveInfo;
    if( volumeInfoErr == KErrNone ) {
        driveInfo = volumeInfo.iDrive;
    } else {
        driveInfoErr = fs.Drive( driveInfo, drive );
        if( driveInfoErr != KErrNone ) {
            errorCode = driveInfoErr;
        }
    }
    
    if( volumeInfoErr == KErrNone || driveInfoErr == KErrNone ) {
        //TDriveInfo driveInfo = volumeInfo.iDrive;
    
        quint32 drvStatus( 0 );
        int err = DriveInfo::GetDriveStatus( fs, drive, drvStatus );
        if( err == KErrNone ) {
            
            if ( ( drvStatus & DriveInfo::EDriveInternal ) &&
                 ( drvStatus & DriveInfo::EDriveExternallyMountable ) ){
                // Handle mass storage bits here
                state |= FmDriverInfo::EDriveMassStorage | FmDriverInfo::EDriveRemovable;
            }
        
            if ( drvStatus & DriveInfo::EDriveUsbMemory )
                {
                state |= FmDriverInfo::EDriveUsbMemory;
                }
            
            if ( drvStatus & DriveInfo::EDriveRemote )
                {
                state |= FmDriverInfo::EDriveRemote;
                }
            
            if ( drvStatus & DriveInfo::EDriveRom ){
                state |= FmDriverInfo::EDriveRom;  
            }
            
            if ( drvStatus & DriveInfo::EDriveRam ){
                state |= FmDriverInfo::EDriveRam;  
            }
        
            if ( driveInfo.iMediaAtt & KMediaAttFormattable ){
                state |= FmDriverInfo::EDriveFormattable;
            }
            if ( driveInfo.iMediaAtt & KMediaAttWriteProtected ){
                state |= FmDriverInfo::EDriveWriteProtected;
            }
            if ( driveInfo.iMediaAtt & KMediaAttHasPassword ){
                state |= FmDriverInfo::EDrivePasswordProtected;
            }    
            if ( driveInfo.iMediaAtt & KMediaAttLocked ){
                state |= FmDriverInfo::EDriveLocked;
            }
        
            if ( driveInfo.iDriveAtt & KDriveAttRemovable ){
                state |= FmDriverInfo::EDriveRemovable;
        
                if ( drvStatus & DriveInfo::EDriveSwEjectable ){
                    state |= FmDriverInfo::EDriveEjectable;
                }
            }
            
            if( driveInfo.iType == EMediaNotPresent ){
                state |= FmDriverInfo::EDriveNotPresent;    
            }
        }
        // If memory card is not ready but type is present,
        // then check if it is reserved.
        if( err == KErrNone && volumeInfoErr == KErrNotReady &&
            driveInfo.iType != EMediaNotPresent )
            {
            // Check USB file transfer state
            TInt prop( ECoreAppUIsUSBFileTransferUninitialized );
            RProperty::Get(
                KPSUidCoreApplicationUIs,
                KCoreAppUIsUSBFileTransfer, prop );
            if ( prop == ECoreAppUIsUSBFileTransferActive )
                {
                errorCode = KErrInUse; // Reserved for file transfer
                }
            }
        if( err!= KErrNone )
            {
            errorCode = err;
            }
    }

    // handle error code
    // volumeInfoErr will occur while drive is lock,corrupted...
    // driveInfoErr can not be promoted for locked, corrupted drive.
    // so we can not use driveInfoErr to justify EDriveAvailable
    switch( errorCode )
    {
    case KErrNone:
        // this drive could be used as it is not be locked, or corrupt.
        state |= FmDriverInfo::EDriveAvailable; 
        break;
    case KErrLocked:
        state |= FmDriverInfo::EDriveLocked;
        break;
    case KErrCorrupt:
        state |= FmDriverInfo::EDriveCorrupted;
        break;
    case KErrInUse:
        state |= FmDriverInfo::EDriveInUse;
        break;
    default: // other errors
        state |= FmDriverInfo::EDriveNotPresent;
        break;
    }
    QString logString ( "FmUtils::queryDriverInfo_" + driveName + 
            "_volumeInfoErr:" + QString::number( volumeInfoErr ) +
            "_driveInfoErr:" + QString::number( driveInfoErr ) +
            "_errorCode:" + QString::number( errorCode ) + 
            "_driveSatus:" + QString::number( state ) );
    FM_LOG( logString );
    fs.Close();
    return FmDriverInfo( volumeInfo.iSize, volumeInfo.iFree, driveName, volumeName, state );
}

/*!
    remove drive password for \a driveName
    \a Pwd is original password.
*/
int FmUtils::removeDrivePwd( const QString &driveName,  const QString &Pwd )
{
    if( driveName.isEmpty() || Pwd.length() > FmMaxLengthofDrivePassword ) {
        return FmErrWrongParam;
    }
    QString logString = "Drive name:" + driveName;
    FM_LOG( logString );
    logString = "Password:" + Pwd;
    FM_LOG( logString );

    CCoeEnv *env = CCoeEnv::Static();
	RFs& fs = env->FsSession();

    TInt drive = 0;

    drive = driveName[0].toUpper().toAscii() - 'A' + EDriveA;

    HBufC* password16 = XQConversions::qStringToS60Desc( Pwd );
    TMediaPassword password;   
    TPtr ptrPassword16( password16->Des() );  
    FmS60Utils::ConvertCharsToPwd( ptrPassword16, password );

    int err( fs.ClearPassword( drive, password ) );

    logString = "Drive:" + QString::number( drive );
    FM_LOG( logString );

    logString = "Clear password error:" + QString::number( err );
    FM_LOG( logString );

    delete password16;

    if( err == KErrNone ){
        return FmErrNone;   
    }
    else if( err == KErrAccessDenied ){
        return FmErrAccessDenied;
    }
    else{
        return FmErrUnKnown;
    }
}

/*!
    Unlock drive \a driveName with provided password \a Pwd
*/
int FmUtils::unlockDrive( const QString &driveName,  const QString &Pwd )
{
    if( driveName.isEmpty() || Pwd.length() > FmMaxLengthofDrivePassword ) {
        return FmErrWrongParam;
    }
    QString logString = "Drive name:" + driveName;
    FM_LOG( logString );
    logString = "Password:" + Pwd;
    FM_LOG( logString );

    CCoeEnv *env = CCoeEnv::Static();
	RFs& fs = env->FsSession();

    TInt drive = 0;
	drive = driveName[0].toUpper().toAscii() - 'A' + EDriveA;
    
    HBufC* password16 = XQConversions::qStringToS60Desc( Pwd );
    TMediaPassword password;   
    TPtr ptrPassword16( password16->Des() );  
    FmS60Utils::ConvertCharsToPwd( ptrPassword16, password );

    int err( fs.UnlockDrive( drive, password, ETrue) );

    logString = "Drive:" + QString::number( drive );
    FM_LOG( logString );
    logString = "Unlock drive error:" + QString::number( err );
    FM_LOG( logString );

    delete password16;

    if( err == KErrNone ){
        return FmErrNone;   
    }
    else if( err == KErrAccessDenied ){
        return FmErrAccessDenied;
    }
    else if( err == KErrAlreadyExists ){
        return FmErrAlreadyExists;
    }
    else if( err == KErrNotSupported ){
        return FmErrNotSupported;
    }
    else{
        return FmErrUnKnown;
    }
}

/*!
    Check if \a pwd is the right password for drive \a driveName
*/
int FmUtils::checkDrivePwd( const QString &driveName, const QString &pwd )
{
    if( driveName.isEmpty() || pwd.length() > FmMaxLengthofDrivePassword ) {
        return FmErrWrongParam;
    }
    QString logString = "checkDrivePwd Drive name:" + driveName;
    logString += " password:" + pwd;
    FM_LOG( logString );

    return setDrivePwd( driveName, pwd, pwd );
}

/*!
    Set new password \a newPwd for drive \a driveName. \a oldPwd is old password
*/
int FmUtils::setDrivePwd( const QString &driveName, const QString &oldPwd, const QString &newPwd)
{
    if( driveName.isEmpty() || 
        oldPwd.length() > FmMaxLengthofDrivePassword || newPwd.length() > FmMaxLengthofDrivePassword  ) {
        return FmErrWrongParam;
    }
    QString logString = "setDrivePwd Drive name:" + driveName ;
    logString += " Old password:" + oldPwd;
    logString += " New password:" + newPwd;
    FM_LOG( logString );

    CCoeEnv *env = CCoeEnv::Static();
	RFs& fs = env->FsSession();

    TInt drive = 0;
	drive = driveName[0].toUpper().toAscii() - 'A' + EDriveA;
	
    HBufC* newPassword16 = XQConversions::qStringToS60Desc( newPwd);
    HBufC* oldPassword16 = XQConversions::qStringToS60Desc( oldPwd );

    TMediaPassword oldPassword;
    TMediaPassword newPassword;
    
    TPtr ptrNewPassword16( newPassword16->Des() );
    TPtr ptrOldPassword16( oldPassword16->Des() );
    
    FmS60Utils::ConvertCharsToPwd( ptrNewPassword16, newPassword );
    FmS60Utils::ConvertCharsToPwd( ptrOldPassword16, oldPassword );

    int err( fs.LockDrive( drive, oldPassword, newPassword, ETrue ) );

    logString = "Drive:" + QString::number( drive );
    FM_LOG( logString );
    logString = "Password set error:" + QString::number( err );
    FM_LOG( logString );

    delete newPassword16;
    delete oldPassword16;
    if( err == KErrNone ){
        return FmErrNone;   
    }
    else if( err == KErrNotSupported ){
        return FmErrNotSupported;
    }
    else{
        return FmErrUnKnown;
    }
}

/*!
    Set \a pwd as empty password
*/
void FmUtils::emptyPwd( QString &pwd )
{
    TBuf< FmMaxLengthofDrivePassword > nullPwd;
    nullPwd.FillZ( nullPwd.MaxLength() );
    nullPwd.Zero();
    pwd = XQConversions::s60DescToQString( nullPwd );
}

/*!
    Set drive volume for drive \a driveName
*/
int FmUtils::renameDrive( const QString &driveName, const QString &newVolumeName)
{
    if( driveName.isEmpty() ) {
        return FmErrWrongParam;
    }
    foreach( const QChar &ch, newVolumeName )
    {
        bool a = ch.isSpace();
        bool b = ch.isLetterOrNumber();
        // If not alphadigit or space, return error
        if( !ch.isLetterOrNumber() && !ch.isSpace() )
        {
            return FmErrBadName;
        }   
    }
        
    //  CCoeEnv *env = CCoeEnv::Static();
    //	RFs& fs = env->FsSession();
    // make new fs so that this function can be called in thread
    RFs fs;
    fs.Connect();

    TInt drive = 0;
	drive = driveName[0].toUpper().toAscii() - 'A' + EDriveA;

    TPtr newName ( ( XQConversions::qStringToS60Desc( newVolumeName ) )->Des() );

    int err( fs.SetVolumeLabel( newName, drive ));
    
    QString logString = "Rename error:" + QString::number( err );
    FM_LOG( logString );

    fs.Close();
    if( err == KErrNone ){
        return FmErrNone;   
    }
    else if( err == KErrNotReady ){
        return FmErrNotReady;
    }
    else{
        return FmErrUnKnown;
    }
}

/*!
    Eject drive \a driveName
*/
int FmUtils::ejectDrive( const QString &driveName )
{
    if( driveName.isEmpty() ) {
        return FmErrWrongParam;
    }
    QString logString = "FmUtils::ejectDrive start";
    FM_LOG( logString );

    TInt drive = 0;
	drive = driveName[0].toUpper().toAscii() - 'A' + EDriveA;

    const int KDriveShift = 16;

    // Let SysAp handle eject
    RProperty::Set(
        KPSUidCoreApplicationUIs,
        KCoreAppUIsMmcRemovedWithoutEject,
        ECoreAppUIsEjectCommandUsedToDrive | ( drive << KDriveShift )
        );
    return FmErrNone;
}

/*!
    Check if drive \a driveName is accessable for user
*/
bool FmUtils::checkDriveAccessFilter( const QString &driveName )
{
    if( driveName.isEmpty() ) {
        return false;
    }
    FmDriverInfo driveInfo = queryDriverInfo( driveName );
    if( ( driveInfo.driveState()& FmDriverInfo::EDriveRam ) ||
        ( driveInfo.driveState()& FmDriverInfo::EDriveRom ) ) {
        return false;
    }
    return true;
}

/*!
    This function should be called to adjust path if user goto a drive.
    data folder will be append to C:\ becuase user could only view C:\data instead C:\ 
*/
QString FmUtils::checkDriveToFolderFilter( const QString &path )
{
    QString checkedPath = fillPathWithSplash( path );
    if( checkedPath.compare( Drive_C, Qt::CaseInsensitive ) == 0 ) {
        checkedPath += QString( "data" ) + QDir::separator();
        return checkedPath;
    }
    return path;

}

/*!
    This function should be called to adjust path if user back to up level path.
    If user is at C:\data then path level should be returned as C:\
    Becuase C:\data is root path for C drive 
*/
QString FmUtils::checkFolderToDriveFilter( const QString &path )
{
    QString logString;
    logString = QString( "checkFolderToDriveFilter: " ) + path;
    FM_LOG( logString );
    QString checkedPath = fillPathWithSplash( path );

    logString = QString( "checkFolderToDriveFilter_fillPathWithSplash: " ) + checkedPath;
    FM_LOG( logString );
    
    if( checkedPath.compare( Folder_C_Data, Qt::CaseInsensitive ) == 0 ) {
        FM_LOG( QString( " change from c:/data/ to C:/" ) );
        return Drive_C;
    }
    return path;

}

/*!
    Check if \a path is accessable for user
*/
int FmUtils::isPathAccessabel( const QString &path )
{
    // Used to check if path is accessable, very important feature
    // and will return filemanager error.
    FM_LOG( QString( "isPathAccessabel:" ) + path );
    if( path.isEmpty() ) {
        return FmErrPathNotExist;
    }

    // used to filter locked/ejected/corrupted drive
    // check if drive is available, no matter if it is a drive, a folder, or a file.
    if( !isDriveAvailable( path ) ) {
        FM_LOG( QString( "isPathAccessabel false: path is drive and not available" ) );
        return FmErrDriveNotAvailable;
    }

    QFileInfo fileInfo( path );

    if( fileInfo.absoluteFilePath().contains( Drive_C, Qt::CaseInsensitive ) &&
        !fileInfo.absoluteFilePath().contains( Folder_C_Data, Qt::CaseInsensitive ) ) {
        FM_LOG( QString( "isPathAccessabel false: path contain C and not in data folder" ) );
        return FmErrPathDenied;
    }
    
    if( isSystemFolder( fileInfo.absoluteFilePath() ) ) {
        FM_LOG( QString( "isPathAccessabel false: path is system path that not accessable" ) );
        return FmErrPathDenied;
    }

    if( !checkDriveAccessFilter( FmUtils::getDriveNameFromPath( fileInfo.absoluteFilePath() ) ) ){
        return FmErrDriveDenied;
    }
    if( !fileInfo.exists() ) {
        FM_LOG( QString( "isPathAccessabel false: path not exist" ) );
        return FmErrPathNotExist;
    }
    FM_LOG( QString( "isPathAccessabel FmErrNone" ) );
    return FmErrNone;
}

/*!
    Check if drive related to \a path is available.
    This function should not check if path is available. Only responsible for checking drive  
    When MMC is not inserted, also return false
*/
bool FmUtils::isDriveAvailable( const QString &path )
{
    FM_LOG( QString( "isDriveAvailable:" ) + path );
    if( path.isEmpty() ) {
        return false;
    }
    FmDriverInfo::DriveState driveState = queryDriverInfo( path ).driveState();
    if( ( driveState & FmDriverInfo::EDriveAvailable ) ) {
        FM_LOG( QString( "isDriveAvailable true" ) );
        return true;
    }
    FM_LOG( QString( "isDriveAvailable false" ) );
    return false;
}

/*!
    Check if \a folderPath is default folder for system
*/
bool FmUtils::isDefaultFolder( const QString &folderPath  )
{
    HBufC *path = XQConversions::qStringToS60Desc( folderPath );
    TPtrC desFolderPath( path->Des() );
    
    bool ret( true );
    TInt pathType( PathInfo::PathType( desFolderPath ) );
    switch( pathType ){
       case PathInfo::ENotSystemPath:{
           QString locString( localize( folderPath ) );
            if ( locString.isEmpty() ){
                ret = false;
                break;
            }
            ret = true;
            break;
            }
        case PathInfo::EPhoneMemoryRootPath: // FALL THROUGH
        case PathInfo::EMemoryCardRootPath: // FALL THROUGH
        case PathInfo::ERomRootPath:{
            ret = false;
            break;
        }
        // Accept other folders
        default:{
            ret = true;
            break;
        }
    }
    delete path;
    return ret;
}

/*!
    Create system default folders for drive \a driveName.
    Default folders should be created after format a drive.
*/
void FmUtils::createDefaultFolders( const QString &driveName )
{
    if( driveName.isEmpty() ) {
        return;
    }
    int err;
    
    TInt drive = 0;
    drive = driveName[0].toUpper().toAscii() - 'A' + EDriveA;
    
    RFs fs;
    err = fs.Connect();
    
    if( err != KErrNone ){
        return;
    }
    
    quint32 drvStatus( 0 );
    err = DriveInfo::GetDriveStatus( fs, drive, drvStatus );
    if ( !( drvStatus & DriveInfo::EDriveUserVisible ) ||
        ( drvStatus & ( DriveInfo::EDriveRemote |
                        DriveInfo::EDriveReadOnly |
                        DriveInfo::EDriveUsbMemory ) ) ){
            return; 
        }
    
    TEntry entry;
    CDesCArray* array = PathInfo::GetListOfPathsLC( drive );
    
    TInt count( array->MdcaCount() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        TPtrC fullPath( array->MdcaPoint( i ) );
        TBool allow( ETrue );

        if ( drvStatus & DriveInfo::EDriveRemovable )
            {
            // Filter few folder types from physically removable memory cards
            TInt pathType( PathInfo::PathType( fullPath ) );
            switch( pathType )
                {
                case PathInfo::EGamesPath: // FALL THROUGH
                case PathInfo::EInstallsPath: // FALL THROUGH
                case PathInfo::EGsmPicturesPath: // FALL THROUGH
                case PathInfo::EMmsBackgroundImagesPath: // FALL THROUGH
                case PathInfo::EPresenceLogosPath:
                    {
                    allow = EFalse;
                    }
                default:
                    {
                    break;
                    }
                }
            }

        if ( allow ){
            fs.MkDirAll( fullPath ); // Ignore error

            if ( fs.Entry( fullPath, entry ) == KErrNone ){
                if( entry.IsHidden() ){
                // If setting fails, File Manager can still go on
                    fs.SetEntry(
                         fullPath, entry.iModified, 0, KEntryAttHidden );
                }
            }
        }
    }
    CleanupStack::PopAndDestroy( array );
}

/*!
    return localized file name by \a path, empty string returned for non-localized path.
    In Symbian system, default folders will be localized.
    So localize also can be used to check if a path is a default folder
    \sa isDefaultFolder
*/
QString FmUtils::localize( const QString &path )
{
    // HbDirectoryNameLocalizer can not recognize path with \ in the end
    QString locPath( removePathSplash( path ) );
    
    HbDirectoryNameLocalizer localizer;
    return localizer.translate( locPath );
}

/*!
    set the \a desFile attributes as the same with \a srcFile
*/
int FmUtils::setFileAttributes( const QString &srcFile, const QString &desFile )
{
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() ); 
    CleanupClosePushL( fsSession );
    RFile64 src;
    RFile64 des;
    HBufC *buf1 = XQConversions::qStringToS60Desc( removePathSplash( formatPath( srcFile ) ) );
    HBufC *buf2 = XQConversions::qStringToS60Desc( removePathSplash( formatPath( desFile ) ) );
    User::LeaveIfError( src.Open( fsSession, *buf1, EFileRead | EFileShareReadersOnly ) );
    User::LeaveIfError( des.Open( fsSession, *buf2, EFileWrite | EFileShareExclusive ) );
    TTime mod;
    int err = src.Modified( mod );;
    if ( err == FmErrNone ) {
        err = des.SetModified( mod );    
    }
    TUint att( 0 );
    if ( err == FmErrNone ) {
        err = src.Att( att );        
    }
    if ( err == FmErrNone ) {
        des.SetAtt( att, ( ~att ) & KEntryAttMaskSupported );
    }    
    src.Close();
    des.Close();
    fsSession.Close();
    CleanupStack::PopAndDestroy(); // fsSession
    return err;
}

/*!
    judge whether there is enough space on \a targetDrive for \a size.
    return true if has, false if not.
*/
bool FmUtils::hasEnoughSpace( const QString &targetDrive, qint64 size )
{
    RFs fsSession;
    QT_TRAP_THROWING( fsSession.Connect() ); 
    CleanupClosePushL( fsSession );
    TInt dstDrv(0);
    HBufC* hbuf = XQConversions::qStringToS60Desc( targetDrive );
    QT_TRAP_THROWING( RFs::CharToDrive( hbuf->operator [](0), dstDrv ) );
    bool ret = !SysUtil::DiskSpaceBelowCriticalLevelL( &fsSession,  size , dstDrv );
    CleanupStack::PopAndDestroy(); // fsSession
    return ret;
    
}

/*!
    move one file insice the same drive, from \a source to \a target.
    return KErrNone if successful, otherwise one of the other system-wide error codes.
*/
int FmUtils::moveInsideDrive( const QString &source, const QString &target )
{
    RFs fsSession;
    QT_TRAP_THROWING( fsSession.Connect() ); 
    CleanupClosePushL( fsSession );
    HBufC* oldName = XQConversions::qStringToS60Desc( source );
    HBufC* newName = XQConversions::qStringToS60Desc( target );
    int ret = fsSession.Rename( *oldName, *newName );
    CleanupStack::PopAndDestroy(); // fsSession
    return ret;    
}

/*!
   Launch a file with associated application.
*/
int FmUtils::launchFile( const QString &filePath )

{
    QFile file( filePath );
    if( !file.exists() ) {
        return false;
    }
        
    XQApplicationManager mAiwMgr;
    XQAiwRequest *request = mAiwMgr.create(file);
    if ( request == 0 ) {
        // No handlers for the URI
        return FmErrUnKnown;
    }
    
    // Set function parameters
    QList<QVariant> args;
    args << file.fileName();
    request->setArguments(args);
    
    // Send the request
    bool res = request->send();
    if  (!res) 
    {
       // Request failed. 
      int error = request->lastError();
      
      delete request;
      return FmErrUnKnown;
    }
    
    delete request;
    return FmErrNone;
}

/*!
    return path for backup restore config file.
    Currently \a appPath is not used.
*/
QString FmUtils::getBurConfigPath( QString appPath )
{
    Q_UNUSED( appPath );
    QString path( BURCONFIGFILE );
    return path;
}

/*!
    return MetaData string for \a filePath
*/
QString FmUtils::getFileType( const QString &filePath  )
{
    RApaLsSession apaSession;
    TDataType dataType;
    TUid appUid;
    
    TBuf<128> mimeTypeBuf;
        
    int err = apaSession.Connect();
    
    if ( err == KErrNone ){   
        err = apaSession.AppForDocument( XQConversions::qStringToS60Desc( filePath )->Des(), 
                                         appUid, dataType );
        
        if( err == KErrNone ){
            mimeTypeBuf.Copy(dataType.Des8());
        }  
    }
    
    apaSession.Close();
    return XQConversions::s60DescToQString( mimeTypeBuf );
}

/*!
    Check if drive \a driveName is drive C
*/
bool FmUtils::isDriveC( const QString &driveName )
{
    if( driveName.isEmpty() ) {
        return false;
    }
    TInt drive = 0;
    drive = driveName[0].toUpper().toAscii() - 'A' + EDriveA;
    if( drive == EDriveC ){
        return true;
    }
    else{
        return false;
    }
   
}

/*!
    return max file name length
*/
int FmUtils::getMaxFileNameLength()
{
    return KMaxFileName;
}

/*!
    Check if length of \a path is exceed max path length. 
*/
bool FmUtils::checkMaxPathLength( const QString& path )
{
    if( path.length() > KMaxPath ) {
        return false;
    }
    return true;
}

/*!
    Set appliation as system mode if \a isSystem is true.
    Otherwise set application as non-system mode
*/
void FmUtils::setSystem( bool isSystem )
{
    CCoeEnv* coeEnv = CCoeEnv::Static();
    CEikonEnv* eikonEnv = (STATIC_CAST(CEikonEnv*,coeEnv));
    if( isSystem ) {
        eikonEnv->SetSystem(ETrue);
    } else {
        eikonEnv->SetSystem(EFalse);
    }
}

/*!
    return drive name by \a drive
*/
QString FmUtils::numberToDriveName( int drive )
{
    QChar driveChar( drive - EDriveA + 'A' );
    QString driveName = QString( driveChar ) + ':';
    return driveName;
}
