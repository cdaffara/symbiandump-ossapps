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
* Description:  The implementation for presentation elements.
*
*/


#ifndef T_IRSESSIONPARAMS_H
#define T_IRSESSIONPARAMS_H

#include <e32cmn.h>

// Browse Log Url Class
class TIRBrowseLog
    {    
public:
    //browse url    
    TBuf<256>     iBrowseUrl;
    //time stamp
	TBuf<32> 	iTimeStamp;	
    //connected from
	TBuf<16> 	iCurrentNetwork;
    TInt          iStatus;    //TO check the status for browse url,1 for channel BRowse url
    }; 


// Channel Server Results
class TIRServer
    {
public:
    //channel url
    TBuf<256> iServerUrl;
    //channel url connection result
	TBuf<32>  iServerResult;
    TInt       iCount;
    };

// Song Recog Log
class TIRSongRecogLog
    {
public:
    TBuf<64> iTimeStamp;
    };

enum TTerminationStatus
    {
    //terminated by user
    EUserTerminated = 0,
    //no connection to the server
    ENoConnectionToServer,
    //no connection to the network
    ENoConnectionToNetwork,    
    };
    
enum TConnectedFrom
	{
    //when session started straight from station details in iSDS
    EIRIsds = 0,
    //when session started from a station details saved to iRAPP favorites and the station is not from iSDS (channel_Id = 0 )
    EIRPresetAdhoc,
    //when session started from a station details saved to iRAPP favorites and the station is from iSDS (channel_ID is iSDS channel_ID)
    EIRPresetIsds,
    //when session started from iRAPP 'station play history' and the station is not from iSDS (channel_Id = 0 )
    EIRHistoryAdhoc,
    //when session started from iRAPP 'station play history' and the station is from iSDS (channel_ID is iSDS channel_ID )
    EIRHistoryIsds,
    //when session started from manually entered stream url
    EIRAdhocManual,
    //when iRAPP session started/triggered from any iRAPP external client e.g. Browser or SMS/MMS
    EIRAdhocExternal
	};

enum TResponse
    {
    //connected 
    EIRConnected = 0,
    //connection failed because server is full (not responding)
    EIRFull,
    //connection timed out
    EIRConnTimeOut,
    //connection failed generic senerio like invalid url    
    EIRConnFailed,
    };

#endif //T_IRSESSIONPARAMS_H
