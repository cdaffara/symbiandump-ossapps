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
#include "loadgen_keypress.h"
#include "loadgen_utils.h"
#include "loadgen.hrh"
#include <loadgen.rsg>

#include <e32math.h>

_LIT(KThreadName, "KeyPress %d");

const TInt KDefaultStart = 50;
const TInt KDefaultPeriod = 5000000;
    
// ===================================== MEMBER FUNCTIONS =====================================

CKeyPress* CKeyPress::NewL(TKeyPressAttributes& aAttributes, TInt aReferenceNumber)
    {
    CKeyPress* self = new(ELeave) CKeyPress(aAttributes, aReferenceNumber);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// --------------------------------------------------------------------------------------------

CKeyPress::~CKeyPress()
    {
    Close();
    }

// --------------------------------------------------------------------------------------------

CKeyPress::CKeyPress(TKeyPressAttributes& aAttributes, TInt aReferenceNumber) : iAttributes(aAttributes)
    {
    iAttributes.iId = aReferenceNumber;
    }

// --------------------------------------------------------------------------------------------

void CKeyPress::ConstructL()
    {
    CLoadBase::ConstructL();
    
    iType = ELoadGenCmdNewLoadKeyPress;
    
    TBuf<64> threadName;
    threadName.Format(KThreadName, iAttributes.iId);
    
    // create a thread
    User::LeaveIfError(iThread.Create(threadName, ThreadFunction, KDefaultStackSize*2, KMinHeapSize, 1024*KMinHeapSize, (TAny*) &iAttributes ));
    
    // set priority of the thread
    SetPriority();
    }

// --------------------------------------------------------------------------------------------

TInt CKeyPress::ThreadFunction(TAny* aThreadArg)
    {
    CTrapCleanup* pC = CTrapCleanup::New();
    CActiveScheduler* pS = new CActiveScheduler;
    CActiveScheduler::Install(pS);

    // start generating load, pass pointer to arguments
    GenerateLoad(*((TKeyPressAttributes*) aThreadArg));

    delete pS;
    delete pC;
    
    return KErrNone;
    }

// --------------------------------------------------------------------------------------------

void CKeyPress::GenerateLoad(TKeyPressAttributes& aAttributes)
    {
    CKeyPressManager* keyPressManager = NULL;
    TRAPD(err, keyPressManager = CKeyPressManager::NewL(aAttributes));
    if (err == KErrNone) CActiveScheduler::Start();
    delete keyPressManager;
    }

// --------------------------------------------------------------------------------------------

void CKeyPress::Resume()
    {
    CLoadBase::Resume();
    
    iThread.Resume();
    }

// --------------------------------------------------------------------------------------------

void CKeyPress::Suspend()
    {
    CLoadBase::Suspend();
    
    iThread.Suspend();
    }

// --------------------------------------------------------------------------------------------

void CKeyPress::SetPriority()
    {
    CLoadBase::SetPriority();
    
    iThread.SetPriority(CLoadGenUtils::SettingItemToThreadPriority(iAttributes.iPriority));
    }
    
// --------------------------------------------------------------------------------------------

void CKeyPress::Close()
    {
    CLoadBase::Close();
    
    if (iThread.ExitReason() == 0) // check if the thread is still alive
        {
        // signal the thread that it needs to close
        iThread.RequestComplete(iAttributes.iDeathStatus, KErrCancel);

        // wait the thread to die
        TRequestStatus waiter;
        iThread.Logon(waiter);
        User::WaitForRequest(waiter);
        iThread.Close();
        }
    }
    
// --------------------------------------------------------------------------------------------

TPtrC CKeyPress::Description()
    {
    TBuf<256> buf;
    TBuf<16> prioBuf;
    CLoadGenUtils::SettingItemToThreadDescription(iAttributes.iPriority, prioBuf);
    
    _LIT(KKeyPressEntry, "[%d] KeyPress prio=%S heartbeat=%dms random=%d%%");
    buf.Format(KKeyPressEntry, iAttributes.iId, &prioBuf, iAttributes.iHeartBeat, iAttributes.iRandomVariance);
   
    return TPtrC(buf);
    }               

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

CKeyPressManager* CKeyPressManager::NewL(TKeyPressAttributes& aAttributes)
    {
    CKeyPressManager* self = new(ELeave) CKeyPressManager(aAttributes);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------------------------

CKeyPressManager::CKeyPressManager(TKeyPressAttributes& aAttributes) :
    CActive(EPriorityStandard), iAttributes(aAttributes)
    {
    }

// --------------------------------------------------------------------------------------------
    
CKeyPressManager::~CKeyPressManager()
    {
    Cancel();
    
    iWsSession.Close();
    }

// --------------------------------------------------------------------------------------------
 
void CKeyPressManager::ConstructL()
    {
    CActiveScheduler::Add(this);
    
    // set the status as pending
    iStatus = KRequestPending;
    SetActive();
    
    // set the death status pointer point to the request status of this ao
    iAttributes.iDeathStatus = &iStatus;
    
    // init
    User::LeaveIfError( iWsSession.Connect() );
    
    // start timer    
    iPeriodicTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    iPeriodicTimer->Start(KDefaultStart, KDefaultPeriod, TCallBack(PeriodicTimerCallBack, this));
    }
    
// --------------------------------------------------------------------------------------------
 
void CKeyPressManager::RunL()
    {
    // request status has completed by the main thread meaning that we need to stop now
    CActiveScheduler::Stop();
    }

// --------------------------------------------------------------------------------------------
 
void CKeyPressManager::DoCancel()
    {
    }

// --------------------------------------------------------------------------------------------

TInt CKeyPressManager::PeriodicTimerCallBack(TAny* aAny)
    {
    CKeyPressManager* self = static_cast<CKeyPressManager*>( aAny );

    self->iPeriodicTimer->Cancel();
    self->SimulateKeyEvent();

    return KErrNone;
    }
    
// --------------------------------------------------------------------------------------------
 
void CKeyPressManager::SimulateKeyEvent()
    {
    // generate a random key event from a to z
    TKeyEvent keyEvent;
    keyEvent.iCode = CLoadGenUtils::RandomNumber('a','z');
    keyEvent.iScanCode = keyEvent.iCode - 32;
    keyEvent.iModifiers = 0;
    keyEvent.iRepeats = 0;
    
    iWsSession.SimulateKeyEvent(keyEvent);
    iWsSession.Flush();
    
    // call timer
    iPeriodicTimer->Start(CLoadGenUtils::MilliSecondsToMicroSeconds(iAttributes.iHeartBeat, iAttributes.iRandomVariance), KDefaultPeriod, TCallBack(PeriodicTimerCallBack, this));
    }
    
// --------------------------------------------------------------------------------------------

// End of File
