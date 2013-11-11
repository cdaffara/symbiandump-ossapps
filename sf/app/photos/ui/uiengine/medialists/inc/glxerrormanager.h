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
* Description:    Utility class to manage attribute retrieval errors
*
*/



#ifndef _GLXERRORMANAGER_H
#define _GLXERRORMANAGER_H

#include <e32base.h>
#include "glxfetcherror.h"

// Forward declaration
class CGlxMedia;

/// How long temporary errors remain valid for until they can be checked again
/// @todo Find optimal value for this
const TInt KGlxTemporaryErrorValidityPeriodInSeconds = 30;

/**
 * GlxErrorManager
 *
 * Utility class for handling the error attribute that may be attached to items
 *
 * @lib glxmedialists.lib
 */
class GlxErrorManager
    {
public:    
    /**
     * Check whether an error has been recorded against the given attribute
     * @param aItem The item to check
     * @param aAttribute The attribute to check
     * @return The error associated with the attribute; KErrNone if none
     */
    IMPORT_C static TInt HasAttributeErrorL(const CGlxMedia* aItem, const TMPXAttribute& aAttribute);
    
    /**
     * Check whether an error has been recorded against the given attribute
     * @param aItem The item to check
     * @param aContentId The content id for attribute to check
     * @return The error associated with the attribute; KErrNone if none
     */
    IMPORT_C static TInt HasAttributeErrorL(const CGlxMedia* aItem, TInt aContentId);

    /**
     * Heuristic to determine whether errors may exist within an item.
     * @return EFalse, if there are no errors; ETrue if there might be errors (includes possibly expired errors)
     */
    static TBool HasError(const CGlxMedia* aItem);

    /**
     * Sets error values for the given attributes
     * @param aItem The item to add the errors to
     * @param aAttributes The attributes affected
     * @param aError The error code to set against the attributes
     */
    static void SetAttributeErrorL(CGlxMedia* aItem, const RArray<TMPXAttribute>& aAttributes, TInt aError);
    
    /**
     * Performs some maintainance on the error attribute of an item.
     * - Removes any expired temporary errors
     * - Removes any errors for attributes that don't occur in aAttributesInUse
     * @param aMedia Media object to clean
     * @param aAttributesInUse Superset of all attributes that the item may hold. 
     */
    static void ClearExpiredAndUnusedErrorsL( CGlxMedia& aMedia, 
        const RArray<TMPXAttribute>& aAttributesInUse );
    
    /**
     * @return the attribute used to store the error list
     */
    static TMPXAttribute ErrorAttribute();

private:
    /**
     * Check whether a given error code is classed as "temporary"
     * @param aError Error code to check
     * @return ETrue iff aError is temporary
     */
    static TBool IsTemporaryError(TInt aError);
    
    /**
     * Given an error, check if it has expired. 
     * @param aError The error to check
     * @return Whether the error has expired
     */
    static TBool IsExpired( const TGlxFetchError& aError );
    };

#endif // _GLXERRORMANAGER_H

