/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#ifndef IRPUBSUBKEYS_H
#define IRPUBSUBKEYS_H

#include <e32def.h>
#include <e32cmn.h>

/** 
 * enum to indicate the state of iRAPP application
 */
enum TIRPSApplicationRunningState
    {
    EIRPSApplicationUninitialized,
    EIRPSApplicationRunning,
    EIRPSApplicationClosing        
    };

/**
 * enum to indicate the state of chanel playing
 */
enum TIRStopPlayState
    {
    EIRStateUninitialized,
    EIRStatePlay,
    EIRStateStop
    };
    
    
/**
 * enum to indicate the state of chanel playing
 */
enum TIRBufferingState
    {
    EIRStateBufferingStart,
    EIRStateBufferingStop,
    EIRStateBufferingError
    };

/** 
 * IR Publish and Subscribe keys base value
 */
const TUint32 KIRPubSubBase = { 0x00000020 } ;

/** 
 * IR Publish and Subscribe key to indicate prest index  change ( For XML Active Idle )
 */
const TUint32 KIRPSPresetIndex = { KIRPubSubBase + 0x00000000 };

/** 
 * IR Publish and Subscribe key to indicate if iRAPP is running ( For XML Active Idle )
 */
const TUint32 KIRPSDataApplicationRunning = { KIRPubSubBase + 0x00000001 };

/** 
 * This key is deprecated and the information is now available via KIRPSMetaData. 
 */
const TUint32 KIRPSArtistTitleChanged = { KIRPubSubBase + 0x00000002 };

/** 
 * This key is deprecated and the information is now available via KIRPSMetaData. 
 */
const TUint32 KIRPSSongTitleChanged = { KIRPubSubBase + 0x00000003 };

/** 
 * IR Publish and Subscribe key to indicate channel change ( For XML Active Idle )
 */
const TUint32 KIRPSDataChannel = { KIRPubSubBase + 0x00000004 };

/** 
 * IR Publish and Subscribe key to indicate volume change ( For XML Active Idle )
 */
const TUint32 KIRPSVolume = { KIRPubSubBase + 0x00000005 };

/** 
 * IR Publish and Subscribe key to indicate stop state ( For XML Active Idle )
 */
const TUint32 KIRPSPlayStop = { KIRPubSubBase + 0x00000006 };

/** 
 * IR Publish and Subscribe key to specify the channel server url
 */
const TUint32 KIRPSChannelUrl = { KIRPubSubBase + 0x00000007 };

/** 
 * IR Publish and Subscribe key to specify the Bitrate
 */
const TUint32 KIRPSBitrate = { KIRPubSubBase + 0x00000008 };

/** 
 * IR Publish and Subscribe key to Handle simultaneous play
 */
const TUint32 KIRPSMultiplePlay = { KIRPubSubBase + 0x00000009 };

/** 
 * IR Publish and Subscribe key to specify the channel server url
 */
const TUint32 KIRPSStationName = { KIRPubSubBase + 0x0000000A };  

/** 
 * IR Publish and Subscribe key to specify the number of application
 * instances running
 */
const TUint32 KIRPSAppInstanceRunning = { KIRPubSubBase + 0x0000000B };  

/**
 * Updated whenever meta data has been received for the channel that is currently streaming.
 *
 * As this data is packed in binary form, listeners of this key can use the following code snippet to support
 * unpacking it:
 *
 * #include <irmetadata.h>
 *
 * In your HandlePropertyChangeL( const TDesC8& aData ) method:
 *
 * if ( aKey == KIRPSMetaData )
 *     {
 *     CIRMetaData* metaData = CIRMetaData::NewL( aData ); 
 *     CleanupStack::PushL( metaData );
 *     // Meta data information is now available via the public access methods of CIRMetaData.
 *     // Do whatever needs to be done with the meta data information here.
 *     const TDesC& artist = metaData->Artist();
 *     ...
 *     CleanupStack::PopAndDestroy( metaData );
 *     }
 */
const TUint32 KIRPSMetaData = { KIRPubSubBase + 0x0000000C };

const TUint32 KIRPSChannelType = { KIRPubSubBase + 0x0000000D };

const TUint32 KIRPSChannelId = { KIRPubSubBase + 0x000000E };

const TUint32 KIRPSDataChannelDesc = { KIRPubSubBase + 0x0000000F };

const TUint32 KIRPSChannelMusicFlag = { KIRPubSubBase + 0x00000010 };

const TUint32 KIRPSControlSetPlayState = { KIRPubSubBase + 0x00000011 };


const TUint32 KIRPSBufferingState = { KIRPubSubBase + 0x00000012 };

/********************************************************************
*  KIRPubSubBase + 0x00000015 should be reserved for startup key which is defined in \qtinternetradio\irqcommon\inc\irservicedef.h
*const unsigned long int KInternetRadioStartupKey = 0x00000035; // = KIRPubSubBase + 0x00000015
*/

#endif // IRPUBSUBKEYS_H

