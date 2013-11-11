/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef LOADGEN_LOADATTRIBUTES_H
#define LOADGEN_LOADATTRIBUTES_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <badesca.h> // CDesCArray

const TInt KCPUSelection_FreeScheduling = -1;
const TInt KCPUSelection_AllCPUs        = 999;

// FORWARD DECLARATIONS


// CLASS DECLARATIONS
class TCPULoadAttributes
    {
public:
    TUint iId;        
    TUint iPriority;        
    TUint iMode;        
    TUint iType;        
    TUint iLength;        
    TUint iIdle;        
    TUint iRandomVariance;
    // iCpu defines in which CPU the load thread should be executed in 
    // (in SMP environments). 0 = CPU0, 1 = CPU1 etc. Negative value means 
    // that CPU is undefined. When CPU is undefined, Scheduler can schedule
    // the load thread freely to any CPU as it pleases.
    TInt  iCpu;
    // Defines how many CPUs the running system has.
    TUint iCpuCount;
    };

class TMemoryEatAttributes
    {
public:
    TUint iId;        
    TUint iPriority;        
    TUint iSource;        
    TUint iType;
    TUint iBuffer;
    TUint iIdle;
    TInt64 iAmount;
    TInt64 iRandomMin;
    TInt64 iRandomMax;
    TBuf<64> iAmountDes;
    TBuf<64> iRandomMinDes;
    TBuf<64> iRandomMaxDes;
    TUint iRandomVariance;        
    TRequestStatus* iDeathStatus;
    }; 

class TPhoneCallAttributes
    {
public:
    TUint iId;        
    TUint iPriority;        
    TBuf<128> iDestination;
    TUint iLength;        
    TUint iIdle;        
    TUint iRandomVariance;        
    TRequestStatus* iDeathStatus;
    };

class TNetConnAttributes
    {
public:
    TUint iId;        
    TUint iPriority;        
    TBuf<128> iDestination;
    TUint iIdle;        
    TUint iRandomVariance;        
    TRequestStatus* iDeathStatus;
    };

class TKeyPressAttributes
    {
public:
    TUint iId;        
    TUint iPriority;        
    TUint iHeartBeat;        
    TUint iRandomVariance;        
    TRequestStatus* iDeathStatus;
    };

class TPointerEventAttributes
    {
public:
    TUint iId;        
    TUint iPriority;        
    TUint iHeartBeat;        
    TUint iRandomVariance;        
    TRequestStatus* iDeathStatus;
    };

class TMessageAttributes
    {
public:
    TUint iId;
    TInt iMessageType;
    TBuf<128> iDestination;
    TUint iPriority; 
    TUint iAmount;
    TInt iLength;    
    TUint iIdle;    
    TUint iRandomVariance;        
    TRequestStatus* iDeathStatus;
    };

class TApplicationsAttributes
    {
public:
    TUint iId;        
    TUint iPriority;
    TUint iLaunchingInterval;
    TUint iKeyPressType;
    TUint iMaxOpen;
    TUint iHeartBeat;
    TUint iRandomVariance;
    CDesCArray* iAppsArray;
    TRequestStatus* iDeathStatus;
    };


class TPhotoCaptureAttributes
    {
public:
    TUint iId;    
    TInt iPriority;
    TInt iCameraCount;
    TInt iCamera;
    TUint iIdle;    
    TUint iRandomVariance;        
    TRequestStatus* iDeathStatus;
    };

class TBluetoothAttributes
    {
public:
    TUint iId;    
    TInt iPriority;    
    TUint iIdle;    
    TUint iRandomVariance;        
    TRequestStatus* iDeathStatus;
    TBool iBluetoothSupported;
    };

#endif
