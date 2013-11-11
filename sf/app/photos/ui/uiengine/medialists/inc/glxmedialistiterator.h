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
* Description:    Iterators for traversing lists
*
*/




#ifndef __C_GLXMEDIALISTITERATOR_H__
#define __C_GLXMEDIALISTITERATOR_H__

#include <e32std.h>
#include <glxmediaid.h>

class MGlxMediaList;
class T_CGlxSelectionIterator; // For E-Unit
class T_CGlxFromFocusOutwardIterator; // For E-Unit

/**
 *  MGlxMediaListIterator
 *  
 *  Interface for going through items in the list in a 
 *  non-sequential order. 
 *
 *  This class is primarily intended for Fetch contexts for going 
 *  through the list's items in priority order. The intention is to abstract 
 *  the selection and  priorities of items from the attributes being requested.
 *  This allows reusing same fetch contexts with multiple item orders.
 *  Can hence have one attribute fethc context that requests first 
 *  and last item and another attribute that requests some items 
 *  starting from focus outwards, without duplicating the attribute fetch 
 *  context code. The order classes can then also be reused in different
 *  fetch contexts, e.g., in thumbnail fetch context.
 * 
 *  @lib glxmedialists.lib
 */	
class MGlxMediaListIterator 
	{
public:
	/**
	 * Resets to the first item to be returned 
	 * What the first item index actually is is entirely
	 * up to the implementation. It could be the focus index,
	 * or it could be the first, or even last item in the list
	 *
	 * @return the index of the first item or KErrNotFound if 
	 *          first item does not exist
	 */
	virtual void SetToFirst(const MGlxMediaList* aList) = 0;

	/**
	 * Returns the index of the current item and moves to the next item
	 * @return the item index or KErrNotFound if the item does not exist
	 */
	virtual TInt operator++(TInt) = 0;
	
	/**
	 * @return ETrue if index is within range, EFalse otherwise
	 */
	virtual TBool InRange(TInt aIndex) const = 0;
	};

/**
 *  TGlxSpecificIdIterator
 * 
 *  Used to retrieve attributes relating to one specific media
 *  id in a media list.
 * 
 *  @lib glxmedialists.lib
 */ 
class TGlxSpecificIdIterator : public MGlxMediaListIterator
	{
public:
    IMPORT_C TGlxSpecificIdIterator(const TGlxIdSpaceId& aIdSpaceId, TGlxMediaId aRequiredId);
	
public: // From MGlxMediaListIterator
	void SetToFirst(const MGlxMediaList* aList);
	TInt operator++(TInt);
	TBool InRange(TInt aIndex) const;
	
private:
    /**
     *  Required id
     */
	TGlxMediaId iMediaId;
	
    /**
	 * List being traversed
	 */
	const MGlxMediaList* iList; 
	
	const TGlxIdSpaceId iIdSpaceId;  
    
    TBool iFinished;
	};

/**
 *  TGlxSequentialIterator
 * 
 *  Iterates though all the items in the list sequentially.
 * 
 *  @lib glxmedialists.lib
 */ 
class TGlxSequentialIterator : public MGlxMediaListIterator
    {
public:   
    IMPORT_C TGlxSequentialIterator();

public: // From MGlxMediaListIterator
    void SetToFirst(const MGlxMediaList* aList);
    TInt operator++(TInt);
    TBool InRange(TInt aIndex) const;
    
public:
	/**
	 * Set the maximum number of items to iterate through
	 * @param aRange Maximum number of items to iterate through. Set as KMaxTInt to remove limit
	 */
    IMPORT_C void SetRange( TInt aRange );    

private:
    TInt iCurrentItem;
    
    /**
     * The maximum number of items to retrieve 
     */
    TInt iRange;
    
    /**
     * List being traversed
     */
    const MGlxMediaList* iList;
    };

/**
 * TGlxFirstThenLastIterator
 * 
 * Ordered that returns the index of first and then the 
 * last item in the list
 * 
 *  @lib glxmedialists.lib
 */	
