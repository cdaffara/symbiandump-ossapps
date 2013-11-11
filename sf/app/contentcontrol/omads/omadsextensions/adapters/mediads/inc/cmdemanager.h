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
* Description:  CMdEManager declaration
*
*/


#ifndef CMDEMANAGER_H
#define CMDEMANAGER_H

#include <e32base.h>
#include <mdesession.h>
#include <mdequery.h>

#include "playlistitem.h"

class MMdEManagerObserver
    {
public:
    virtual void AlbumsReaded( TInt aError ) = 0;
    virtual void AlbumReplaced( TInt aError ) = 0;
    };

class CMdEManager : public CBase,
                        MMdESessionObserver,
                        MMdEQueryObserver
    {
public:
    // Cancel and destroy
    ~CMdEManager();

    // Two-phased constructor.
    static CMdEManager* NewL( MMdEManagerObserver& aObserver );

public:
    
    /**
	 * Start enumerating albums. Calls MMdEManagerObserver::AlbumsReaded when ready.
	 * This needs to be called before any other operations.
	 */ 
    void GetAlbumsL();
    

    RPointerArray<CPlaylistItem>& AlbumsArray(){ return iAlbums; };
    const CPlaylistItem& AlbumL( TInt aAlbumId ) const;
    
    void CreateAlbumL( CPlaylistItem& aAlbum );
    void ReplaceAlbumL( TInt aAlbumId, CPlaylistItem& aAlbum );
    void DeleteAlbumL( TInt aAlbumId );
    
	/**
	 * Cancels any outstanding request if any.
	 */
    void Cancel();
    
private:
    void StartProcessingAlbumsL();
    void ProcessNextAlbumL();
    void FindItemsOnAlbumL( TItemId aAlbumObjectId );
    
private: // from MMdESessionObserver
    /**
     * Called to notify the observer that opening the session has been 
     * completed and, if the opening succeeded, the session is ready for use.
     *
     * @param aSession session
     * @param aError   <code>KErrNone</code>, if opening the session succeeded;
     *                 or one of the system-wide error codes, if opening the 
     *                 session failed
     */
    void HandleSessionOpened(CMdESession& aSession, TInt aError);

    /**
     * Called to notify the observer about errors, which are not a direct 
     * consequence of the operations initiated by the client but caused by 
     * some external source (e.g., other clients). The error cannot be 
     * recovered and all on-going operations initiated by the client have been 
     * aborted. Any attempts to continue using the session will cause a panic. 
     * The client should close the session immediately and try to open a new 
     * session, if it needs to continue using the metadata engine.
     *
     * @param aSession session
     * @param aError one of the system-wide error codes
     */
    void HandleSessionError(CMdESession& aSession, TInt aError);

private: // from MMdEQueryObserver
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
private:
    
    CMdEManager( MMdEManagerObserver& aObserver );
    
    void ConstructL();
    
    void HandleAlbumQueryCompletedL( CMdEObjectQuery& aQuery );
    void HandleObjectQueryCompletedL( CMdEObjectQuery& aQuery );
    void HandleRelationQueryCompletedL( CMdERelationQuery& aQuery );
    
private:
    enum TCMdEManagerState
        {
        EUninitialized,             // Uninitialized / initialization is ongoing
        EWaitingToEnumerateAlbums,  // Initializing and starting collecting albums info
        EEnumeratingAlbums,         // Album enumeration is ongoing
        EReplacingAlbum,            // Replacing album (searching removed relation id:s)
        EIdle                       // Ready for next operation
        };

private:
    MMdEManagerObserver& iObserver;
    TCMdEManagerState   iState;
    
    CMdESession*        iMde;
    CMdEObjectQuery*    iAlbumQuery;
    CMdEObjectQuery*    iObjectQuery;
    CMdERelationQuery*  iContainmentQuery;
    
    RPointerArray<CPlaylistItem> iAlbumsInProgress;
    RPointerArray<CPlaylistItem> iAlbums;
    
    };

#endif // CMDEMANAGER_H
