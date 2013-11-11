/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Cache test helper class
*
*/



#ifndef __T_CACHETESTHELPERS_H__
#define __T_CACHETESTHELPERS_H__

//  EXTERNAL INCLUDES

//  INTERNAL INCLUDES
#include "glxcache.h"

//  FORWARD DECLARATIONS
class CGlxMedia;

//  CLASS DEFINITION
/**
 * Helper class for testing cache
 *
 */
NONSHARABLE_CLASS( T_CacheTestHelpers )
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        T_CacheTestHelpers(CGlxCache* aCache) : iCache(aCache) {};
        /**
         * Destructor
         */
        ~T_CacheTestHelpers() {};

    public:

        RPointerArray<CGlxMedia>& ItemPool() const { return iCache->iItemPool; };
        static TInt MediaItemOrderByKey(const TGlxMediaId* aMediaId, const CGlxMedia& aItem2) { return CGlxCache::MediaItemOrderByKey(aMediaId, aItem2); };

    private:    // Data
		
        CGlxCache* iCache;
    };

#endif      //  __T_CACHETESTHELPERS_H__

// End of file