class TGlxFirstThenLastIterator : public MGlxMediaListIterator
	{
public:
	IMPORT_C TGlxFirstThenLastIterator();
	IMPORT_C ~TGlxFirstThenLastIterator();

// From MGlxMediaListIterator
	virtual void SetToFirst(const MGlxMediaList* aList);
	virtual TInt operator++(TInt);
	virtual TBool InRange(TInt aIndex) const;

private:
	/**	
	 * The count or ordinal of the item that is currently "selected"
	 * This translates to how many times ++ has been called
	 */
	TInt iCurrentItem;

	/**
	 * List being traversed
	 */
	const MGlxMediaList* iList;
	};

/**
 * TGlxFromFocusOutwardIterator
 * 
 * Ordered that returns the index of the focused item first and 
 * then the surrounding items evenly from both sides
 * 
 *  @lib glxmedialists.lib
 */	
class TGlxFromFocusOutwardIterator : public MGlxMediaListIterator
	{
public:
	IMPORT_C TGlxFromFocusOutwardIterator();
	IMPORT_C ~TGlxFromFocusOutwardIterator();
	
	/**
	 * Set the range offsets
	 */
    IMPORT_C void SetRangeOffsets(TInt aRearOffset, TInt aFrontOffset);
    
// From MGlxMediaListIterator
	virtual void SetToFirst(const MGlxMediaList* aList);
	virtual TInt operator++(TInt);
	virtual TBool InRange(TInt aIndex) const;

private:
	/**	
	 * The count or ordinal of the item that is currently "selected"
	 * This translates to how many times ++ has been called
	 */
	TInt iCurrentItem;

	/**
	 * Number of items within range after focus index
	 */
	TInt iFrontOffset;

	/**
	 * Number of items within range before focus index
	 */
	TInt iRearOffset;
	
	/**
	 * List being traversed
	 */
	const MGlxMediaList* iList;
	
	// For E-Unit
	friend class T_CGlxFromFocusOutwardIterator;
	
    /**
     * Number of items within range as configured by clients
     * This shall be used to restore back 
     * while coming out of low memory conditions
     */
    TInt iOriginalFrontOffset;
    TInt iOriginalRearOffset;
	};

/**
 * TGlxFromVisibleIndexOutwardListIterator
 * 
 * Ordered that returns the index of the focused item first and 
 * then the surrounding items evenly from both sides based on the rangeoffset
 * This is mainly designed for 10.1 List View(album) and mapped with
 * VisibleWindowIndex and not FocusIndex as for Direct UI.
 * 
 *  @lib glxmedialists.lib
 */ 
class TGlxFromVisibleIndexOutwardListIterator : public MGlxMediaListIterator
    {
public:
    IMPORT_C TGlxFromVisibleIndexOutwardListIterator();
    IMPORT_C ~TGlxFromVisibleIndexOutwardListIterator();
    
    /**
     * Set the range offsets
     */
    IMPORT_C void SetRangeOffsets(TInt aRearOffset, TInt aFrontOffset);
    
// From MGlxMediaListIterator
    virtual void SetToFirst(const MGlxMediaList* aList);
    virtual TInt operator++(TInt);
    virtual TBool InRange(TInt aIndex) const;

private:
    /** 
     * The count or ordinal of the item that is currently "selected"
     * This translates to how many times ++ has been called
     */
    TInt iCurrentItem;

    /**
     * Number of items within range after focus index
     */
    TInt iFrontOffset;

    /**
     * Number of items within range before focus index
     */
    TInt iRearOffset;
    
    /**
     * List being traversed
     */
    const MGlxMediaList* iList;
    
    // For E-Unit
    friend class T_CGlxFromFocusOutwardIterator;
    
    /**
     * Number of items within range as configured by clients
     * This shall be used to restore back 
     * while coming out of low memory conditions
     */
    TInt iOriginalFrontOffset;
    TInt iOriginalRearOffset;
    };

/**
 * TGlxFromIndexOutwardBlockyIterator
 * 
 * Ordered that returns the index of the focused item first and 
 * then the surrounding items evenly from both sides
 */	
