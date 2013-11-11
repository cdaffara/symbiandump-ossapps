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
* Description:  One bookmark item in browser settings.
*
*/


#ifndef CWPWAPBOOKMARKITEM_H
#define CWPWAPBOOKMARKITEM_H

// INCLUDES
#include <e32base.h>
#include <MWPVisitor.h>

#include "CWPWAPItemBAse.h"


// FORWARD DECLARATIONS
class CBookmarkDb;

// CLASS DECLARATION

/**
 * CWPWAPBookmarkItem handles one bookmark item in browser settings.
 *
 * @lib WPWAPAdapter
 * @since 2.0
 */ 
class CWPWAPBookmarkItem : public CBase, public MWPWAPItemBase, private MWPVisitor
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aTitle The title to be returned in Name().
        * @param aDefaultName The default name for bookmarks.
        * @param aCharacteristic The RESOURCE characteristic.
        * @param aLink Link to NAPDEF
        * @param aDb The bookmark database to use. Created if NULL.
        * @return CWPWAPBookmarkItem instance
        */
        static CWPWAPBookmarkItem* NewL(  const TDesC& aTitle, 
            const TDesC& aDefaultName, 
            CWPCharacteristic& aCharacteristic, 
            CWPCharacteristic*& aLink
            );


        /**
        * Destructor.
        */
        ~CWPWAPBookmarkItem();

    public:   // From MWPMMSItemBase

        const TDesC& Name() const;
        const TDesC& Title() const;
        TBool CanSetAsDefault() const;
        void SaveL();
        void SetAsDefaultL();
        TBool ValidateL();
        void AddL( RPointerArray<MWPWAPItemBase>& aShownItems, 
            RPointerArray<MWPWAPItemBase>& aHiddenItems );
        const TDesC8& SaveData() const;
        const TDesC& Address() const;

    public:  // From MWPVisitor

        void VisitL(CWPCharacteristic& aCharacteristic);
        void VisitL(CWPParameter& aParameter);
        void VisitLinkL(CWPCharacteristic& aCharacteristic );

    private:  // New functions
        /**
        * C++ constructor.
        * @param aTitle The title to be returned in Name().
        * @param aDefaultName The default name for bookmarks.
        * @param aLink Link to access point
        * @param aDb The bookmark database to use. Created if NULL.
        */
        CWPWAPBookmarkItem( const TDesC& aTitle, 
            const TDesC& aDefaultName, 
            CWPCharacteristic*& aLink
             );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aCharacteristic The RESOURCE characteristic 
        * to scan.
        */
        void ConstructL( CWPCharacteristic& aCharacteristic );

        /**
        * Create the bookmarks db.
        */
        void CreateDbL();

    private:
        // The title for bookmark items
        const TDesC& iTitle;

        // The default name for bookmarks.
        const TDesC& iDefaultName;

        // The access point characteristic. Refs.
        CWPCharacteristic*& iLink;

        // The name of the bookmark
        TPtrC iName;

        // The URI of the bookmark
        TPtrC iURI;

        // Startpage parameter
        TBool iHasStartPage;

        //RFavouritesDb       iBookmarkDb;
        //RFavouritesSession  iSession;

        // Access point uid.
        TUint32 iUID;

        // UIDs of the saved items
        TPckgBuf<TWPWAPSaveItem> iSaveItem;
    };

#endif  // CWPWAPBOOKMARKITEM_H
            
// End of File
