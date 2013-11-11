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
* Description: Wrapper for mpx collection framework utilities.
*
*/

#include "mpmpxcollectionframeworkwrapper.h"
#include "mpmpxcollectiondata.h"
 
/*!
 Constructs the utility wrapper.
 */
MpMpxCollectionFrameworkWrapper::MpMpxCollectionFrameworkWrapper( quint32 clientSecureId, MpSongData *songData, QObject *parent )
    : QObject( parent )
{
    Q_UNUSED(clientSecureId);
    Q_UNUSED(songData);
}

/*!
 Destructs the utility wrapper.
 */
MpMpxCollectionFrameworkWrapper::~MpMpxCollectionFrameworkWrapper()
{

}

/*!
 Opens the collection for the given \a context.

 \sa collectionOpened()
 */
void MpMpxCollectionFrameworkWrapper::openCollection( TCollectionContext context )
{
    Q_UNUSED(context);
}

/*!
 Opens the collection at a specific \a index.

 \sa collectionOpened(), playlistOpened()
 */
void MpMpxCollectionFrameworkWrapper::openCollectionItem( int index )
{
    Q_UNUSED(index);
}

/*!
 Navigates back to the container of the current items.

 \sa collectionOpened()
 */
void MpMpxCollectionFrameworkWrapper::back()
{

}

/*!
 Loads the user created \a playLists.

 */
void MpMpxCollectionFrameworkWrapper::findPlaylists( QStringList &playlists )
{
    Q_UNUSED(playlists);
}

/*!
 Creates a new playlist with name \a playlistName and adds \a selection and optionally uses \a collectionData.
 */
void MpMpxCollectionFrameworkWrapper::createPlaylist( QString &playlistName, QList<int> &selection, MpMpxCollectionData* collectionData )
{
    Q_UNUSED(playlistName);
    Q_UNUSED(selection);
    Q_UNUSED(collectionData);
}

/*!
 Adds \a selection to the playlist specified in \a playlistIndex.
 */
void MpMpxCollectionFrameworkWrapper::saveToPlaylist( int playlistIndex, QList<int> &selection )
{
    Q_UNUSED(playlistIndex);
    Q_UNUSED(selection);
}

/*!
 Rename a playlist by \a index with \a name.
 */
void MpMpxCollectionFrameworkWrapper::renamePlaylist( QString &newName, int index )
{
    Q_UNUSED(newName);
    Q_UNUSED(index);
}

/*!
 Adds \a selection to the current playlist from the specified \a collectionData.
 */
void MpMpxCollectionFrameworkWrapper::saveToCurrentPlaylist( QList<int> &selection, MpMpxCollectionData *collectionData )
{
    Q_UNUSED(selection);
    Q_UNUSED(collectionData);
}

/*!
 Rename a playlist with \a name.
 */
void MpMpxCollectionFrameworkWrapper::renamePlaylist( QString &newName )
{
    Q_UNUSED(newName);
}

/*!
 Deletes \a selection from the collection.
 */
void MpMpxCollectionFrameworkWrapper::deleteSongs( QList<int> &selection )
{
    Q_UNUSED(selection);
}

/*!
 Sets the suffle according to \a active.
 */
void MpMpxCollectionFrameworkWrapper::setShuffle( bool active )
{
    Q_UNUSED(active);
}

/*!
 Initiate a playback preview for the selected item.
 */
void MpMpxCollectionFrameworkWrapper::previewItem( int index )
{
    Q_UNUSED(index);
}


/*!
 Opens the an isolated collection with \a context.
 \sa isolatedCollectionOpened()
 */
void MpMpxCollectionFrameworkWrapper::openIsolatedCollection( TCollectionContext context )
{
    Q_UNUSED(context);
}

/*!
 Releases the resources used for the isolated collection.
 */
void MpMpxCollectionFrameworkWrapper::releaseIsolatedCollection()
{

}

/*!
 Finds all songs beloging to the album specified by the \a index.
 */
void MpMpxCollectionFrameworkWrapper::findAlbumSongs( int index )
{
    Q_UNUSED(index);
}

/*!
 Plays album with \a albumIndex starting with the songs with \a songIndex.
*/
void MpMpxCollectionFrameworkWrapper::playAlbumSongs( int albumIndex, int songIndex, MpMpxCollectionData* collectionData  )
{
    Q_UNUSED(albumIndex);
    Q_UNUSED(songIndex);
    Q_UNUSED(collectionData);
}

/*!
 Returns pointer to MpMpxCollectionData, which is the collection data.
 */
MpMpxCollectionData *MpMpxCollectionFrameworkWrapper::collectionData()
{
    MpMpxCollectionData *data = new MpMpxCollectionData();
    return data;
}

/*!
 Called from an engine slot to reopen the collection in its current state.

 \sa collectionOpened()
 */
void MpMpxCollectionFrameworkWrapper::reopenCollection()
{

}

/*!
 Called from an engine slot to request a reorder operation , indicates that 
 the item with \a playlistId , \a songId is to be moved from 
 \a originalOrdinal to \a newOrdinal.
 */
void MpMpxCollectionFrameworkWrapper::reorderPlaylist( int playlistId, int songId, int originalOrdinal, int newOrdinal )
{
    Q_UNUSED(playlistId);
    Q_UNUSED(songId);
    Q_UNUSED(originalOrdinal);
    Q_UNUSED(newOrdinal);
}

/*!
  Called to set the repeat feature to \a enable
 */
void MpMpxCollectionFrameworkWrapper::setRepeatFeatureEnabled( bool enable ) 
{
    Q_UNUSED(enable);
}

/*!
 Called to set the shuffle feature to \a enable
 */
void MpMpxCollectionFrameworkWrapper::setShuffleFeatureEnabled( bool enable )
{
    Q_UNUSED(enable);
}

/*!
 Called to open all songs path and trigger shuffle all songs play
 */
void MpMpxCollectionFrameworkWrapper::openShuffleAllSongsPath()
{

}

/*!
 Retrieves song details for the specified \a index.
 */
void MpMpxCollectionFrameworkWrapper::retrieveSongDetails( int index )
{
    Q_UNUSED(index);
}

/*!
 Called to save playback data used for publishing activity (i.e. restore path) 
 */
void  MpMpxCollectionFrameworkWrapper::savePath( QByteArray &data )
{
    Q_UNUSED(data);
}

/*!
 Called to load playback data used from returning activity (i.e. restore path) 
 */
void MpMpxCollectionFrameworkWrapper::restorePath( const QByteArray &data ) 
{
    Q_UNUSED(data);
}
/*!
 Cancels CollectionUiHelper request.
 */
void MpMpxCollectionFrameworkWrapper::cancelRequest()
{
}

//EOF
