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
#include "mpmpxcollectionframeworkwrapper_p.h"

/*!
    \class MpMpxCollectionFrameworkWrapper
    \brief Wrapper for mpx framework utilities.

    MPX framework wrapper provides Qt style interface to the MPX framework
    utilities. Its implementation is hidden using private class data pattern.
*/

/*!
    \fn void collectionPlaylistOpened()

    This signal is emitted when A new collection playlist is opened and 
    playback is initialized.

 */

/*!
    \fn void playlistSaved( bool success )

    This signal is emitted when playlist save operation is completed, it 
    indicates the operation \a success .

 */

/*!
    \fn void deleteStarted();

    This signal is emitted when song delete operation is started

 */

/*!
    \fn void songsDeleted( bool success )

    This signal is emitted when song delete operation is completed, it 
    indicates the operation \a success .

 */

/*!
    \fn void playlistsRenamed( bool success )

    This signal is emitted when playlist rename operation is completed, it
    indicates the operation  \a success .

 */

/*!
    \fn void aboutToAddSongs( int count )

    This signal is emitted when play list is prepared and about to be added.
    \a count Indicates number of songs to be added

 */

/*!
    \fn void isolatedCollectionOpened( MpMpxCollectionData* collectionData )

    This signal is emitted when an isolated collection is opened on \a context

 */
 
/*!
    \fn void containerContentsChanged()

    This signal is emitted when items are removed or inserted on the current 
    container.

 */
 
/*!
 Constructs the utility wrapper.
 */
MpMpxCollectionFrameworkWrapper::MpMpxCollectionFrameworkWrapper( quint32 clientSecureId, MpSongData *songData, QObject *parent )
    : QObject( parent )
{
    d_ptr = new MpMpxCollectionFrameworkWrapperPrivate( this );
    d_ptr->init( clientSecureId, songData );
}

/*!
 Destructs the utility wrapper.
 */
MpMpxCollectionFrameworkWrapper::~MpMpxCollectionFrameworkWrapper()
{
    delete d_ptr;
}

/*!
 Opens the collection for the given \a context.

 \sa collectionOpened()
 */
void MpMpxCollectionFrameworkWrapper::openCollection( TCollectionContext context )
{
    d_ptr->openCollection( context );
}

/*!
 Opens the collection at a specific \a index.

 \sa collectionOpened(), playlistOpened()
 */
void MpMpxCollectionFrameworkWrapper::openCollectionItem( int index )
{
    d_ptr->openCollectionItem( index );
}

/*!
 Navigates back to the container of the current items.

 \sa collectionOpened()
 */
void MpMpxCollectionFrameworkWrapper::back()
{
    d_ptr->back();
}

/*!
 Loads the user created \a playLists.

 */
void MpMpxCollectionFrameworkWrapper::findPlaylists( QStringList &playlists )
{
    d_ptr->findPlaylists( playlists );
}

/*!
 Creates a new playlist with name \a playlistName and adds \a selection and optionally uses \a collectionData.
 */
void MpMpxCollectionFrameworkWrapper::createPlaylist( QString &playlistName, QList<int> &selection, MpMpxCollectionData* collectionData )
{
    d_ptr->createPlaylist( playlistName, selection, collectionData );
}

/*!
 Adds \a selection to the playlist specified in \a playlistIndex.
 */
void MpMpxCollectionFrameworkWrapper::saveToPlaylist( int playlistIndex, QList<int> &selection )
{
    d_ptr->saveToPlaylist( playlistIndex, selection );
}

/*!
 Rename a playlist by \a index with \a name.
 */
void MpMpxCollectionFrameworkWrapper::renamePlaylist( QString &newName, int index )
{
    d_ptr->renamePlaylist( newName, index );
}

/*!
 Adds \a selection to the current playlist from the specified \a collectionData.
 */
void MpMpxCollectionFrameworkWrapper::saveToCurrentPlaylist( QList<int> &selection, MpMpxCollectionData *collectionData )
{
    d_ptr->saveToCurrentPlaylist( selection, collectionData );
}

/*!
 Rename a playlist with \a name.
 */