class TGlxFromIndexOutwardBlockyIterator : public MGlxMediaListIterator
	{
	
public:
    class MGlxIndex 
    	{
    public:
    	virtual TInt Index() const = 0;
    	};
    	
public:
	 IMPORT_C ~TGlxFromIndexOutwardBlockyIterator( );
	 IMPORT_C TGlxFromIndexOutwardBlockyIterator(const MGlxIndex& aIndexFunctor);
	
	/**
	 * Set the range offsets
	 */
     IMPORT_C void SetRangeOffsets(TInt aRearOffset, TInt aFrontOffset);
    
// From MGlxMediaListIterator
	virtual void SetToFirst(const MGlxMediaList* aList);
	virtual TInt operator++(TInt);
	virtual TBool InRange(TInt aIndex) const;
    
//protected: 
    /**
     * Functor to return index in media list based on unknown rule
     * Rule is defined by a deriving class
     */		
 /*   class MGlxIndex 
        {
    public:
        virtual TInt Index() const = 0;
        };*/
        
private:
	/**	
	 * The count or ordinal of the item that is currently "selected"
	 * This translates to how many times ++ has been called
	 */
	TInt iCurrentItem;

	/**
	 * Number of items within range after focus index
	 */
	TInt iFrontOffset;

	/**
	 * Number of items within range before focus index
	 */
	TInt iRearOffset;
    
    /**
     * Functor from which to get the index around which iteration centers (e.g., focus)
     */
    const MGlxIndex& iIndexFunctor;

protected:	
	/**
	 * List being traversed
	 */
	const MGlxMediaList* iList;
	};
    
/**
 * TGlxFromFocusOutwardBlockyIterator
 * 
 * Ordered that returns the index of the focused item first and 
 * then the surrounding items evenly from both sides
 */		
class TGlxFromFocusOutwardBlockyIterator : public TGlxFromIndexOutwardBlockyIterator,  
        public TGlxFromIndexOutwardBlockyIterator::MGlxIndex
	{
public:
    IMPORT_C TGlxFromFocusOutwardBlockyIterator();
	IMPORT_C ~TGlxFromFocusOutwardBlockyIterator();
   
// From MGlxIndex    
    TInt Index() const;
    };

/**
 * TGlxFromManualIndexOutwardBlockyIterator
 */		
class TGlxFromManualIndexOutwardBlockyIterator : public TGlxFromIndexOutwardBlockyIterator, 
        public TGlxFromIndexOutwardBlockyIterator::MGlxIndex
	{
public:
    IMPORT_C TGlxFromManualIndexOutwardBlockyIterator();
	IMPORT_C ~TGlxFromManualIndexOutwardBlockyIterator();
   
    IMPORT_C void SetIndex( TInt aIndex );
    
// From MGlxIndex    
    TInt Index() const;
    
private:
    TInt iIndex;
    };
    
/**
 * TGlxSelectionIterator
 * 
 * Iterates through the selected items of a media list
 * 
 *  @lib glxmedialists.lib
 */	
class TGlxSelectionIterator : public MGlxMediaListIterator
	{
public:
	IMPORT_C TGlxSelectionIterator();
	IMPORT_C ~TGlxSelectionIterator();

public:
    /**
    * Set the maximum number of selected items to iterate through
    @param aMaxItems Maximum number of selected items to iterate through. Set as KMaxTInt to remove limit
    */
    IMPORT_C void SetRange(TInt aMaxItems);
   
   /**
    * Disables the iterator if the number of selected items is greater than
    * the range. (specified using SetRange() above).
    * @param aDisabledIfMoreThanRangeSelected If ETrue, the iterator is 
    * disabled if the number of selected items is greater than the range. 
    * If EFalse, the iterator is enabled if the number of selected items 
    * is greater than the range.
    */
    IMPORT_C void SetDisabledIfMoreThanRangeSelected(TBool aDisabledIfMoreThanRangeSelected);
    
    // From MGlxMediaListIterator
    IMPORT_C virtual void SetToFirst(const MGlxMediaList* aList);
    IMPORT_C virtual TInt operator++(TInt);
    IMPORT_C virtual TBool InRange(TInt aIndex) const;

private:
    /**	
    * The count or ordinal of the item that is currently "selected"
    * This translates to how many times ++ has been called
    */
    TInt iCurrentItem;

    /**
    * List being traversed
    */
    const MGlxMediaList* iList;

    /**
    * If ETrue, the iterator is  disabled if the number of selected 
    * items is greater than the range. If EFalse, the iterator is enabled 
    * if the number of selected items is greater than the range.
    */
    TInt iDisabledIfMoreThanRangeSelected;
    
    /**
    * The maximum number of selected items to iterate through.
    */
    TInt iRange;

    // For E-Unit
    friend class T_CGlxSelectionIterator;
    };

