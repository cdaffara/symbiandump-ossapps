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
* Description:  Setting item for browser settings.
*
*/


#ifndef CWPWAPBROWSERITEM_H
#define CWPWAPBROWSERITEM_H

// INCLUDES
#include <e32base.h>
#include <MWPVisitor.h>
#include <favouritesdb.h>
#include "CWPWAPItemBAse.h"

// FORWARD DECLARATIONS
class MWPWAPItemBase;
class CBookmarkDb;
class RCmManagerExt;
// CLASS DECLARATION

/**
 * CWPWAPBrowserItem handles one browser setting.
 *
 * @lib WPWAPAdapter
 * @since 2.0
 */ 


class CWPWAPBrowserItem : public CWPWAPItemBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aFirst ETrue if this browser item is first
        * @param aTitle Title for browser items
        * @param aBookmarkTitle Title for bookmark items
        * @param aDefaultName Default name for browser items
        * @param aDefaultBookmarkName Default name for bookmarks.
        * @param aCharacteristic The APPLICATION characteristic
        * @param aCommsDb The CommsDB to use. Created if NULL.
        * @param aAPHandler The APEngine data handler to use. Created if NULL.
        * @param aBookmarkDb The bookmark database to use. Created if NULL.
        * @return CWPWAPBrowserItem instance
        */
        static CWPWAPBrowserItem* NewL( TBool aFirst,
            const TDesC& aTitle, 
            const TDesC& aBookmarkTitle,
            const TDesC& aDefaultName, 
            const TDesC& aDefaultBookmarkName,
            CWPCharacteristic& aCharacteristic, 
            CCommsDatabase*& aCommsDb, 
            RCmManagerExt*& aCmManager           
            );

        /**
        * Destructor.
        */
        ~CWPWAPBrowserItem();

    public:   // From CWPMMSItemBase

        TBool CanSetAsDefault() const;
        void SaveL();
        void SetAsDefaultL();
        TBool ValidateL();
        void ResourceL( CWPCharacteristic& aResource );
        void AddL( RPointerArray<MWPWAPItemBase>& aShownItems, 
            RPointerArray<MWPWAPItemBase>& aHiddenItems );

    private:  // New functions
        /**
        * C++ default constructor.
        * @see CWPWAPBrowserItem::NewL() for description of parameters.
        */
        CWPWAPBrowserItem( TBool aFirst,
            const TDesC& aTitle, 
            const TDesC& aBookmarkTitle,
            const TDesC& aDefaultName, 
            const TDesC& aDefaultBookmarkName,
            CWPCharacteristic& aCharacteristic, 
            CCommsDatabase*& aCommsDb, 
            RCmManagerExt*& aCmManager,            
            );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:
        // The bookmarks. Owns.
        RPointerArray<MWPWAPItemBase> iBookmarks;

        // The default name for bookmarks.
        const TDesC& iDefaultBookmarkName;

        // The title for bookmarks
        const TDesC& iBookmarkTitle;
        
        //RFavouritesDb iDb;

        // ETrue if this browser item is first in list.
        TBool iFirst;
    };

#endif  // CWPWAPBROWSERITEM_H
            
// End of File
