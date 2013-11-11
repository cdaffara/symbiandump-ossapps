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



#include "_glxlisttesthelpers.h"

namespace NGlxListTestHelpers
    {
    // -----------------------------------------------------------------------------
    // Constructor
    // -----------------------------------------------------------------------------
    //
    TStaticItemInfo::TStaticItemInfo( const TStaticItemInfo::TData aInfoData ) 
        {
        iData.iId = aInfoData.iId;
        iData.iPosition = aInfoData.iPosition;
        }        
    // -----------------------------------------------------------------------------
    // Operator ==
    // -----------------------------------------------------------------------------
    //
    TBool TStaticItemInfo::operator==( const TStaticItemInfo::TData aInfoData ) const
        {
        return iData.iId == aInfoData.iId
            && iData.iPosition == aInfoData.iPosition;
        }
        
    // -----------------------------------------------------------------------------
    // Operator !=
    // -----------------------------------------------------------------------------
    //
    TBool TStaticItemInfo::operator!=( const TStaticItemInfo::TData aInfoData ) const
        {
        return !( (*this) == aInfoData );
        }
    } // NGlxListTest
