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
 * 
 * Description:
 *      The source file of the backup engine implement for win32
 */

#include "FmBkupEngine_p.h"
#include <QThread>

#include "fmbkupcommon.h"
#include "fmbackupsettings.h"
#include "fmrestoresettings.h"
#include "fmdefine.h"

#include <QSettings>

FmBkupEnginePrivate::FmBkupEnginePrivate(FmBkupEngine* bkupEngine): q(bkupEngine), mError( FmErrNone )
{
   mBkupThread = new FmBkupThread( this );

    connect( mBkupThread, SIGNAL( notifyPreparing( bool ) ), this, SLOT( onNotifyPreparing( bool ) ) );
	connect( mBkupThread, SIGNAL( notifyStart( bool, int ) ), this, SLOT( onNotifyStart( bool, int ) ) );
	connect( mBkupThread, SIGNAL( notifyUpdate(int) ), this, SLOT( onNotifyUpdate(int) ) );
	connect( mBkupThread, SIGNAL( notifyFinish(int) ), 
			this, SLOT( onNotifyFinish(int) ) );
	connect( mBkupThread, SIGNAL( notifyMemoryLow(int, int& ) ), this, SLOT( onNotifyMemoryLow(int, int&) ) );
}

FmBkupEnginePrivate::~FmBkupEnginePrivate()
{

}

void FmBkupEnginePrivate::onNotifyPreparing( bool cancelable )
{
    emit notifyPreparing( cancelable );
}
void FmBkupEnginePrivate::onNotifyStart( bool cancelable, int totalCount )
{
    emit notifyStart( cancelable, totalCount );
}
void FmBkupEnginePrivate::onNotifyUpdate( int count )
{
    emit notifyUpdate( count );
}
void FmBkupEnginePrivate::onNotifyFinish( int err )
{
    emit notifyFinish( err );
}
void FmBkupEnginePrivate::onNotifyMemoryLow( int memoryValue, int &userError )
{
    emit notifyMemoryLow( memoryValue, userError );
}

bool FmBkupEnginePrivate::startBackup( QList<FmBkupDrivesAndOperation* > drivesAndOperationList,
QList<FmBkupBackupCategory*> backupCategoryList,QString drive, quint32 content )
{
    mError = mBkupThread->asyncBackup( drivesAndOperationList, backupCategoryList, drive, content );
    return ( mError == FmErrNone );
}

void FmBkupEnginePrivate::cancelBackup()
{
    mBkupThread->stop();
}

int FmBkupEnginePrivate::deleteBackup( QList<FmBkupDrivesAndOperation* > drivesAndOperationList )
{
    mError = FmErrNone;

    selection.clear();
    FmRestoreSettings& rstSettings( *( q->RestoreSettingsL() ) );
    rstSettings.GetSelectionL( selection );
    QSettings settings("Nokia", "FileManager");
    foreach( FmRestoreInfo restoreInfo, selection ) {
        switch( restoreInfo.content() )
        {
        case FmBkupEngineCommon::EFileManagerBackupContentSettings:
            settings.beginGroup("BackupSettings");
            settings.remove( QString( "" ) );
            settings.endGroup();
            break;
        case FmBkupEngineCommon::EFileManagerBackupContentMessages:
            settings.beginGroup("BackupMessages");
            settings.remove( QString( "" ) );
            settings.endGroup();
            break;
        case FmBkupEngineCommon::EFileManagerBackupContentContacts:
            settings.beginGroup("BackupContacts");
            settings.remove( QString( "" ) );
            settings.endGroup();
            break;
        case FmBkupEngineCommon::EFileManagerBackupContentCalendar:
            settings.beginGroup("BackupCalendar");
            settings.remove( QString( "" ) );
            settings.endGroup();
            break;
        case FmBkupEngineCommon::EFileManagerBackupContentBookmarks:
            settings.beginGroup("BackupBookmarks");
            settings.remove( QString( "" ) );
            settings.endGroup();
            break;
        case FmBkupEngineCommon::EFileManagerBackupContentUserFiles:
            settings.beginGroup("BackupUserFiles");
            settings.remove( QString( "" ) );
            settings.endGroup();
            break;
        default:
            break;
        }
    }

    

    return mError;
}

//    TUint32 FmgrToBkupMask( const TUint32 aFmrgMask );
int FmBkupEnginePrivate::error()
{
    return mError;

}

//
bool FmBkupEnginePrivate::StartRestoreL( QList<FmBkupDrivesAndOperation* > drivesAndOperationList )
{
    selection.clear();
    FmRestoreSettings& rstSettings( *( q->RestoreSettingsL() ) );
    rstSettings.GetSelectionL( selection );

    mError = mBkupThread->asyncRestore( drivesAndOperationList,selection );
    return ( mError == FmErrNone );

}
    

