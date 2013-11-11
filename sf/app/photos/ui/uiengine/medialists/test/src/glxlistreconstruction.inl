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
* Description:   Class that reconstructs a list from notifications
*
*/



// CONSTRUCTION

#include "glxmedia.h"

template < class _ListClass >
inline CGlxListReconstruction< _ListClass >::CGlxListReconstruction()
    {
    }

template < class _ListClass >
inline void CGlxListReconstruction< _ListClass >::SetList( _ListClass& aList )
    {
    iList = &aList;
    }
    
template < class _ListClass >
inline const TGlxMediaId& CGlxListReconstruction< _ListClass >::OriginalItem( TInt aIndex ) const    
    {
    return iList->Item( aIndex ).Id();
    }
        
template < class _ListClass >
inline TBool CGlxListReconstruction< _ListClass >::ItemsEqual() const
    {
    // test lengths match
    TBool fail = ( iList->Count() != iReconstructedList.Count() );
    // test all items are the same
    TInt i = 0;
    while ( i < iList->Count() && !fail )
        {
        fail |= ( iList->Item( i ).Id() == iReconstructedList[ i ] );
        i++;
        }
    return !fail;
    }
    
