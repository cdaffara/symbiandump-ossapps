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
* Description:   Unit tests
*
*/



#ifndef __GLXLISTTESTHELPERS_H__
#define __GLXLISTTESTHELPERS_H__

//  EXTERNAL INCLUDES
#include <e32std.h>
#include <glxlistdefs.h>

//  INTERNAL INCLUDES

namespace NGlxListTestHelpers
    {
    struct TStaticItemInfo
        {
        // Data needs to be a simple struct with no member functions, to allow 
        // arrays to be easily declared 
        struct TData
            {
            TChar iId;
            NGlxListDefs::TInsertionPosition iPosition;
            };
            
        TStaticItemInfo( const TStaticItemInfo::TData aData );
        TBool operator==( const TStaticItemInfo::TData aInfoData ) const;
        TBool operator!=( const TStaticItemInfo::TData aInfoData ) const;
        TData iData;
        };
        
    #define K_STATIC_ITEMS_END 0, static_cast<NGlxListDefs::TInsertionPosition>( -1 )
    const TStaticItemInfo::TData KEndMarkerData = { K_STATIC_ITEMS_END };
    const TStaticItemInfo KEndMarker = KEndMarkerData;    
    } // NGlxListTest

#endif      //  __GLXLISTTESTHELPERS_H__
