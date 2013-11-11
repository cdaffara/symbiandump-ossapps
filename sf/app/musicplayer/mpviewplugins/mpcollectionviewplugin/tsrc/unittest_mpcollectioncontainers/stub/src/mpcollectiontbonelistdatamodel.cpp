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
#include "stub/inc/mpplaybackdata.h"
#include "mptrace.h"

/*!
 Stub function
 */
MpCollectionTBoneListDataModel::MpCollectionTBoneListDataModel( MpMpxCollectionData *data, 
        MpPlaybackData *playbackData, QObject *parent )
    : QAbstractListModel(parent),
      mCollectionData(data),
      mRowCount(0)
{
    Q_UNUSED(playbackData);
    TX_LOG
}

/*!
 Stub function
 */
MpCollectionTBoneListDataModel::~MpCollectionTBoneListDataModel()
{
    TX_LOG
}

/*!
 Stub function
 */
int MpCollectionTBoneListDataModel::rowCount( const QModelIndex &parent ) const
{
    TX_LOG
    Q_UNUSED(parent);
    return mRowCount;
}

/*!
 Stub function
 */
QVariant MpCollectionTBoneListDataModel::data(const QModelIndex &index, int role) const
{
    TX_ENTRY
    Q_UNUSED(index);
    Q_UNUSED(role);
    QVariant returnValue = QVariant();
    TX_EXIT
    return returnValue;
}

/*!
 Stub function
 */
void MpCollectionTBoneListDataModel::refreshModel()
{
    TX_LOG
}

