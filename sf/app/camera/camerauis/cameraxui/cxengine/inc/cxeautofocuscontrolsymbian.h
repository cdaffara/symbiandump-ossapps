/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CXEAUTOFOCUSCONTROLSYMBIAN_H
#define CXEAUTOFOCUSCONTROLSYMBIAN_H

#include <QVariant>
#include <ecam.h> // CCamera
#include <ecam/ecamadvsettingsintf.h> // CCamera
#include "cxenamespace.h"
#include "cxeautofocuscontrol.h"
#include "cxestatemachine.h"

//forward declarations
class CxeCameraDevice;
class CxeSettings;



/**
 * CxeAutoFocusControl is responsible for handling Autofocus events from the camera adaptation.
 * It also routes the Autofocus successful and failed events to the clients interested in ´knowing the
 * status of Autofocus.
 */
class CxeAutoFocusControlSymbian : public CxeAutoFocusControl,
                                   public CxeStateMachine
    {
    Q_OBJECT
public:

    CxeAutoFocusControlSymbian( CxeCameraDevice &cameraDevice, CxeSettings &settings );
    virtual ~CxeAutoFocusControlSymbian();

    CxeError::Id  start(bool soundEnabled = true);
    void cancel();
    void setMode( CxeAutoFocusControl::Mode newMode );
    CxeAutoFocusControl::Mode mode() const;
    bool isFixedFocusMode(CxeAutoFocusControl::Mode mode) const;

    CxeAutoFocusControl::State state() const;
    bool supported() const;

    bool isSoundEnabled() const;

protected: // from CxeStateMachine
    void handleStateChanged( int newStateId, CxeError::Id error );

protected:
    void initializeStates();
    // intializes all the resources we need for AF functionality
    void initializeResources();

    void setFocusRange(CCamera::CCameraAdvancedSettings::TFocusRange range);
    CCamera::CCameraAdvancedSettings::TFocusRange  focusRange() const;

    void setFocusType(CCamera::CCameraAdvancedSettings::TAutoFocusType type);
    int supportedFocusTypes() const;
    CCamera::CCameraAdvancedSettings::TAutoFocusType focusType() const;

protected slots:

    // notifications when camera reference is changed
    void prepareForCameraDelete();
    void handleCameraAllocated(CxeError::Id error);

    // notification for when camera is released
    void prepareForRelease();

    // Handle ECam events
    void handleCameraEvent( int eventUid, int error );
    void handleSceneChanged(const QVariant &scene);
    void handleSettingValueChanged(const QString &settingId, QVariant newValue);

private:

    // handles to-do things if the event is AF event
    void handleAfEvent(int eventUid, int error);

protected:
    CxeCameraDevice &mCameraDevice;
    CCamera::CCameraAdvancedSettings *mAdvancedSettings; // not owned
    CxeAutoFocusControl::Mode mAfMode;
    CCamera::CCameraAdvancedSettings::TFocusRange mAFRange;
    CxeSettings &mSettings;
    bool mCancelled;
    bool mSoundEnabled;
    bool mFaceTrackingOverride; //need for temporary override of the AF mode if FT is enabled by user
    CxeAutoFocusControl::Mode mPreviousAFMode; //for restoring previous AF mode in case of FT override
    };

#endif // CXEAUTOFOCUSCONTROLSYMBIAN_H
