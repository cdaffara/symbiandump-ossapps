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
 * Description: STIF testclass declaration
 *
 */

#ifndef SettingsTestMODULE_H
#define SettingsTestMODULE_H

// System includes
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
// User includes
#include "cradiosettings.h"
#include "mradiosettingsobserver.h"
#include "trace.h"

// CONSTANTS
const TInt KErrBadTestParameter = -1000; // Error on configuration file
const TInt KErrEventPending = -2000; // Timeout and not callback 
const TInt KErrCallbackErrorCode = -2001; // Callback return error code
const TInt KErrUnexpectedValue = -2002; // Unexpected value at setting    
const TInt KExpectedFrequencyChange = -2003; // Is not finding more than one station
const TInt KErrTimeoutController = -2007; // Unexpected notification

// MACROS
#define TEST_CLASS_VERSION_MAJOR 1
#define TEST_CLASS_VERSION_MINOR 1
#define TEST_CLASS_VERSION_BUILD 1
// Logging path
_LIT( KSettingsTestModuleLogPath, "\\logs\\testframework\\SettingsTestModule\\" );
// Log file
_LIT( KSettingsTestModuleLogFile, "SettingsTestModule.txt" );
_LIT( KSettingsTestModuleLogFileWithTitle, "SettingsTestModule_[%S].txt" );

// CLASS DECLARATION

/**
 *  CSettingsTestModule test class for STIF Test Framework TestScripter.
 *  ?other_description_lines
 *
 *  @lib SettingsTestModule.dll
 *  @since S60 10.1
 */
NONSHARABLE_CLASS(CSettingsTestModule) : public CScriptBase,
        public MRadioSettingsObserver
    {
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CSettingsTestModule* NewL(CTestModuleIf& aTestModuleIf);

    /**
     * Destructor.
     */
    virtual ~CSettingsTestModule();

public:
    // Functions from base classes

    /**
     * From CScriptBase Runs a script line.
     * @since ?Series60_version
     * @param aItem Script line containing method name and parameters
     * @return Symbian OS error code
     */
    virtual TInt RunMethodL(CStifItemParser& aItem);

    // MRadioSettingsObserver

    /**
     * From MRadioSettingsObserver.
     * Notifies of the change in RDS alternate frequency setting
     *
     * @param aEnabled ETrue if AF search is enabled, otherwise EFalse
     */
    virtual void RdsAfSearchSettingChangedL(TBool aEnabled);

    /**
     * From MRadioSettingsObserver.
     * Notifies of the change in region setting when it is changed
     * outside of the particular radio settings instance
     *
     * @param aRegion The new region ID
     */
    virtual void RegionSettingChangedL(TInt aRegion);

protected:
    // New functions

    // Test case functions
    TInt CreateSettingsL(CStifItemParser& aItem);
    TInt DestroySettingsL(CStifItemParser& aItem);

    TInt IsRegionAllowed(CStifItemParser& aItem);

    TInt ResolveDriveL(CStifItemParser& aItem);

    // Application Settings functions

    TInt SetFirstTimeStartPerformed(CStifItemParser& aItem);
    TInt IsFirstTimeStart(CStifItemParser& aItem);

    TInt SetUiFlags(CStifItemParser& aItem);

    TInt UiFlags(CStifItemParser& aItem);

    // Settings functions

    TInt HeadsetVolume(CStifItemParser& aItem);
    TInt SpeakerVolume(CStifItemParser& aItem);
    TInt Volume(CStifItemParser& aItem);
    TInt OutputMode(CStifItemParser& aItem);
    TInt AudioRoute(CStifItemParser& aItem);
    TInt IsHeadsetVolMuted(CStifItemParser& aItem);
    TInt IsSpeakerVolMuted(CStifItemParser& aItem);
    TInt IsVolMuted(CStifItemParser& aItem);
    TInt IsPowerOn(CStifItemParser& aItem);
    TInt TunedFrequency(CStifItemParser& aItem);
    TInt DefaultMinVolumeLevel(CStifItemParser& aItem);
    TInt CountRegions(CStifItemParser& aItem);
    TInt Region(CStifItemParser& aItem);
    TInt RegionId(CStifItemParser& aItem);
    TInt FrequencyStepSize(CStifItemParser& aItem);
    TInt MaxFrequency(CStifItemParser& aItem);
    TInt MinFrequency(CStifItemParser& aItem);
    TInt DecimalCount(CStifItemParser& aItem);
    TInt DefaultRegion(CStifItemParser& aItem);
    TInt NetworkId(CStifItemParser& aItem);
    TInt CountryCode(CStifItemParser& aItem);
    TInt OperatorMusicStoreURL(CStifItemParser& aItem);
    TInt OperatorMusicStoreName(CStifItemParser& aItem);

    // Setter functions

    TInt SetHeadsetVolume(CStifItemParser& aItem);
    TInt SetSpeakerVolume(CStifItemParser& aItem);
    TInt SetVolume(CStifItemParser& aItem);
    TInt SetOutputMode(CStifItemParser& aItem);
    TInt SetAudioRoute(CStifItemParser& aItem);
    TInt SetHeadsetVolMuted(CStifItemParser& aItem);
    TInt SetSpeakerVolMuted(CStifItemParser& aItem);
    TInt SetVolMuted(CStifItemParser& aItem);
    TInt SetPowerOn(CStifItemParser& aItem);
    TInt SetTunedFrequency(CStifItemParser& aItem);
    TInt SetRegionId(CStifItemParser& aItem);
    TInt SetNetworkId(CStifItemParser& aItem);
    TInt SetCountryCode(CStifItemParser& aItem);
    TInt SetOperatorMusicStoreURL(CStifItemParser& aItem);
    TInt SetOperatorMusicStoreName(CStifItemParser& aItem);

    // Tst functions
    TInt ReadConfigurableKeysL(CStifItemParser& aItem);
    TInt tstSetRegionIdL(CStifItemParser& aItem);
    TInt tstIsRegionSettingChangeNotified(CStifItemParser& aItem);
    TInt tstResetStartCount(CStifItemParser& aItem);

private:

    /**
     * C++ default constructor.
     */
    CSettingsTestModule(CTestModuleIf& aTestModuleIf);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    // Prohibit copy constructor if not deriving from CBase.
    // ?classname( const ?classname& );
    // Prohibit assigment operator if not deriving from CBase.
    // ?classname& operator=( const ?classname& );

    /**
     * Frees all resources allocated from test methods.
     * @since ?Series60_version
     */
    void Delete();

    /**
     * Test methods are listed below. 
     */

    /**
     * Example test method.
     * @since ?Series60_version
     * @param aItem Script line containing parameters.
     * @return Symbian OS error code.
     */
    virtual TInt ExampleL(CStifItemParser& aItem);

    /**
     * Method used to log version of test class
     */
    void SendTestClassVersion();

private:
    // Data

    /** Radio Settings */
    CRadioSettings* iSettings;

    TBool iRegionSettingChangeNotified;
    };

#endif      // SettingsTestMODULE_H
// End of File
