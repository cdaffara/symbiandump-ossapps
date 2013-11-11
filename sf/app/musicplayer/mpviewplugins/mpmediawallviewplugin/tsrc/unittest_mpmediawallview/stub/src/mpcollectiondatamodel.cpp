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


#include "stub/inc/mpcollectiondatamodel.h"
#include "stub/inc/mpmpxcollectiondata.h"
#include "stub/inc/mpplaybackdata.h"


/*!
 Stub function
 */
MpCollectionDataModel::MpCollectionDataModel( MpMpxCollectionData *data, MpPlaybackData *playbackData, QObject *parent )
    : QAbstractListModel(parent)
{
    Q_UNUSED(data);
    Q_UNUSED(playbackData);
}

/*!
 Stub function
 */
MpCollectionDataModel::~MpCollectionDataModel()
{
}

/*!
 Stub function
 */
int MpCollectionDataModel::rowCount( const QModelIndex &parent ) const
{
    Q_UNUSED(parent);
    return 0;
}

/*!
 Stub function
 */
QVariant MpCollectionDataModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index);
    Q_UNUSED(role);
    return QVariant();
}

/*!
 Stub function
 */
void MpCollectionDataModel::refreshModel()
{
}

/*!
 Stub function
 */
MpMpxCollectionData *MpCollectionDataModel::collectionData()
{
    return new MpMpxCollectionData();
}

/*!
 Stub function
 */
void MpCollectionDataModel::setItemVisibility(const QModelIndex &index, bool visible)
{
    Q_UNUSED(index);
    Q_UNUSED(visible);
}