/**
 *  ExclusionIterator
 * 
 *  Iterates through the non visible items  
 *  
 * 
 *  @lib glxmedialists.lib
 */ 
class TGlxExclusionIterator : public MGlxMediaListIterator
	{
	
public:
    IMPORT_C TGlxExclusionIterator( MGlxMediaListIterator   &aIncludedItemsIterator, MGlxMediaListIterator &aExcludedItemsIterator );
    IMPORT_C ~TGlxExclusionIterator();
    
    // From MGlxMediaListIterator    
    IMPORT_C void SetToFirst(const MGlxMediaList* aList);
    
    TInt operator++(TInt);
    
    TBool InRange(TInt aIndex) const;
	
private:   
 
    /**
    * The iterator whixh specifies ,which items to be included.
    */ 
    MGlxMediaListIterator& iIncludedItemsIterator;
    
    /**
    * The iterator which specifies ,which items should be excluded.,
    */
    MGlxMediaListIterator& iExcludedItemsIterator; 
    
	};

/**
 * TGlxFromManualIndexBlockyIterator
 * 
 * Ordered that returns the index of the visible window item first and 
 * then the surrounding items evenly from both sides
 */	
class TGlxFromManualIndexBlockyIterator : public MGlxMediaListIterator
	{
	
public:
	 IMPORT_C ~TGlxFromManualIndexBlockyIterator( );
	 IMPORT_C TGlxFromManualIndexBlockyIterator( );
	
	/**
	 * Set the range offsets
	 */
     IMPORT_C void SetRangeOffsets(TInt aRearOffset, TInt aFrontOffset);
    
    // From MGlxMediaListIterator
	virtual void SetToFirst(const MGlxMediaList* aList);
	virtual TInt operator++(TInt);
	virtual TBool InRange(TInt aIndex) const;

private:
	/**	
	 * The count or ordinal of the item that is currently "selected"
	 * This translates to how many times ++ has been called
	 */
	TInt iCurrentItem;

	/**
	 * Number of items within range after visible index
	 */
	TInt iFrontOffset;

	/**
	 * Number of items within range before visible index
	 */
	TInt iRearOffset;
    
    /**
     * Number of items within range as configured by clients
     * This shall be used to restore back 
     * while coming out of low memory conditions
     */
	TInt iOriginalFrontOffset;
	TInt iOriginalRearOffset;
	TInt iDefaultVisItems;
protected:	
	/**
	 * List being traversed
	 */
	const MGlxMediaList* iList;
	};
	
/**
 * TGlxScrollingDirectionIterator
 * 
 * Ordered that returns the index of the visible window item based on ScrollDirection
 */	
class TGlxScrollingDirectionIterator : public MGlxMediaListIterator
	{
	
public:
    
	 IMPORT_C ~TGlxScrollingDirectionIterator( );
	 IMPORT_C TGlxScrollingDirectionIterator( );
	
	/**
	 * Set the range offsets
	 */
     IMPORT_C void SetRangeOffsets(TInt aRearOffset, TInt aFrontOffset);
    
    // From MGlxMediaListIterator
	virtual void SetToFirst(const MGlxMediaList* aList);
	virtual TInt operator++(TInt);
	virtual TBool InRange(TInt aIndex) const;

private:
    
    enum TMovingDirection
        {
        EForward,
        EBackward,
        EIdle
        };
    
	/**	
	 * The count or ordinal of the item that is currently "selected"
	 * This translates to how many times ++ has been called
	 */
	TInt iCurrentItem;

	/**
	 * Number of items within range after visible index
	 */
	TInt iFrontOffset;

	/**
	 * Number of items within range before visible index
	 */
	TInt iRearOffset;
    
	TInt iDefaultVisItems;
	TMovingDirection iMovingDirection;
	TInt iExVisindex;
	
protected:	
	/**
	 * List being traversed
	 */
	const MGlxMediaList* iList;
	};
	
#endif // __C_GLXMEDIALISTITERATOR_H__

