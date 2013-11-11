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

#ifndef MRADIOSYSTEMEVENTOBSERVER_H
#define MRADIOSYSTEMEVENTOBSERVER_H

// Data types

/** Type of system event */
enum TRadioSystemEventType
    {
    ERadioHeadsetConnected,         ///< Headset was connected
    ERadioHeadsetDisconnected,      ///< Headset was disconnected
    ERadioNetworkCoverageUp,        ///< Network coverage detected
    ERadioNetworkCoverageDown,      ///< Network coverage lost
    ERadioCallActivated,            ///< Call activated or ringing
    ERadioCallDeactivated,          ///< Call disconnected
    ERadioEmergencyCallActivated,   ///< Call activated or ringing
    ERadioEmergencyCallDeactivated, ///< Call disconnected
    ERadioLowDiskSpace,             ///< Low disk space
    ERadioAudioRouteHeadset,        ///< Audio routed through headset
    ERadioAudioRouteSpeaker,        ///< Audio routed through speaker ( IHF )
    ERadioAudioResourcesAvailable,  ///< Audio resources have become available
    ERadioAudioAutoResumeForbidden  ///< Audio auto resuming is forbidden
    };


/**
 * Observer for any system event state change.
 */
NONSHARABLE_CLASS( MRadioSystemEventObserver )
    {
public:

    /**
     * Called when system event occurs.
     * @param aEventType Type of the event
     */
    virtual void HandleSystemEventL( TRadioSystemEventType aEventType ) = 0;

    };

#endif // MRADIOSYSTEMEVENTOBSERVER_H


