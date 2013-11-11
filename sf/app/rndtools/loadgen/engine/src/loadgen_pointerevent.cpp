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
#include "loadgen_pointerevent.h"
#include "loadgen_utils.h"
#include "loadgen.hrh"
#include <loadgen.rsg>

#include <e32math.h>

_LIT(KThreadName, "PointerEvent %d");

const TInt KDefaultStart = 50;
const TInt KDefaultPeriod = 5000000;
    
// ===================================== MEMBER FUNCTIONS =====================================

CPointerEvent* CPointerEvent::NewL(TPointerEventAttributes& aAttributes, TInt aReferenceNumber)
    {
    CPointerEvent* self = new(ELeave) CPointerEvent(aAttributes, aReferenceNumber);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// --------------------------------------------------------------------------------------------

CPointerEvent::~CPointerEvent()
    {
    Close();
    }

// --------------------------------------------------------------------------------------------

CPointerEvent::CPointerEvent(TPointerEventAttributes& aAttributes, TInt aReferenceNumber) : iAttributes(aAttributes)
    {
    iAttributes.iId = aReferenceNumber;
    }

// --------------------------------------------------------------------------------------------

void CPointerEvent::ConstructL()
    {
    CLoadBase::ConstructL();
    
    iType = ELoadGenCmdNewLoadPointerEvent;
    
    TBuf<64> threadName;
    threadName.Format(KThreadName, iAttributes.iId);
    
    // create a thread
    User::LeaveIfError(iThread.Create(threadName, ThreadFunction, KDefaultStackSize*2, KMinHeapSize, 1024*KMinHeapSize, (TAny*) &iAttributes ));
    
    // set priority of the thread
    SetPriority();
    }

// --------------------------------------------------------------------------------------------

TInt CPointerEvent::ThreadFunction(TAny* aThreadArg)
    {
    CTrapCleanup* pC = CTrapCleanup::New();
    CActiveScheduler* pS = new CActiveScheduler;
    CActiveScheduler::Install(pS);

    // start generating load, pass pointer to arguments
    GenerateLoad(*((TPointerEventAttributes*) aThreadArg));

    delete pS;
    delete pC;
    
    return KErrNone;
    }

// --------------------------------------------------------------------------------------------

void CPointerEvent::GenerateLoad(TPointerEventAttributes& aAttributes)
    {
    CPointerEventManager* pointerEventManager = NULL;
    TRAPD(err, pointerEventManager = CPointerEventManager::NewL(aAttributes));
    if (err == KErrNone) CActiveScheduler::Start();
    delete pointerEventManager;
    }

// --------------------------------------------------------------------------------------------

void CPointerEvent::Resume()
    {
    CLoadBase::Resume();
    
    iThread.Resume();
    }

// --------------------------------------------------------------------------------------------

void CPointerEvent::Suspend()
    {
    CLoadBase::Suspend();
    
    iThread.Suspend();
    }

// --------------------------------------------------------------------------------------------

void CPointerEvent::SetPriority()
    {
    CLoadBase::SetPriority();
    
    iThread.SetPriority(CLoadGenUtils::SettingItemToThreadPriority(iAttributes.iPriority));
    }
    
// --------------------------------------------------------------------------------------------

void CPointerEvent::Close()
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

TPtrC CPointerEvent::Description()
    {
    TBuf<256> buf;
    TBuf<16> prioBuf;
    CLoadGenUtils::SettingItemToThreadDescription(iAttributes.iPriority, prioBuf);
    
    _LIT(KPointerEventEntry, "[%d] PointerEvent prio=%S heartbeat=%dms random=%d%%");
    buf.Format(KPointerEventEntry, iAttributes.iId, &prioBuf, iAttributes.iHeartBeat, iAttributes.iRandomVariance);
   
    return TPtrC(buf);
    }               

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

CPointerEventManager* CPointerEventManager::NewL(TPointerEventAttributes& aAttributes)
    {
    CPointerEventManager* self = new(ELeave) CPointerEventManager(aAttributes);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------------------------

CPointerEventManager::CPointerEventManager(TPointerEventAttributes& aAttributes) :
    CActive(EPriorityStandard), iAttributes(aAttributes)
    {
    }

// --------------------------------------------------------------------------------------------
    
CPointerEventManager::~CPointerEventManager()
    {
    Cancel();    
    }

// --------------------------------------------------------------------------------------------
 
void CPointerEventManager::ConstructL()
    {
    CActiveScheduler::Add(this);
    
    // set the status as pending
    iStatus = KRequestPending;
    SetActive();
    
    // set the death status pointer point to the request status of this ao
    iAttributes.iDeathStatus = &iStatus;
    
    // init    
    
    // start timer    
    iPeriodicTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    iPeriodicTimer->Start(KDefaultStart, KDefaultPeriod, TCallBack(PeriodicTimerCallBack, this));
    }
    
// --------------------------------------------------------------------------------------------
 
void CPointerEventManager::RunL()
    {
    // request status has completed by the main thread meaning that we need to stop now
    CActiveScheduler::Stop();
    }

// --------------------------------------------------------------------------------------------
 
void CPointerEventManager::DoCancel()
    {
    }

// --------------------------------------------------------------------------------------------

TInt CPointerEventManager::PeriodicTimerCallBack(TAny* aAny)
    {
    CPointerEventManager* self = static_cast<CPointerEventManager*>( aAny );

    self->iPeriodicTimer->Cancel();
    self->SimulatePointerEvent();

    return KErrNone;
    }
    
// --------------------------------------------------------------------------------------------
 
void CPointerEventManager::SimulatePointerEvent()
    {
    // Check the screen size and select pointer event position
    // randomly
    TSize screenSize = CLoadGenUtils::ScreenSize();
        
    TInt x1 = CLoadGenUtils::RandomNumber(0, screenSize.iWidth);
    TInt y1 = CLoadGenUtils::RandomNumber(0, screenSize.iHeight);
    
    // Generate "Pointer down" event:
    TRawEvent eventPointerDown;    
    eventPointerDown.Set(TRawEvent::EButton1Down, x1, y1);
    UserSvr::AddEvent(eventPointerDown);
    
    // Get new random position for "Move pointer" and 
    // "Pointer up" events
    TInt x2 = CLoadGenUtils::RandomNumber(0, screenSize.iWidth);
    TInt y2 = CLoadGenUtils::RandomNumber(0, screenSize.iHeight);
    
    // Generate "Move pointer" event:
    TRawEvent eventMovePointer;
    eventMovePointer.Set(TRawEvent::EPointerMove, x2, y2);
    UserSvr::AddEvent(eventMovePointer);
    
    // Generate "Pointer up" event:
    TRawEvent eventPointerUp;
    eventPointerUp.Set(TRawEvent::EButton1Up, x2, y2);
    UserSvr::AddEvent(eventPointerUp);
    
    // call timer
    iPeriodicTimer->Start(CLoadGenUtils::MilliSecondsToMicroSeconds(iAttributes.iHeartBeat, iAttributes.iRandomVariance), KDefaultPeriod, TCallBack(PeriodicTimerCallBack, this));
    }
    
// --------------------------------------------------------------------------------------------

// End of File
