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
* Description:  This class provides the functions to manipulate the Podcast table
*
*/


#ifndef MPXDBPODCAST_H
#define MPXDBPODCAST_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <e32property.h>
#include <badesca.h>
#include <mpxmediageneraldefs.h>
#include <mpxpodcastdefs.h>
#include <mpxmessage2.h>

#include "mpxdbtable.h"

#include "mpxpodcastcollectiondbstd.h"
#include "mpxdbactivetask.h"
#include "mpxdbcommonstd.h"

// CONSTANTS
const TInt KMaxSystemPlaylistQuery = 20;

// FORWARD DECLARATIONS
class CMPXTableView;
class CMCGenreParser;
class CMPXCollectionPath;

// CLASS DECLARATION

/**
* Observer interface for handling the category ID functionality.
* This is needed because the incoming CMPXMedia parameters in Add and Update methods
* contain the names for the category items and the Podcast table has to use IDs linking
* to the category tables. Using this interface removes the dependency to the category
* class.
*
* Note that this interface is only used when information is written to the Podcast table,
* when information is retrieved the query takes care of also retrieving the names from
* the corresponding category tables so the outgoing CMPXMedia will contain the correct
* information.
*/
class MMPXDbPodcastObserver
    {
    public:
        /**
        * Called when a new category item has to be added to a category table.
        * @param aCategory category type
        * @param aName name string
        * @param aDriveId drive to add the category item to
        * @param aItemChangedMessages NULL or valid changed mesages array to be updated
        * @return the ID of the category item (new or existing)
        */
        virtual TUint32 AddCategoryItemL(TMPXGeneralCategory aCategory, const TDesC& aName,
            TInt aDriveId, CMPXMessageArray* aItemChangedMessages) = 0;

        /**
        * Called when the ID of a category item changed for a Music record, for example when
        * the artist name changed for a song. The implementation is supposed to update the
        * category record and add corresponding item changed messages to the array parameter.
        * @param aCategory category type
        * @param aCategoryId category item ID
        * @param aItemChangedMessages NULL or valid changed mesages array to be updated
        */
        virtual void DeleteEpisodeForCategoryL(TMPXGeneralCategory aCategory,
            TUint32 aCategoryId, TInt aDriveId,
            CMPXMessageArray* aItemChangedMessages) = 0;
    };

