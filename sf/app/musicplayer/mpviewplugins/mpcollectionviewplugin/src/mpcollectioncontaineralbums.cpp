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
* Description: Music Player collection container definition - Albums.
*
*/

#include <QtCore>

#include <hbdocumentloader.h>
#include <hblistview.h>
#include <hbgroupbox.h>
#include <hbindexfeedback.h>

#include <hgmediawall.h>

#include "mpcollectioncontaineralbums.h"
#include "mpmpxcollectiondata.h"
#include "mpcollectiondatamodel.h"
#include "mpcollectiontbonelistdatamodel.h"
#include "mptrace.h"
#include "mpenginefactory.h"

/*!
    \class MpCollectionContainerAlbums
    \brief Music Player collection container definition - Albums.

    'Albums' collection container implements the interface specified
    by MpCollectionContainer. It provides a layout and widgets for the
    'Albums' view.

    This container handles the following contexts:
    \li ECollectionContextAlbums
    \li ECollectionContextAlbumsTBone

    \sa MpCollectionContainer
*/

/*!
 Constructs the collection container.
 */
MpCollectionContainerAlbums::MpCollectionContainerAlbums( HbDocumentLoader *loader, QGraphicsItem *parent )
    : MpCollectionListContainer(loader, parent),
      mTBone(0),
      mTBoneListModel(0),
      mCurrentAlbumIndex(0)
{
    TX_LOG
    mCollectionContext = ECollectionContextAlbums;
}

/*!
 Destructs the collection container.
 */
MpCollectionContainerAlbums::~MpCollectionContainerAlbums()
{
    TX_ENTRY
	delete mTBone;
    delete mList;
    delete mTBoneListModel;
    TX_EXIT
}

/*!
 Sets the data model for the container.
 */
void MpCollectionContainerAlbums::setDataModel( MpCollectionDataModel *dataModel )
{
    TX_ENTRY
    MpCollectionContainer::setDataModel(dataModel);
    if ( mCollectionContext == ECollectionContextAlbums ) {
        if ( mList ) {
            mList->setModel(dataModel);
            if ( mCollectionData->count() ) {
                mList->scrollTo( dataModel->index(mCurrentAlbumIndex, 0) );
            }
        }
    }
    else if ( mCollectionContext == ECollectionContextAlbumsTBone ) {
        if ( mTBone ) {
            mTBone->setModel(dataModel);
            mTBone->scrollTo( dataModel->index(mCurrentAlbumIndex, 0) );
            if ( mTBoneListModel == 0 ) {
                mTBoneListModel = new MpCollectionTBoneListDataModel(mCollectionData, MpEngineFactory::sharedEngine()->playbackData());
                connect( mTBoneListModel, SIGNAL(albumDataChanged()), this, SLOT(albumDataChanged()) );
                connect( mTBoneListModel, SIGNAL(albumDataAvailable()), this, SLOT(albumDataAvailable()) );
            }
            mList->setModel(mTBoneListModel);
            if ( mCollectionData->setCurrentAlbum(mCurrentAlbumIndex) ) {
                if ( mCollectionData->albumSongsCount() > 1 ) {
                    emit shuffleEnabled(true);
                }
                else {
                    emit shuffleEnabled(false);
                }
            }
            else {
                emit findAlbumSongs(mCurrentAlbumIndex);
                emit shuffleEnabled(false);
            }
        }
    }
    TX_EXIT
}

/*!
 Slot to be called when an item is selected by the user.
*/
void MpCollectionContainerAlbums::itemActivated( const QModelIndex &index )
{
    if ( mCollectionContext == ECollectionContextAlbums ) {
        mCurrentAlbumIndex = index.row();
        TX_ENTRY_ARGS("mCurrentAlbumIndex=" << mCurrentAlbumIndex);
        MpCollectionListContainer::itemActivated(index);
    }
    else if ( mCollectionContext == ECollectionContextAlbumsTBone ) {
        int row = index.row();
        TX_ENTRY_ARGS("index=" << row);
        if ( mViewMode == MpCommon::FetchView ) {
            MpCollectionListContainer::itemActivated(index);
        }
        else {
            emit playAlbumSongs(mCurrentAlbumIndex, row);
        }
    }
    TX_EXIT
}

/*!
 Slot to be called when scrolling ends in media wall and an album is centered.
 */
void MpCollectionContainerAlbums::albumCentered( const QModelIndex &index )
{
    TX_ENTRY
    if ( mCurrentAlbumIndex != index.row() ) {
        // Prevent reloading if user just moves the center album a little
        // and the same album re-centers.
        mCurrentAlbumIndex = index.row();
        TX_LOG_ARGS("mCurrentAlbumIndex=" << mCurrentAlbumIndex);
        if ( mCollectionData->setCurrentAlbum(mCurrentAlbumIndex) ) {
            if ( mCollectionData->albumSongsCount() > 1 ) {
                emit shuffleEnabled(true);
            }
            // Enable context menu
            mLongPressEnabled = true;
        }
        else {
            emit findAlbumSongs(mCurrentAlbumIndex);
        }
    }
    else {
        // Landed on the same album. Just update menu.
        if ( mCollectionData->albumSongsCount() > 1 ) {
            emit shuffleEnabled(true);
        }
        // Enable context menu
        mLongPressEnabled = true;
    }
    TX_EXIT
}

