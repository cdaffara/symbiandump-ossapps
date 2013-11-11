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
 * Description: 
 *
 */

/*glxmlwrapper_p.h*/

#ifndef GLXMLWRAPPER_P_H
#define GLXMLWRAPPER_P_H
#include <QObject>
#include <glxfiltergeneraldefs.h>
#include <glxlistdefs.h>
#include <mpxcollectionobserver.h>
#include <qdatetime.h>
#include "glxmedialistiterator.h"
#include "glxmlwrapper.h"
#include <QImage>
#include "mglxtitlefetcherobserver.h"

//Forward Declarations
class MGlxMediaList;
class CGlxMLGenericObserver;
class CGlxThumbnailContext;
class HbIcon;
class QImage;
class CGlxDefaultAttributeContext;
class CGlxDefaultThumbnailContext;
class CGlxDefaultListAttributeContext;
class CGlxTitleFetcher;
class CGlxDRMUtility;


/**
 * Class Description
 * Medialist wrapper private class to acess the media property and thumbnail data
 * To use first call GlxMLWrapperPrivate::Instance then set the mode by calling GlxMLWrapperPrivate::SetContextMode()
 */
class GlxMLWrapperPrivate: public QObject, public MGlxTitleFetcherObserver
{
friend class TestGlxMLWrapper;
public:
    /**
     * Two-phased constructor.
     *
     * @return Pointer to newly created object. NULL if creation fails. Avoiding Leave as would be called by a QT wrapper Class
     */
    static GlxMLWrapperPrivate* Instance( GlxMLWrapper* aMLWrapper, int aCollectionId,
        int aHierarchyId, TGlxFilterItemType aFilterType, QString uri,
        GlxFetcherFilterType fetcherFilterType );
    /**
     * Destructor.
     */
    ~GlxMLWrapperPrivate();
    /**
     * for setting the attribute context mode will be used mainly for retreiving thumbnail of different sizes.
     */
    void SetContextMode( GlxContextMode aContextMode );

    /**
     * For removing the contect used
     */
    void RemoveContextMode( GlxContextMode aContextMode );

    /**
     * for retreiving the Media List Count.
     */
    int GetItemCount();
    
    /**
     * for retrieving the focus Index
     */
    int GetFocusIndex() const;
    
    /**
     * for setting the focus Index
     */
    void SetFocusIndex( int aItemIndex );

    /**
     * for setting the focus Index
     */
    void SetSelectedIndex( int aItemIndex );

    /**
     * for retrieving the visible window Index
     */
    int GetVisibleWindowIndex();
    
    /**
     * for setting the visible window Index
     */
    void SetVisibleWindowIndex( int aItemIndex );

public:
    // From MGlxTitleFetcherObserver
    IMPORT_C void HandleTitleAvailableL( const TDesC& aTitle );

public:
    void HandleItemAddedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList );
    void HandleItemRemovedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList*/* aList */);
    void HandleAttributesAvailableL( TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes,
        MGlxMediaList* aList );
    void HandleFocusChangedL( NGlxListDefs::TFocusChangeType aType, TInt aNewIndex, TInt aOldIndex,
        MGlxMediaList* aList );
    void HandleItemSelectedL( TInt aIndex, TBool aSelected, MGlxMediaList* aList );
    void HandleMessageL( const CMPXMessage& aMessage, MGlxMediaList* aList );
    void HandleError( TInt aError );
    //todo remove comment  void HandleCommandCompleteL( CMPXCommand* aCommandResult, TInt aError,MGlxMediaList* aList );
    void HandleMediaL( TInt aListIndex, MGlxMediaList* aList );
    void HandleItemModifiedL( const RArray<TInt>& aItemIndexes, MGlxMediaList* aList );
    void HandlePopulatedL( MGlxMediaList* aList );

public:
    /**
     * RetrieveItemIcon() -  To retreive the full screen and grid thumbnail
     */
    HbIcon* RetrieveItemIcon( int index, GlxTBContextType aTBContextType );    
    QImage RetrieveItemImage( int index, GlxTBContextType aTBContextType );
    QString RetrieveListTitle( int index );
    QString RetrieveListDesc( int index );
    QString RetrieveListSubTitle( int index );
    QString RetrieveItemUri( int index );
    QSize RetrieveItemDimension( int index );
    int RetrieveItemSize( int index );
    QDate RetrieveItemDate( int index );
    QTime RetrieveItemTime( int index );
    int RetrieveItemFrameCount( int aItemIndex );
    CFbsBitmap* RetrieveBitmap( int aItemIndex );
    int RetrieveListItemCount( int aItemIndex );
    bool isSystemItem( int aItemIndex );
    /*
     *Sets the context for fetcing comments
     */
    void SetDescontextL();
    QString RetrieveViewTitle();
    bool IsPopulated();
    bool IsDrmProtected( int index );
    bool IsDrmValid( int index );
    void setDrmValid( int index, bool valid );
    bool IsCorruptedImage( int aItemIndex );
