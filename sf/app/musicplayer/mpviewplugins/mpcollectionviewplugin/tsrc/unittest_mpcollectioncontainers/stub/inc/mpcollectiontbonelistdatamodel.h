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
* Description: Music Player collection abstract data model (for TBone) stub
*              for testing mpcollectioncontainers.
*
*/

#ifndef MPCOLLECTIONTBONELISTDATAMODEL_H
#define MPCOLLECTIONTBONELISTDATAMODEL_H

#include <QAbstractListModel>

class MpMpxCollectionData;
class MpPlaybackData;

class MpCollectionTBoneListDataModel : public QAbstractListModel
{
    Q_OBJECT

public:

    explicit MpCollectionTBoneListDataModel( MpMpxCollectionData *data, MpPlaybackData *playbackData = 0,
            QObject *parent=0 );
    virtual ~MpCollectionTBoneListDataModel();

    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

public slots:

    void refreshModel();

private:

    MpMpxCollectionData             *mCollectionData;   // Not own
    int                             mRowCount;

};

#endif // MPCOLLECTIONTBONELISTDATAMODEL_H

