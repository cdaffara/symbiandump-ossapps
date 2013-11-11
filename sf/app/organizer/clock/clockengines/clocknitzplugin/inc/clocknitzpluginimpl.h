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
* Description:   The header file of CClockNitzPluginImpl class.
*
*/

#ifndef __CLOCKNITZ_PLUGINIMPL_H__ 
#define __CLOCKNITZ_PLUGINIMPL_H__

// System includes
#include <etelmm.h>
#include <e32base.h>

// User includes
#include "clocknitzlistener.h"

// Forward declarations
class CClockNitzPlugin;

// Class declaration
/** 
* @class CClockNitzPluginImpl
* @brief An instance of class CClockNitzPluginImpl which is the actual implementation of NITZ
* 		 algorithm.
* @dll clocknitzplugin.dll 
*/
class CClockNitzPluginImpl : public CBase
	{
	public:			// Constructor and destructor
	
		/**
		* @brief Creates a CClockNitzPluginImpl object
		* @return CClockNitzPluginImpl*
		*/
        static CClockNitzPluginImpl* NewL( CClockNitzPlugin* aNitzPlugin );
		
		/**
		* @brief Destroy the object and release all memory objects
		*/
	    virtual ~CClockNitzPluginImpl();
	    
	public:			// New functions
	
		/**
		* @brief From CClockTimeSourceInterface, pure virtual at the base. Need to implement here.
		* @param aInterfaceAttribute of type TInterfaceAttribute, indicates the type of data requested.
		* @param aTimeAttributes The type of data requested
		*/
		void GetTimeInformationL( TInterfaceAttribute aInterfaceAttribute, STimeAttributes* aTimeAttributes );
		
		/**
		* @brief From CClockTimeSourceInterface, pure virtual at the base. Need to implement here.
		* @param aInterfaceAttribute of type TInterfaceAttribute, indicates the type of data requested.
		*/
		void NotifyOnNITZInfoChange( TInterfaceAttribute aInterfaceAttribute );
		
		/**
		* @brief Notifies any client in case NITZ info is received
		* @param aNitzInfo RMobilePhone::TMobilePhoneNITZ
		*/
		void NotifyNITZInfoChangeL();
		
	private:		// New functions
	
		/**
		* @brief C++ default constructor
		*/
  		CClockNitzPluginImpl( CClockNitzPlugin* aNitzPlugin );
		
		/** 
		* @brief Symbian OS 2nd phase constructor is private.
		*/
	    void ConstructL();
	    
	public:			// Data
		
		/**
		* @var iNitzInfo 
		* @brief User define NITZ info structure of type RMobilePhone::TMobilePhoneNITZ
		*/
		RMobilePhone::TMobilePhoneNITZ 	iNitzInfo;
	    
	private:		// Data
		
		/**
		* @var iNitzSrv 
		* @brief Interface to the NITZ server of type CClockNitzListener*
		*/
		CClockNitzListener*			   	iNitzSrv;
		
		/**
		* @var iObserver 
		* @brief This is essentially the plugin which is waiting for notification from the implementations.
		*/
		CClockNitzPlugin*				iNitzPlugin;
	};

#endif		// __CLOCKNITZ_PLUGINIMPL_H__

// End of file
