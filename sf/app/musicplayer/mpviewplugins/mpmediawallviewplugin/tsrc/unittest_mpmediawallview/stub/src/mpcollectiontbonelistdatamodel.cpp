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

#include "stub/inc/mpcollectiontbonelistdatamodel.h"
#include "stub/inc/mpmpxcollectiondata.h"
#include "mptrace.h"

/*!
 Stub function
 */
MpCollectionTBoneListDataModel::MpCollectionTBoneListDataModel(MpMpxCollectionData *collectionData,
                                                               MpPlaybackData *playbackData, QObject *parent )
    : QAbstractListModel(parent)
{
    Q_UNUSED(collectionData);
    Q_UNUSED(playbackData);
}

/*!
 Stub function
 */
MpCollectionTBoneListDataModel::~MpCollectionTBoneListDataModel()
{
}

/*!
 Stub function
 */
int MpCollectionTBoneListDataModel::rowCount( const QModelIndex &parent ) const
{
    Q_UNUSED(parent);
    return 0;
}

/*!
 Stub function
 */
QVariant MpCollectionTBoneListDataModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index);
    Q_UNUSED(role);    
    return QVariant();;
}

/*!
 Stub function
 */
void MpCollectionTBoneListDataModel::enablePlaybackIndicatorEnable(bool enable)
{
    Q_UNUSED(enable);
}

/*!
 Stub function
 */
void MpCollectionTBoneListDataModel::fileCorrupted(int songId)
{
    Q_UNUSED(songId);
}
