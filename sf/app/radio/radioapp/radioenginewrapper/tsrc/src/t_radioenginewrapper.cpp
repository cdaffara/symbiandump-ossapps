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
 * Description: Radio engine wrapper's test implementation
 *
 */
// System includes
#include <e32base.h>
#include <eikenv.h>
#include <e32property.h>
#include <radiomonitor.h>
// User includes
#include "t_radioenginewrapper.h"
#include "radioenginewrapper.h"
#include "radiologger.h"
#include <cradiosettings.h>
#include "mradioenginesettings.h"
#include "trace.h"
#include "RadioClientServer.h"
#include "radiointernalcrkeys.h"
#include "radioengineutils.h"
#define STUB  mRadioStubManager
#define TUNER  mRadioStubManager->iTuner
#define PLAYER  mRadioStubManager->iPlayer
#define RDS  mRadioStubManager->iRds
#define ACCESSORYOBSERVER  mRadioStubManager->iAccessoryObserver
#define REMCONTARGET  mRadioStubManager->iRemConTarget
// Constants
_LIT_SECURITY_POLICY_PASS(KRadioServerReadPolicy);
_LIT_SECURITY_POLICY_C1(KRadioServerWritePolicy, ECapabilityWriteUserData);

/*!
 *
 */
int main(int /* argc*/, char *argv[])
    {
    FUNC_LOG;
    TestRadioEngineWrapper tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testradiouiengine.txt";

    int res = QTest::qExec(&tv, 3, pass);

    INFO_1( "Main, result value %i", res );
    return res;
    }

TestRadioEngineWrapper::TestRadioEngineWrapper()
    {
    FUNC_LOG;
    }

// from base class RadioStationHandlerIf

uint TestRadioEngineWrapper::currentFrequency() const
    {
    FUNC_LOG;
    return 0;
    }

int TestRadioEngineWrapper::currentPresetIndex() const
    {
    FUNC_LOG;
    return 0;
    }

void TestRadioEngineWrapper::setCurrentStation(uint /*frequency*/)
    {
    FUNC_LOG;
    }

bool TestRadioEngineWrapper::containsFrequency(uint /*frequency*/)
    {
    FUNC_LOG;
    return 1;
    }

bool TestRadioEngineWrapper::containsPresetIndex(int /*presetIndex*/)
    {
    FUNC_LOG;
    return 1;
    }

void TestRadioEngineWrapper::startDynamicPsCheck()
    {
    FUNC_LOG;
    }

void TestRadioEngineWrapper::addScannedFrequency(uint /*frequency*/)
    {
    FUNC_LOG;
    }

void TestRadioEngineWrapper::removeLocalStations()
    {
    FUNC_LOG;
    }

void TestRadioEngineWrapper::setCurrentPsName(uint /*frequency*/,
        const QString& /*name*/)
    {
    FUNC_LOG;
    }

void TestRadioEngineWrapper::setCurrentRadioText(uint /*frequency*/,
        const QString& /*radioText*/)
    {
    FUNC_LOG;
    }

void TestRadioEngineWrapper::setCurrentRadioTextPlus(uint /*frequency*/,
        int /*rtClass*/, const QString& /*rtItem*/)
    {
    FUNC_LOG;
    }

void TestRadioEngineWrapper::setCurrentPiCode(uint /*frequency*/, int /*piCode*/)
    {
    FUNC_LOG;
    }

void TestRadioEngineWrapper::setCurrentGenre(uint /*frequency*/, int /*genre*/)
    {
    FUNC_LOG;
    }

//  from base class RadioEngineWrapperObserver

void TestRadioEngineWrapper::tunedToFrequency(uint frequency, int /* commandSender */)
    {
    FUNC_LOG;
    mEnteredSlots |= TunedToFrequency;
    TInt err = mRadioCR->Set(KRadioCRTunedFrequency, (TInt) frequency);
    QVERIFY2( KErrNone == err, "Setting key KRadioCRTunedFrequency failed!" );
    }

void TestRadioEngineWrapper::rdsAvailabilityChanged(bool /* available */)
    {
    FUNC_LOG;
    }