private:

    /**
     * C++ default constructor.
     */
    GlxMLWrapperPrivate( GlxMLWrapper* aMLWrapper );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( int aCollectionId, 
                          int aHierarchyId, 
                          TGlxFilterItemType aFilterType,
                          QString uri = NULL, 
                          GlxFetcherFilterType fetcherFilterType = EGlxFetcherFilterNone );
    /**
     * Create an instance of Media List.
     *
     * @leave with error code if creation fails.
     */
    void CreateMediaListL( int aCollectionId, 
                              int aHierarchyId, 
                              TGlxFilterItemType aFilterType,
                              GlxFetcherFilterType fetcherFilterType = EGlxFetcherFilterNone );
    
    void CreateMediaListAlbumItemL( int aCollectionId, 
                                      int aHierarchyId,
                                      TGlxFilterItemType aFilterType, GlxFetcherFilterType fetcherFilterType );
    
    void CreateMediaListFavoritesItemL( int aCollectionId, int aHierarchyId, TGlxFilterItemType aFilterType, QString uri );
    void SetThumbnailContextL( GlxContextMode aContextMode );
    void SetListContextL( GlxContextMode aContextMode );
    void SetFavouriteContextL();
    void CreateGridContextL();
    void CreateLsFsContextL();
    void CreatePtFsContextL();
    void RemoveGridContext();
    void RemoveLsFsContext();
    void RemovePtFsContext();
    void RemoveListContext();
    void RemoveFavouriteContext();
    void RemoveDescContext();
    //for the attribute filtering
    TInt CheckTBAttributesPresenceandSanity( TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes, TMPXAttribute aThumbnailAttribute );
    void CheckGridTBAttribute( TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes );
    void CheckPtFsTBAttribute( TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes );
    void CheckLsFsTBAttribute( TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes );
    void CheckListAttributes( TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes );
    /*
     * to check the attributes returned for details view is present
     */
    void CheckDetailsAttributes( TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes );
    
    /**
     * convertFBSBitmapToHbIcon() - convert the CFbsbitmap to HbIcon
     * @param - bitmap of media
     * @return - Hbicon of the modia.
     */
    HbIcon *convertFBSBitmapToHbIcon( CFbsBitmap* aBitmap );
    
    /*
     * convertFBSBitmapToHbIcon() - convert the CFbsbitmap to HbIcon and scale down upto itemWidth and itemHeight
     * @param - bitmap of media
     * @param - expected HbIcon width
     * @param - expected HbIcon height
     * @param - aspect ratio parameter in the case of scaling the thumbnail
     * @return - Hbicon of the modia.
     */
    HbIcon * convertFBSBitmapToHbIcon( CFbsBitmap* aBitmap, TInt itemWidth, TInt itemHeight, Qt::AspectRatioMode aspectRatio = Qt::KeepAspectRatio );

private:
    // Instance of Media List
    MGlxMediaList* iMediaList;
    //observer for ML
    CGlxMLGenericObserver* iMLGenericObserver;
    GlxContextMode iContextMode;
    //Parent QT based Wrapper
    GlxMLWrapper* iMLWrapper;
    //contexts
    // Thumbnail context
    CGlxThumbnailContext* iGridThumbnailContext;

    // FS related thumbnail contexts
    CGlxDefaultThumbnailContext* iPtFsThumbnailContext;
    CGlxDefaultThumbnailContext* iLsFsThumbnailContext;
    CGlxDefaultThumbnailContext* iFocusFsThumbnailContext;
    CGlxDefaultThumbnailContext* iFocusGridThumbnailContext;

    //List related contexts
    // Fetch context for retrieving title attribute
    CGlxDefaultListAttributeContext* iTitleAttributeContext;

    // Fetch context for retrieving subtitle
    CGlxDefaultListAttributeContext* iSubtitleAttributeContext;

    //to find if the image is in favorites or not
    CGlxDefaultAttributeContext *iFavouriteContext;

    //To fetch the details
    CGlxDefaultAttributeContext *iDescContext;

    CGlxThumbnailContext* iListThumbnailContext;
    // for thumbnail context
    TGlxFromVisibleIndexOutwardListIterator iThumbnailIterator;

    // Iterators for Grid and FS
    TGlxScrollingDirectionIterator iBlockyIterator;
    TGlxFromManualIndexBlockyIterator iBlockyIteratorForFilmStrip;

    //Variables for checking the active contexts
    //todo merge all these variables into 1 and have bitwise operation on them
    TBool iGridContextActivated;
    TBool iLsFsContextActivated;
    TBool iPtFsContextActivated;
    TBool iLsListContextActivated; //currently not used as we have not implemented the logic for 3 thumbnails
    TBool iPtListContextActivated;
    TBool iDetailsContextActivated; //this is to fetch the comments attributes
    TBool iSelectionListContextActivated;
    CGlxTitleFetcher* iTitleFetcher;
    QImage iCorruptImage;
    QString iViewTitle;
    CGlxDRMUtility * iDrmUtility;
};
#endif //GLXMLWRAPPER_P_H 
