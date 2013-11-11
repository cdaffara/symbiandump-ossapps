/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Bookmarks harvester plugin header
*
*/

#ifndef CBOOKMARKSPLUGIN_H
#define CBOOKMARKSPLUGIN_H

#include <cindexingplugin.h>

#include <e32base.h>
#include <FavouritesSession.h>
#include <FavouritesDb.h>
#include <FavouritesDbObserver.h>

#include "common.h"
#include "delayedcallback.h"

class CSearchDocument;
class CCPixIndexer;
class CActiveFavouritesDbNotifier;

#define KBookmarksDllUid 0x2001A9D3

//Bookmark field name to be stored
_LIT(KBookMarkFieldName, "Name");
_LIT(KBookMarkUrl, "Url");
_LIT(KBookMarkDomain, "Domain");

_LIT(KMimeTypeField, CPIX_MIMETYPE_FIELD);
_LIT(KMimeTypeBookmark, BOOKMARK_MIMETYPE);

class CBookmarksPlugin : public CIndexingPlugin, public MDelayedCallbackObserver, public MFavouritesDbObserver
{
public:
    // Constructor and Destructors.
    static CBookmarksPlugin *NewL();
    static CBookmarksPlugin *NewLC();
    virtual ~CBookmarksPlugin();
    
public:
    /* 
     * Prepares the plugin for StartHarvesterL and also  starts
     * the favourites db observer.  
     */
    void StartPluginL();
    
    /*
     * @description Start the harvester.
     * @param aQualifiedBaseAppClass the base app class. In this case
     *        use BOOKMARK_QBASEAPPCLASS
     * @return void
     * Leaves in case of errors.
     */
    void StartHarvestingL(const TDesC& aQualifiedBaseAppClass);
    
    virtual void PausePluginL();
    
    virtual void ResumePluginL();
    
    //From MFavouritesDbObserver.
    void HandleFavouritesDbEventL(RDbNotifier::TEvent aEvent);
    
public:
    // From MDelayedCallbackObserver
    void DelayedCallbackL(TInt aCode);
    void DelayedError(TInt aErrorCode);
    
private:
    //private constructors.
    CBookmarksPlugin();
    void ConstructL();
    
    /*
     * @description Add, update or delete bookmark with UID aBookMarkUid based on aActionType.
     * @param aBookmarkUid: obtained from the array of UIDs got via GetUids().
     * @param aActionType: add, update or delete.
     * @return void
     * Leaves in case of error.
     */
    void CreateBookmarksIndexItemL(TInt aBookMarkUid, TCPixActionType aActionType);
    
    /*
     * @description Helper function to add/update index.
     * @param aItem Item to be indexed.
     * @param aDocidStr descriptor version of the bookmarkUid.
     * @param aActionType add/update actions only.
     * @return void
     * Leaves in case of error.
     * @note: aItem is a reference to a pointer. Ownership remains with the caller.
     */
    void DoIndexingL(CFavouritesItem*& aItem, const TDesC& aDocidStr, TCPixActionType& aActionType);
    
private:
    CDelayedCallback *iAsynchronizer; //to schedule the harvesting into chunks. Owned.
    CCPixIndexer *iIndexer; //Owned
    RFavouritesSession iFavouritesSession; //Favourites session for use by FavouritesDb.
    TInt iCurrentIndex; //Index of bookmark in UID array currently being added to index. 
    TInt iCurrentCount; //Total number of bookmarks currently in the bookmarks DB.
    RFavouritesDb iFavouritesDb; //THE bookmarks DB.
    CActiveFavouritesDbNotifier *iFavoritesNotifier; //Active object that notifies commits to favourites DB. Owned.
    CArrayFix<TInt> *iArrUidsCurrentBookmarkList; //List of UIDs obtained from Favourites DB that will be indexed.Owned.

#ifdef __PERFORMANCE_DATA
    TTime iStartTime;
    TTime iCompleteTime;
    void UpdatePerformaceDataL();
#endif
};

#endif // CBOOKMARKSPLUGIN_H
