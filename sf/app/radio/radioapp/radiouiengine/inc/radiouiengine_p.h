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

#ifndef RADIOUIENGINE_P_H_
#define RADIOUIENGINE_P_H_

// System includes
#include <QScopedPointer>
#include <QSharedPointer>

// User includes
#include "radio_global.h"
#include "radioenginewrapperobserver.h"

// Forward declarations
class RadioUiEngine;
class RadioEngineWrapper;
class RadioStationModel;
class RadioHistoryModel;
class RadioPresetStorage;
class RadioControlService;
class RadioMonitorService;
class RadioScannerEngine;
class QTimer;

class RadioUiEnginePrivate : public RadioEngineWrapperObserver
{
    Q_DECLARE_PUBLIC( RadioUiEngine )
    Q_DISABLE_COPY( RadioUiEnginePrivate )

public:

    RadioUiEnginePrivate( RadioUiEngine* engine );
    virtual ~RadioUiEnginePrivate();

    RadioUiEngine& api();

    bool init();

    void cancelSeeking();

    RadioEngineWrapper& wrapper();

private:

// from base class RadioEngineWrapperObserver

    void tunedToFrequency( uint frequency, int reason );
    void radioStatusChanged( bool radioIsOn );
    void rdsAvailabilityChanged( bool available );
    void increaseVolume();
    void decreaseVolume();
    void volumeChanged( int volume );
    void muteChanged( bool muted );
    void audioRouteChanged( bool loudspeaker );
    void antennaStatusChanged( bool connected );
    void skipPrevious();
    void skipNext();

// New functions

    /*!
     * Tunes to next or previous station
     */
    uint skip( StationSkip::Mode mode, uint startFrequency = 0, const int reason = TuneReason::Skip );

private: // data

    /**
     * Pointer to the public class
     * Not own.
     */
    RadioUiEngine*                          q_ptr;

    QScopedPointer<RadioEngineWrapper>      mEngineWrapper;

    QScopedPointer<RadioPresetStorage>      mPresetStorage;

    QScopedPointer<RadioStationModel>       mStationModel;

    QScopedPointer<RadioHistoryModel>       mHistoryModel;

    QScopedPointer<RadioControlService>     mControlService;

    QScopedPointer<RadioMonitorService>     mMonitorService;

    QWeakPointer<RadioScannerEngine>        mScannerEngine;

    /**
     * Power off timer
     * Owned by public class by setting parent
     */
    QTimer*                                 mPowerOffTimer;

};

#endif // RADIOUIENGINE_P_H_
