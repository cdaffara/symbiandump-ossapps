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
* Description:    List utilities
*
*/




#ifndef __GLXLISTUTILS_H__
#define __GLXLISTUTILS_H__

#include <e32std.h>

// constants
const TUint KGlxDefaultVisibleItemsGranularity = 15;
const TUint KGlxQHDVisibleItemsGranularity = 20;
const TUint KGlxVGAVisibleItemsGranularity = 21;

/**
 *  GlxListUtils
 *
 *  @author Aki Vanhatalo
 *
 * @internal reviewed 16/07/2007 by Kimmo Hoikka
 */
class GlxListUtils
	{
public:
	/** 
	 * Normalises index in a loop into the range. If aIndex is outside the 
	 * list indexes (i.e., 0 and list size), this function "loops the list"
	 * to return the correct index normalised to inside the list.
	 * This function must not be called if the list is empty.
     * @param aIndex Index to normalise
     * @param aListLength Length of the list
	 */
	static TInt NormalizedIndex( TInt aIndex, TInt aListLength );

    /**
     * Returns Visible items granularity based on feature 
     * layout (i.e., qHD, VGA)  
     * @return TInt visible items granularity / count
     */
	static TInt VisibleItemsGranularityL();
	};

#endif // __GLXLISTUTILS_H__