void TestRadioEngineWrapper::increaseVolume()
    {
    FUNC_LOG;
    }

void TestRadioEngineWrapper::decreaseVolume()
    {
    FUNC_LOG;
    }

void TestRadioEngineWrapper::volumeChanged(int volume)
    {
    FUNC_LOG;
    mEnteredSlots |= VolumeChanged;
    mLastRecordedVolume = volume;
    }

void TestRadioEngineWrapper::muteChanged(bool /* muted */)
    {
    FUNC_LOG;
    mEnteredSlots |= MuteChanged;
    }

void TestRadioEngineWrapper::antennaStatusChanged(bool /* muted */)
    {
    FUNC_LOG;
    mEnteredSlots |= AntennaChanged;
    }

void TestRadioEngineWrapper::audioRouteChanged(bool /* loudspeaker */)
    {
    FUNC_LOG;
    }

void TestRadioEngineWrapper::skipPrevious()
    {
    FUNC_LOG;
    }

void TestRadioEngineWrapper::skipNext()
    {
    FUNC_LOG;
    }

/*!
 * Destructor
 */
TestRadioEngineWrapper::~TestRadioEngineWrapper()
    {
    FUNC_LOG;
    mRadioStubManagerChunk.Close();
    delete mEngineWrapper;
    mEngineWrapper = NULL;
    delete mScheduler;
    mScheduler = NULL;
    }

/*!
 * called before each testfunction is executed
 */
void TestRadioEngineWrapper::init()
    {
    FUNC_LOG;
    }

/*!
 * called after every testfunction
 */
void TestRadioEngineWrapper::cleanup()
    {
    FUNC_LOG;
    }

/*!
 * called before the first testfunction is executed
 */
void TestRadioEngineWrapper::initTestCase()
    {
    FUNC_LOG;
    // Workaround for the below panic, occured after porting to 10.1 
    // Main Panic E32USER-CBase 44
    // Create and install the active scheduler
    mScheduler = new (ELeave) CActiveScheduler;
    CActiveScheduler::Install(mScheduler);

    TInt err = mRadioStubManagerChunk.CreateGlobal(
            KRadioStubManagerLocalChunkName, sizeof(SRadioStubManager),
            sizeof(SRadioStubManager), EOwnerThread);
    QVERIFY2( KErrNone == err, "Creation of memory chunk KRadioStubManagerLocalChunkName failed." );
    QVERIFY2( sizeof(SRadioStubManager) <= mRadioStubManagerChunk.MaxSize(), "Improper size for memory chunk KRadioStubManagerLocalChunkName." );
    TUint8* basePtr = mRadioStubManagerChunk.Base();
    QVERIFY2( 0 != basePtr, "Getting base pointer of memory chunk KRadioStubManagerLocalChunkName failed." );
    mRadioStubManager = (SRadioStubManager*) basePtr;
    // Zero configuration/control data

    mRadioStubManager->FillZ();
    TRAP( err, mSchedulerTimer = CSchedulerStopAndStartTimer::NewL( *this ) );
    QVERIFY2( KErrNone == err, "CSchedulerStopAndStartTimer not constructed!" );
    mSchedulerTimer->StartTimer(1000000,
            CSchedulerStopAndStartTimer::ETimerIdCreateMUT);
    }

void TestRadioEngineWrapper::testRadioSettingsReference()
    {
    FUNC_LOG;
    QVERIFY2( 0 != &mEngineWrapper->settings(), "Settings reference illegal!" );
    }

