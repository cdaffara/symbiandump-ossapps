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


// INCLUDE FILES
#include "loadgen_bluetooth.h"
#include "loadgen_utils.h"
#include "loadgen.hrh"
#include "loadgen_traces.h"
#include <loadgen.rsg>
#include <e32hal.h>
#include <u32hal.h>
#include <e32math.h>
#include <eikenv.h>
#include <btserversdkcrkeys.h>
#include <btnotifierapi.h>
#include <btpm.h>

_LIT(KThreadName, "Bluetooth %d");
_LIT(KBTLinkManagerStr, "BTLinkManager");

const TInt KDefaultStart = 50;
const TInt KDefaultPeriod = 5000000;
   
// ===================================== MEMBER FUNCTIONS =====================================

CBluetooth* CBluetooth::NewL(TBluetoothAttributes& aAttributes, TInt aReferenceNumber)
    {
    CBluetooth* self = new(ELeave) CBluetooth(aAttributes, aReferenceNumber);
    CleanupStack::PushL(self);
    TRAPD(err, self->ConstructL());
    if( err != KErrNone )
        {
        User::Leave(err);
        }
    CleanupStack::Pop(self);
    return self;    
    }

// --------------------------------------------------------------------------------------------

CBluetooth::~CBluetooth()
    {
    Close();
    }

// --------------------------------------------------------------------------------------------

CBluetooth::CBluetooth(TBluetoothAttributes& aAttributes, TInt aReferenceNumber) : iAttributes(aAttributes)
    {
    iAttributes.iId = aReferenceNumber;
    }

// --------------------------------------------------------------------------------------------

void CBluetooth::ConstructL()
    {    
    CLoadBase::ConstructL();
    
    iType = ELoadGenCmdNewLoadBluetooth;
    
    InitializeBluetoothL();
                        
    TBuf<64> threadName;
    threadName.Format(KThreadName, iAttributes.iId);
            
    // create a thread
    User::LeaveIfError(iThread.Create(threadName, ThreadFunction, KDefaultStackSize*2, KMinHeapSize, 1024*KMinHeapSize, (TAny*) &iAttributes ));
    
    // set priority of the thread
    SetPriority();
    iState = ELoadStateConstructed;
    }

// --------------------------------------------------------------------------------------------

TInt CBluetooth::ThreadFunction(TAny* aThreadArg)
    {
    TInt err = KErrNone;
    
    CTrapCleanup* pC = CTrapCleanup::New();
    CActiveScheduler* pS = new CActiveScheduler;
    CActiveScheduler::Install(pS);

    // start generating load, pass pointer to arguments
    GenerateLoad(*((TBluetoothAttributes*) aThreadArg));

    delete pS;
    delete pC;
    
    return err;
    }

// --------------------------------------------------------------------------------------------

void CBluetooth::GenerateLoad(TBluetoothAttributes& aAttributes)
    {    
    CBTManager* btManager = 0;
    TRAPD(err, btManager = CBTManager::NewL(aAttributes));
    if (err == KErrNone) CActiveScheduler::Start();
    delete btManager;    
    }

// --------------------------------------------------------------------------------------------

void CBluetooth::Resume()
    {    
    CLoadBase::Resume();    
    iThread.Resume();
    }

// --------------------------------------------------------------------------------------------

void CBluetooth::Suspend()
    {
    CLoadBase::Suspend();    
    iThread.Suspend();
    }

// --------------------------------------------------------------------------------------------

void CBluetooth::SetPriority()
    {
    CLoadBase::SetPriority();    
    iThread.SetPriority(CLoadGenUtils::SettingItemToThreadPriority(iAttributes.iPriority));
    }
    
// --------------------------------------------------------------------------------------------

void CBluetooth::Close()
    {
    LOGSTRING2("LoadGen: CBluetooth::~CBluetooth() - State: %d", iState);
    if( iState != ELoadStateInvalid )
        {
        CLoadBase::Close();        
        // kill the thread immediately
        iThread.Kill(0);
        iThread.Close();
        }
    }
    
// --------------------------------------------------------------------------------------------
TPtrC CBluetooth::Description()
    {
    TBuf<256> buf;
    TBuf<16> prioBuf;
    CLoadGenUtils::SettingItemToThreadDescription(iAttributes.iPriority, prioBuf);
        
    _LIT(KBluetoothEntry, "[%d] Bluetooth prio=%S idle=%dms random=%d%%");
    buf.Format(KBluetoothEntry, iAttributes.iId, &prioBuf, iAttributes.iIdle, iAttributes.iRandomVariance);
       
    return TPtrC(buf);
    }

// --------------------------------------------------------------------------------------------
void CBluetooth::InitializeBluetoothL()
    {
    // Create central repository for checking and setting bluetooth power state    
    CRepository* btPowerStateCRepo = CRepository::NewL(KCRUidBluetoothPowerState);
    
    // Check if the BT is already turned on:    
    TInt btPowerState = 0;
    btPowerStateCRepo->Get(KBTPowerState, btPowerState);
    delete btPowerStateCRepo;
    btPowerStateCRepo = 0;
        
    if(btPowerState == EBTPowerOff)
        {
        // Switch bt power on:
        if (SetBTPowerState(ETrue) == EFalse)
            {
            LOGSTRING("LoadGen: Bluetooth could not be switched on");
            User::Leave(KErrNotReady);
            }
        }
    }

