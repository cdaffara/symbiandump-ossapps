/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Setting item for MMS settings.
*
*/


#ifndef CWPWAPMMSITEM_H
#define CWPWAPMMSITEM_H

// INCLUDES
#include <e32base.h>
#include <msvapi.h>
#include "CWPWAPItemBAse.h"

// FORWARD DECLARATIONS
class CBookmarkDb;
class RCmManagerExt;

// CLASS DECLARATION

/**
 * CWPWAPMMSItem contains one MMS setting.
 *
 * @lib WPWAPAdapter
 * @since 2.0
 */ 
class CWPWAPMMSItem : public CWPWAPItemBase, private MMsvSessionObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aTitle Title for MMS items
        * @param aDefaultName Default name for MMS items
        * @param aCharacteristic The APPLICATION characteristic
        * @param aCommsDb The CommsDB to use. Created if NULL.
        * @param aAPHandler The APEngine data handler to use. Created if NULL.
        */
        static CWPWAPMMSItem* NewL( const TDesC& aTitle, 
            const TDesC& aDefaultName, 
            CWPCharacteristic& aCharacteristic, 
            CCommsDatabase*& aCommsDb, 
            RCmManagerExt*& aCmManager);

        /**
        * Destructor.
        */
        ~CWPWAPMMSItem();

    public:   // From CWPWAPMMSItemBase

        TBool CanSetAsDefault() const;
        void SaveL();
        void SetAsDefaultL();
        TBool ValidateL();
        void ResourceL( CWPCharacteristic& aResource );
        void AddL( RPointerArray<MWPWAPItemBase>& aShownItems, 
            RPointerArray<MWPWAPItemBase>& aHiddenItems );
        void VisitLinkL( CWPCharacteristic& aCharacteristic );

    private: // from MMsvSessionObserver

        void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

    private:  // New functions
        /**
        * C++ default constructor.
        * @see CWPWAPMMSItem::NewL() for description of parameters.
        */
        CWPWAPMMSItem(  const TDesC& aTitle, 
            const TDesC& aDefaultName, 
            CWPCharacteristic& aCharacteristic, 
            CCommsDatabase*& aCommsDb, 
            RCmManagerExt*& aCmManager);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();



    private:   // Data
        // The bookmarks database. Refs.
        CBookmarkDb* iDb;

        // The potential links
        RPointerArray<CWPCharacteristic> iLinks;
    };

#endif  // CWPWAPMMSITEM_H
            
// End of File