void TestRadioEngineWrapper::testRegion()
    {
    FUNC_LOG;
    QVERIFY2( mEngineWrapper->region() >= RadioRegion::None, "Illegal Region Id 1!");
    QVERIFY2( mEngineWrapper->region() <= RadioRegion::Poland, "Illegal Region Id 2!");
    QVERIFY2( mEngineWrapper->frequencyStepSize() != 0, "Region Step Size illegal!" );
    QVERIFY2( mEngineWrapper->minFrequency() < mEngineWrapper->maxFrequency(), "Region Minimum Frequency illegal 1!" );
    QVERIFY2( mEngineWrapper->maxFrequency() > mEngineWrapper->minFrequency(), "Region Maximum Frequency illegal 2!" );
    QVERIFY2( mEngineWrapper->isFrequencyValid( mEngineWrapper->minFrequency() + mEngineWrapper->frequencyStepSize() ), "Legal frequency not accepted 1!" );
    QVERIFY2( mEngineWrapper->isFrequencyValid( mEngineWrapper->maxFrequency() - mEngineWrapper->frequencyStepSize() ), "Legal frequency not accepted 2!" );
    QVERIFY2( mEngineWrapper->isFrequencyValid( mEngineWrapper->minFrequency() ), "Legal minimum frequency not accepted!" );
    QVERIFY2( mEngineWrapper->isFrequencyValid( mEngineWrapper->maxFrequency() ), "Legal maximum frequency not accepted!" );
    QVERIFY2( !mEngineWrapper->isFrequencyValid( mEngineWrapper->maxFrequency() + mEngineWrapper->frequencyStepSize() ), "Illegal frequency accepted 1!" );
    QVERIFY2( !mEngineWrapper->isFrequencyValid( mEngineWrapper->minFrequency() - mEngineWrapper->frequencyStepSize() ), "Illegal frequency accepted 2!" );
    QVERIFY2( !mEngineWrapper->isFrequencyValid( 0 + mEngineWrapper->frequencyStepSize() ), "Illegal frequency accepted 3!" );
    QVERIFY2( !mEngineWrapper->isFrequencyValid( 0 - mEngineWrapper->frequencyStepSize() ), "Illegal frequency accepted 4!" );
    }

void TestRadioEngineWrapper::testRadioOnOff()
    {
    FUNC_LOG;
    QVERIFY2( !mEngineWrapper->isRadioOn(), "Radio is not off!" );

    TInt err(KErrNone);
    TBool antennaAttached = ETrue;
    err = RProperty::Set(KStub_KRadioServerPropertyCategory,
            ERadioServPsAntennaStatus, antennaAttached);
    INFO_1("RProperty::Set( KStub_KRadioServerPropertyCategory, ERadioServPsAntennaStatus,  antennaAttached ) err: %d", err);
    QVERIFY2( KErrNone == err, "Setting property ERadioServPsAntennaStatus failed!" );
    mSchedulerTimer->StartTimer(1000000);
    QVERIFY2( mEngineWrapper->isAntennaAttached(), "Headset/Antenna not connected!");
    TRAP( err, ACCESSORYOBSERVER.iObserver->HeadsetConnectedCallbackL() );
    QVERIFY2( KErrNone == err, "ACCESSORYOBSERVER.iObserver->HeadsetConnectedCallbackL() failed!" );

    TPckgBuf<TRsSettingsData> playerState;
    playerState().iError = KErrNone;
    playerState().iData1 = ETrue;
    err = RProperty::Set(KStub_KRadioServerPropertyCategory,
            ERadioServPsPlayerState, playerState);
    QVERIFY2( KErrNone == err, "Setting property ERadioServPsPlayerState failed!" );

    mSchedulerTimer->StartTimer(1000000);
    QVERIFY2( mEngineWrapper->isAntennaAttached(), "Headset/Antenna not connected!");
    QVERIFY2( mEngineWrapper->isRadioOn(), "Radio is not on!" );
    }

