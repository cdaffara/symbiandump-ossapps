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
* Description:   The header file of the CClkMccListener class.
*
*/

#ifndef __CLOCKSERVER_MCC_LISTENER_H__
#define __CLOCKSERVER_MCC_LISTENER_H__

// System includes
#include <e32base.h>
#include <etelmm.h>

// User includes

// Forward declaration
class MClkSrvMccObserver;

// Constants

// Class declaration
/**
* @class CClkMccListener
* @brief Listens to ETelServer for the MCC registered.
*/
class CClkMccListener : public CActive
	{
	public:			// New functions
	
		/**
		* @brief Two phased constructor, performs the first phase of the object construction.
		* @return CClkMccListener* Pointer to an object of CClkMccListener
		*/
		static CClkMccListener* NewL( MClkSrvMccObserver* aObserver );
		
		/**
		* @brief Destructor.
		*/
		~CClkMccListener();
		
	public:			// New functions
	
		/**
        * @brief Called by the user creator of the object and RunL, to monitor the changes in MCC.
        */
        void Start();
        
        /**
        * @brief Gets the Mobile Country Code that was retrieved from ETelServer.
        * @param aMcc to which the value of the MCC is returned.
        * @return TInt an Error code.
        */
		TInt GetCurrentMcc( RMobilePhone::TMobilePhoneNetworkCountryCode& aMcc );
		    
		/**
        * @brief Deactivates the MCC monitoring service.
        */
        void Stop();
        
	protected:		// From base classes
	
		/**
        * @brief From CActive. Called when the value of iStatus changes.
        */
        void RunL();
        
        /**
        * @brief From CActive. Called when a Cancel() is called on the active object.
        */
        void DoCancel();
        
	private:		// New functions
	
		/**
		* @brief Performs the second phase of construction
		*/
		void ConstructL();
		
		/**
		* @brief The C++ default constructor
		*/
		CClkMccListener( MClkSrvMccObserver* aObserver );
		
	private:		// Data
	
		/**
		* @var iEtelServer
		* @brief Clinet handle to the ETelServer.
		*/
		RTelServer										iEtelServer;
		
		/**
		* @var iMobilePhone
		* @brief Clinet handle to the RMobilePhone.
		*/
        RMobilePhone                    				iMobilePhone;
        
        /**
		* @var iIsInitialized
		* @brief Will be ETrue if listening for changes in MCC.
		*/      
        TBool                           				iIsInitialized;
        
        /**
		* @var iNetworkInfo
		* @brief The network info
		*/
        RMobilePhone::TMobilePhoneNetworkInfoV1			iNetworkInfo;
        
        /**
		* @var iNetworkInfoPckg
		* @brief The network info package.
		*/
        RMobilePhone::TMobilePhoneNetworkInfoV1Pckg  	iNetworkInfoPckg;
        
        /**
		* @var iCurrentMcc
		* @brief Has the current MCC.
		*/
        RMobilePhone::TMobilePhoneNetworkCountryCode 	iCurrentMcc; 
        
        /**
		* @var iObserver
		* @brief The observer for mcc changes.
		*/
        MClkSrvMccObserver*								iObserver; 		
	};

#endif		// __CLOCKSERVER_MCC_LISTENER_H__

// End of file
