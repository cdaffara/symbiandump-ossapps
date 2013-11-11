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

#ifndef T_RADIOENGINEWRAPPER_H_
#define T_RADIOENGINEWRAPPER_H_

// System includes
#include <QtTest/QtTest>
#include <e32property.h>
#include <centralrepository.h>
#include <RadioStubManager.h>

// User includes
#include "radioenginewrapperobserver.h"
#include "radiostationhandlerif.h"
#include "t_schedulerstartandstoptimer.h"

// Forward declarations
class RadioUiEngine;
class RadioStationModel;
class RadioPlayLogModel;
class RadioPresetStorage;
class RadioEngineWrapper;

class TestRadioEngineWrapper : public QObject,
        RadioStationHandlerIf,
        RadioEngineWrapperObserver,
        MSchedulerStartAndStopTimerObserver
    {
Q_OBJECT

    /**
     * Flags to indicate which slots have been entered since calling API method. 
     * Declared to use QFlags<> to ease flag usage and to enforce type safety.
     */
    enum SlotEnteredFlag
        {
        NoSlotsEntered = 0
        ,TunedToFrequency = 1 << 0
        ,MuteChanged = 1 << 1
        ,VolumeChanged = 1 << 2
        ,AntennaChanged = 1 << 3
        ,ItemAdded = 1 << 4
        };
    Q_DECLARE_FLAGS( Slots, SlotEnteredFlag )

public:

    TestRadioEngineWrapper();
    ~TestRadioEngineWrapper();

private slots:
    // test framework called slots 
    void initTestCase();
    void init();
    void cleanup();

    void testRadioSettingsReference();

    void testRegion();

    void testRadioOnOff();

    void testTuning();

    void testCancelSeeking();

    // Mute's callback function CRadioEngine::MrpoMuteChange() is commented in radio engine, so no point to test here.
    // void testMute();

    void testVolumeSetting();

    void testLoudSpeakerUsage();

    void cleanupTestCase();

private:

    // from base class RadioStationHandlerIf =>
    uint currentFrequency() const;
    int currentPresetIndex() const;

    void setCurrentStation(uint frequency);

    bool containsFrequency(uint frequency);

    bool containsPresetIndex(int presetIndex);

    void startDynamicPsCheck();

    void addScannedFrequency(uint frequency);

    void removeLocalStations();

    void setCurrentPsName(uint frequency, const QString& name);
    void setCurrentRadioText(uint frequency, const QString& radioText);
    void setCurrentRadioTextPlus(uint frequency, int rtClass,
            const QString& rtItem);
    void setCurrentPiCode(uint frequency, int piCode);
    void setCurrentGenre(uint frequency, int genre);
    // <= from base class RadioStationHandlerIf 

    // from base class RadioStationHandlerIf =>
    void tunedToFrequency(uint frequency, int commandSender);

    void rdsAvailabilityChanged(bool available);

    void increaseVolume();
    void decreaseVolume();
    void volumeChanged(int volume);
    void muteChanged(bool muted);

    void antennaStatusChanged(bool connected);

    void audioRouteChanged(bool loudspeaker);

    void skipPrevious();
    void skipNext();
    // <= from base class RadioStationHandlerIf 

    // from base class MSchedulerStartAndStopTimerObserver =>
    void Timeout(TUint aTimerId);
    void CreateMUT();
    void DeleteMUT();
    // <= from base class MSchedulerStartAndStopTimerObserver

    void tstSetTunerCababilities(uint category = 0);
    void tstSetFrequency(TUint aFrequency);
    TInt tstGetFrequency();
    void tstSetScanningData(TUint aCount, TInt aMinFreq,
            TInt aFrequencyStepSize);
    void tstDefineAndAttachRadioServerProperties();
    TInt tstCreateCRObjects();

private:
    RadioEngineWrapper* mEngineWrapper;

    // RadioStubManager pointer points inside RadioStubManagerChunk
    SRadioStubManager* mRadioStubManager;

    // RadioStubManagerChunk handle
    RChunk mRadioStubManagerChunk;

    // Active scheduler
    // Workaround for the below panic, occured after porting to 10.1 
    // Main Panic E32USER-CBase 44
    // Create and install the active scheduler
    CActiveScheduler* mScheduler;


    Slots mEnteredSlots;

    CSchedulerStopAndStartTimer* mSchedulerTimer;

    RProperty mPropertyPlayerState;
    RProperty mPropertyAntennaStatus;
    RProperty mPropertyOfflineMode;
    RProperty mPropertyTransmitterStatus;
    RProperty mPropertyFrequency;
    RProperty mPropertyFrequencyRange;
    RProperty mPropertyForceMonoReception;
    RProperty mPropertySquelch;
    RProperty mPropertyVolume;
    RProperty mPropertyMuteStatus;
    RProperty mPropertyBalance;

    CRepository* mRadioCR;

    TInt mLastRecordedVolume;
    };

#endif /* T_RADIOENGINEWRAPPER_H_ */
