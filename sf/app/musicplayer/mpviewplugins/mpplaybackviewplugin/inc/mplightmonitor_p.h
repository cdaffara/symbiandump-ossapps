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
* Description: Light monitor for Playback - Private. 
*
*/

#ifndef MPLIGHTMONITOR_P_H
#define MPLIGHTMONITOR_P_H

#include <hwrmlight.h>

class MpLightMonitor;

class MpLightMonitorPrivate : public MHWRMLightObserver
{
public:

    explicit MpLightMonitorPrivate( MpLightMonitor *qq );
    void init();
    virtual ~MpLightMonitorPrivate();

private:
  
    void DoInitL();
    // From MHWRMLightObserver
    virtual void LightStatusChanged( TInt aTarget, CHWRMLight::TLightStatus aStatus );

private:

    MpLightMonitor                      *q_ptr;
    CHWRMLight::TLightStatus            iLightStatus;
    CHWRMLight                          *iLight;        //Own            

};

#endif  // MPLIGHTMONITOR_P_H
