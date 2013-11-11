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
* Description:
*
*/
#ifndef IRQENUMS_H_
#define IRQENUMS_H_

#define RETURN_IF_ERR(err)  if( EIRQErrorNone != (err) ) \
  return EIRQErrorGeneral;

enum IRQError
{
    EIRQErrorNone = 0,
    EIRQErrorGeneral = -1000,
    EIRQErrorBadParameter,
    EIRQErrorOutOfMemory,
    EIRQErrorOpenInstanceFailed,
    EIRQErrorNotFound,
    EIRQErrorAlreadyExist,
    EIRQErrorCouldNotConnect,
    EIRQErrorTimeOut,
    EIRQErrorServiceUnavailable,
    EIRQErrorCorrupt,
    EIRQErrorFull,
    
    //player engine errors
    EIRQPlayerErrorGeneral,
    EIRQPlayerErrorConnectingFailed,   //Failed to connect to station
    EIRQPlayerErrorConnectionLost,     //Connection lost
    EIRQPlayerErrorAudioDeviceLost,    //Audio device is occupied by another app with higher priority
    EIRQPlayerErrorSetStereoFailed,    //Set setero effect failed
    EIRQPlayerErrorServerFull,
    EIRQPlayerErrorTimeOut
};

enum IRQTerminatedType
{
    EIRQUnknownTermination = -1,
    //terminated by user
    EIRQUserTerminated = 0,
    //no connection to the server
    EIRQNoConnectionToServer,
    //no connection to the network
    EIRQNoConnectionToNetwork,
    //a call is activated
    EIRQCallIsActivated
};

enum IRQConnectedFrom
{
    //when session started straight from station details in iSDS
    EIRQIsds = 0,
    //when session started from a station details saved to iRAPP favorites and the station is not from iSDS (channel_Id = 0 )
    EIRQPresetAdhoc,
    //when session started from a station details saved to iRAPP favorites and the station is from iSDS (channel_ID is iSDS channel_ID)
    EIRQPresetIsds,
    //when session started from iRAPP 'station play history' and the station is not from iSDS (channel_Id = 0 )
    EIRQHistoryAdhoc,
    //when session started from iRAPP 'station play history' and the station is from iSDS (channel_ID is iSDS channel_ID )
    EIRQHistoryIsds,
    //when session started from manually entered stream url
    EIRQAdhocManual,
    //when iRAPP session started/triggered from any iRAPP external client e.g. Browser or SMS/MMS
    EIRQAdhocExternal,
    // Max value
    EIRQConnectedFromMaxValue
};

enum IRQNmsType
{
    //find in music shop 
    EIRQFind = 0,
    //launch music shop
    EIRQLaunch
};

enum IRQPreferredQuality
{
    EIRQStandardQuality = 0,
    EIRQHighQuality
};

#endif /* IRQERROR_H_ */

