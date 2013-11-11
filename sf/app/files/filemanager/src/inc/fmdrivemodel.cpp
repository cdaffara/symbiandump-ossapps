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

#include "fmdrivemodel.h"
#include "fmfileiconprovider.h"
#include "fmutils.h"

#include <QDir>
#include <QFileInfo>

#include <hbglobal.h>

FmDriveModel::FmDriveModel( QObject *parent, Options options,
        FmDriveListProvider *driveListProvider ) :
    QAbstractListModel( parent ), mOptions( options ), mDriveListProvider( driveListProvider )
{
    mIconProvider = new FmFileIconProvider();
    refresh();
}

FmDriveModel::~FmDriveModel()
{
    delete mIconProvider;
}

void FmDriveModel::refresh()
{
    emit layoutAboutToBeChanged();
    mDriveList.clear();
    
    // if mDriveListProvider existed, use it to fetch drive list
    // otherwise use FmUtils::getDriveList to fetch drive list.
    if( mDriveListProvider ) {
        mDriveListProvider->getDriveList( mDriveList );
    } else {
        if( mOptions & HideUnAvailableDrive ) {
            FM_LOG( QString( "FmDriveModel::refresh HideUnAvailableDrive_true" ) );
            FmUtils::getDriveList( mDriveList, true );
        } else {
            FM_LOG( QString( "FmDriveModel::refresh HideUnAvailableDrive_false" ) );
            FmUtils::getDriveList( mDriveList, false );
        }
    }

	emit layoutChanged();
	for( int i=0; i<mDriveList.count(); i++ ) {
        emit dataChanged(index( i, 0 ), index( i, 0 ));
	}
}


int FmDriveModel::rowCount( const QModelIndex &parent ) const
{
    if (!parent.isValid())
        return mDriveList.size();

    return 0;
}

int FmDriveModel::columnCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );
    return 1;
}

QVariant FmDriveModel::data( const QModelIndex &index, int role ) const
{
    if (!indexValid( index ))
        return QVariant();

    if (role == Qt::DisplayRole ) {
       return displayString( index );
    } 
    if (index.column() == 0) {
        if (role == Qt::DecorationRole ) {
            QString path = driveName( index );
            return mIconProvider->icon( QFileInfo( path ) );
        }
    }

    if (index.column() == 1 && role == Qt::TextAlignmentRole) {
        return Qt::AlignRight;
    }

    return QVariant();
}


QVariant FmDriveModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if (orientation == Qt::Horizontal) {
        if (role != Qt::DisplayRole)
            return QVariant();

        switch (section) {
            case 0: return hbTrId("Name");
            case 1: return hbTrId("Size");
            case 2: return hbTrId("Type");
            case 3: return hbTrId("Date Modified");
            default: return QVariant();
        }
    }

    return QAbstractItemModel::headerData( section, orientation, role );
}

bool FmDriveModel::indexValid( const QModelIndex &index ) const
{
    if( !(&index) )
        return false;
    return true;
}

QString FmDriveModel::driveName( const QModelIndex &index ) const
{
    QString data;
    if (index.row() >= 0 && index.row() < mDriveList.size()) {
        int row = index.row();
        QString diskName = mDriveList[ row ];
        data = diskName;
    }
    return data;
}

QVariant FmDriveModel::displayString( const QModelIndex &index ) const
{
    QStringList data;
    if (index.row() >= 0 && index.row() < mDriveList.size()) {
        int row = index.row();
        QString diskName = mDriveList[ row ];

        if( mOptions & FillWithVolume ) {
            data << FmUtils::fillDriveVolume( diskName, mOptions & FillWithDefaultVolume );
        } else {
            data << FmUtils::removePathSplash( diskName );
        }
        diskName = FmUtils::fillPathWithSplash( diskName );
        FmDriverInfo driverInfo = FmUtils::queryDriverInfo( diskName );
        if ( mOptions & FillWithTotalSize ) {                    
            if( driverInfo.driveState() & FmDriverInfo::EDriveAvailable ) {
                data << QString( hbTrId ( "Size: " ) + FmUtils::formatStorageSize( driverInfo.size() ) );                
            } else if( driverInfo.driveState() & FmDriverInfo::EDriveLocked ) {
                data << QString( hbTrId ( "Locked" ) );                
            } else if( driverInfo.driveState() & FmDriverInfo::EDriveCorrupted ) {
                data << QString( hbTrId ( "Corrupted" ) );                
            } else if( driverInfo.driveState() & FmDriverInfo::EDriveNotPresent ) {
                data << QString( hbTrId ( "Not Ready" ) );                
            }            
        }
        if ( mOptions & FillWithFreeSize ) {        
            if( driverInfo.driveState() & FmDriverInfo::EDriveAvailable ) {                
                data << QString( hbTrId ( "Free: " ) + FmUtils::formatStorageSize( driverInfo.freeSize() ) );
            } else if( ( driverInfo.driveState() & FmDriverInfo::EDriveLocked ) ||
                    ( driverInfo.driveState() & FmDriverInfo::EDriveCorrupted ) ||
                    ( driverInfo.driveState() & FmDriverInfo::EDriveNotPresent )) {                
                data << QString( QString(" ") );
            }            
        }
    }
    return data;
}
