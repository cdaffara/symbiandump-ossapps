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
*     Base for WAP adapters.
*
*/


#ifndef CWPWAPADAPTERBASE_H
#define CWPWAPADAPTERBASE_H

// INCLUDES
#include <CWPAdapter.h>
#include <MWPContextExtension.h>

// FORWARD DECLARATIONS
class CWPCharacteristic;
class MWPWAPItemBase;
class CCommsDatabase;
class RCmConnectionMethodExt;
class RCmManagerExt;

// CLASS DECLARATION

/**
 * CWPWAPAdapterBase is a base for WAP adapters.
 *
 * @lib WPWAPAdapter
 * @since 2.0
 */ 
class CWPWAPAdapterBase : public CWPAdapter, private MWPContextExtension
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        ~CWPWAPAdapterBase();

    public: // from CWPAdapter
        
        TInt ItemCount() const;     
        const TDesC16& SummaryTitle(TInt aIndex) const;     
        const TDesC16& SummaryText(TInt aIndex) const;      
        void SaveL( TInt aItem );
        TBool CanSetAsDefault( TInt aItem ) const;
        void SetAsDefaultL( TInt aItem );
        TInt DetailsL( TInt aItem, MWPPairVisitor& aVisitor );
        void VisitL(CWPCharacteristic& aElement);
        void VisitL(CWPParameter& aElement);
        void VisitLinkL(CWPCharacteristic& aCharacteristic );
        TInt ContextExtension( MWPContextExtension*& aExtension );

    public: // from MWPContextExtension
        virtual const TDesC8& SaveDataL( TInt aIndex ) const;
        virtual void DeleteL( const TDesC8& aSaveData );
        TUint32 Uid() const;

    public: // New methods
        /**
        * Handles one application characteristic.
        * @param aCharacteristic The application characteristic
        */
        virtual void ApplicationL( CWPCharacteristic& aCharacteristic ) = 0;

    protected:  // New functions
        /**
        * C++ default constructor.
        */
        CWPWAPAdapterBase();

        /**
        * 2nd phase base constructor. Derived classes must call this.
        */
        void BaseConstructL();

        /**
        * Create CCommsDatabase and APEngine if NULL.
        */
        void CreateDbL();

    protected:  // Data
        // The application if of the current characteristic
        TPtrC iAppID;

        // The visible setting items. Owns.
        RPointerArray<MWPWAPItemBase> iItems;

        // The hidden setting items. Owns.
        RPointerArray<MWPWAPItemBase> iHiddenItems;

        // Comms database for saving. Owns.
        CCommsDatabase* iCommsDb;

        // APEngine for saving. Owns.
        RCmManagerExt* iCmManager;
    };

#endif  // CWPWAPADAPTERBASE_H
            
// End of File
