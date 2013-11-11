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
* Description:  Handles mailbox number settings in OMA provisioning.
*
*/


#ifndef MbxNbrAdapter_H
#define MbxNbrAdapter_H

// INCLUDES

#include    <CWPAdapter.h>

// FORWARD DECLARATIONS
//struct TMbxNbrAdapterData;


// MODULE DATA STRUCTURES
    
// ENUMERATIONS


// CLASS DECLARATION

/**
 * CMbxNbrAdapter handles accesspoint settings.
 */ 
class CMbxNbrAdapter : public CWPAdapter
    {
    private:    // ENUMERATIONS
    
        enum TMailboxType
            {
            ECSVoiceMailbox = 1,    // Default if type is not received in XML
            ECSVideoMailbox
            };
            
    private:    // MODULE DATA STRUCTURES
    
        struct TMbxNbrAdapterData
            {
            TPtrC iName;
            TPtrC iMailboxNumber;
            TBool iVidiosupport;
            TBool iIsVideosupport;
            TMailboxType iMailboxType;
            };    
                
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @return a instance of class.
        */
        static CMbxNbrAdapter* NewL();

        /**
        * Destructor
        */
        virtual ~CMbxNbrAdapter();

    public: // Functions from base classes

        /**
        * From CWPAdapter
        */
        TInt ItemCount() const;

        /**
        * From CWPAdapter
        */
        const TDesC16& SummaryTitle( TInt aIndex ) const;

        /**
        * From CWPAdapter
        */
        const TDesC16& SummaryText( TInt aIndex ) const;

        /**
        * From CWPAdapter
        */
        TInt DetailsL( TInt aItem, MWPPairVisitor& aVisitor );

        /**
        * From CWPAdapter
        */
        void SaveL( TInt aItem );

        /**
        * From CWPAdapter
        */
        TBool CanSetAsDefault( TInt aItem ) const;

        /**
        * From CWPAdapter
        */
        void SetAsDefaultL( TInt aItem );

        /**
        * From CWPAdapter
        */
        void VisitL( CWPCharacteristic& aElement );

        /**
        * From CWPAdapter
        */
        void VisitL( CWPParameter& aElement );

        /**
        * From CWPAdapter
        */
        void VisitLinkL( CWPCharacteristic& aCharacteristic );

    private:  // Default constructors

        /**
        * C++ default constructor.
        */
        CMbxNbrAdapter();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // New functions

        /**
        * Validates the data in iTempData.
        */
        TBool IsValid();

        /**
        * Validates the phone number string.
        */
        TBool IsValidPhoneNumber( const TDesC& aPtr );

        /**
        * Parses an mailbox type from a descriptor.
        */
        void ParseMbxTypeL( const TDesC& aPtr, TMailboxType& aInt );

        /**
        * Parses an video ON/OFF type from a descriptor.
        */

        void ParseVideoTypeL( const TDesC& aPtr, TBool& aInt, TBool& aInt1 );

    private: // Data

        HBufC* iTitle;

        TPtrC iAppId;
        RArray<TMbxNbrAdapterData> iSettings;
        TMbxNbrAdapterData iTempData;

	private:
		friend class T_CWPMbxNbrAdapter;
	

    };

#endif  // MbxNbrAdapter_H
            
// End of File