/**
*  Podcast table handler
*
*  @lib mpxpodcastdbplugin.lib
*/
class CMPXDbPodcast :
    public CMPXDbTable
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aObserver Observer for podcast notifications
        * @return New CMPXDbPodcast instance
        */
        static CMPXDbPodcast* NewL(CMPXDbManager& aDbManager, CMPXResource& aResource,
            MMPXDbPodcastObserver& aObserver);

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aObserver Observer for podcast notifications
        * @return New CMPXDbPodcast instance
        */
        static CMPXDbPodcast* NewLC(CMPXDbManager& aDbManager, CMPXResource& aResource,
            MMPXDbPodcastObserver& aObserver);

        /**
        * Destructor.
        */
        virtual ~CMPXDbPodcast();

    public: // New functions

        /**
        * Add a episode to the podcast table
        * @param aMedia  media object which contains file path,
        *                artist, composer, name(title), track, etc...
        * @param aDrive  drive to add the episode to
        * @return The unique id of the episode added.
        */
        TUint32 AddEpisodeL(const CMPXMedia& aMedia, TInt aDrive);

        /**
        * Update info for an episode in the Podcast table
        * @param aEpisodeId episode ID
        * @param aMedia media object which contains file path,
        *                artist, composer, name(title), track, etc...
        * @param aItemChangedMessages a list of change events
        * @return ETrue if the change is visible in the UI
        */
        CMPXDbActiveTask::TChangeVisibility UpdateEpisodeL(TUint32 aEpisodeId, const CMPXMedia& aMedia,
            CMPXMessageArray& aItemChangedMessages);

        /**
        * Cleans up the deleted records
        */
        void CleanupL();

        /**
        * Remove an episode from the Podcast table
        * @param aEpisodeId the ID of the episode to be removed
        * @param aUriArray on return contains the URI of the deleted episode
        * @param aItemChangedMessages on return contains changed messages
        * @param IsAdd denotes whether DeleteEpisodeL is called as part of 
        * adding an episode or deleting an episode ,if called as part of adding
        * pass ETrue else if part of deletion pass EFalse.
        * @param aDeleteRecord indicates whether the client has explictly requested
        *                to delete the records from the database. If ETrue, records
        *                associated with the media will be deleted from the database;
        *                if EFalse, the record will be marked as deleted only.
        */
        void DeleteEpisodeL(TUint32 aEpisodeId, CDesCArray& aUriArray,
            CMPXMessageArray& aItemChangedMessages, TBool IsAdd, TBool aDeleteRecord);

        /**
        * Remove episodes matching a category from the Podcast table
        * @param aCategory the category to remove
        * @param aCategoryId ID of the category to remove
        * @param aUriArray on return will be filled with all the paths that were removed
        * @param aItemChangedMessages change events as result of the episode removal
        */
        void DeleteCategoryL(TMPXGeneralCategory aCategory, TUint32 aCategoryId,
            CDesCArray& aUriArray, CMPXMessageArray& aItemChangedMessages);

        /**
        * Remove all episodes from the podcast table that are published today
        * @param aUriArray, on return will be filled with all the paths that were removed
        * @param aItemChangedMessages change events as result of the episode removal
        */
        void DeleteTodayEpisodesL(CDesCArray& aUriArray,
            CMPXMessageArray& aItemChangedMessages);

        /**
        * Remove all episodes from the podcast table that are published yesterday
        * @param aUriArray, on return will be filled with all the paths that were removed
        * @param aItemChangedMessages change events as result of the episode removal
        */
        void DeleteYesterdayEpisodesL(CDesCArray& aUriArray,
            CMPXMessageArray& aItemChangedMessages);

        /**
        * Remove all episodes from the podcast table that are published this week
        * @param aUriArray, on return will be filled with all the paths that were removed
        * @param aItemChangedMessages change events as result of the episode removal
        */
        void DeleteThisWeekEpisodesL(CDesCArray& aUriArray,
            CMPXMessageArray& aItemChangedMessages);

        /**
        * Remove all episodes from the podcast table that are published a week ago
        * @param aUriArray, on return will be filled with all the paths that were removed
        * @param aItemChangedMessages change events as result of the episode removal
        */
        void DeleteLastWeekEpisodesL(CDesCArray& aUriArray,
            CMPXMessageArray& aItemChangedMessages);

        /**
        * Remove all episodes from the podcast table that are published 2 weeks ago
        * @param aUriArray, on return will be filled with all the paths that were removed
        * @param aUpdatedPlaylists playlists updated as result of the episode removal
        * @param aUpdatedCategories, a list of updated categories
        */
        void Delete2WeeksAgoEpisodesL(CDesCArray& aUriArray,
            CMPXMessageArray& aItemChangedMessages);

        /**
        * Remove all episodes from the podcast table that are published 3 weeks ago
        * @param aUriArray, on return will be filled with all the paths that were removed
        * @param aItemChangedMessages change events as result of the episode removal
        */
        void Delete3WeeksAgoEpisodesL(CDesCArray& aUriArray,
            CMPXMessageArray& aItemChangedMessages);

        /**
        * Remove all episodes from the podcast table that are published last month
        * @param aUriArray, on return will be filled with all the paths that were removed
        * @param aUpdatedPlaylists playlists updated as result of the episode removal
        * @param aUpdatedCategories, a list of updated categories
        */
        void DeleteLastMonthEpisodesL(CDesCArray& aUriArray,
            CMPXMessageArray& aItemChangedMessages);

        /**
        * Remove all episodes from the podcast table that are published earlier
        * @param aUriArray, on return will be filled with all the paths that were removed
        * @param aItemChangedMessages change events as result of the episode removal
        */
        void DeleteEarlierEpisodesL(CDesCArray& aUriArray,
            CMPXMessageArray& aItemChangedMessages);

        /**
        * Remove all episodes from the podcast table that do not have a publish date
        * @param aUriArray on return will be filled with all the paths that were removed
        * @param aItemChangedMessages change events as result of the episode removal
        */
        void DeleteUnknownEpisodesL(CDesCArray& aUriArray,
            CMPXMessageArray& aItemChangedMessages);

        /**
        * Get duration for the given episode
        * @param aEpisodeId episode ID of the episode to search for
        * @return Episode duration
        */
        TInt EpisodeDurationL(TUint32 aEpisodeId);

        /**
        * Get duration for all episodes
        * @return Duration value
        */
        TInt AllEpisodesDurationL();

        /**
        * Get duration for all episodes of a specified title
        * @param aTitleId identifies the title
        * @return Duration value
        */
        TInt TitleDurationL(TUint32 aTitleId);

        /**
        * Get duration for not played episodes
        * @return Duration value
        */
        TInt NotPlayedDurationL();

        /**
        * Get duration for recently added episodes
        * @return Duration value
        */
        TInt RecentlyAddedDurationL();

        /**
        * Get the name of the episode that matches the given episode ID
        * @param aEpisodeId ID of the episode
        * @return Episode name, ownership is transferred.
        */
        HBufC* GetNameL(TUint32 aEpisodeId);

        /**
        * Get URI of the episode
        * @param aEpisodeId ID of the episode
        * @return Episode URI, ownership is transferred.
        */
        HBufC* GetUriL(TUint32 aEpisodeId);

        /**
        * Get the drive of the episode
        * @param aEpisodeId ID of the episode
        * @return Episode drive
        */
        TInt GetDriveL(TUint32 aEpisodeId);

        /**
        * Gets information for an episode
        * @param aEpisodeId identifies the episode
        * @param aAttrs attributes to be returned
        * @param aMedia returns the episode info
        */
        void GetEpisodeL(TUint32 aEpisodeId, const TArray<TMPXAttribute>& aAttrs,
            CMPXMedia& aMedia);

        /**
        * Gets all episodes
        * @param aAttrs attributes to be retrieved
        * @param aMediaArray returns the episode info
        */
        void GetAllEpisodesL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Gets the episodes for a category
        * @param aCategory category type
        * @param aCategoryId category item
        * @param aAttrs attributes to be retrieved
        * @param aMediaArray returns the episodes info
        * @param aOrder should the query be ordered, default EFalse
        */
        void GetEpisodesForCategoryL(TMPXGeneralCategory aCategory, TUint32 aCategoryId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray,
            TBool aOrder = EFalse);

        /**
        * Get all the episodes that match the given title ID
        * @param aTitleId ID of the title to match
        * @param aAttrs Array of attributes to return
        * @param aMediaArray Array to place all the matching episodes' required info
        */
        void GetEpisodesMatchingTitleL(TUint aTitleId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Get the episodes from the podcast table with playcount equal to zero
        * @param aAttrs, the attributes to include for the artist(s) matching the
        *        selection criteria
        * @param aMediaArray Array to place all the matching episodes' required info
        * @param aIndexOfCurrentlyPlayingItem index of the currently playing
        *        episode or KErrNotFound(-1) if no currently playing episode within
        *        this playlist
        */
        void GetNotYetPlayedPlaylistEpisodesL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray, TInt& aIndexOfCurrentlyPlayingItem);

        /**
        * Get the episodes from the podcast table that were added within the last 7 days
        * @param aAttrs, the attributes to include for the artist(s) matching the
        *        selection criteria
        * @param aMediaArray Array to place all the matching episodes' required info
        * @param aIndexOfCurrentlyPlayingItem index of the currently playing
        *        episode or KErrNotFound(-1) if no currently playing episode within
        *        this playlist
        */
        void GetRecentlyAddedPlaylistEpisodesL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray, TInt& aIndexOfCurrentlyPlayingItem);

        /**
        * Get the episodes from the podcast table published today
        * @param aPublishDateCategoryId if specified (> 0), record the category id in the
        *        item in the media array
        * @param aAttrs, the attributes to include for the artist(s) matching the
        *        selection criteria
        * @param aMediaArray Array to place all the matching episodes' required info
        */
        void GetTodayEpisodesL(const TMPXItemId& aPublishDateCategoryId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Get the episodes from the podcast table published today
        * @param aPublishDateCategoryId if specified (> 0), record the category id in the
        *        item in the media array
        * @param aAttrs, the attributes to include for the artist(s) matching the
        *        selection criteria
        * @param aMediaArray Array to place all the matching episodes' required info
        */
        void GetYesterdayEpisodesL(const TMPXItemId& aPublishDateCategoryId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Get the episodes from the podcast table published this week
        * @param aPublishDateCategoryId if specified (> 0), record the category id in the
        *        item in the media array
        * @param aAttrs, the attributes to include for the artist(s) matching the
        *        selection criteria
        * @param aMediaArray Array to place all the matching episodes' required info
        */
        void GetThisWeekEpisodesL(const TMPXItemId& aPublishDateCategoryId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Get the episodes from the podcast table published last week.
        * @param aPublishDateCategoryId if specified (> 0), record the category id in the
        *        item in the media array
        * @param aAttrs, the attributes to include for the artist(s) matching the
        *        selection criteria
        * @param aMediaArray Array to place all the matching episodes' required info
        */
        void GetLastWeekEpisodesL(const TMPXItemId& aPublishDateCategoryId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Get the episodes from the podcast table published two weeks ago
        * @param aPublishDateCategoryId if specified (> 0), record the category id in the
        *        item in the media array
        * @param aAttrs, the attributes to include for the artist(s) matching the
        *        selection criteria
        * @param aMediaArray Array to place all the matching episodes' required info
        */
        void Get2WeeksAgoEpisodesL(const TMPXItemId& aPublishDateCategoryId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Get the episodes from the podcast table published three weeks ago
        * @param aPublishDateCategoryId if specified (> 0), record the category id in the
        *        item in the media array
        * @param aAttrs, the attributes to include for the artist(s) matching the
        *        selection criteria
        * @param aMediaArray Array to place all the matching episodes' required info
        */
        void Get3WeeksAgoEpisodesL(const TMPXItemId& aPublishDateCategoryId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Get the episodes from the podcast table published last month
        * @param aPublishDateCategoryId if specified (> 0), record the category id in the
        *        item in the media array
        * @param aAttrs, the attributes to include for the artist(s) matching the
        *        selection criteria
        * @param aMediaArray Array to place all the matching episodes' required info
        */
        void GetLastMonthEpisodesL(const TMPXItemId& aPublishDateCategoryId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Get the episodes from the podcast table published earlier
        * @param aPublishDateCategoryId if specified (> 0), record the category id in the
        *        item in the media array
        * @param aAttrs, the attributes to include for the artist(s) matching the
        *        selection criteria
        * @param aMediaArray Array to place all the matching episodes' required info
        */
        void GetEarlierEpisodesL(const TMPXItemId& aPublishDateCategoryId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Get the episodes from the podcast table without a publish date
        * @param aPublishDateCategoryId if specified (> 0), record the category id in the
        *        item in the media array
        * @param aAttrs, the attributes to include for the artist(s) matching the
        *        selection criteria
        * @param aMediaArray Array to place all the matching episodes' required info
        */
        void GetUnknownEpisodesL(const TMPXItemId& aPublishDateCategoryId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Find the total number of episodes without loading any media objects
        * Generates a simple sql query to open a column quickly to find the count
        * @return number of items
        */
        TInt CountL();

        /**
        * Get the number of episodes from the podcast table published today
        * @return Number of episodes published today.
        */
        TInt GetTodayEpisodesCountL();

        /**
        * Get the number of episodes from the podcast table published today
        * @return Number of episodes published yesterday.
        */
        TInt GetYesterdayEpisodesCountL();

        /**
        * Get the number of episodes from the podcast table published this week
        * @return Number of episodes published this week.
        */
        TInt GetThisWeekEpisodesCountL();

        /**
        * Get the number of episodes from the podcast table published last week.
        * @return Number of episodes published last week.
        */
        TInt GetLastWeekEpisodesCountL();

        /**
        * Get the number of episodes from the podcast table published two weeks ago
        * @return Number of episodes published 2 weeks ago.
        */
        TInt Get2WeeksAgoEpisodesCountL();

        /**
        * Get the number of episodes from the podcast table published three weeks ago
        * @return Number of episodes published 3 weeks ago.
        */
        TInt Get3WeeksAgoEpisodesCountL();

        /**
        * Get the number of episodes from the podcast table published last month
        * @return Number of episodes published last month.
        */
        TInt GetLastMonthEpisodesCountL();

        /**
        * Get the number of episodes from the podcast table published earlieronth.
        */
        TInt GetEarlierEpisodesCountL();

        /**
        * Get the episodes from the podcast table without a publish date
        * @return Number of episodes without a publish date.
        */
        TInt GetUnknownEpisodesCountL();

        /**
        * Find the total number of episodes recently added.
        * @return number of episodes
        */
        TInt GetRecentlyAddedEpisodesCountL();

        /**
        * Find the total number of episodes that have not been played without loading
        * any media objects. Generates a simple sql query to open al column quickly
        * to find the count
        * @return number of items
        */
        TInt GetNotYetPlayedEpisodesCountL();

        /**
        * Find episode(s) which match(es) the selection criteria
        * @param aGeneralId selection criteria (could be episode/album/artist/genre/composer Id)
        * @param aContainerId selection criteria (could be artist/album id)
        * @param aType criteria type (EMPXItem or EMPXGroup)
        * @param aPodcastType podcast criteria type (EMPXPodcastGroup or EMPXPodcastItem)
        * @param aCriteria selection criteria
        * @param aAttrs the attributes to include for the artist(s) matching the
        *        selection criteria
        * @param aMediaArray Returns the episodes matching the selection criteria
        */
        void FindEpisodesL(TUint32 aGeneralId, TUint32 aContainerId, TMPXGeneralType aType,
            TMPXPodcastType aPodcastType, const CMPXMedia& aCriteria,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Sets the value of the IsPlaying field for a given episode
        * @param aEpisodeId identifies the episode
        * @param aIsPlaying IsPlaying field value
        */
        void SetIsPlayingL(TUint32 aEpisodeId, TBool aIsPlaying);

        /**
        * Returns the podcast count for a given drive
        * @param aDrive drive
        * @return the count
        */
        TUint GetDrivePodcastCountL(TInt aDrive);

        /**
        * Get URIs for a given number of podcasts
        * @param aDrive, specifies drive number for db selection
        * @param aFromID, specifies unique ID of last record retrieved
        * @param aRecords, specifies the number of records to get
        * @param aUriArr, will contain all URIs on return
        * @param aLastID, will contain unique ID of a last record in the array
        */
        void GetPodcastUriArrayL(TInt aDrive, TInt aFromID, TInt aRecords,
                                 CDesCArray& aUriArr, TInt& aLastID);

    private:

        /**
        * Adds an episode to the Podcast table
        * @param aEpisodeId identifies the episode
        * @param aMedia contains the episode attributes
        * @param aDrive drive to add the episode to
        * @param aItemChangedMessages item changed messages to update
        */
        TBool DoAddEpisodeL(TUint32 aEpisodeId, const CMPXMedia& aMedia, TInt aDrive,
            CMPXMessageArray* aItemChangedMessages);

        /**
        * Updates an episode in the Podcast table
        * @param aEpisodeId identifies the episode
        * @param aMedia contains the episode attributes
        * @param aItemChangedMessages NULL or item changed messages to update
        */
        CMPXDbActiveTask::TChangeVisibility DoUpdateEpisodeL(TUint32 aEpisodeId, const CMPXMedia& aMedia,
            CMPXMessageArray* aItemChangedMessages);

        /**
        * Updates an episode in the Podcast table
        * @param aEpisodeId identifies the episode
        * @param aMedia contains the episode attributes
        * @param aDrive drive to add the episode to
        * @param aItemChangedMessages item changed messages to update
        * @param aRecordset current episode record in the Podcast table.
        *       Required to detect any field changes.
        */
        CMPXDbActiveTask::TChangeVisibility DoUpdateEpisodeL(TUint32 aEpisodeId, const CMPXMedia& aMedia, TInt aDrive,
            CMPXMessageArray* aItemChangedMessages, RSqlStatement& aRecordset);

        /**
        * Deletes an episode corresponding to the current record in aRecordset
        * @param aRecordset current episode record in the Podcast table.
        *       Required to detect any field changes.
        * @param aUriArray Returns the episode URI
        * @param aItemChangedMessages Returns item changed messages
        * @param aDeleteRecord ETrue if the episode should be deleted, EFalse if it
        *       should just be marked as deleted
        */
        void DeleteRecordsetL(RSqlStatement& aRecordset, CDesCArray& aUriArray,
            CMPXMessageArray& aItemChangedMessages, TBool aDeleteRecord = EFalse);

        /**
        * Remove a episode from the podcast table
        * @param aRecordset episode record to be removed
        * @param aUriArray, on return will be filled with all the paths that were removed
        * @param aItemChangedMessages: change events as result of the episode removal
        * @param IsAdd denotes whether DoDeleteEpisodeL is called as part of 
        * adding an episode or deleting an episode.
        * @param aDeleteRecord: indicates whether the client has explictly requested
        *                to delete the records from the database. If ETrue, records
        *                associated with the media will be deleted from the database;
        *                if EFalse, whether to mark the records as deleted or to delete
        *                the records will be based on other deciding factors.
        * @return Error code.
        */
        void DoDeleteEpisodeL(RSqlStatement& aRecordset, CDesCArray& aUriArray,
            CMPXMessageArray& aItemChangedMessages, TBool IsAdd, TBool aDeleteRecord = EFalse);

        /**
        * Constructs the fields and corresponding values to be written into the Podcast
        * table for update and insert statements.
        * @param aEpisodeId identifies the song
        * @param aMedia contains the new song attributes
        * @param aItemChangedMessages item changed mesages to add to
        * @param aPodcastTable points to the existing record in the Podcast table for
        *       updates or NULL for inserts
        * @param aFields returns the field names to be updated
        * @param aValues returns the field values to be updated
        * @param aDrive drive the song is on
        * @return ETrue if the change is visible in the UI
        */
        CMPXDbActiveTask::TChangeVisibility GeneratePodcastFieldsValuesL(TUint32 aEpisodeId, const CMPXMedia& aMedia,
            CMPXMessageArray* aItemChangedMessages, RSqlStatement* aPodcastTable,
            CDesCArray& aFields, CDesCArray& aValues, TInt aDrive);

        /**
        * Checks if the specified category field (artist/album/genre/composer)
        * will change and notifies the observer if so.
        * @param aCategory identifies the category
        * @param aMedia contains the new record attributes
        * @param aAttribute corresponding attribute in the media parameter
        * @param aOldId previous value of the category field or 0 if new record
        * @param aDriveId drive ID the update is for
        * @param aItemChangedMessages item changed messages to append to
        * @param aItemId returns the category item ID
        * @return ETrue if the field was modified
        */
        TBool UpdateCategoryFieldL(TMPXGeneralCategory aCategory, const CMPXMedia& aMedia,
            const TMPXAttribute& aAttribute, TUint32 aOldId, TInt aDriveId,
            CMPXMessageArray* aItemChangedMessages, TUint32& aItemId);

        /**
        * Update the media  with information found in DB
        * @param aPodcastTable podcast record
        * @param aAttrs, the attributes to include for the artist(s) matching the
        *        selection criteria
        * @param aMedia the episode details object
        */
        void UpdateMediaL(RSqlStatement& aPodcastTable, const TArray<TMPXAttribute>& aAttrs,
            CMPXMedia& aMedia);

        /**
        * Set the media general attributes in media object with its corresponding
        * value from the podcast table
        * @param aPodcastTable podcast record
        * @param aAttrId the required attributes
        * @param aMedia the media object to set
        */
        void UpdateMediaGeneralL(RSqlStatement& aPodcastTable, TUint aAttrId,
            CMPXMedia& aMedia);

        /**
        * Set the media podcast attributes in media object with its corresponding
        * value from the podcast table
        * @param aPodcastTable podcast record
        * @param aAttrId the required attributes
        * @param aMedia the media object to set
        */
        void UpdateMediaMusicL(RSqlStatement& aPodcastTable, TUint aAttrId,
            CMPXMedia& aMedia);

        /**
        * Set the media audio attributes in media object with its corresponding
        * value from the podcast table
        * @param aPodcastTable podcast record
        * @param aAttrId the required attributes
        * @param aMedia the media object to set
        */
        void UpdateMediaAudioL(RSqlStatement& aPodcastTable, TUint aAttrId,
            CMPXMedia& aMedia);

        /**
        * Set the podcast attributes in media object with its corresponding value
        * from the podcast table
        * @param aPodcastTable podcast record
        * @param aAttrId the required attributes
        * @param aMedia the media object to set
        */
        void UpdateMediaPodcastL(RSqlStatement& aPodcastTable, TUint aAttrId,
            CMPXMedia& aMedia);

        /**
        * Get the DRM type of the given file
        * @param aFile: file
        * @return DRM type.
        */
        TMCDrmType DRMTypeL(const TDesC& aFile);

        /**
        * Generate matching columns and matching values for podcast table from the given criteria
        * @param aGeneralId, selection criteria (could be episode/album/artist/genre/composer Id)
        * @param aContainerId, selection criteria (could be artist/album id)
        * @param aType, criteria type (EMPXItem or EMPXGroup)
        * @param aPodcastType, podcast criteria type (EMPXPodcastGroup or EMPXPodcastItem)
        * @param aCriteria, selection criteria
        * @return Newly allocated SQL criteria string on the CS.
        *       Ownership is passed to the caller.
        */
        HBufC* GeneratePodcastMatchingCriteriaLC(TUint32 aGeneralId, TUint32 aContainerId,
            TMPXGeneralType aType, TMPXPodcastType aPodcastType, const CMPXMedia& aCriteria);

        /**
        * Generate publish date category criteria string for the specified
        * publish date category
        * @param aPublishDateCateogry the publish date category to generate
        *        criteria string for
        * @return the generated criteria string
        */
        HBufC* GeneratePublishDateCategoryCriteriaL(const TUint& aPublishDateCategoryID);

        /**
        * Generates a full URI from a Podcast record.
        * @param aRecordset Podcast table record
        * @return Full URI string. The ownership is passed to the caller.
        */
        HBufC* ConstructUriL(RSqlStatement& aRecordset);

        /**
        * Checks if the specified episode exists.
        * @param aId identifies the episode
        * @return ETrue if it exists, EFalse otherwise
        */
        TBool EpisodeExistsL(TUint32 aEpisodeId);

        /**
        * Checks if extra attributes are required. The "standard" attribute set includes:
        * EMPXMediaGeneralId, EMPXMediaGeneralType, EMPXMediaGeneralCategory,
        * EMPXMediaGeneralTitle, EMPXMediaGeneralUri, and EMPXMediaGeneralFlags
        * @param aAttrs attributes to be checked
        * @return ETrue if extra attributes are required
        */
        TBool ExtraFieldsRequired(const TArray<TMPXAttribute>& aAttrs);

        /**
        * Adds the publish date category ID as a MPXMediaPodcastPubDateCategoryId
        * attribute to the last media item in the array. Only does this if
        * the category id is valid (<>0).
        * @param aPublishDateCategoryId category ID to be added
        * @param aMediaArray array containing media items
        */
        void ProcessPublishDateCategoryL(const TMPXItemId& aPublishDateCategoryId,
            CMPXMediaArray& aMediaArray);

        /**
        * Returns the index of the playing item in the arry or KErrNotFound if no
        * item is marked as playing.
        * @param aMediaArray array to be processed
        * @return index of playing item or KErrNotFound
        */
        TInt GetPlayingItemL(CMPXMediaArray& aMediaArray);

    private:    // from MMPXTable

        /**
        * @see MMPXTable
        */
        virtual void CreateTableL(RSqlDatabase& aDatabase, TBool aCorruptTable);

        /**
        * @see MMPXTable
        */
        virtual void DropTableL(RSqlDatabase& aDatabase);

        /**
        * @see MMPXTable
        */
        virtual TBool CheckTableL(RSqlDatabase& aDatabase);

    private:

        /**
        * C++ constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aObserver Observer for podcast notifications
        */
        CMPXDbPodcast(CMPXDbManager& aDbManager,
            MMPXDbPodcastObserver& aObserver);

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL(CMPXResource& aResource);

    private:

        /**
        * Column indexes in the music table
        */
        enum TPodcastColumns
            {
            EPodcastUniqueId = KMPXTableDefaultIndex,
            EPodcastDeleted,
            EPodcastDbFlag,
            EPodcastVolumeId,
            EPodcastIsPlaying,
            EPodcastTitle,
            EPodcastLocation,
            EPodcastPublishDate,
            EPodcastAlbumTrack,
            EPodcastPlayCount,
            EPodcastTimeAdded,
            EPodcastTimePlayed,
            EPodcastDuration,
            EPodcastSync,
            EPodcastModified,
            EPodcastArtist,
            EPodcastAlbum,
            EPodcastGenre,
            EPodcastComposer,
            EPodcastReleaseDate,
            EPodcastRating,
            EPodcastComment,
            EPodcastCopyright,
            EPodcastArt,
            EPodcastUrl,
            EPodcastDRM,
            EPodcastLastPlayPosition,
            EPodcastSampleRate,
            EPodcastBitRate,
            EPodcastNumChannels,
            EPodcastCodec,
            EPodcastMimeType,
            EPodcastArtistName,
            EPodcastAlbumName,
            EPodcastGenreName,
            EPodcastComposerName,
            EPodcastFieldCount
            };

    private:    // Data

        CDesCArrayFlat* iExtensionsDrm;    // file extensions to check DRM type, owned
        MMPXDbPodcastObserver& iObserver;
    };

#endif      // MPXDBPODCAST_H

// End of File
