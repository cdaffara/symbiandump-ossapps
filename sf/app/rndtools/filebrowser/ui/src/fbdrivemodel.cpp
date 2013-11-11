/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "fbdrivemodel.h"
#include "enginewrapper.h"
#include "fbdriveentry.h"
#include "filebrowsersettings.h"
#include "FB.hrh"

#include <QModelIndex>
#include <QFileIconProvider>

/**
  Constructs a file browser custom system model with the given \a engineWrapper and \a parent.
  */
FbDriveModel::FbDriveModel(EngineWrapper *engineWrapper, QObject *parent) :
    QAbstractListModel(parent),
    mEngineWrapper(engineWrapper),
    mFileIconProvider(0)
{
    mFileIconProvider = new QFileIconProvider();
}

/**
  Destroys this file browser custom system model.
  */
FbDriveModel::~FbDriveModel()
{  
    if (mFileIconProvider) {
        delete mFileIconProvider;
    }
}

/**
  \reimp
  */
int FbDriveModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mEngineWrapper->itemCount();
}

/**
  \reimp
  */
QVariant FbDriveModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.model() != this)
        return QVariant();

    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole: {
            QStringList listItem;
            FbDriveEntry driveEntry(mEngineWrapper->getDriveEntry(index));
            if (mEngineWrapper->settings().fileViewMode() == EFileViewModeSimple)
            {
                const QString SimpleDriveEntry("%1: <%2>");
                listItem /*<< driveEntry.IconId() */
                        << SimpleDriveEntry.arg(driveEntry.driveLetter())
                                           .arg(driveEntry.mediaTypeString());
            } else if (mEngineWrapper->settings().fileViewMode() == EFileViewModeExtended) {
                const QString SimpleDriveEntry("%1: <%2>");
                const QString ExtendedDriveEntry("%1/%2 kB");
                listItem /*<< driveEntry.IconId()*/
                        << SimpleDriveEntry.arg(driveEntry.driveLetter())
                                           .arg(driveEntry.mediaTypeString())
                        << ExtendedDriveEntry.arg(QString::number(driveEntry.volumeInfoFree()/1024))
                                             .arg(QString::number(driveEntry.volumeInfoSize()/1024));

            }
            return listItem;
        }
    case Qt::DecorationRole: {
            if (mEngineWrapper) {
                QIcon icon;
                //TODO Drive ico has to be provided, for some reason it is not visible
                icon = mFileIconProvider->icon(QFileIconProvider::Drive);
                return QVariant(icon);
            }
        }
    }
    return QVariant();
}

/**
  \reimp
  */
QVariant FbDriveModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);

    // TODO, implement or remove
    return QVariant();
}


FbDriveEntry FbDriveModel::driveEntry(const QModelIndex &index) const
{
    return mEngineWrapper->getDriveEntry(index);
}

QString FbDriveModel::driveLetter(const QModelIndex &index) const
{
    QString diskLetter;
    if (index.row() >= 0 && index.row() < mEngineWrapper->itemCount()) {
        FbDriveEntry driveEntry(mEngineWrapper->getDriveEntry(index));
        diskLetter = driveEntry.driveLetter();
    }
    return diskLetter;
}

QString FbDriveModel::mediaTypeString(const QModelIndex &index) const
{
    QString mediaTypeString;
    if (index.row() >= 0 && index.row() < mEngineWrapper->itemCount()) {
        FbDriveEntry driveEntry(mEngineWrapper->getDriveEntry(index));
        mediaTypeString = driveEntry.mediaTypeString();
    }
    return mediaTypeString;
}

// ---------------------------------------------------------------------------
