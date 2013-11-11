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
* Description: Music Player collection view base container definition.
*
*/

#include <QtCore>

#include <hbabstractviewitem.h>
#include <hbgroupbox.h>

#include "mpcollectiondocumentloader.h"
#include "mpcollectioncontainer.h"
#include "mpcollectiondatamodel.h"
#include "mpmpxcollectiondata.h"
#include "mptrace.h"

/*!
    \class MpCollectionContainer
    \brief Music Player collection view base container definition.

    Collection container is a base container that provides interface to
    be implemented by the specific collection context containers, e.g.
    All Songs, Artists, etc.
*/

/*!
    \fn void itemActivated( int index );

    This signal is emitted when an item of the container with \a index is activated.
 */

/*!
    \fn void itemLongPressed( const QModelIndex &index, const QPointF &coords );

    This signal is emitted when an item of the container with \a index is long pressed.
 */

/*!
    \fn void albumCentered( int index );

    This signal is emitted only when container contains hgmediawall T-Bone when album
    with \a index is centered following a scrolling action.
 */

/*!
 Constructs the collection container.
 */
MpCollectionContainer::MpCollectionContainer( HbDocumentLoader *loader, QGraphicsItem *parent )
    : HbWidget(parent),
      mDocumentLoader(loader),
      mViewMode(MpCommon::DefaultView)
{
    TX_ENTRY
    mInfoBar = qobject_cast<HbGroupBox*>(mDocumentLoader->findWidget(QString( "infoBar")));
    TX_EXIT
}

/*!
 Destructs the collection container.
 */
MpCollectionContainer::~MpCollectionContainer()
{
    TX_LOG
}

/*!
 Sets the data model for the container.
 Some of the containers are used for multiple contexts, i.e. Artist, Albums, Playlists. So,
 setDataModel() is called multiple times to the same container object. We only want to connect
 to dataReloaded() signal once or we'll receive multiple call backs.
 */
void MpCollectionContainer::setDataModel( MpCollectionDataModel *dataModel )
{
    TX_ENTRY
    mDataModel = dataModel;
    mCollectionData = mDataModel->collectionData();
    mCollectionContext = mCollectionData->context();
    connect( dataModel, SIGNAL(dataReloaded()), this, SLOT(dataReloaded()), Qt::UniqueConnection );

    setupContainer();
    TX_EXIT
}

/*!
 Sets the view mode.
 */
void MpCollectionContainer::setViewMode( MpCommon::MpViewMode viewMode )
{
    mViewMode = viewMode;
}

