/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Logs domain Central Repository keys
*
*/



#ifndef LOGSDOMAINCRKEYS_H
#define LOGSDOMAINCRKEYS_H

/**************************************************************************/
/** Logs Timers API */
/** Provides access to the timers/counters related to Logs processing */

const TUid KCRUidLogs = {0x101F874E};


/**
* Actual last call timer
* This timer must be updated every time a call is ended.
* Integer type
**/
const TUint32 KLogsActualLastCallTimer      = 0x00000000;

/**
* Dialled calls timer to ALS Line 1
* This is incremented every time when call on Line 1 is ended
* Integer type
**/
const TUint32 KLogsDialledCallsTimerLine1   = 0x00000001;

/**
* Dialled calls timer to ALS Line 2
* This is incremented every time when call on Line 2 is ended
* Integer type
**/
const TUint32 KLogsDialledCallsTimerLine2   = 0x00000002;

/**
* Last call timer to ALS Line 1
* This timer must be updated every time a call is ended in Line 1.
* Integer type
**/
const TUint32 KLogsLastCallTimerLine1       = 0x00000003;

/**
* Last call timer to ALS Line 2
* This timer must be updated every time a call is ended in Line 2.
* Integer type
**/
const TUint32 KLogsLastCallTimerLine2       = 0x00000004;

/**
* Informs other applications that is the Logs application's
* logging enabled.
* Note! If this has been set OFF, it does not mean that you
* will not log. If your application is meant to log, it will
* log always.
* Integer type
* 0 (OFF)
* 1 (ON)
*
* Default value: 1
**/
const TUint32 KLogsLoggingEnabled           = 0x00000005;

/**
* Informs the Logs application about the amount of new missed calls.
* Integer type
**/
const TUint32 KLogsNewMissedCalls                   = 0x00000006;

/**
* Received calls timer to ALS Line 1
* This is incremented every time when received call on Line 1
* is ended
* Integer type
**/
const TUint32 KLogsReceivedCallsTimerLine1  = 0x00000007;

/**
* Received calls timer to ALS Line 2
* This is incremented every time when received call on Line 2
* is ended
* Integer type
**/
const TUint32 KLogsReceivedCallsTimerLine2  = 0x00000008;


//FIXME: THIS IS NOT ANYMORE PART OF LOGS APPLICATION, SO IT SHOULD BE MOVED TO 
//DEVICEMANAGEMENT DOMAIN
/**
* Inidicates whether call duration is shown or not in Phone Application
*
* Integer, possible values are:
*
* 0 (call duration not shown in Phone application)
* 1 (call duration shown Phone application)
*
* Default value: 0
**/
const TUint32 KLogsShowCallDuration    = 0x00000009;


/**
* This key is required for VoIP Last Call Timer
*/
const TUint32 KLogsLastCallTimerVoIP = 0x0000000A;
 
/**
* This key is required for VoIP Received Calls Timer
*/
const TUint32 KLogsReceivedCallsTimerVoIP = 0x0000000B;
 
/**
* This key is required for VoIP Dialled Calls Timer
*/
const TUint32 KLogsDialledCallsTimerVoIP = 0x0000000C;

/**
* KLogsPredictiveSearch
* Controls the feature of predictive search in Logs matches view
* 0 - Predictive search is permanently Off, can't be switched on from Logs
* 1 - Predictive search is On
* 2 - Predicitve search is temporarily Off, can be switched on from Logs
*/
const TUint32 KLogsPredictiveSearch = 0x0000000D;

/**
* KLogsFakeExitEnabled
* Controls the fake exiting feature
* 0 - Fake exit is disabled
* 1 - Fake exit is enabled
*/
const TUint32 KLogsFakeExitEnabled = 0x0000000E;

/**************************************************************************/


#endif      // LOGSDOMAINCRKEYS_H
