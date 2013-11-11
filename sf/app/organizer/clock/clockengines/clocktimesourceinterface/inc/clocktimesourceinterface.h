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
* Description:   The header file for the CClockTimeSourceInterface class.
*
*/

#ifndef __CLOCK_TIMESOURCE_INTERFACE_H__
#define __CLOCK_TIMESOURCE_INTERFACE_H__

// System includes
#include <e32std.h>
#include <e32base.h>

// User includes
#include "clocktimesourceinterface.hrh"

// Forward declarations
class MClockPluginObserver;

/** 
* @brief This is an interface class that needs to be implemented by
*        every ECOM based plugin which updates the device time.
* @dll clocktimesourceinterface.dll
*/
class CClockTimeSourceInterface : public CBase
	{
	public:			// Constructor and Destructor

		/**
		* @brief Two phases constructor, to construct the object of the 
		*        ECOM implementation referred to by the aImpId.
		* @param aImpId Implementation id of the ECOM implementation to be loaded.
		* @return CClockTimeSourceInterface* Pointer to an instance of aImpId.
		*/
		IMPORT_C static CClockTimeSourceInterface* NewL( const TUid& aPluginId,
    								  					 MClockPluginObserver* aObserver );
		
		/**
		* @brief Destroy the object and release all memory objects
		*/
	  	IMPORT_C ~CClockTimeSourceInterface();

	public:			// New functions

		
		/**
		* @brief Gets the UTC Time, pure virtual, to be implemented by the plugin.
		* @param aInterfaceAttribute anyone of enum TInterfaceAttribute defined in clocktimesourceinterface.hrh
		* @param aTimeAttributes The type of data requested
		*/
		IMPORT_C virtual void GetTimeInformationL( TInterfaceAttribute aInterfaceAttribute,
		                                           STimeAttributes* aTimeAttributes ) = 0;
		
		/**
		* @brief Gets the UTC Time, pure virtual, to be implemented by the plugin.
		* @param aInterfaceAttribute anyone of enum TInterfaceAttribute defined in clocktimesourceinterface.hrh
		* @param aPtr TAny* to the type of data requested
		*/
		IMPORT_C virtual void NotifyOnTimeChange( TInterfaceAttribute aInterfaceAttribute ) = 0;
		
		/**
		* @brief Returns the plugin implementation id. Unique for each plugin.
		* @return TInt The Uid of the plugin.
		*/
		IMPORT_C virtual TInt GetPluginUid() = 0;
						
	private:		// New functions
	
		/**
		* @brief Symbiam OS 2nd Phase construct, performs the 2nd phase construction.
		* @param aPluginId The UID of the plugin that needs to be constructed
		* @param aObserver An observer of type MClockPluginObserver
		*/
		void ConstructL( MClockPluginObserver* aObserver );
		
	protected:		// Data

		/**
		* @var iObserver
		* @brief This is essentially the ClockServer which is waiting for notification from the implementations.
		*/
		MClockPluginObserver*           iObserver;
		
		/**
		* @var iPluginId
		* @brief The plugin UID.
		*/
		TUid                            iPluginId;
		
	private:       // Data

        /**
        * @var iDtor_ID_key
        * @brief Implementation UID of the Time updater implementation.
        */
        TUid                            iDtor_ID_key;
	};

#endif		// __CLOCK_TIMESOURCE_INTERFACE_H__

// End of file
