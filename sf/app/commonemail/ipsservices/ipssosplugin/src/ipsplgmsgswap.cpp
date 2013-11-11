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
* Description:  Swapper class for message sorting function
*
*/


#include "emailtrace.h"
#include "ipsplgheaders.h"
// ---------------------------------------------------------------------------
// Basic constructor
// ---------------------------------------------------------------------------

TIpsPlgMsgSwap::TIpsPlgMsgSwap( CMsvEntry& aFolderEntry )
    : iFolderEntry(aFolderEntry)
    {
    FUNC_LOG;
    // none
    }

// ---------------------------------------------------------------------------
// Swaps the messages inside the folder CMsvEntry instance. 
// This is somewhat extreme solution assuming that CMsvEntry returns
// references to the actual TMsvEntries (not copies) and that CMsvEntry
// does not do any sorting itself.
// ---------------------------------------------------------------------------

void TIpsPlgMsgSwap::Swap( TInt aLeft, TInt aRight ) const
    {
    FUNC_LOG;
    if ( aLeft == aRight )
        {
        return;
        }
        
    TInt childCount( iFolderEntry.Count() );
    if ( aLeft >= childCount || aRight >= childCount  )
        {
        return;
        }
        
    Mem::Swap( (TAny*) &( iFolderEntry[aLeft] ), 
        (TAny*) &( iFolderEntry[aRight] ), sizeof( TMsvEntry ) );
    }


