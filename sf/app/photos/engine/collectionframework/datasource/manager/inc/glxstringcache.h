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
* Description:   This class caches resource strings
*
*/



#ifndef _GLXSTRINGCACHE_H_
#define _GLXSTRINGCACHE_H_

// INCLUDES
#include <e32cmn.h>
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION

/**
 *  CGlxStringCache class 
 *
 *  This class caches resource strings.
 *  It maintains a mapping between the resource id and the string
 *  @author Rowland Cook
 */
NONSHARABLE_CLASS(CGlxStringCache) : public CBase
    {
public:
	/** 
	 * Two-phase constructor
	*/
	static IMPORT_C CGlxStringCache* NewL();	
    
	/**
	 * destructor
	 */
    ~CGlxStringCache();    
	
    /**
	 * Loads a string from a resurce file
	 *
	 * @param aResourceFile - name of the resource file (does not contain the path).
	 * @param aResourceId - Id of the string to be loaded
	 * @return HBufC* containing the required string.
	 */	
    IMPORT_C HBufC* LoadLocalizedStringLC(const TDesC& aResourceFile, const TInt aResourceId);
    
private:
    /**
     *  CGlxStringItem class 
     *
     *  This class encapsulates a resource id and it's associated string.
     */
    NONSHARABLE_CLASS(CGlxStringItem) : public CBase
        {
    public:
    	/**
    	 * NewL
    	 * @param the resource id
    	 * @param the string
    	 * use this constructor prior to calling InsertL() (Above).
    	 */
        static CGlxStringItem* NewL(TInt aId, HBufC* aString);
        
    	/**
    	 * Constructor
    	 * @param the resource id
    	 * use this constructor directly to create a dummy class typically used before calling Find() (Above).
    	 */
        CGlxStringItem(TInt aId);
        
    	/**
    	 * destructor
    	 */
        ~CGlxStringItem();
        
    	/**
    	 * GetId
    	 * @return the Id
    	 */
        TInt GetId() const;
        
    	/**
    	 * GetString
    	 * @return HBufC* to the string
    	 */
        HBufC* GetString();
        
    private:    
    	/**
    	 * 2nd Phase constructor
    	 * @param the string. Creates own copy of string
    	 */
        
        void ConstructL(HBufC* aString);


    private:
        /// The resource id of the string
        TInt    iId;
        
        /// HBufC containing the string (Owned)
        HBufC*  iString;
        };
private:
	/**
	 * Default Constructor
	 */
    CGlxStringCache();
    
	/**
	 * Searches for a string given the resource id
	 * @param - resource id
	 * @return - an HBufC* to the string. NULL if not found. Ownership of string passed to caller.
	 */
    HBufC* FindL(TInt aId);
    
	/**
	 * Inserts a string and resource id into the cache
	 * @param - resource id
	 * @return - an HBufC* to the string. NULL if not found. Ownership of string passed to caller.
	 */    
    void InsertL(TInt aId, HBufC* aString);
    /**
     * Count
     * @return number of elements in array
     */ 
    TInt Count();
    
    /**
     * This method is a callback passed to RPointerArray.InsertInOrder
     * @ref see TLinearOrder.
     * @param first item to be compared
     * @param second item to be compared
     * @return a negative value is Item1 is less than Item2. Zero if Equal, otherwise a positive value
     */
    static TInt OrderById(const CGlxStringCache::CGlxStringItem& aItem1, const CGlxStringCache::CGlxStringItem& aItem2);

    /**
     * This method is a callback passed to RPointerArray.Find
     * @ref see TIdentityRelation.
     * @param first item to be compared
     * @param second item to be compared
     * @return ETrue id Item1 matches Item2, otherwise EFalse
     */
    static TBool MatchById(const CGlxStringCache::CGlxStringItem& aItem1, const CGlxStringCache::CGlxStringItem& aItem2);
    
    /**
	 * Loads a string from a resurce file
	 *
	 * @param aResourceFile - name of the resource file (does not contain the path).
	 * @param aResourceId - Id of the string to be loaded
	 * @return HBufC* containing the required string.
	 */	
	HBufC* LoadLocalizedStringFromDriveL(const TDesC& aResourceFile, const TInt aResourceId);

private:
    /// Array of CGlxStringItems
    RPointerArray<CGlxStringItem>   iStringCache;
    
    // remembers the valid resource file name
    TFileName           iResourceFileName;
    };


#endif