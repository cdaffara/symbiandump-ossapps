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
* Description: 
*     Contains extension constants used in Logs Engine
*
*/


#ifndef __LogsApiConsts_H__
#define __LogsApiConsts_H__

//Symbian phone caller type strings are defined in LOGWRAP.RLS (Z:\src\common\generic\syslibs\logeng\ongoing\LogWrap).
//Below are additional strings for caller type strings handling.
_LIT( KLogsPrivateText,                 "Private" );    //Text in remote party in case of private number
_LIT( KLogsPayphoneText,                "Payphone" );   //Text in remote party in case of payphone number

// Strings for ClogEvent Data field tagging
_LIT8( KLogsDataFldNameDelimiter,       "\t" );         //Name=value pairs separated by tabs
_LIT8( KLogsDataFldValueDelimiter,      "=" );          //Names and values separated by =
_LIT8( KLogsDataFldTag_CNAP,            "CNAP" );       //Caller name presentation
_LIT8( KLogsDataFldTag_Emergency,       "EMERG" );      //Emergency calls
_LIT8( KLogsDataFldTag_MA,              "MA" );         //My address 
_LIT8( KLogsDataFldTag_POC,             "POC" );        //Push-to-talk
_LIT8( KLogsDataFldTag_Type,            "TYPE" );       //Internal type information for Poc 
_LIT8( KLogsDataFldTag_IP,              "VOIP" );       //Voice over ip
_LIT8( KLogsDataFldTag_VT,              "VT" );         //Video telephony
_LIT8( KLogsDataFldTag_URL,             "URL" );        //For sip-uri etc 
_LIT8( KLogsDataFldTag_ServiceId,       "SI");          //ServiceId
_LIT8( KLogsDataFldTag_ContactLink,     "CL");          //ContactLink
// _LIT8( KLogsDataFldTag_DataSent,     "DS" );
// _LIT8( KLogsDataFldTag_DataReceived, "DR" );


//Symbian messaging delivery status strings are defined in LOGWRAP.RLS (Z:\src\common\generic\syslibs\logeng\ongoing\LogWrap).
//Below are additional strings for delivery status handling.
_LIT(KLogsMsgReadText,                  "MsgRead");


//Symbian LogDB event types are defined in LOGENG.H  / LOGWRAP.HRH. Additional event types that not provided 
//by Symbian LogEngine yet, are provided here. 
//Additional event UIDs:
//The range 0x100058B3 to 0x10005C9A has been assigned to Series 60 by Symbian, i.e. 
//new event UID values can be allocated within this range if not (yet) available by Symbian Log engine.

#define KLogsEngMmsEventType            0x1000595e
#define KLogsEngWlanEventType           0x1000595f      //KLogsEngMmsEventType+1 for WLAN Instead of 0x101F8EB7  
#define KLogsEngPocInfoEventType        0x10005960

const TUid KLogsEngMmsEventTypeUid =    {KLogsEngMmsEventType};
const TUid KLogsEngWlanEventTypeUid =   {KLogsEngWlanEventType};
const TUid KLogsEngPocInfoEventTypeUid= {KLogsEngPocInfoEventType}; 

#endif  // __LogsApiConsts_H__
           
// End of File
