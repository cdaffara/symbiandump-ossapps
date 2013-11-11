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
* Description: 
*
*/

#include "fmoperationformat.h"
#include "fmcommon.h"
#include "fmoperationbase.h"
#include "fmutils.h"
#include "fmviewmanager.h"
#include "fmserviceutils.h"

#include <QString>
#include <QSettings>

#include <f32file.h>
#include <e32property.h>
#include <coreapplicationuisdomainpskeys.h>

/* \fn  void driveSpaceChanged()
 * This signal is emitted when copy or move is completed, and used to update the drive size.
 */

/*
 * Constructs a format operation with \a parent
 * \a mDriverName the drive to be formatted.
 */
FmOperationFormat::FmOperationFormat( QObject *parent, const QString &mDriverName ) : FmOperationBase( parent, FmOperationService::EOperationTypeFormat ),
    mDriverName( mDriverName )
{
    connect( this, SIGNAL( driveSpaceChanged() ),
            parent, SLOT( on_operation_driveSpaceChanged() ) );
}

/*
 * Destructs the operation.
 */
FmOperationFormat::~FmOperationFormat()
{
}

/*
 * Returns the to be formatted drive name
 */
QString FmOperationFormat::driverName()
{
    return mDriverName;
}

/*
 * Starts to format.
 * \a isStopped not used
 */
void FmOperationFormat::start( volatile bool */*isStopped*/ )
{ 
    QString logString = "FmOperationFormat::start";
    FM_LOG( logString );
    
    emit notifyPreparing( false );
    
    if( mDriverName.isEmpty() ) {
        emit notifyError( FmErrWrongParam, QString() );
        return;
    }
    FmViewManager::viewManager()->serviceUtils()->closeApps();
    
    
    RFormat format;
    
    RFs fs;
    int err = fs.Connect();
    
    if( err != KErrNone ){
        emit notifyError( FmErrTypeFormatFailed, QString() );
        return;
    }

    TInt drive = 0;
    drive = mDriverName[0].toUpper().toAscii() - 'A' + EDriveA;
    // save volume name for MassStorage
    storeVolumeName( drive );
    
    TDriveName formatDriveName( TDriveUnit( drive ).Name() );

    int finalValue = 0;
    
    format.Close();

    err = format.Open(
        fs, formatDriveName, EFullFormat | ESpecialFormat, finalValue );
    
    if( err == KErrNone ){
        logString = "emit notifyStart";
        FM_LOG( logString );
        mTotalSteps = finalValue;
        emit notifyStart( false, finalValue );
    }
    
    logString = "Format open error:" + QString::number( err );
    FM_LOG( logString );
    
    if( err == KErrLocked ){
        err = fs.ErasePassword( drive );
        
        logString = "ErasePassword error:" + QString::number( err );
        FM_LOG( logString );
        
        if( err == KErrNone ){
            err = format.Open(
               fs, formatDriveName, EFullFormat, finalValue ); 
            
            if( err == KErrNone ){
                logString = "emit notifyStart";
                FM_LOG( logString );
                mTotalSteps = finalValue;
                emit notifyStart( false, finalValue );
            }           
            logString = "Second format open error:" + QString::number( err );
            FM_LOG( logString );
        }
    }
    
    if (err == KErrInUse){
        TBool reallyFormat = ETrue;
        if (reallyFormat){
            err = format.Open(
                    fs, formatDriveName, EFullFormat | EForceFormat, finalValue );
            
            if( err == KErrNone ){
                logString = "emit notifyStart";
                FM_LOG( logString );
                mTotalSteps = finalValue;
                emit notifyStart( false, finalValue );
            } 
            
            logString = "Reallyformat open error:" + QString::number( err );
            FM_LOG( logString );
        }
    }
    
    TFullName fsName;
    if ( err == KErrNone )
        {
        err = fs.FileSystemName( fsName, drive  );
        
        logString = "FileSystemName error:" + QString::number( err );
        FM_LOG( logString );

        if ( err == KErrNone && fsName.Length() > 0 )
            {
            // Prevent SysAp shutting down applications
            RProperty::Set(
                    KPSUidCoreApplicationUIs,
                    KCoreAppUIsMmcRemovedWithoutEject,
                    ECoreAppUIsEjectCommandUsed );
            
            logString = "Prevent SysAp shutting down applications" ;
            FM_LOG( logString );
            }
        else
            {
            // Don't continue with format if there is no file system name
            // or file system name could not be obtained.
            err = KErrCancel;
            
            logString = QString( "Format cancel" );
            FM_LOG( logString );
            }
        }
    
    if( err == KErrNone &&  finalValue ){
    
        while ( finalValue ){        
            logString = "Format tracks:" + QString::number( finalValue );
            FM_LOG( logString );
            err = format.Next( finalValue );
            
            if( err != KErrNone ){ 
                logString = "Format error:" + QString::number( err );
                FM_LOG( logString );
                break; 
            }
            
            logString = "emit notifyProgress";
            FM_LOG( logString );
            emit notifyProgress( mTotalSteps - finalValue );
        }
    }
    
    if( !finalValue || err != KErrNone ){
        format.Close();        
        fs.Close();
        
        // restore volume name for MassStorage
        restoreVolumeName( drive );
        FmUtils::createDefaultFolders( mDriverName );
    }

    FmViewManager::viewManager()->serviceUtils()->restartApps();
    
    // refresh drive space no care if cancel, error or finished.
    // as filemanger cannot notify drive space changed
    // do not refresh path as QFileSystemModel will do auto-refresh
    emit driveSpaceChanged();   
    if( err == KErrNone ){
        emit notifyFinish();        
    }
    else{
        emit notifyError( FmErrTypeFormatFailed, QString() );
    }
}

void FmOperationFormat::storeVolumeName( int drive )
{
    FmDriverInfo driverInfo = FmUtils::queryDriverInfo( FmUtils::numberToDriveName( drive ) );
    if ( driverInfo.driveType() == FmDriverInfo::EDriveTypeMassStorage )
    {
        QString volumeName( driverInfo.volumeName() );
        QSettings settings( "Nokia", "FileManager" );
        settings.beginGroup( "Settings" );
        settings.setValue( "MassStorageDriveNumber", drive );
        settings.setValue( "MassStorageVolumeName", volumeName );
        settings.endGroup();
    }
}

// -----------------------------------------------------------------------------
// CFileManagerEngine::RestoreVolumeNameL()
//
// -----------------------------------------------------------------------------
//  
void FmOperationFormat::restoreVolumeName( int drive )
{

    FmDriverInfo driveInfo = FmUtils::queryDriverInfo( FmUtils::numberToDriveName( drive ));
    if ( driveInfo.driveType() == FmDriverInfo::EDriveTypeMassStorage )
    {
        QSettings settings("Nokia", "FileManager");
        settings.beginGroup("Settings");
        int massStorageDriveNumber    = settings.value( "MassStorageDriveNumber", -1 ).toInt();
        QString massStorageVolumeName = settings.value( "MassStorageVolumeName", QString() ).toString();
        settings.endGroup();
    
        if ( ( !massStorageVolumeName.isEmpty() ) &&
                ( massStorageDriveNumber == drive ) )
        {
            FmUtils::renameDrive( FmUtils::numberToDriveName( drive ), massStorageVolumeName );
        }
    }
}
