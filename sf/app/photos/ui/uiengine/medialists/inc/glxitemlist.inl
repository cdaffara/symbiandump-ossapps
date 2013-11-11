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
* Description:    List of media items
*
*/




#ifndef __C_GLXITEMLIST_INL__
#define __C_GLXITEMLIST_INL__

// -----------------------------------------------------------------------------
// Return id space id
// -----------------------------------------------------------------------------
//
inline const TGlxIdSpaceId& CGlxItemList::IdSpaceId() const
    {
    return iIdSpaceId;
    }
      
// -----------------------------------------------------------------------------
// Return count
// -----------------------------------------------------------------------------
//
inline TInt CGlxItemList::Count() const
    {
    return iItems.Count();
    }

// -----------------------------------------------------------------------------
// Return item by index
// -----------------------------------------------------------------------------
//
inline TGlxMedia& CGlxItemList::Item( TInt aIndex )
    {
    return iItems[ aIndex ];
    }
    
// -----------------------------------------------------------------------------
// Return item by index
// -----------------------------------------------------------------------------
// 
TGlxMedia& CGlxItemList::operator[]( TInt aIndex ) 
    {
    return iItems[ aIndex ];
    }
        
#endif // __C_GLXITEMLIST_INL__
