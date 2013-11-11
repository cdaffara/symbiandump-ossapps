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
 *      The source file of the backup engine from backup engine wrapper
 */

#include "fmbkupengine.h"
#include "fmbkupengine_p.h"

#include "fmbackupsettings.h"
#include "fmrestoresettings.h"

#include <QObject>
FmBkupEngine::FmBkupEngine(QObject* parent):
    QObject(parent), d(new FmBkupEnginePrivate(this)), iBackupSettings( 0 ), iRestoreSettings( 0 )
{
    connect( d, SIGNAL( notifyPreparing( bool ) ), this, SIGNAL( notifyPreparing( bool ) ) );
	connect( d, SIGNAL( notifyStart( bool, int ) ), this, SIGNAL( notifyStart( bool, int ) ) );
	connect( d, SIGNAL( notifyUpdate(int) ), this, SIGNAL( notifyUpdate(int) ) );
	connect( d, SIGNAL( notifyFinish(int) ), 
			this, SIGNAL( notifyFinish(int) ), Qt::DirectConnection );
	connect( d, SIGNAL( notifyMemoryLow(int, int& ) ), this, SIGNAL( notifyMemoryLow(int, int&) ) );
	connect( d, SIGNAL( notifyBackupFilesExist( bool& )), this, SIGNAL( notifyBackupFilesExist( bool&) ) );
}


FmBkupEngine::~FmBkupEngine()
{
    delete d;
    if( iBackupSettings ) {
        delete iBackupSettings;
    }
    if( iRestoreSettings ) {
        delete iRestoreSettings;
    }
}

bool FmBkupEngine::startBackup( QList<FmBkupDrivesAndOperation* > drivesAndOperationList,
    QList<FmBkupBackupCategory*> backupCategoryList,
    QString drive, quint32 content)
{
    return d->startBackup( drivesAndOperationList, backupCategoryList, drive, content );
}
void FmBkupEngine::cancelBackup()
{
	d->cancelBackup();
}
bool FmBkupEngine::startRestore( QList<FmBkupDrivesAndOperation* > drivesAndOperationList )
{
   return d->StartRestoreL( drivesAndOperationList );
}


int FmBkupEngine::error() const
{
    return d->error();
}

FmBackupSettings *FmBkupEngine::BackupSettingsL()
{
    if ( !iBackupSettings )
        {
        iBackupSettings = new FmBackupSettings( this );
        }
    return iBackupSettings;
}

FmRestoreSettings *FmBkupEngine::RestoreSettingsL()
{
    if ( !iRestoreSettings )
        {
        iRestoreSettings = new FmRestoreSettings( *this );
        }
    return iRestoreSettings;
}
    
void FmBkupEngine::GetRestoreInfoArray( QList<FmBkupDrivesAndOperation* > &drivesAndOperationList,
        QList< FmRestoreInfo > &restoreInfoList,
        const QString& aDrive )
{
    d->GetRestoreInfoArray( drivesAndOperationList, restoreInfoList, aDrive );
}

int FmBkupEngine::deleteBackup( QList<FmBkupDrivesAndOperation* > drivesAndOperationList )
{
    return d->deleteBackup( drivesAndOperationList );
}


void FmBkupEngine::getBackupDriveList( QStringList &driveList )
{
    d->getBackupDriveList( driveList );
}