void FmBkupEnginePrivate::GetRestoreInfoArray( QList<FmBkupDrivesAndOperation* > drivesAndOperationList,
        QList< FmRestoreInfo > &restoreInfoList,
        const QString& aDrive )
{
    restoreInfoList.clear();
    QString debugString;
    QSettings settings("Nokia", "FileManager");
    QDateTime dateTime;

    settings.beginGroup("BackupSettings");
    quint32 content  = (settings.value("content", 0).toUInt());
    debugString = settings.value( "dateTime").toString();
    dateTime.setTime_t( settings.value( "dateTime" ).toUInt() );
    QString drive = settings.value( "drive" ).toString();
    settings.endGroup();
	if( content != 0 && ( drive.contains( aDrive, Qt::CaseInsensitive ) || aDrive.isEmpty() ) )
    {
        FmRestoreInfo restoreInfo( content, dateTime, drive );
        restoreInfoList.push_back( restoreInfo );
    }

    settings.beginGroup("BackupMessages");
    content  = (settings.value("content", 0).toUInt());
    dateTime.setTime_t( settings.value( "dateTime" ).toUInt() );
    drive = settings.value( "drive" ).toString();
    settings.endGroup();
   if( content != 0 && ( drive.contains( aDrive, Qt::CaseInsensitive ) || aDrive.isEmpty() ) )
    {
        FmRestoreInfo restoreInfo( content, dateTime, drive );
        restoreInfoList.push_back( restoreInfo );
    }

    settings.beginGroup("BackupContacts");
    content  = (settings.value("content", 0).toUInt());
    dateTime.setTime_t( settings.value( "dateTime" ).toUInt() );
    drive = settings.value( "drive" ).toString();
    settings.endGroup();
    if( content != 0 && ( drive.contains( aDrive, Qt::CaseInsensitive ) || aDrive.isEmpty() ) )
    {
        FmRestoreInfo restoreInfo( content, dateTime, drive );
        restoreInfoList.push_back( restoreInfo );
    }

    settings.beginGroup("BackupCalendar");
    content  = (settings.value("content", 0).toUInt());
    dateTime.setTime_t( settings.value( "dateTime" ).toUInt() );
    drive = settings.value( "drive" ).toString();
    settings.endGroup();
    if( content != 0 && ( drive.contains( aDrive, Qt::CaseInsensitive ) || aDrive.isEmpty() ) )
    {
        FmRestoreInfo restoreInfo( content, dateTime, drive );
        restoreInfoList.push_back( restoreInfo );
    }

    settings.beginGroup("BackupBookmarks");
    content  = (settings.value("content", 0).toUInt());
    dateTime.setTime_t( settings.value( "dateTime" ).toUInt() );
    drive = settings.value( "drive" ).toString();
    settings.endGroup();
    if( content != 0 && ( drive.contains( aDrive, Qt::CaseInsensitive ) || aDrive.isEmpty() ) )
    {
        FmRestoreInfo restoreInfo( content, dateTime, drive );
        restoreInfoList.push_back( restoreInfo );
    }

    settings.beginGroup("BackupUserFiles");
    content  = (settings.value("content", 0).toUInt());
    dateTime.setTime_t( settings.value( "dateTime" ).toUInt() );
    drive = settings.value( "drive" ).toString();
    settings.endGroup();
    if( content != 0 && ( drive.contains( aDrive, Qt::CaseInsensitive ) || aDrive.isEmpty() ) )
    {
        FmRestoreInfo restoreInfo( content, dateTime, drive );
        restoreInfoList.push_back( restoreInfo );
    }

}




FmBkupThread::FmBkupThread( QObject *parent ) : QThread( parent ), mRunType( RunTypeNone ), mStop( false )
{
}

FmBkupThread::~FmBkupThread()
{

}

int FmBkupThread::asyncBackup( QList<FmBkupDrivesAndOperation* > drivesAndOperationList,
QList<FmBkupBackupCategory*> backupCategoryList,QString drive, quint32 content )
{
    if( this->isRunning() ){
        return FmErrAlreadyStarted;
    }

    if( drivesAndOperationList.count() == 0 || backupCategoryList.count() == 0 ){
        return FmErrWrongParam;
    }

    mRunType = RunTypeBackup;
    mDrivesAndOperationList.clear();
    mBackupCategoryList.clear();

    foreach( FmBkupDrivesAndOperation* driversAndOperation , drivesAndOperationList ) {
        mDrivesAndOperationList.push_back( driversAndOperation );
    }
    foreach( FmBkupBackupCategory* backupCategory , backupCategoryList ) {
        mBackupCategoryList.push_back( backupCategory );
    }

    mDrive = drive;
    mContent = content;
    start();
    return FmErrNone;
}
int FmBkupThread::asyncRestore( QList<FmBkupDrivesAndOperation* > drivesAndOperationList, QList< FmRestoreInfo > selection )
{
    if( this->isRunning() ) {
        return FmErrAlreadyStarted;
    }

    if( drivesAndOperationList.count() == 0 ){
        return FmErrWrongParam;
    }

    mRunType = RunTypeRestore;
    mDrivesAndOperationList.clear();
    foreach( FmBkupDrivesAndOperation* driversAndOperation , drivesAndOperationList ) {
        mDrivesAndOperationList.push_back( driversAndOperation );
    }
    
    foreach( FmRestoreInfo info , selection ) {
        mSelection.push_back( info );
    }
    start();
    return FmErrNone;
}



