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
* Description:    Media and attribute fetch context
*
*/




#ifndef M_GLXFETCHCONTEXT_H
#define M_GLXFETCHCONTEXT_H

#include <e32base.h>
#include "glxmediaid.h"

#include <mpxattributespecs.h>

class MGlxMediaList;
class TMPXAttribute;

/**
 *  MGlxFetchContext
 *
 *  Interface that allows clients to define which media items should be 
 *  retrieved and which properties for them
 *
 *  @lib none
 */
class MGlxFetchContext 
    {
public:
    /**
     * Specifies which attributes should be fetched for a particular item
     * in the list.
     * @param aList the list for which the request relates to
     * @return in aItemIndices the list indices for which the attributes should be requested for
     *         if no items requiring attributes, returns an empty array.
     *         Caller should make sure the array is empty before calling.
     * @return in aAttributes the attributes that should be retrieved
     * @return in aDetailedSpecs a new detailed specification object 
     *		   regarding the attributes or NULL. Caller must set the 
     *		   parameter to NULL.
     * @return TInt number of items with outstanding requests.
     *     KErrGeneral if there are no more requests but there are outstanding errors.
     */
    virtual TInt AttributeRequestL(const MGlxMediaList* aList, RArray<TInt>& aItemIndices, 
    	RArray<TMPXAttribute>& aAttributes, CMPXAttributeSpecs*& aDetailedSpecs) const = 0;
    
    /**
     * Lists all attributes that a required for a particular item in the list.
     * Ignores whether the attributes have been loaded or not. This function 
     * is used for garbage collection, in order to know if an attribute is 
     * still in use. 
     * @param aList the list for which the call relates to
     * @param aListIndex the index in the list of the particular item
     * @return in aAttributes all attributes that this context requires.
     *		   if no attributes are required, returns the array as empty.
     *	       Caller must make sure the array is empty before calling.
     */
    virtual void AllAttributesL(const MGlxMediaList* aList, TInt aListIndex, 
    	RArray<TMPXAttribute>& aAttributes) const = 0;

    /**
     * Provides the number of items with currently outstanding requests
     *
     * @param aList the list for which the call relates to
     * @return TInt number of items with outstanding requests.
     *     KErrGeneral if there are no more requests but there are outstanding errors.
     */
    virtual TInt RequestCountL(const MGlxMediaList* aList) const = 0;
    };

#endif // M_GLXFETCHCONTEXT_H