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
* Description:     Defines a class, which holds physical proxy information.
*
*/





#ifndef CWPAPPHYSICALPROXY_H
#define CWPAPPHYSICALPROXY_H

// INCLUDES

#include <MWPVisitor.h>
#include "CWPAPItemBase.h"
#include <cmconnectionmethodext.h>

// DATA TYPES

enum TWPAPPxAddrType
	{
	EPxAddrTypeIPv4 = 1,
    EPxAddrTypeIPv6,
	EPxAddrTypeUnsupported // (IPv6, E164, ALPHA are not supported)
	};

// FORWARD DECLARATIONS

class CWPAPLogicalProxy;
class CWPAPPort;
class CWPAPNapdef;

// CLASS DECLARATION

/**
 * CWPAPPhysicalProxy contains physical proxy information
 */ 
class CWPAPPhysicalProxy : public CWPAPItemBase, private MWPVisitor
	{
	public:  // Constructors and destructor

        /**
        * Two-phased constructor.
		* @param aDefaultName The default name of an access point
        * @param aCharacteristic The PXPHYSICAL characteristic.
        * @param aPort Reference to a pointer containing PORT of a PXLOGICAL.
        */
		static CWPAPPhysicalProxy* NewLC( const TDesC& aDefaultName, 
            CWPCharacteristic& aCharacteristic, 
            CWPCharacteristic& aLogicalCharacteristic,
            CWPAPPort*& aPort );

        /**
        * Destructor.
        */
		~CWPAPPhysicalProxy();

	public: // From CWPAPItemBase

		TBool ValidateL();
		void AddDataL( RCmConnectionMethodExt& aCmItem );
        TInt AddItemsL( RPointerArray<CWPAPAccesspointItem>& aItems,
            CWPAPItemBase* aLogicalProxy,
            CWPAPItemBase* aPhysicalProxy  );
        const TDesC& Name();
        void SaveWlanDataL( TUint32 /*aIapId*/, CCommsDatabase& /*aCommsDb*/ ) {};
        void UpdateLingerL(const TUint32 /*aIapId*/){};

	private: // From CWPAPItemBase. Override default implementation.

		void VisitL( CWPCharacteristic& aElement );		
		void VisitL( CWPParameter& aElement );	
		void VisitLinkL( CWPCharacteristic& aCharacteristic );

	private:  // New functions

        /**
        * C++ default constructor.
		* @param aDefaultName The default name of an access point
        * @param aPort Reference to a pointer containing PORT of a PXLOGICAL.
        */
		CWPAPPhysicalProxy( const TDesC& aDefaultName, 
            CWPCharacteristic& aLogicalCharacteristic,
            CWPAPPort*& aPort );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();
		
	private: // Data

		/// Pointer to data model parameter. Refs.
		CWPParameter* iProxyAddr;

        /// Pointer to data model parameter. Refs.
        CWPParameter* iName;

		/// Proxy address type
		TWPAPPxAddrType iPxAddrType;

		/// Pointer to port object. Owns.
		CWPAPPort* iPort;

        /// Pointer to port object owned by logical proxy. Refs.
        CWPAPPort*& iLogicalPort;

        /// Napdefs linked to by PXPHYSICAL
        RPointerArray<CWPAPNapdef> iNapdefs;

        /// Pointer to characteristic
        CWPCharacteristic& iLogicalCharacteristic;

	private: // For testing

		friend class T_CWPAPLogicalProxy;
		friend class T_CWPAPAdapter;
		friend class T_WPAPTestUtils;
	};

#endif	// CWPAPPHYSICALPROXY_H
            
// End of File
