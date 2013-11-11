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
* Description:     Defines a class, which holds proxy port information.
*
*/





#ifndef CWPAPPORT_H
#define CWPAPPORT_H

// INCLUDES

#include <MWPVisitor.h>
#include "CWPAPItemBase.h"

// FORWARD DECLARATIONS



// CLASS DECLARATION

/**
 * CWPAPPort contains port information
 */ 
class CWPAPPort : public CWPAPItemBase, private MWPVisitor
	{
	public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aCharacteristic The PORT characteristic
        */
		static CWPAPPort* NewLC( CWPCharacteristic& aCharacteristic );

        /**
        * Destructor.
        */
		virtual ~CWPAPPort();

	public: // From CWPAPItemBase 

		TBool ValidateL();
		void AddDataL( RCmConnectionMethodExt& aCmItem ); //CMManager
        TInt AddItemsL( RPointerArray<CWPAPAccesspointItem>& aItems,
                        CWPAPItemBase* aLogicalProxy,
                        CWPAPItemBase* aPhysicalProxy );
        const TDesC& Name();
        void SaveWlanDataL( TUint32 /*aIapId*/, CCommsDatabase& /*aCommsDb*/ ) {};
        void UpdateLingerL(const TUint32 /*aIapId*/){};

	private: // From MWPVisitor

		void VisitL(CWPCharacteristic& aCharacteristic);
		void VisitL(CWPParameter& aParameter);
        void VisitLinkL( CWPCharacteristic& /*aLink*/ ) {};
		
	private:  // New functions
		
        /**
        * Constructor.
        */
		CWPAPPort();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();
		
		/**
		* Helper method to set the internal values.
		* @param aConnectionSecurity sets value for iConnectionSecurity.
		* @param aSessionMode sets value for iSessionMode.
		* @param aReceivedField marks the field as received
		*/
		void SetValues( TBool aConnectionSecurity, 
			            TInt aSessionMode,
			            TMarkedField aReceivedField );

	private:  // Data

        /// ETrue if secure connection should be established
		TBool           iConnectionSecurity;
        /// Session mode for WAP connections: connection oriented/less.
		TInt            iSessionMode;
		/// Pointer to port number parameter. Refs.
		CWPParameter*   iPortNbr;

	private:
		friend class T_CWPAPProxyData;
		friend class T_CWPAPAdapter;
		friend class T_WPAPTestUtils;
	};

#endif // CWPAPPORT_H

// End of File