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

#ifndef CXUIFAKEAUTOFOCUSCONTROL_H
#define CXUIFAKEAUTOFOCUSCONTROL_H

#include "cxeautofocuscontrolsymbian.h"
#include "cxestatemachine.h"

//forward declarations
class CxeCameraDevice;
class CxeAutoFocusControlSymbian;


/**
 * CxeAutoFocusControl is responsible for handling Autofocus events from the camera adaptation.
 * It also routes the Autofocus successful and failed events to the clients interested in ´knowing the
 * status of Autofocus.
 */
class CxuiFakeAutoFocusControl : public CxeAutoFocusControlSymbian
{
    Q_OBJECT
public:
    
    CxuiFakeAutoFocusControl( CxeCameraDevice &cameraDevice );
    virtual ~CxuiFakeAutoFocusControl();

    CxeError::Id  start();
    void cancel();
    void setMode( CxeAutoFocusControl::Mode newMode );
    CxeAutoFocusControl::Mode mode() const;

    CxeAutoFocusControl::State state() const;
    bool supported() const;
    int setFakeState(CxeAutoFocusControl::State);

protected: // from CxeStateMachine    
    void handleStateChanged( int newStateId, CxeError::Id error );


private:
    // handles to-do things if the event is AF event
    void handleAfEvent(int eventUid, int error);

private:
    CxeCameraDevice &mCameraDevice;
    CCamera::CCameraAdvancedSettings *mAdvancedSettings; // not owned
    CxeAutoFocusControl::Mode mAfMode;
    CCamera::CCameraAdvancedSettings::TFocusRange mAFRange;
};

#endif // CXUIFAKEAUTOFOCUSCONTROL_H