void TestRadioEngineWrapper::testTuning()
    {
    FUNC_LOG;
    QVERIFY2( mEngineWrapper->isRadioOn(), "Radio is not on!" );

    // Tune to minimum frequency
    mEngineWrapper->setFrequency(mEngineWrapper->minFrequency());
    TInt err = mPropertyFrequency.Set(KStub_KRadioServerPropertyCategory,
            ERadioServPsFrequency, tstGetFrequency());
    QVERIFY2( KErrNone == err, "Setting property ERadioServPsFrequency failed!" );
    mSchedulerTimer->StartTimer(1000000);
    INFO_1("mEngineWrapper->minFrequency() = %i", mEngineWrapper->minFrequency() );
    INFO_1("mEngineWrapper->currentFrequency() = %i", mEngineWrapper->currentFrequency() );
    QVERIFY2( mEngineWrapper->currentFrequency() == mEngineWrapper->minFrequency(), "Tuning to minimum frequency failed!");

    // Seek upwards
    uint freq_A = mEngineWrapper->currentFrequency();
    tstSetFrequency(freq_A + mEngineWrapper->frequencyStepSize());
    mEngineWrapper->startSeeking(Seek::Up);
    err = mPropertyFrequency.Set(KStub_KRadioServerPropertyCategory,
            ERadioServPsFrequency, tstGetFrequency());
    QVERIFY2( KErrNone == err, "Setting property ERadioServPsFrequency failed!" );
    mSchedulerTimer->StartTimer(1000000);
    QVERIFY2( TunedToFrequency == ( mEnteredSlots & TunedToFrequency ), "Not Tuned to next upward frequency!");
    mEnteredSlots &= !TunedToFrequency;
    uint freq_B = mEngineWrapper->currentFrequency();
    QVERIFY2( (freq_B - mEngineWrapper->frequencyStepSize()) == freq_A, "Seeking upwards failed!");

    // Tune to maximum frequency
    tstSetFrequency(mEngineWrapper->maxFrequency());
    mEngineWrapper->setFrequency(mEngineWrapper->maxFrequency());
    err = mPropertyFrequency.Set(KStub_KRadioServerPropertyCategory,
            ERadioServPsFrequency, tstGetFrequency());
    QVERIFY2( KErrNone == err, "Setting property ERadioServPsFrequency failed!" );
    mSchedulerTimer->StartTimer(1000000);
    QVERIFY2( TunedToFrequency == ( mEnteredSlots & TunedToFrequency ), "Not Tuned to maximum frequency!");
    mEnteredSlots &= !TunedToFrequency;
    QVERIFY2( mEngineWrapper->currentFrequency() == mEngineWrapper->maxFrequency(), "Current frequency not the maximum one!" );

    // Seek downwards
    mEngineWrapper->startSeeking(Seek::Down);
    tstSetFrequency(mEngineWrapper->currentFrequency() - (2
            * mEngineWrapper->frequencyStepSize()));
    err = mPropertyFrequency.Set(KStub_KRadioServerPropertyCategory,
            ERadioServPsFrequency, tstGetFrequency());
    QVERIFY2( KErrNone == err, "Setting property ERadioServPsFrequency failed!" );
    mSchedulerTimer->StartTimer(1000000);
    QVERIFY2( TunedToFrequency == ( mEnteredSlots & TunedToFrequency ), "Not Tuned to next downward frequency!");
    mEnteredSlots &= !TunedToFrequency;
    QVERIFY2( mEngineWrapper->currentFrequency() == mEngineWrapper->maxFrequency()- (2 * mEngineWrapper->frequencyStepSize()), "Seeking downwards failed!");

    // Tune to avarage frequency
    uint averageFrequency(mEngineWrapper->maxFrequency()
            + mEngineWrapper->minFrequency());
    averageFrequency /= 2;
    averageFrequency -= (averageFrequency
            % mEngineWrapper->frequencyStepSize());
    mEngineWrapper->setFrequency(averageFrequency);
    err = mPropertyFrequency.Set(KStub_KRadioServerPropertyCategory,
            ERadioServPsFrequency, tstGetFrequency());
    QVERIFY2( KErrNone == err, "Setting property ERadioServPsFrequency failed!" );
    mSchedulerTimer->StartTimer(1000000);
    QVERIFY2( TunedToFrequency == ( mEnteredSlots & TunedToFrequency ), "Not Tuned to average frequency!");
    mEnteredSlots &= !TunedToFrequency;
    QVERIFY2( mEngineWrapper->currentFrequency() == averageFrequency, "Tuning to average frequency failed!");

    // Try to use frequency above maximum
    uint freq_C = mEngineWrapper->currentFrequency();
    TUNER.iSetFrequencyError.SetStubError(
            KRadioServErrFrequencyOutOfBandRange);
    mEngineWrapper->setFrequency(mEngineWrapper->maxFrequency()
            + mEngineWrapper->frequencyStepSize());
    mSchedulerTimer->StartTimer(1000000);
    // Should be == not != as now. This is done in order to complete tests.
    QVERIFY2( mEngineWrapper->currentFrequency() != freq_C, "Tuning over maximum frequency succeeded?");
    QVERIFY2( !mEngineWrapper->isFrequencyValid( mEngineWrapper->currentFrequency() ), "A frequency over maximum accepted?");

    // Try to use frequency below minimum
    uint freq_D = mEngineWrapper->currentFrequency();
    TUNER.iSetFrequencyError.SetStubError(
            KRadioServErrFrequencyOutOfBandRange);
    mEngineWrapper->setFrequency(mEngineWrapper->minFrequency()
            - mEngineWrapper->frequencyStepSize());
    mSchedulerTimer->StartTimer(1000000);
    // Should be == not != as now. This is done in order to complete tests.
    QVERIFY2( mEngineWrapper->currentFrequency() != freq_D, "Tuning below minimum frequency succeeded?");
    QVERIFY2( !mEngineWrapper->isFrequencyValid( mEngineWrapper->currentFrequency() ), "A frequency below minimum accepted?");
    }

