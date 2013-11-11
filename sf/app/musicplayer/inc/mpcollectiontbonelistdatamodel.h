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
* Description: Music Player collection abstract data model. This is
*              primarily used to support TBone Mediawall in Collection View.
*              This model represents the list containing album songs.
*
*/

#ifndef MPCOLLECTIONTBONELISTDATAMODEL_H
#define MPCOLLECTIONTBONELISTDATAMODEL_H

#include <QAbstractListModel>

class MpMpxCollectionData;
class MpPlaybackData;

#if defined(BUILD_MPDATA_LIB)
#define MPDATA_EXPORT Q_DECL_EXPORT
#else
#define MPDATA_EXPORT Q_DECL_IMPORT
#endif

class MPDATA_EXPORT MpCollectionTBoneListDataModel : public QAbstractListModel
{
    Q_OBJECT

public:

    explicit MpCollectionTBoneListDataModel( MpMpxCollectionData *collectionData, 
            MpPlaybackData *playbackData = 0, QObject *parent = 0 );
    virtual ~MpCollectionTBoneListDataModel();

    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    void enablePlaybackIndicatorEnable(bool enable);

signals:

    void albumDataChanged();
    void albumDataAvailable();

public slots:

    void refreshModel();
    void updateSong();
    void updatePlaybackState();
    void fileCorrupted(int songId);
    

private:

    MpMpxCollectionData             *mCollectionData; // Not own
    MpPlaybackData                  *mPlaybackData;   // Not own
    int                             mRowCount;
    int                             mCurrentSongId;
    bool                            mPlaybackActive;
    bool                            mPlaybackIndicatorEnabled;

};

#endif // MPCOLLECTIONTBONELISTDATAMODEL_H

