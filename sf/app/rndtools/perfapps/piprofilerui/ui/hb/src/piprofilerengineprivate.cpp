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


#include <e32std.h>
#include <qstring.h>
#include <qlist.h>
#include <qstringlist.h>
#include <f32file.h>
#include <xqconversions.h>
#include <utf.h>
#include <bautils.h>
#include <sysutil.h>
#include "piprofilerengineprivate.h"
#include <piprofiler/ProfilerSession.h>
#include "pluginattributes.h"
#include "generalattributes.h"
#include "piprofilerengine.h"

// literals for default general setting values
_LIT8(KTraceOutput, "file_system");
_LIT8(KTraceDebugOutput, "debug_output");
_LIT8(KProfilerDefaultDrive, "E:\\data");
_LIT8(KProfilerDefaultPrefix, "Profiler_#");
_LIT(KProfilerEngineExe, "PIProfilerEngine.exe");
const TInt KProfilerDefaultTimedSamplingPeriod = 60; // Sampling time in seconds  

// ---------------------------------------------------------------------------

PIProfilerEnginePrivate::PIProfilerEnginePrivate(PIProfilerEngine *aEngine) :
    iPublic(aEngine), iSamplerAttributes(0), iStatusChecker(0), iLeaveProfilingOnAfterClosing(
        EFalse)
{

}

// ---------------------------------------------------------------------------

PIProfilerEnginePrivate::~PIProfilerEnginePrivate()
{

    // remove profiler client
    RProfiler::RemoveClient();
    // Terminate engine in case it is running.
    if (iLeaveProfilingOnAfterClosing == EFalse) {
        terminateEngine();
    }

    // delete sampler attributes.
    if (iSamplerAttributes) {
        delete iSamplerAttributes;
        iSamplerAttributes = 0;
    }

    if (iStatusChecker) {
        iStatusChecker->Cancel();
        delete iStatusChecker;
        iStatusChecker = NULL;
    }
}

// ---------------------------------------------------------------------------