void TestRadioEngineWrapper::testCancelSeeking()
    {
    FUNC_LOG;
    QVERIFY2( mEngineWrapper->isRadioOn(), "Radio is not on!" );

    mEngineWrapper->startSeeking(Seek::Up,
            TuneReason::StationScanInitialization);
    mSchedulerTimer->StartTimer(1000000);
    TInt frequency1(mEngineWrapper->currentFrequency());

    mEngineWrapper->startSeeking(Seek::Up, TuneReason::StationScan);
    tstSetFrequency(mEngineWrapper->currentFrequency()
            + mEngineWrapper->frequencyStepSize());
    TInt err = mPropertyFrequency.Set(KStub_KRadioServerPropertyCategory,
            ERadioServPsFrequency, tstGetFrequency());
    QVERIFY2( KErrNone == err, "Setting property ERadioServPsFrequency failed!" );
    mSchedulerTimer->StartTimer(1000000);
    QVERIFY2( mEngineWrapper->currentFrequency() > frequency1, "Seeking Upwards failed!" );
    frequency1 = mEngineWrapper->currentFrequency();

    mEngineWrapper->cancelSeeking();
    }

/* Mute's callback function CRadioEngine::MrpoMuteChange() is commented in radio engine, so no point to test here.
 void TestRadioEngineWrapper::testMute()
 {
 FUNC_LOG;
 QVERIFY2( mEngineWrapper->isRadioOn(), "Radio is not on!" );
 // Toggle mute.
 TBool muted_1( mEngineWrapper->isMuted() );
 INFO_1( "muted_1", muted_1 );
 mEngineWrapper->setMute( !muted_1 );
 TInt err = mPropertyFrequency.Set( KStub_KRadioServerPropertyCategory, ERadioServPsMuteStatus,  muted_1 ? 0x0 : 0xff );
 QVERIFY2( KErrNone == err, "Setting property ERadioServPsMuteStatus failed!" );
 mSchedulerTimer->StartTimer( 1000000 );
 TBool muted_2( mEngineWrapper->isMuted() );
 QVERIFY2(  mEnteredSlots &= MuteChanged, "Mute not changed!");
 mEnteredSlots &= !MuteChanged;    
 QVERIFY2( muted_1 != muted_2, "Mute() not working!" );

 // Toggle back to original value
 mEngineWrapper->setMute( muted_1 );
 err = mPropertyFrequency.Set( KStub_KRadioServerPropertyCategory, ERadioServPsMuteStatus,  muted_1 );
 QVERIFY2( KErrNone == err, "Setting property ERadioServPsMuteStatus failed!" );
 mSchedulerTimer->StartTimer( 1000000 );
 TBool muted_3( mEngineWrapper->isMuted() );
 QVERIFY2(  mEnteredSlots &= MuteChanged, "Mute not changed!");
 mEnteredSlots &= !MuteChanged;    
 QVERIFY2( muted_1 == muted_3, "Mute status not changed to original value!") ;
 }
 */

