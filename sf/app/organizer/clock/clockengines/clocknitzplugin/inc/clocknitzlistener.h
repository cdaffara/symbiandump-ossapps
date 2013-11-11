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
* Description:   The header file for the CClockNitzListener class.
*
*/

#ifndef __CLOCK_NITZ_LISTENER_H__
#define __CLOCK_NITZ_LISTENER_H__

// System includes
#include <e32base.h>
#include <etelmm.h>

// User includes
#include "clocktimesourceinterface.h"

// Forward declarations
class CClockNitzPluginImpl;

// Class declaration
/** 
* @class CClockNitzListener
* @brief This is an interface to the ETel server
* @dll clocknitzplugin.dll
*/
class CClockNitzListener : public CActive
    {
    public: 			// Constructors and destructor

		enum { ENitzInvalidUtcOffsetMin = -1 };
        
        /**
        * @brief First phase constructor.
        * @param aObserver CClockNitzPluginImpl*
        * @return CClockNitzListener*
        */
        static CClockNitzListener* NewL( CClockNitzPluginImpl* aObserver );

        /**
        * @brief Destructor
        */
        ~CClockNitzListener();

    public: 			// New functions

        /**
        * @brief Starts the active object to listen to NITZ notifications
        */
        void Start();

        /**
        * @brief Stops the active object
        */
        void Stop();

        /**
        * @brief Gets the current NITZ information.
        * @param aNITZInfo The infomation to be set.
        * @return an error code
        */
        TInt GetCurrentNITZInfoL( RMobilePhone::TMobilePhoneNITZ& aNitzInfo );

        /**
        * @brief Notifies the observer whenever the NITZ information changes
        */
        void NotifyObserverL();


   protected: 			// Functions from base classes
		
		/**
        * @brief From CActive. Make NITZ active.
        */
        void RunL();

        /**
        * @brief From CActive. Make NITZ inactive.
        */
        void DoCancel();

    private:    		// New functions

        /**
        * @brief Default constructor
        * @param aServer CClockNitzPluginImpl*
        */
        CClockNitzListener( CClockNitzPluginImpl* aObserver );

        /**
        * @brief Second phase constructor
        */
        void ConstructL();

        /**
        * @brief Checks the date time value is valid.
        * @param aDateTime
        * @return error code
        */
        TInt CheckDateTimeVal( const TDateTime& aDateTime );
        
    private:            // Data
    	
    	/**
		* @var iNitzUtcOffsetMin 
		* @brief TTimeIntervalMinutes Contains the standard UTC offset.
		*/
    	TTimeIntervalMinutes				iNitzUtcOffsetMin;
    	
    	/**
		* @var iEtelServer 
		* @brief RTelServer Interface to the ETel server.
		*/
        RTelServer                      	iEtelServer;
        
        /**
		* @var iPhone 
		* @brief RMobilePhone Interface to the MobilePhone server.
		*/
        RMobilePhone						iPhone;
		
		/**
		* @var iObserver 
		* @brief CClockNitzPluginImpl Observer/Client waiting for NITZ info.
		*/
        CClockNitzPluginImpl*           	iObserver;
        
        /**
		* @var iNitzInfo 
		* @brief Member of type RMobilePhone::TMobilePhoneNITZ.
		*/
        RMobilePhone::TMobilePhoneNITZ		iNitzInfo;
    };

#endif  // __CLOCK_NITZ_LISTENER_H__

// End of file

