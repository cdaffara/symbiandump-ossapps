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
#include "cxegeotaggingtrail_desktop_p.h"


namespace
{
    // in milliseconds
    const int STOP_TRAIL_INTERVAL = 10*1000;
}


/*!
* Constructor
*/
CxeGeoTaggingTrailPrivate::CxeGeoTaggingTrailPrivate(CxeStillCaptureControl &stillControl,
                                                     CxeVideoCaptureControl &videoControl,
													 CxeSettings &settings)
    : CxeStateMachine("CxeGeoTaggingTrailPrivate"),
      mSettings(settings)
{
    CX_DEBUG_ENTER_FUNCTION();	
    Q_UNUSED(stillControl);
    Q_UNUSED(videoControl);

    qRegisterMetaType<CxeGeoTaggingTrail::State>();
    initializeStates();

    connect(&mSettings, SIGNAL(settingValueChanged(const QString&,QVariant)),
            this, SLOT(handleSettingValueChanged(const QString&,QVariant)));
    
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Destructor
*/
CxeGeoTaggingTrailPrivate::~CxeGeoTaggingTrailPrivate()
{
    CX_DEBUG_ENTER_FUNCTION();

    // stop trail and close location utility session
    stop(true);

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Start location trail.
*/
void CxeGeoTaggingTrailPrivate::start()
{
    CX_DEBUG_ENTER_FUNCTION();

    setState(CxeGeoTaggingTrail::Connected);
    setState(CxeGeoTaggingTrail::TrailStarted);

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Stop location trail.
* @ param closeSession, indicates if we are willing to close the location utility session.
*/
void CxeGeoTaggingTrailPrivate::stop(bool closeSession)
{
    CX_DEBUG_ENTER_FUNCTION();

    setState(CxeGeoTaggingTrail::NotConnected);

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
