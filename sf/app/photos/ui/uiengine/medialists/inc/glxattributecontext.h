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
* Description:    Generic fetch context to retrieve attributes
*
*/




#ifndef C_GLXATTRIBUTECONTEXT_H
#define C_GLXATTRIBUTECONTEXT_H

#include <e32std.h>
#include "mglxfetchcontext.h"
#include "glxmedialistiterator.h"	// for TGlxSequentialIterator

// Forward declarations
class MGlxMediaList;
class TMPXAttribute;
class T_CGlxAttributeContext; // For E-unit only

/**
 *  CGlxAttributeContext
 *
 *  Fetch context to retrieve thumbnails
 *
 *  @lib glxmedialists.lib
 */
class CGlxAttributeContext : public CBase, public MGlxFetchContext
	{
public:
	/**
	 * Constructor
	 * @param aIterator Iterator that determines the order of attribute 
	 *  	  retrieval and for which items attributes are and are not retrieved
	 */
	IMPORT_C CGlxAttributeContext(MGlxMediaListIterator* aIterator);
	IMPORT_C ~CGlxAttributeContext();

	/**
	 * Adds an attribute to be retrieved for all items
	 * @param aAttribute The attribute to be retrieved
	 */
	IMPORT_C void AddAttributeL(const TMPXAttribute& aAttribute);

	/**
	 * Removes an attribute from the "retrieval instructions"
	 * @param aAttribute The attribute to be removed 
	 */
	IMPORT_C void RemoveAttribute(const TMPXAttribute& aAttribute);

	/**
	 * Returns the count of the attributes to be retrieved
	 * @return the count of the attribtues to be retrieved
	 */
	IMPORT_C TInt AttributeCount();
	
	/**
	 * Sets granularity of the item index array returned from AttributeRequestL
	 */
	IMPORT_C void SetGranularity(TUint aGranularity);

	/**
	 * Sets the default fetch specification. 
	 * @param aWidth the width of required bitmap. 
	 * @param aHeight the heigth of required bitmap. 
	 */
	IMPORT_C void SetDefaultSpec(TInt aWidth, TInt aHeight);

public: // From MGlxFetchContext
    /// See @ref MGlxFetchContext::AttributeRequestL
    TInt AttributeRequestL(const MGlxMediaList* aList, RArray<TInt>& aItemIndices, 
    	RArray<TMPXAttribute>& aAttributes, CMPXAttributeSpecs*& aDetailedSpecs) const;

    /// See @ref MGlxFetchContext::AllAttributesL
    void AllAttributesL(const MGlxMediaList* aList, TInt aListIndex, 
    	RArray<TMPXAttribute>& aAttributes) const;

    /// See @ref MGlxFetchContext::RequestCountL
    TInt RequestCountL(const MGlxMediaList* aList) const;
    	
private:
	/**
	 * Determine attributes to request for an item
	 * @param aIndexInList item index in media list
	 * @param aList media list
	 * @param aAttributes requested attributes
	 * @return ETrue if attributes to be requested, otherwise EFalse
	 */
	TBool AddItemAttributesL(TInt aIndexInList, const MGlxMediaList* aList, RArray<TMPXAttribute>& aAttributes, 
	                         TInt& aError, TBool aFirstItem) const;

	/**
	 * Append attribute to array, no duplicates
	 * @param aAttributes requested attributes
	 * @param aAttribute attribute to be added
	 */
	void AddItemAttributeL(RArray<TMPXAttribute>& aAttributes, const TMPXAttribute& aAttribute) const;
	
private:
	/** Attributes to be requested for all items */
	RArray<TMPXAttribute> iAttributes;
	
	/**
	 * Iterator for traversing the list
	 */
	MGlxMediaListIterator* iIterator;	
	
	/** 
	 * Granularity of the item index array returned from AttributeRequestL
	 */
	TUint iGranularity;

	/** Size of the default spec */
	TSize iDefaultSpecSize;

	/** Tester class */
	friend class T_CGlxAttributeContext;
	};
	
/**
 *  CGlxDefaultAttributeContext
 *
 *  Fetch context to retrieve MPX attributes starting from focus outward
 *
 *  @lib glxmedialists.lib
 */
class CGlxDefaultAttributeContext : public CGlxAttributeContext
	{
public:
	IMPORT_C static CGlxDefaultAttributeContext* NewL();
	IMPORT_C ~CGlxDefaultAttributeContext();

	/**
	 * Set the range offsets. These determine the outer limits
	 * of attributes to be retrieved, compared to current focus in
	 * the list
	 * If ranges not defined, retrieves the attributes for focused item only
	 */
    IMPORT_C void SetRangeOffsets(TInt aFrontOffset, TInt aRearOffset);

private:
	CGlxDefaultAttributeContext();

private:
	TGlxFromFocusOutwardIterator iFromFocusIterator;	
	};
	
/**
 *  CGlxDefaultListAttributeContext
 *
 *  Fetch context to retrieve MPX attributes by using TGlxFromFocusOutwardListIterator
 *  iterator specially designed for list in 10.1
 *
 *  @lib glxmedialists.lib
 */
class CGlxDefaultListAttributeContext : public CGlxAttributeContext
    {
public:
    IMPORT_C static CGlxDefaultListAttributeContext* NewL();
    IMPORT_C ~CGlxDefaultListAttributeContext();

    /**
     * Set the range offsets. These determine the outer limits
     * of attributes to be retrieved, compared to current focus in
     * the list.
     * 
     * NOTE : This works on Visible WindowIndex and not FocusIndex
     * 
     */
    IMPORT_C void SetRangeOffsets(TInt aFrontOffset, TInt aRearOffset);

private:
    CGlxDefaultListAttributeContext();

private:
    TGlxFromVisibleIndexOutwardListIterator iFromVisibleIndexIterator;    
    };

#endif // C_GLXATTRIBUTECONTEXT_H

