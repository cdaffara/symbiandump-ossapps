/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  mmsaccount  declaration
*
*/



#ifndef __MMSACCOUNT_H
#define __MMSACCOUNT_H

//  INCLUDES
#include <e32base.h>
#include <es_sock.h>
#include <flogger.h>

// USERINCLUDE FILES
#include "mmssettings.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CMsvScheduleSettings;
class CMsvOffPeakTimes;
class CMsvSendErrorActions;
class CMsvSysAgentActions;

// CLASS DECLARATION
/**
* CMmsAccount is the master class of the component: it offers a centralised API
* for loading and saving different setting types (implemented as different
* settings classes).
* @lib mmsserversettings.lib
* @since 3.0
*/
class CMmsAccount : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
    	IMPORT_C static CMmsAccount* NewL();

        /**
        * Destructor
        */
	    virtual ~CMmsAccount();


    public: 
    
    public: // New functions

        /**
        * Loads MMS Settings from Central Repository to aMmsSettings object
        * given as a parameter.
        * Client is responsible for creating and deleting the parameter objects.
        * @param aMmsSettings object where the settings are loaded to
        */
        IMPORT_C void LoadSettingsL( CMmsSettings& aMmsSettings );

        /**
        * Saves MMS Settings to Central Repository from aMmsSettings given as
        * a parameter.
        * Client is responsible for ensuring that parameter object contains
        * sensible values.
        * Client is responsible for creating and deleting the parameter objects.
        * @param aMmsSettings object containing MMS Settings to be saved
        */
        IMPORT_C void SaveSettingsL( CMmsSettings& aMmsSettings );

        // Note that following settings have only 'load' method: they cannot
        // be saved at runtime.
        
        /**
        * Loads MmsScheduleSendSettings from Central Repository to the objects
        * given as parameters.
        * Client is responsible for creating and deleting the parameter objects.
        * @param aScheduleSettings object will contain related settings
        * @param aOffPeakTimes NOT SUPPORTED.
        *   ACTS AS A PLACE HOLDER FOR FUTURE IMPLEMENTATION
        * @param aErrorActions object will contain related settings
        * @param aSysAgentActions NOT SUPPORTED
        *   ACTS AS A PLACE HOLDER FOR FUTURE IMPLEMENTATION
        */
        IMPORT_C void LoadScheduleSendSettingsL(
            CMsvScheduleSettings& aScheduleSettings,
            CMsvOffPeakTimes& aOffPeakTimes,
            CMsvSendErrorActions& aErrorActions,
            CMsvSysAgentActions& aSysAgentActions );

        /**
        * Loads MmsScheduleReceiveSettings from Central Repository to the objects
        * given as parameters.
        * Client is responsible for creating and deleting the parameter objects.
        * @param aScheduleSettings object will contain related settings
        * @param aOffPeakTimes NOT SUPPORTED
        *   ACTS AS A PLACE HOLDER FOR FUTURE IMPLEMENTATION
        * @param aErrorActions object will contain related settings
        * @param aSysAgentActions NOT SUPPORTED
        *   ACTS AS A PLACE HOLDER FOR FUTURE IMPLEMENTATION
        */
        IMPORT_C void LoadScheduleReceiveSettingsL(
            CMsvScheduleSettings& aScheduleSettings,
            CMsvOffPeakTimes& aOffPeakTimes,
            CMsvSendErrorActions& aErrorActions,
            CMsvSysAgentActions& aSysAgentActions );
            
    public: // Functions from base classes        
        
    protected:  // New functions

    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
        CMmsAccount();

        /**
        * By default EPOC constructor is private.
        * @return void
        */
    	void ConstructL();

    public:     // Data
    protected:  // Data
    private:    // Data

    };

#endif // __MMSACCOUNT_H   
            
// End of File
