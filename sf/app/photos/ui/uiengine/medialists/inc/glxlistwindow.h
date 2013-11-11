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
* Description:    Window to a list of objects
*
*/




#ifndef C_GLXLISTWINDOW_H
#define C_GLXLISTWINDOW_H

#include <e32base.h>


/**
 * Interface to get, setup and cleanup objects that go into a list window
 * Not strictly a factory, as also handles cleanup
 */
class MGlxWindowObjectFactory
    {
public:
	/**
     * Creates an object that the list window will contain
     * (not called NewObjectL since signature would be too similar to CGlxListWindowBase::NewObjectL)
	 * @return an object to be put into the window
	 */
	virtual CBase* CreateObjectL() const = 0;
	
	/**
	 * Makes the object at the index ready for use
	 * This is called when the object is added to the window
	 */
	virtual void SetupObject( TInt aListIndex, CBase& aObject ) = 0;
	
	/**
	 * Makes the object at the index ready for removal
	 * If your object builds links from other objects to it, use this
	 * function to remove those links. The object will be removed from 
	 * the window after this function returns, and may be deleted or reused.
	 */
	virtual void CleanupObject( TInt aListIndex, CBase& aObject ) = 0;
    };

class CGlxDataWindow;

/**
 * Iterator that iterates through the items in a window 
 */
class TGlxWindowIterator     
    {
public:
    /** Constructor. Not exported since CGlxDataWindow is internal. */
    TGlxWindowIterator( const CGlxDataWindow& aWindow );
    /** Copy constructor */
    IMPORT_C TGlxWindowIterator( const TGlxWindowIterator& aIterator );
    /** 
     * @return the current item index in the list and move to next
     *         or KErrNotFound if has reached the end of the list
     */
    IMPORT_C TInt operator++( TInt );    
    
private: 
    /// window being iterated
    const CGlxDataWindow* iWindow;
    /// current position of iteration
    TInt iWindowIndex;
    };
    
/**
 *  CGlxListWindowBase
 */
