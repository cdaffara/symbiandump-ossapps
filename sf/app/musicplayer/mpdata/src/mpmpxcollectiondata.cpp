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
* Description: Music Player collection data.
*
*/

#include "mpmpxcollectiondata.h"
#include "mpmpxcollectiondata_p.h"

/*!
    \class MpMpxCollectionData
    \brief Music Player collection data.

    Collection data provides access to collection data needed by the view.
    Every item of data can be accessed via an index and its details by specifying
    the data type of the selected data item.
*/

/*!
    \enum MpMpxCollectionData::DataType

    This enum defines available data types.
 */

/*!
    \fn void contextChanged( TCollectionContext context )

    This signal is emitted when a new data set is available and the
    collection context is changed.

    \sa setMpxMedia()
 */

/*!
    \fn void dataChanged()

    This signal is emitted when a new data set is available within the
    same context. This can happen after operations like delete and several
    playlist related operations (save, rearrange, remove).

    \sa setMpxMedia()
 */

/*!
    \fn void dataChanged( int fromIndex, int toIndex )

    This signal is emitted when a new data set is available within the
    same context. This can happen during an incremental open operation,
    if the view accesses a range of data that doesn't have final data
    (i.e. Unknown). This signal is emitted upon completion of incremental
    open operation, if such unavailable range was accessed.

    \sa setMpxMedia()
 */

/*!
    \fn void albumDataChanged()

    This signal is specific to views with TBone. This signal is emitted when
    a new data set is available for the list section of the TBone. Currently,
    the only operation that can trigger this is the delete operation.

    \sa setMpxMedia()
 */

/*!
    \fn void refreshAlbumSongs()

    This signal is specific to views with TBone. This signal is emitted when
    album song data is available (for the list section of the TBone), upon
    completion of MpEngine's findAlbumSongs() operation.

    \sa setAlbumContent()
 */

/*!
 Constructs the collection data.
 */
MpMpxCollectionData::MpMpxCollectionData( QObject *parent )
    : QObject(parent)
{
    d_ptr = new MpMpxCollectionDataPrivate(this);
}

/*!
 Destructs the collection data.
 */
MpMpxCollectionData::~MpMpxCollectionData()
{
    delete d_ptr;
}

/*!
 Returns the current collection context.
 /sa setContext()
 */
TCollectionContext MpMpxCollectionData::context() const
{
    return d_ptr->context();
}

/*!
 Returns the number of data entries.
 */
int MpMpxCollectionData::count() const
{
    return d_ptr->count();
}

/*!
 Returns the \a title of the collection data based on the current context.
 */
QString MpMpxCollectionData::collectionTitle() const
{
    return d_ptr->collectionTitle();
}

/*!
 Returns the data for the specified \a index and \a type.
 */
QString MpMpxCollectionData::itemData( int index, MpMpxCollectionData::DataType type ) const
{
    return d_ptr->itemData(index, type);
}

/*!
 Returns if the current container is of type 'Auto playlist'.
 */
bool MpMpxCollectionData::isAutoPlaylist()
{
    return d_ptr->isAutoPlaylist();
}

/*!
 Returns if the item specified by \a index is of type 'Auto playlist'.
 */
bool MpMpxCollectionData::isAutoPlaylist( int index )
{
    return d_ptr->isAutoPlaylist(index);
}

/*!
 Returns the count of the items contained within item specified by \a index.
 */
int MpMpxCollectionData::itemCount( int index )
{
    return d_ptr->itemCount(index);
}

/*!
 Returns the current container id.
 */
int MpMpxCollectionData::containerId()
{
    return d_ptr->containerId();
}

/*!
 Returns id of the item specified by \a index
 */
int MpMpxCollectionData::itemId( int index )
{
    return d_ptr->itemId(index);
}

/*!
 Returns id of the album song specified by \a index
 */
int MpMpxCollectionData::albumSongId( int index )
{
    return d_ptr->albumSongId( index );
}

/*!
 Removes item specified by \a index, also internally caches the item for drag
 and drop operations.
 
 \sa testCachedItem() insertCachedItem()
 */
void MpMpxCollectionData::removeItem( int index )
{
    d_ptr->removeItem( index );
}

/*!
 Test wether the cached item matches \a itemId.
 \sa removeItem() insertCachedItem()
 */
bool MpMpxCollectionData::testCachedItem( int itemId )
{
    return d_ptr->testCachedItem( itemId );
}

/*!
 Inserts the cached item in the location specified by \a index
 \sa removeItem() testCachedItem()
 */
