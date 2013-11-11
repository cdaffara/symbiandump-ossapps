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


#ifndef PERFMON_ENGINE_H
#define PERFMON_ENGINE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <apgcli.h>
#include <gdi.h>

#include "perfmon.hrh"
#include "perfmon_powerlistener.h"

const TUint KMaxCPUs = 4;


// setting keys (do not change uids of existing keys to maintain compatibility to older versions!)
const TUid KPMSettingHeartBeat                              = { 0x00 };
const TUid KPMSettingMaxSamples                             = { 0x01 };
const TUid KPMSettingPriority                               = { 0x02 };
const TUid KPMSettingCPUMode                                = { 0x03 };
const TUid KPMSettingKeepBackLightOn                        = { 0x04 };

const TUid KPMSettingDataPopupVisbility                     = { 0x05 };
const TUid KPMSettingDataPopupLocation                      = { 0x06 };
const TUid KPMSettingDataPopupSources                       = { 0x07 };

const TUid KPMSettingGraphsVerticalBarPeriod                = { 0x08 };
const TUid KPMSettingGraphsSources                          = { 0x09 };

const TUid KPMSettingLoggingMode                            = { 0x0A };
const TUid KPMSettingLoggingFilePath                        = { 0x0B };
const TUid KPMSettingLoggingSources                         = { 0x0C };

const TUid KPMSettingPowerMonitoringEnabled                 = { 0x0D };

// FORWARD DECLARATIONS
class CPerfMonValuesContainer;
class CPerfMonGraphsContainer;
class CPerfMonDataPopupContainer;
class CEikonEnv;
class CCoeControl;
class CDictionaryFileStore;


// CLASS DECLARATIONS

class TPerfMonNOPCounter
    {
public:
    TInt64      iCounterValue;
    TInt        iCPUNumber;
    TInt        iAmountOfCPUs;
    };

class TPerfMonSources
    {
public:
    TBool iSrcEnabled[ESourcesLength];

public:
    inline void SetDefaults1()
        {
        iSrcEnabled[ESourceCPU] = ETrue;
        iSrcEnabled[ESourceRAM] = ETrue;
        iSrcEnabled[ESourceC]   = ETrue;
        iSrcEnabled[ESourceD]   = EFalse;
        iSrcEnabled[ESourceE]   = EFalse;
        iSrcEnabled[ESourceF]   = EFalse;
        iSrcEnabled[ESourceG]   = EFalse;
        iSrcEnabled[ESourceH]   = EFalse;
        iSrcEnabled[ESourceI]   = EFalse;
        iSrcEnabled[ESourcePwr] = EFalse;
        }
    inline void SetDefaults2()
        {
        iSrcEnabled[ESourceCPU] = ETrue;
        iSrcEnabled[ESourceRAM] = ETrue;
        iSrcEnabled[ESourceC]   = EFalse;
        iSrcEnabled[ESourceD]   = EFalse;
        iSrcEnabled[ESourceE]   = EFalse;
        iSrcEnabled[ESourceF]   = EFalse;
        iSrcEnabled[ESourceG]   = EFalse;
        iSrcEnabled[ESourceH]   = EFalse;
        iSrcEnabled[ESourceI]   = EFalse;
        iSrcEnabled[ESourcePwr] = EFalse;
        }
    TInt EnabledSourcesCount()
        {
        TInt srcCount(0);
        
        for (TInt i=0; i<ESourcesLength; i++)
            {
            if (iSrcEnabled[i])
                srcCount++;
            }
        
        return srcCount;
        }
    };
    

class TPerfMonSettings
    {
public:
    TInt                iHeartBeat;
    TInt                iMaxSamples;
    TInt                iPriority;
    TInt                iCPUMode;
    TBool               iKeepBacklightOn;

    TInt                iDataPopupVisibility;
    TInt                iDataPopupLocation;
    TPerfMonSources     iDataPopupSources;

    TInt                iGraphsVerticalBarPeriod;
    TPerfMonSources     iGraphsSources;

    TInt                iLoggingMode;
    TFileName           iLoggingFilePath;
    TPerfMonSources     iLoggingSources;
    
    TBool               iLoggingEnabled;

    TBool               iPowerMonitoringEnabled;
    };


