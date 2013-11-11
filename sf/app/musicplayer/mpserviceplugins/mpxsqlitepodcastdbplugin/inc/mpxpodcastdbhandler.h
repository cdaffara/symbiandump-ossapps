/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class provides the functions to manipulate the podcast collection
*                database.
*
*/


#ifndef MPXPODCASTDBHANDLER_H
#define MPXPODCASTDBHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>

#include <mpxattribute.h>
#include <mpxmediageneraldefs.h>
#include <mpxpodcastdefs.h>

#include "mpxdbcommonstd.h"

#include "mpxpodcastcollectiondbstd.h"
#include "mpxpodcastcollectiondb.hrh"
#include "mpxdbpodcast.h"       // For MMPXDbPodcastObserver
#include "mpxdbactivetask.h"

// FORWARD DECLARATIONS
class CMPXMedia;
class CMPXMediaArray;

class CMPXPodcastDbManager;
class CMPXDbCategory;
class CMPXDbAuxiliary;

// CLASS DECLARATION

/**
*  Podcast collection database handler
*
*  @lib mpxpodcastdbplugin.lib
*/
class CMPXPodcastDbHandler :
    public CBase,
    public MMPXDbPodcastObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aFs file server session
        * @return New CMPXPodcastDbHandler instance
        */
        static CMPXPodcastDbHandler* NewL(RFs& aFs, CMPXResource& aResource);

        /**
        * Two-phased constructor.
        * @param aFs file server session
        * @return New CMPXPodcastDbHandler instance
        */
        static CMPXPodcastDbHandler* NewLC(RFs& aFs, CMPXResource& aResource);

        /**
        * Destructor.
        */
        virtual ~CMPXPodcastDbHandler();

    public:

        /**
        * Add a episode to the podcast collection database
        * @param aMedia media object which contains file path,
        *                  author, name(title), episode no., etc...
        * @return TUint32 id of the item added
        */
        TUint32 AddEpisodeL(const CMPXMedia& aMedia);

        /**
        * Update episode info for a episode in the podcast collection database
        * @param aMedia episode details object  which contains file path,
        *                  author, name(title), episode no., etc...
        * @param aMessageArray resulted change events
        * @return ETrue if the change is UI visible
        */
        CMPXDbActiveTask::TChangeVisibility UpdateEpisodeL(const CMPXMedia& aMedia, CMPXMessageArray& aMessageArray);

        /**
        * Remove all episodes within podcast collection database
        */
        void RemoveEntireCollectionL();

        /**
        * Remove a episode from the podcast collection database
        * @param aEpisodeId ID of the episode to be removed
        * @param aUriArray a descriptor array to contain the URI of the deleted file
        * @param aMessageArray a list of change events as a result of the
        *                             episode removal
        * @param aDeleteRecord indicates whether the client has explictly requested
        *                to delete the records from the database. If ETrue, records
        *                associated with the media will be deleted from the database;
        *                if EFalse, whether to mark the records as deleted or to delete
        *                the records will be based on other deciding factors.
        */
        void RemoveEpisodeL(TUint32 aEpisodeId, CDesCArray& aUriArray,
            CMPXMessageArray& aMessageArray, TBool aDeleteRecord = EFalse);

        /**
        * Removes a category of episodes from the podcast collection, and its
        * corresponding category in the lookup table
        * @param aCategory the category to remove
        * @param aCategoryId ID of the category to remove
        * @param aUriArray a descriptor array to contain the URI of the deleted file(s)
        * @param aMessageArray change events as result of the episode removal
        */
        void RemoveEpisodesMatchingCategoryL(TMPXGeneralCategory aCategory,
            TUint32 aCategoryId, CDesCArray& aUriArray, CMPXMessageArray& aMessageArray);

        /**
        * Removes a publish date category of episodes from the podcast collection
        * @param aCategoryId the publish date cateogry to remove
        * @param aUriArray a descriptor array to contain the URI of the deleted file(s)
        * @param aMessageArray change events as result of the episode removal
        */
        void RemoveEpisodesMatchingPublishDateCategoryL(TUint32 aCategoryId,
            CDesCArray& aUriArray, CMPXMessageArray& aMessageArray);

        /**
        * Delete records in all databases which have been marked as deleted
        */
        void CleanupDeletedRecordsL();

        /**
        * Get all episodes from the podcast collection databasee
        * @param aAttrs Array of attributes to @return
        * @param aMediaArray Array to place all the episodes' required info
        */
        void GetAllEpisodesL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Get all the episodes that match the given title ID
        * @param aTitleId ID of the title to match
        * @param aAttrs Array of attributes to return
        * @param aMediaArray Array to place all the matching episodes' required info
        */
        void GetEpisodesMatchingTitleL(TUint aTitleId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Get all the episodes that match the given playlist ID.
        * Podcast collection only support Recently Added and Not Yet Played
        * auto playlists.
        * @param aPlaylistId ID of the playlist to match
        * @param aAttrs Array of attributes to return
        * @param aMediaArray Array to place all the required episode info
        * @param aIndexOfCurrentlyPlayingItem index of the currently playing
        *        episode or KErrNotFound (-1) if no currently playing episode within
        *        this playlist
        */
        void GetEpisodesMatchingPlaylistL(TUint aPlaylistId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray,
            TInt& aIndexOfCurrentlyPlayingItem);

        /**
        * Get the episode info that matches the given episode ID
        * @param aEpisodeId ID of the episode
        * @param aAttrs Array of attributes to return
        * @param aMediaArray Array to place all the required episode info
        */
        void GetEpisodeL(TUint32 aEpisodeId, const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Get the episode info that matches the given episode ID
        * @aEpisodeId ID of the episode
        * @param aAttrs Array of attributes to return
        * @param aMedia CMPXMedia instance to place all the required song info
        */
        void GetEpisodeL(TUint32 aEpisodeId, const TArray<TMPXAttribute>& aAttrs,
            CMPXMedia& aMedia);

        /**
        * Get the episode ID of the episode that matches the given URI
        * @param aUri URI to match
        * @return Episode ID
        */
        TUint32 GetEpisodeIdMatchingUriL(const TDesC& aUri);

        /**
        * Get all the podcast title names from the podcast collection database
        * @param aAttrs Array of attributes to return
        * @param aMediaArray Array to place all the titles required info
        */
        void GetAllPodcastTitlesL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Get all episodes matching the specified publish date
        * @param aPlaylistId the publish date playlist to retrieve
        * @param aAttrs the attributes to return
        * @param aPendingCategoryDeletionFlag true if the episodes within a publish
        *        date category are being retrieved to be deleted
        * @param aMediaArray Array to place all the episode info
        */
        TInt GetEpisodesMatchingPublishPlaylistL(TUint aPlaylistId,
            const TArray<TMPXAttribute>& aAttrs, TBool aPendingCategoryDeletionFlag,
            CMPXMediaArray& aMediaArray);

        /**
        * Get title name using the specified ID
        * @param aId the id of the title to retrieve the name of
        * @return the podcast title matching the specified ID, ownership is transferred
        */
        HBufC* GetTitleNameMatchingIdL(const TUint32 aId);

        /**
        * Get URI from the Id
        * @param aId to search for
        * @return URI matching the ID, ownership is transferred
        */
        HBufC* GetUriMatchingIdL (const TUint32 aId);

        /**
        * Get the duration for all episodes
        * @return duration value
        */
        TInt GetAllEpisodesDurationL();

        /**
        * Get the duration for a specified title
        * @param aTitleId identifies the title
        * @return duration value
        */
        TInt GetTitleDurationL(TUint32 aTitleId);

        /**
        * Get the duration for the episodes not yet played
        * @return duration value
        */
        TInt GetNotPlayedDurationL();

        /**
        * Get the duration for recently added episodes
        * @return duration value
        */
        TInt GetRecentlyAddedDurationL();

        /**
        * Fetch the total number of items in the specified category
        * @param aCategory category to be queried
        * @return number of items.
        */
        TInt NumberOfItemsL(TMPXPodcastCategory aCategory);

        /**
        * Find media(s) from the collection
        * @param aCriteria selection criteria
        * @param aAttrs attributes for the matching media
        * @return matching media. Ownership is transferred.
        */
        CMPXMedia* FindAllLC(const CMPXMedia& aCriteria,
            const TArray<TMPXAttribute>& aAttrs);

        /**
        * Set the last refreshed time
        * @param aTime the updated last refresh time
        */
        void SetLastRefreshedTimeL( TTime aTime );

        /**
        * Get the last refreshed time
        * @return last refreshed time
        */
        TTime GetLastRefreshedTimeL();

        /**
        * Set the db corrupted bit in the aux table for all drives
        * @param aCorrupted flag indicating whether or not the db is corrupted
        */
        void SetDBCorruptedL(TBool aCorrupted);

        /**
        * Get the db corrupted flag
        * @return ETrue if database is corrupted, else EFalse
        */
        TBool IsDBCorruptedL();

        /**
        * Checks if the database has been created
        * @return ETrue if file has been created
        */
        TBool DatabaseCreated();

        /**
        * Close and re-open the podcast collection database stored on the specified drive
        * @param aDrive drive containing the podcast collection database to re-open
        */
        void OpenDatabaseL(TInt aDrive);

        /**
        * Close a database for a particular drive
        * @param aDrive drive to close
        */
        void CloseDatabaseL(TInt aDrive);

        /**
        * Recreate all podcast databases
        */
        void ReCreateDatabasesL();

        /**
        * Start a refresh operation
        */
        void RefreshStartL();

        /**
        * End a refresh operation
        */
        void RefreshEndL();

        /**
        * Sets the value of the IsPlaying field for a given episode
        * @param aMedia contains episode information
        * @param aIsPlaying value of the IsPlaying field
        */
        void SetIsPlayingL(const CMPXMedia& aMedia, TBool aIsPlaying);

        /**
        * Get total record count for podcasts in db
        * @param aDrive, specifies drive number for db selection
        * @return count
        */
        TUint GetTotalCountL(TInt aDrive);

        /**
        * Get URIs for all podcasts
        * @param aDrive, specifies drive number for db selection
        * @param aFromID, specifies last record's unique ID
        * @param aRecords, specifies the number of records to get
        * @param aUriArr, will contain all URIs on return
        * @param aLastID, will contain a unique ID of last record in the array
        */
        void GetPodcastUriArrayL(TInt aDrive, TInt aFromID, TInt aRecords,
                                 CDesCArray& aUriArr, TInt& aLastID);

        /**
        * Starts a transaction
        */
        void BeginTransactionL();

        /**
        * Ends a transaction. Depending on the error code either commits or
        * rollsback the transaction.
        * @param aError error code
        */
        void EndTransactionL(TInt aError);

        /**
        * Checks if the database is currently in a transaction.
        * @return ETrue if database is currently in a transaction, EFalse otherwise
        */
        TBool InTransaction();
        
        /**
         * Checks if the spefified drive is a remove drive
         */      
        TBool IsRemoteDrive(TDriveNumber aDrive);

    private:

        /**
        * Find media entry/entries from the collection
        * @param aCriteria selection criteria
        * @param aAttrs attributes for the matching media
        * @param aMediaArray array of matching media entry/entries
        */
        void FindAllL(const CMPXMedia& aCriteria, const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Find episode(s) which match(es) the selection criteria
        * @param aCriteria selection criteria
        * @param aAttrs the attributes to include for the artist(s) matching the
        *        selection criteria
        * @param aMediaArray artist(s) matching the selection criteria
        */
        void FindEpisodesL(const CMPXMedia& aCriteria, const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Add an episode to the podcast collection database
        * @param aMedia media object which contains file path, author, name, podcast title etc...
        * @return episode Id of the episode that has been added to the database
        */
        TUint32 DoAddEpisodeL(const CMPXMedia& aMedia);

        /**
        * Update episode info for a episode in the podcast collection database
        * @param aMedia episode details object  which contains file path,
        *               author, name(title), podcast title, etc...
        * @param aItemChangedMessages a list of items changed
        * @return ETrue if UI visible change
        */
        CMPXDbActiveTask::TChangeVisibility DoUpdateEpisodeL(const CMPXMedia& aMedia,
            CMPXMessageArray& aItemChangedMessages);

        /**
        * Remove a episode from the podcast collection database
        * @param aEpisodeId ID of the episode to be removed
        * @param aUriArray a descriptor array to contain the URI of the deleted file
        * @param aMessageArray change events as result of the episode removal
        * @param aDeleteRecord if ETrue the episode will be deleted, if EFalse
        *           it will be marked as deleted
        */
        void DoRemoveEpisodeL(TUint32 aEpisodeId, CDesCArray& aUriArray,
            CMPXMessageArray& aMessageArray, TBool aDeleteRecord);

        /**
        * Remove the episodes for a category from the podcast collection database
        * @param aCategory category type
        * @param aCategoryId category item
        * @param aUriArray a descriptor array to contain the URI of the deleted file
        * @param aMessageArray change events as result of the episode removal
        */
        void DoRemoveEpisodesMatchingCategoryL(TMPXGeneralCategory aCategory,
            TUint32 aCategoryId, CDesCArray& aUriArray, CMPXMessageArray& aMessageArray);

        /**
        * Remove the episodes for a publish date category
        * @param aCategoryId identifies the publish date category
        * @param aUriArray a descriptor array to contain the URI of the deleted file
        * @param aMessageArray change events as result of the episode removal
        */
        void DoRemoveEpisodesMatchingPublishDateCategoryL(TUint32 aCategoryId,
            CDesCArray& aUriArray, CMPXMessageArray& aMessageArray);

        /**
        * Delete records in all databases which have been marked as deleted
        */
        void DoCleanupDeletedRecordsL();

        /**
        * Makes sure all the folders specified in the array parameter are created,
        * otherwise it tries to create them. Updates the iDbDrives member variable
        * with the drives corresponding to the folders.
        * @param aFolders contains all the folders to be processed
        */
        void ProcessPodcastFoldersL(const CDesCArray& aFolders);

        /**
        * Returns the CMPXDbCategory instance associated with the specified
        * category type
        * @param aCategory category type
        * @return corresponding CMPXDbCategory instance
        * @leave KErrNotSupported if invalid category
        */
        CMPXDbCategory* DbCategoryL(TMPXGeneralCategory aCategory) const;

        /**
        * Verify the volume id for the databases
        */
        void VerifyVolumeIdL( TInt aDrive );
        void VerifyVolumeIdL();

        /**
         * Checks if there is a drive that has a low disk space
         * @leave KErrDiskFull if the space on one of the drives is below the
         *         critical level
         */
        void CheckDiskSpaceOnDrivesL();

#if defined (__MTP_PROTOCOL_SUPPORT)
        /**
        * Reads the "save deleted records" MTP flag from CenRep and returns
        * the value.
        * @return value of the "save deleted records" MTP flag
        */
        TBool SaveDeletedSongs();
#endif

#ifdef RD_MULTIPLE_DRIVE
        /**
        * Retrieve all visible podcast folder locations
        */
        CDesCArrayFlat* GetPodcastFoldersL();
#endif // RD_MULTIPLE_DRIVE

    private:    // From MMPXDbPodcastObserver

        /**
        * @see MMPXDbPodcastObserver
        */
        TUint32 AddCategoryItemL(TMPXGeneralCategory aCategory, const TDesC& aName,
            TInt aDriveId, CMPXMessageArray* aItemChangedMessages);

        /**
        * @see MMPXDbPodcastObserver
        */
        void DeleteEpisodeForCategoryL(TMPXGeneralCategory aCategory, TUint32 aCategoryId,
            TInt aDriveId, CMPXMessageArray* aItemChangedMessages);

    private:
        /**
        * C++ constructor
        * @param file session
        */
        CMPXPodcastDbHandler(RFs& aFs, CMPXResource& aResource);

        /**
        * Symbian 2nd phase constructor..
        */
        void ConstructL();

    private:    // Data

        // owned member variables
        CMPXPodcastDbManager* iDbManager;
        CMPXDbPodcast* iDbPodcast;
        CMPXDbCategory* iDbArtist;
        CMPXDbCategory* iDbAlbum;
        CMPXDbCategory* iDbGenre;
        CMPXDbCategory* iDbComposer;
        CMPXDbAuxiliary* iDbAuxiliary;

        CDesCArrayFlat* iPodcastPublishDateCat; // podcast publish date category
        RArray<TInt> iPodcastPublishDateIds;    // podcast publish date ID
        RArray<TInt> iDbDrives;

        // not owned member variables
        RFs& iFs;
        CMPXResource& iResource;
        TBool iOutOfDisk;                // Are we in out of disk mode?
    };

#endif      // MPXPODCASTDBHANDLER_H

// End of File
