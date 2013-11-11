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
* Description:  Factory for WAP items.
*
*/


#ifndef WPITEMFACTORY_H
#define WPITEMFACTORY_H

// INCLUDE FILES
#include <e32base.h>

// FORWARD DECLARATIONS
class MWPWAPItemBase;
class CWPCharacteristic;
class CCommsDatabase;
class RCmConnectionMethodExt;
class RCmDestinationExt;
class CBookmarkDb;

// CLASS DECLARATION
class RCmManagerExt;
/**
 * WPWAPItemFactory is a factory for all WAP items.
 *
 * @lib WPWAPAdapter
 * @since 2.0
 */ 
class WPWAPItemFactory
    {
    public:
        /**
        * Create an MMS item.
        * @param aTitle Title for MMS items
        * @param aDefaultName Default name for MMS items
        * @param aCharacteristic The APPLICATION characteristic
        * @param aCommsDb The CommsDB to use. Created if NULL.
        * @param aAPHandler The APEngine data handler to use. Created if NULL.
        * @return MWPWAPItemBase instance
        */
        static MWPWAPItemBase* CreateMMSLC( const TDesC& aTitle, 
            const TDesC& aDefaultName, 
            CWPCharacteristic& aCharacteristic, 
            CCommsDatabase*& aCommsDb, 
            RCmManagerExt*& aCmManager );


        /**
        * Create a browser item.
        * @param aFirst ETrue if this browser item is first
        * @param aTitle Title for browser items
        * @param aBookmarkTitle Title for bookmark items
        * @param aDefaultName Default name for browser items
        * @param aCharacteristic The APPLICATION characteristic
        * @param aDb The CommsDB to use. Created if NULL.
        * @param aAPHandler The APEngine data handler to use. Created if NULL.
        * @param aBookmarkDb The bookmark database to use. Created if NULL.
        * @return MWPWAPItemBase instance
        */
        /*static MWPWAPItemBase* CreateBrowserLC( TBool aFirst,
            const TDesC& aTitle, 
            const TDesC& aBookmarkTitle,
            const TDesC& aDefaultName, 
            const TDesC& aDefaultBookmarkName,
            CWPCharacteristic& aCharacteristic, 
            CCommsDatabase*& aCommsDb, 
            RCmManagerExt*& aCmManager,
            RFavouritesDb aBookmarkDb );
            
        /**
        * Create a bookmark item.
        * @param aTitle The title to be returned in Name().
        * @param aDefaultName The default name for bookmarks.
        * @param aResource The RESOURCE characteristic.
        * @param aLink Link to NAPDEF
        * @param aDb The bookmark database to use. Created if NULL.
        * @return MWPWAPItemBase instance
        */
        static MWPWAPItemBase* CreateBookmarkLC( const TDesC& aTitle, 
            const TDesC& aDefaultName, 
            CWPCharacteristic& aResource, 
            CWPCharacteristic*& aNapdef);

    };


#endif  // WPITEMFACTORY_H
            
// End of File
