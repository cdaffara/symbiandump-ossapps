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
* Description:  mmsschedulesettingsutils  declaration
*
*/



#ifndef __MMSSCHEDULESETTINGSUTILS_H
#define __MMSSCHEDULESETTINGSUTILS_H

//  INCLUDES
#include <e32base.h>
#include <es_sock.h>
#include <flogger.h>

// USERINCLUDE FILES
// CONSTANTS
// Error actions are stored in 0x10 increments in CenRep
const TUint32 KMmsErrorActionElementIncrement = 0x00000010;

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CRepository;
class CMsvScheduleSettings;
class CMsvOffPeakTimes;
class CMsvSendErrorActions;
class CMsvSysAgentActions;
class TMsvSendErrorAction;

// CLASS DECLARATION
/**
* TMmsScheduleSettingsUtils is a helper class for CMmsAccount.
* It implements the actual loading operation for schedule settings.
* @lib mmsserversettings.lib
* @since 3.0
*/
class TMmsScheduleSettingsUtils
    {
    public: // New functions

        /**
        * LoadScheduleSendSettingsL loads schedule settings for
        * send operations
        */
    	static void LoadScheduleSendSettingsL(
    	    CMsvScheduleSettings& aScheduleSettings,
    	    CRepository& aRepository );

        /**
        * LoadScheduleReceiveSettingsL loads schedule settings for
        * receive operations
        */
    	static void LoadScheduleReceiveSettingsL(
    	    CMsvScheduleSettings& aScheduleSettings,
    	    CRepository& aRepository );
    	    
        /**
        * LoadOffPeakSettingsL loads offpeak settings
        * CURRENTLY OFFPEAK SETTINGS NOT USED: CONTAINS EMPTY IMPLEMENTATION
        */
    	static void LoadOffPeakSettingsL(
        	CMsvOffPeakTimes& aOffPeakTimes, 
        	CRepository& aRepository );
        	
        /**
        * LoadSendErrorSettingsL loads error settings for send operations
        */
    	static void LoadSendErrorSettingsL(
        	CMsvSendErrorActions& aErrorActions, 
        	CRepository& aRepository );

        /**
        * LoadReceiveErrorSettingsL loads error settings for receive operations
        */
    	static void LoadReceiveErrorSettingsL(
        	CMsvSendErrorActions& aErrorActions, 
        	CRepository& aRepository );
        	
        /**
        * LoadSysAgentSettingsL loads sysagent settings
        * CURRENTLY SYSAGENT SETTINGS NOT USED: CONTAINS EMPTY IMPLEMENTATION
        */
    	static void LoadSysAgentSettingsL(
    	    CMsvSysAgentActions& aSysAgentActions, 
    	    CRepository& aRepository );	
    	    
    	    
    public: // Functions from base classes        
        
    protected:  // New functions

    protected:  // Functions from base classes
        
    private:

        /**
        * LoadScheduleSettingsL loads schedule settings - common code
        * @param aScheduleSettings schedule settings structure to be filled
        * @param aRepository repository where the values are read from
        * @param aLongInterval Cen Rep key for long interval value
        * @param aShortInterval Cen Rep key for short inteval value
        * @param aVariableIntervalsCount Cen Rep key for variable interval count
        * @param aVariableIntervalArrayBase Cen Rep key for variable interval base
        */
    	static void LoadScheduleSettingsL(
    	    CMsvScheduleSettings& aScheduleSettings,
    	    CRepository& aRepository,
            const TUint32 aLongInterval,
            const TUint32 aShortInterval,
            const TUint32 aVariableIntervalsCount,
            const TUint32 aVariableIntervalArrayBase
    	    );
    	    
        /**
        * LoadErrorSettingsL loads error settings for operations - common code
        * @param aErrorActions error action structure to be filled.
        * @param aRepository repository where the actios are read from.
        * @param aDefaultErrorActionId Central Repository key for default error action.
        * @param aErrorActionCount Central Repository key for number of error actions
        * @param aErrorActionBase Central Repository key from which the error actions start
        */
    	static void LoadErrorSettingsL(
        	CMsvSendErrorActions& aErrorActions, 
        	CRepository& aRepository,
        	const TUint32 aDefaultErrorActionId,
        	const TUint32 aErrorActionCount,
        	const TUint32 aErrorActionBase );
        	
        /**
        * Helper method for loading error actions
        */
        static void LoadErrorActionL(
            TUint32 aErrorActionId,
            TMsvSendErrorAction& aErrorAction,
            CRepository& aRepository );

    	/**
    	* Load error codes that cause immediate failure
    	* Same codes for sending and receiving
    	*/        
    	static void LoadImmediateFailuresL(
        	CMsvSendErrorActions& aErrorActions, 
        	CRepository& aRepository );
    	
    public:     // Data
    protected:  // Data
    private:    // Data

    };

#endif // __MMSSCHEDULESETTINGSUTILS_H   
            
// End of File
