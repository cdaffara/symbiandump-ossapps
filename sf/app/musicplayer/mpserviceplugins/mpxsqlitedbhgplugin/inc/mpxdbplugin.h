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
* Description:  Implementation of collection Database Plugin interface
*
*/


#ifndef MPXDBPLUGIN_H
#define MPXDBPLUGIN_H

// INCLUDES
#include <badesca.h>
#include <mpxcommonframeworkdefs.h>
#include <mpxcollectionframeworkdefs.h>
#include <mpxcollectionplugin.h>
#include <mpxcollectionpath.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessage2.h>

#include "mpxdbactivetask.h"
#include "mpxcollectiondb.hrh"

// FORWARD DECLARATIONS
class CMPXMedia;
class CMPXDbHandler;
class CMPXDrmMediaUtility;
class CMPXMediaArray;
class CMPXResource;
struct TMPXOpenDataBlock;

// CLASS DECLARATION

/**
* CMPXDbPlugin class
* Implements the plugin interface.
*
* @lib MPXDbPlugin.lib
*/
NONSHARABLE_CLASS(CMPXDbPlugin) :
    public CMPXCollectionPlugin,
    public MMPXDbActiveTaskObserver
    {
public: // Constructors and destructor

    /**
    * Two-phased constructor
    * @param aInitParams: initialization parameters
    * @return object constructed
    */
    static CMPXDbPlugin* NewL (TAny* aInitParams);

    /**
    * Destructor
    */
    virtual ~CMPXDbPlugin();

public: // from base classes

    /**
    * From CMPXCollectionPlugin
    * Navigates to the given path. This method is serviced
    * asynchronously and HandleOpenL is called upon completion
    * @param aPath a path
    * @param aAttrs attributes requested
    * @param aFilter filter to apply or NULL if none
    */
    void OpenL(const CMPXCollectionPath& aPath, const TArray<TMPXAttribute>& aAttrs,
        CMPXFilter* aFilter);

    /**
    * From CMPXCollectionPlugin
    * Get the extended properties of the item referred to by the
    * specified collection path.  This method is serviced asynchronously and
    * HandleMediaL is called upon completion
    * @param aPath a collection path referring to the item to retrieve the
    *               extended properties for
    * @param aAttrs array of attributes requested
    * @param aCaps platsec capabilities of client requesting media; plug-in
    *        should also verify its process capabilities (not yet supported)
    * @aParam aSpecs specifications for attributes (not yet supported)
    */
    void MediaL(const CMPXCollectionPath& aPath, const TArray<TMPXAttribute>& aAttrs,
        const TArray<TCapability>& aCaps, CMPXAttributeSpecs* aSpecs);

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
    void CommandL (TMPXCollectionCommand aCmd, TInt aArg = 0);

    /**
    * From CMPXCollectionPlugin
    * Executes the given CMPXCommand command on the collection
    * @param aCmd a command
    */
    void CommandL(CMPXCommand& aCmd);

    /**
    * From CMPXCollectionPlugin
    * Adds an item (song or playlist) to the collection
    * @param aMedia Properties of the item
    */
    void AddL(const CMPXMedia& aMedia);

    /**
    * From CMPXCollectionPlugin
    * Remove an item from the collection database using the given path
    * Note that the selection indices are hidden within the path
    * @param aPath path to the item to be removed
    *
    */
    void RemoveL(const CMPXCollectionPath& aPath);

    /**
    *  Remove item(s) from the collection database using the given media properties
    *  @param aMedia Properties of the item(s) to be removed. May cantain a URI
    *           or metadata. All item(s) matching the specified properties
    *           will be removed.
    */
    void RemoveL(const CMPXMedia& aMedia);

    /**
    *  Sets/updates the specified media for an item in the collection.  Media
    *  is identified by specifying an ID or URI.
    *  @param aMedia new property values for the item
    */
    void SetL(const CMPXMedia& aMedia);

    /**
    * From CMPXCollectionPlugin
    *  Find the items matching the media specifications.  This method is serviced
    *  asynchronously and HandleFindAllL is called upon completion
    *  @param aCriteria Properties of the item that is to be found
    *  @param aAttrs Requested attributes to return
    */
    void FindAllL (const CMPXMedia& aCriteria, const TArray<TMPXAttribute>& aAttrs);

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
    * From CMPXCollectionPlugin
    * Get the list of supported capabilities
    * @return TCollectionCapability, bitmask of supported capabilities
    */
    TCollectionCapability GetCapabilities ();

    /**
    * From MMPXDbActiveTaskObserver
    */
    TBool HandleStepL();

    /**
    * From MMPXDbActiveTaskObserver
    */
    void HandleOperationCompleted(TInt aErr);

private:
    /**
    * Constructor
    */
    CMPXDbPlugin();

    /**
    * Symbian 2nd phase constructor.
    */
    void ConstructL();

    /**
    * Process the OpenL command
    * @param aPath a path
    * @param aAttrs Requested attributes to return
    * @param aEntries (Output parameter) collection items containing media properties
    *                  of all items, returned if opening a group of items
    * @param aFilter CMPXFilter Object
    * @return ETrue if the open is for a song, EFalse otherwise
    */
    TBool DoOpenL(const CMPXCollectionPath& aPath, const TArray<TMPXAttribute>& aAttrs,
        CMPXMedia& aEntries, CMPXFilter* aFilter );

    /**
    * Process the OpenL(EBrowseAll) command
    * @param aPath Path
    * @param aAttrs Requested attributes to return
    * @param aEntries (Output parameter) collection items containing media properties
    *                  of all items, returned if opening a group of items
    * @param aArray (Output parameter) collection items
    * @return ETrue if the open is for a song, EFalse otherwise
    */
    TBool DoOpenBrowseAllL(const CMPXCollectionPath& aPath, const TArray<TMPXAttribute>& aAttrs,
        CMPXMedia& aEntries, CMPXMediaArray* aArray);

    /**
    * Process the OpenL(EBrowseArtist) command
    * @param aPath Path
    * @param aAttrs Requested attributes to return
    * @param aEntries (Output parameter) collection items containing media properties
    *                  of all items, returned if opening a group of items
    * @param aArray (Output parameter) collection items
    * @return ETrue if the open is for a song, EFalse otherwise
    */
    TBool DoOpenBrowseArtistL(const CMPXCollectionPath& aPath,
        const TArray<TMPXAttribute>& aAttrs, CMPXMedia& aEntries, CMPXMediaArray* aArray);

    /**
    * Process the OpenL(EBrowseAlbum) command
    * @param aPath Path
    * @param aAttrs Requested attributes to return
    * @param aEntries (Output parameter) collection items containing media properties
    *                  of all items, returned if opening a group of items
    * @param aArray (Output parameter) collection items
    * @return ETrue if the open is for a song, EFalse otherwise
    */
    TBool DoOpenBrowseAlbumL(const CMPXCollectionPath& aPath, const TArray<TMPXAttribute>& aAttrs,
        CMPXMedia& aEntries, CMPXMediaArray* aArray);

    /**
    * Process the OpenL(EBrowsePlaylist) command
    * @param aPath Path
    * @param aAttrs Requested attributes to return
    * @param aEntries (Output parameter) collection items containing media properties
    *                  of all items, returned if opening a group of items
    * @param aArray (Output parameter) collection items
    * @return ETrue if the open is for a song, EFalse otherwise
    */
    TBool DoOpenBrowsePlaylistL(const CMPXCollectionPath& aPath, const TArray<TMPXAttribute>& aAttrs,
        CMPXMedia& aEntries, CMPXMediaArray* aArray);

    /**
    * Process the OpenL(EBrowseGenre) command
    * @param aPath Path
    * @param aAttrs Requested attributes to return
    * @param aEntries (Output parameter) collection items containing media properties
    *                  of all items, returned if opening a group of items
    * @param aArray (Output parameter) collection items
    * @return ETrue if the open is for a song, EFalse otherwise
    */
    TBool DoOpenBrowseGenreL(const CMPXCollectionPath& aPath, const TArray<TMPXAttribute>& aAttrs,
       CMPXMedia& aEntries, CMPXMediaArray* aArray);

    /**
    * Process the OpenL(EBrowseComposer) command
    * @param aPath Path
    * @param aAttrs Requested attributes to return
    * @param aEntries (Output parameter) collection items containing media properties
    *                  of all items, returned if opening a group of items
    * @param aArray (Output parameter) collection items
    * @return ETrue if the open is for a song, EFalse otherwise
    */
    TBool DoOpenBrowseComposerL(const CMPXCollectionPath& aPath,  const TArray<TMPXAttribute>& aAttrs,
        CMPXMedia& aEntries, CMPXMediaArray* aArray);

    /**
    * Process an OpenL command with a EMPXOpenPlaylistOnly open mode
    * @param aPath path that we want to open
    * @param aAttrs default attributes to fetch
    * @return CMPXCollectionPath* which is the new valid browse path
    */
    CMPXCollectionPath* DoOpenPlaylistL( const CMPXCollectionPath& aPath, const TArray<TMPXAttribute>& aAttrs );

    /**
    * Process the MediaL command
    * @param aPath a path
    * @param aAttrs Requested attributes to return
    * @param aEntries collection items containing media properties of all items, returned if opening a group of items
    */
    void DoMediaL (const CMPXCollectionPath& aPath, const TArray<TMPXAttribute>& aAttrs, CMPXMedia& aEntries);

    /**
    * Get the plugin title, subtitle, icons for this plugin
    * @param aAttrs attributes to find
    * @param aMedia media object
    */
    void DoRootMediaL(const TArray<TMPXAttribute>& aAttrs, CMPXMedia& aMedia);

    /**
    * Get the media under ALL SONGS category
    * @param aPath path that leads to a media under ALL SONGS
    * @param aAttrs attributes of media to find
    * @param aEntries the media referred by the path
    * @param aMediaArray media entries contained in the media referred by the path
    */
    void DoAllSongsMediaL(const CMPXCollectionPath& aPath,
                            const TArray<TMPXAttribute>& aAttrs,
                            CMPXMedia& aEntries,
                            CMPXMediaArray& aMediaArray);

    /**
    * Get the media under ARTISTS category
    * @param aPath path that leads to a media under ARTISTS
    * @param aAttrs attributes of media to find
    * @param aEntries the media referred by the path
    * @param aMediaArray media entries contained in the media referred by the path
    */
    void DoArtistMediaL(const CMPXCollectionPath& aPath,
                          const TArray<TMPXAttribute>& aAttrs,
                          CMPXMedia& aEntries,
                          CMPXMediaArray& aMediaArray);

    /**
    * Get the root media under ALBUMS/PLAYLISTS/GENRES/COMPOSERS
    * @param aAttrs attributes of media to find
    * @param aRootCategoryId Id of the root media in the category, i.e. all albums,
    *        ...etc.
    * @param aCategory specifies the category. Will be either EMPXAlbum, EMPXPlaylist,
    *        EMPXGenre, or EMPXComposer
    * @param aEntries the media referred by the path
    */
    void DoRootCategoryMediaL(const TArray<TMPXAttribute>& aAttrs,
                               TMPXItemId aRootCategoryId,
                               TMPXGeneralCategory aCategory,
                               CMPXMedia& aEntries);

    /**
    * Get the media under album/playlist/genre/composer category
    * @param aPath path that leads to a media under ALBUMS/PLAYLISTS/GENRES/COMPOSERS
    * @param aAttrs attributes of media to find
    * @param aCategory specifies the category. Will be either EMPXAlbum, EMPXPlaylist,
    *        EMPXGenre, or EMPXComposer
    * @param aEntries the media referred by the path
    * @param aMediaArray medias contained in the media referred by the path
    */
    void DoCategoryMediaL(const CMPXCollectionPath& aPath,
                           const TArray<TMPXAttribute>& aAttrs,
                           TMPXGeneralCategory aCategory,
                           CMPXMedia& aEntries,
                           CMPXMediaArray& aMediaArray);

    /**
    * Set all the attributes in CMPXMedia corresponding to KMPXMediaIdDrm
    * @param aMedia, media to update
    * @param aDrmAttributes, bitwise or of the flags we want
    * @param aLocation, full file path including filename
    */
    void DoSetMediaDrmL(CMPXMedia& aMedia, TUint aDrmAttributes, const TDesC& aLocation);

    /**
    * Add media objects to the array with attributes from song details
    * @param aPath a path
    * @param aAttrs requested attributes
    * @param aEntry media to contain information about the container if it contains
    *        than one entry
    * @param aMediaArray array to contain all the media objects added
    */
    void GetSongInfoL(const CMPXCollectionPath& aPath, const TArray<TMPXAttribute>& aAttrs,
        CMPXMedia& aEntry, CMPXMediaArray& aMediaArray);

    /**
    * Add media objects to the array with attributes from song details
    * @param aAttrs requested attributes
    * @param aEntryId song ID
    * @param aPlaylistId corresponding playlist ID, if 0 the attributes will be read from
    *        the songs table, otherwise they will be read from the playlist table.
    * @param aEntry media to contain information about the container if it contains
    *        than one entry
    */
    void DoGetSongInfoL(const TArray<TMPXAttribute>& aAttrs, TInt aEntryId, TInt aPlaylistId,
        CMPXMedia& aEntry);

    /**
    * Get the collection details from the db
    * @param aAttrs, attributes to find
    * @param aMedia, media object
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
    * @param aMedia: Properties of the item that is to be removed. It may cantain URI only
    *                or meta data, all of items matched properties will be removed.
    * @param aDeleteRecord: indicates whether the client has explictly requested
    *                to delete the records from the database. If ETrue, records
    *                associated with the media will be deleted from the database;
    *                if EFalse, whether to mark the records as deleted or to delete
    *                the records will be based on other deciding factors.
    */
    void DoRemoveL(const CMPXMedia& aMedia, TBool aDeleteRecord);

    /**
    * Remove an item from the collection database using a command
    * @param aCmd: remove command which contains collection path to the media(s) to be
    *        removed. This method should be called until all the selected items are
    *        removed. When that happens, EMPXCommandCompleted flag is set to ETrue.
    *        see KMPXCommandIdCollectionRemove in mpxcollectioncommanddefs.h
    */
    void DoRemovePathL(CMPXCommand& aCmd);

    /**
    * Remove a media/medias from the collection database using a command
    * @param aCmd: remove command which contains the media(s) to be removed. This
    *        will delete the records from the database rather than marking them
    *        as deleted. Designated caller is MTP.
    */
    void DoRemoveMediaL(CMPXCommand& aCmd);

    /**
    * Remove a media/medias from all songs view
    * @param aPath, collection path that points to the media items to be removed
    * @param aSelections, multiple media items selected. This array contains the id
    *        of the selected media.
    * @param aUriArray: a descriptor array containing the URI(s) of the deleted file(s)
    * @param aItemChangedMessages: a list of change events as a result of the
    *        song removals
    */
    void DoRemoveFromAllSongsL(const CMPXCollectionPath& aPath,
                               const TArray<TMPXItemId>& aSelections,
                               CDesCArray& aUriArray,
                               CMPXMessageArray& aItemChangedMessages);

    /**
    * Remove a media/medias from artists view
    * @param aPath, collection path that points to the media(s) to be removed
    * @param aSelections, multiple media items selected. This array contains the id
    *        of the selected media.
    * @param aUriArray: a descriptor array to contain the URI of the deleted file
    * @param aItemChangedMessages: a list of change events as a result of the
    *        song removals
    */
    void DoRemoveFromArtistsL(const CMPXCollectionPath& aPath,
                              const TArray<TMPXItemId>& aSelections,
                              CDesCArray& aUriArray,
                              CMPXMessageArray& aItemChangedMessages);

    /**
    * Remove a media/medias from artists view
    * @param aPath, collection path that points to the media(s) to be removed
    * @param aItemId media item to be removed.
    * @param aUriArray: a descriptor array to contain the URI of the deleted file
    * @param aItemChangedMessages: a list of change events as a result of the
    *        song removals
    */
    void RemoveFromArtistsL(const CMPXCollectionPath& aPath,
                                TUint32 aItemId,
                                CDesCArray& aUriArray,
                                CMPXMessageArray& aItemChangedMessages);

    /**
    * Remove a media entry/entries from albums/genres/composers view
    * @param aPath, collection path that points to the media entry/entries to be removed
    * @param aSelections, multiple media entries selected. This array contains the id
    *        of the selected media.
    * @param aCategory, indicates one of the 3 categories, EMPXAlbum, EMPXGenre, or
    *        EMPXComposer
    * @param aUriArray: a descriptor array to contain the URI(s) of the deleted file(s)
    * @param aItemChangedMessages: a list of change events as a result of the
    *        song removals
    */
    void DoRemoveFromCategoriesL(const CMPXCollectionPath& aPath,
                                 const TArray<TMPXItemId>& aSelections,
                                 TMPXGeneralCategory aCategory,
                                 CDesCArray& aUriArray,
                                 CMPXMessageArray& aItemChangedMessages);

    /**
    * Remove a media entry from albums/genres/composers view
    * @param aPath, collection path that points to the media entry/entries to be removed
    * @param aItemId item ID to be removed.
    * @param aCategory, indicates one of the 3 categories, EMPXAlbum, EMPXGenre, or
    *        EMPXComposer
    * @param aUriArray: a descriptor array to contain the URI(s) of the deleted file(s)
    * @param aItemChangedMessages: a list of change events as a result of the
    *        song removals
    */
    void RemoveFromCategoriesL(const CMPXCollectionPath& aPath,
                                TUint32 aItemId,
                                TMPXGeneralCategory aCategory,
                                CDesCArray& aUriArray,
                                CMPXMessageArray& aItemChangedMessages);

    /**
    * Remove a media entry/entries from playlists view
    * @param aPath, collection path that points to the media(s) to be removed
    * @param aSelections, multiple medias selected. This array contains the id
    *        of the selected media.
    * @param aUriArray: a descriptor array to contain the URI of the deleted file
    * @param aItemChangedMessages: a list of change events as a result of the
    *        song removals
    */
    void DoRemoveFromPlaylistsL(const CMPXCollectionPath& aPath,
                                const TArray<TMPXItemId>& aSelections,
                                CDesCArray& aUriArray,
                                CMPXMessageArray& aItemChangedMessages);

    /**
    * Remove a media entry/entries from playlists view
    * @param aPath, collection path that points to the media(s) to be removed
    * @param aItemId item to be removed
    * @param aIndex item index to be removed
    * @param aUriArray: a descriptor array to contain the URI of the deleted file
    * @param aItemChangedMessages: a list of change events as a result of the
    *        song removals
    */
    void RemoveFromPlaylistsL(const CMPXCollectionPath& aPath,
                                const TMPXItemId& aItemId,
                                TInt aIndex,
                                CDesCArray& aUriArray,
                                CMPXMessageArray& aItemChangedMessages);

    /**
    * Retrieves URIs associated with the path for file deletion
    * @param aCmd, command to retrieve associated URIs for the given path. If
    *        path ends with a particular artist/album/genre/composer, the path
    *        will be updated to include all songs in that category. If path
    *        ends with one or more medias in a particular playlist, an array
    *        of 0 elements will be returned to avoid client deleting them from
    *        the file system. see KMPXCommandIdCollectionRetrieveUriForDeletion
    *        defined in mpxcollectioncommanddefs.h
    */
    void DoRetrieveUriForDeletionL(CMPXCommand& aCmd);

    /**
    * Deletes records which have been marked as deleted from the collection
    * @param aCmd command to cleanup deleted medias.
    *        see KMPXCommandIdCollectionCleanupDeletedRecords defined in
    *        mpxcollectioncommanddefs.h
    */
    void CleanupDeletedRecordsL(CMPXCommand& aCmd);

    /**
    * Fetch the duration for a specific category
    * @param aMedia a media object of which EMPXMediaGeneralDuration is to be
    *        populated with the duration calculated in this method
    * @param aFirstCat category of the 1st Id
    * @param aId id for identifying which category to calculate the duration for
    * @param aSecondCat category of the 2nd Id
    * @param aSubId 2nd id for identifying which category to calculate the
    *        duration for. For example, to calculate the duration of an album of
    *        a particular artist, 1st Id provided will be the artist Id and the
    *        2nd Id provided will be the album Id.
    */
    TInt DoDurationL(CMPXMedia& aMedia, TMPXGeneralCategory aFirstCat, TMPXItemId aId = 0,
                      TMPXGeneralCategory aSecondCat = EMPXNoCategory, TMPXItemId aSubId = 0);

    /**
    * Append a collection level to the collection path
    * @param aPath, path to append the level to
    * @param aMedia, media to extract media ids from
    * @note: Selection will be put on the first song in the collection path
    */
    TInt DoAppendLevelL(CMPXCollectionPath& aPath, CMPXMedia& aMedia);

    /**
    * Append a collection level to the collection path with all songs under the
    * selected category/categories (e.g. artist/artist-album/album/genre/composer).
    * Once the level is appended, all songs under the selected category/categories
    * are selected.
    * @param aPath, path to append the level to. Path remains unchanged if it's not
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
    */
    TUint32 DoAddItemL(const CMPXMedia& aMedia, CMPXMessageArray& aMessageArray,
                        TBool aBatchCommit = EFalse);

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
    * Set an item media to update the collection
    * @param aMedia media to update
    * @param aMessageArray array to append change messages
    * @return ETrue if the update is UI visible, EFalse otherwise
    */
    CMPXDbActiveTask::TChangeVisibility DoSetItemL(const CMPXMedia& aMedia, CMPXMessageArray& aMessageArray);

    /**
    * Set the drm properties for an array of medias
    * @param mediaArray CMPXMediaArray list of medias
    * @param aAtts attribute list
    */
    void DoSetDrmForArrayL(const CMPXMediaArray& mediaArray,
                            const TArray<TMPXAttribute>& aAttrs);

    /**
    * Send a change message
    *
    * @param aMessage: a change message to be sent
    */
    void HandleChangeL(const CMPXMessage& aMessage);

    /**
    * Construct a CMPXMedia from the array parameter and call HandleChange
    *
    * @param aItemChangedMessages: array in the change message to be sent
    * @param aCommandid command that was executed
    */
    void DoHandleChangeL(CMPXMessageArray* aItemChangedMessages,
                         TMPXCommandId aCommandId = 0 );

    /**
    * Returns the category for a given browse type.
    *
    * @param aBrowseType browse type
    * @return Category ID
    */
    TMPXGeneralCategory CategoryForBrowseType(TMCBrowseType aBrowseType);

    /**
    * Returns the browse type for a given category ID.
    *
    * @param aCategory category ID
    * @return Browse type
    */
    TMCBrowseType BrowseTypeForCategory(TMPXGeneralCategory aCategory);

    /**
    * Sets the type, category and title attributes in the specified media.
    *
    * @param aMedia media instance to be updated
    * @param aType type attribute value
    * @param aCategory category attribute value
    * @param aTitle title attribute value
    */
    void SetMediaGeneralAttributesL(CMPXMedia& aMedia, TMPXGeneralType aType,
        TMPXGeneralCategory aCategory, const TDesC& aTitle);

    /**
    * Sets the type, category and title attributes in the specified media.
    *
    * @param aMedia media instance to be updated
    * @param aType type attribute value
    * @param aCategory category attribute value
    * @param aId ID of the item to the title from
    */
    void SetMediaGeneralAttributesL(CMPXMedia& aMedia, TMPXGeneralType aType,
        TMPXGeneralCategory aCategory, TInt aId);

    /**
    * Leaving function to handle async op completion
    * @param aErr error code
    */
    void DoHandleOperationCompletedL(TInt aErr);

    /**
    * Complete a delete operation
    * Sends out change messages and compacts the database
    */
    void DoHandleDeleteCompleteL( CMPXCommand& aCmd );

    /**
    * Reorder a song in a playlist
    * @param aCmd command to reoder a song in a playlist
    *        see KMPXCommandIdReorderPlaylist defined in
    *        mpxcollectioncommanddefs.h
    */
    void DoReorderPlaylistL( const CMPXCommand& aCmd );

    /**
    * Get total song and playlist count
    * @param aCmd command to return count
    *        see KMPXCommandCollectionGetCount defined in
    *        mpxcollectioncommanddefs.h
    */
    void DoGetCollectionCountL( const CMPXCommand& aCmd );

    /**
    * Get an array of URIs for all songs and playlists in a given db
    * @param aCmd command to return CDesCArray
    *        see KMPXCommandCollectionGetURIs defined in
    *        mpxcollectioncommanddefs.h
    */
    void DoGetCollectionUriL( const CMPXCommand& aCmd );

    /**
    * Perform 1 step of the incremental read operation
    * @param aCmd Command to process
    */
    void DoIncrementalOpenL( const CMPXCommand& aCmd );

    /**
    * Open the all songs view incrementally
    * @param aPath path that leads to a media under ALL SONGS
    * @param aAttrs attributes of media to find
    * @param aEntries the media referred by the path
    * @param aMediaArray media entries contained in the media referred by the path
    */
    TBool DoOpenIncrementalL( const CMPXCollectionPath& aPath,  const TArray<TMPXAttribute>& aAttrs,
                              CMPXMedia& aEntries, CMPXMediaArray* aArray);

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
    * @param aMessage: message(s) to be printed
    */
    void PrintMessagesL(const CMPXMessage& aMessage);

    /**
    * Prints a single message for debugging
    * @param aMessage: a message to be printed
    */
    void PrintMessage(const CMPXMessage& aMessage);
#endif // _DEBUG

private:
    // owned member variables
    RFs                  iFs;
    CMPXResource*        iResource;
    CMPXDbHandler*       iDbHandler;
    CMPXDrmMediaUtility* iDrmMediaUtility;
    CDesCArrayFlat*      iMusicLibraryMenuTitles;      // titles for each of the menu items
    CDesCArrayFlat*      iMusicLibraryTitles;          // titles for each of the category views
    RArray<TInt>         iMusicLibraryMenuIds;         // music library item menu IDs
    HBufC*               iAllSongsForArtistTitle;
    HBufC*               iMusicMenuTitle;
    TBool                iDisablePodcasting;
    TBool                iMtpInUse;
    TBool                iRefreshing;
    CMPXDbActiveTask*    iActiveTask;
    RArray<TMPXItemId>   iSelections;   //selected items in the path
    TBool iFirstDeleteStep;
    TBool iAllSongsValid;    //to indicate if there's enough space to get all songs information from collection DB.
    };

#endif   // MPXDBPLUGIN_H


