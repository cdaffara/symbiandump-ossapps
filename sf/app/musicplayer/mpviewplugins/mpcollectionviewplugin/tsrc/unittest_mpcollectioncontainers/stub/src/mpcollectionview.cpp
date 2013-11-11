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
* Description: Music Player collection view stub for testing mpcollectioncontainers
*
*/

// INCLUDE FILES

#include <cstdlib>
#include <QtCore>

#include "stub/inc/mpcollectionview.h"
#include "mpcollectiondocumentloader.h"
#include "mpcollectioncontainerfactory.h"

#include "mptrace.h"



const char*MUSIC_COLLECTION_DOCML = ":/docml/musiccollection.docml";


/*!
 Stub function
 */
MpCollectionView::MpCollectionView()
    : mContainerFactory(0),
      mDocumentLoader(0)
{
    TX_LOG
}

/*!
 Stub function
 */
MpCollectionView::~MpCollectionView()
{
    TX_ENTRY
    delete mContainerFactory;
    delete mDocumentLoader;
    TX_EXIT
}

/*!
 Stub function
 */
void MpCollectionView::initializeView()
{
    TX_ENTRY

    mDocumentLoader = new MpCollectionDocumentLoader();
    bool ok = false;
    mDocumentLoader->load( MUSIC_COLLECTION_DOCML, &ok );
    mContainerFactory = new MpCollectionContainerFactory(this, mDocumentLoader);

    TX_EXIT
}

/*!
 Stub function
 */
void MpCollectionView::openIndex( int index )
{
    Q_UNUSED(index);
}

/*!
 Stub function
 */
void MpCollectionView::openContextMenu( const QModelIndex &index, const QPointF &coords )
{
    Q_UNUSED(index);
    Q_UNUSED(coords);
}

/*!
 Stub function
 */
void MpCollectionView::findAlbumSongs( int index )
{
    Q_UNUSED(index);
}

/*!
 Stub function
 */
void MpCollectionView::playAlbumSongs( int albumIndex, int songIndex )
{
    Q_UNUSED(albumIndex);
    Q_UNUSED(songIndex);
}


