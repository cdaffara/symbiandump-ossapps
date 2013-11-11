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
* Description:     Defines a class, which holds logical proxy information.
*
*/





#ifndef CWPAPLOGICALPROXY_H
#define CWPAPLOGICALPROXY_H

// INCLUDES

#include <MWPVisitor.h>
#include "CWPAPItemBase.h"
#include <cmconnectionmethodext.h>

// FORWARD DECLARATIONS

class CWPAPPhysicalProxy;
class CWPAPAccesspointItem;
class CWPAPPort;

// CLASS DECLARATION

/**
 * CWPAPLogicalProxy contains logical proxy data.
 */ 
class CWPAPLogicalProxy : public CWPAPItemBase, private MWPVisitor
	{
	public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aDefaultName The default name of an access point
        * @param aCharacteristic The PXLOGICAL characteristic
        */
		static CWPAPLogicalProxy* NewLC( const TDesC& aDefaultName, 
            CWPCharacteristic& aCharacteristic );

        /**
        * Destructor.
        */
		~CWPAPLogicalProxy();

	public: // From CWPAPItemBase

		TBool ValidateL();
		void AddDataL( RCmConnectionMethodExt& aCmItem );
        TInt AddItemsL( RPointerArray<CWPAPAccesspointItem>& aItems,
            CWPAPItemBase* aLogicalProxy,
            CWPAPItemBase* aPhysicalProxy );
        const TDesC& Name();
        void SaveWlanDataL( TUint32 /*aIapId*/, CCommsDatabase& /*aCommsDb*/ ) {};

        void UpdateLingerL(const TUint32 /*aIapId*/){};

	private: // From CWPAPItemBase

		void VisitL(CWPCharacteristic& aElement);		
		void VisitL(CWPParameter& aElement);
        void VisitLinkL( CWPCharacteristic& /*aLink*/ ) {};

	private: // New methods

        /**
        * C++ default constructor.
        * @param aDefaultName The default name of an access point
        */
		CWPAPLogicalProxy( CWPCharacteristic& aCharacteristic,
            const TDesC& aDefaultName );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();
		
	private: // Data

		/// Pointer to data model parameter. Refs.
		CWPParameter* iHomepage;

		/// Pointer to data model parameter. Refs.
		CWPParameter* iName;

		/// Pointer to port object. Owns.
		CWPAPPort* iPort;

		/// Array of physical proxy objects. Owns.
		RPointerArray<CWPAPPhysicalProxy> iPhysicalProxies;

        /// Characteristic
        CWPCharacteristic& iCharacteristic;

	private: // For testing

		friend class T_CWPAPLogicalProxy;
		friend class T_CWPAPAdapter;
		friend class T_WPAPTestUtils;
	};

#endif	// CWPAPLOGICALPROXY_H
            
// End of File