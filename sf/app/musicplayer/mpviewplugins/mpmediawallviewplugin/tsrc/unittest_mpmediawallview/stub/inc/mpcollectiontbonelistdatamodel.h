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
*              for testing mpmediawallview.
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

    explicit MpCollectionTBoneListDataModel( MpMpxCollectionData *collectionData,
                MpPlaybackData *playbackData = 0, QObject *parent = 0 );
    virtual ~MpCollectionTBoneListDataModel();

    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    void enablePlaybackIndicatorEnable(bool enable);
    
public slots:

    void fileCorrupted(int songId);


};

#endif // MPCOLLECTIONTBONELISTDATAMODEL_H

