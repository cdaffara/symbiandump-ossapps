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

#ifndef FMDRIVEMODEL_H
#define FMDRIVEMODEL_H

#include <QAbstractItemModel>
#include <QStringList>
#include <QModelIndex>

class FmFileIconProvider;

/*!
    \class FmDriveListProvider
    \brief The class FmDriveListProvider provide drive list which is used in FmDriveModel
 */
class FmDriveListProvider
{
public:
    FmDriveListProvider()
    {
    }
    
    virtual ~FmDriveListProvider()
    {
    }
    
    /*!
     implement this function to provide drive list.
     */
    virtual void getDriveList( QStringList &driveList ) = 0;
};

class FmDriveModel : public QAbstractListModel
{
Q_OBJECT
public:
    enum Option
    {
        FillWithVolume        = 0x00000001,  // add volume behind disname
        FillWithDefaultVolume = 0x00000002,  // add default diskname while enable FillWithVolume and volume is empty
        HideUnAvailableDrive  = 0x00000004,  // for example: drive F when no MMC card inserted.
        FillWithTotalSize     = 0x00000008,  // show the drive's total size.
        FillWithFreeSize      = 0x00000010   //show the drive's free size.
    };
    Q_DECLARE_FLAGS(Options, Option)

    explicit FmDriveModel( QObject *parent = 0, Options options = 0,
            FmDriveListProvider *driveListProvider = 0 );
    virtual ~FmDriveModel();

    void refresh();
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    bool indexValid( const QModelIndex &index ) const;
    QString driveName( const QModelIndex &index ) const;
    QVariant displayString( const QModelIndex &index ) const;
    
private:
    QStringList         mFindResult;
    FmFileIconProvider  *mIconProvider;
    QStringList         mDriveList;
    Options             mOptions;
    
    // DriveListProvider will ignore HideUnAvailableDrive option.
    // DriveListProvide can be set by others to provide special drive list
    FmDriveListProvider *mDriveListProvider;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(FmDriveModel::Options)

#endif