/*!
 Slot to be called data model has new data.
 Two cases:
     1) User deleted an album.
	 2) User deleted last song in an album.
 */
void MpCollectionContainerAlbums::dataReloaded()
{
    TX_ENTRY
    if ( mCollectionContext == ECollectionContextAlbumsTBone ) {
        if ( mCurrentAlbumIndex > 0 ) {
            --mCurrentAlbumIndex;
        }
        mTBone->scrollTo( mDataModel->index(mCurrentAlbumIndex, 0) );
        if ( mCollectionData->setCurrentAlbum(mCurrentAlbumIndex) ) {
            if ( mCollectionData->albumSongsCount() == 1 ) {
                emit shuffleEnabled(false);
            }
        }
        else {
            emit findAlbumSongs(mCurrentAlbumIndex);
        }
    }
    else if( mCollectionContext == ECollectionContextAlbums ) {
        mInfoBar->setHeading(hbTrId("txt_mus_subhead_albums_1l").arg(mCollectionData->count()));
    }
    else {
        MpCollectionListContainer::dataReloaded();
    }
    TX_EXIT
}

/*!
 Slot to be called data model has new data.
 User has deleted one of the songs from TBone list.
 */
void MpCollectionContainerAlbums::albumDataChanged()
{
    TX_ENTRY
    emit findAlbumSongs(mCurrentAlbumIndex);
    emit shuffleEnabled(false);
    TX_EXIT
}

/*!
 Slot to be called TBone starts scrolling.
 */
void MpCollectionContainerAlbums::scrollingStarted()
{
    TX_ENTRY
    // Disable shuffle action from the menu
    emit shuffleEnabled(false);
    // Disable context menu
    mLongPressEnabled = false;
    TX_EXIT
}

/*!
 Slot to be called album data is available. This is a result of findAlbumSongs signal.
 */
void MpCollectionContainerAlbums::albumDataAvailable()
{
    TX_ENTRY
    int count = mCollectionData->albumSongsCount();
    if ( count > 1 ) {
        emit shuffleEnabled(true);
    }
    // Enable context menu
    mLongPressEnabled = true;
    TX_EXIT
}

/*!
 Sets up the container by organizing widgets according to its layout.

 \reimp
 */
void MpCollectionContainerAlbums::setupContainer()
{
    TX_ENTRY_ARGS("mCollectionContext=" << mCollectionContext);
    
    mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "showInfoBar");
    
    if ( mCollectionData->count() ) {
        bool ok = false;
        QGraphicsWidget *widget;
        if ( mCollectionContext == ECollectionContextAlbums ) {
            mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "albums", &ok);
            if ( !ok ) {
                TX_LOG_ARGS("Error: invalid xml file.");
                Q_ASSERT_X(ok, "MpCollectionContainerAlbums::setupContainer", "invalid xml file");
            }
            if ( !mList ) {
                widget = mDocumentLoader->findWidget(QString("albumsList"));
                mList = qobject_cast<HbListView*>(widget);
                mIndexFeedback->setItemView(mList);
                initializeList();
            }
            if ( mTBone ) {
                delete mTBone;
                mTBone = 0;
            }
                        
            mInfoBar->setHeading(hbTrId("txt_mus_subhead_albums_1l").arg(mCollectionData->count()));
            
        }
        else if ( mCollectionContext == ECollectionContextAlbumsTBone ) {
            if ( mViewMode == MpCommon::FetchView ) {
                mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "albumTBoneFetcher", &ok);
                if ( !ok ) {
                    TX_LOG_ARGS("Error: invalid xml file.");
                    Q_ASSERT_X(ok, "MpCollectionContainerAlbums::setupContainer", "invalid xml file");
                }
 
                mInfoBar->setHeading(hbTrId("txt_mus_subtitle_select_song"));
            }
            else {
                mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "albumTBone", &ok);
                if ( !ok ) {
                    TX_LOG_ARGS("Error: invalid xml file.");
                    Q_ASSERT_X(ok, "MpCollectionContainerAlbums::setupContainer", "invalid xml file");
                }
                
                mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "hideInfoBar");
                
            }
            widget = mDocumentLoader->findWidget(QString("albumWall"));
            mTBone = qobject_cast<HgMediawall*>(widget);
            HbIcon defaultIcon( "qtg_large_album_art" );
            defaultIcon.setSize(mTBone->itemSize());
            mTBone->setDefaultImage( defaultIcon.pixmap().toImage() );
            mTBone->setScrollBarPolicy( HgWidget::ScrollBarAlwaysOff );
            mTBone->enableReflections( false );
            connect( mTBone, SIGNAL(scrollingStarted()), this, SLOT(scrollingStarted()) );
            connect( mTBone, SIGNAL(animationAboutToEnd(QModelIndex)), this, SLOT(albumCentered(QModelIndex)) );
        }
    }
    else {
        // Must delete widgets when last song is deleted and view is reloaded.
        if ( mTBone ) {
            delete mTBone;
            mTBone = 0;
        }
        
        mInfoBar->setHeading(hbTrId("txt_mus_subhead_albums_1l").arg(0));
        
        // Call empty list from base class
        setupEmptyListContainer();
    }
    TX_EXIT
}

