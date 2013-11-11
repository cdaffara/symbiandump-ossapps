/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  WPWAPItemFactory creates WAP settings items.
*
*/



// INCLUDE FILES
#include "WPWAPItemFactory.h"
#include "CWPWAPMMSItem.h"
//#include "CWPWAPBrowserItem.h"
#include "CWPWAPBookmarkItem.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// WPWAPItemFactory::CreateMMSL
// -----------------------------------------------------------------------------
//
MWPWAPItemBase* WPWAPItemFactory::CreateMMSLC( const TDesC& aTitle, 
            const TDesC& aDefaultName, 
            CWPCharacteristic& aCharacteristic, 
            CCommsDatabase*& aCommsDb, 
            RCmManagerExt*& aCmManager )
    {
    CWPWAPMMSItem* item = CWPWAPMMSItem::NewL( aTitle, aDefaultName, 
        aCharacteristic, aCommsDb, aCmManager );
    CleanupStack::PushL( item );
    return item;
    }

// -----------------------------------------------------------------------------
// WPWAPItemFactory::CreateBrowserL
// -----------------------------------------------------------------------------
//
/*MWPWAPItemBase* WPWAPItemFactory::CreateBrowserLC( TBool aFirst,
            const TDesC& aTitle, 
            const TDesC& aBookmarkTitle,
            const TDesC& aDefaultName, 
            const TDesC& aDefaultBookmarkName,
            CWPCharacteristic& aCharacteristic, 
            CCommsDatabase*& aCommsDb, 
            RCmManagerExt*& aCmManager,
            RFavouritesDb aBookmarkDb )
    {
    CWPWAPBrowserItem* item = CWPWAPBrowserItem::NewL( aFirst, aTitle, aBookmarkTitle, 
        aDefaultName, aDefaultBookmarkName, aCharacteristic, aCommsDb, aCmManager, aBookmarkDb );
    CleanupStack::PushL( item );
    return item;
    }
*/
// -----------------------------------------------------------------------------
// WPWAPItemFactory::CreateMMSL
// -----------------------------------------------------------------------------
//
MWPWAPItemBase* WPWAPItemFactory::CreateBookmarkLC( const TDesC& aTitle, 
            const TDesC& aDefaultName, 
            CWPCharacteristic& aResource, 
            CWPCharacteristic*& aLink
            )
    {
    CWPWAPBookmarkItem* item = CWPWAPBookmarkItem::NewL( aTitle, 
        aDefaultName, aResource, aLink);
    CleanupStack::PushL( item );
    return item;
    }

//  End of File  