void MpMpxCollectionFrameworkWrapper::renamePlaylist( QString &newName )
{
    d_ptr->renamePlaylist( newName );
}

/*!
 Deletes \a selection from the collection.
 */
void MpMpxCollectionFrameworkWrapper::deleteSongs( QList<int> &selection )
{
    d_ptr->deleteSongs( selection );
}

/*!
 Sets the suffle according to \a active.
 */
void MpMpxCollectionFrameworkWrapper::setShuffle( bool active )
{
    d_ptr->setShuffle( active );
}

/*!
 Initiate a playback preview for the selected item.
 */
void MpMpxCollectionFrameworkWrapper::previewItem( int index )
{
    d_ptr->previewItem( index );
}


/*!
 Opens the an isolated collection with \a context.
 \sa isolatedCollectionOpened()
 */
void MpMpxCollectionFrameworkWrapper::openIsolatedCollection( TCollectionContext context )
{
       d_ptr->openIsolatedCollection( context );     
}

/*!
 Releases the resources used for the isolated collection.
 */
void MpMpxCollectionFrameworkWrapper::releaseIsolatedCollection()
{
    d_ptr->releaseIsolatedCollection();
}

/*!
 Finds all songs beloging to the album specified by the \a index.
 */
void MpMpxCollectionFrameworkWrapper::findAlbumSongs( int index )
{
    d_ptr->findAlbumSongs(index);
}

/*!
 Plays album with \a albumIndex starting with the songs with \a songIndex.
*/
void MpMpxCollectionFrameworkWrapper::playAlbumSongs( int albumIndex, int songIndex, MpMpxCollectionData* collectionData  )
{
     d_ptr->playAlbumSongs( albumIndex, songIndex, collectionData );
}

/*!
 Returns pointer to MpMpxCollectionData, which is the collection data.
 */
MpMpxCollectionData *MpMpxCollectionFrameworkWrapper::collectionData()
{
    return d_ptr->collectionData();
}

/*!
 Called from an engine slot to reopen the collection in its current state.

 \sa collectionOpened()
 */
void MpMpxCollectionFrameworkWrapper::reopenCollection()
{
    d_ptr->reopenCollection();
}

/*!
 Called from an engine slot to request a reorder operation , indicates that 
 the item with \a playlistId , \a songId is to be moved from 
 \a originalOrdinal to \a newOrdinal.
 */
void MpMpxCollectionFrameworkWrapper::reorderPlaylist( int playlistId, int songId, int originalOrdinal, int newOrdinal )
{
    d_ptr->reorderPlaylist( playlistId, songId, originalOrdinal, newOrdinal );
}

/*!
  Called to set the repeat feature to \a enable
 */
void MpMpxCollectionFrameworkWrapper::setRepeatFeatureEnabled( bool enable ) 
{
    d_ptr->setRepeatFeatureEnabled( enable );
}

/*!
 Called to set the shuffle feature to \a enable
 */
void MpMpxCollectionFrameworkWrapper::setShuffleFeatureEnabled( bool enable )
{
    d_ptr->setShuffleFeatureEnabled( enable );
}

/*!
 Called to open all songs path and trigger shuffle all songs play
 */
void MpMpxCollectionFrameworkWrapper::openShuffleAllSongsPath()
{
    d_ptr->openShuffleAllSongsPath();
}

/*!
 Retrieves song details for the specified \a index.
 */
void MpMpxCollectionFrameworkWrapper::retrieveSongDetails( int index )
{
    d_ptr->retrieveSongDetails( index );
}

/*!
 Called to save playback data used for publishing activity (i.e. restore path) 
 */
void  MpMpxCollectionFrameworkWrapper::savePath( QByteArray &data )
{
    d_ptr->savePath( data );
}

/*!
 Called to load playback data used from returning activity (i.e. restore path) 
 */
void MpMpxCollectionFrameworkWrapper::restorePath( const QByteArray &data ) 
{
    return d_ptr->restorePath( data );
}
/*!
 Cancels CollectionUiHelper request.
 */
void MpMpxCollectionFrameworkWrapper::cancelRequest()
{
    d_ptr->cancel();
}

//EOF
