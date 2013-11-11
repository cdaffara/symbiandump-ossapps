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
* Description: Music Player collection abstract data model stub for testing
*              mpcollectioncontainers.
*
*/

#ifndef MPCOLLECTIONDATAMODEL_H
#define MPCOLLECTIONDATAMODEL_H

#include <QAbstractListModel>

class MpMpxCollectionData;
class MpCollectionAlbumArtManager;
class MpPlaybackData;


class MpCollectionDataModel : public QAbstractListModel
{
    Q_OBJECT

public:

    explicit MpCollectionDataModel( MpMpxCollectionData *data,  MpPlaybackData *playbackData = 0,
            QObject *parent=0 );
    virtual ~MpCollectionDataModel();

    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

    void refreshModel();
    MpMpxCollectionData *collectionData();

public:

    MpMpxCollectionData             *mCollectionData;
	int                             mRowCount;
    bool                            mCachingInProgress;

};

#endif // MPCOLLECTIONDATAMODEL_H

