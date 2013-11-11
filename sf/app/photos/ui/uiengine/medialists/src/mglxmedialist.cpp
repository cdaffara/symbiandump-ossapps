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
* Description:    Reader interface of media item lists
*
*/




#include "mglxmedialist.h"
#include "glxmedialist.h"
#include <glxtracer.h>

// -----------------------------------------------------------------------------
// Returns a new/existing media list interface
// -----------------------------------------------------------------------------
EXPORT_C MGlxMediaList* MGlxMediaList::InstanceL(const CMPXCollectionPath& aPath, 
        const TGlxHierarchyId& aHierarchyId, CMPXFilter* aFilter)
    {
    TRACER("MGlxMediaList::InstanceL");
            
    return CGlxMediaList::InstanceL(aPath, aHierarchyId, aFilter);
    }
// -----------------------------------------------------------------------------
// Unmark all medialist items.
// -----------------------------------------------------------------------------
EXPORT_C void MGlxMediaList::UnmarkAllL( MGlxMediaList& aList )
	{
	TRACER("MGlxMediaList::UnmarkAllL");
	
	TInt count = aList.Count(); 
		 
  	for ( TInt i = 0; i <= count ; i++ )
  		{
		aList.SetSelectedL( i, EFalse );
  		}
	}

