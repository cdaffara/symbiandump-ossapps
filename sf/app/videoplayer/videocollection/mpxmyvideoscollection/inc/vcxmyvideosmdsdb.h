/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   MPX My Videos collection plugins' MDS database class*
*/



#ifndef VCXMYVIDEOSMDSDB_H
#define VCXMYVIDEOSMDSDB_H


#include <mdesession.h>
#include <mdequery.h>
#include <harvesterclient.h>
#include <mpxcollectionmessagedefs.h>
#include <vcxmyvideosdefs.h>
#include <e32property.h>
#include "vcxmyvideosalbum.h"

class CVcxMyVideosMdsAlbums;
class CMPXMedia;
class CVcxMyVideosMdsCmdQueue;

const TUid KHarvesterPSShutdown = { 0x200009F5 } ;
const TInt KMdSShutdown = 0x00000002; // values 1 = shutdown, 0 = restart, normal state

static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
static _LIT_SECURITY_POLICY_C1(KPowerMgmtPolicy,ECapabilityPowerMgmt);

/**
 *  MPX My Videos collection MDS database observer class.
 *  Part of ECOM Plugin.
 */
NONSHARABLE_CLASS(MVcxMyVideosMdsDbObserver)
    {        
    public:
    
        /* 
        
        Defined in mpxcollectionmessagedefs.h:  
        
        enum TMPXChangeEventType
            {
            EMPXItemDeleted,      // item/playlist deleted
            EMPXItemInserted,     // item/playlist added
            EMPXItemModified      // item/playlist/artist/album/genre/composer modified
            };
        */
        
        /**
        * Handler function for database events.
        *
        * @param aEvent       Event type.
        * @param aId          Array of IDs in database.
        * @param aEventsLeft  How many events are still coming.
        */
        virtual void HandleMyVideosDbEvent( TMPXChangeEventType aEvent, RArray<TUint32>& aId, TInt aEventsLeft ) = 0;

        /**
        * Handler function for video list fetching events. This callback is called as a response
        * to CreateVideoList( aSync = EFalse ).
        *
        * @param aVideoList          List of videos fetched from MDS.
        * @param aNewItemsStartIndex Start position for the new items added. If -1, then no new
        *                            items were added.
        * @param aComplete           ETrue if list is complete and no more items are expected,
        *                            EFalse if there are more to come.
        */
        virtual void HandleCreateVideoListResp( CMPXMedia* aVideoList,
                TInt aNewItemsStartIndex, TBool aComplete ) = 0;

        /**
        * Handler function for media remove/insert events. This is called every time
        * media is removed or inserted (objects become present or non present).
        */
        virtual void HandleObjectPresentNotification() = 0;
    };


/**
 *  MPX My Videos collection MDS albums related functionality observer class.
 *  Part of ECOM Plugin.
 */