bool PIProfilerEnginePrivate::Init()
{
    TRAPD(error, this->LaunchEngineL());
    if (error != KErrNone) {
        return false;
    }

    // initialize attribute arrays

    TRAP(error, iSamplerAttributes = new (ELeave) CArrayFixFlat<TSamplerAttributes> (20)); // max sampler count is 20
    if (error != KErrNone) {
        return false;
    }

    // engine status checker
    TRAP(error, iStatusChecker = CProfilerEngineStatusChecker::NewL());
    if (error != KErrNone) {
        return false;
    }    

    iStatusChecker->SetObserver(this);

    TRAP(error, LoadGeneralSettingsL());
    if (error != KErrNone) {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------


int PIProfilerEnginePrivate::LaunchEngineL()
{

    TRequestStatus stat = KRequestPending;
    RProcess proc;

    TInt err(KErrNone);

    // check if process exists
    err = FindProcessL(proc);

    // check if already exists and don't start a new eshell profiling
    if (err == KErrNotFound) {
        // try create new process
        err = proc.Create(KProfilerEngineExe, _L(""));

        // check if RProcess::Create() succeeded
        if (err == KErrNone) {
            // Trigger rendezvous on the supplied TRequestStatus object
            proc.Rendezvous(stat);

            // kick off the engine process
            proc.Resume();

            // wait for the constructor to complete 
            User::WaitForRequest(stat);

            // just lose the handle
            proc.Close();
        }
    }

    // Increase the client reference count in server:
    AttachClient();
}

// ---------------------------------------------------------------------------

void PIProfilerEnginePrivate::AttachClient()
{
    RProfiler::AttachClient();
}
// ---------------------------------------------------------------------------

int PIProfilerEnginePrivate::FindProcessL(RProcess& aProc)
{
    TProcessId engId;
    TFindProcess procName;
    procName.Find(_L("PIProfilerEngine.exe*"));
    TFullName aResult;
    TFullName aResult2;
    TInt err(KErrNone);

    // find the first appearance
    err = procName.Next(aResult);
    if (err != KErrNone) {
        // did not find any engine process
        return err;
    }
    else {
        err = aProc.Open(procName);
        if (err == KErrNone) {
            if (aProc.ExitCategory().Length() > 0) {
                aProc.Close();
                // process already exited => create a new one
                return KErrNotFound;
            }
            aProc.Close();
        }
    }

    // check now if a second appearance exists in process list, 
    // i.e. engine started from eshell => two engine processes appear in normal case
    procName.Next(aResult2);

    // check if aResult2 contained the second appearance of profiler engine
    if(aResult2.CompareF(aResult) > 0)
        {
        // other process found, i.e. right process to communicate with, in case started from eshell
        err = aProc.Open(procName);
        if(err == KErrNone)
            {
            if(aProc.ExitCategory().Length() > 0)
                {
                // process already exited => create a new one
                return KErrNotFound;
                }
            aProc.Close();
            }
        }

    return err;
}

// ---------------------------------------------------------------------------

void PIProfilerEnginePrivate::terminateEngine()
{
    // exit profiler engine 
    RProfiler::ExitProfiler();
}
// ---------------------------------------------------------------------------

void PIProfilerEnginePrivate::NotifyUIReady()
{
    // load initial plugins
    loadPlugins();

    // get the initial state
    int initialState = iStatusChecker->GetInitialState();
    if (initialState == ERunning) {
        HandleProfilerStatusChange(ERunning);
    }
}

// ---------------------------------------------------------------------------


void PIProfilerEnginePrivate::loadPlugins()
{
    // get samplers from Profiler Engine (client-server session) 
    // and add the to the samplers list for the first time
    LOGTEXT(_L("CProfilerGuiModel::LoadPlugins - get sampler plugins"));

    TInt err = RProfiler::GetSamplerAttributes(*iSamplerAttributes);

    // check if engine provided a list of samplers
    if (err != KErrNone) {
        // could not get samplers from engine
        LOGTEXT(_L("CProfilerGuiModel::LoadPlugins - failed to connect engine"));
    }
    else {
        LOGTEXT(_L("CProfilerGuiModel::LoadPlugins - adding new samplers into view"));
        addNewSamplers(*iSamplerAttributes);
    }LOGTEXT(_L("CProfilerGuiModel::LoadPlugins - exit"));
}
// ---------------------------------------------------------------------------

void PIProfilerEnginePrivate::addNewSamplers(CArrayFixFlat<TSamplerAttributes>& aAttributes)
{
    TSamplerAttributes item;

    TInt count(aAttributes.Count());

    // loop the attribute array and insert them into view list

    QList<PluginAttributes> samplerList;

    for (TInt i(0); i < count; i++) {
        // get a TSamplerAttributes from list at a time  
        item = aAttributes.At(i);

        PluginAttributes samplerAttributes;
        convertTSamplerAttributesToPluginAttributes(item, samplerAttributes);

        samplerList.append(samplerAttributes);
    }

    emit iPublic->pluginListUpdated(samplerList);

}

// ---------------------------------------------------------------------------

void PIProfilerEnginePrivate::convertTSamplerAttributesToPluginAttributes(
    TSamplerAttributes &tSamplerAttributes, PluginAttributes &samplerAttributes)
{

    samplerAttributes.mUid = tSamplerAttributes.iUid;
    samplerAttributes.mSampleRate = tSamplerAttributes.iSampleRate;
    samplerAttributes.mEnabled = tSamplerAttributes.iEnabled;
    samplerAttributes.mIsHidden = tSamplerAttributes.iIsHidden;
    samplerAttributes.mItemCount = tSamplerAttributes.iItemCount;

    TBuf16<8> temp8;
    temp8.Copy(tSamplerAttributes.iShortName);
    samplerAttributes.mShortName = QString((QChar*) temp8.Ptr(), temp8.Length());

    TBuf16<64> temp64;
    temp64.Copy(tSamplerAttributes.iName);
    samplerAttributes.mName = QString((QChar*) temp64.Ptr(), temp64.Length());

    TBuf16<256> temp256;
    temp256.Copy(tSamplerAttributes.iDescription);
    samplerAttributes.mDescription = QString((QChar*) temp256.Ptr(), temp256.Length());

    convertTSettingItemToSettingItem(tSamplerAttributes.iSettingItem1,
        samplerAttributes.mSettingItem1);
    convertTSettingItemToSettingItem(tSamplerAttributes.iSettingItem2,
        samplerAttributes.mSettingItem2);
    convertTSettingItemToSettingItem(tSamplerAttributes.iSettingItem3,
        samplerAttributes.mSettingItem3);
    convertTSettingItemToSettingItem(tSamplerAttributes.iSettingItem4,
        samplerAttributes.mSettingItem4);
    convertTSettingItemToSettingItem(tSamplerAttributes.iSettingItem5,
        samplerAttributes.mSettingItem5);
    convertTSettingItemToSettingItem(tSamplerAttributes.iSettingItem6,
        samplerAttributes.mSettingItem6);
}

// ---------------------------------------------------------------------------

bool PIProfilerEnginePrivate::SavePluginSettings(const PluginAttributes &samplerAttributes)
{
    for (int index = 0; index < iSamplerAttributes->Count(); index++) {
        if (samplerAttributes.mUid == this->iSamplerAttributes->At(index).iUid) {
            iSamplerAttributes->At(index).iEnabled = samplerAttributes.mEnabled;
            iSamplerAttributes->At(index).iSampleRate = samplerAttributes.mSampleRate;

            convertSettingItemToTSettingItem(this->iSamplerAttributes->At(index).iSettingItem1,
                samplerAttributes.mSettingItem1);
            convertSettingItemToTSettingItem(this->iSamplerAttributes->At(index).iSettingItem2,
                samplerAttributes.mSettingItem2);
            convertSettingItemToTSettingItem(this->iSamplerAttributes->At(index).iSettingItem3,
                samplerAttributes.mSettingItem3);
            convertSettingItemToTSettingItem(this->iSamplerAttributes->At(index).iSettingItem4,
                samplerAttributes.mSettingItem4);
            convertSettingItemToTSettingItem(this->iSamplerAttributes->At(index).iSettingItem5,
                samplerAttributes.mSettingItem5);
            convertSettingItemToTSettingItem(this->iSamplerAttributes->At(index).iSettingItem6,
                samplerAttributes.mSettingItem6);

            TSamplerAttributes attr = iSamplerAttributes->At(index);

            if (RProfiler::SetSamplerAttributes(this->iSamplerAttributes->At(index)) == KErrNone) {
                return true;
            }
            else {
                return false;
            }
        }
    }
    return false;

}

// ---------------------------------------------------------------------------

void PIProfilerEnginePrivate::StartAllSamplerItemsL(TProfilingMode aProfilingMode)
{
    TBuf<256> activeWriterDes;
    TBuf8<256> writer8;

    //iState = MProfilerStatusObserver::EInitializing;

    RProfiler::TProfilingMode profilingMode = aProfilingMode == EProfilingModeTimed
        ? RProfiler::EProfilingModeTimed : RProfiler::EProfilingModeNormal;

    // try to start profiling process through client-server interface
    if (RProfiler::StartSampling(profilingMode) == KErrNotFound) {
        // profiler stopped (e.g. from eshell) and must be restarted 
        LaunchEngineL();
        // try to launch sampling again
        RProfiler::StartSampling(profilingMode);
    }
}

// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

void PIProfilerEnginePrivate::StopProfiling()
{
    // Stop profiling process through CS session
    RProfiler::StopSampling();

}

// --------------------------------------------------------------------------------------------

void PIProfilerEnginePrivate::HandleProfilerStatusChange(KProfilerStatus aStatus)
{
    if (aStatus == EIdle || aStatus == ERunning) {

        if (iGeneralAttributes.iTraceOutput == KTraceOutput) {

            TBuf<256> buf;
            // get profiler file name 
            TBool valu = RProfiler::GetFileName(buf);

            QString filename = QString((QChar*) buf.Ptr(), buf.Length());

            // Let ui know that status has changed

            if (aStatus == EIdle) {
                QString text = QString("Wrote trace data to: \n");
                text.append(filename);
                emit iPublic->profilingStatusChanged(PI_FINISHED_SUCCEFULLY, text,
                    PI_PROFILINGMODENORMAL, PI_FILE_OUTPUT);
            }
            else if (aStatus == ERunning) {
                QString text = QString("Writing trace data to: \n");
                text.append(filename);
                emit iPublic->profilingStatusChanged(PI_PROFILING, text, PI_PROFILINGMODENORMAL,
                    PI_FILE_OUTPUT);
            }
        }
        else {
            // Let ui know that status has changed
            if (aStatus == EIdle) {
                emit iPublic->profilingStatusChanged(PI_FINISHED_SUCCEFULLY, QString(
                    "Wrote trace data to debug output"), PI_PROFILINGMODENORMAL, PI_DEBUG_OUTPUT);
            }
            else if (aStatus == ERunning) {
                emit iPublic->profilingStatusChanged(PI_PROFILING, QString(
                    "Writing trace data to debug output"), PI_PROFILINGMODENORMAL, PI_DEBUG_OUTPUT);
            }
        }
    }

}

// ---------------------------------------------------------------------------

void PIProfilerEnginePrivate::HandleProfilerErrorL(TInt aError)
{
    {
        QString errorMsg = QString("Error: ");
        QString KNoMemory = QString("Cannot write to file, check settings");

        // message from pwr sampler
        if (aError < -1000) {
            errorMsg.append(QString("Stop other power measurement tools!"));
        }
        else if (aError == KErrAlreadyExists || aError == 11) {
            errorMsg.append(QString("Close old Profiler before start!"));
        }
        else if (aError == KErrNotReady) {
            errorMsg.append(QString("Memory card removed, failed to write!"));
        }
        else if (aError == KErrPathNotFound) {
            errorMsg.append(QString("Given trace data location does not exist"));
        }
        else {
            if (aError == KErrNoMemory || aError == KErrOverflow || aError == KErrDirFull || aError
                == KErrDiskFull || aError == KErrNotReady) {
                errorMsg.append(KNoMemory);
            }
            else {
                errorMsg.append(QString("code: "));
                errorMsg.append(aError);
            }
        }

        emit iPublic->profilingStatusChanged(PI_ERROR, errorMsg);
    }

}
// ---------------------------------------------------------------------------

bool PIProfilerEnginePrivate::StartProfiling()
{
    TRAPD(error, this->StartAllSamplerItemsL(PIProfilerEnginePrivate::EProfilingModeNormal));
    if (error == KErrNone) {
        return true;
    }
    return false;

}

// ---------------------------------------------------------------------------

bool PIProfilerEnginePrivate::StartTimedProfiling()
{
    TRAPD(error, this->StartAllSamplerItemsL(PIProfilerEnginePrivate::EProfilingModeTimed));
    if (error == KErrNone) {
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------

void PIProfilerEnginePrivate::convertTGeneralAttributesToGeneralAttributes(
    TGeneralAttributes &tSamplerAttributes, GeneralAttributes &samplerAttributes)
{
    samplerAttributes.mTimedSamplingPeriod = tSamplerAttributes.iTimedSamplingPeriod;
    int test = tSamplerAttributes.iSaveFileDrive.Length();
    TBuf16<KPrefixMaxLength> test2;
    test2.Copy(tSamplerAttributes.iSaveFileDrive);
    samplerAttributes.mSaveFileDrive = QString((QChar*) test2.Ptr(), test2.Length());
    test2.Copy(tSamplerAttributes.iTraceFilePrefix);
    samplerAttributes.mTraceFilePrefix = QString((QChar*) test2.Ptr(), test2.Length());
    test2.Copy(tSamplerAttributes.iTraceOutput);
    samplerAttributes.mTraceOutput = QString((QChar*) test2.Ptr(), test2.Length());
}
// ---------------------------------------------------------------------------

void PIProfilerEnginePrivate::convertGeneralAttributesToTGeneralAttributes(
    TGeneralAttributes &tSamplerAttributes, GeneralAttributes &samplerAttributes)
{
    tSamplerAttributes.iTimedSamplingPeriod = samplerAttributes.mTimedSamplingPeriod;

    tSamplerAttributes.iTraceOutput.Copy(TBuf<KPrefixMaxLength> (
        samplerAttributes.mTraceOutput.utf16()));
    tSamplerAttributes.iSaveFileDrive.Copy(TBuf<KPrefixMaxLength> (
        samplerAttributes.mSaveFileDrive.utf16()));
    tSamplerAttributes.iTraceFilePrefix.Copy(TBuf<KPrefixMaxLength> (
        samplerAttributes.mTraceFilePrefix.utf16()));

}
// ---------------------------------------------------------------------------

TInt PIProfilerEnginePrivate::LoadGeneralSettingsL()
{
    // local variable for getting saved settings from profiler engine
    TGeneralAttributes generalAttr;
    TInt err(KErrNone);

    // before loading saved settings (from settings file) set the default values 
    iGeneralAttributes.iTraceOutput.Copy(KTraceOutput);
    iGeneralAttributes.iTraceFilePrefix.Copy(KProfilerDefaultPrefix);
    iGeneralAttributes.iSaveFileDrive.Copy(KProfilerDefaultDrive);
    iGeneralAttributes.iTimedSamplingPeriod = KProfilerDefaultTimedSamplingPeriod;

    // request to 
    err = RProfiler::GetGeneralAttributes(generalAttr);

    // check that request succesfull
    if (err != KErrNone) {
        // could not connect profiler engine, use 
        return err;
    }

    // check if saved settings different than the default
    if (generalAttr.iTraceOutput.MatchF(iGeneralAttributes.iTraceOutput) == KErrNotFound) {
        iGeneralAttributes.iTraceOutput.Copy(generalAttr.iTraceOutput);
    }

    if (generalAttr.iTraceFilePrefix.MatchF(iGeneralAttributes.iTraceFilePrefix) == KErrNotFound) {
        iGeneralAttributes.iTraceFilePrefix.Copy(generalAttr.iTraceFilePrefix);
    }

    if (generalAttr.iSaveFileDrive.MatchF(iGeneralAttributes.iSaveFileDrive) == KErrNotFound) {
        iGeneralAttributes.iSaveFileDrive.Copy(generalAttr.iSaveFileDrive);
    }

    if (generalAttr.iTimedSamplingPeriod > 0) {
        iGeneralAttributes.iTimedSamplingPeriod = generalAttr.iTimedSamplingPeriod;
    }

    return err;
}

// ---------------------------------------------------------------------------

void PIProfilerEnginePrivate::GetGeneralSettings(GeneralAttributes &settings)
{
    convertTGeneralAttributesToGeneralAttributes(iGeneralAttributes, settings);
}
// ---------------------------------------------------------------------------

bool PIProfilerEnginePrivate::SaveGeneralSettings(GeneralAttributes &settings)
{
    convertGeneralAttributesToTGeneralAttributes(iGeneralAttributes, settings);
    TRAPD(error, SaveGeneralSettingsL());
    if (error != KErrNone) {
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------

void PIProfilerEnginePrivate::SaveGeneralSettingsL()
{
    TInt err(KErrNone);

    // save general attributes to Profiler Engine
    err = RProfiler::SetGeneralAttributes(iGeneralAttributes);

    // check if save failed
    if (err == KErrNotFound) {
        // profiler stopped (e.g. from eshell) and must be restarted 
        LaunchEngineL();

        err = RProfiler::SetGeneralAttributes(iGeneralAttributes);
        if (err != KErrNone) {
            // leave no use to continue
            User::Leave(err);
        }
    }
}
// --------------------------------------------------------------------------------------------

int PIProfilerEnginePrivate::GetTimeLimit()
{
    return iGeneralAttributes.iTimedSamplingPeriod;
}

// --------------------------------------------------------------------------------------------

void PIProfilerEnginePrivate::convertTSettingItemToSettingItem(TSettingItem &tSettingItem,
    SettingItem &settingItem)
{

    settingItem.mType = tSettingItem.iType;
    settingItem.mSettingDescription = QString((QChar*) tSettingItem.iSettingDescription.Ptr(),
        tSettingItem.iSettingDescription.Length());
    settingItem.mSettingText = QString((QChar*) tSettingItem.iSettingText.Ptr(),
        tSettingItem.iSettingText.Length());
    settingItem.mUIText = QString((QChar*) tSettingItem.iUIText.Ptr(),
        tSettingItem.iUIText.Length());
    settingItem.mValue = QString((QChar*) tSettingItem.iValue.Ptr(), tSettingItem.iValue.Length());

}
// --------------------------------------------------------------------------------------------

void PIProfilerEnginePrivate::convertSettingItemToTSettingItem(TSettingItem &tSettingItem,
    const SettingItem &settingItem)
{
    tSettingItem.iType = settingItem.mType;
    tSettingItem.iSettingDescription.Copy(TBuf<256> (settingItem.mSettingDescription.utf16()));
    tSettingItem.iSettingText.Copy(TBuf<64> (settingItem.mSettingText.utf16()));
    tSettingItem.iUIText.Copy(TBuf<64> (settingItem.mUIText.utf16()));
    tSettingItem.iValue.Copy(TBuf<128> (settingItem.mValue.utf16()));

}

// --------------------------------------------------------------------------------------------

void PIProfilerEnginePrivate::LeaveProfilingOnAfterClosing()
{
    iLeaveProfilingOnAfterClosing = ETrue;
}

// --------------------------------------------------------------------------------------------

bool PIProfilerEnginePrivate::CheckTraceLocationSanity(QString& location)
{
    TBool value = EFalse;

    TBuf8<KPrefixMaxLength> fileLocation;
    fileLocation.Copy(TBuf<KPrefixMaxLength> (location.utf16()));
    TRAPD(error, value = CheckTraceLocationSanityL(fileLocation));
    if (error != KErrNone) {
        return false;
    }
    return value;
}

// --------------------------------------------------------------------------------------------

TBool PIProfilerEnginePrivate::CheckTraceLocationSanityL(TBuf8<KPrefixMaxLength> &aAttr)
{
    RFs fs;
    User::LeaveIfError(fs.Connect());

    TBuf<32> drive;

    CnvUtfConverter::ConvertToUnicodeFromUtf8(drive, aAttr);

    TDriveUnit driveUnit = TDriveUnit(drive);

    TBool ret(EFalse);

    // check that the root folder is correct
    if (drive.Length() > 2 && BaflUtils::CheckFolder(fs, drive.Left(3)) == KErrNone) {
        // check then if drive has still some space
        if (!SysUtil::DiskSpaceBelowCriticalLevelL(&fs, 0, driveUnit)) {
            ret = ETrue;
        }
    }

    fs.Close();
    return ret;
}