void MpMpxCollectionData::insertCachedItem(int index)
{
    d_ptr->insertCachedItem( index );
}

/*!
 Sets the current album in focus. Also, triggers refreshAlbumSongs() signal
 if data contains album songs already and returns true. Otherwise, function
 returns false.
 */
bool MpMpxCollectionData::setCurrentAlbum( int index )
{
    return d_ptr->setCurrentAlbum( index );
}

/*!
 Returns the current album index.
 \sa setCurrentAlbum()
 */
int MpMpxCollectionData::currentAlbumIndex() const
{
    return d_ptr->currentAlbumIndex();
}

/*!
 Returns the number of songs in the current album.
 */
int MpMpxCollectionData::albumSongsCount() const
{
    return d_ptr->albumSongsCount();
}

/*!
 Returns the data for the specified \a index and \a type in the current album.
 Currently supported data types are: Title and Uri. All others will return

 empty string.

 \sa setCurrentAlbum()
 */
QString MpMpxCollectionData::albumSongData( int index, MpMpxCollectionData::DataType type ) const
{
    return d_ptr->albumSongData( index, type );
}

/*!
 Returns item property for the specified \a index and \a type in the current item.
 Currently supported data types are: Corrupted and DrmExpired. All others will return
 false bool.
 */

bool MpMpxCollectionData::hasItemProperty( int index, MpMpxCollectionData:: DataProperty type ) const
{
    return d_ptr->hasItemProperty( index, type );
}

/*!
 Returns item property for the specified \a index and \a type in the current album.
 Currently supported data types are: Corrupted and DrmExpired. All others will return
 false bool.
 */
bool MpMpxCollectionData::hasAlbumSongProperty( int index, MpMpxCollectionData:: DataProperty type ) const
{
    return d_ptr->hasAlbumSongProperty( index, type);
}

/*!
 Sets the media \a entries from the MPX framework. The \a reopen flag indicates
 whether the new media is a result of reopenCollection() operation in the MpEngine.
 Internal usage only from MpEngine.
 */
void MpMpxCollectionData::setMpxMedia( const CMPXMedia& entries, bool reopen )
{
    d_ptr->setMpxMedia(entries, reopen);
}

/*!
 Called when collection was opened with incremental open. This indicates that the
 media received in setMpxMedia() has an update.
 */
void MpMpxCollectionData::incrementalOpenUpdate()
{
    d_ptr->incrementalOpenUpdate();
}

/*!
 Returns the container media from the MPX framework.
 Internal usage only from MpEngine.

 \sa setMpxMedia()
 */
const CMPXMedia& MpMpxCollectionData::containerMedia()
{
    return d_ptr->containerMedia();
}

/*!
 Sets the context.
 Used to fake open() and back() in T-Bone supporting views.
 Used to force the context for Media Wall view.
 */
void MpMpxCollectionData::setContext( TCollectionContext context )
{
    d_ptr->setContext( context );
}

/*!
 Sets the media \a albumContent from the MPX framework.
 Internal usage only from MpEngine.
 Album songs have been populated. Notify the change by emitting
 refreshAlbumSongs() signal.

 \sa setCurrentAlbum()
 */
void MpMpxCollectionData::setAlbumContent( const CMPXMedia& albumContent )
{
    d_ptr->setAlbumContent(albumContent);
}

/*!
 Returns the index of the item with \a ItemUniqueId.
 Only supported for ECollectionContextAlbumsMediaWall.

 \sa setContext()
 */
int MpMpxCollectionData::itemIndex( int itemUniqueId )
{
    return d_ptr->itemIndex( itemUniqueId );
}

/*!
 Returns the index within the album data for the song with \a songUniqueId.

 */
int MpMpxCollectionData::albumSongIndex( int songUniqueId )
{
    return d_ptr->albumSongIndex( songUniqueId );
}

/*!
 Returns the list of index for the song with \a songUniqueId. 
 A list is returned due to playlist can contain the same song 
 multiple times.

 */
QList<int>  MpMpxCollectionData::songIndex( int songUniqueId )
{
    return d_ptr->songIndex( songUniqueId );
}

/*!
 Set model index \a index as corrupted.  

 */
void MpMpxCollectionData::setCorruptValue( QModelIndex index, bool tBone )
{
    d_ptr->setCorruptValue( index, tBone );
}

/*!
 Set the reloadAlbumContent variable to \a reloadAlbum

 */
void MpMpxCollectionData::setReloadAlbumContent( bool reloadAlbum )
{
    d_ptr->setReloadAlbumContent( reloadAlbum );
}