NONSHARABLE_CLASS(MVcxMyVideosMdsAlbumsObserver)
    {        
    public:
            
        /**
        * This callback is called as a response to GetAlbumsL.
        *
        * @param aAlbumList List of albums fetched from MDS.
        */
        virtual void HandleGetAlbumsResp( CMPXMedia* aAlbumList ) = 0;

        /**
        * This callback is called as a response to GetAlbumContentIdsL.
        *
        * @param aAlbumId      Album ID which was fetched.
        * @param aContentArray Array containing MDS object IDs. This array is the same which
        *                      was given in GetAlbumContentIdsL call.
        */
        virtual void HandleGetAlbumContentIdsResp( TUint32 aAlbumId,
                RArray<TVcxMyVideosAlbumVideo>& aContentArray ) = 0;

        /**
         * Called as a response to GetAlbumContentVideosL.
         * 
         * @param aAlbumId           ID of the album containing the result videos.
         * @param aVideoList         Media containing array of videos.
         * @param aError             Error code in case of failure.
         * @param aFirstNewItemIndex Index of the first new item.
         * @param aNewItemCount      How many new items in the query since the last results.
         * @param aComplete          ETrue if query is complete, EFalse if there is more to come.
         */
        virtual void HandleGetAlbumContentVideosResp( TUint32 aAlbumId, CMPXMedia& aVideoList,
                TInt aError, TInt aFirstNewItemIndex, TInt aNewItemCount, TBool aComplete ) = 0;

        /**
         * Called as a response to AddVideosToAlbumL. Results are written from aItemArray
         * to aCmd (KVcxMediaMyVideosInt32Value attribute). KErrNone if operation was successful,
         * KErrGeneral otherwise.
         * 
         * @param aCmd        Pointer to same object which was given in AddVideosToAlbumL function call.
         * @param aItemArray  Array of relation items that were added to MDS.
         */
        virtual void HandleAddVideosToAlbumResp( CMPXMedia* aCmd,
                RPointerArray<CMdEInstanceItem>& aItemArray ) = 0;

        /**
         * Called as a response to RemoveRelationsL.
         * 
         * @param aRelationIds  Relations which were tried to be removed.
         * @param aResults      Result codes, in sync with aRelationIds. KErrNone if successful,
         *                      KErrGeneral if failed.
         */
        virtual void HandleRemoveRelationsResp( RArray<TUint32>& aRelationIds,
                RArray<TUint32>& aResults ) = 0;

        /**
         * Called as a response to RemoveAlbumsL.
         * 
         * @param aCmd      Cmd which was given in RemoveAlbumsL call.
         * @param aResults  Result IDs.
         */
        virtual void HandleRemoveAlbumsResp( CMPXMedia* aCmd,
                RArray<TUint32>& aResultIds ) = 0;

        /**
         * Event handler function for relation delete events.
         * 
         * @param aType           Event type
         * @param aRelationArray  Array containing relations.
         */
        virtual void HandleRelationEvent( TObserverNotificationType aType,
                const RArray<TMdERelation>& aRelationArray ) = 0;

#if 0
        /**
         * Event handler function for relation add events.
         * 
         * @param aType           Event type
         * @param aRelationArray  Array containing relations.
         */
        virtual void HandleRelationIdEvent( TObserverNotificationType aType,
                const RArray<TUint32>& aRelationIdArray ) = 0;
#endif
        
    };

/**
 * PSCW Listener Observer interface for signaling that MDS has Shutdown/restarted
 */
class MVcxMdsShutdownMonitorObserver
    {
public:

    virtual void ShutdownNotification( TInt aShutdownState ) = 0;
    };

/**
 *  Active object for observing P&S keys
 */
class CVcxMdsShutdownMonitor: public CActive
    {
public:

    /**
     * Two-phased constructor.
     *
     * @return Instance of CVcxMdsShutdownMonitor.
     */
    static CVcxMdsShutdownMonitor* NewL( MVcxMdsShutdownMonitorObserver& aObserver,
            const TUid& aKeyCategory, const TInt aPropertyKey, TBool aDefineKey);

    /**
     * Destructor
     */
    virtual ~CVcxMdsShutdownMonitor();
    
protected:

    /**
     * Handles an active object's request completion event.
     */
    void RunL();

    /**
     * Implements cancellation of an outstanding request.
     */
    void DoCancel();

private:

    /**
     * C++ default constructor
     *
     * @return Instance of CVcxMdsShutdownMonitor.
     */
    CVcxMdsShutdownMonitor( MVcxMdsShutdownMonitorObserver& aObserver,
            const TUid& aKeyCategory, const TInt iPropertyKey, TBool aDefineKey );

    /**
     * Symbian 2nd phase constructor can leave.
     */
    void ConstructL();

private:
    
    // not own
    MVcxMdsShutdownMonitorObserver& iObserver;
    
    const TUid& iKeyCategory;
    RProperty iProperty;
    TInt iPropertyKey;
    
    TBool iDefineKey;
};

