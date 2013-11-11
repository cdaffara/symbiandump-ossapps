/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Part of SyncML Data Synchronization Plug In Adapter
*
*/



#ifndef _CMEDIAMANAGER_H
#define _CMEDIAMANAGER_H

#include <e32base.h>
#include <mpxmediageneraldefs.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionmediator.h>
#include <mpxcollectionobserver.h>
#include <mpxharvesterutilityobserver.h>
#include <mpxharvesterutility.h>
#include <mpxcollectionhelperobserver.h>
 
#include "changefinder.h"
#include "songitem.h"

class CMPXMedia;
class MMPXCollectionHelper;

class MMediaManagerObserver : public MMPXCollectionObserver
    {
public:
    virtual void RegisterAllPlayListsCompleted( TInt aError ) = 0;
    virtual void RegisterAllSongsCompleted( TInt aError ) = 0;
    };

/**
 * @brief This class manages playlist's on media library. Also songs metadata can be queried.
 * 
 */
class CMediaManager : public CActive, private MMPXCollectionHelperObserver
    {
public:

    static CMediaManager* NewL( RFs& aFs, MMediaManagerObserver* aObserver, TKeyArrayFix aKey, CMD5& aHasher );
    
    ~CMediaManager();
    
    /**
     * Starts scanning music library.
     * Calls MMPXCollectionObserver::HandleCollectionMessage when completed.
     */
    void ScanL();
    
    /**
     * Cancel scan
     */
    void CancelScanL();
    
    /**
     * Set current snapshot.
     * @param aSnapshot
     */
    void SetSnapshot( CSnapshotArray* aSnapshot ){iSnapshotArray=aSnapshot;};
    
    /**
     * Starts registering all playlists.
     * Snapshot must be set before calling this.
     * Calls MMediaManagerObserver::RegisterAllPlayListsCompleted when ready
     * @return KErrNone if operation succesfully started.
     */
    TInt RegisterAllPlayLists();
    
    /**
     * Starts registering all songs.
     * Snapshot must be set before calling this.
     * Calls MMediaManagerObserver::RegisterAllSongsCompleted when ready
     * @return KErrNone if operation succesfully started.
     */
    TInt RegisterAllSongs();
    
    /**
     * Gets CPlaylistItem
     * @param aPlaylistId playlist id to get.
     * @param aPlaylist returned playlist.
     */
    void GetPlayListL( TInt aPlaylistId, CPlaylistItem& aPlaylist );
    
    /**
     * Gets CSongItem
     * @param aSongId song id to get.
     * @param aSongItem returned song.
     */
    void GetSongL( TInt aSongId, CSongItem& aSongItem );
    
    /**
     * Creates new playlist to music library
     * @param aNewUid returns new playlist id, if succesfully added.
     * @param aPlaylist playlist to be added. Uri is updated to aPlaylist.
     */
    void CreateNewPlaylistL( TInt& aNewUid, CPlaylistItem& aPlaylist );
    
    /**
     * Replaces existing playlist in music library
     * @param aPlaylistId playlist id to be replaced. Uri is read to aPlaylist.
     * @param aPlaylist new content.
     */
    void ReplacePlaylistL( TInt aPlaylistId, CPlaylistItem& aPlaylist );
    
    /**
     * Deletes playlist from music library
     * @param aPlaylistId playlist id to be deleted.
     */
    void DeletePlaylistL( TInt aPlaylistId );
    
private: // From MMPXCollectionHelperObserver
    void HandleAddFileCompleteL( TInt aErr );
    
private:
    CMediaManager( RFs& aFs, MMediaManagerObserver* aObserver, TKeyArrayFix aKey, CMD5& aHasher );
    
    void ConstructL();
    
    void RegisterPlaylistL( const CMPXMedia& aMedia );
    
    void RegisterSongL( const CMPXMedia& aMedia );
    
    CMPXMedia* FetchPlaylistContentL( TInt aId );
    
    CMPXMedia* FindPlaylistL( TInt aUid );
    
    void AddPlaylistL( CMPXMedia* aMedia );
    
    void ReadSongAttributesL( const CMPXMedia& aMedia, CSongItem& aSongItem, TBool aReadFileDetails );
    
    CMPXMedia* CreatePlaylistMediaL( const CPlaylistItem& aPlaylist );
    
    CMPXMediaArray* CreateMediaArrayLC( const CPlaylistItem& aPlaylist );
    
    void ReadPlaylistCountL();
    void ReadPlaylistItemsL();
    
    void ReadSongCountL();
    void ReadSongItemsL();
private: // from CActive
    
    void DoCancel();
    void RunL();
    TInt RunError( TInt aError );
    
private:
    
    enum TState
        {
        EIdle = 0,
        EReadingPlaylistCount,
        EEnumeratingPlaylists,
        EReadingSongCount,
        EEnumeratingSongs
        };
    TState iCurrentState;
    
    CMPXMedia*              iFoundedMedia;
    TInt                    iReadedItems;
    TInt                    iItemsFound;
    CSnapshotArray*         iSnapshotArray; // not owned
    
    MMediaManagerObserver*  iObserver;
    const TKeyArrayFix      iKey;
    MMPXCollectionHelper*   iCollectionHelper;
    
    CMD5&                   iHasher;
    
    MMPXHarvesterUtility*   iHarvester;
    CMPXCollectionMediator* iMediator;
    MMPXCollectionUtility*  iCollection;
    RFs&                    iFs;
    
    bool                    iWaitOngoing;
    CActiveSchedulerWait    iWait;
    };


#endif // _CMEDIAMANAGER_H
