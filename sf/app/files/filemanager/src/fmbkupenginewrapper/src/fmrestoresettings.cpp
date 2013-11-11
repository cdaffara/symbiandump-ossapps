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
 *     The source file of the restore settings of file manager
 */

#include "fmrestoresettings.h"
#include "fmbackupsettings.h"
#include "fmbkupengine.h"
#include "fmbkupcommon.h"

#include <QString>
#include <QDateTime>
#include <QSettings>

FmRestoreSettings::FmRestoreSettings( FmBkupEngine& engine ) : mEngine( engine )
{

}

FmRestoreSettings::~FmRestoreSettings()
{
    resetAndDestoryRestoreEntry();
}

void FmRestoreSettings::load( QList<FmBkupDrivesAndOperation* > drivesAndOperationList )
{
    refreshList( drivesAndOperationList );
}

QList< FmRestoreEntry* > FmRestoreSettings::restoreEntryList()
{
    return mRestoreEntryList;
}

void FmRestoreSettings::resetAndDestoryRestoreEntry()
{
    for( QList< FmRestoreEntry* >::iterator it = mRestoreEntryList.begin(); 
        it!= mRestoreEntryList.end(); ++it ){
        delete *it;
    }
    mRestoreEntryList.clear();
}

void FmRestoreSettings::refreshList( QList<FmBkupDrivesAndOperation* > &drivesAndOperationList )
{
    resetAndDestoryRestoreEntry();

    QList< FmRestoreInfo > restoreInfoList;
    
    QString targetDrive = mEngine.BackupSettingsL()->availableTargetDrive();
    mEngine.GetRestoreInfoArray( drivesAndOperationList, restoreInfoList, targetDrive );
    

    // fetch info list from engine, and then init the entry list for display

    int count( restoreInfoList.count() );
    quint32 mask( FmBkupEngineCommon::EFileManagerBackupContentFirst);
    while ( mask <= FmBkupEngineCommon::EFileManagerBackupContentLast )
        {
        for ( int i( 0 ); i < count; ++i )
            {
            FmRestoreInfo& info( restoreInfoList[ i ] );
            if ( info.content() & mask )
                {
                FmRestoreEntry* entry = CreateEntry( info );
                mRestoreEntryList.push_back( entry );
                }
            }
        mask <<= 1;
        }
}


FmRestoreEntry *FmRestoreSettings::CreateEntry( const FmRestoreInfo &info )
{
    QString text = FmBackupSettings::contentToString( info.content() );

    FmRestoreEntry *entry = new FmRestoreEntry( text, info );
    return entry;
}

void FmRestoreSettings::SetSelection(
        const quint64& aSelection )
    {
    iSelection = aSelection;
    }

void FmRestoreSettings::GetSelectionL(
        QList< FmRestoreInfo >& infoArray ) const
{
    infoArray.clear();

    int count( mRestoreEntryList.count() );

    for ( int i( 0 ); i < count; ++i )
    {
        if ( ( ( ( quint64 ) 1 ) << i ) & iSelection )
        {
            infoArray.append( mRestoreEntryList[ i ]->restoreInfo() );
        }
    }
}
