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

#ifndef CRADIOSYSTEMEVENTCOLLECTORIMP_H
#define CRADIOSYSTEMEVENTCOLLECTORIMP_H

#include "cradiosystemeventcollector.h"
#include "mradioaudioroutingobserver.h"
#include "mradioheadseteventobserver.h"
#include "mradiosystemeventdetectorobserver.h"
#include "mradiosystemeventobserver.h"

class CRadioAccessoryObserver;
class CRadioSystemEventDetector;

/**
 *  Collects system events and forwards them to observers
 *  Class also wraps up the getters for some system states
 */
NONSHARABLE_CLASS( CRadioSystemEventCollectorImp ) : public CRadioSystemEventCollector
                                                   , public MRadioAudioRoutingObserver
                                                   , public MRadioHeadsetEventObserver
                                                   , public MRadioSystemEventDetectorObserver
    {
public:

    static CRadioSystemEventCollectorImp* NewL();

    ~CRadioSystemEventCollectorImp();

private:

    CRadioSystemEventCollectorImp();

    void ConstructL();

// from base class CRadioSystemEventCollector

    void AddObserverL( MRadioSystemEventObserver* aSystemObserver );
    void RemoveObserver( MRadioSystemEventObserver* aSystemObserver );
    TBool IsNetworkCoverage() const;
    TBool IsMobileNetworkCoverage() const;
    TBool IsCallActive() const;
    TBool IsAudioResourcesAvailable() const;
    TBool IsHeadsetConnectedL() const;
    void AsObserver( MRadioAudioRoutingObserver*& obj )const;
    
    void NotifyObserversL( TRadioSystemEventType aEvent );

// from base class MRadioSystemEventDetectorObserver

    void NetworkUpCallbackL();
    void NetworkDownCallbackL();
    void CallActivatedCallbackL();
    void CallDeactivatedCallbackL();
    void AudioResourcesAvailableL();
    void AudioAutoResumeForbiddenL();
    void ErrorCallbackL( TInt aError );

// from base class MRadioHeadsetEventObserver

    void HeadsetConnectedCallbackL();
    void HeadsetDisconnectedCallbackL();

// from base class MRadioAudioRoutingObserver

    void AudioRouteChangedL( RadioEngine::TRadioAudioRoute aRoute );

private: // data

    /**
     * Array of system event observers
     */
    RPointerArray<MRadioSystemEventObserver> iSystemEventObservers;

    /**
     * Detects system events
     */
    CRadioSystemEventDetector* iSystemEventDetector;

    /**
     * Observer for accessory events
     */
    CRadioAccessoryObserver* iHeadsetObserver;

    };

#endif // CRADIOSYSTEMEVENTCOLLECTORIMP_H