/**
 *  MPX My Videos collection ECOM plugin's MDS database class.
 */
NONSHARABLE_CLASS(CVcxMyVideosMdsDb) :
                                public CBase,
                                public MMdESessionObserver,
                                public MMdEQueryObserver,
                                public MMdEObjectObserver,
                                public MMdEObjectPresentObserver,
                                public MVcxMdsShutdownMonitorObserver
    {
        
public:

    friend class CVcxMyVideosMdsAlbums;
    friend class CVcxMyVideosMdsCmdQueue;

    enum TRequestType
        {
        EAll,
        EGetVideoList,
        EGetAlbums,
        EGetAlbumContentIds,
        EGetAlbumContentVideos,
        EAddVideosToAlbum,
        ERemoveRelations,
        ERemoveAlbums
        };
    
    class TEvent
        {
    public:
        
        TUint32 iMdsId;
        TInt    iEventType;
        };
    
    /**
     * Two-phased constructor.
     * @param aObserver The db change observer.
     */
    static CVcxMyVideosMdsDb* NewL( MVcxMyVideosMdsDbObserver* aObserver,
            RFs& aFs, MVcxMyVideosMdsAlbumsObserver* aAlbumsObserver = NULL );

    /**
     * Two-phased constructor.
     * @param aObserver The db change observer.
     */
    static CVcxMyVideosMdsDb* NewLC( MVcxMyVideosMdsDbObserver* aObserver,
            RFs& aFs, MVcxMyVideosMdsAlbumsObserver* aAlbumsObserver = NULL );


    /**
    * Destructor.
    */
    virtual ~CVcxMyVideosMdsDb();

    /**
    * Cancels possible ongoing asynchronous operations.
    * 
    * @param aType What kind of requests are cancelled. 
    */
    void Cancel( TRequestType aType = EAll );

    /**
     * Creates list of videos from MDS. This function utilizes CVcxMyVideosMdsQueue, ie
     * commands are put in queue if there are other commands pending.
     *
     * @param aSortingOrder Sorting order, see values from TSortingOrder.
     * @param aAscending    Sort direction, if ETrue then ascending, else descending.
     * @param aFullDetails  If ETrue, then all details will be filled to media objects,
     *                      if EFalse, then only subset.
     * @param aVideoList    Reference to pointer variable. Container type CMPXMedia class, contains
     *                      CMPXMediaArray filled with CMPXMedia objects. If aVideoList
     *                      pointer variable is NULL, then new medialist is created.
     *                      If pointer variable already contains value, then the given medialist
     *                      should have empty media array. New items are appended to it.
     *                      Caller naturally owns the data.
     *                      
     */
    void CreateVideoListL( TVcxMyVideosSortingOrder aSortingOrder,
            TBool aAscending, TBool aFullDetails, CMPXMedia*& aVideoList );

    /**
     * Add new video.
     *
     * @param aVideo    Reference to CMPXMedia object.
     * @param aMdsId    New mds id is inserted to aMdsId
     */
    void AddVideoL( CMPXMedia& aVideo, TUint32& aMdsId );

    /**
     * Remove video.
     *
     * @param aMdsId ID of the item to be removed.
     * @return       KErrNotFound if video was not found, KErrNone if found and deleted.
     *               If failed, system-wide error code.
     */
    TInt RemoveVideo( TUint32 aMdsId );

    /**
     * Update video.
     *
     * @param aVideo Video to update in MDS database.
     */
    void UpdateVideoL( CMPXMedia& aVideo );

    /**
     * Get one video item from MDS and create a new media
     * object from it.
     *
     * @param aId          Video identifier in MDS database.
     * @param aFullDetails If ETrue, then all details are fetched, othewise only subset.
     * @return             Pointer to media object, ownership moves. NULL if
     *                     not found.
     */
    CMPXMedia* CreateVideoL( TUint32 aId, TBool aFullDetails = ETrue );

    /**
     * Called by iEventProcessor.
     */
    static TInt ProcessEvents( TAny* aPtr );
    
protected:
    
// from MMdESessionObserver

	/**
	 * Called to notify the observer that opening the session has been
	 * completed and, if the opening succeeded, the session is ready for use.
	 *
	 * @param aSession  session
	 * @param aError    <code>KErrNone</code>, if opening the session succeeded
	 *     or one of the system-wide error codes, if opening the session failed
	 */
	void HandleSessionOpened(CMdESession& aSession, TInt aError);

	/**
	 * Called to notify the observer about errors, which are not a direct
	 * consequence of the operations initiated by the client but caused by some
	 * external source (e.g., other clients). The error cannot be recovered and
	 * all on-going operations initiated by the client have been aborted.
	 * Any attempts to continue using the session will cause a panic. The
	 * client should close the session immediately and try to open a new
	 * session if it needs to continue using the metadata engine.
	 *
	 * @param aSession  session
	 * @param aError    one of the system-wide error codes
	 */
	void HandleSessionError(CMdESession& aSession, TInt aError);


//  from MMdEQueryObserver

    /**
     * Called to notify the observer that new results have been received 
     * in the query.
     *
     * @param aQuery              Query instance that received new results.
     * @param aFirstNewItemIndex  Index of the first new item that was added
     *                            to the result item array.
     * @param aNewItemCount       Number of items added to the result item 
     *                            array.
     */
    void HandleQueryNewResults(CMdEQuery& aQuery,
                                       TInt aFirstNewItemIndex,
                                       TInt aNewItemCount);

    /**
     * Called to notify the observer that the query has been completed,
     * or that an error has occured.
     *
     * @param aQuery  Query instance.
     * @param aError  <code>KErrNone</code>, if the query was completed
     *                successfully. Otherwise one of the system-wide error 
     *                codes.
     */
    void HandleQueryCompleted(CMdEQuery& aQuery, TInt aError);


// from MMdEObjectObserver

    void HandleObjectNotification( CMdESession& aSession,
            TObserverNotificationType aType,
            const RArray<TItemId>& aObjectIdArray);

// from MMdEObjectPresentObserver
    void HandleObjectPresentNotification(CMdESession& aSession, 
			TBool aPresent, const RArray<TItemId>& aObjectIdArray);
			
// from MVcxMdsShutdownMonitorObserver
    void ShutdownNotification( TInt aShutdownState );

private:

    /**
     * Constructor.
     * @param aObserver The db change observer.
     * @param aFs      Session to file server.
     */
    CVcxMyVideosMdsDb( MVcxMyVideosMdsDbObserver* aObserver,
            RFs& aFs, MVcxMyVideosMdsAlbumsObserver* aAlbumsObserver = NULL );

    void ConstructL();

    /**
     * Gets object from MDS.
     *
     * @param aId      The ID of the object to get.
     * @param aIsVideo Set to ETrue if the object to be fetched is video. If set to
     *                 EFalse, then the object is assumed to be album.
     * @return         The object if found, NULL otherwise.
     */
    CMdEObject* ObjectL( const TItemId aId, TBool aIsVideo = ETrue );

    /**
     * Read the video details from the given object to the media class.
     *
     * @param aObject       The object to read from.
     * @param aVideo        The media object to write to.
     * @param aFullDetails  If ETrue, then all parameters are copied, if
     *                      EFalse, then only subset which is (usually) needed when
     *                      showing videolist on UI.
     */
    void Object2MediaL(
            CMdEObject& aObject,
            CMPXMedia& aVideo,
            TBool aFullDetails = EFalse );

    /**
     * Fill the object from media.
     *
     * @param aMedia  The media class to read from.
     * @param aObject The object to modify.
     */
    void Media2ObjectL( CMPXMedia& aVideo, CMdEObject& aObject );

    /*
     * Get the schema definitions needed.
     */
    void GetSchemaDefinitionsL();

    /**
    * Handles MDS object changes.
    *
    * @param aType           Type of the event.
    * @param aObjectIdArray  MDS IDs.
    */
    void DoHandleObjectNotificationL(
            TObserverNotificationType aType,
            const RArray<TItemId>& aObjectIdArray );

    /**
     * Leaving version of HandleQueryNewResults.
     * Called to notify the observer that new results have been received 
     * in the query.
     *
     * @param aQuery              Query instance that received new results.
     * @param aFirstNewItemIndex  Index of the first new item that was added
     *                            to the result item array.
     * @param aNewItemCount       Number of items added to the result item 
     *                            array.
     */
    void DoHandleQueryNewResultsL(CMdEQuery& aQuery,
            TInt aFirstNewItemIndex, TInt aNewItemCount);

    /**
     * Creates list of videos from MDS.
     *
     * @param aSortingOrder Sorting order, see values from TSortingOrder.
     * @param aAscending    Sort direction, if ETrue then ascending, else descending.
     * @param aFullDetails  If ETrue, then all details will be filled to media objects,
     *                      if EFalse, then only subset.
     * @param aVideoList    Reference to pointer variable. Container type CMPXMedia class, contains
     *                      CMPXMediaArray filled with CMPXMedia objects. If aVideoList
     *                      pointer variable is NULL, then new medialist is created.
     *                      If pointer variable already contains value, then the given medialist
     *                      should have empty media array. New items are appended to it.
     *                      Caller naturally owns the data.
     *                      
     */
    void DoCreateVideoListL( TVcxMyVideosSortingOrder aSortingOrder,
            TBool aAscending, TBool aFullDetails, CMPXMedia*& aVideoList );

    /**
     * Handles query complete events asynchronously.
     */
    static TInt AsyncHandleQueryCompleted( TAny* aPtr );

    /**
     * Sets Creation Date and Modified properties to object. Uses current date
     * value.
     * 
     * @param aObject  Object to modify.
     */
    void SetCreationAndModifiedDatesL( CMdEObject& aObject );

    /**
     * Sets creation date to aObject from aVideo.
     * 
     * @param aVideo  Creation date is copied from here.
     * @param aObject Creation date is written here.
     */
    void SetCreationDateToObjectL( const CMPXMedia& aVideo, CMdEObject& aObject );

    /**
     * Opens MDS session.
     */
    void OpenMdsSessionL();

    /**
    * @return MDS session.
    */
    CMdESession& MdsSessionL();
    
    /**
     * Called by ProcessEvents.
     */
    TInt DoProcessEvents();
    
public:

    /**
    * Set to ETrue when starting to fetch media list from MDS.
    * Set to EFalse when fetching is finished.
    */
    TBool iVideoListFetchingIsOngoing;

    /**
    * Album related functionality.
    */
    CVcxMyVideosMdsAlbums* iAlbums;
        
private: // data

    /**
    * Command queue. Own.
    */
    CVcxMyVideosMdsCmdQueue* iCmdQueue;

    /**
     * The MDS session object. Own.
     */
    CMdESession* iMdsSession;

    /**
     * The sessions error state.
     */
    TInt iMdsSessionError;

    /**
    * Asynchronous video list fetching query is stored here.
    */
    CMdEObjectQuery* iVideoQuery;
    
    /**
    * Pointer to video list which is being fetched from the MDS.
    * Not own.
    */
    CMPXMedia* iVideoList;
    
    /**
     * Requested detail level is stored here during the asynchronous video list fetching.
     */
    TBool iFullDetails;
    
    /**
    * The default namespace definition. Not own.
    */
    CMdENamespaceDef* iNamespaceDef;

    /**
     * The Video object definition. Not own.
     */
    CMdEObjectDef* iVideoObjectDef;

    /**
     * 2.
     * The Title property definition. Not own.
     */
    CMdEPropertyDef* iTitlePropertyDef;

    /**
     * 3.
     * The Description property definition. Not own.
     */
    CMdEPropertyDef* iDescriptionPropertyDef;

    // 4. URI ( = file path ), set with SetUriL method

    /**
     * 5.
     * The Size property definition. Not own.
     */
    CMdEPropertyDef* iSizePropertyDef;

    /**
     * 6.
     * The Creation Date property definition. Not own.
     */
    CMdEPropertyDef* iCreationDatePropertyDef;
    
    /**
    * Time offset from the universal time. Not own.
    */
    CMdEPropertyDef* iTimeOffsetPropertyDef;

    /**
     * 7.
     * The Flags property definition. Not own.
     */
    CMdEPropertyDef* iFlagsPropertyDef;

    /**
     * 8.
     * The Copyright property definition. Not own.
     */
    CMdEPropertyDef* iCopyrightPropertyDef;

    /**
     * 9.
     * The Item Type property definition. Not own.
     */
    CMdEPropertyDef* iItemTypePropertyDef;

    /**
     * 10.
     * The Modified Date property definition. Not own.
     */
    CMdEPropertyDef* iLastModifiedDatePropertyDef;

    /**
     * 11.
     * The Age Profile property definition. Not own.
     */
    CMdEPropertyDef* iAgeProfilePropertyDef;

    /**
     * 12.
     * The Audio Language property definition. Not own.
     */
    CMdEPropertyDef* iAudioLanguagePropertyDef;

    /**
     * 13.
     * The Author property definition. Not own.
     */
    CMdEPropertyDef* iAuthorPropertyDef;

    /**
     * 14.
     * The Origin property definition. Not own.
     */
    CMdEPropertyDef* iOriginPropertyDef;

    /**
     * 15.
     * The Duration property definition. Not own.
     */
    CMdEPropertyDef* iDurationPropertyDef;

    /**
     * 16.
     * The Last Play Point property definition. Not own.
     */
    CMdEPropertyDef* iLastPlayPositionPropertyDef;
    
    /**
    * 18.
    * Rating property definition, not own.
    */
    CMdEPropertyDef* iRatingPropertyDef;
    
    /**
    * 19.
    * Bitrate property definition, not own.
    */
    CMdEPropertyDef* iBitratePropertyDef;
    
    /**
    * DRM protection flag, not own.
    */
    CMdEPropertyDef* iDrmPropertyDef;

    /**
    * Not own.
    */
    CMdEPropertyDef* iAudioFourCcPropertyDef;
    
    /**
    * 22.
    * Resolution Y-axis unit. Not own.
    */
    CMdEPropertyDef* iWidthPropertyDef;
    
    /**
    * 23. 
    * Resolution X-axis unit. Not own.
    */
    CMdEPropertyDef* iHeightPropertyDef;
    
    /**
     * 24.
     * The artist property definition. Not own.
     */
    CMdEPropertyDef* iArtistPropertyDef;
    
    /**
     * Active scheduled waiter Own..
     */
    CActiveSchedulerWait* iActiveSchedulerWait;

    /**
     * The file server session.
     */
    RFs& iFs;

    /**
    * The observer for db changes. Not own.
    */
    MVcxMyVideosMdsDbObserver* iMdsDbObserver;
    
    /**
     * The observer for albums related database events. Not own.
     */
    MVcxMyVideosMdsAlbumsObserver* iAlbumsObserver;
    
    /**
     * Used to handle complete events asynchronously.
     * This avoids problems like deleting query objects in the
     * handler or starting new query from the handler.
     */
    CAsyncCallBack* iAsyncHandleQueryCompleteCaller;

    /**
    * Monitors Mds server shutdown states.
    */
    CVcxMdsShutdownMonitor* iMdsShutdownMonitor;

    /**
     * Used for handling events on background.
     */
    CIdle* iEventProcessor;
    
    /**
     * Incoming events are stored here and "slowly" fed to observer.
     */
    RArray<TEvent> iEvents;
    };

#endif // VCXMYVIDEOSMDSDB_H

