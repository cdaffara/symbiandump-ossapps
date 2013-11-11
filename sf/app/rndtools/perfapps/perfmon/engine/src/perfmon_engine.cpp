/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "perfmon_engine.h"
#include "perfmon_powerlistener.h"
#include <coeutils.h>
#include <bautils.h>
#include <eikenv.h>
#include <e32hal.h>
#include <u32std.h>
#include <s32file.h>
#include <akntitle.h> 
#include <eikspane.h>
#include <aknnotewrappers.h>
_LIT(KDefaultLogFilePath, "c:\\data\\PerfMon.log");

const TInt KCalibrationLength = 2;
const TInt KMinimumSamplesLength = 16;
const TInt KCPUTimeMultiplier = 1000000; // used to avoid TReal conversions

const TInt KSettingsDrive = EDriveC;
_LIT(KSettingsFileName, "perfmon_settings.ini");

// --------------------------------------------------------------------------------------------

TInt CPULoadCount(TAny* aPtr)
    {
    TPerfMonNOPCounter& atts = *((TPerfMonNOPCounter*) aPtr);

    // increase the counter
    atts.iCounterValue++;
    return 1;
    }

TInt CPULoadNOPThread(TAny* aPtr)
    {
    // set the thread running in correct CPU
    TPerfMonNOPCounter& atts = *((TPerfMonNOPCounter*) aPtr);
    
    if (atts.iAmountOfCPUs > 1)
        {
        // request via HAL that the current thread will run in CPU defined in atts.iCPUNumber
        TInt err = UserSvr::HalFunction(EHalGroupKernel, 19 /*EKernelHalLockThreadToCpu*/, (TAny*) atts.iCPUNumber, 0);

        if (err != KErrNone)
            {
            // error, stop this thread
            return err;
            }
        }

    // init trap clean and install active scheduler
    CTrapCleanup* pC = CTrapCleanup::New();
    CActiveScheduler* pS = new CActiveScheduler;
    CActiveScheduler::Install(pS);

    // call the counter to simulate polling the null thread for cpu time
    CIdle* idle = CIdle::NewL(CActive::EPriorityStandard);
    TCallBack cb(CPULoadCount, aPtr);
    idle->Start(cb);

    pS->Start();

    delete idle;
    delete pS;
    delete pC;

    return KErrNone;
    }

// ===================================== MEMBER FUNCTIONS =====================================

CPerfMonEngine::CPerfMonEngine() :
    CActive(EPriorityUserInput),
    iPowerClient(0)
    {
    }

// --------------------------------------------------------------------------------------------

