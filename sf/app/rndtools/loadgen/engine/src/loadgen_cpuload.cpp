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
#include "loadgen_cpuload.h"
#include "loadgen_utils.h"
#include "loadgen.hrh"
#include <loadgen.rsg>
#include <e32hal.h>
#include <u32hal.h>
#include <e32math.h>
#include <e32svr.h>

_LIT(KThreadName, "CPULoad %d");
    
// ===================================== MEMBER FUNCTIONS =====================================

CCPULoad* CCPULoad::NewL(TCPULoadAttributes& aAttributes, TInt aReferenceNumber)
    {
    CCPULoad* self = new(ELeave) CCPULoad(aAttributes, aReferenceNumber);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// --------------------------------------------------------------------------------------------

CCPULoad::~CCPULoad()
    {
    Close();
    }

// --------------------------------------------------------------------------------------------

CCPULoad::CCPULoad(TCPULoadAttributes& aAttributes, TInt aReferenceNumber) : iAttributes(aAttributes)
    {
    iAttributes.iId = aReferenceNumber;
    }

// --------------------------------------------------------------------------------------------

void CCPULoad::ConstructL()
    {
    CLoadBase::ConstructL();
    
    iType = ELoadGenCmdNewLoadCPULoad;
    
    TBuf<64> threadName;
    threadName.Format(KThreadName, iAttributes.iId);
    
    // create a thread
    User::LeaveIfError(iThread.Create(threadName, ThreadFunction, KDefaultStackSize*2, KMinHeapSize, 1024*KMinHeapSize, (TAny*) &iAttributes ));
    
    // set priority of the thread
    SetPriority();
    }

// --------------------------------------------------------------------------------------------

TInt CCPULoad::ThreadFunction(TAny* aThreadArg)
    {
    TCPULoadAttributes* threadArg = (TCPULoadAttributes*)aThreadArg;
    TInt err = KErrNone;

    // if a cpu is defined, tie this thread to the given cpu (SMP environment)
    if (threadArg->iCpu >= 0 && threadArg->iCpu != KCPUSelection_AllCPUs )
        {
        UserSvr::HalFunction(EHalGroupKernel, KHalFunction_EKernelHalLockThreadToCpu, (TAny*) threadArg->iCpu, 0);
        }
    
    CTrapCleanup* pC = CTrapCleanup::New();
    CActiveScheduler* pS = new CActiveScheduler;
    CActiveScheduler::Install(pS);

    // start generating load, pass pointer to arguments
    GenerateLoad(*((TCPULoadAttributes*) aThreadArg));

    delete pS;
    delete pC;
    
    return err;
    }

// --------------------------------------------------------------------------------------------

void CCPULoad::GenerateLoad(TCPULoadAttributes& aAttributes)
    {
    for (;;)
        {   
        if (aAttributes.iType == ECpuLoadTypeContinuous)
            {
            // do constantly heave stuff
            DoHeaveStuff(aAttributes.iMode);
            }

        else if (aAttributes.iType == ECpuLoadTypePeriodic)
            {
            // do periodically heave stuff
            TTime startTime;
            startTime.HomeTime(); // get start time

            TTime currentTime;
            TTimeIntervalMicroSeconds interval;
            TInt processPeriod; 
			#ifdef QT_SUPPORT
				
			#else
				processPeriod = CLoadGenUtils::MilliSecondsToMicroSeconds(aAttributes.iLength, aAttributes.iRandomVariance);
			#endif
            do
                {
                // do heave stuff
                DoHeaveStuff(aAttributes.iMode);

                currentTime.HomeTime();
                interval = currentTime.MicroSecondsFrom(startTime);                
                }
            while (interval.Int64() < processPeriod);       


            // now wait
            User::After( CLoadGenUtils::MilliSecondsToMicroSeconds(aAttributes.iIdle, aAttributes.iRandomVariance) );
            }   

        else
            {
            User::Panic(_L("Unk.type"), 888);
            }
        }
    }

// --------------------------------------------------------------------------------------------

void CCPULoad::DoHeaveStuff(TInt aMode)
    {
    TTime now;
    now.HomeTime();
    TInt64 seed = now.Int64();
    
    TReal random = Math::FRand(seed);
    
    TReal target(10);
    TReal source(10);
    
    target += random;
    
    Math::Cos(target, source);
    
    source = source / 1.0382873;
    source -= 32.24343;
    source += 132.24343;
    source *= random;
    
    // yield trick
    if (aMode == ECpuLoadModeYielding)
        {
        // sleep randomly
        if (User::TickCount() % 50 == 0)
            User::AfterHighRes(1);            
        }
    }

// --------------------------------------------------------------------------------------------

void CCPULoad::Resume()
    {
    CLoadBase::Resume();
    
    iThread.Resume();
    }

// --------------------------------------------------------------------------------------------

void CCPULoad::Suspend()
    {
    CLoadBase::Suspend();
    
    iThread.Suspend();
    }

// --------------------------------------------------------------------------------------------

void CCPULoad::SetPriority()
    {
    CLoadBase::SetPriority();
    
    iThread.SetPriority(CLoadGenUtils::SettingItemToThreadPriority(iAttributes.iPriority));
    }
    
// --------------------------------------------------------------------------------------------

void CCPULoad::Close()
    {
    CLoadBase::Close();

    // kill the thread immediately
    iThread.Kill(0);   

    iThread.Close();
    }
    
// --------------------------------------------------------------------------------------------

TPtrC CCPULoad::Description()
    {
    TBuf<256> buf;
    TBuf<16> prioBuf;
    CLoadGenUtils::SettingItemToThreadDescription(iAttributes.iPriority, prioBuf);
    
    if (iAttributes.iType == ECpuLoadTypeContinuous)
        {
        if (iAttributes.iMode == ECpuLoadModeYielding)
            {
            _LIT(KCPULoadEntryContinuous, "[%d] CPULoad prio=%S mode=yielding type=cont");
            buf.Format(KCPULoadEntryContinuous, iAttributes.iId, &prioBuf);
            }
        else if (iAttributes.iMode == ECpuLoadModeBlocking)
            {
            _LIT(KCPULoadEntryContinuous, "[%d] CPULoad prio=%S mode=blocking type=cont");
            buf.Format(KCPULoadEntryContinuous, iAttributes.iId, &prioBuf);
            }
        }
    
    else if (iAttributes.iType == ECpuLoadTypePeriodic)
        {
        if (iAttributes.iMode == ECpuLoadModeYielding)
            {
            _LIT(KCPULoadEntryPeriodic, "[%d] CPULoad prio=%S mode=yielding type=period peak=%dms idle=%dms random=%d%%");
            buf.Format(KCPULoadEntryPeriodic, iAttributes.iId, &prioBuf, iAttributes.iLength, iAttributes.iIdle, iAttributes.iRandomVariance);
            }        
        else if (iAttributes.iMode == ECpuLoadModeBlocking)
            {
            _LIT(KCPULoadEntryPeriodic, "[%d] CPULoad prio=%S mode=blocking type=period peak=%dms idle=%dms random=%d%%");
            buf.Format(KCPULoadEntryPeriodic, iAttributes.iId, &prioBuf, iAttributes.iLength, iAttributes.iIdle, iAttributes.iRandomVariance);
            }        
        }
    
    // if we are running a load in a specific cpu, add the "name" of the
    // cpu to the description. (SMP environment)
    if (iAttributes.iCpu >= 0 && iAttributes.iCpu != KCPUSelection_AllCPUs)
        {
        TBuf<15> cpu;
        _LIT(KCPU, " CPU%d");
        cpu.Format(KCPU, iAttributes.iCpu);
        buf.Append(cpu);
        }
    
    return TPtrC(buf);
    }               

// --------------------------------------------------------------------------------------------


// End of File
