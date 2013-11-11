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

#include "cxutils.h"
#include "cxestate.h"
#include "cxesettings.h"
#include "cxenamespace.h"
#include "cxestillcapturecontrol.h"
#include "cxevideocapturecontrol.h"
#include "cxegeotaggingtrail_symbian_p.h"

#include <locationtrailpskeys.h>

namespace
{
    // in milliseconds
    const int STOP_TRAIL_INTERVAL = 10*1000;
    const int START_TRAIL_ATTEMPTS = 5;
}

/*!
* Constructor
*/
CxeGeoTaggingTrailPrivate::CxeGeoTaggingTrailPrivate(CxeStillCaptureControl &stillControl, 
                                                     CxeVideoCaptureControl &videoControl,
                                                     CxeSettings &settings)
    : CxeStateMachine("CxeGeoTaggingTrailPrivate"),
      mStillCaptureControl(stillControl),
      mVideoCaptureControl(videoControl),
      mSettings(settings),
      mStopLocationTrailTimer(),
      mPendingStopTrailSession(false)
{
    CX_DEBUG_ENTER_FUNCTION();

    qRegisterMetaType<CxeGeoTaggingTrail::State>();
    initializeStates();

    QVariant locationTrailState;
    // Get initial location trail state.
    mSettings.get(KPSUidLocationTrail.iUid, KLocationTrailState, 
                  Cxe::PublishAndSubscribe, locationTrailState);

    connect(&mSettings, SIGNAL(settingValueChanged(long int, unsigned long int, QVariant)),
            this, SLOT(handleGeoTaggingPropertyEvent(long int, unsigned long int, QVariant)));


    connect(&mSettings, SIGNAL(settingValueChanged(const QString&,QVariant)),
            this, SLOT(handleSettingValueChanged(const QString&,QVariant)));
    
    connect(&mStopLocationTrailTimer, SIGNAL(timeout()),
            this, SLOT(timeout()), Qt::UniqueConnection);    
   
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Destructor
*/
CxeGeoTaggingTrailPrivate::~CxeGeoTaggingTrailPrivate()
{
    CX_DEBUG_ENTER_FUNCTION();

    // close the location utility session
    mLocationTrail.Close();

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Start location trail.
*/
void CxeGeoTaggingTrailPrivate::start()
{
    CX_DEBUG_ENTER_FUNCTION();

    int err = KErrNone;
    int retryTrail = START_TRAIL_ATTEMPTS; // In case RLocationTrail::StartLocationTrail() fail, we will retry
    int settingValue = Cxe::GeoTaggingOff;
    settingValue = mSettings.get(CxeSettingIds::GEOTAGGING, settingValue);
    
    if (settingValue == Cxe::GeoTaggingOn) {
        while(retryTrail) {
            // geotagging setting is ON, trying to start location trail
            if (state() == CxeGeoTaggingTrail::NotConnected) {
                err = mLocationTrail.Connect();
                if (!err) {
                    CX_DEBUG(("CxeGeoTaggingTrail <> location trail connected"));
                    setState(CxeGeoTaggingTrail::Connected);
                }
            }
            // KErrAlreadyExists error means no harm to us
            if (state() == CxeGeoTaggingTrail::Connected &&
                (!err || err == KErrAlreadyExists)) {
                err = mLocationTrail.StartLocationTrail(RLocationTrail::ECaptureAll);
                if (!err || err == KErrAlreadyExists) {
                    CX_DEBUG(("CxeGeoTaggingTrail <> starting location trail"));
                    mStopLocationTrailTimer.stop(); // stop location timer.
                    setState(CxeGeoTaggingTrail::TrailStarted);
                }
            }
            if (err && err != KErrAlreadyExists) {
                CX_DEBUG(("CxeGeoTaggingTrailPrivate::start <> FAILED: error = %d . Retries left = %d", err, retryTrail));
                mLocationTrail.Close();
                retryTrail--;
            }
            else {
                retryTrail = 0;
            }
        }
    } else {
        // geotagging setting off, do nothing.
        CX_DEBUG(("CxeGeoTaggingTrail <> start -- Geotagging setting OFF, do nothing.."));
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Stop location trail.
*/
void CxeGeoTaggingTrailPrivate::stop()
{
    CX_DEBUG_ENTER_FUNCTION();

    bool ok2StopTrail = canStopTrail();

    if (ok2StopTrail) {

        if (state() == CxeGeoTaggingTrail::TrailStarted ||
            state() == CxeGeoTaggingTrail::DataAvailable) {
            CX_DEBUG(("CxeGeoTaggingTrailPrivate::StopLocationTrail"));
            // stop location trail timer.
            mStopLocationTrailTimer.stop();
            mLocationTrail.StopLocationTrail();
            setState(CxeGeoTaggingTrail::Connected);
        }

    } else {
        // not ready to stop the location trail, TrailStarted the timer.
        mStopLocationTrailTimer.start(STOP_TRAIL_INTERVAL);
    }
    
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Checking possible stillcapturecontrol/videocapturecontrol states for stopping
* location trail.
*/
bool CxeGeoTaggingTrailPrivate::canStopTrail() const
{
    // checking still capture control states
    bool ok = mStillCaptureControl.state() != CxeStillCaptureControl::Capturing;
    
    // Still side OK, checking video capture control states
    if (ok) {
        ok = (mVideoCaptureControl.state() != CxeVideoCaptureControl::Recording &&
              mVideoCaptureControl.state() != CxeVideoCaptureControl::Paused &&
              mVideoCaptureControl.state() != CxeVideoCaptureControl::Stopping);
    }

    return ok;
}

/*!
* Slot that is called when timer timeout signal is triggered. We track this to do pending
* stopping of location trail.
*/
void CxeGeoTaggingTrailPrivate::timeout()
{
    CX_DEBUG_ENTER_FUNCTION();
    
    // stop the pending location trail utility
    stop();
    
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle new setting value.
* Check if the geotagging setting has changed.
*/
void CxeGeoTaggingTrailPrivate::handleSettingValueChanged(const QString& settingId, QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    
    if (settingId == CxeSettingIds::GEOTAGGING) {
        if (newValue.toInt() == Cxe::GeoTaggingOn) {
            // setting is turned ON, start location trail
            start();
        } else {
            // setting is turned OFF, stopping location trail
            stop();
        }
    } else if (settingId == CxeSettingIds::GEOTAGGING_DISCLAIMER) {
        if (newValue.toInt() == Cxe::GeoTaggingDisclaimerDisabled) {
            // geotagging disclaimer is diabled, we can start location trail.
            start();
        }
    }
    
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle new setting value.
* Check if the geotagging setting has changed.
*/
void CxeGeoTaggingTrailPrivate::handleGeoTaggingPropertyEvent(long int uid,
                                                              unsigned long int key,
                                                              QVariant value)
{
    CX_DEBUG_ENTER_FUNCTION();
    bool stateOk = (state() == CxeGeoTaggingTrail::DataAvailable || state() == CxeGeoTaggingTrail::TrailStarted);
    if (uid == KPSUidLocationTrail.iUid && key == KLocationTrailState && stateOk) {
        CX_DEBUG(("Location trail: new state = %d ", value.toInt()));

        RLocationTrail::TTrailState newState = 
                static_cast<RLocationTrail::TTrailState>(value.toInt());

        if (newState == RLocationTrail::ETrailStarted) {
            CX_DEBUG(("CxeGeoTaggingTrail <> location trail started, data available."));
            setState(CxeGeoTaggingTrail::DataAvailable);
        } else {
           // ignoring all other state changes.
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
Returns current state of Location trail
*/
CxeGeoTaggingTrail::State CxeGeoTaggingTrailPrivate::state() const
{
    return static_cast<CxeGeoTaggingTrail::State> (stateId());
}

/*!
* slot called when state is changed.
*/
void CxeGeoTaggingTrailPrivate::handleStateChanged(int newStateId, CxeError::Id error)
{
    emit stateChanged(static_cast<CxeGeoTaggingTrail::State> (newStateId), error);
    
}

/*!
* Initialize states for geotaggingtrail
*/
void CxeGeoTaggingTrailPrivate::initializeStates()
{
    // addState( id, name, allowed next states )
    addState(new CxeState(CxeGeoTaggingTrail::NotConnected, "NotConnected", CxeGeoTaggingTrail::Connected));
    
    addState(new CxeState(CxeGeoTaggingTrail::Connected, "Connected", CxeGeoTaggingTrail::TrailStarted | 
                                                                      CxeGeoTaggingTrail::NotConnected));
    
    addState(new CxeState(CxeGeoTaggingTrail::TrailStarted, "TrailStarted", CxeGeoTaggingTrail::DataAvailable |
                                                                            CxeGeoTaggingTrail::Connected |
                                                                            CxeGeoTaggingTrail::NotConnected));
    
    addState(new CxeState(CxeGeoTaggingTrail::DataAvailable, "DataAvailable", CxeGeoTaggingTrail::Connected | 
                                                                              CxeGeoTaggingTrail::NotConnected));
    

    setInitialState(CxeGeoTaggingTrail::NotConnected);
}

// end of file