class TSampleData
    {
public:
    TInt64                          iFree;
    TInt64                          iSize;
    TTimeIntervalMicroSeconds       iTimeFromStart;
    };

typedef CArrayFixSeg<TSampleData> CSampleDataArray;


class TSampleEntry
    {
public:
    TBuf<16>            iDescription;
    TBuf<16>            iUnitTypeShort; //eg. b
    TBuf<16>            iUnitTypeLong;  //eg. bytes
    TInt                iDriveNumber;   //used only for disk drives
    TRgb                iGraphColor;
    CSampleDataArray*   iSampleDataArray;
    };

typedef CArrayFixSeg<TSampleEntry> CSampleEntryArray;



class CPerfMonEngine : public CActive
    {
public:
    ~CPerfMonEngine();
    void ActivateEngineL();
    void DeActivateEngineL();
    void EnableLogging(TBool aEnable);

private:
    void RunL();
    void DoCancel();

    void LoadSettingsL();
    void OpenLogFile(TBool aOpen);
    void CreateSamplesDataArrayL();
    void UpdateSamplesDataL();
    void AppendLatestSamplesToLogsL();
    void ActivateCPUMonitoringL();
    TBool OpenHandleToNullThread();
    void DeActivateCPUMonitoring();
    TBool CPUTimeSupported();
    TInt GetAmountOfCPUs();
    TThreadPriority SettingItemToThreadPriority(TInt aIndex);
    void LoadDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, TInt& aValue);
    void LoadDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, TDes& aValue);
    void LoadDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, TPerfMonSources& aValue);
    void SaveDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, const TInt& aValue);
    void SaveDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, const TDes& aValue);
    void SaveDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, const TPerfMonSources& aValue);
    void ActivatePowerMonitoringL();
    void DeActivatePowerMonitoring();

    TInt SampleEntryPosToSettingPos(TInt aSampleEntryPos);

protected:
    CPerfMonEngine();
    void ConstructL();
    void SaveSettingsL();
    virtual void SendDrawEventToContainersL() = 0;
    virtual void HandleSettingsChangeL();
    
public:
    inline TPerfMonSettings& Settings() { return iSettings; }
    inline CEikonEnv* EikonEnv() { return iEnv; }
    inline RApaLsSession& LsSession() { return iLs; }

    inline CSampleEntryArray* SampleEntryArray() { return iSampleEntryArray; }
    inline TInt CPU0PositionInSamples()          { return iCPU0PositionInSamples; }
    inline TInt RAMPositionInSamples()           { return iRAMPositionInSamples; }
    inline TInt CDrivePositionInSamples()        { return iCDrivePositionInSamples; }
    inline TInt PowerPositionInSamples()         { return iPowerPositionInSamples; }
    inline TInt AmountOfCPUs()                   { return iAmountOfCPUs; }
    inline TBool PowerMonitoringSupported()      { return iPowerClient->IsSupported(); }

protected:
    RTimer                          iTimer;
    CEikonEnv*                      iEnv;
    TPerfMonSettings                iSettings;
    RApaLsSession                   iLs;
    CSampleEntryArray*              iSampleEntryArray;
    TTime                           iStartTime;

    TInt                            iCurrentCPUMode;

    RThread                         iCPULoadNOPThread[KMaxCPUs];
    TPerfMonNOPCounter              iCPULoadNOPCounter[KMaxCPUs];

    RThread                         iNullThreads[KMaxCPUs];
    TBool                           iCPULoadCalibrating;
    TInt                            iCPULoadCalibrationCounter;
    TInt64                          iCPULoadMaxValue;
    TInt64                          iCPULoadPreviousValue[KMaxCPUs];
    TInt                            iAmountOfCPUs;
    TTime                           iPreviousTime;

    TInt                            iCPU0PositionInSamples;
    TInt                            iRAMPositionInSamples;
    TInt                            iCDrivePositionInSamples;
    TInt                            iPowerPositionInSamples;

    TBool                           iLogFileInitialized;
    RFile                           iLogFile;

    CPerfMonPowerListener*          iPowerClient;
    };


#endif
