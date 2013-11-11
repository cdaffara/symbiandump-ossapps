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

#include "fbfilemodel.h"
#include "enginewrapper.h"
#include "fbfileentry.h"
#include "filebrowsersettings.h"
#include "FB.hrh"

#include <QModelIndex>
#include <QFileIconProvider>

/**
  Constructs a file browser custom system model with the given \a engineWrapper and \a parent.
  */
FbFileModel::FbFileModel(EngineWrapper *engineWrapper, QObject *parent) :
    QAbstractListModel(parent),
    mEngineWrapper(engineWrapper),
    mFileIconProvider(0)
{
    mFileIconProvider = new QFileIconProvider();
}

/**
  Destroys this file browser custom system model.
  */
FbFileModel::~FbFileModel()
{  
    if (mFileIconProvider) {
        delete mFileIconProvider;
    }
}

/**
  \reimp
  */
int FbFileModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mEngineWrapper->itemCount();
}

/**
  \reimp
  */
QVariant FbFileModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.model() != this)
        return QVariant();

    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole: {
            QStringList listItem;
            FbFileEntry fileEntry(mEngineWrapper->getFileEntry(index));
            if (mEngineWrapper->settings().fileViewMode() == EFileViewModeSimple)
            {
                listItem /*<< fileEntry.IconId()*/
                        << fileEntry.name();
            } else if (mEngineWrapper->settings().fileViewMode() == EFileViewModeExtended) {
                QString extraData;
                extraData.append(fileEntry.modifiedString());
                if (fileEntry.isDir() && fileEntry.dirEntries() >= 0) {
                    extraData.append(" - ");
                    extraData.append(fileEntry.dirEntriesString());
                } else if (!fileEntry.isDir()) {
                    extraData.append(" - ");
                    extraData.append(fileEntry.sizeString());
                }
                listItem /*<< fileEntry.IconId()*/
                        << fileEntry.name() << extraData << fileEntry.attributesString();
            }
            return listItem;
        }
    case Qt::DecorationRole: {
            if (mEngineWrapper) {
                QIcon icon;
                    FbFileEntry fileEntry(mEngineWrapper->getFileEntry(index));
                    if (fileEntry.isDir()) {
                        icon = mFileIconProvider->icon(QFileIconProvider::Folder);
                    } else {
                        icon = mFileIconProvider->icon(QFileIconProvider::File);
                    }
//                }
                return QVariant(icon);
            }
        }
    }

    return QVariant();
}

/**
  \reimp
  */
QVariant FbFileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(role)

    // TODO, implement or remove
    return QVariant();
}

// ---------------------------------------------------------------------------
