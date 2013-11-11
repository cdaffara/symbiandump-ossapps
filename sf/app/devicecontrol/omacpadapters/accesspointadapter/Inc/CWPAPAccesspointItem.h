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
* Description:    Class stores data for one accesspoint
*
*/



#ifndef CWPAPACCESSPOINTITEM_H
#define CWPAPACCESSPOINTITEM_H

// INCLUDES
#include <e32def.h>
#include <e32std.h>
#include <e32base.h>
#include <cmmanagerext.h>

// FORWARD DECLARATIONS
class CWPEngine;
class CWPAPItemBase;
class CWPParameter;
class CWPCharacteristic;
class CCommsDatabase;
class CWPAPNapdef;
class RCmManagerExt;

// CLASS DECLARATION

/**
 * CWPAPAccesspointItem handles accesspoint settings. It links data from
 * logical proxy, physical proxy and napdef into one item.
 */ 
class CWPAPAccesspointItem : public CBase
	{
	public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aDefaultName The default name if none is found
        * @param aLogicalProxy The logical proxy 
        * @param aPhysicalProxy The physical proxy 
        * @param aNapdef The napdef
        * @param aCharacteristic The characteristic 
        */
		static CWPAPAccesspointItem* NewLC(
		                            const TDesC& aDefaultName,
                                    CWPAPItemBase* iLogicalProxy,
                                    CWPAPItemBase* iPhysicalProxy,
                                    CWPAPItemBase* aNapdef,
                                    CWPCharacteristic& aCharacteristic );

        /**
        * Destructor.
        */
		virtual ~CWPAPAccesspointItem();

	public: // New methods

		/**
		* Get the accesspoint name.
		* @return reference to name.
		*/
		const TDesC& Name() const;
		
		/**
		* Get the NAPdef pointer
		* @return pointer to napdef.
		*/
	  	CWPAPItemBase* NapDef();
	  	
	  	/**
		* Set the pointer to the CM manager
		* @return void
		*/
	  	void SetCMManager( RCmManagerExt* aCm );
	  	
	  	/**
		* Get the characteristic value
		* @return iCharacteristic.
		*/
	  	TInt Characteristic();
		
		/**
		* Save this accesspoint to comms database.
		* @param aCommsDb reference to CCommsdatabase
		*/
		TUint32 SaveL( CCommsDatabase& aCommsDb );

        /**
        * Return the UID of the saved access point.
        * @return UID
        */
        const TDesC8& Uid() const;

        /**
        * Delete old data from engine model.
        */
        void DeleteUidData() const;

	private:  // New functions

		/**
		* Store the accesspoint data to comms database.
		* @param aCommsDb reference to CCommsdatabase
		*/ 
		TUint32 StoreL( CCommsDatabase& aCommsDb );

		/**
        * C++ default constructor.
        * @param aDefaultName The default name if none is found
        * @param aLogicalProxy The logical proxy 
        * @param aPhysicalProxy The physical proxy 
        * @param aNapdef The napdef
        * @param aCharacteristic The characteristic 
        */
		CWPAPAccesspointItem( const TDesC& aDefaultName,
                              CWPAPItemBase* iLogicalProxy,
                              CWPAPItemBase* iPhysicalProxy,
                              CWPAPItemBase* aNapdef,
                              CWPCharacteristic& aCharacteristic );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();

	private: // Data (Total ownership is assumed unless otherwise stated)

        /// The default name if no other is found
        const TDesC& iDefaultName;

        /// The napdef
        CWPAPItemBase* iNapdef;

        /// The physical proxy
        CWPAPItemBase* iPhysicalProxy;

        /// The logical proxy
        CWPAPItemBase* iLogicalProxy;

        /// The uid created when saving
        TPckgBuf<TUint32> iUid;

        /// The characteristic the caused the creation of an access point
        CWPCharacteristic& iCharacteristic;
        RCmManagerExt* iCmManager;

	private: // For testing usage.
		friend class T_CWPAPAccesspointItem;
		friend class T_CWPAPAdapter;
		friend class T_WPAPTestUtils;
	};

#endif	// CWPAPACCESSPOINTITEM_H
            
// End of File
