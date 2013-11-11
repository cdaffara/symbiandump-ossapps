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
#include "loadgen_netconn.h"
#include "loadgen_utils.h"
#include "loadgen.hrh"
#include <loadgen.rsg>

#include <e32math.h>
#include <commdb.h>

_LIT(KThreadName, "NetConn %d");

const TInt KDefaultStart = 50;
const TInt KDefaultPeriod = 5000000;
    
// ===================================== MEMBER FUNCTIONS =====================================

CNetConn* CNetConn::NewL(TNetConnAttributes& aAttributes, TInt aReferenceNumber)
    {
    CNetConn* self = new(ELeave) CNetConn(aAttributes, aReferenceNumber);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// --------------------------------------------------------------------------------------------

CNetConn::~CNetConn()
    {
    Close();
    }

// --------------------------------------------------------------------------------------------

CNetConn::CNetConn(TNetConnAttributes& aAttributes, TInt aReferenceNumber) : iAttributes(aAttributes)
    {
    iAttributes.iId = aReferenceNumber;
    }

// --------------------------------------------------------------------------------------------

void CNetConn::ConstructL()
    {
    CLoadBase::ConstructL();
    
    iType = ELoadGenCmdNewLoadNetConn;

    TBuf<64> threadName;
    threadName.Format(KThreadName, iAttributes.iId);
    
    // create a thread
    User::LeaveIfError(iThread.Create(threadName, ThreadFunction, KDefaultStackSize*2, 1024*KMinHeapSize, 16*1024*KMinHeapSize, (TAny*) &iAttributes ));
    
    // set priority of the thread
    SetPriority();
    }

// --------------------------------------------------------------------------------------------

TInt CNetConn::ThreadFunction(TAny* aThreadArg)
    {
    CTrapCleanup* pC = CTrapCleanup::New();
    CActiveScheduler* pS = new CActiveScheduler;
    CActiveScheduler::Install(pS);

    // start generating load, pass pointer to arguments
    GenerateLoad(*((TNetConnAttributes*) aThreadArg));

    delete pS;
    delete pC;
    
    return KErrNone;
    }

// --------------------------------------------------------------------------------------------

void CNetConn::GenerateLoad(TNetConnAttributes& aAttributes)
    {
    CNetConnManager* netConnManager = NULL;
    TRAPD(err, netConnManager = CNetConnManager::NewL(aAttributes));
    if (err == KErrNone) CActiveScheduler::Start();
    delete netConnManager;
    }

// --------------------------------------------------------------------------------------------

void CNetConn::Resume()
    {
    CLoadBase::Resume();
    
    iThread.Resume();
    }

// --------------------------------------------------------------------------------------------

void CNetConn::Suspend()
    {
    CLoadBase::Suspend();
    
    iThread.Suspend();
    }

// --------------------------------------------------------------------------------------------

void CNetConn::SetPriority()
    {
    CLoadBase::SetPriority();
    
    iThread.SetPriority(CLoadGenUtils::SettingItemToThreadPriority(iAttributes.iPriority));
    }
    
// --------------------------------------------------------------------------------------------

void CNetConn::Close()
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

TPtrC CNetConn::Description()
    {
    TBuf<256> buf;
    TBuf<16> prioBuf;
    CLoadGenUtils::SettingItemToThreadDescription(iAttributes.iPriority, prioBuf);
    
    _LIT(KNetConnEntry, "[%d] NetConn prio=%S dest=%S idle=%dms random=%d%%");
    buf.Format(KNetConnEntry, iAttributes.iId, &prioBuf, &iAttributes.iDestination, iAttributes.iIdle, iAttributes.iRandomVariance);
   
    return TPtrC(buf);
    }
                   
// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

CNetConnManager* CNetConnManager::NewL(TNetConnAttributes& aAttributes)
    {
    CNetConnManager* self = new(ELeave) CNetConnManager(aAttributes);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------------------------

CNetConnManager::CNetConnManager(TNetConnAttributes& aAttributes) :
    CActive(EPriorityStandard), iAttributes(aAttributes)
    {
    }

// --------------------------------------------------------------------------------------------
    
CNetConnManager::~CNetConnManager()
    {
    Cancel();

    delete iHTTPReceiver;
    }

// --------------------------------------------------------------------------------------------
 
void CNetConnManager::ConstructL()
    {
    CActiveScheduler::Add(this);
    
    // set the status as pending
    iStatus = KRequestPending;
    SetActive();
    
    // set the death status pointer point to the request status of this ao
    iAttributes.iDeathStatus = &iStatus;
    
    iHTTPReceiver = CHTTPReceiver::NewL( *this );
    
    // start timer    
    iPeriodicTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    iPeriodicTimer->Start(KDefaultStart, KDefaultPeriod, TCallBack(PeriodicTimerCallBack, this));
    }
    
// --------------------------------------------------------------------------------------------
 
void CNetConnManager::RunL()
    {
    // request status has completed by the main thread meaning that we need to stop now
    CActiveScheduler::Stop();
    }

// --------------------------------------------------------------------------------------------
 
void CNetConnManager::DoCancel()
    {
    }

// --------------------------------------------------------------------------------------------

TInt CNetConnManager::PeriodicTimerCallBack(TAny* aAny)
    {
    CNetConnManager* self = static_cast<CNetConnManager*>( aAny );

    self->iPeriodicTimer->Cancel();
    TRAPD(err,self->StartDownloadL());

    return err;
    }
    
// --------------------------------------------------------------------------------------------
 
void CNetConnManager::StartDownloadL()
    {

    // create new download    
    TBuf8<256> url;
    url.Copy(iAttributes.iDestination);
    
    iHTTPReceiver->SendHTTPGetL( url );
    }

// --------------------------------------------------------------------------------------------

void CNetConnManager::HTTPFileReceived( TInt aStatus )
    {
    switch ( aStatus )
        {
        case 200:
            {
            // assume that the download has finished in this stage
            iPeriodicTimer->Start(CLoadGenUtils::MilliSecondsToMicroSeconds(iAttributes.iIdle, iAttributes.iRandomVariance), KDefaultPeriod, TCallBack(PeriodicTimerCallBack, this));
            break;
            }

        default:
            {
            break;
            }
        }
    }

// --------------------------------------------------------------------------------------------

// End of File
