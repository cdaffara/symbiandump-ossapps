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
* Description: Music Player collection data - private implementation.
*
*/

#include <thumbnaildata.h>
#include <thumbnailobjectsource.h>

#include <mpxmedia.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>

#include "mpmpxcollectiondata_p.h"
#include "mptrace.h"

/*!
    \class MpMpxCollectionDataPrivate
    \brief Music Player collection data - private implementation.

    This is a private implementation of the collection data interface.
*/

/*!
 \internal
 */
MpMpxCollectionDataPrivate::MpMpxCollectionDataPrivate( MpMpxCollectionData *wrapper )
    : q_ptr(wrapper),
      iContext(ECollectionContextUnknown),
      iContainerMedia(0),
      iMediaArray(0),
      iCachedRemovedItem(0),
      iCurrentAlbumIndex(-1),
      iAlbumSongCount(0),
      iReloadAlbumContent(false),
	  iNeedReload(false)
{
    TX_LOG
}

/*!
 \internal
 */
MpMpxCollectionDataPrivate::~MpMpxCollectionDataPrivate()
{
    TX_ENTRY
    delete iContainerMedia;
    delete iCachedRemovedItem;
    TX_EXIT
}

/*!
 \internal
 */
TCollectionContext MpMpxCollectionDataPrivate::context() const
{
    return iContext;
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::count() const
{
    if ( iMediaArray ) {
        return iMediaArray->Count();
    }
    return 0;
}

/*!
 \internal
 */
QString MpMpxCollectionDataPrivate::collectionTitle() const
{
    TX_ENTRY
    QString title;
    if ( iContainerMedia && iContainerMedia->IsSupported( KMPXMediaGeneralTitle ) ) {
        const TDesC& titleText = iContainerMedia->ValueText( KMPXMediaGeneralTitle );
        if ( titleText.Compare( KNullDesC ) != 0 ) {
            title = QString::fromUtf16( titleText.Ptr(), titleText.Length() );
        }
    }
    TX_EXIT_ARGS("title =" << title);
    return title;
}

/*!
 \internal
 */
QString MpMpxCollectionDataPrivate::itemData( int index, MpMpxCollectionData::DataType type )
{
    TX_ENTRY_ARGS("index=" << index << ", type=" << type);
    QString data;
    TRAPD(err, DoGetDataL(index, type, data));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
    return data;
}

/*!
 \internal
 */
bool MpMpxCollectionDataPrivate::isAutoPlaylist()
{
    TX_ENTRY
    if ( iContext != ECollectionContextPlaylistSongs ) {
        TX_EXIT
        return false;
    }

    bool isAuto = false;
    TRAPD(err, isAuto = DoIsAutoPlaylistL());
    if ( err == KErrNone ) {
        TX_LOG_ARGS("isAuto=" << isAuto);
    }
    else {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
    return isAuto;
}

/*!
 \internal
 */
bool MpMpxCollectionDataPrivate::isAutoPlaylist( int index )
{
    TX_ENTRY_ARGS("index=" << index);
    if ( iContext != ECollectionContextPlaylists ) {
        TX_EXIT
        return false;
    }

    bool isAuto = false;
    TRAPD(err, isAuto = DoIsAutoPlaylistL(index));
    if ( err == KErrNone ) {
        TX_LOG_ARGS("isAuto=" << isAuto);
    }
    else {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
    return isAuto;
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::itemCount( int index )
{
    TX_ENTRY_ARGS("index=" << index);
    int count = 0;
    TRAPD(err, count = DoGetItemCountL(index));
    if ( err == KErrNone ) {
        TX_LOG_ARGS("count=" << count);
    }
    else {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
    return count;
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::containerId()
{
    int id = -1;
    TRAPD( err, id = DoGetContainerIdL() );
    if ( err == KErrNone ) {
        TX_LOG_ARGS("id=" << id);
    }
    else {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
    return id;
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::itemId( int index )
{
    TX_ENTRY_ARGS( "index=" << index );
    int id = -1;
    TRAPD( err, id = DoGetItemIdL( index ) );
    if ( err == KErrNone ) {
        TX_LOG_ARGS( "id=" << id );
    }
    else {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
    TX_EXIT
    return id;
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::albumSongId( int index )
{
    TX_ENTRY_ARGS( "index=" << index );
    int id = -1;
    TRAPD( err, id = DoGetAlbumSongIdL( index ) );
    if ( err == KErrNone ) {
        TX_LOG_ARGS( "id=" << id );
    }
    else {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
    TX_EXIT
    return id;
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::removeItem(int index)
{
    TX_ENTRY_ARGS("index=" << index);
     TRAPD(err,  DoRemoveItemL(index));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
bool MpMpxCollectionDataPrivate::testCachedItem( int itemId )
{
    TX_ENTRY_ARGS( "itemId=" << itemId);
    bool match = false;
    TRAPD( err, match = DoTestCachedItemL( itemId ) );
    if ( err == KErrNone ) {
        TX_LOG_ARGS("match=" << match);
    }
    else {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
    return match;
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::insertCachedItem(int index)
{
    TX_ENTRY_ARGS("index=" << index);
    iMediaArray->Insert( iCachedRemovedItem, index );
    iCachedRemovedItem = 0; //ownership tranferred above.
    TX_EXIT
}

/*!
 \internal
 */
bool MpMpxCollectionDataPrivate::setCurrentAlbum( int index )
{
    TX_ENTRY_ARGS( "index=" << index);
    bool available = false;
    TRAPD( err, available = DoSetCurrentAlbumL( index ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
    return available;
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::currentAlbumIndex() const
{
    return iCurrentAlbumIndex;
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::albumSongsCount() const
{
    return iAlbumSongCount;
}

/*!
 \internal
 */
QString MpMpxCollectionDataPrivate::albumSongData( int index, MpMpxCollectionData::DataType type ) const
{
    TX_ENTRY_ARGS("index=" << index << ", type=" << type);
    QString data;
    TRAPD(err, DoGetAlbumSongDataL(index, type, data));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
    return data;
}

/*!
 \internal
 */
bool MpMpxCollectionDataPrivate::hasItemProperty( int index, MpMpxCollectionData:: DataProperty type ) const
{
    TX_ENTRY_ARGS("index=" << index << ", type=" << type);
    bool available = false;
    TRAPD(err, available = DoHasItemPropertyL(index, type));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
    return available;
}

/*!
 \internal
 */
bool MpMpxCollectionDataPrivate::hasAlbumSongProperty( int index, MpMpxCollectionData:: DataProperty type ) const
{
    TX_ENTRY_ARGS("index=" << index << ", type=" << type);
    bool available = false;
    TRAPD(err, available = DoHasAlbumSongPropertyL(index, type));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
    return available;
}

/*!
 \internal
 New data from MPX collection. This could be due to Open operation, in which case
 context would have changed. This could also be due to Re-open after operations
 such as delete, playlist renaming, playlist rearraging, etc., in which case the
 context would remain the same, but the internal data may have changed.
 */
void MpMpxCollectionDataPrivate::setMpxMedia( const CMPXMedia& entries, bool reopen )
{
    TX_ENTRY
    TCollectionContext prevContext = iContext;
    int prevCount = count();

    TRAPD(err, DoSetMpxMediaL(entries));
    if ( err == KErrNone ) {
        int newCount = count();
        if ( (newCount == 0) || (prevCount == 0) ) {
            TX_LOG_ARGS("Empty container");
            // Two cases:
            //     1) newCount is zero: Last item in the model was deleted.
            //     2) prevCount is zero: Refresh operation found new data.
            // In these cases, independent of context change, we must emit the
            // contextChanged() signal so that the container can properly reload
            // the layout.
            emit q_ptr->contextChanged(iContext);
        }
        else if ( iContext != prevContext ) {
            TX_LOG_ARGS("Context changed: iContext=" << iContext);
            if ( prevContext == ECollectionContextArtistAlbumsTBone
                 && iContext == ECollectionContextArtistAlbums
                 && reopen ) {
                // Special case 1: This occurs when a song was deleted from TBone list
                // within artist container. And the fact that the new context is ArtistAlbums
                // indicates that the artist has more than 1 album.
                // Restore context to ArtistAlbumsTBone.
                iContext = ECollectionContextArtistAlbumsTBone;
                if ( newCount != prevCount ) {
                    // Change in count indicates that the deleted song was the last song
                    // in the TBone list. As a result, the album was deleted also, therefore
                    // we must emit dataChanged() indicating changes to the album section
                    // of the TBone.
                    emit q_ptr->dataChanged();
                }
                else {
                    // Same count indicates that one of the songs in the TBone's list
                    // section was deleted. We only want to reload the list section of the
                    // TBone in this case.
                    emit q_ptr->albumDataChanged();
                }
            }
            else if ( prevContext == ECollectionContextAlbumsTBone && reopen ) {
                // Special case 2: This occurs when a song was deleted from TBone list
                // within album container. Restore context to AlbumsTBone.
                iContext = ECollectionContextAlbumsTBone;
                if ( newCount != prevCount ) {
                    // Change in count indicates that the deleted song was the last song
                    // in the TBone list. As a result, the album was deleted also, therefore
                    // we must emit dataChanged() indicating changes to the album section
                    // of the TBone.
                    emit q_ptr->dataChanged();
                }
                else {
                    // Same count indicates that one of the songs in the TBone's list
                    // section was deleted. We only want to reload the list section of the
                    // TBone in this case.
                    emit q_ptr->albumDataChanged();
                }
            }
            else {
                // Simple case where the context has really changed and it didn't
                // involve TBone.
                emit q_ptr->contextChanged(iContext);
            }
        }
        else if ( prevContext == ECollectionContextArtistAlbumsTBone
                  && iContext == ECollectionContextArtistAlbumsTBone
                  && reopen ) {
            // Special case 3: This occurs when a song was deleted from TBone list
            // within artist container. This is similar to special case 1, however, the
            // fact that the new context is also ArtistAlbumsTBone indicates that the
            // artist has only 1 album. We only want to reload the list section of the
            // TBone in this case.
            emit q_ptr->albumDataChanged();
        }
        else {
            // Same context, but the data has changed.
            emit q_ptr->dataChanged();
        }
    }
    else {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 This indicates data update during incremental open operation. This indicates
 that media received in setMpxMedia() has updates.
 */
void MpMpxCollectionDataPrivate::incrementalOpenUpdate()
{
    TX_ENTRY_ARGS( "iNeedReload=" << iNeedReload);
    if ( iNeedReload ) {
        if ( itemId(iReloadRange.second) != -1 ) {
            iNeedReload = false;
            emit q_ptr->dataChanged(iReloadRange.first, iReloadRange.second);
        }
    }
    TX_EXIT
}

/*!
 \internal
 */
const CMPXMedia& MpMpxCollectionDataPrivate::containerMedia()
{
    return *iContainerMedia;
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::setContext( TCollectionContext context )
{
    iContext = context;
    TX_LOG_ARGS("Context changed: iContext=" << iContext);
    loadAlbumsLookup();
    emit q_ptr->contextChanged(iContext);
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::setAlbumContent( const CMPXMedia& albumContent )
{
    TX_ENTRY
    TRAPD(err, DoSetAlbumContentL(albumContent));
    if ( err == KErrNone ) {
        TX_LOG_ARGS("Album content is available.");
        loadAlbumSongsLookup();
        emit q_ptr->refreshAlbumSongs();
    }
    else {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
   Currently only used to lookup playing song album id to index of albums on 
   media wall.
 */
int MpMpxCollectionDataPrivate::itemIndex( int itemUniqueId )
{
    return iAlbumIdIndexMapping.value( itemUniqueId, -1 );
}

/*!
 \internal
   Currently only used to lookup playing song id to index of song in the 
   current album on media wall.
 */
int MpMpxCollectionDataPrivate::albumSongIndex( int songUniqueId )
{
    return iAlbumSongIdIndexMapping.value( songUniqueId, -1 );
}

/*!
 \internal
   Use to lookup playing song id to index of song in collection and playlist
   view 
 */
QList<int>  MpMpxCollectionDataPrivate::songIndex( int songUniqueId )
{
    TX_ENTRY
    if(iSongIdIndexMapping.empty()){
        for ( int i = count() - 1 ; i >= 0 ; i-- ) {
            iSongIdIndexMapping.insertMulti( itemId2( i ) , i );
        }
    }
    TX_EXIT
    return iSongIdIndexMapping.values( songUniqueId );
}

/*!
 \internal
   Set item at index to corrupted depends on if viewing TBone
 */
void MpMpxCollectionDataPrivate::setCorruptValue( QModelIndex index, bool tBone)
{
    TX_ENTRY
    TRAPD(err, DoSetCorruptValueL(index, tBone));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::setReloadAlbumContent( bool reloadAlbum )
{
    iReloadAlbumContent = reloadAlbum;
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::loadAlbumsLookup()
{
    //Clearing all the album ids.
    iAlbumIdIndexMapping.clear();
    if ( iContext == ECollectionContextAlbumsMediaWall) {
        //Adding album ids and indixes to the hash, for itemIndex lookup.
        //This is disabled for other containers to save resources.
        for ( int i = count() - 1 ; i >= 0 ; i-- ) {
            iAlbumIdIndexMapping.insert( itemId( i ) , i );
        }
    }
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::loadAlbumSongsLookup()
{
    //Clearing all the song ids.
    iAlbumSongIdIndexMapping.clear();
    //Adding album song ids and indixes to the hash, for albumSongIndex lookup.
    //This is disabled for other containers to save resources.
    for ( int i = albumSongsCount() - 1 ; i >= 0 ; i-- ) {
        iAlbumSongIdIndexMapping.insert( albumSongId( i ) , i );
    }
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::setReloadRange( int index )
{
    TX_ENTRY_ARGS( "index=" << index);
    if ( !iNeedReload ) {
        iNeedReload = true;
        iReloadRange.first = index;
        iReloadRange.second = index;
    }
    else if ( index < iReloadRange.first ) {
        iReloadRange.first = index;
    }
    else if ( index > iReloadRange.second ) {
        iReloadRange.second = index;
    }
    TX_EXIT
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::itemId2( int index )
{
    TX_ENTRY_ARGS( "index=" << index );
    int id = -1;
    TRAPD( err, id = DoGetItemId2L( index ) );
    if ( err == KErrNone ) {
        TX_LOG_ARGS( "id=" << id );
    }
    else {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
    TX_EXIT
    return id;
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::DoGetDataL( int index, MpMpxCollectionData::DataType type, QString& data )
{
    TX_ENTRY
    CMPXMedia* currentMedia( iMediaArray->AtL( index ) );

    if ( currentMedia->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId) == KMPXInvalidItemId ) {
        // Accessing a data that hasn't been fully fetched from the collection.
        // This can happen during incremental open. Set the index as reload candidate.
        setReloadRange(index);
    }

    TBuf<10> countBuf;
    TInt count = 0;
    switch ( type ) {
        case MpMpxCollectionData::Title:
            if ( currentMedia->IsSupported( KMPXMediaGeneralTitle ) ) {
                const TDesC& title = currentMedia->ValueText( KMPXMediaGeneralTitle );
                if ( title.Compare( KNullDesC ) != 0 ) {
                    data = QString::fromUtf16( title.Ptr(), title.Length() );
                }
            }
            break;
        case MpMpxCollectionData::Artist:
            if ( currentMedia->IsSupported( KMPXMediaMusicArtist ) ) {
                const TDesC& artist = currentMedia->ValueText( KMPXMediaMusicArtist );
                if ( artist.Compare( KNullDesC ) != 0 ) {
                    data = QString::fromUtf16( artist.Ptr(), artist.Length() );
                }
            }
            break;
        case MpMpxCollectionData::Album:
            if ( currentMedia->IsSupported( KMPXMediaMusicAlbum ) ) {
                const TDesC& album = currentMedia->ValueText( KMPXMediaMusicAlbum );
                if ( album.Compare( KNullDesC ) != 0 ) {
                    data = QString::fromUtf16( album.Ptr(), album.Length() );
                }
            }
            break;
        case MpMpxCollectionData::Count:
            if ( currentMedia->IsSupported( KMPXMediaGeneralCount ) ) {
                count = currentMedia->ValueTObjectL<TInt>( KMPXMediaGeneralCount );
            }
            countBuf.AppendNum( count );
            data = QString::fromUtf16( countBuf.Ptr(), countBuf.Length() );
            break;
        case MpMpxCollectionData::AlbumArtUri:
            if ( currentMedia->IsSupported( KMPXMediaMusicAlbumArtFileName ) ) {
                const TDesC& album = currentMedia->ValueText( KMPXMediaMusicAlbumArtFileName );
                if ( album.Compare( KNullDesC ) != 0 ) {
                    data = QString::fromUtf16( album.Ptr(), album.Length() );
                }
            }
            break;
        case MpMpxCollectionData::Uri:
            if ( currentMedia->IsSupported( KMPXMediaGeneralUri ) ) {
                const TDesC& uri = currentMedia->ValueText( KMPXMediaGeneralUri );
                if ( uri.Compare( KNullDesC ) != 0 ) {
                    data = QString::fromUtf16( uri.Ptr(), uri.Length() );
                }
            }
            break;
        default:
            break;
    }
    TX_EXIT
}

/*!
 \internal
 */
bool MpMpxCollectionDataPrivate::DoIsAutoPlaylistL()
{
    if ( iContainerMedia->IsSupported(KMPXMediaGeneralNonPermissibleActions) ) {
        TMPXGeneralNonPermissibleActions attr(
                iContainerMedia->ValueTObjectL<TMPXGeneralNonPermissibleActions>(
                KMPXMediaGeneralNonPermissibleActions ) );
        if ( attr & EMPXWrite ) {
            return true;
        }
    }
    return false;
}

/*!
 \internal
 */
bool MpMpxCollectionDataPrivate::DoIsAutoPlaylistL( int index )
{
    const CMPXMediaArray* containerArray = iContainerMedia->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    User::LeaveIfNull( const_cast<CMPXMediaArray*>( containerArray ));
    CMPXMedia* media( containerArray->AtL(index) );

    if ( media->IsSupported(KMPXMediaGeneralNonPermissibleActions) ) {
        TMPXGeneralNonPermissibleActions attr(
            media->ValueTObjectL<TMPXGeneralNonPermissibleActions>(
                KMPXMediaGeneralNonPermissibleActions ) );
        if ( attr & EMPXWrite ) {
            return true;
        }
    }
    return false;
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::DoGetItemCountL( int index )
{
    CMPXMedia* currentMedia( iMediaArray->AtL( index ) );
    int count = 0;
    if ( currentMedia->IsSupported( KMPXMediaGeneralCount ) ) {
        count = currentMedia->ValueTObjectL<TInt>( KMPXMediaGeneralCount );
    }
    return count;
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::DoGetContainerIdL()
{
    if ( !iContainerMedia->IsSupported( KMPXMediaGeneralId ) ) {
        User::Leave(KErrNotFound);
    }
    return iContainerMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::DoGetItemIdL( int index )
{
    CMPXMedia* currentMedia( iMediaArray->AtL( index ) );
    if ( currentMedia->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId) == KMPXInvalidItemId ) {
        return -1;
    }
    else {
        int id1 = (currentMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId )).iId1;
        int id2 = (currentMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId )).iId2;
        TX_LOG_ARGS( "id1=" << id1 << ", id2=" << id2 );
        return (currentMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId )).iId1;
    }
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::DoGetItemId2L( int index )
{
    CMPXMedia* currentMedia( iMediaArray->AtL( index ) );
    if ( currentMedia->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId) == KMPXInvalidItemId ) {
        return -1;
    }
    else {
        return (currentMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId )).iId2;
    }
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::DoGetAlbumSongIdL( int index )
{
    CMPXMedia* album( iMediaArray->AtL( iCurrentAlbumIndex ) );
    const CMPXMediaArray* songs = album->Value<CMPXMediaArray>(KMPXMediaArrayContents);
    User::LeaveIfNull(const_cast<CMPXMediaArray*>(songs));
    CMPXMedia* song = songs->AtL(index);
    if ( song->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId) == KMPXInvalidItemId ) {
        return -1;
    }
    else {
        return song->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
    }
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::DoRemoveItemL( int index )
{
    delete iCachedRemovedItem;
    iCachedRemovedItem = 0;
    iCachedRemovedItem = CMPXMedia::NewL( *iMediaArray->AtL( index ) );
    iMediaArray->Remove( index );
}

/*!
 \internal
 */
bool MpMpxCollectionDataPrivate::DoTestCachedItemL( int itemId )
{
    if ( !iCachedRemovedItem || !iCachedRemovedItem->IsSupported( KMPXMediaGeneralId ) ) {
        User::Leave(KErrNotFound);
    }
    return ( itemId == TInt( iCachedRemovedItem->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) ) );
}

/*!
 \internal
 */
bool MpMpxCollectionDataPrivate::DoSetCurrentAlbumL( int index )
{
    TX_ENTRY_ARGS( "index=" << index);
    iCurrentAlbumIndex = index;

    bool songsAvailable = false;
    if (!iReloadAlbumContent){    
        CMPXMedia* album( iMediaArray->AtL( index ) );
        if ( album->IsSupported(KMPXMediaArrayContents) ) {
            // We've previously fetched the songs for this album so
            // all we do now is populate the list with the song titles.
            const CMPXMediaArray* songs = album->Value<CMPXMediaArray>(KMPXMediaArrayContents);
            iAlbumSongCount = songs->Count();
            songsAvailable = true;
            TX_LOG_ARGS("Songs available.");
            loadAlbumSongsLookup();
            emit q_ptr->refreshAlbumSongs();
        }
    }
    TX_EXIT
    return songsAvailable;
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::DoGetAlbumSongDataL( int index, MpMpxCollectionData::DataType type, QString& data ) const
{
    TX_ENTRY
    CMPXMedia* album( iMediaArray->AtL( iCurrentAlbumIndex ) );
    if ( album->IsSupported(KMPXMediaArrayContents) ) {
        const CMPXMediaArray* songs = album->Value<CMPXMediaArray>(KMPXMediaArrayContents);
        User::LeaveIfNull(const_cast<CMPXMediaArray*>(songs));
        CMPXMedia* song = songs->AtL(index);

        switch ( type ) {
            case MpMpxCollectionData::Title:
                if ( song->IsSupported( KMPXMediaGeneralTitle ) ) {
                    const TDesC& title = song->ValueText( KMPXMediaGeneralTitle );
                    if ( title.Compare( KNullDesC ) != 0 ) {
                        data = QString::fromUtf16( title.Ptr(), title.Length() );
                    }
                }
                break;
            case MpMpxCollectionData::Uri:
                if ( song->IsSupported( KMPXMediaGeneralUri ) ) {
                    const TDesC& uri = song->ValueText( KMPXMediaGeneralUri );
                    if ( uri.Compare( KNullDesC ) != 0 ) {
                        data = QString::fromUtf16( uri.Ptr(), uri.Length() );
                    }
                }
                break;
            default:
                break;
        }
    }
    TX_EXIT
}

/*!
 \internal
 */
bool MpMpxCollectionDataPrivate::DoHasItemPropertyL( int index, MpMpxCollectionData:: DataProperty type ) const
{
    TX_ENTRY
    CMPXMedia* currentMedia( iMediaArray->AtL( index ) );

    TInt flags(0);
    if ( currentMedia->IsSupported( KMPXMediaGeneralFlags ) ) {
        flags = currentMedia->ValueTObjectL<TUint>( KMPXMediaGeneralFlags );
    }
    switch ( type ) {
        case MpMpxCollectionData::Corrupted:
            if ( ( flags ) & ( KMPXMediaGeneralFlagsIsCorrupted ) ){
                return true;
            }
            break;
        case MpMpxCollectionData::DrmExpired:
            if ( ( flags ) & ( KMPXMediaGeneralFlagsIsDrmLicenceInvalid ) ){
                return true;
            }
            break;
        default:
            break;
    }
    TX_EXIT
    return false;    
}

/*!
 \internal
 */
bool MpMpxCollectionDataPrivate::DoHasAlbumSongPropertyL( int index, MpMpxCollectionData:: DataProperty type ) const
{
    TX_ENTRY
    CMPXMedia* album( iMediaArray->AtL( iCurrentAlbumIndex ) );
    if ( album->IsSupported(KMPXMediaArrayContents) ) {
        const CMPXMediaArray* songs = album->Value<CMPXMediaArray>(KMPXMediaArrayContents);
        User::LeaveIfNull(const_cast<CMPXMediaArray*>(songs));
        CMPXMedia* song = songs->AtL(index);
        TInt flags(0);
        if ( song->IsSupported( KMPXMediaGeneralFlags ) ) {
            flags = song->ValueTObjectL<TUint>( KMPXMediaGeneralFlags );
        }
        switch ( type ) {
            case MpMpxCollectionData::Corrupted:
                if ( ( flags ) & ( KMPXMediaGeneralFlagsIsCorrupted ) ){
                    return true;
                }
                break;
            case MpMpxCollectionData::DrmExpired:
                if ( ( flags ) & ( KMPXMediaGeneralFlagsIsDrmLicenceInvalid ) ){
                    return true;
                }
                break;
            default:
                break;
        }
        
    }
    TX_EXIT
    return false;    
}
/*!
 \internal
 */
void MpMpxCollectionDataPrivate::SetCollectionContextL()
{
    TX_ENTRY
	// Clear Song Index Hash when context switched
    iSongIdIndexMapping.clear();
    TMPXGeneralType containerType( EMPXNoType );
    if ( iContainerMedia->IsSupported( KMPXMediaGeneralType ) ) {
        containerType = iContainerMedia->ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType );
    }

    TMPXGeneralCategory containerCategory( EMPXNoCategory );
    if( iContainerMedia->IsSupported( KMPXMediaGeneralCategory ) ) {
        containerCategory = iContainerMedia->ValueTObjectL<TMPXGeneralCategory>( KMPXMediaGeneralCategory );
    }
    TX_LOG_ARGS("type=" << containerType << ", category=" << containerCategory );

    iContext = ECollectionContextUnknown;
    if ( containerType == EMPXGroup ) {
        switch (containerCategory) {
            case EMPXSong:
                iContext = ECollectionContextAllSongs;
                break;
            case EMPXArtist:
                iContext = ECollectionContextArtists;
                break;
            case EMPXAlbum:
                iContext = ECollectionContextAlbums;
                break;
            case EMPXPlaylist:
                iContext = ECollectionContextPlaylists;
                break;
        }
    }
    else if ( containerType == EMPXItem ) {
        switch (containerCategory) {
            case EMPXArtist:
                if ( iMediaArray->Count() > 1 ) {
                    iContext = ECollectionContextArtistAlbums;
                }
                else {
                    // Single album. Go directly to TBone.
                    iContext = ECollectionContextArtistAlbumsTBone;
                }
                break;
            case EMPXSong:
                // All songs for an artist
                iContext = ECollectionContextArtistAllSongs;
                break;
            case EMPXPlaylist:
                iContext = ECollectionContextPlaylistSongs;
                break;
        }
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::DoSetMpxMediaL( const CMPXMedia& entries )
{
    TX_ENTRY
    delete iContainerMedia;
    iContainerMedia = 0;
    iContainerMedia = CMPXMedia::NewL(entries);
    iMediaArray = const_cast<CMPXMediaArray*>(iContainerMedia->Value<CMPXMediaArray>( KMPXMediaArrayContents ) );
    TX_LOG_ARGS("media count=" << iMediaArray->Count() );
    iReloadAlbumContent = false;
    SetCollectionContextL();
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::DoSetAlbumContentL( const CMPXMedia& albumContent )
{
    TX_ENTRY
    CMPXMediaArray* songArray(const_cast<CMPXMediaArray*>( albumContent.Value<CMPXMediaArray>(
                    KMPXMediaArrayContents ) ) );
    User::LeaveIfNull( songArray );

    // Save the songs to the album so that we don't need to find them again
    // if the same album is selected again.
    iAlbumSongCount = songArray->Count();

    if ( iAlbumSongCount ) {
        CMPXMedia* albumMedia( iMediaArray->AtL( iCurrentAlbumIndex ) );
        albumMedia->SetCObjectValueL(KMPXMediaArrayContents, songArray);
        albumMedia->SetTObjectValueL<TInt>(KMPXMediaArrayCount, iAlbumSongCount);
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::DoSetCorruptValueL(QModelIndex index, bool tBone)
{
    TX_ENTRY
    if (tBone){
        CMPXMedia* album( iMediaArray->AtL( iCurrentAlbumIndex ) );
        if ( album->IsSupported(KMPXMediaArrayContents) ) {
            const CMPXMediaArray* songs = album->Value<CMPXMediaArray>(KMPXMediaArrayContents);
            User::LeaveIfNull(const_cast<CMPXMediaArray*>(songs));
            CMPXMedia* song = songs->AtL( index.row() );
            song->SetTObjectValueL<TUint>( KMPXMediaGeneralFlags,KMPXMediaGeneralFlagsIsCorrupted );
        }
    }
    else {
        CMPXMedia* song( iMediaArray->AtL( index.row() ) );
        song->SetTObjectValueL<TUint>( KMPXMediaGeneralFlags,KMPXMediaGeneralFlagsIsCorrupted );        
    }
    TX_EXIT  
}