void FmBkupThread::stop()
{
    mStop = true;
}




void FmBkupThread::run()
{
    mStop = false;

 
    if( mRunType == RunTypeBackup )
    {
        emit notifyPreparing( true );
        this->sleep( 1 );
        emit notifyStart( true, 6 );

        QSettings settings("Nokia", "FileManager");

        if( mContent && FmBkupEngineCommon::EFileManagerBackupContentSettings ) {
            settings.beginGroup("BackupSettings");
            settings.setValue( "content", FmBkupEngineCommon::EFileManagerBackupContentSettings );
            settings.setValue( "dateTime", QDateTime::currentDateTime().toTime_t() );
            settings.setValue( "drive", mDrive );
            settings.endGroup();
            this->msleep( 500 );
            emit notifyUpdate( 1 );
            if( mStop ) {
                emit notifyFinish( FmErrCancel );
                return;
            }
        } 
        if( mContent && FmBkupEngineCommon::EFileManagerBackupContentMessages ) {
            settings.beginGroup("BackupMessages");
            settings.setValue( "content", FmBkupEngineCommon::EFileManagerBackupContentMessages );
            settings.setValue( "dateTime", QDateTime::currentDateTime().toTime_t() );
            settings.setValue( "drive", mDrive );
            settings.endGroup();
            this->msleep( 500 );
            emit notifyUpdate( 2 );
            if( mStop ) {
                emit notifyFinish( FmErrCancel );
                return;
            }
        }
        if ( mContent && FmBkupEngineCommon::EFileManagerBackupContentContacts ) {
            settings.beginGroup("BackupContacts");
            settings.setValue( "content", FmBkupEngineCommon::EFileManagerBackupContentContacts );
            settings.setValue( "dateTime", QDateTime::currentDateTime().toTime_t() );
            settings.setValue( "drive", mDrive );
            settings.endGroup();
            this->msleep( 500 );
            emit notifyUpdate( 3 );
            if( mStop ) {
                emit notifyFinish( FmErrCancel );
                return;
            }
        }
        if ( mContent && FmBkupEngineCommon::EFileManagerBackupContentCalendar ) {
            settings.beginGroup("BackupCalendar");
            settings.setValue( "content", FmBkupEngineCommon::EFileManagerBackupContentCalendar );
            settings.setValue( "dateTime", QDateTime::currentDateTime().toTime_t() );
            settings.setValue( "drive", mDrive );
            settings.endGroup();
            this->msleep( 500 );
            emit notifyUpdate( 4 );
            if( mStop ) {
                emit notifyFinish( FmErrCancel );
                return;
            }
        }
        if ( mContent && FmBkupEngineCommon::EFileManagerBackupContentBookmarks ) {
            settings.beginGroup("BackupBookmarks");
            settings.setValue( "content", FmBkupEngineCommon::EFileManagerBackupContentBookmarks );
            settings.setValue( "dateTime", QDateTime::currentDateTime().toTime_t() );
            settings.setValue( "drive", mDrive );
            settings.endGroup();
            this->msleep( 500 );
            emit notifyUpdate( 5 );
            if( mStop ) {
                emit notifyFinish( FmErrCancel );
                return;
            }
        }
        if ( mContent && FmBkupEngineCommon::EFileManagerBackupContentUserFiles ) {
            settings.beginGroup("BackupUserFiles");
            settings.setValue( "content", FmBkupEngineCommon::EFileManagerBackupContentUserFiles );
            settings.setValue( "dateTime", QDateTime::currentDateTime().toTime_t() );
            settings.setValue( "drive", mDrive );
            settings.endGroup();
            emit notifyUpdate( 6 );
            if( mStop ) {
                emit notifyFinish( FmErrCancel );
                return;
            }
        }
    } else if( mRunType == RunTypeRestore )
    {

        emit notifyPreparing( false );
        this->sleep( 1 );
        emit notifyStart( false, 6 );
        this->msleep( 500 );
        emit notifyUpdate( 1 );
        this->msleep( 500 );
        emit notifyUpdate( 2 );
        this->msleep( 500 );
        emit notifyUpdate( 3 );
        this->msleep( 500 );
        emit notifyUpdate( 4 );
        this->msleep( 500 );
        emit notifyUpdate( 5 );
        this->msleep( 500 );
        emit notifyUpdate( 6 );
        this->msleep( 500 );
    }

    emit notifyFinish( FmErrNone );
}


int FmBkupEnginePrivate::DriverNameToNumber( QString driverName )
{
    int drive = 0;
    drive = driverName[0].toUpper().toAscii() - 'A' + 0/*EDriveA*/;
    return drive;
}
QString FmBkupEnginePrivate::NumberToDriverName( int driver )
{
    QChar driverChar( driver - 0/*EDriveA*/ + 'A' );
    QString driverName = QString( driverChar ) + ':';
    return driverName;
}

void FmBkupEnginePrivate::getBackupDriveList( QStringList &driveList )
    {
    driveList.append("E:/");
    driveList.append("F:/");
    }

