/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Definitions of domain keys related to MmsEngine settings in CenRep
*
*/



#ifndef MMSENGINEDOMAINCRKEYS_H
#define MMSENGINEDOMAINCRKEYS_H

const TUid KCRUidMmsEngine = {0x100058DB};
const TUid KCRUidMMSApplicationRegistrations = {0x101FB0D9};

/**
* KMmsEngineMaximumSendSize 
* Defines the maximum possible size for an MMS to be sent in bytes.
* It is application's responsibility to assert this.
* Default value: 307200 ( = 300*1024 )
*/
const TUint32 KMmsEngineMaximumSendSize =               0x00000010;

/**
* KMmsEngineMaximumReceiveSize 
* Defines the maximum possible size for a received MMS in bytes.
* This value should be at least 4 kilobytes larger than KMmsEngineMaximumSendSize
* in order reserve space for MMS headers and attachment MIME headers.
* Default value: 311296 ( = 304*1024 )
*/ 
const TUint32 KMmsEngineMaximumReceiveSize =            0x00000011;

/**
* KMmsEngineCreationMode 
* Defines what kind of MMS messages can be created and sent.
* It is application's responsibility to assert this.
* Possible values:
* 0 ( Restricted )
* 1 ( Warning )
* 2 ( Free )
* Default value: 1
*/
const TUint32 KMmsEngineCreationMode =                  0x00000013;

/**
* KMmsEngineImageWidth 
* Defines image width in pixels.
* This is for application level use.
* Default value: 640
*/
const TUint32 KMmsEngineImageWidth =                    0x0000000B;

/**
* KMmsEngineImageHeight 
* Defines image height in pixels.
* This is for application level use.
* Default value: 480
*/
const TUint32 KMmsEngineImageHeight =                   0x0000000C;

/**
* KMmsEngineNbrOfRegisteredApplications 
* Defines the number of elements in the array of registered applications
* Default value: 0
*/
const TUint32 KMmsEngineNbrOfRegisteredApplications =	0x01000000;

/**
* KMmsEngineRegisteredApplicationsArrayBase 
* Defines the base id for registered application id array.
* The elements of the array consist of descriptors. 
*/
const TUint32 KMmsEngineRegisteredApplicationsArrayBase = 0x00000000;

/**
* KMmsEngineApplicationNameArrayBase 
* Defines the base id for registered application name array.
* The elements of the array consist of descriptors. 
*/
const TUint32 KMmsEngineApplicationNameArrayBase = 0x00800000;

/**
* KMmsEngineAccessPointCount 
* Defines the number of elements in the accesspoint array
* Default value: 0
*/
const TUint32 KMmsEngineAccessPointCount =              0x0000001A;

/**
* KMmsAccesspointArrayBase 
* Defines the base id for accespoint array. The elements of the array consist
* of 32 bit integers. There can be 'KMmsAccesspointArrayMaxSize' elements
* in the array.
*/
const TUint32 KMmsAccesspointArrayBase =                0x00001000;

/**
* KMmsOmaEncapsulationVersion 
* OMA MMS encapsulation version number.
* Possible values:
* 18 ( 0x12, OMA MMS 1.2 )
* 19 ( 0x13, OMA MMS 1.3 )
* Default value: 18
*/
const TUint32 KMmsOmaEncapsulationVersion =             0x00000022;

#endif      // MMSENGINEDOMAINCRKEYS_H