void CPerfMonEngine::ConstructL()
    {
    iCurrentCPUMode = ECPUModeNotSet;
    iLogFileInitialized = EFalse;
    iAmountOfCPUs = 1;

    iEnv = CEikonEnv::Static();
    User::LeaveIfError(iLs.Connect());

    User::LeaveIfError(iTimer.CreateLocal());
    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------------------------

void CPerfMonEngine::ActivateEngineL()
    {
    // load settings
    TRAP_IGNORE(LoadSettingsL());

    // create data storages for the samples
    CreateSamplesDataArrayL();

    // set default modes
    HandleSettingsChangeL();

    // start sampling data immediately (jump to RunL)    
    iTimer.After(iStatus, 100);
    SetActive();
    }

// --------------------------------------------------------------------------------------------

void CPerfMonEngine::DeActivateEngineL()
    {
    Cancel();

    DeActivatePowerMonitoring();
    DeActivateCPUMonitoring();

    // close log file
    OpenLogFile(EFalse);
    }

// --------------------------------------------------------------------------------------------

CPerfMonEngine::~CPerfMonEngine()
    {
    DeActivatePowerMonitoring();

    if (iPowerClient)
        {
        delete iPowerClient;
        iPowerClient = 0;
        }

    iTimer.Close();

    // clear data storages
    if (iSampleEntryArray)
        {
        for (TInt i = 0; i < iSampleEntryArray->Count(); i++)
            {
            delete iSampleEntryArray->At(i).iSampleDataArray;
            }

        delete iSampleEntryArray;
        }

    iLs.Close();
    }

// --------------------------------------------------------------------------------------------

void CPerfMonEngine::DoCancel()
    {
    iTimer.Cancel();
    }

// --------------------------------------------------------------------------------------------

void CPerfMonEngine::RunL()
    {
    // calculate new values 
    UpdateSamplesDataL();

    // log changes
    AppendLatestSamplesToLogsL();

    // redraw views
    SendDrawEventToContainersL();

    // continue
    iTimer.After(iStatus, iSettings.iHeartBeat * 1000); // convert from milliseconds to microseconds
    SetActive();
    }

// --------------------------------------------------------------------------------------------

void CPerfMonEngine::HandleSettingsChangeL()
    {
    // set priority of the thread
    RThread().SetPriority(SettingItemToThreadPriority(iSettings.iPriority));

    // init cpu monitor if setting has been changed
    if (iCurrentCPUMode != iSettings.iCPUMode)
        {
        DeActivateCPUMonitoring();
        ActivateCPUMonitoringL();
        }

    // Check for power setting changes every time
    if (iSettings.iPowerMonitoringEnabled)
        {
        ActivatePowerMonitoringL();
        }
    else
        {
        DeActivatePowerMonitoring();
        }

    // close log file
    OpenLogFile(EFalse);

    // enable log file
    if (iSettings.iLoggingEnabled && (iSettings.iLoggingMode
            == ELoggingModeLogFile || iSettings.iLoggingMode
            == ELoggingModeRDebugLogFile))
        OpenLogFile(ETrue);
    }

// --------------------------------------------------------------------------------------------

void CPerfMonEngine::EnableLogging(TBool aEnable)
    {
    if (aEnable)
        {
        if (iSettings.iLoggingMode == ELoggingModeLogFile
                || iSettings.iLoggingMode == ELoggingModeRDebugLogFile)
            OpenLogFile(ETrue);

        iSettings.iLoggingEnabled = ETrue;
        }
    else // disable
        {
        iSettings.iLoggingEnabled = EFalse;
        OpenLogFile(EFalse);
        }
    }

// --------------------------------------------------------------------------------------------

void CPerfMonEngine::OpenLogFile(TBool aOpen)
    {
    // open log file for writing
    if (aOpen)
        {
        if (!iLogFileInitialized)
            {
            TInt err(KErrNone);

            // open the log file for writing
            if (iLogFile.Open(iEnv->FsSession(), iSettings.iLoggingFilePath,
                    EFileWrite) != KErrNone)
                {
                iEnv->FsSession().MkDirAll(iSettings.iLoggingFilePath);
                err = iLogFile.Replace(iEnv->FsSession(),
                        iSettings.iLoggingFilePath, EFileWrite);
                }
            else
                {
                // file opens correctly, seek to the end
                TInt fileSize = 0;
                iLogFile.Size(fileSize);
                err = iLogFile.Seek(ESeekCurrent, fileSize);
                }

            if (err == KErrNone)
                {
                iLogFileInitialized = ETrue;
                }
            else
                {
                // show error
                CAknErrorNote* note = new (ELeave) CAknErrorNote();
                note->ExecuteLD(_L("Unable to create log file, check settings"));
                }
            }
        }

    // close handle to log file
    else
        {
        if (iLogFileInitialized)
            {
            iLogFile.Flush();
            iLogFile.Close();

            iLogFileInitialized = EFalse;
            }
        }
    }

// --------------------------------------------------------------------------------------------

void CPerfMonEngine::ActivateCPUMonitoringL()
    {
    // reset counter variables
    iCPULoadCalibrating = ETrue;
    iCPULoadCalibrationCounter = 0;
    iCPULoadMaxValue = 999999999;

    for (TInt i=0; i<KMaxCPUs; i++)
        {
        iCPULoadPreviousValue[i] = 1;
        }

    // use null thread is cpu time is supported and the setting is on
    if (CPUTimeSupported() && iSettings.iCPUMode == ECPUModeCPUTime)
        {
        // try to open handle to null thread
        if (OpenHandleToNullThread())
            {
            // handle is open, get initial value for each CPU
            for (TInt i=0; i<iAmountOfCPUs; i++)
                {
                TTimeIntervalMicroSeconds time;
                iNullThreads[i].GetCpuTime(time);
                iCPULoadPreviousValue[i] = time.Int64();
                }

            iPreviousTime.HomeTime();

            iCurrentCPUMode = ECPUModeCPUTime;
            return; // cpu time is succesfully in use
            }
        }

    // otherwise use normal sampling with nops
    iCurrentCPUMode = ECPUModeNotSet;

    // show a warning if cpu time cannot be taken in use
    if (iSettings.iCPUMode == ECPUModeCPUTime)
        {
        CAknInformationNote* note = new (ELeave) CAknInformationNote();
        note->ExecuteLD(
                _L("CPU Time not supported in this system, using NOPs sampling"));
        }

    // get the amount of CPUs
    iAmountOfCPUs = GetAmountOfCPUs();

    // create a thread for CPU load monitoring
    for (TInt i=0; i<iAmountOfCPUs; i++)
        {
        // initialize the NOP counter variables
        iCPULoadNOPCounter[i].iCounterValue = 0;
        iCPULoadNOPCounter[i].iCPUNumber = i;
        iCPULoadNOPCounter[i].iAmountOfCPUs = iAmountOfCPUs;

        // create a name for the thread
        TFileName cpuLoadThreadName;
        cpuLoadThreadName.Copy(_L("PerfMonCPULoadNOPThread"));
        cpuLoadThreadName.AppendNum(i);
        
        // create the thread and resume it
        User::LeaveIfError(iCPULoadNOPThread[i].Create(cpuLoadThreadName,
                CPULoadNOPThread, 0x1000, 0x1000, 0x100000, &iCPULoadNOPCounter[i]));
        iCPULoadNOPThread[i].SetPriority(EPriorityLess);
        iCPULoadNOPThread[i].Resume();
        }

    iCurrentCPUMode = ECPUModeNOPs; // NOPs taken succesfully in use
    }

// --------------------------------------------------------------------------------------------

TBool CPerfMonEngine::OpenHandleToNullThread()
    {
    // find the kernel process and then the null thread
    TFindProcess fp(_L("ekern.exe*"));

    TFullName kernelName;
    if (fp.Next(kernelName) == KErrNone)
        {
        iAmountOfCPUs = 0;

        // find all null threads in the system by following the order ie "Null", "Null1", "Null2", "Null3"
        for (TInt i(0); i<KMaxCPUs; i++)
            {
            // construct name of the null thread from the process name
            TFullName nullThreadName(kernelName);
            nullThreadName.Append(_L("::Null"));
            
            if (i >= 1) // add identifier for CPUs higher than zero
                nullThreadName.AppendNum(i);

            TFindThread ft(nullThreadName);
            TFullName threadName;

            if (ft.Next(threadName) == KErrNone)
                {
                if (iNullThreads[i].Open(threadName) != KErrNone)
                    return EFalse;
                
                iAmountOfCPUs++;
                }
            else
                {
                break;  // break the loop, no more matches can be found
                }
            }
        }

    // process not found
    else
        return EFalse;

    // success!
    return ETrue;
    }

// --------------------------------------------------------------------------------------------

void CPerfMonEngine::DeActivateCPUMonitoring()
    {
    if (iCurrentCPUMode == ECPUModeCPUTime)
        {
        // close handles to null threads
        for (TInt i=0;i<iAmountOfCPUs;i++)
            {
            iNullThreads[i].Close();
            }
        }

    else if (iCurrentCPUMode == ECPUModeNOPs)
        {
        // kill the cpu load thread
        for (TInt i=0;i<iAmountOfCPUs;i++)
            {
            iCPULoadNOPThread[i].Kill(0);
            iCPULoadNOPThread[i].Close();
            }
        }
    }

// --------------------------------------------------------------------------------------------

TBool CPerfMonEngine::CPUTimeSupported()
    {
    TTimeIntervalMicroSeconds time;
    TInt err = RThread().GetCpuTime(time);

    if (err == KErrNone && time.Int64() > 0)
        return ETrue;
    else
        return EFalse;
    }

// --------------------------------------------------------------------------------------------

TInt CPerfMonEngine::GetAmountOfCPUs()
    {
    // query from HAL the amount of CPUs and then check it returns a valid value
    TInt amountOfCPUs = UserSvr::HalFunction(EHalGroupKernel, 16 /*EKernelHalNumLogicalCpus*/, 0, 0);
    
    if (amountOfCPUs >= 1 && amountOfCPUs <= KMaxCPUs)
        return amountOfCPUs;
    else
        return 1; // HAL may not support this feature, so just return one CPU
    }

// --------------------------------------------------------------------------------------------

TThreadPriority CPerfMonEngine::SettingItemToThreadPriority(TInt aIndex)
    {
    TThreadPriority threadPriority = EPriorityNull;

    switch (aIndex)
        {
        case EThreadPriorityTypeMuchLess:
            {
            threadPriority = EPriorityMuchLess;
            break;
            }
        case EThreadPriorityTypeLess:
            {
            threadPriority = EPriorityLess;
            break;
            }
        case EThreadPriorityTypeNormal:
            {
            threadPriority = EPriorityNormal;
            break;
            }
        case EThreadPriorityTypeMore:
            {
            threadPriority = EPriorityMore;
            break;
            }
        case EThreadPriorityTypeMuchMore:
            {
            threadPriority = EPriorityMuchMore;
            break;
            }
        case EThreadPriorityTypeRealTime:
            {
            threadPriority = EPriorityRealTime;
            break;
            }
        case EThreadPriorityTypeAbsoluteVeryLow:
            {
            threadPriority = EPriorityAbsoluteVeryLow;
            break;
            }
        case EThreadPriorityTypeAbsoluteLow:
            {
            threadPriority = EPriorityAbsoluteLow;
            break;
            }
        case EThreadPriorityTypeAbsoluteBackground:
            {
            threadPriority = EPriorityAbsoluteBackground;
            break;
            }
        case EThreadPriorityTypeAbsoluteForeground:
            {
            threadPriority = EPriorityAbsoluteForeground;
            break;
            }
        case EThreadPriorityTypeAbsoluteHigh:
            {
            threadPriority = EPriorityAbsoluteHigh;
            break;
            }

        default:
            {
            User::Panic(_L("Wrong tp index"), 276);
            break;
            }
        }

    return threadPriority;
    }

// --------------------------------------------------------------------------------------------

void CPerfMonEngine::CreateSamplesDataArrayL()
    {
    TInt maxSamples =
            iSettings.iMaxSamples >= KMinimumSamplesLength ? iSettings.iMaxSamples
                    : KMinimumSamplesLength;

    // create the data structure to store all samples
    iSampleEntryArray = new (ELeave) CSampleEntryArray(16);

    // set sample data array positions for each type
    iCPU0PositionInSamples = 0;
    iRAMPositionInSamples = KMaxCPUs;
    iCDrivePositionInSamples = KMaxCPUs+1;
    iPowerPositionInSamples = iCDrivePositionInSamples + ( ESourceI - ESourceC ) + 1;

    // add all CPU source entries
    for (TInt i=0; i<KMaxCPUs; i++)
        {
        TSampleEntry newSampleEntry;

        newSampleEntry.iDescription.Copy(_L("CPU"));
        newSampleEntry.iDescription.AppendNum(i);

        newSampleEntry.iUnitTypeShort.Copy(KNullDesC);
        newSampleEntry.iUnitTypeLong.Copy(KNullDesC);
        newSampleEntry.iDriveNumber = -1;
        newSampleEntry.iGraphColor = KRgbYellow;
        newSampleEntry.iGraphColor.SetGreen(255-i*75);
        newSampleEntry.iGraphColor.SetBlue(i*75);

        newSampleEntry.iSampleDataArray = new(ELeave) CSampleDataArray(maxSamples);
        iSampleEntryArray->AppendL(newSampleEntry);
        }

    // add RAM entry
    {
    TSampleEntry newSampleEntry;

    newSampleEntry.iDescription.Copy(_L("RAM"));
    newSampleEntry.iUnitTypeShort.Copy(_L("b"));
    newSampleEntry.iUnitTypeLong.Copy(_L("bytes"));
    newSampleEntry.iDriveNumber = -1;
    newSampleEntry.iGraphColor = KRgbGreen;

    newSampleEntry.iSampleDataArray = new(ELeave) CSampleDataArray(maxSamples);
    iSampleEntryArray->AppendL(newSampleEntry);
    }

    // add all disk drives
    for (TInt i=0; i<ESourceI-ESourceC+1; i++)
        {
        TSampleEntry newSampleEntry;

        TChar driveLetter = 'C' + i; // C is the first drive

        newSampleEntry.iDescription.Append(driveLetter);
        newSampleEntry.iDescription.Append(_L(":"));
        newSampleEntry.iUnitTypeShort.Copy(_L("b"));
        newSampleEntry.iUnitTypeLong.Copy(_L("bytes"));

        iEnv->FsSession().CharToDrive(driveLetter,
                newSampleEntry.iDriveNumber);

        newSampleEntry.iGraphColor = KRgbCyan;
        newSampleEntry.iGraphColor.SetGreen(255 - (i - ESourceC) * 30);
        newSampleEntry.iGraphColor.SetRed(i * 30);

        newSampleEntry.iSampleDataArray = new (ELeave) CSampleDataArray(
                maxSamples);

        iSampleEntryArray->AppendL(newSampleEntry);
        }

    // add power entry
    {
    TSampleEntry newSampleEntry;

    newSampleEntry.iDescription.Copy(_L("Power"));
    newSampleEntry.iUnitTypeShort.Copy(_L("mW"));
    newSampleEntry.iUnitTypeLong.Copy(_L("milliwatts"));
    newSampleEntry.iDriveNumber = -1;
    newSampleEntry.iGraphColor = KRgbRed;

    newSampleEntry.iSampleDataArray = new(ELeave) CSampleDataArray(maxSamples);
    iSampleEntryArray->AppendL(newSampleEntry);
    }

    // save current time as start time
    iStartTime.HomeTime();
    }

// --------------------------------------------------------------------------------------------

void CPerfMonEngine::UpdateSamplesDataL()
    {
    // reset inactivity timers
    if (iSettings.iKeepBacklightOn)
        User::ResetInactivityTime();

    // get current time
    TTime currentTime;
    currentTime.HomeTime();

    // calculate time difference
    TTimeIntervalMicroSeconds timeDeltaFromPreviousSample =
            currentTime.MicroSecondsFrom(iPreviousTime);

    // remember current time as previous
    iPreviousTime = currentTime;

    // get CPU load value for each CPU
    for (TInt i=0;i<iAmountOfCPUs;i++)
        {
        TInt64 cpuLoadDelta(0);
        TInt64 cpuLoadFree(0);
        TInt64 currentCPUValue(0);
        
        if (iCurrentCPUMode == ECPUModeCPUTime || iCurrentCPUMode == ECPUModeNOPs)
            {
            if (iCurrentCPUMode == ECPUModeCPUTime)
                {
                // get CPU time of null thread
                TTimeIntervalMicroSeconds time;
                iNullThreads[i].GetCpuTime(time);
                currentCPUValue = time.Int64();
                }
            else if (iCurrentCPUMode == ECPUModeNOPs)
                {
                // get CPU time from sampler
                currentCPUValue = iCPULoadNOPCounter[i].iCounterValue;
                }

            // get delta and store the previous value
            cpuLoadDelta = currentCPUValue - iCPULoadPreviousValue[i];
            iCPULoadPreviousValue[i] = currentCPUValue;
            
            // velocity = distance / time
            cpuLoadFree = cpuLoadDelta * KCPUTimeMultiplier / timeDeltaFromPreviousSample.Int64();

            // detect maximum value (this is common to all CPUs)
            if (cpuLoadFree > iCPULoadMaxValue)
                {
                iCPULoadMaxValue = cpuLoadFree;
                }
            }

        // save cpu sample data
        TSampleData cpuSample;
        cpuSample.iFree = cpuLoadFree;
        cpuSample.iSize = iCPULoadCalibrating ? cpuLoadFree : iCPULoadMaxValue;  // if calibrating, size==free, otherwise store the correct size value
        cpuSample.iTimeFromStart = currentTime.MicroSecondsFrom(iStartTime);

        iSampleEntryArray->At(i).iSampleDataArray->InsertL(0, cpuSample);
        }
        
    // check calibration status, the calibration will be only done against CPU0
    if (iCPULoadCalibrating)
        {
        iCPULoadCalibrationCounter++;
        TInt64 cpuLoadSize = iSampleEntryArray->At(iCPU0PositionInSamples).iSampleDataArray->At(0).iSize;
        
        // check if need to calibrate anymore
        if (iCPULoadCalibrationCounter > KCalibrationLength)
            {
            iCPULoadCalibrating = EFalse;
            
            // from the samples, get the minimum value, and let it be the max value
            for (TInt i=0; i<iSampleEntryArray->At(0).iSampleDataArray->Count(); i++)
                {
                TInt64 newCPULoadMaxValue = iCPULoadMaxValue;
                
                if (iSampleEntryArray->At(0).iSampleDataArray->At(i).iFree < newCPULoadMaxValue)
                    {
                    newCPULoadMaxValue = iSampleEntryArray->At(0).iSampleDataArray->At(i).iFree;
                    }
                
                iCPULoadMaxValue = newCPULoadMaxValue;
                }
            
            // adjust priority of the poller thread
            if (iCurrentCPUMode == ECPUModeNOPs)
                {
                for (TInt i=0; i<iAmountOfCPUs; i++)
                    {
                    iCPULoadNOPThread[i].SetPriority(EPriorityAbsoluteVeryLow);
                    }
                }

            }
        }

    // get ram memory
    TMemoryInfoV1Buf ramMemory;
    UserHal::MemoryInfo(ramMemory);

    TSampleData memorySample;
    memorySample.iFree = ramMemory().iFreeRamInBytes;
    memorySample.iSize = ramMemory().iMaxFreeRamInBytes;
    memorySample.iTimeFromStart = currentTime.MicroSecondsFrom(iStartTime);

    iSampleEntryArray->At(iRAMPositionInSamples).iSampleDataArray->InsertL(0, memorySample);
    
    // all drives
    for (TInt i = iCDrivePositionInSamples; i < iPowerPositionInSamples; i++)
        {
        TSampleData driveSample;

        // get volume info from RFs
        TVolumeInfo volumeInfo;
        if (iEnv->FsSession().Volume(volumeInfo,
                iSampleEntryArray->At(i).iDriveNumber) == KErrNone)
            {
            driveSample.iFree = volumeInfo.iFree;
            driveSample.iSize = volumeInfo.iSize;
            }
        else
            {
            driveSample.iFree = 0;
            driveSample.iSize = 0;
            }

        driveSample.iTimeFromStart = currentTime.MicroSecondsFrom(iStartTime);

        iSampleEntryArray->At(i).iSampleDataArray->InsertL(0, driveSample);
        }

    // Power data
    TSampleData powerSample;
    if (iSettings.iPowerMonitoringEnabled)
        {
        // Values in milliwatts
        powerSample.iFree = ( iPowerClient->GetMaxPower() - iPowerClient->GetPower() ) / 1000;
        powerSample.iSize = iPowerClient->GetMaxPower() / 1000;
        }
    else
        {
        powerSample.iFree = 0;
        powerSample.iSize = 0;
        }

    powerSample.iTimeFromStart = currentTime.MicroSecondsFrom(iStartTime);

    iSampleEntryArray->At(iPowerPositionInSamples).iSampleDataArray->InsertL(0, powerSample);

    // compress sample data arrays to save memory
    TInt curLength(iSampleEntryArray->At(0).iSampleDataArray->Count());

    TInt maxSamples =
            iSettings.iMaxSamples >= KMinimumSamplesLength ? iSettings.iMaxSamples
                    : KMinimumSamplesLength;

    if (curLength > maxSamples && curLength % 5 == 0)
        {
        for (TInt i = 0; i < iSampleEntryArray->Count(); i++)
            {
            if (SampleEntryArray()->At(i).iSampleDataArray->Count() > 0)
                {
                iSampleEntryArray->At(i).iSampleDataArray->ResizeL(maxSamples); // looses old samples
                iSampleEntryArray->At(i).iSampleDataArray->Compress();
                }
            }
        }
    }

// --------------------------------------------------------------------------------------------

void CPerfMonEngine::AppendLatestSamplesToLogsL()
    {
    if (iSettings.iLoggingEnabled && SampleEntryArray())
        {
        // loop all sources
        for (TInt i = 0; i < SampleEntryArray()->Count(); i++)
            {
            // check if this setting has been enabled and it has some data
            if (iSettings.iLoggingSources.iSrcEnabled[SampleEntryPosToSettingPos(i)]
                    && SampleEntryArray()->At(i).iSampleDataArray->Count() > 0)
                {
                // get current sample
                TSampleData& currentSample =
                        SampleEntryArray()->At(i).iSampleDataArray->At(0);

                TBuf<128> buf;
                buf.Append(_L("PERFMON;"));
                buf.Append(SampleEntryArray()->At(i).iDescription);
                buf.Append(_L(";"));
                buf.AppendNum(currentSample.iTimeFromStart.Int64());
                buf.Append(_L(";"));
                buf.AppendNum(currentSample.iFree);
                buf.Append(_L(";"));
                buf.AppendNum(currentSample.iSize);

                // print to RDebug
                if (iSettings.iLoggingMode == ELoggingModeRDebug
                        || iSettings.iLoggingMode == ELoggingModeRDebugLogFile)
                    {
                    RDebug::Print(buf);
                    }

                // print to log file
                if (iSettings.iLoggingMode == ELoggingModeLogFile
                        || iSettings.iLoggingMode == ELoggingModeRDebugLogFile)
                    {
                    buf.Append(_L("\r\n"));

                    TBuf8<128> buf8;
                    buf8.Copy(buf);

                    iLogFile.Write(buf8);
                    }
                }
            }
        }
    }

void CPerfMonEngine::LoadSettingsL()
    {
    // set defaults
    iSettings.iHeartBeat = 600;
    iSettings.iMaxSamples = 64;
    iSettings.iPriority = EThreadPriorityTypeNormal;
    iSettings.iCPUMode = ECPUModeCPUTime;
    iSettings.iKeepBacklightOn = ETrue;

    iSettings.iDataPopupVisibility = EDataPopupVisbilityAlwaysOn;
    iSettings.iDataPopupLocation = EDataPopupLocationTopRight;
    iSettings.iDataPopupSources.SetDefaults1();

    iSettings.iGraphsVerticalBarPeriod = 5;
    iSettings.iGraphsSources.SetDefaults2();

    iSettings.iLoggingMode = ELoggingModeRDebug;
    iSettings.iLoggingFilePath.Copy(KDefaultLogFilePath);
    iSettings.iLoggingSources.SetDefaults2();

    iSettings.iLoggingEnabled = EFalse;

    iSettings.iPowerMonitoringEnabled = ETrue;

    // make sure that the private path of this app in c-drive exists
    iEnv->FsSession().CreatePrivatePath(KSettingsDrive); // c:\\private\\20011385\\
    
    // handle settings always in the private directory 
    if (iEnv->FsSession().SetSessionToPrivate(KSettingsDrive) == KErrNone)
        {
        const TUid KUidPerfMon =
            {
            0x20011385
            };
        // open or create a dictionary file store
        CDictionaryFileStore* settingsStore = CDictionaryFileStore::OpenLC(
                iEnv->FsSession(), KSettingsFileName, KUidPerfMon);

        LoadDFSValueL(settingsStore, KPMSettingHeartBeat, iSettings.iHeartBeat);
        LoadDFSValueL(settingsStore, KPMSettingMaxSamples,
                iSettings.iMaxSamples);
        LoadDFSValueL(settingsStore, KPMSettingPriority, iSettings.iPriority);
        
        LoadDFSValueL(settingsStore, KPMSettingCPUMode, iSettings.iCPUMode);
        
        LoadDFSValueL(settingsStore, KPMSettingKeepBackLightOn,
                iSettings.iKeepBacklightOn);

        LoadDFSValueL(settingsStore, KPMSettingDataPopupVisbility,
                iSettings.iDataPopupVisibility);
        LoadDFSValueL(settingsStore, KPMSettingDataPopupLocation,
                iSettings.iDataPopupLocation);
        LoadDFSValueL(settingsStore, KPMSettingDataPopupSources,
                iSettings.iDataPopupSources);

        LoadDFSValueL(settingsStore, KPMSettingGraphsVerticalBarPeriod,
                iSettings.iGraphsVerticalBarPeriod);
        LoadDFSValueL(settingsStore, KPMSettingGraphsSources,
                iSettings.iGraphsSources);

        LoadDFSValueL(settingsStore, KPMSettingLoggingMode,
                iSettings.iLoggingMode);
        LoadDFSValueL(settingsStore, KPMSettingLoggingFilePath,
                iSettings.iLoggingFilePath);
        LoadDFSValueL(settingsStore, KPMSettingLoggingSources,
                iSettings.iLoggingSources);

        LoadDFSValueL(settingsStore, KPMSettingPowerMonitoringEnabled,
                iSettings.iPowerMonitoringEnabled);

        CleanupStack::PopAndDestroy(); // settingsStore
        }
    }

// --------------------------------------------------------------------------------------------

void CPerfMonEngine::SaveSettingsL()
    {
    // handle settings always in c:\\private\\20011385\\ 
    if (iEnv->FsSession().SetSessionToPrivate( KSettingsDrive ) == KErrNone)
        {
        // delete existing store to make sure that it is clean and not eg corrupted
        if (BaflUtils::FileExists(iEnv->FsSession(), KSettingsFileName))
            {
            iEnv->FsSession().Delete(KSettingsFileName);
            }
        const TUid KUidPerfMon =
            {
            0x20011385
            };
        // create a dictionary file store
        CDictionaryFileStore* settingsStore = CDictionaryFileStore::OpenLC(
                iEnv->FsSession(), KSettingsFileName, KUidPerfMon);

        SaveDFSValueL(settingsStore, KPMSettingHeartBeat, iSettings.iHeartBeat);
        SaveDFSValueL(settingsStore, KPMSettingMaxSamples,
                iSettings.iMaxSamples);
        SaveDFSValueL(settingsStore, KPMSettingPriority, iSettings.iPriority);
        SaveDFSValueL(settingsStore, KPMSettingCPUMode, iSettings.iCPUMode);
        SaveDFSValueL(settingsStore, KPMSettingKeepBackLightOn,
                iSettings.iKeepBacklightOn);

        SaveDFSValueL(settingsStore, KPMSettingDataPopupVisbility,
                iSettings.iDataPopupVisibility);
        SaveDFSValueL(settingsStore, KPMSettingDataPopupLocation,
                iSettings.iDataPopupLocation);
        SaveDFSValueL(settingsStore, KPMSettingDataPopupSources,
                iSettings.iDataPopupSources);

        SaveDFSValueL(settingsStore, KPMSettingGraphsVerticalBarPeriod,
                iSettings.iGraphsVerticalBarPeriod);
        SaveDFSValueL(settingsStore, KPMSettingGraphsSources,
                iSettings.iGraphsSources);

        SaveDFSValueL(settingsStore, KPMSettingLoggingMode,
                iSettings.iLoggingMode);
        SaveDFSValueL(settingsStore, KPMSettingLoggingFilePath,
                iSettings.iLoggingFilePath);
        SaveDFSValueL(settingsStore, KPMSettingLoggingSources,
                iSettings.iLoggingSources);

        SaveDFSValueL(settingsStore, KPMSettingPowerMonitoringEnabled,
                iSettings.iPowerMonitoringEnabled);

        settingsStore->CommitL();
        CleanupStack::PopAndDestroy(); // settingsStore
        }
    }

// ---------------------------------------------------------------------------

void CPerfMonEngine::LoadDFSValueL(CDictionaryFileStore* aDicFS,
        const TUid& aUid, TInt& aValue)
    {
    if (aDicFS->IsPresentL(aUid))
        {
        RDictionaryReadStream in;
        in.OpenLC(*aDicFS, aUid);
        aValue = in.ReadInt16L();
        CleanupStack::PopAndDestroy(); // in        
        }
    }

// ---------------------------------------------------------------------------

void CPerfMonEngine::LoadDFSValueL(CDictionaryFileStore* aDicFS,
        const TUid& aUid, TDes& aValue)
    {
    if (aDicFS->IsPresentL(aUid))
        {
        RDictionaryReadStream in;
        in.OpenLC(*aDicFS, aUid);
        TInt bufLength = in.ReadInt16L(); // get length of descriptor
        in.ReadL(aValue, bufLength); // get the descriptor itself
        CleanupStack::PopAndDestroy(); // in
        }
    }

// ---------------------------------------------------------------------------

void CPerfMonEngine::LoadDFSValueL(CDictionaryFileStore* aDicFS,
        const TUid& aUid, TPerfMonSources& aValue)
    {
    if (aDicFS->IsPresentL(aUid))
        {
        RDictionaryReadStream in;
        in.OpenLC(*aDicFS, aUid);
        TInt bufLength = in.ReadInt16L(); // get length of the array

        if (bufLength < 0 || bufLength > ESourcesLength) // check for validaty
            User::Leave(KErrNotSupported);

        for (TInt i = 0; i < bufLength; i++) // get all items
            aValue.iSrcEnabled[i] = in.ReadInt16L();

        CleanupStack::PopAndDestroy(); // in
        }
    }

// ---------------------------------------------------------------------------

void CPerfMonEngine::SaveDFSValueL(CDictionaryFileStore* aDicFS,
        const TUid& aUid, const TInt& aValue)
    {
    RDictionaryWriteStream out;
    out.AssignLC(*aDicFS, aUid);
    out.WriteInt16L(aValue);
    out.CommitL();
    CleanupStack::PopAndDestroy(); // out
    }

// ---------------------------------------------------------------------------

void CPerfMonEngine::SaveDFSValueL(CDictionaryFileStore* aDicFS,
        const TUid& aUid, const TDes& aValue)
    {
    RDictionaryWriteStream out;
    out.AssignLC(*aDicFS, aUid);
    out.WriteInt16L(aValue.Length()); // write length of the descriptor
    out.WriteL(aValue, aValue.Length()); // write the descriptor itself
    out.CommitL();
    CleanupStack::PopAndDestroy(); // out
    }

// ---------------------------------------------------------------------------

void CPerfMonEngine::SaveDFSValueL(CDictionaryFileStore* aDicFS,
        const TUid& aUid, const TPerfMonSources& aValue)
    {
    RDictionaryWriteStream out;
    out.AssignLC(*aDicFS, aUid);

    out.WriteInt16L(ESourcesLength); // write length of the array

    for (TInt i = 0; i < ESourcesLength; i++) // write all items
        out.WriteInt16L(aValue.iSrcEnabled[i]);

    out.CommitL();
    CleanupStack::PopAndDestroy(); // out
    }

// ---------------------------------------------------------------------------

void CPerfMonEngine::ActivatePowerMonitoringL()
    {
    if (!iPowerClient)
        {
        iPowerClient = CPerfMonPowerListener::NewL();
        }

    // disable power monitoring if initialization fails
    TInt err = iPowerClient->Activate();
    if (err != KErrNone )
        {
        DeActivatePowerMonitoring();

        iSettings.iPowerMonitoringEnabled = EFalse;
        }
    }

// ---------------------------------------------------------------------------

void CPerfMonEngine::DeActivatePowerMonitoring()
    {
    if (iPowerClient)
        {
        iPowerClient->DeActivate();
        }
    }

// ---------------------------------------------------------------------------

TInt CPerfMonEngine::SampleEntryPosToSettingPos(TInt aSampleEntryPos)
    {
    TInt settingPos(0); // return position of aSampleEntryPos in settings
    
    if (aSampleEntryPos >= iCPU0PositionInSamples && aSampleEntryPos < iRAMPositionInSamples)
        {
        settingPos = ESourceCPU;
        }
    else if (aSampleEntryPos == iRAMPositionInSamples)
        {
        settingPos = ESourceRAM;
        }
    else
        {
        settingPos = ESourceC + (aSampleEntryPos-iCDrivePositionInSamples);
        }
    
    return settingPos;
    }

// End of File
