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
*     Base class for WAP setting items.
*
*/


#ifndef CWPWAPITEMBASE_H
#define CWPWAPITEMBASE_H

// INCLUDES
#include <e32base.h>
#include <MWPVisitor.h>
#include "MWPWAPItemBase.h"
#include <cmmanagerext.h>
#include <cmconnectionmethodext.h>

// FORWARD DECLARATIONS
class CCommsDatabase;


// CLASS DECLARATION

/**
 * CWPWAPItemBase is a base class for browser settings.
 *
 * @lib WPWAPAdapter
 * @since 2.0
 */ 
class CWPWAPItemBase : public CBase, private MWPVisitor, public MWPWAPItemBase
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        ~CWPWAPItemBase();

    public:  // From MWPVisitor

        void VisitL(CWPCharacteristic& aCharacteristic);
        void VisitL(CWPParameter& aParameter);
        void VisitLinkL(CWPCharacteristic& aCharacteristic );

    public:   // From MWPWAPItemBase

        const TDesC& Name() const;
        const TDesC& Title() const;
        const TDesC& Address() const;
        const TDesC8& SaveData() const;

    protected:   // New methods
        /**
        * C++ default constructor.
        * @param aTitle Title for items
        * @param aDefaultName Default name for items
        * @param aCharacteristic The APPLICATION characteristic
        * @param aCommsDb The CommsDB to use. Created if NULL.
        * @param aAPHandler The APEngine data handler to use. Created if NULL.
        */
        CWPWAPItemBase( const TDesC& aTitle, 
            const TDesC& aDefaultName, 
            CWPCharacteristic& aCharacteristic, 
            CCommsDatabase*& aCommsDb, 
            RCmManagerExt*& aCmManager);

        /**
        * 2nd phase base constructor. Derived classes must call this.
        */
        void BaseConstructL();

        /**
        * Handles one resource item.
        * @param aResource The RESOURCE item
        */
        virtual void ResourceL( CWPCharacteristic& aResource ) = 0;

        /**
        * Create CCommsDatabase and APEngine if NULL.
        */
        void CreateDbL();

        /**
        * Writes a home page to an access point. If access point already
        * has a home page, create a new one.
        * @param aItem Access point to access
        */
        void WriteHomePageL( RCmConnectionMethodExt& aCmItem );

        /**
        * Create a valid name for the access point.
        * @param aName The base name for access point
        * @param aDb CommsDb instance
        */
        void MakeValidNameL( TDes& aName, CCommsDatabase& aDb ) const;

    protected:
        // The parent characteristic. Refs.
        CWPCharacteristic& iCharacteristic;

        // The preferred access point name
        TPtrC iName;

        // The access point address, Owns.
        HBufC* iAddr;

        // The UID of the access point
        TUint32 iUID;

        // UID of the saved item
        TPckgBuf<TWPWAPSaveItem> iSaveItem;

        // Temporary current proxy while visiting. Refs.
        CWPCharacteristic* iLink;

        // Text to return in SummaryTitle().
        const TDesC& iTitle;

        // Default name of access point.
        const TDesC& iDefaultName;

        // Comms db. Refs.
        CCommsDatabase*& iCommsDb;

                // @var Pointer to Connection Method Manager that is required to access
        // Access Point data
        RCmManagerExt*&   iCmManager;
    };

#endif  // CWPWAPITEMBASE_H
            
// End of File
