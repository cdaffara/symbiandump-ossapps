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
* Description: This file contains testclass implementation.
*
*/

// System includes
#include <e32svr.h>
#include <e32cmn.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <data_caging_path_literals.hrh>
#include <centralrepository.h>

// User includes
#include "SettingsTestModule.h"
#include "cradiosettings.h"
#include "cradiosettingsimp.h"
#include "mradiosettingssetter.h"
#include "mradioapplicationsettings.h"
#include "mradioenginesettings.h"
#include "radiointernalcrkeys.h"
#include "radioengineutils.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPresetUtilityTestModule::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CSettingsTestModule::Delete()
    {
    FUNC_LOG;

    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CSettingsTestModule::RunMethodL(CStifItemParser& aItem)
    {
    FUNC_LOG;

    static TStifFunctionInfo const
            KFunctions[] =
                {
                // Copy this line for every implemented function.
                        // First string is the function name used in TestScripter script file.
                        // Second is the actual implementation member function. 
                        ENTRY( "Example", CSettingsTestModule::ExampleL ),
                        // [test cases entries] - Do not remove
                                ENTRY( "CreateSettingsL", CSettingsTestModule::CreateSettingsL ),
                                ENTRY( "DestroySettingsL", CSettingsTestModule::DestroySettingsL ),
                                ENTRY( "IsRegionAllowed", CSettingsTestModule::IsRegionAllowed ),
                                ENTRY( "ResolveDriveL", CSettingsTestModule::ResolveDriveL ),
                                ENTRY( "SetFirstTimeStartPerformed", CSettingsTestModule::SetFirstTimeStartPerformed ),
                                ENTRY( "IsFirstTimeStart", CSettingsTestModule::IsFirstTimeStart ),
                                ENTRY( "SetUiFlags", CSettingsTestModule::SetUiFlags ),
                        ENTRY( "UiFlags", CSettingsTestModule::UiFlags ),
                                ENTRY( "HeadsetVolume", CSettingsTestModule::HeadsetVolume ),
                                ENTRY( "SpeakerVolume", CSettingsTestModule::SpeakerVolume ),
                        ENTRY( "Volume", CSettingsTestModule::Volume ),
                                ENTRY( "OutputMode", CSettingsTestModule::OutputMode ),
                                ENTRY( "AudioRoute", CSettingsTestModule::AudioRoute ),
                                ENTRY( "IsHeadsetVolMuted", CSettingsTestModule::IsHeadsetVolMuted ),
                                ENTRY( "IsSpeakerVolMuted", CSettingsTestModule::IsSpeakerVolMuted ),
                                ENTRY( "IsVolMuted", CSettingsTestModule::IsVolMuted ),
                        ENTRY( "IsPowerOn", CSettingsTestModule::IsPowerOn ),
                                ENTRY( "TunedFrequency", CSettingsTestModule::TunedFrequency ),
                                ENTRY( "DefaultMinVolumeLevel", CSettingsTestModule::DefaultMinVolumeLevel ),
                                ENTRY( "CountRegions", CSettingsTestModule::CountRegions ),
                        ENTRY( "Region", CSettingsTestModule::Region ),
                        ENTRY( "RegionId", CSettingsTestModule::RegionId ),
                                ENTRY( "FrequencyStepSize", CSettingsTestModule::FrequencyStepSize ),
                                ENTRY( "MaxFrequency", CSettingsTestModule::MaxFrequency ),
                                ENTRY( "MinFrequency", CSettingsTestModule::MinFrequency ),
                                ENTRY( "DecimalCount", CSettingsTestModule::DecimalCount ),
                                ENTRY( "DefaultRegion", CSettingsTestModule::DefaultRegion ),
                        ENTRY( "NetworkId", CSettingsTestModule::NetworkId ),
                                ENTRY( "CountryCode", CSettingsTestModule::CountryCode ),
                                ENTRY( "SetHeadsetVolume", CSettingsTestModule::SetHeadsetVolume ),
                                ENTRY( "SetSpeakerVolume", CSettingsTestModule::SetSpeakerVolume ),
                        ENTRY( "SetVolume", CSettingsTestModule::SetVolume ),
                                ENTRY( "SetOutputMode", CSettingsTestModule::SetOutputMode ),
                                ENTRY( "SetAudioRoute", CSettingsTestModule::SetAudioRoute ),
                                ENTRY( "SetHeadsetVolMuted", CSettingsTestModule::SetHeadsetVolMuted ),
                                ENTRY( "SetSpeakerVolMuted", CSettingsTestModule::SetSpeakerVolMuted ),
                                ENTRY( "SetVolMuted", CSettingsTestModule::SetVolMuted ),
                                ENTRY( "SetPowerOn", CSettingsTestModule::SetPowerOn ),
                                ENTRY( "SetTunedFrequency", CSettingsTestModule::SetTunedFrequency ),
                                ENTRY( "SetRegionId", CSettingsTestModule::SetRegionId ),
                                ENTRY( "SetCountryCode", CSettingsTestModule::SetCountryCode ),
                                ENTRY( "SetNetworkId", CSettingsTestModule::SetNetworkId ),
                                ENTRY( "tstSetRegionIdL", CSettingsTestModule::tstSetRegionIdL ),
                                ENTRY( "tstResetStartCount", CSettingsTestModule::tstResetStartCount ),
                                ENTRY( "ReadConfigurableKeysL", CSettingsTestModule::ReadConfigurableKeysL )
                //ADD NEW ENTRY HERE

                };

    const TInt count = sizeof(KFunctions) / sizeof(TStifFunctionInfo);

    return RunInternalL(KFunctions, count, aItem);

    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSettingsTestModule::ExampleL(CStifItemParser& aItem)
    {
    FUNC_LOG;
    // Print to UI
    _LIT( KSettingsTestModule, "SettingsTestModule" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf(0, KSettingsTestModule, KExample);
    // Print to log file
    iLog->Log(KExample);

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while (aItem.GetNextString(string) == KErrNone)
        {
        TestModuleIf().Printf(i, KSettingsTestModule, KParam, i, &string);
        ++i;
        }

    return KErrNone;

    }

//-----------------------------------------------------------------------------
// CPresetUtilityTest::CreateSettingsL
//-----------------------------------------------------------------------------
TInt CSettingsTestModule::CreateSettingsL(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;

    TInt err = KErrNone;
    if (!iSettings)
        {
        RadioEngineUtils::InitializeL();
        TRAPD( err, iSettings = CRadioSettings::NewL() );
        if (KErrNone == err)
            {
            iSettings->RadioSetter().SetObserver(this);
            }
        else
            {
            RadioEngineUtils::Release();
            INFO_1( "CRadioSettings::NewL() failed err = %i", err )
            User::Leave(err);
            }
        }
    else
        {
        err = KErrArgument;
        }

    return err;
    }

//-----------------------------------------------------------------------------
// CSettingsTestModule::DestroyiSettingsL
//-----------------------------------------------------------------------------
TInt CSettingsTestModule::DestroySettingsL(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;

    delete iSettings;
    iSettings = NULL;
    RadioEngineUtils::Release();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::ResolveDriveL()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::ResolveDriveL(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;
    TFileName resourceFileName;
    resourceFileName.Append(KRadioSettingsResourceFile);
    iSettings->ResolveDriveL(resourceFileName, KDC_RESOURCE_FILES_DIR);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::IsRegionAllowed()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::IsRegionAllowed(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;
    TRadioRegion regionId(iSettings->EngineSettings().RegionId());
    TUint currentAllowedStatus(iSettings->IsRegionAllowed(regionId));
    TInt err(KErrNone);
    INFO_1( "Failed: currentAllowedStatus=%i", currentAllowedStatus );
    return err;

    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::IsFirstTimeStart()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::IsFirstTimeStart(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TBool expectedFirstTimeStartStatus(EFalse);
    // read parameters
    if (aItem.GetNextInt(expectedFirstTimeStartStatus))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: expectedFirstTimeStartStatus." );
        }
    TBool firstTimeStartStatus(
            iSettings->ApplicationSettings().IsFirstTimeStart());
    if (expectedFirstTimeStartStatus != firstTimeStartStatus)
        {
        INFO_2( "Failed: firstTimeStartStatus=%i, expectedFirstTimeStartStatus=%i.", firstTimeStartStatus, expectedFirstTimeStartStatus );
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::SetFirstTimeStart()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::SetFirstTimeStartPerformed(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TBool firstTimeStartPerformed(EFalse);
    // read parameters
    if (aItem.GetNextInt(firstTimeStartPerformed))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: firstTimeStartPerformed." );
        }

    iSettings->ApplicationSettings().SetFirstTimeStartPerformed(
            firstTimeStartPerformed);
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::SetUiFlags()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::SetUiFlags(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TUint uiFlags(0);
    TInt err(KErrNone);

    // read parameters
    if (aItem.GetNextInt(uiFlags))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: uiFlags." );
        }
    err = iSettings->ApplicationSettings().SetUiFlags(uiFlags);
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::UiFlags()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::UiFlags(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TUint expectedUiFlags(0);
    TUint currentUiFlags(0);
    TInt err(KErrNone);

    // read parameters
    if (aItem.GetNextInt(expectedUiFlags))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: expectedUiFlags." );
        }
    currentUiFlags = iSettings->ApplicationSettings().UiFlags();
    if (expectedUiFlags != currentUiFlags)
        {
        INFO_2( "Failed: currentUiFlags=%i, expectedUiFlags=%i.", currentUiFlags, expectedUiFlags );
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::HeadsetVolume()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::HeadsetVolume(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TUint expectedVolume(0);
    TUint currentVolume(0);
    TInt err(KErrNone);

    // read parameters
    if (aItem.GetNextInt(expectedVolume))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: expectedVolume." );
        }
    currentVolume = iSettings->EngineSettings().HeadsetVolume();
    if (expectedVolume != currentVolume)
        {
        INFO_2( "Failed: currentVolume=%i, expectedVolume=%i.", currentVolume, expectedVolume );
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::SetHeadsetVolume()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::SetHeadsetVolume(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TUint volume(0);
    TInt err(KErrNone);
    // read parameters
    if (aItem.GetNextInt(volume))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: volume." );
        }
    err = iSettings->RadioSetter().SetHeadsetVolume(volume);
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::SpeakerVolume()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::SpeakerVolume(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TUint expectedVolume(0);
    TUint currentVolume(0);
    TInt err(KErrNone);

    // read parameters
    if (aItem.GetNextInt(expectedVolume))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: expectedVolume." );
        }
    currentVolume = iSettings->EngineSettings().SpeakerVolume();
    if (expectedVolume != currentVolume)
        {
        INFO_2( "Failed: currentVolume=%i, expectedVolume=%i.", currentVolume, expectedVolume );
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::SetSpeakerVolume()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::SetSpeakerVolume(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TUint volume(0);
    TInt err(KErrNone);

    // read parameters
    if (aItem.GetNextInt(volume))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: volume." );
        }
    err = iSettings->RadioSetter().SetSpeakerVolume(volume);
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::Volume()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::Volume(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TUint expectedVolume(0);
    TUint currentVolume(0);
    TInt err(KErrNone);

    // read parameters
    if (aItem.GetNextInt(expectedVolume))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: expectedVolume." );
        }
    currentVolume = iSettings->EngineSettings().Volume();
    if (expectedVolume != currentVolume)
        {
        INFO_2( "Failed: currentVolume=%i, expectedVolume=%i.", currentVolume, expectedVolume );
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::SetVolume()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::SetVolume(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TUint volume(0);
    TInt err(KErrNone);

    // read parameters
    if (aItem.GetNextInt(volume))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: volume." );
        }
    err = iSettings->RadioSetter().SetVolume(volume);
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::OutputMode()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::OutputMode(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TUint expectedMode(0);
    TUint currentMode(0);
    TInt err(KErrNone);

    // read parameters
    if (aItem.GetNextInt(expectedMode))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: expectedMode." );
        }
    currentMode = iSettings->EngineSettings().OutputMode();
    if (expectedMode != currentMode)
        {
        INFO_2( "Failed: currentMode=%i, expectedMode=%i.", currentMode, expectedMode );
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::SetOutputMode()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::SetOutputMode(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TUint mode(0);
    TInt err(KErrNone);

    // read parameters
    if (aItem.GetNextInt(mode))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: mode." );
        }
    err = iSettings->RadioSetter().SetOutputMode(mode);
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::AudioRoute()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::AudioRoute(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TUint expectedRoute(0);
    TUint currentRoute(0);
    TInt err(KErrNone);

    // read parameters
    if (aItem.GetNextInt(expectedRoute))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: expectedRoute." );
        }
    currentRoute = iSettings->EngineSettings().AudioRoute();
    if (expectedRoute != currentRoute)
        {
        INFO_2( "Failed: currentMode=%i, expectedRoute=%i.", currentRoute, expectedRoute );
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::SetAudioRoute()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::SetAudioRoute(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TUint route(0);
    TInt err(KErrNone);

    // read parameters
    if (aItem.GetNextInt(route))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: route." );
        }
    err = iSettings->RadioSetter().SetAudioRoute(route);
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::IsHeadsetVolMuted()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::IsHeadsetVolMuted(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TUint expectedMuteStatus(0);
    TUint currentMuteStatus(0);

    // read parameters
    if (aItem.GetNextInt(expectedMuteStatus))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: expectedMuteStatus." );
        }
    currentMuteStatus = iSettings->EngineSettings().IsHeadsetVolMuted();
    if (currentMuteStatus != expectedMuteStatus)
        {
        INFO_2( "Failed: currentMuteStatus=%i, expectedMuteStatus=%i.", currentMuteStatus, expectedMuteStatus );
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::SetHeadsetVolMuted()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::SetHeadsetVolMuted(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TUint muteStatus(0);
    TInt err(KErrNone);

    // read parameters
    if (aItem.GetNextInt(muteStatus))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: muteStatus." );
        }
    err = iSettings->RadioSetter().SetHeadsetVolMuted(muteStatus);
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::IsSpeakerVolMuted()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::IsSpeakerVolMuted(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TUint expectedMuteStatus(0);
    TUint currentMuteStatus(0);

    // read parameters
    if (aItem.GetNextInt(expectedMuteStatus))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: expectedMuteStatus." );
        }
    currentMuteStatus = iSettings->EngineSettings().IsSpeakerVolMuted();
    if (currentMuteStatus != expectedMuteStatus)
        {
        INFO_2( "Failed: currentMuteStatus=%i, expectedMuteStatus=%i.", currentMuteStatus, expectedMuteStatus );
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::SetSpeakerVolMuted()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::SetSpeakerVolMuted(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TUint muteStatus(0);
    TInt err(KErrNone);

    // read parameters
    if (aItem.GetNextInt(muteStatus))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: muteStatus." );
        }
    err = iSettings->RadioSetter().SetSpeakerVolMuted(muteStatus);
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::IsVolMuted()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::IsVolMuted(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TUint expectedMuteStatus(0);
    TUint currentMuteStatus(0);

    // read parameters
    if (aItem.GetNextInt(expectedMuteStatus))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: expectedMuteStatus." );
        }
    currentMuteStatus = iSettings->EngineSettings().IsVolMuted();
    if (currentMuteStatus != expectedMuteStatus)
        {
        INFO_2( "Failed: currentMuteStatus=%i, expectedMuteStatus=%i.", currentMuteStatus, expectedMuteStatus );
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::SetVolMuted()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::SetVolMuted(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TUint muteStatus(0);
    TInt err(KErrNone);

    // read parameters
    if (aItem.GetNextInt(muteStatus))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: muteStatus." );
        }
    err = iSettings->RadioSetter().SetVolMuted(muteStatus);
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::IsPowerOn()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::IsPowerOn(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TUint expectedPowerStatus(0);
    TUint currentPowerStatus(0);

    // read parameters
    if (aItem.GetNextInt(expectedPowerStatus))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: expectedPowerStatus." );
        }
    currentPowerStatus = iSettings->EngineSettings().IsPowerOn();
    if (currentPowerStatus != expectedPowerStatus)
        {
        INFO_2( "Failed: currentPowerStatus=%i, expectedPowerStatus=%i.", currentPowerStatus, expectedPowerStatus );
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::SetPowerOn()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::SetPowerOn(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TUint powerStatus(0);
    TInt err(KErrNone);

    // read parameters
    if (aItem.GetNextInt(powerStatus))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: powerStatus." );
        }
    err = iSettings->RadioSetter().SetPowerOn(powerStatus);
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::TunedFrequency()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::TunedFrequency(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TUint expectedTunedFrequency(0);
    TUint currentTunedFrequency(0);

    // read parameters
    if (aItem.GetNextInt(expectedTunedFrequency))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: expectedTunedFrequency." );
        }
    currentTunedFrequency = iSettings->EngineSettings().TunedFrequency();
    if (currentTunedFrequency != expectedTunedFrequency)
        {
        INFO_2( "Failed: currentTunedFrequency=%i, expectedTunedFrequency=%i.", currentTunedFrequency, expectedTunedFrequency );
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::SetTunedFrequency()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::SetTunedFrequency(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TUint tunedFrequency(0);
    TInt err(KErrNone);

    // read parameters
    if (aItem.GetNextInt(tunedFrequency))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: tunedFrequency." );
        }
    err = iSettings->RadioSetter().SetTunedFrequency(tunedFrequency);
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::DefaultMinVolumeLevel()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::DefaultMinVolumeLevel(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TUint currentDefaultMinVolumeLevel(
            iSettings->EngineSettings().DefaultMinVolumeLevel());
    INFO_1( "OK currentDefaultMinVolumeLevel=%i", currentDefaultMinVolumeLevel );
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::CountRegions()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::CountRegions(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TUint currentRegionCount(iSettings->EngineSettings().CountRegions());
    INFO_1( "OK currentRegionCount=%i", currentRegionCount );
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::Region()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::Region(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TInt regionIndex(iSettings->EngineSettings().RegionId());
    //CRadioRegion& radioRegion( iSettings->EngineSettings().RegionIndexForId( regionIndex ) );
    // 17.5.2010: Region handling has to be reimplemented anyways, so don't make any detailed tets
    // Above won't work because called function RegionIndexForId() is private one.

    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::RegionId()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::RegionId(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TInt expectedRegionIndex(0);
    TInt currentRegionIndex(0);
    TInt err(KErrNone);

    // read parameters
    if (aItem.GetNextInt(expectedRegionIndex))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: expectedRegionIndex." );
        }
    currentRegionIndex = iSettings->EngineSettings().RegionId();
    if (currentRegionIndex != expectedRegionIndex)
        {
        INFO_2( "Failed: currentRegionIndex=%i, expectedRegionIndex=%i.", currentRegionIndex, expectedRegionIndex );
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::SetRegionId()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::SetRegionId(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TInt regionId(0);
    TInt err(KErrNone);

    // read parameters
    if (aItem.GetNextInt(regionId))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: regionId." );
        }
    err = iSettings->RadioSetter().SetRegionId(regionId);
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::FrequencyStepSize()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::FrequencyStepSize(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TUint32 currentFrequencyStepSize(
            iSettings->EngineSettings().FrequencyStepSize());
    INFO_1( "OK currentFrequencyStepSize=%i", currentFrequencyStepSize );
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::MaxFrequency()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::MaxFrequency(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TUint32 currentMaxFrequency(iSettings->EngineSettings().MaxFrequency());
    INFO_1( "OK currentMaxFrequency=%i", currentMaxFrequency );
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::MinFrequency()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::MinFrequency(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TUint32 currentMinFrequency(iSettings->EngineSettings().MinFrequency());
    INFO_1( "OK currentMinFrequency=%i", currentMinFrequency );
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::DecimalCount()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::DecimalCount(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TInt currentDecimalCount(iSettings->EngineSettings().DecimalCount());
    INFO_1( "OK currentDecimalCount=%i", currentDecimalCount );
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::DefaultRegion()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::DefaultRegion(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TRadioRegion defaultRegion(iSettings->EngineSettings().DefaultRegion());
    INFO_1( "OK defaultRegion=%i", defaultRegion );
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::NetworkId()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::NetworkId(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TPtrC expectedNetworkId;
    if (aItem.GetNextString(expectedNetworkId))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: expectedNetworkId." );
        }
    INFO_1( "expectedNetworkId=%S.", &expectedNetworkId );
    TPtrC actualNetworkId(iSettings->EngineSettings().NetworkId());
    if (!actualNetworkId.Compare(expectedNetworkId))
        {
        INFO_2( "Failed: actualNetworkId=%S, expectedNetworkId=%S.", &actualNetworkId, &expectedNetworkId );
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::CountryCode()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::CountryCode(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TPtrC expectedCountryCode;
    if (aItem.GetNextString(expectedCountryCode))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: expectedCountryCode." );
        }
    INFO_1( "expectedCountryCode=%S.", &expectedCountryCode );
    TPtrC actualCountryCode(iSettings->EngineSettings().CountryCode());
    if (!actualCountryCode.Compare(expectedCountryCode))
        {
        INFO_2( "Failed: actualCountryCode=%S, expectedCountryCode=%S.", &actualCountryCode, &expectedCountryCode );
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::SetNetworkId()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::SetNetworkId(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TPtrC networkId;
    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);
    if (aItem.GetNextString(networkId))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: networkId." );
        }
    err = iSettings->RadioSetter().SetNetworkId(networkId);
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::SetCountryCode()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::SetCountryCode(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TPtrC countryCode;
    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);
    if (aItem.GetNextString(countryCode))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: countryCode." );
        }
    err = iSettings->RadioSetter().SetCountryCode(countryCode);
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::ReadConfigurableKeysL()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::ReadConfigurableKeysL(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TInt regionAllowed(EFalse);
    CRepository* cr(CRepository::NewL(KRadioCRUid));
    if (cr)
        {
        cr->Get(KRadioCRRegionAllowedJapan, regionAllowed);
        delete cr;
        }
    INFO_1( "KRadioCRRegionAllowedJapan status = %i", regionAllowed );
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::RdsAfSearchSettingChangedL()
// Returns: None.
// -----------------------------------------------------------------------------
void CSettingsTestModule::RdsAfSearchSettingChangedL(TBool /*aEnabled*/)
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::RegionSettingChangedL()
// Returns: None.
// -----------------------------------------------------------------------------
void CSettingsTestModule::RegionSettingChangedL(TInt /*aRegion*/)
    {
    FUNC_LOG;
    iRegionSettingChangeNotified = ETrue;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::tstSetRegionIdL()
// Returns: errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::tstSetRegionIdL(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TInt regionId(0);
    TInt err(KErrNone);

    // read parameters
    if (aItem.GetNextInt(regionId))
        {
        err = KErrBadTestParameter;
        ERROR( err, "Missing Parameter: regionId." );
        }
    CRepository* cr(CRepository::NewLC(KRadioCRUid));
    User::LeaveIfError(cr->Set(KRadioCRCurrentRegion, regionId));
    CleanupStack::Pop(cr);
    delete cr;
    return err;
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::tstResetStartCount()
// Returns: errors.
// -----------------------------------------------------------------------------
TInt CSettingsTestModule::tstResetStartCount(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;
    TInt countResetValue(0);
    TInt err(KErrNone);

    CRepository* cr(CRepository::NewLC(KRadioCRUid));
    User::LeaveIfError(cr->Set(KRadioCRLaunchCount, countResetValue));
    CleanupStack::Pop(cr);
    delete cr;
    return err;
    }
//  End of File
