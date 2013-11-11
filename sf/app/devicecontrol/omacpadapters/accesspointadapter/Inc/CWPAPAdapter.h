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
* Description:    Main class for handling provisioning accesspoint data
*
*/



#ifndef CWPAPADAPTER_H
#define CWPAPADAPTER_H

// INCLUDES

#include <CWPAdapter.h>
#include <MWPContextExtension.h>
#include <cmmanagerext.h>

// FORWARD DECLARATIONS

class CWPAPAccesspointItem;
class CCommsDatabase;

// CLASS DECLARATION

/**
 * CWPAPAdapter handles accesspoint settings.
 */ 
class CWPAPAdapter : public CWPAdapter, public MWPContextExtension
	{
	public:  // Constructors and destructor

        /**
        * Two-phased constructor.
		* @return a instance of class.
        */
		static CWPAPAdapter* NewL();

		/**
		* Destructor
		*/
		~CWPAPAdapter();

	public: // From CWPAdapter

		TInt ItemCount() const;
		const TDesC16& SummaryTitle(TInt aIndex) const;
		const TDesC16& SummaryText(TInt aIndex) const;
		TInt DetailsL( TInt aItem, MWPPairVisitor& aVisitor );
		void SaveL( TInt aItem );
		TBool CanSetAsDefault( TInt aItem ) const;
		void SetAsDefaultL( TInt aItem );
		void VisitL(CWPCharacteristic& aElement);
		void VisitL(CWPParameter& aElement);
		void VisitLinkL(CWPCharacteristic& aCharacteristic );
		TInt ContextExtension( MWPContextExtension*& );

	public: // From MWPContextExtension

		const TDesC8& SaveDataL( TInt aIndex ) const;
		void DeleteL( const TDesC8& aSaveData );
        TUint32 Uid() const;

	private:  // New functions

        /**
        * C++ default constructor.
        */
		CWPAPAdapter();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();

		/**
		* Check whether this characteristic is already linked.
		* @param aCharacteristic characteristic to compare.
		* @return ETrue if link to given characteristic was found.
		*/
		TBool IsAlreadyLinked( CWPCharacteristic& aCharacteristic );

	private: // Data

		/// Array of linked NAPDEF and PXLOGICAL characteristics. Refs.
		RPointerArray<CWPCharacteristic> iLinks;
		/// Array of data items for memory management. Owns.
		RPointerArray<CBase> iDataItems;
        /// Array of save items. Owns.
        RPointerArray<CWPAPAccesspointItem> iSaveItems;
		/// Pointer to commsdatabase used in saving. Owns.
		CCommsDatabase* iCommsDb;
		/// Summary title. Owns.
		HBufC* iTitle;
        /// Default name. Owns.
        HBufC* iDefaultName;
		/// Indicates whether these settings are already saved.
		TBool iAlreadySavedOnce;
		// CMManager for managing connection methods
		RCmManagerExt* iCmManager;
	  RArray<TInt> iAPValue;

	private: // For testing.
		friend class T_CWPAPAdapter;
	};

#endif	// CWPAPADAPTER_H
            
// End of File