void TestRadioEngineWrapper::testVolumeSetting()
    {
    FUNC_LOG;
    // SetVoliúme(() seems to accept any value. Probably because the real check point
    //  is missing from test constellation.
    QVERIFY2( mEngineWrapper->isRadioOn(), "Radio is not on!" );
    INFO_1( "mLastRecordedVolume: %i", mLastRecordedVolume );
    TInt volume_1(10);
    mEngineWrapper->setVolume(volume_1);

    mSchedulerTimer->StartTimer(1000000);

    INFO_1( "mLastRecordedVolume: %i", mLastRecordedVolume );
    QVERIFY2( mEnteredSlots &= VolumeChanged, "Volume not changed!");
    mEnteredSlots &= !VolumeChanged;
    QVERIFY2( volume_1 == mLastRecordedVolume, "Volume has unexpected value!");

    volume_1 = mLastRecordedVolume;
    TInt volume_2((mLastRecordedVolume + 5) % 20);
    mEngineWrapper->setVolume(volume_2);

    mSchedulerTimer->StartTimer(1000000);

    INFO_1( "mLastRecordedVolume: %i", mLastRecordedVolume );
    QVERIFY2( mEnteredSlots &= VolumeChanged, "Volume not changed!");
    mEnteredSlots &= !VolumeChanged;
    QVERIFY2( volume_2 == mLastRecordedVolume, "Volume has unexpected value!");

    // Increase volume
    mEngineWrapper->setVolume(volume_1);
    mSchedulerTimer->StartTimer(1000000);
    mEngineWrapper->increaseVolume();
    mSchedulerTimer->StartTimer(1000000);
    QVERIFY2( mEnteredSlots &= VolumeChanged, "Volume not increased!");
    mEnteredSlots &= !VolumeChanged;
    QVERIFY2( volume_1 != mLastRecordedVolume, "Volume has unexpected value, not increased!");
    INFO_1( "mLastRecordedVolume: %i", mLastRecordedVolume );

    // Decrease volume
    mEngineWrapper->setVolume(volume_1);
    mSchedulerTimer->StartTimer(1000000);
    mEngineWrapper->decreaseVolume();
    mSchedulerTimer->StartTimer(1000000);
    QVERIFY2( mEnteredSlots &= VolumeChanged, "Volume not decreased!");
    mEnteredSlots &= !VolumeChanged;
    QVERIFY2( volume_1 != mLastRecordedVolume, "Volume has unexpected value, not decreased!");
    INFO_1( "mLastRecordedVolume: %i", mLastRecordedVolume );

    // Toggle twice just to get API function to be called.
    // Suggest ideas for better tests.
    mEngineWrapper->toggleAudioRoute();
    mEngineWrapper->toggleAudioRoute();

    }

void TestRadioEngineWrapper::testLoudSpeakerUsage()
    {
    FUNC_LOG;
    QVERIFY2( mEngineWrapper->isRadioOn(), "Radio is not on!" );
    TBool isLoudSpeakerUsed(mEngineWrapper->isUsingLoudspeaker());
    INFO_1( "isLoudSpeakerUsed: %i", isLoudSpeakerUsed );
    }

/*!
 * called after the last testfunction was executed
 */
void TestRadioEngineWrapper::cleanupTestCase()
    {
    FUNC_LOG;
    DeleteMUT();
    delete mScheduler;
    mScheduler = NULL;
    mRadioStubManagerChunk.Close();
    mPropertyPlayerState.Close();
    mPropertyAntennaStatus.Close();
    mPropertyOfflineMode.Close();
    mPropertyTransmitterStatus.Close();
    mPropertyFrequency.Close();
    mPropertyFrequencyRange.Close();
    mPropertyForceMonoReception.Close();
    mPropertySquelch.Close();
    mPropertyVolume.Close();
    mPropertyMuteStatus.Close();
    mPropertyBalance.Close();
    }

void TestRadioEngineWrapper::tstSetTunerCababilities(uint /*category*/)
    {
    FUNC_LOG;
    TUNER.iCaps.iFrequencyRange = ERsRangeFmEuroAmerica;
    TUNER.iCaps.iCapabilities
            = TRsTunerCapabilities::ETunerFunctionAvailableInOfflineMode
                    | TRsTunerCapabilities::ETunerFunctionRds
                    | TRsTunerCapabilities::ETunerFunctionDualTuner;
    }

