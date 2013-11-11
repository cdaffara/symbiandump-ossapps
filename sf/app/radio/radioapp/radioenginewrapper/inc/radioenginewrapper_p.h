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

#ifndef RADIOENGINEWRAPPER_P_H
#define RADIOENGINEWRAPPER_P_H

// System includes
#include <e32std.h>
#include <QScopedPointer>
#include <QList>

// User includes
#include "radioenginewrapper.h"
#include "mradioenginehandlerobserver.h"

// Forward declarations
class RadioEngineHandler;
class RadioControlEventListener;
class RadioRdsListener;
class RadioSettingsIf;
class RadioSettings;
class RadioStationHandlerIf;
class RadioEngineWrapperObserver;

typedef QList<RadioEngineWrapperObserver*> ObserverList;

// Constants

#define RUN_NOTIFY_LOOP( list, func ) \
        foreach( RadioEngineWrapperObserver* observer, list ) { \
            observer->func; \
        }

// Class declaration
class RadioEngineWrapperPrivate : public MRadioEngineHandlerObserver
{
    Q_DECLARE_PUBLIC( RadioEngineWrapper )
    Q_DISABLE_COPY( RadioEngineWrapperPrivate )

    friend class RadioFrequencyScanningHandler;

public:

    RadioEngineWrapperPrivate( RadioEngineWrapper* wrapper,
                               RadioStationHandlerIf& stationHandler );

    virtual ~RadioEngineWrapperPrivate();

    /**
     * Initialization and startup
     */
    bool init();

    /**
     * Returns the radio settings
     */
    RadioSettingsIf& settings();

    /**
    * Getter for CRadioEngineHandler instance.
    * Returns reference to the CRadioEngineHandler
    */
    RadioEngineHandler& radioEnginehandler();

    /**
     * Tunes to given frequency
     */
    void setFrequency( uint frequency, const int reason );

    ObserverList& observers();

    void startSeeking( Seek::Direction direction, const int reason = TuneReason::Unspecified );

private:

// from base class MRadioEngineObserver

    void PowerEventL( TBool aPowerState, TInt aError );
    void FrequencyEventL( TUint32 aFrequency, RadioEngine::TRadioFrequencyEventReason aReason, TInt aError );
    void VolumeEventL( TInt aVolume, TInt aError );
    void MuteEventL( TBool aMuteState, TInt aError );
    void AudioModeEventL( TInt aAudioMode, TInt aError );
    void AntennaEventL( TBool aAntennaAttached, TInt aError );
    void AudioRoutingEventL( TInt aAudioDestination, TInt aError );
    void SeekingEventL( TInt aSeekingState, TInt aError );
    void RegionEventL( TInt aRegion, TInt aError );
    void FmTransmitterEventL( TBool /*aActive*/ ) {}

// from base class MRadioSystemEventObserver

    void HandleSystemEventL( TRadioSystemEventType aEventType );

private: // data

    /**
     * Pointer to the public class
     * Not own.
     */
    RadioEngineWrapper*                             q_ptr;

    /**
     * Map of radio stations read from the radio engine
     * Own.
     */
    RadioStationHandlerIf&                          mStationHandler;

    /**
     * Reference to the wrapper observer
     */
    ObserverList                                    mObservers;

    /**
     * Radio settings handler
     * Own.
     */
    QScopedPointer<RadioSettings>                   mSettings;

    /**
     * Radio engine handler.
     * Own.
     */
    QScopedPointer<RadioEngineHandler>              mEngineHandler;

    /**
     * RemCon listener.
     * Own.
     */
    QScopedPointer<RadioControlEventListener>       mControlEventListener;

    /**
     * Rds listener
     * Own.
     */
    QScopedPointer<RadioRdsListener>                mRdsListener;

    /**
     * Reason for the tune event. Can be FrequencyStrip, Carousel or station scanner
     */
    int                                             mTuneReason;

    /**
     * Flag to indicate whether or not audio should be routed to loudspeaker
     */
    bool                                            mUseLoudspeaker;

};

#endif // RADIOENGINEWRAPPER_P_H
