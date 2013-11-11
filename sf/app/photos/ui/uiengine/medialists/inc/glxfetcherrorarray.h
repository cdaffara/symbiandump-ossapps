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
* Description:    Array of attribute retrieval errors
*
*/




#ifndef C_GLXFETCHERROR_H
#define C_GLXFETCHERROR_H

#include <e32base.h>
#include "glxfetcherror.h"

/**
 * CGlxFetchErrorArray
 *
 * Ordered array of attribute retrieval errors
 * @see TGlxFetchError
 *
 * @lib glxmedialists.lib
 */
NONSHARABLE_CLASS(CGlxFetchErrorArray) : public CBase
    {
public:
   /**
     * Constructor
     */
    CGlxFetchErrorArray();

    /**
     * Two-phase copy constructor
     * @param aErrorArray The existing CGlxFetchErrorArray to copy
     */
    static CGlxFetchErrorArray* NewL(const CGlxFetchErrorArray* aErrorArray);

    /**
     * Two-phase copy constructor
     * @param aErrorArray The existing CGlxFetchErrorArray to copy
     */
    static CGlxFetchErrorArray* NewLC(const CGlxFetchErrorArray* aErrorArray);

    /**
     * Destructor
     */    
    ~CGlxFetchErrorArray();

    /**
     * Number of errors stored in array
     * @return Number of errors stored in array
     */
    TInt ErrorCount() const;
    
    /**
     * The error at a particular index
     * @param aIndex The index for the error
     * @return The error
     */
    TGlxFetchError Error(TInt aIndex) const;

    /**
     * Add an error to the array
     * @param aError Error to add
     */
    void AddErrorL(TGlxFetchError aError);
    
    /**
     * Remove an error from the array
     * @param aIndex index of the error to remove
     */
    void Remove( TInt aIndex );
    
    /**
     * Find an error given the attribute
     * @param aAttr The attribute to look for
     * @return The error code, or KErrNone if none
     */
    TInt FindError(TMPXAttribute aAttr) const;
    
    /**
     * Find an error given the content id
     * @param aContentId The content id to look for
     * @return The error code, or KErrNone if none
     */
    TInt FindError(TInt aContentId) const;
    
private:
    /**
     * Second-phase copy constructor
     * @param aErrorArray The existing CGlxFetchErrorArray to copy
     */
    void CopyConstructL(const CGlxFetchErrorArray* aErrorArray);
    
    /**
     * Ordering function for array
     * @see TLinearOrder
     */
    static TInt ErrorOrderByKey(const TMPXAttribute* aAttr, const TGlxFetchError& aItem2);

    /**
     * Ordering function for array
     * @see TLinearOrder
     */
    static TInt ErrorOrderById(const TGlxFetchError& aItem1, const TGlxFetchError& aItem2); 
    
private:
    /// Ordered array of errors
    RArray<TGlxFetchError> iFetchErrors;
    };
    
#endif // C_GLXFETCHERROR_H
