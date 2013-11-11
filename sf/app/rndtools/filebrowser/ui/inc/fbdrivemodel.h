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

#ifndef FBDRIVEMODELH_H_
#define FBDRIVEMODELH_H_

#include "fbdriveentry.h"

#include <QAbstractListModel>
#include <QVariant>

// Forward declarations
class EngineWrapper;
class QModelIndex;
class QFileIconProvider;
class QObject;

class FbDriveModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit FbDriveModel(EngineWrapper *engineWrapper, QObject *parent = 0);
    virtual ~FbDriveModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    FbDriveEntry driveEntry(const QModelIndex &index) const;
    QString driveLetter( const QModelIndex &index ) const;
    QString mediaTypeString(const QModelIndex &index) const;

private:
    EngineWrapper *mEngineWrapper;
    QFileIconProvider *mFileIconProvider;
};



#endif /* FBDRIVEMODELH_H_ */
