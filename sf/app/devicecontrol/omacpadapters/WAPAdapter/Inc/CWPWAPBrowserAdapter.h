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
* Description:  Handles browser settings in provisioning.
*
*/


#ifndef CWPWAPBROWSERADAPTER_H
#define CWPWAPBROWSERADAPTER_H

// INCLUDES
#include "CWPWAPAdapterBase.h"

#include <favouritessession.h>
#include <FavouritesDb.h>


// FORWARD DECLARATIONS
class CWPCharacteristic;
class CBookmarkDb;
// CLASS DECLARATION

/**
 * CWPWAPBrowserAdapter handles browser settings.
 *
 * @lib WPWAPAdapter
 * @since 2.0
 */ 
class CWPWAPBrowserAdapter : public CWPWAPAdapterBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @return CWPWAPBrowserAdapter instance
        */
        static CWPWAPBrowserAdapter* NewL();

        /**
        * Destructor.
        */
        ~CWPWAPBrowserAdapter();

    public: // from CWPWAPAdapterBase
        
        void ApplicationL( CWPCharacteristic& aCharacteristic );
        void DeleteL( const TDesC8& aSaveData );
        
    private:  // New functions
        /**
        * C++ default constructor.
        */
        CWPWAPBrowserAdapter();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Create the bookmarks db.
        */
        void CreateDbL();

    private:    // Data
        // Title for browser items. Owns.
        HBufC* iTitle;

        // Title for bookmarks. Owns.
        HBufC* iBookmarkTitle;

        // Default name for browser items. Owns.
        HBufC* iDefaultName;

        // Default name for bookmarks. Owns.
        HBufC* iDefaultBookmarkName;

        // ETrue if one browser application has been processed.
        TBool iProcessed;
        
        // Favourites database. Owns.
        CBookmarkDb* iBookmarkDb;        

        RFavouritesSession iSession;
        
        RFavouritesDb iFavouritesDb;
    };

#endif  // CWPWAPBROWSERADAPTER_H
            
// End of File