class CGlxListWindow : public CBase 
	{
public:
    /** Constructor */
	IMPORT_C CGlxListWindow( MGlxWindowObjectFactory& aObjectFactory );
    /** Second-phase constructor */
	IMPORT_C void ConstructL();
    /** Destructor */
	IMPORT_C ~CGlxListWindow();

	/**
	 * Cleans up the objects, so that the list window can be deleted
	 * If the objects contained in the window do not build any links
	 * from other objects to them, this function does not have to be called.
	 * In this case also your implementation of CleanupObject can be empty.
 	 *
	 * Depending on usage, may not be necessary to call on destruction
	 * of the window's owner (e.g., when the whole app is being closed,
	 * removing links to other objects is usually unnecessary. However,
	 * if you are removing one list window but leaving others, then
	 * the list window should be cleaned up, assuming your implementation 
	 * of CleanupObject has some code in it.)
     *
     * Cannot be called by the destructor of this class, as long as deriving
     * classes implement MGlxWindowObjectFactory, since vtable would not
     * be valid in the destructor 
	 */
	IMPORT_C void CleanupL();

	/**
	 * Updates range offset. The range is the span of indexes that defined 
	 * which objects should exists as actual objects in the window (and which 
	 * should be abstract in the form of indexes). The range is calculated
	 * as a number (aFrontOffset) of objects in the front of the focus index
	 * and as a number (aRearOffset) of objects after the focus index.
	 */
	IMPORT_C void SetRangeOffsetsL( TInt aFrontOffset, TInt aRearOffset ); /// @todo depricated
	IMPORT_C void SetRangeOffsetsL( TInt aFocusIndex, TInt aTotalSize,
	    TInt aFrontOffset, TInt aRearOffset );

    /** @return an iterator that allows going through the items in the window */
    IMPORT_C TGlxWindowIterator Iterator() const;
	
 	/**
 	 * Sets the focus. 
 	 * @param aFocusIndex index of the object to be focused in the list
 	 */
	/* @todo depricated */ IMPORT_C void SetFocusIndexL( TInt aFocusIndex ); 
	IMPORT_C void SetFocusIndexL( TInt aFocusIndex, TInt aTotalSize );

	/**
	 * Adds objects to the list. Updates window if necessary.
	 * The function assumes that the underlying data structure has
	 * already changed.
	 */
	/* @todo depricated */ IMPORT_C void AddObjectsL( TInt aFirstNewIndex, TInt aLastNewIndex ); 
	IMPORT_C void AddObjectsL( TInt aFocusIndex, TInt aTotalSize,
	    TInt aFirstNewIndex, TInt aLastNewIndex );
	
	/**
	 * Removes objects from the list. Updates window if necessary.
	 * The function assumes that the underlying data structure has
	 * already changed.
	 */
	/* @todo depricated */ IMPORT_C void RemoveObjectsL( TInt aFirstRemovedIndex, TInt aLastRemovedIndex ); 
	IMPORT_C void RemoveObjectsL( TInt aFocusIndex, TInt aTotalSize, 
	    TInt aFirstRemovedIndex, TInt aLastRemovedIndex );
	
	/**
	 * Access to objects in window
	 * For iteration through items in the window use Iterator()
	 * @return a pointer to an object or NULL if not found
	 */	
	IMPORT_C CBase* At( TInt aIndex );
	IMPORT_C const CBase* At( TInt aIndex ) const;

public:    
    /** Range of the window in the list */
    struct TRange
        {
        TInt iStartIndex; 
        TInt iLength;
        };
        
private:
    /** Type of chance that happened to the full list */
	enum TChangeType 
		{
		EChangeNone,            
		EChangeObjectsAdded,    
		EChangeObjectsRemoved   
		};
        
    /** Class to describe a change to the full list */
    class TChange // declare struct as class to avoid CodeScanner warning...
        {   
    public:
        /** Constructor */
        TChange( TInt aNewFocusIndex, TInt aNewTotalSize, TChangeType aChangeType, 
            TInt aFirstChangedIndex, TInt aLastChangedIndex );
            
        TInt iNewFocusIndex;
        TInt iNewTotalSize;
        TChangeType iChangeType;
        /// first index added or removed, depending on iChangeType
        TInt iFirstChangedIndex; 
        /// last index added or removed, depending on iChangeType
        TInt iLastChangedIndex;
        };

    /** Update the window based on the change */
    void UpdateL( const TChange& aChange );
    /** Move objects from main window to working window, or object pool if not needed */
    void PopulateExistingAndUnuseOldL( const TChange& aChange );
    /** Populate main window with items from object pool */
    void PopulateNew();
    /** Make working window the main window and vice versa */
    void SwapWindows();
    /** @return an object from the object pool */
    CBase* UnusedObject();
    /** 
     * @return the index of the item at aIndex, when adjusted by the change, so 
     *         that index keeps pointing to the same logical item.
     */
    TInt IndexAfterChange( TInt aIndex, const TChange& aChange ) const;
        
	/** @return the first and last index of the range range */
    TRange Range( const TChange& aChange ) const;
	
private:
	/**
	 * Position of the focus 
     * If not defined (=list empty) value is -1 
	 */
    /// @todo remove as soon as possible, this is causing the focus change logic on remove/addition
    /// to be specified in media list AND here
	TInt iFocusIndex;	
	
	/**
	 * Front offset to focus for where window will start
	 * The window will start at position iFocusIndex + iFrontOffset
	 * (Although it may loop around the start and end of the list)
	 * Hence, iFrontOffset must be negative or zero
	 */
	TInt iFrontOffset;

	/**
	 * Front offset to focus for where window will end
	 * The window will end at position iFocusIndex + iRearOffset
	 * (Although it may loop around the start and end of the list)
	 * Hence, iRearOffset must be positive or zero
	 */
	TInt iRearOffset;

    /// Main window 
    CGlxDataWindow* iWindow;
    /// Working window, used when the list is being updates
    CGlxDataWindow* iWorkingWindow;
    /// Unused objects pool
    RPointerArray< CBase > iUnusedObjects;

    /// Factory that creates, sets up and cleans up objects that are stored in the window
    MGlxWindowObjectFactory& iObjectFactory;
    
    __DECLARE_TEST;
	};

#endif // C_GLXLISTWINDOW_H

