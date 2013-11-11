


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
* Description: Music Player collection album art manager.
*
*/

#include <thumbnailmanager_qt.h>
#include <hbicon.h>

#include "mpcollectionalbumartmanager.h"
#include "mpmpxcollectiondata.h"
#include "mptrace.h"

const int KInitCacheSize = 10;
const int KMaxCacheSize = 50;

/*!
    \class MpCollectionAlbumArtManager
    \brief Music Player collection album art manager.

    Collection album art manager provides access to album art needed for
    display in certain collection views. It hides interface to the thumbnail
    manager and also implements a caching mechanism for performance reasons.
*/

/*!
    \fn void albumArtReady( int index )

    This signal is emitted when album art for \a index is ready.

    \sa getAlbumArt()
 */

/*!
    \fn void albumCacheReady()

    This signal is emitted when album art cache is ready.

    \sa cacheAlbumArt()
 */

/*!
 Constructs the album art manager.
 */
MpCollectionAlbumArtManager::MpCollectionAlbumArtManager( MpMpxCollectionData *data, QObject *parent )
    : QObject( parent ),
      mCollectionData( data ),
      mThumbnailManager( 0 ),
      mCachingInProgress( false ),
      mDefaultIcon( "qtg_large_album_art" ),
      mPendingRequest( false ),
      mDefaultArtEnabled( true )
{
    TX_ENTRY
    mThumbnailManager = new ThumbnailManager(this);
    mThumbnailManager->setMode(ThumbnailManager::Default);
    mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForQuality);
    mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailSmall);
    connect( mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
             this, SLOT(thumbnailReady(QPixmap, void *, int, int)) );

    mImageCache.setMaxCost(KMaxCacheSize);
    TX_EXIT
}

/*!
 Destructs the album art manager.
 */
MpCollectionAlbumArtManager::~MpCollectionAlbumArtManager()
{
    TX_ENTRY
    cancel();
    delete mThumbnailManager;
    TX_EXIT
}

/*!
 Returns the album art for the given \a index. If the album art is not
 available in its cache, an asynchronous request is made to the thumbnail manager
 and default icon is returned.

 \sa signal albumArtReady
 */
const HbIcon MpCollectionAlbumArtManager::albumArt( int index )
{
    TX_ENTRY_ARGS("index=" << index);
    HbIcon icon;
    if ( mImageCache.contains( index ) ) {
        icon = *mImageCache[index];
    }
    else {
        if ( mDefaultArtEnabled ) {
            icon = mDefaultIcon;
        }
        if ( !mRequestQueue.contains(index) ) {
            // Icon was not found in cache. If the item has AlbumArtUri, request it
            // through ThumbnailManager interface.
            QString albumArtUri = mCollectionData->itemData(index, MpMpxCollectionData::AlbumArtUri);
            if ( !albumArtUri.isEmpty() ) {
                if ( !mPendingRequest ) {
                    void *clientData = reinterpret_cast<void *>(index);
                    mRequestId = mThumbnailManager->getThumbnail( albumArtUri, clientData );
                    if ( mRequestId != -1 ) {
                        mPendingRequest = true;
                        TX_EXIT_ARGS("false - album art requested");
                    }
                    else {
                        TX_EXIT_ARGS("Err: thumbnail manager returned (-1) for getThumbnail request!");
                    }
                }
                else {
                    mRequestQueue.append( index );
                    TX_EXIT_ARGS("false - request queued");
                }
            }
        }
    }
    return icon;
}

/*!
 Before providing the new data to the view (list, grid, etc.), we want
 to make sure that we have enough album arts for the first screen.
 */
void MpCollectionAlbumArtManager::cacheFirstScreen()
{
    TX_ENTRY
    int count = mCollectionData->count();
    int initCount = ( count > KInitCacheSize ) ? KInitCacheSize : count;
    for ( int i = 0; i < initCount; i++ ) {
        albumArt(i);
    }
    if ( mPendingRequest ) {
        mCachingInProgress = true;
    }
    TX_EXIT
}

/*!
 Cancels all outstanding album art requests.

 \sa getAlbumArt, cacheAlbumArt
 */
void MpCollectionAlbumArtManager::cancel()
{
    TX_ENTRY
    if ( mPendingRequest ) {
        mThumbnailManager->cancelRequest(mRequestId);
    }
    mImageCache.clear();
    mRequestQueue.clear();
    mPendingRequest = false;
    mCachingInProgress = false;
    TX_EXIT
}


/*!
 changes the thumbnail size.

 */
void MpCollectionAlbumArtManager::setThumbnailSize(MpCommon::MpThumbType type)
{
    switch ( type ) {
    case MpCommon::ListThumb:
        mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailSmall);
        break;
    case MpCommon::TBoneThumb:
        mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailMedium);
        break;
    case MpCommon::MediaWallThumb:
    default:
        mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailLarge);
        break;
    }
    cancel();
}

/*!
 sets the Default art \a enabled.
 */
void MpCollectionAlbumArtManager::enableDefaultArt( bool enabled )
{
    mDefaultArtEnabled = enabled;
}

/*!
 Slot to be called when thumbnail bitmap generation or loading is complete.
 */
void MpCollectionAlbumArtManager::thumbnailReady( QPixmap pixmap, void *data, int id, int error )
{
    int index = reinterpret_cast<int>( data );
    TX_ENTRY_ARGS("index=" << index << ", id=" << id << ", error=" << error);
    if ( !error && id == mRequestId && !pixmap.isNull() ) {
        // Find the index
        HbIcon *tmpIconPtr = new HbIcon(pixmap);
        //TODO:Enable This optimization for WK16
        //tmpIconPtr->setFlags( HbIcon::NonThemeable );
        mImageCache.insert( index, tmpIconPtr );
        TX_LOG_ARGS( "Album art ready for index=" << index );
        if ( !mCachingInProgress ) {
            emit albumArtReady(index);
        }
    }
    else {
        TX_EXIT_ARGS("Err: thumbnail manager returned error for getThumbnail request!");
    }

    if ( mCachingInProgress ) {
        if ( index >= (KInitCacheSize - 1) || !mRequestQueue.count() ) {
            mCachingInProgress = false;
            for ( int i = 0; i <= KInitCacheSize; ++i ) {
                if ( mImageCache.contains(i) ) {
                    emit albumArtReady(i);
                }
            }
        }
    }

    mPendingRequest = false;
    if ( mRequestQueue.count() ) {
        int index = mRequestQueue.takeFirst();
        QString albumArtUri = mCollectionData->itemData(index, MpMpxCollectionData::AlbumArtUri);
        void *clientData = reinterpret_cast<void *>(index);
        mRequestId = mThumbnailManager->getThumbnail( albumArtUri, clientData );
        if ( mRequestId != -1 ) {
            mPendingRequest = true;
            TX_EXIT_ARGS("next album art requested");
        }
        else {
            TX_EXIT_ARGS("Err: thumbnail manager returned (-1) for getThumbnail request!");
        }
    }
    TX_EXIT
}