// --------------------------------------------------------------------------------------------
TBool CBluetooth::SetBTPowerState(TBool aState)
    {    
    // Ask user to turn on the bluetooth device:
    RNotifier btPowerNotifier;
    TInt errCode = btPowerNotifier.Connect();
    
    TBool retVal = errCode == KErrNone;
    
    if( errCode == KErrNone )
        {
        TPckgBuf<TBool> powerRequest(aState);
        TPckgBuf<TBool> powerReply(EFalse);
        TRequestStatus powerRequestStatus;
        btPowerNotifier.StartNotifierAndGetResponse(powerRequestStatus, 
                                                KPowerModeSettingNotifierUid, 
                                                powerRequest, 
                                                powerReply);
        User::WaitForRequest(powerRequestStatus);
        btPowerNotifier.CancelNotifier(KPowerModeSettingNotifierUid);
        btPowerNotifier.Close();  
        if( powerReply() == EFalse )
            {
            LOGSTRING("LoadGen: Bluetooth activating cancelled.");
            }
        retVal = powerReply();
        }
    
    return retVal; 
    }

// --------------------------------------------------------------------------------------------
CBTManager* CBTManager::NewL(   TBluetoothAttributes& aAttrs)
    {
    CBTManager* self = new (ELeave) CBTManager( aAttrs);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
 
// --------------------------------------------------------------------------------------------
CBTManager::~CBTManager()
    {    
    if (iPeriodicTimer)
        {
        iPeriodicTimer->Cancel();
        delete iPeriodicTimer;
        }    
    iHostResolver.Close();        
    Cancel();
    }
 
// --------------------------------------------------------------------------------------------
CBTManager::CBTManager( TBluetoothAttributes& aAttrs) 
: 
CActive(EPriorityStandard),
iAttributes(aAttrs)
    {    
    }
 
// --------------------------------------------------------------------------------------------
void CBTManager::ConstructL()
    {
    // Socket server session initiation:
    User::LeaveIfError(iSocketServerHnd.Connect());    

    CActiveScheduler::Add(this);
    // set the status as pending
    iStatus = KRequestPending;
    SetActive();
        
    // set the death status pointer point to the request status of this ao
    iAttributes.iDeathStatus = &iStatus;
    
    // start timer    
    iPeriodicTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    iPeriodicTimer->Start(KDefaultStart, KDefaultPeriod, TCallBack(PeriodicTimerCallBack, this));
    }

// --------------------------------------------------------------------------------------------
void CBTManager::DoCancel()
    {
    LOGSTRING("LoadGen: CBTManager::DoCancel()");    
    }

// --------------------------------------------------------------------------------------------
void CBTManager::RunL()
    {    
    // request status has completed by the main thread meaning that we need to stop now
    CActiveScheduler::Stop();    
    }

// --------------------------------------------------------------------------------------------
TInt CBTManager::StartBTDeviceDiscovery()
    {
    TInt retVal = KErrNone; 
    RThread myThread;
           
    // Initialize host resolver      
    iHostResolver.Close();        
    TProtocolDesc protocolInfo;
    retVal = iSocketServerHnd.FindProtocol(KBTLinkManagerStr(), protocolInfo);
    if( retVal == KErrNone )
        {
        retVal = iHostResolver.Open(iSocketServerHnd, protocolInfo.iAddrFamily, protocolInfo.iProtocol);        
        }
    
    if( retVal == KErrNone )
        {
        TInquirySockAddr socketAddress;
        socketAddress.SetIAC( KGIAC );
        socketAddress.SetAction(KHostResInquiry|KHostResName|KHostResIgnoreCache);
        retVal = iHostResolver.GetByAddress(socketAddress, iBTNameEntry);        
        // Loop all discovered devices:
        while( retVal == KErrNone )        
            {
            LOGSTRING3("LoadGen: Thread %S found device: %S", &(myThread.Name()), &(iBTNameEntry().iName) );
            // Get next device
            retVal = iHostResolver.Next(iBTNameEntry);            
            }
        }
    
    // KErrEof is returned when no more devices to loop
    if( retVal == KErrEof )
        {
        retVal = KErrNone;
        }
            
    iPeriodicTimer->Start( CLoadGenUtils::MilliSecondsToMicroSeconds( iAttributes.iIdle,
                        iAttributes.iRandomVariance ), KDefaultPeriod, 
                        TCallBack( PeriodicTimerCallBack, this ) );

    return retVal;
    }

// --------------------------------------------------------------------------------------------

TInt CBTManager::PeriodicTimerCallBack(TAny* aAny)
    {        
    CBTManager* self = static_cast<CBTManager*>( aAny );
    self->iPeriodicTimer->Cancel();
    // Perform device discovery:
    self->StartBTDeviceDiscovery();
    return KErrNone;
    }

// End of File
