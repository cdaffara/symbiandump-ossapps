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
* Description:   The header file of the CClockNitzPlugin class.
*
*/

#ifndef __CLOCKNITZ_PLUGIN_H__ 
#define __CLOCKNITZ_PLUGIN_H__

// System includes
#include <etelmm.h>

// User includes
#include "clocktimesourceinterface.h"

// Forward declarations
class CClockNitzPluginImpl;

// Constants
static const TInt KNitzPluginId( 0x200159A5 );

// Class declaration
/** 
* @class CClockNitzPlugin
* @brief An instance of class CClockNitzPlugin which is an implementation of the
*		 interface CClockTimeSourceInterface.
* @dll clocknitzplugin.dll 
*/
class CClockNitzPlugin : public CClockTimeSourceInterface
	{
	public:				// Constructor and destructor
	
		/**
		* @brief Creates a CClockNitzPlugin object
		* @return CClockNitzPlugin*
		*/
        static CClockNitzPlugin* NewL();
		
		/**
		* @brief Destroy the object and release all memory objects
		*/
	    virtual ~CClockNitzPlugin();
	    
	public:				// From base classes
	
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
		virtual void NotifyOnTimeChange( TInterfaceAttribute aInterfaceAttribute );
		
		/**
		* @brief Returns the plugin implementation id. Unique for each plugin.
		* @return TInt The Uid of the plugin.
		*/
		TInt GetPluginUid();
		
		/**
		* @brief Notifies any client in case NITZ info is received
		*/
		void NotifyNITZInfoChangeL();
		
	private:			// New functions
		
		/** 
		* @brief Symbian OS 2nd phase constructor is private.
		*/
	    void ConstructL();
		
	private:			// Data
	
		/**
		* @var iNitzPluginImpl
		* @brief An object of type CClockNitzPluginImpl
		*/
		CClockNitzPluginImpl* 			iNitzPluginImpl;
	};

#endif	// __CLOCKNITZ_PLUGIN_H__

// End of file
 
