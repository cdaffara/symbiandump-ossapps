/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Fetch context to retrieve thumbnails
*
*/




#ifndef C_GLXTHUMBNAILCONTEXT_H
#define C_GLXTHUMBNAILCONTEXT_H

#include <e32std.h>
#include "mglxfetchcontext.h"
#include "glxmedialistiterator.h"
#include <mglxresolutionchangeobserver.h>

// Forward declarations

class CGlxAttributeContext;
class CGlxDRMUtility;
class CGlxResolutionUtility;
//class MGlxCache;
class MGlxMediaList;
class TGlxMedia;

/**
 *  CGlxThumbnailContext
 *
 *  Fetch context to retrieve thumbnails
 *
 *  @lib glxmedialists.lib
 */
class CGlxThumbnailContext : public CBase, public MGlxFetchContext/*,
        public MGlxResolutionChangeObserver*/
	{
public:
    /**
	 * Constructor
	 * @param aIterator Iterator that determines the order of thumbnail 
	 *  	  retrieval and for which items thumbnails are and are not retrieved
	 */
	IMPORT_C static CGlxThumbnailContext* NewL(MGlxMediaListIterator* aIterator);

	/**
	 * Constructor
	 * @param aIterator Iterator that determines the order of thumbnail 
	 *  	  retrieval and for which items thumbnails are and are not retrieved
	 */
	IMPORT_C CGlxThumbnailContext(MGlxMediaListIterator* aIterator);
	IMPORT_C ~CGlxThumbnailContext();

	/**
	 * Adds a fetch specification for an individual item
	 * @param aWidth the width of required bitmap. The context
	 *				 may retrieve first a thumbnail of difference size
	 *				 if it thinks it helps user experience.
	 * @param aHeight the width of required bitmap. 
	 * @param aFocusOffset Delta to focus. 0 means focus.
	 */
	IMPORT_C void AddSpecForItemL(TInt aWidth, TInt aHeight, TInt aFocusOffset);

	/**
	 * Sets the default fetch specification. The context uses this
	 * for all items that do not have a specific spec added.
	 * @param aWidth the width of required bitmap. The context
	 *				 may retrieve first a thumbnail of difference size
	 *				 if it thinks it helps user experience.
	 * @param aHeight the heigth of required bitmap. 
	 */
	IMPORT_C void SetDefaultSpec(TInt aWidth, TInt aHeight);

    /**
     * Set whether only high quality thumbnails are wanted.
     * @param aHighQualityOnly ETrue to skip fetching low quality thumbnails.
     */
    IMPORT_C void SetHighQualityOnly( TBool aHighQualityOnly );

public: // From MGlxFetchContext
    /// See @ref MGlxFetchContext::AttributeRequestL
    TInt AttributeRequestL(const MGlxMediaList* aList, RArray<TInt>& aItemIndices, 
    	RArray<TMPXAttribute>& aAttributes, CMPXAttributeSpecs*& aDetailedSpecs) const;

    /// See @ref MGlxFetchContext::AllAttributesL
    void AllAttributesL(const MGlxMediaList* aList, TInt aListIndex, 
    	RArray<TMPXAttribute>& aAttributes) const;

    /// See @ref MGlxFetchContext::RequestCountL
    TInt RequestCountL(const MGlxMediaList* aList) const;

protected:
    /**
    * 2nd phase construction
    */
    void ConstructL();

private:    // From MGlxResolutionChangeObserver
   // void HandleResolutionChangedL();

private: 
        /**
         * Select the best thumbnail for processing
         *
         * @param aList pointer to media list
         * @param aError indication of any errors encountered when selecting item
         */
        TInt SelectItemL(const MGlxMediaList* aList, TInt& aError) const;

        /**
         * Calculates a score for thumbnail retrieval
         *
         * @param aIndexInList index of the item in the list
         * @param aList pointer to media list
         * @param aDistance Distance from highest priority index
         * @param aError indication of any errors encountered when calculating score
         *
         * @returns a score of how important processing this item's
         * 			thumbnail is. The higher, the more important.
         */
	TUint GetThumbnailScoreL(TInt aIndexInList, const MGlxMediaList* aList,
		TInt aDistance, TInt& aError) const;

    /** Item to specify what kind of thumbnail is needed */
    struct TFetchSpec
        {
        inline TFetchSpec() { iPossWidth = 0; iPossHeight = 0; };

        /** Width of the thumbnail */
        TReal32 iPossWidth;

        /** Height of the thumbnail */
        TReal32 iPossHeight;
        };

    /** 
    * Item to specify what kind of thumbnail is needed 
    * for which item (compared to focus)
    */
    struct TAssignedFetchSpec : public TFetchSpec
        {
        inline TAssignedFetchSpec() { iFocusOffset = 0; };

        /** Compares two specs by focus offset. Used for RArray::Find() */
        static TBool Match( const TAssignedFetchSpec& aSpec1,
                            const TAssignedFetchSpec& aSpec2 );

        /** Delta to focus. If zero, then focused item */
        TInt iFocusOffset;
        };

    /**
     * Get the fetch spec to be used for a given media item.
     * @param aListIndex Index of current item
     * @param aList Pointer to media list
     * @return Reference to fetch spec to use for the given item.
     */
    const TFetchSpec& SpecForIndex(TInt aListIndex,
                                    const MGlxMediaList* aList) const;

    /**
     * Get the size in pixels for a fetch spec.
     * @param aSpec Reference to fetch spec.
     * @return Size in pixels for the current screen resolution.
     */
    TSize SizeFromSpec( const TFetchSpec& aSpec ) const;

    /**
     * Check whether a given item has any full thumbnail attributes.
     * @param aItem Media item to check.
     * @return ETrue if a full thumbnail attribute is present.
     */
    TBool HasFullThumbnail( const TGlxMedia& aItem ) const;

    /**
     * Check DRM status of item at given index. Updates the fetch spec
     * for an item with invalid DRM
     * @param aListIndex index of current item
     * @param aList pointer to media list
     * @param aSpec reference to fetch spec
	 * @return ETrue if the item at index is not protected or has valid rights
     */
    TBool CheckDRMStatusL( TInt aListIndex, const MGlxMediaList* aList,
                                        TSize& aSize ) const;           

private:
    /** Specs applies to individual items */
    RArray<TAssignedFetchSpec> iSpecs;

    /** Request spec that is applied to all other items than in iSpecs */
    TFetchSpec iDefaultSpec;

    /** Whether to skip fetching low quality thumbnails */
    TBool iHighQualityOnly;

    /** Iterator for traversing the list */
    MGlxMediaListIterator* iIterator;	

    /** Cache manager interface (owned) */
    //MGlxCache* iCache;

    /** Utility to translate between pixels and poss units */
    CGlxResolutionUtility* iResolutionUtility;

    /** DRM utility for checking rights */
    CGlxDRMUtility* iDrmUtility;

    /** Attribute context to fetch DRM attributes */
    CGlxAttributeContext* iDrmFetchContext;
    
    /** Size of the default spec */
    TSize iDefaultSpecSize;
    };

/**
 *  CGlxDefaultAttributeContext
 *
 *  Fetch context to retrieve MPX attributes starting from focus outward
 *
 *  @lib glxmedialists.lib
 */
class CGlxDefaultThumbnailContext : public CGlxThumbnailContext
	{
public:
	IMPORT_C static CGlxDefaultThumbnailContext* NewL();
	IMPORT_C ~CGlxDefaultThumbnailContext();

	/**
	 * Set the range offsets. These determine the outer limits
	 * of attributes to be retrieved, compared to current focus in
	 * the list
	 * If ranges not defined, retrieves the attributes for focused item only
	 */
    IMPORT_C void SetRangeOffsets(TInt aFrontOffset, TInt aRearOffset);

private:
	CGlxDefaultThumbnailContext();

private:
	TGlxFromFocusOutwardIterator iFromFocusIterator;	
	};

#endif // C_GLXTHUMBNAILCONTEXT_H

