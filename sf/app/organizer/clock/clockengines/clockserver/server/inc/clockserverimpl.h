/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   The header file of the CClkSrvImpl class.
*
*/

#ifndef __CLOCKSERVER_IMPL_H__
#define __CLOCKSERVER_IMPL_H__

// System includes
#include <e32base.h>
#include <e32cmn.h>
#include <etelmm.h>
#include <ecom/implementationinformation.h>

// User includes
#include "clockpluginobserver.h"
#include "clockservermccobserver.h"
#include "clockecomobserver.h"

// Forward declarations
class CClockTimeSourceInterface;
class CClkSrvMain;
class CClkMccListener;
class CClockTimeZoneResolver;
class CClockEComListener;

// Class declaration
/**
* @class CClkSrvImpl
* @brief Performs the actual service requested by the client.
*		 Owned by CClkSrv and used by CClkSrvSession.
*/
class CClkSrvImpl : public CBase,
						   MClockPluginObserver,
						   MClkSrvMccObserver,
						   MClockEComObserver
	{
	
	public:				// New functions
		
		/**
		* @brief Two phased constructor
		* @return CClkSrvImpl* Pointer to an object of CClkSrvImpl
		*/
		static CClkSrvImpl* NewL( CClkSrvMain* aClkSrv );

		/**
		* @brief Destructor.
		*/
		~CClkSrvImpl();
		
	public:				// New functions
	
	    /**
		* @brief Activates all the given protocols.
		* @return An error code
		*/
		TInt ActivateAllProtocolsL();
		
		/**
		* @brief Activates the given protocol
		* @param aClkSrvProtocol The protocol to be activated.
		* @return An error code
		*/
		TInt ActivateProtocolL( TInt aClkSrvProtocol );
		
		/**
		* @brief Checks if aClkSrvProtocol is Active or not
		* @param aIsActive Reference argument, ETrue if aClkSrvProtocol is active, EFalse otherwise.
		* @param aClkSrvProtocol The protocol.
		* @return An error code
		*/
		TInt IsProtocolActive( TBool& aIsActive, TInt aClkSrvProtocol = 0 );
		
		/**
		* @brief Deactivate all the installed protocol.
		* @return An error code
		*/
		TInt DeActivateAllProtocolsL();
		
		/**
		* @brief Deactivates the given protocol.
		* @param aClkSrvProtocol The protocol.
		* @return An error code
		*/
		TInt DeActivateProtocolL( TInt aClkSrvProtocol );
		
		/**
		* @brief Returns the time information
		* @param aTimeInformation A reference to an object of STimeAttributes.
		* @return An error code
		*/
		TInt GetProtocolInformationL( STimeAttributes& aTimeInformation );
		
		/**
		* @brief Returns the current mcc
		* @param aCurrentMcc A reference variable holding the current mcc value.
		* @return An error code
		*/		
		TInt GetCurrentMccL( TInt& aCurrentMcc );
		
		/**
		* @brief Returns the current timezone Id
		* @param aCurrentTimeZoneId A reference variable holding the current timezone Id value.
		* @return An error code
		*/
		TInt GetCurrentTimeZoneIdL( TInt& aCurrentTimeZoneId );
		
		/**
		* @brief Checks if any protocols have been enabled.
		* @param aEnabled ETrue if atleast one protocol is active, EFalse otherwise.
		* @return TInt Error code.
		*/
		TInt IsAutoTimeUpdateOn( TBool& aEnabled );

	public:				// From base classes
		
		/**
		* @brief From MTimeSourceObserver. This gets called by one of the time source plugins, when they have
		*		 a time update.
		*/
		void NotifyTimeChangeL( CClockTimeSourceInterface& aPluginImpl );
		
		/**
		* @brief From MClkSrvMccObserver. Called by CClkMccListener when Mcc is available or there is a 
		*		 change in the mcc.
		*/
		void NotifyMccChangeL();
		
		/**
		* @brief From MClockEComObserver. Called whenever the a new timesource plugin gets installed or uninstalled.
		* @param aImplInfo The information of the plugin which got installed or uninstalled.
		* @param aInstalled ETrue if a plugin has been installed, EFalse otherwise.
		*/
		void NotifyPluginInstallUninstallL( const CImplementationInformation& aImplInfo, TBool aInstalled );

	private:			// New functions
		
		/**
		* @brief Performs the second phase of construction
		*/
		void ConstructL();
		
		/**
		* @brief The C++ default constructor
		*/
		CClkSrvImpl( CClkSrvMain* aClkSrv );

	private:			// Data

		/**
		* @var iTimeSourceObjArray
		* @brief Pointer to the CClockTimeSourceInterface Array.
		*/
		RPointerArray<CClockTimeSourceInterface>        iTimeSourceObjArray;
		
		/**
		* @var iClkSrv
		* @brief Pointer to the clock server object. Not owned.
		*/
		CClkSrvMain*									iClkSrv;
		
		/**
		* @var iMccListener
		* @brief Pointer to the Mcc listener object. Owned.
		*/
		CClkMccListener*								iMccListener;
		
		/**
		* @var iMccListener
		* @brief Pointer to the Mcc listener object. Owned.
		*/
		RMobilePhone::TMobilePhoneNetworkCountryCode	iMcc;
		
		/**
		* @var iTimeAttributes
		* @brief Object of type struct STimeAttribute, holds the time and offset related information.
		*/
		STimeAttributes*								iTimeAttributes;
		
		/**
		* @var iMccReceived
		* @brief ETrue if MCC is received.
		*/
		TBool											iMccReceived;
		
		/**
		* @var iPluginData
		* @brief ETrue if we have data from plugins.
		*/
		TBool 											iPluginData;
		
		/**
		* @var iTzResolver 
		* @brief Resolver of type CTimezoneResolver.
		*/
		CClockTimeZoneResolver*							iTzResolver;
		
		/**
		* @var iTimeZoneId
		* @brief Hold the current timezone Id.
		*/
		TInt											iTimeZoneId;
		
		/**
        * @var iClockEComListener
        * @brief Listens for changes in the ECOM registry.
        */
		CClockEComListener*                             iClockEComListener;
	};


#endif			// __CLOCKSERVER_IMPL_H__

// End of file
