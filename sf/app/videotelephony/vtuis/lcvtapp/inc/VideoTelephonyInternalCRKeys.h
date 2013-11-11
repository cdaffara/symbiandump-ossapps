/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video Telephony internal Central Repository keys
*
*/


#ifndef VIDEOTELEPHONYINTERNALCRKEYS_H
#define VIDEOTELEPHONYINTERNALCRKEYS_H

#include <e32std.h>

/**
* Video Telephony Variation API
* Keys under this category are used in defining video telephony variation.
*/
const TUid KCRUidVTVariation = {0x102072D0};

/**
* Video Telephony Local Variation Flags. Values are defined in 
* VideoTelephonyVariant.hrh.
*/
const TUint32 KVTLocalVariationFlags = 0x00000001;

/**
* Video Telephony Configuration API
* Keys under this category are used for configuring video call behaviour.
* @since S60 3.1
*/
const TUid KCRUidVTConfiguration = {0x10281872};

/**
* This key controls minimum, normal and maximum frame rate for video encoder.
* Integer values in the string are between 0 and 31 separated by space.
* Example for key value is "5 10 5".
*/
const TUint32 KVTVideoFrameRates = 0x00000001;

/**
* Defines layout (screen orientation) used by Video Telephone application 
* per active camera. Each camera has one of the following configurations:
* 1) "LS" = Landscape
* 2) "PR" = Portrait
* 3) "OL" = Obey device layout (normal operation)
* Camera values are:
* 1) "C1" = VT primary camera (inwards)
* 2) "C2" = VT secondary camera (outwards)
*  Example value (portrait for cam1, landscape for cam2): C1:PR C2:LS
*/
const TUint32 KVTCameraLayout = 0x00000002;

/**
* Lists implementation UIDs for MPEG4, H.263 and H.264 encoders for video 
* telephony in hexadecimal separated by space
*/
const TUint32 KVTVideoEncoderUIDs = 0x00000003;

/**
* Lists implementation UIDs for MPEG4, H.263 and H.264 decoders for video 
* telephony in hexadecimal separated by space
*/
const TUint32 KVTVideoDecoderUIDs = 0x00000004;

/**
* Lists implementation UIDs for AMR-NB and future audio encoders for video
* telephony (in hex, separated by space)
*/
const TUint32 KVTAudioEncoderUIDs = 0x00000005;

/**
* Lists implementation UIDs for AMR-NB and future audio decoders for video 
* telephony (in hex, separated by space)
*/
const TUint32 KVTAudioDecoderUIDs = 0x00000006;
                                               
/**
* Lists processing delay for MPEG4, H.263 and H.264 encoders for video 
* telephony in unsigned 32-bit integers separated by space
*/
const TUint32 KVTVideoEncoderDelay = 0x00000007;

/**
* Lists processing delay for MPEG4, H.263 and H.264 decoders for video 
* telephony in unsigned 32-bit integers separated by space
*/
const TUint32 KVTVideoDecoderDelay = 0x00000008;

/**
* Lists processing delay for AMR-NB and future audio encoders for video
* telephony (in unsigned 32-bit integers, separated by space)
*/
const TUint32 KVTAudioEncoderDelay = 0x00000009;

/**
* Lists processing delay for AMR-NB and future audio decoders for video 
* telephony (in unsigned 32-bit integers, separated by space)
*/
const TUint32 KVTAudioDecoderDelay = 0x0000000A;

/*
* Defines number of toolbar buttons. Three buttons is minimum
* and seven is maximum.
*/
//const TUint32 KVTToolbarCommands  = 0x0000000B;

#endif      // VIDEOTELEPHONYINTERNALCRKEYS_H

// End of file