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
* Description:  Implementation of Podcast Collection Database Plugin interface
*
*/


#ifndef MPXPODCASTDBPLUGIN_H
#define MPXPODCASTDBPLUGIN_H

// INCLUDES
#include <badesca.h>

#include <mpxcommonframeworkdefs.h>
#include <mpxcollectionframeworkdefs.h>
#include <mpxcollectionplugin.h>
#include <mpxcollectionpath.h>
#include <mpxmediageneraldefs.h>
#include <mpxpodcastdefs.h>
#include <mpxmessage2.h>

#include "mpxdbactivetask.h"
#include "mpxpodcastcollectiondb.hrh"

// FORWARD DECLARATIONS
class CMPXMedia;
class CMPXPodcastDbHandler;
class CMPXDrmMediaUtility;
class CMPXMediaArray;
class CMPXResource;

// CONSTANTS

// CLASS DECLARATION

/**
* Plug-in provides access to podcast collection.
*
* @lib mpxpodcastdbplugin.lib
*/
NONSHARABLE_CLASS(CMPXPodcastDbPlugin) :
    public CMPXCollectionPlugin,
    public MMPXDbActiveTaskObserver
    {
public: // Constructors and destructor

    /**
    * Two-phased constructor
    * @param aInitParams: initialization parameters
    * @return object constructed
    */
    static CMPXPodcastDbPlugin* NewL (TAny* aInitParams);

    /**
    * Destructor
    */
    virtual ~CMPXPodcastDbPlugin();

public: // from base clase
    /**
    * From CMPXCollectionPlugin
    * Navigates to the given path.  This method is serviced
    * asynchronously and HandleOpenL is called upon completion
    * @param aPath a path
    * @param aAttrs attributes requested
    * @param aFilter filter to apply or NULL if none
    */
    void OpenL(const CMPXCollectionPath& aPath,
               const TArray<TMPXAttribute>& aAttrs,
               CMPXFilter* aFilter);

    /**
    * From CMPXCollectionPlugin
    * Get the extended properties of the item referred to by the
    * specified collection path.  This method is serviced asynchronously and
    * HandleMediaL is called upon completion
    * @param aPath a collection path referring to the item to retrieve the
    *               extended properties for
    * @param aAttr: array of attributes requested
    * @param aCaps platsec capabilities of client requesting media; plug-in
    *        should also verify its process capabilities (not yet supported)
    * @aParam aSpecs specifications for attributes (not yet supported)
    */
    void MediaL(const CMPXCollectionPath& aPath,
                const TArray<TMPXAttribute>& aAttrs,
                const TArray<TCapability>& aCaps,
                CMPXAttributeSpecs* aSpecs);

    /**
    * From CMPXCollectionPlugin
    * Cancel outstanding request. Note*** CancelRequest is not currently
    * implemented because all requests are processed synchronously
    */
    void CancelRequest();

    /**
    * From CMPXCollectionPlugin
    * Executes the given TMPXCollectionCommand command on the collection
    * @param aCmd a command
    * @param aArg an argument
    */
    void CommandL(TMPXCollectionCommand aCmd, TInt aArg = 0);

    /**
    * From CMPXCollectionPlugin
    * Executes the given CMPXCommand command on the collection
    * @param aCmd a command
    */
    void CommandL(CMPXCommand& aCmd);

    /**
    * From CMPXCollectionPlugin
    * Adds an episode to the collection
    * @param aMedia Properties of the item
    */
    void AddL(const CMPXMedia& aMedia);

    /**
    * From CMPXCollectionPlugin
    * Remove an item(s) from the collection database using the given path
    * Note that the selection indices are hidden within the path
    * @param aPath: path to the item to be removed
    *
    */
    void RemoveL(const CMPXCollectionPath& aPath);

    /**
    *  Remove item(s) from the collection database using the given media properties
    *  @param aMedia Properties of the item(s) to be removed. May cantain a URI
    *                 or metadata. All item(s) matching the specified properties
    *                 will be removed.
    */
    void RemoveL(const CMPXMedia& aMedia);

    /**
    *  Sets/updates the specified media for an item in the collection.  Media
    *  is identified by specifying an ID or URI.
    *  @param aMedia: new property values for the item
    */
    void SetL(const CMPXMedia& aMedia);

    /**
    * From CMPXCollectionPlugin
    *  Find the items matching the media specifications.  This method is serviced
    *  asynchronously and HandleFindAllL is called upon completion
    *  @param aCriteria Properties of the item that is to be found
    *  @param aAttrs Requested attributes to return
    */
    void FindAllL(const CMPXMedia& aCriteria, const TArray<TMPXAttribute>& aAttrs);

    /**
    *  From CMPXCollectionPlugin
    *  Find the items matching the media specifications
    *  @param aCriteria Properties of the item that is to be found
    *  @param aAttrs Requested attributes to return
    *  @return results of the search. Method abandons ownership of results.
    */
    CMPXMedia* FindAllSyncL(const CMPXMedia& aCriteria,
                            const TArray<TMPXAttribute>& aAttrs);

    /**
    * Get the list of supported capabilities
    * @return TCollectionCapability, bitmask of supported capabilities
    */
    TCollectionCapability GetCapabilities();

    /**
    * From MMPXDbActiveTaskObserver
    */
    TBool HandleStepL();

    /**
    * From MMPXDbActiveTaskObserver
    */
    void HandleOperationCompleted( TInt aErr );

private:
    /**
    * Constructor
    */
    CMPXPodcastDbPlugin ();

    /**
    * Symbian 2nd phase constructor.
    */
    void ConstructL ();

    /**
    * Process the OpenL command
    * @param aPath a path
    * @param aAttrs Requested attributes to return
    * @param aEntries (Output parameter) collection items containing media properties
    *                  of all items, returned if opening a group of items
    * @param aFlagToSignalToBePlayed: signals that item is opened to be played if type == episode
    * @return ETrue if the open is for a episode, EFalse otherwise
    */
    TBool DoOpenL(const CMPXCollectionPath& aPath,
                  const TArray<TMPXAttribute>& aAttrs,
                  CMPXMedia& aEntries,
                  TBool aFlagToSignalToBePlayed);

    /**
    * Process the OpenL(EBrowseAll) command
    * @param aPath Path
    * @param aAttrs Requested attributes to return
    * @param aEntries (Output parameter) collection items containing media properties
    *                  of all items, returned if opening a group of items
    * @param aArray (Output parameter) collection items
    * @return ETrue if the open is for an episode, EFalse otherwise
    */
    TBool DoOpenBrowseAllL(const CMPXCollectionPath& aPath, const TArray<TMPXAttribute>& aAttrs,
        CMPXMedia& aEntries, CMPXMediaArray& aArray);

    /**
    * Process the OpenL(EBrowsePubDate) command
    * @param aPath Path
    * @param aAttrs Requested attributes to return
    * @param aEntries (Output parameter) collection items containing media properties
    *                  of all items, returned if opening a group of items
    * @param aArray (Output parameter) collection items
    * @return ETrue if the open is for an episode, EFalse otherwise
    */
    TBool DoOpenBrowsePubDateL(const CMPXCollectionPath& aPath, const TArray<TMPXAttribute>& aAttrs,
        CMPXMedia& aEntries, CMPXMediaArray& aArray);

    /**
    * Process the OpenL(EBrowseTitle) command
    * @param aPath Path
    * @param aAttrs Requested attributes to return
    * @param aEntries (Output parameter) collection items containing media properties
    *                  of all items, returned if opening a group of items
    * @param aArray (Output parameter) collection items
    * @return ETrue if the open is for an episode, EFalse otherwise
    */
    TBool DoOpenBrowseTitleL(const CMPXCollectionPath& aPath, const TArray<TMPXAttribute>& aAttrs,
        CMPXMedia& aEntries, CMPXMediaArray& aArray);

    /**
    * Process the OpenL(EBrowseRecentlyAdded) command
    * @param aPath Path
    * @param aAttrs Requested attributes to return
    * @param aEntries (Output parameter) collection items containing media properties
    *                  of all items, returned if opening a group of items
    * @param aArray (Output parameter) collection items
    * @return ETrue if the open is for an episode, EFalse otherwise
    */
    TBool DoOpenBrowseRecentlyAddedL(const CMPXCollectionPath& aPath,
        const TArray<TMPXAttribute>& aAttrs, CMPXMedia& aEntries, CMPXMediaArray& aArray);

    /**
    * Process the OpenL(EBrowseNotPlayed) command
    * @param aPath Path
    * @param aAttrs Requested attributes to return
    * @param aEntries (Output parameter) collection items containing media properties
    *                  of all items, returned if opening a group of items
    * @param aArray (Output parameter) collection items
    * @return ETrue if the open is for an episode, EFalse otherwise
    */
    TBool DoOpenBrowseNotPlayedL(const CMPXCollectionPath& aPath, const TArray<TMPXAttribute>& aAttrs,
        CMPXMedia& aEntries, CMPXMediaArray& aArray);

   /**
    * Process an OpenL command with a EMPXOpenPlaylistOnly open mode
    * @param aPath path that we want to open
    * @param aAttrs default attributes to fetch
    * @param aFlagToSignalToBePlayed: signals that item is opened to be played if type == episode
    * @return CMPXCollectionPath* which is the new valid browse path
    *                             Method abandons ownership.
    */
    CMPXCollectionPath* DoOpenPlaylistL( const CMPXCollectionPath& aPath,
                                         const TArray<TMPXAttribute>& aAttrs,
                                         TBool aFlagToSignalToBePlayed);

    /**
    * Process the MediaL command
    * @param aPath a path
    * @param aAttrs Requested attributes to return
    * @param aEntries collection items containing media properties
    *                  of all items, returned if opening a group of items
    */
    void DoMediaL(const CMPXCollectionPath& aPath,
                   const TArray<TMPXAttribute>& aAttrs,
                   CMPXMedia& aEntries);

    /**
    * Get the plugin title, subtitle, icons for this plugin
    * @param aAttrs attributes to find
    * @param aMedia media object
    */
    void DoRootMediaL(const TArray<TMPXAttribute>& aAttrs, CMPXMedia& aMedia);

    /**
    * Get the media under ALL EPISODES category
    * @param aPath path that leads to a media under ALL EPISODES
    * @param aAttrs attributes of media to find
    * @param aEntries the media referred by the path
    * @param aMediaArray media entries contained in the media referred by the path
    */
    void DoAllEpisodesMediaL(const CMPXCollectionPath& aPath,
                            const TArray<TMPXAttribute>& aAttrs,
                            CMPXMedia& aEntries,
                            CMPXMediaArray& aMediaArray);

    /**
    * Find the collection media for by publish date category
    * @param aPath path that leads to a media under BY PUBLISH DATE
    * @param aAttrs attributes of media to find
    * @param aEntries the media referred by the path
    * @param aMediaArray media entries contained in the media referred by the path
    */
    void DoByPublishDateMediaL(const CMPXCollectionPath& aPath,
                                  const TArray<TMPXAttribute>& aAttrs,
                                  CMPXMedia& aEntries,
                                  CMPXMediaArray& aMediaArray);

    /**
    * Find the collection media for by titles category
    * @param aPath path that leads to a media under TITLES
    * @param aAttrs attributes of media to find
    * @param aEntries the media referred by the path
    * @param aMediaArray media entries contained in the media referred by the path
    */
    void DoTitlesMediaL(const CMPXCollectionPath& aPath,
                          const TArray<TMPXAttribute>& aAttrs,
                          CMPXMedia& aEntries,
                          CMPXMediaArray& aMediaArray);

    /**
    * Find the collection media for by recently added category
    * @param aPath path that leads to a media under RECENTLY ADDED
    * @param aAttrs attributes of media to find
    * @param aEntries the media referred by the path
    * @param aMediaArray medias contained in the media referred by the path
    */
    void DoRecentlyAddedMediaL(const CMPXCollectionPath& aPath,
                                 const TArray<TMPXAttribute>& aAttrs,
                                 CMPXMedia& aEntries,
                                 CMPXMediaArray& aMediaArray);


    /**
    * Find the collection media for by not yet played category
    * @param aPath path that leads to a media under NOT YET PLAYED
    * @param aAttrs attributes of media to find
    * @param aEntries the media referred by the path
    * @param aMediaArray medias contained in the media referred by the path
    */
    void DoNotYetPlayedMediaL(const CMPXCollectionPath& aPath,
                                const TArray<TMPXAttribute>& aAttrs,
                                CMPXMedia& aEntries,
                                CMPXMediaArray& aMediaArray);

    /**
    * Get the root media.
    * @param aAttrs attributes of media to find
    * @param aRootCategoryId Id of the root media in the category, i.e. all albums,
    *        ...etc.
    * @param aCategory specifies the category.
    * @param aEntries the media referred by the path
    */
    void DoRootCategoryMediaL(const TArray<TMPXAttribute>& aAttrs,
                               TMPXItemId aRootCategoryId,
                               TMPXPodcastCategory aCategory,
                               CMPXMedia& aEntries);

    /**
    * Set all the attributes in CMPXMedia corresponding to KMPXMediaIdDrm
    * @param aMedia, media to update
    * @param aDrmAttributes, bitwise or of the flags we want
    * @param aLocation, full file path including filename
    */
    void DoSetMediaDrmL(CMPXMedia& aMedia, TUint aDrmAttributes, const TDesC& aLocation);

    /**
    * Add media objects to the array with attributes from episode details
    * @param aPath a path
    * @param aAttrs requested attributes
    * @param aEntry media to contain information about the container if it contains
    *        than one entry
    * @param aMediaArray array to contain all the media objects added
    */
    void GetEpisodeInfoL(const CMPXCollectionPath& aPath, const TArray<TMPXAttribute>& aAttrs,
        CMPXMedia& aEntry, CMPXMediaArray& aMediaArray);

    /**
    * Add media objects to the array with attributes from episode details
    * @param aAttrs requested attributes
    * @param aEntryId episode ID
    * @param aEntry media to contain information about the container if it contains
    *        than one entry
    */
    void DoGetEpisodeInfoL(const TArray<TMPXAttribute>& aAttrs, TInt aEntryId, CMPXMedia& aEntry);

    /**
    * Get the collection details from the db
    * @param aAttrs attributes to find
    * @param aPath path to be set if required
    * @param aMedia media object
    */
    void DoHandleOtherMediaAttributesL(const TArray<TMPXAttribute>& aAttrs,
        const CMPXCollectionPath& aPath, CMPXMedia& aMedia);

    /**
    * Remove an item from the collection database using the given path
    * Note that the selection indices are hidden within the path
    * @param aPath path to the item to be removed
    * @param aChangeMsgArray array to store change messages
    * @return an array of files associated with the removed media entries
    */
    CDesCArray* DoRemoveL(const CMPXCollectionPath& aPath,
                           CMPXMessageArray& aChangeMsgArray);

    /**
    * Remove an item/items from the collection database using the given media properties
    * @param aMedia Properties of the item that is to be removed. It may cantain URI only
    *                or meta data, all of items matched properties will be removed.
    * @param aDeleteRecord indicates whether the client has explictly requested
    *                to delete the records from the database. If ETrue, records
    *                associated with the media will be deleted from the database;
    *                if EFalse, whether to mark the records as deleted or to delete
    *                the records will be based on other deciding factors.
    */
    void DoRemoveL(const CMPXMedia& aMedia, TBool aDeleteRecord);

    /**
    * Remove an item from the collection database using a command
    * @param aCmd remove command which contains collection path to the media entries to be
    *        removed. This method should be called until all the selected items are
    *        removed. When that happens, EMPXCommandCompleted flag is set to ETrue.
    */
    void DoRemovePathL(CMPXCommand& aCmd);

    /**
    * Remove a media entry/entries from the collection database using a command
    * @param aCmd remove command which contains the media(s) to be removed. This
    *        will delete the records from the database rather than marking them
    *        as deleted. Designated caller is MTP.
    */
    void DoRemoveMediaL(CMPXCommand& aCmd);

    /**
    * Remove a media entry/entries from all episodes view
    * @param aPath collection path that points to the media entry/entries to be removed
    * @param aSelections multiple media items selected. This array contains the id
    *        of the selected media.
    * @param aUriArray a descriptor array containing the URI(s) of the deleted file(s)
    * @param aItemChangedMessages a list of change events as a result of the
    *        episode removals
    */
    void DoRemoveFromAllEpisodesL(const CMPXCollectionPath& aPath,
                                  const TArray<TMPXItemId>& aSelections,
                                  CDesCArray& aUriArray,
                                  CMPXMessageArray& aItemChangedMessages);

    /**
    * Remove media from Publish Date view
    * @param aPath collection path that points to the media entry/entries to be removed
    * @param aSelections multiple media entry/entries selected. This array contains the id
    *        of the selected media.
    * @param aUriArray a descriptor array to contain the URI(s) of the deleted file(s)
    * @param aItemChangedMessages a list of change events as a result of the
    *        episode removals
    */
    void DoRemoveFromPublishDateL(const CMPXCollectionPath& aPath,
                                  const TArray<TMPXItemId>& aSelections,
                                  CDesCArray& aUriArray,
                                  CMPXMessageArray& aItemChangedMessages);

    /**
    * Remove a media item from Publish Date view
    * @param aPath collection path that points to the media entry/entries to be removed
    * @param aItemId ID of the item to be removed
    * @param aUriArray a descriptor array to contain the URI(s) of the deleted file(s)
    * @param aItemChangedMessages a list of change events as a result of the
    *        episode removals
    */
    void RemoveFromPublishDateL(const CMPXCollectionPath& aPath, TInt aItemId,
        CDesCArray& aUriArray, CMPXMessageArray& aItemChangedMessages);

    /**
    * Remove a media/medias from albums/genres/composers view
    * @param aPath collection path that points to the media(s) to be removed
    * @param aSelections multiple medias selected. This array contains the id
    *        of the selected media.
    * @param aCategory indicates one of the 3 categories, EMPXAlbum, EMPXGenre, or
    *        EMPXComposer
    * @param aUriArray a descriptor array to contain the URI of the deleted file
    * @param aItemChangedMessages a list of change events as a result of the
    *        episode removals
    */
    void DoRemoveFromCategoriesL(const CMPXCollectionPath& aPath,
                                 const TArray<TMPXItemId>& aSelections,
                                 TMPXGeneralCategory aCategory,
                                 CDesCArray& aUriArray,
                                 CMPXMessageArray& aItemChangedMessages);

    /**
    * Remove a media item from albums/genres/composers view
    * @param aPath collection path that points to the media entry/entries to be removed
    * @param aItemId ID of the item to be removed
    * @param aCategory indicates one of the 3 categories, EMPXAlbum, EMPXGenre, or
    *        EMPXComposer
    * @param aUriArray a descriptor array to contain the URI(s) of the deleted file(s)
    * @param aItemChangedMessages a list of change events as a result of the
    *        episode removals
    */
    void RemoveFromCategoriesL(const CMPXCollectionPath& aPath, TInt aItemId,
        TMPXGeneralCategory aCategory, CDesCArray& aUriArray,
        CMPXMessageArray& aItemChangedMessages);

   /**
    * Fetch the duration for a specific category
    * @param aMedia a media object of which EMPXMediaGeneralDuration is to be
    *        populated with the duration calculated in this method
    * @param aCategory category of the ID
    * @param aId id for identifying which category to calculate the duration for
    */
    TInt DoDurationL(CMPXMedia& aMedia, TMPXPodcastCategory aCategory, TMPXItemId aId = 0);

    /**
    * Retrieves URIs associated with the path for file deletion
    * @param aCmd command to retrieve associated URIs for the given path. If
    *        path ends with a particular artist/album/genre/composer, the path
    *        will be updated to include all episodes in that category. If path
    *        ends with one or more medias in a particular playlist, an array
    *        of 0 elements will be returned to avoid client deleting them from
    *        the file system
    */
    void DoRetrieveUriForDeletionL(CMPXCommand& aCmd);

    /**
    * Deletes records which have been marked as deleted from the collection
    * @param aCmd command to cleanup deleted medias.
    *        see KMPXCommandIdCollectionCleanupDeletedMedias defined in
    *        mpxcollectioncommanddefs.h
    */
    void CleanupDeletedRecordsL(CMPXCommand& aCmd);

    /**
    * Append a collection level to the collection path
    * @param aPath path to append the level to
    * @param aMedia media to extract media ids from
    * @note: Selection will be put on the first episode in the collection path
    */
    TInt DoAppendLevelL(CMPXCollectionPath& aPath, CMPXMedia& aMedia);

    /* Append a collection level to the collection path with all episodes under the
    * selected category/categories (e.g. artist/artist-album/album/genre/composer).
    * Once the level is appended, all episodes under the selected category/categories
    * are selected.
    * @param aPath path to append the level to. Path remains unchanged if it's not
    *        one of the following:
    *        1) path ends at an artist or selected artists
    *        2) path ends at an album of an artist or selected albums of an artist
    *        3) path ends at an album or selected albums
    *        4) path ends at a genre or selected genres
    *        5) path ends at a composer or selected composers
    */
    void DoAppendLevelL(CMPXCollectionPath& aPath);

    /**
    * Execute a task step for an asynchronous add
    * @return ETrue if done
    */
    TBool DoAddAsyncL();

    /**
    * Add a media object to the collection
    * @param aMedia media to add
    * @return TUint32 item id of the item added
    */
    TUint32 DoAddL(const CMPXMedia& aMedia);

    /**
    * Add an item into the collection
    * @param aMedia media to add
    * @param aMessageArray change message array
    * @return TUint32 item id of the item added
    */
    TUint32 DoAddItemL(const CMPXMedia& aMedia, CMPXMessageArray& aMessageArray);

    /**
    * Set a media object to update the collection
    * @param aMedia media can be an item or a list. The item is treated as a list
    *               if it contains a media array
    */
    void DoSetL(const CMPXMedia& aMedia);

    /**
    * Execute a task step for an asynchronous set
    */
    TBool DoSetAsyncL();

    /**
    * Send a change message
    * @param aMessage a change message to be sent
    */
    void HandleChangeL(const CMPXMessage& aMessage);

    /**
    * Construct a CMPXMedia from the array parameter and call HandleChange
    * @param aItemChangedMessages array in the change message to be sent
    */
    void DoHandleChangeL(CMPXMessageArray* aItemChangedMessages);

    /**
    * Generates and calls back an out of disk CMPXMessage
    * This function will callback to the client context to
    * broadcast an out of disk event
    */
    void HandleOutOfDiskMessageL();

    /**
    * Leaving function to handle async op completion
    * @param aErr error code
    */
    void DoHandleOperationCompletedL(TInt aErr);

    /**
    * Complete a delete operation
    * Sends out change messages and compacts the database
    */
    void DoHandleDeleteCompleteL(CMPXCommand& aCmd);

    /**
    * Returns the category for a given browse type.
    * @param aBrowseType browse type
    * @return Category ID
    */
    TMPXPodcastCategory CategoryForBrowseType(TMCBrowseType aBrowseType);

    /**
    * Returns the browse type for a given category ID.
    * @param aCategory category ID
    * @return Browse type
    */
    TMCBrowseType BrowseTypeForCategory(TMPXPodcastCategory aCategory);

    /**
    * Sets the type, category and title attributes in the specified media.
    * @param aMedia media instance to be updated
    * @param aGeneralType general type attribute value
    * @param aType podcast type attribute value
    * @param aCategory category attribute value
    * @param aTitle title attribute value
    * @param aCount general count attribute value
    */
    void SetMediaGeneralAttributesL(CMPXMedia& aMedia, TMPXGeneralType aGeneralType,
        TMPXPodcastType aType, TMPXPodcastCategory aCategory, const TDesC& aTitle,
        TInt aCount = -1);

    /**
    * Sets the type, category and title attributes in the specified media.
    * @param aMedia media instance to be updated
    * @param aGeneralType general type attribute value
    * @param aType type attribute value
    * @param aCategory category attribute value
    * @param aId ID of the item to the title from
    * @param aCount general count attribute value
    */
    void SetMediaGeneralAttributesL(CMPXMedia& aMedia, TMPXGeneralType aGeneralType,
        TMPXPodcastType aType, TMPXPodcastCategory aCategory, TInt aId, TInt aCount = -1);

    /**
    * Get total podcasts count
    * @param aCmd command to return count
    *        see KMPXCommandCollectionGetCount defined in
    *        mpxcollectioncommanddefs.h
    */
    void DoGetCollectionCountL( const CMPXCommand& aCmd );

    /**
    * Get an array of URIs for all podcasts in a given db
    * @param aCmd command to return CDesCArray
    *        see KMPXCommandCollectionGetURIs defined in
    *        mpxcollectioncommanddefs.h
    */
    void DoGetCollectionUriL( const CMPXCommand& aCmd );

    /**
    * Set the attribute list according to current path
    * @param aPath a path
    * @param aAttrs attributes array to be filled
    * @param aSupportedIds array of supported content IDs to be filled
    */
    void SetAttributesL( const CMPXCollectionPath& aPath, RArray<TMPXAttribute>& aAttrs,
                         RArray<TInt>& aSupportedIds );
    
#ifdef _DEBUG
    /**
    * Prints messages for debugging
    * @param aMessage message(s) to be printed
    */
    void PrintMessagesL(const CMPXMessage& aMessage);

    /**
    * Prints a single message for debugging
    * @param aMessage a message to be printed
    */
    void PrintMessage(const CMPXMessage& aMessage);
#endif

private:

    RFs                  iFs;
    CMPXResource*        iResource;
    CMPXPodcastDbHandler*iDbHandler;
    CMPXDrmMediaUtility* iDrmMediaUtility;
    CDesCArrayFlat*      iPodcastLibraryTitles;                 // podcast library view titles
    CDesCArrayFlat*      iPodcastLibraryMainMenuItemTitles;     // podcast library item menu item titles
    RArray<TInt>         iPodcastLibraryMainMenuItemIds;        // podcast library item menu IDs
    CDesCArrayFlat*      iPodcastTitlePublishDateCat;           // publish date view menu titles
    RArray<TInt>         iPodcastPublishDateIds;                // publish date view menu IDs
    CDesCArrayFlat*      iPodcastEpisodeViewPublishDateTitle;   // episodes view publish date titles
    RArray<TInt>         iPodcastEpisodeViewPublishDateIds;     // episodes view publish date title IDs

    HBufC*               iTitleMyPodcast;       // podcast menu view title string
    HBufC*               iTitleAllEpisodes;     // All Episodes title string
    HBufC*               iTitlePubDate;         // Published Date title string
    HBufC*               iTitleTitles;          // Titles title string
    HBufC*               iTitleAdded;           // Recently Added title string
    HBufC*               iTitleUnplayed;        // Not Yet played title string

    TInt                 iStateChangedDrive;    // drive number of the media has just opened/close
    TInt                 iStateChangedOldMaster;
    TBool                iStateChangedOpen;
    TBool                iMtpInUse;
    TBool                iRefreshing;

    TInt                 iNumberOfEpisodesInCurrentPublishDateCategory;

    CMPXDbActiveTask*    iActiveTask;
    RArray<TMPXItemId>   iSelections;   //selected items in the path
    TBool iFirstDeleteStep;

    };

#endif   // MPXPODCASTDBPLUGIN_H