void TestRadioEngineWrapper::Timeout(TUint aTimerId)
    {
    FUNC_LOG;
    if (CSchedulerStopAndStartTimer::ETimerIdCreateMUT == aTimerId)
        {
        INFO("ETimerIdCreateMUT elapsed");
        }
    else if (CSchedulerStopAndStartTimer::ETimerIdDeleteMUT == aTimerId)
        {
        INFO("ETimerIdDeleteMUT elapsed");
        }
    else if (CSchedulerStopAndStartTimer::ETimerIdRunMUT == aTimerId)
        {
        INFO("ETimerIdRunMUT elapsed");
        }
    else
        {
        INFO("Unknown timer elapsed");
        }
    }

void TestRadioEngineWrapper::CreateMUT()
    {
    FUNC_LOG;
    tstDefineAndAttachRadioServerProperties();
    TInt err(KErrNone);
    err = tstCreateCRObjects();
    QVERIFY2( KErrNone == err, "Radio CR not created!" );
    TRAP( err, mEngineWrapper = new (ELeave) RadioEngineWrapper( *this ) );
    RadioEngineUtils::InitializeL();
    bool retVal(EFalse);
    retVal = mEngineWrapper->init();
    QVERIFY2( EFalse != retVal, "mEngineWrapper->init() failed!");
    mEngineWrapper->addObserver(this);
    QVERIFY2( KErrNone == err, "Radio Engine not constructed!" );
    tstSetTunerCababilities();
    }

void TestRadioEngineWrapper::DeleteMUT()
    {
    FUNC_LOG;
    mEngineWrapper->removeObserver(this);
    delete mEngineWrapper;
    mEngineWrapper = NULL;
    }

void TestRadioEngineWrapper::tstSetFrequency(TUint aFrequency)
    {
    FUNC_LOG;
    TUNER.iFrequency = aFrequency;
    }

TInt TestRadioEngineWrapper::tstGetFrequency()
    {
    FUNC_LOG;
    return TUNER.iFrequency;
    }

void TestRadioEngineWrapper::tstSetScanningData(TUint aCount, TInt aMinFreq,
        TInt aFrequencyStepSize)
    {
    FUNC_LOG;
    TUNER.iScanStations.iCount = aCount;
    TUNER.iScanStations.iMinFreq = aMinFreq;
    TUNER.iScanStations.iFrequencyStepSize = aFrequencyStepSize;
    }

void TestRadioEngineWrapper::tstDefineAndAttachRadioServerProperties()
    {
    FUNC_LOG;
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsPlayerState, RProperty::EByteArray,
                    KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsAntennaStatus, RProperty::EInt,
                    KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsOfflineMode, RProperty::EInt,
                    KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsTransmitterStatus, RProperty::EInt,
                    KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsFrequency, RProperty::EInt,
                    KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsFrequencyRange, RProperty::EInt,
                    KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsForceMonoReception, RProperty::EInt,
                    KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsSquelch, RProperty::EInt,
                    KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsVolume, RProperty::EInt,
                    KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsMuteStatus, RProperty::EInt,
                    KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsBalance, RProperty::EByteArray,
                    KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");

    QVERIFY2(KErrNone == mPropertyPlayerState.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsPlayerState ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertyAntennaStatus.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsAntennaStatus ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertyOfflineMode.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsOfflineMode ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertyTransmitterStatus.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsTransmitterStatus ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertyFrequency.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsFrequency ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertyFrequencyRange.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsFrequencyRange ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertyForceMonoReception.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsForceMonoReception ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertySquelch.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsSquelch ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertyVolume.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsVolume ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertyMuteStatus.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsMuteStatus ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertyBalance.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsBalance ), "Property Attach() failed!");
    }

TInt TestRadioEngineWrapper::tstCreateCRObjects()
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TRAP( err, mRadioCR = CRepository::NewL(KStub_KRadioCRUid) );
    INFO_1( "Returning err = %i", err );
    return err;
    }

