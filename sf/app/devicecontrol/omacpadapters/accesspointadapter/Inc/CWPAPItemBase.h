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
* Description:     Defines a base class for all the items in WPAPAdapter
*
*/





#ifndef CWPAPITEMBASE_H
#define CWPAPITEMBASE_H

// INCLUDES

#include <e32base.h>
#include <cmconnectionmethodext.h>
#include <cmmanagerext.h>

// DATA TYPES

// Field types that can be marked received.
// Values defined to the power of to to enable masking.
enum TMarkedField
	{
	EWPParameterPxAddrType_id	= 2,
	EWPParameterPxAddrFQDN_id	= 4,
	EWPParameterNapID_id		= 8,
	EWPParameterService_id		= 16,
	EWPParameterPortNbr_id		= 32,
	EWPParameterPxAddr_id		= 64,
	EWPParameterAuthentication_id = 512,
    EWPParameterIfNetworks_id   = 1024,
    EWPParameterInternet_id     = 2048,
    EWPParameterWlan_id         = 4096
	};

// FORWARD DECLARATIONS

class CWPAPAccesspointItem;
class CCommsDatabase;

// CLASS DECLARATION

/**
 * CWPAPItemBase is a base class for all the items in WPAPAdapter.
 */ 
class CWPAPItemBase : public CBase
	{
	public:

        /**
        * Destructor.
        */
		~CWPAPItemBase();

	public: // New methods

		/**
		* Store the item data to APEngine's accesspoint item.
		* @param aAPItem item where data is stored.
		*/
		virtual void AddDataL( RCmConnectionMethodExt& ) = 0;

        /**
        * Add access point items to an array.
        * @param aItems The array to add to
        * @param aLogicalProxy The related logical proxy, if any
        * @param aPhysicalProxy The related physical proxy, if any
        */
        virtual TInt AddItemsL( RPointerArray<CWPAPAccesspointItem>& aItems,
            CWPAPItemBase* aLogicalProxy,
            CWPAPItemBase* aPhysicalProxy ) = 0;

        /**
        * The name of the item to be displayed to the user.
        * @return The name
        */
        virtual const TDesC& Name() = 0;
        
        /**
        * SaveWlanDataL
        */    
        virtual void SaveWlanDataL( TUint32 aIapId, CCommsDatabase& aCommsDb ) = 0;
    
        virtual void UpdateLingerL(const TUint32 aIapId) = 0;

	protected: // New methods. Only for subclasses use.

        /**
        * Base constructor.
        * @param aDefaultName The default name to use for an access point
        */
        CWPAPItemBase( const TDesC& aDefaultName );

		/**
		* Mark that this field has arrived and was correctly handled.
		* @param aField received field enumeration
		*/
		void MarkReceived( TMarkedField aField );

		/**
		* Check has the given field been received.
		* @param aField received field enumeration
		* @return ETrue if the field has already been received and handled
		*/
		TBool IsReceived( TMarkedField aField );

	protected:

        /// The default name for an access point
        const TDesC& iDefaultName;

	private: // Data

		/// Received fields marked here.
		TUint32 iReceivedFieldsFlags;

	private: // For testing.
		friend class T_CWPAPItemBase;
	};

#endif	// CWPAPITEMBASE_H
            
// End of File
