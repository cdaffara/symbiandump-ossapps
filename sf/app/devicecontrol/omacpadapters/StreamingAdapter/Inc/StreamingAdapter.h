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
* Description:  Handles streaming app settings in OMA provisioning.
*
*/


#ifndef STREAMINGADAPTER_H
#define STREAMINGADAPTER_H

// INCLUDES

#include    <CWPAdapter.h>
#include    <MWPContextExtension.h>

// FORWARD DECLARATIONS
class CMPSettingsModel;
struct TStreamingAdapterData;

// CLASS DECLARATION

/**
 * CStreamingAdapter handles accesspoint settings.
 */ 
class CStreamingAdapter : public CWPAdapter,
                          public MWPContextExtension
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @return a instance of class.
        */
        static CStreamingAdapter* NewL();

        /**
        * Destructor
        */
        virtual ~CStreamingAdapter();

    public: // Functions from base classes

        /**
        * From CWPAdapter
        */
        TInt ItemCount() const;

        /**
        * From CWPAdapter
        */
        const TDesC16& SummaryTitle(TInt aIndex) const;

        /**
        * From CWPAdapter
        */
        const TDesC16& SummaryText(TInt aIndex) const;

        /**
        * From CWPAdapter
        */
        TInt DetailsL( TInt aItem, MWPPairVisitor& aVisitor );

        /**
        * From CWPAdapter
        */
        void SaveL(TInt aItem);

        /**
        * From CWPAdapter
        */
        TBool CanSetAsDefault(TInt aItem) const;

        /**
        * From CWPAdapter
        */
        void SetAsDefaultL(TInt aItem);

        /**
        * From CWPAdapter
        */
        void VisitL(CWPCharacteristic& aElement);

        /**
        * From CWPAdapter
        */
        void VisitL(CWPParameter& aElement);

        /**
        * From CWPAdapter
        */
        void VisitLinkL(CWPCharacteristic& aCharacteristic);

        /**
        * From CWPAdapter
        */
        TInt ContextExtension(MWPContextExtension*& aExtension);

        /**
        * From MWPContextExtension
        */
		const TDesC8& SaveDataL(TInt aIndex) const;

        /**
        * From MWPContextExtension
        */
		void DeleteL(const TDesC8& aSaveData);

        /**
        * From MWPContextExtension
        */
        TUint32 Uid() const;


    private:  // Default constructors

        /**
        * C++ default constructor.
        */
        CStreamingAdapter();

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
        * Parses an integer from a descriptor.
        * Returns error code.
        */
        TInt ParseIntegerL(const TDesC& aPtr, TInt& aInt);

		/**
        * ResetAndDestroy() cleanup for ecom implementation array.
        * @param aAny the implementation array
        */
        static void Cleanup( TAny* aAny );
        
        /**
        * Initialise CMPSettingsModel
        * @param None
        * @return None
        */
        void InitSettingsModelL();

    private: // Data

        HBufC* iTitle;
        CMPSettingsModel* iModel;

        TPtrC iAppId;
        TStreamingAdapterData* iCurrentData;
        TStreamingAdapterData* iTempData;

        TPckgBuf<TUint32> iSavedID;

	private:
		friend class TM_CWPStreamingAdapterTestCase;
	

    };

#endif  // STREAMINGADAPTER_H
            
// End of File
