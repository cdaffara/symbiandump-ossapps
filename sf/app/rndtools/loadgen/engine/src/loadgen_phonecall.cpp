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
#include "loadgen_phonecall.h"
#include "loadgen_utils.h"
#include "loadgen.hrh"
#include <loadgen.rsg>

#include <e32math.h>

_LIT(KThreadName, "PhoneCall %d");

const TInt KDefaultStart = 50;
const TInt KDefaultPeriod = 5000000;
    
// ===================================== MEMBER FUNCTIONS =====================================

CPhoneCall* CPhoneCall::NewL(TPhoneCallAttributes& aAttributes, TInt aReferenceNumber)
    {
    CPhoneCall* self = new(ELeave) CPhoneCall(aAttributes, aReferenceNumber);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// --------------------------------------------------------------------------------------------

CPhoneCall::~CPhoneCall()
    {
    Close();
    }

// --------------------------------------------------------------------------------------------

CPhoneCall::CPhoneCall(TPhoneCallAttributes& aAttributes, TInt aReferenceNumber) : iAttributes(aAttributes)
    {
    iAttributes.iId = aReferenceNumber;
    }

// --------------------------------------------------------------------------------------------

void CPhoneCall::ConstructL()
    {
    CLoadBase::ConstructL();
    
    iType = ELoadGenCmdNewLoadPhoneCall;
    
    TBuf<64> threadName;
    threadName.Format(KThreadName, iAttributes.iId);
    
    // create a thread
    User::LeaveIfError(iThread.Create(threadName, ThreadFunction, KDefaultStackSize*2, KMinHeapSize, 1024*KMinHeapSize, (TAny*) &iAttributes ));
    
    // set priority of the thread
    SetPriority();
    }

// --------------------------------------------------------------------------------------------

TInt CPhoneCall::ThreadFunction(TAny* aThreadArg)
    {
    CTrapCleanup* pC = CTrapCleanup::New();
    CActiveScheduler* pS = new CActiveScheduler;
    CActiveScheduler::Install(pS);

    // start generating load, pass pointer to arguments
    GenerateLoad(*((TPhoneCallAttributes*) aThreadArg));

    delete pS;
    delete pC;
    
    return KErrNone;
    }

// --------------------------------------------------------------------------------------------

void CPhoneCall::GenerateLoad(TPhoneCallAttributes& aAttributes)
    {
    CPhoneCallManager* phoneCallManager = NULL;
    TRAPD(err, phoneCallManager = CPhoneCallManager::NewL(aAttributes));
    if (err == KErrNone) CActiveScheduler::Start();
    delete phoneCallManager;
    }

// --------------------------------------------------------------------------------------------

void CPhoneCall::Resume()
    {
    CLoadBase::Resume();
    
    iThread.Resume();
    }

// --------------------------------------------------------------------------------------------

void CPhoneCall::Suspend()
    {
    CLoadBase::Suspend();
    
    iThread.Suspend();
    }

// --------------------------------------------------------------------------------------------

void CPhoneCall::SetPriority()
    {
    CLoadBase::SetPriority();
    
    iThread.SetPriority(CLoadGenUtils::SettingItemToThreadPriority(iAttributes.iPriority));
    }
    
// --------------------------------------------------------------------------------------------

void CPhoneCall::Close()
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

TPtrC CPhoneCall::Description()
    {
    TBuf<256> buf;
    TBuf<16> prioBuf;
    CLoadGenUtils::SettingItemToThreadDescription(iAttributes.iPriority, prioBuf);
    
    _LIT(KPhoneCallEntry, "[%d] PhoneCall prio=%S dest=%S length=%dms idle=%dms random=%d%%");
    buf.Format(KPhoneCallEntry, iAttributes.iId, &prioBuf, &iAttributes.iDestination, iAttributes.iLength, iAttributes.iIdle, iAttributes.iRandomVariance);
   
    return TPtrC(buf);
    }               

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

CPhoneCallManager* CPhoneCallManager::NewL(TPhoneCallAttributes& aAttributes)
    {
    CPhoneCallManager* self = new(ELeave) CPhoneCallManager(aAttributes);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------------------------

CPhoneCallManager::CPhoneCallManager(TPhoneCallAttributes& aAttributes) :
    CActive(EPriorityStandard), iAttributes(aAttributes), iState(EStateIdle)
    {
    }

// --------------------------------------------------------------------------------------------
    
CPhoneCallManager::~CPhoneCallManager()
    {
    Cancel();
    
    if (iPeriodicTimer)
        {
        iPeriodicTimer->Cancel();
        delete iPeriodicTimer;
        }
        
    delete iDialer;
    }

// --------------------------------------------------------------------------------------------
 
void CPhoneCallManager::ConstructL()
    {
    CActiveScheduler::Add(this);
    
    // set the status as pending
    iStatus = KRequestPending;
    SetActive();
    
    // set the death status pointer point to the request status of this ao
    iAttributes.iDeathStatus = &iStatus;

    // init dialer ao
    iDialer = CDialer::NewL(*this);
    
    // start timer    
    iPeriodicTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    iPeriodicTimer->Start(KDefaultStart, KDefaultPeriod, TCallBack(PeriodicTimerCallBack, this));
    }

// --------------------------------------------------------------------------------------------
 
void CPhoneCallManager::RunL()
    {
    // request status has completed by the main thread meaning that we need to stop now
    CActiveScheduler::Stop();
    }

// --------------------------------------------------------------------------------------------
 
void CPhoneCallManager::DoCancel()
    {
    }
    
// --------------------------------------------------------------------------------------------

TInt CPhoneCallManager::PeriodicTimerCallBack(TAny* aAny)
    {
    CPhoneCallManager* self = static_cast<CPhoneCallManager*>( aAny );

    self->iPeriodicTimer->Cancel();
    self->HandleCalls();

    return KErrNone;
    }

// --------------------------------------------------------------------------------------------

void CPhoneCallManager::HandleCalls()
    {
    if (iState == EStateIdle)
        {
        // make a new call
        iState = EStateCall;
        iDialer->Dial(iAttributes.iDestination);
        }
    
    else if (iState == EStateCall)
        {
        // hangup existing call
        iState = EStateIdle;
        iDialer->Hangup();
        }
    }

// --------------------------------------------------------------------------------------------

void CPhoneCallManager::HandleStatus(TInt /*aErr*/)
    {
    // call timer after wait
    if (iState == EStateCall)
        {
        iPeriodicTimer->Start(CLoadGenUtils::MilliSecondsToMicroSeconds(iAttributes.iLength, iAttributes.iRandomVariance), KDefaultPeriod, TCallBack(PeriodicTimerCallBack, this));
        }

    else if (iState == EStateIdle)
        {
        iPeriodicTimer->Start(CLoadGenUtils::MilliSecondsToMicroSeconds(iAttributes.iIdle, iAttributes.iRandomVariance), KDefaultPeriod, TCallBack(PeriodicTimerCallBack, this));
        }        
    }

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

CDialer* CDialer::NewL(CPhoneCallManager& aManager)
    {
    CDialer* self = new(ELeave) CDialer(aManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------------------------

CDialer::CDialer(CPhoneCallManager& aManager) :
    CActive(EPriorityStandard), iManager(aManager), iCallParamsPckg(iCallParams)
    {
    }

// --------------------------------------------------------------------------------------------
    
CDialer::~CDialer()
    {
    Cancel();
    delete iTelephony;
    }

// --------------------------------------------------------------------------------------------
 
void CDialer::ConstructL()
    {
    CActiveScheduler::Add(this);
    
    // init telephony object
    iTelephony = CTelephony::NewL();
    }

// --------------------------------------------------------------------------------------------
 
void CDialer::RunL()
    {
    // notify the observer that status has changed
    iManager.HandleStatus(iStatus.Int());
    }
    
// --------------------------------------------------------------------------------------------
 
void CDialer::DoCancel()
    {
    iTelephony->CancelAsync(CTelephony::EDialNewCallCancel);
    iTelephony->CancelAsync(CTelephony::EHangupCancel);
    }

// --------------------------------------------------------------------------------------------
    
void CDialer::Dial(const TDesC& aDestination)
    {
    CTelephony::TTelNumber telNumber(aDestination);
    iCallParams.iIdRestrict = CTelephony::ESendMyId;
    iTelephony->DialNewCall(iStatus, iCallParamsPckg, telNumber, iCallId);
    SetActive();
    }

// --------------------------------------------------------------------------------------------

void CDialer::Hangup()
    {
    iTelephony->Hangup(iStatus, iCallId);
    SetActive();
    }

// --------------------------------------------------------------------------------------------

    

// End of File
