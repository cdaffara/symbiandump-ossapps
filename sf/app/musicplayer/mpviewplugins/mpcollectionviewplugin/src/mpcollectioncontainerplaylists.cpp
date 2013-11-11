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
* Description: Music Player collection container definition - Playlists.
*
*/

#include <QtCore>

#include <hbdocumentloader.h>
#include <hblistview.h>
#include <hbgroupbox.h>
#include <hblabel.h>
#include <hbparameterlengthlimiter.h>

#include "mpcollectioncontainerplaylists.h"
#include "mpcollectiondatamodel.h"
#include "mpmpxcollectiondata.h"
#include "mptrace.h"


/*!
    \class MpCollectionContainerPlaylists
    \brief Music Player collection container definition - Playlists.

    'Playlists' collection container implements the interface specified
    by MpCollectionContainer. It provides a layout and widgets for the
    'Playlists' view.

    This container handles the following contexts:
    \li ECollectionContextPlaylists
    \li ECollectionContextPlaylistSongs

    \sa MpCollectionContainer
*/

/*!
 Constructs the collection container.
 This container handles the both the auto playlists and user created playlists.
 */
MpCollectionContainerPlaylists::MpCollectionContainerPlaylists( HbDocumentLoader *loader, QGraphicsItem *parent )
    : MpCollectionListContainer(loader, parent),
      mCurrentPlaylistIndex(0)
{
    TX_LOG
    mCollectionContext = ECollectionContextPlaylists;

}

/*!
 Destructs the collection container.
 */
MpCollectionContainerPlaylists::~MpCollectionContainerPlaylists()
{
    TX_ENTRY
    delete mList;
    TX_EXIT
}

/*!
 Sets the data model for the container.
 */
void MpCollectionContainerPlaylists::setDataModel( MpCollectionDataModel *dataModel )
{
    TX_ENTRY
    MpCollectionListContainer::setDataModel(dataModel);
    if ( mCollectionContext == ECollectionContextPlaylists ) {
        if ( mList ) {
            if ( mCollectionData->count() ) {
                mList->scrollTo( dataModel->index(mCurrentPlaylistIndex, 0) );
            }
        }
    }
    TX_EXIT
}

/*!
 Slot to be called when an item is selected by the user.
*/
void MpCollectionContainerPlaylists::itemActivated( const QModelIndex &index )
{
    if ( mCollectionContext == ECollectionContextPlaylists ) {
        mCurrentPlaylistIndex = index.row();
        TX_ENTRY_ARGS("mCurrentPlaylistIndex=" << mCurrentPlaylistIndex);
    }
    MpCollectionListContainer::itemActivated(index);
    TX_EXIT
}

/*!
 Slot to be called when data model has new data.
 Use cases:
     1) User renames a playlist.
	 2) Song added or removed from playlist.
 */
void MpCollectionContainerPlaylists::dataReloaded()
{
    TX_ENTRY
    MpCollectionListContainer::dataReloaded();
    if ( mCollectionContext == ECollectionContextPlaylistSongs ) {
        // Playlist could have been renamed.
        QString details = HbParameterLengthLimiter("txt_mus_subhead_1_2l").arg(mCollectionData->collectionTitle()).arg(mCollectionData->count());
        mInfoBar->setHeading(details);
        if ( mCollectionData->count() > 1 ) {
            emit shuffleEnabled(true);
        }
        else {
            emit shuffleEnabled(false);
        }
    }
    else if ( mCollectionContext == ECollectionContextPlaylists ) {
        mInfoBar->setHeading(hbTrId("txt_mus_subhead_playlists_1l").arg(mCollectionData->count()));
    }
    
    TX_EXIT
}

/*!
 Sets up the container by organizing widgets according to its layout.

 \reimp
 */
void MpCollectionContainerPlaylists::setupContainer()
{
    TX_ENTRY_ARGS("mCollectionContext=" << mCollectionContext);
    
    mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "showInfoBar");
       
    if ( mCollectionData->count() ) {
        bool ok = false;
        QGraphicsWidget *widget;
        if ( mCollectionContext == ECollectionContextPlaylists ) {
            mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "playlists", &ok);
            if ( !ok ) {
                TX_LOG_ARGS("Error: invalid xml file.");
                Q_ASSERT_X(ok, "MpCollectionContainerPlaylists::setupContainer", "invalid xml file");
            }
            if ( !mList ) {
                widget = mDocumentLoader->findWidget(QString("playlistsList"));
                mList = qobject_cast<HbListView*>(widget);
                initializeList();
            }

            mInfoBar->setHeading(hbTrId("txt_mus_subhead_playlists_1l").arg(mCollectionData->count()));
        }
        else if ( mCollectionContext == ECollectionContextPlaylistSongs ) {
            mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "playlistSongs", &ok);
            if ( !ok ) {
                TX_LOG_ARGS("Error: invalid xml file.");
                Q_ASSERT_X(ok, "MpCollectionContainerPlaylists::setupContainer", "invalid xml file");
            }
            
            QString details;
            if ( mViewMode == MpCommon::FetchView ) {
                details = hbTrId("txt_mus_subtitle_select_song");
            }
            else {
                details = HbParameterLengthLimiter("txt_mus_subhead_1_2l").arg(mCollectionData->collectionTitle()).arg(mCollectionData->count()); 
            }
            mInfoBar->setHeading(details);
        }
        if ( mNoMusic ) {
            delete mNoMusic;
            mNoMusic = 0;
        }
    }
    else {
    
        mInfoBar->setHeading(HbParameterLengthLimiter("txt_mus_subhead_1_2l").arg(mCollectionData->collectionTitle()).arg(0));

        // Call empty list from base class    
        setupEmptyListContainer();
    }
    TX_EXIT
}

