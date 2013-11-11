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

// System includes
#include <e32cmn.h>

// User includes
#include "cradioaccessoryobserver.h"
#include "cradiosystemeventcollectorimp.h"
#include "cradiosystemeventdetector.h"
#include "radioengineutils.h"
#include "cradioenginelogger.h"

const TInt KVRObserverArrayGranularity( 2 );

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSystemEventCollectorImp::CRadioSystemEventCollectorImp() :
    iSystemEventObservers( KVRObserverArrayGranularity )
    {
    LEVEL3( LOG_METHOD_AUTO );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::ConstructL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    RadioEngineUtils::InitializeL();
    iSystemEventDetector = CRadioSystemEventDetector::NewL( *this );

    iHeadsetObserver = CRadioAccessoryObserver::NewL();
    iHeadsetObserver->SetObserver( this );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSystemEventCollectorImp* CRadioSystemEventCollectorImp::NewL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    CRadioSystemEventCollectorImp* self = new( ELeave ) CRadioSystemEventCollectorImp;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSystemEventCollectorImp::~CRadioSystemEventCollectorImp()
    {
    LEVEL3( LOG_METHOD_AUTO );
    delete iSystemEventDetector;
    delete iHeadsetObserver;

    iSystemEventObservers.Close();
    RadioEngineUtils::Release();
    }

// ---------------------------------------------------------------------------
// Adds a system event observer
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::AddObserverL( MRadioSystemEventObserver* aHeadsetObserver )
    {
    LEVEL3( LOG_METHOD_AUTO );
    iSystemEventObservers.AppendL( aHeadsetObserver );
    }

// ---------------------------------------------------------------------------
// Removes a system event observer
// ---------------------------------------------------------------------------
//
 void CRadioSystemEventCollectorImp::RemoveObserver( MRadioSystemEventObserver* aSystemObserver )
    {
     LEVEL3( LOG_METHOD_AUTO );
    TInt objectIndex = iSystemEventObservers.Find( aSystemObserver );

    if ( objectIndex != KErrNotFound )
        {
        iSystemEventObservers.Remove( objectIndex );
        }
    }

// ---------------------------------------------------------------------------
// Getter for mobile network state
// ---------------------------------------------------------------------------
//
TBool CRadioSystemEventCollectorImp::IsMobileNetworkCoverage() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iSystemEventDetector->IsMobileNetworkCoverage();
    }

// ---------------------------------------------------------------------------
// Getter for network state
// ---------------------------------------------------------------------------
//
TBool CRadioSystemEventCollectorImp::IsNetworkCoverage() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iSystemEventDetector->IsNetworkCoverage();
    }

// ---------------------------------------------------------------------------
// Getter for call state
// ---------------------------------------------------------------------------
//
TBool CRadioSystemEventCollectorImp::IsCallActive() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iSystemEventDetector->IsCallActive();
    }

// ---------------------------------------------------------------------------
// Getter for audio resource state
// ---------------------------------------------------------------------------
//
TBool CRadioSystemEventCollectorImp::IsAudioResourcesAvailable() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iSystemEventDetector->IsAudioResourcesAvailable();
    }

// ---------------------------------------------------------------------------
// Getter for headset connection status
// ---------------------------------------------------------------------------
TBool CRadioSystemEventCollectorImp::IsHeadsetConnectedL() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iHeadsetObserver->IsHeadsetConnectedL();
    }

// ---------------------------------------------------------------------------
// returns this as MRadioAudioRoutingObserver 
// ---------------------------------------------------------------------------
void CRadioSystemEventCollectorImp::AsObserver( MRadioAudioRoutingObserver*& obj )const
{
    CRadioSystemEventCollectorImp *object = const_cast<CRadioSystemEventCollectorImp*>( this );
    obj = object;
}

// ---------------------------------------------------------------------------
// Notifies the observers of system event
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::NotifyObserversL( TRadioSystemEventType aEvent )
    {
    LEVEL3( LOG_METHOD_AUTO );
    for ( TInt i = 0; i < iSystemEventObservers.Count(); ++i )
        {
        iSystemEventObservers[i]->HandleSystemEventL( aEvent );
        }
    }

// ---------------------------------------------------------------------------
// From class MRadioSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::NetworkUpCallbackL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    NotifyObserversL( ERadioNetworkCoverageUp );
    }

// ---------------------------------------------------------------------------
// From class MRadioSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::NetworkDownCallbackL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    NotifyObserversL( ERadioNetworkCoverageDown );
    }

// ---------------------------------------------------------------------------
// From class MRadioSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::CallActivatedCallbackL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    NotifyObserversL( ERadioCallActivated );
    }

// ---------------------------------------------------------------------------
// From class MRadioSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::CallDeactivatedCallbackL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    NotifyObserversL( ERadioCallDeactivated );
    }

// ---------------------------------------------------------------------------
// From class MRadioSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::AudioResourcesAvailableL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    NotifyObserversL( ERadioAudioResourcesAvailable );
    }

// ---------------------------------------------------------------------------
// From class MRadioSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::AudioAutoResumeForbiddenL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    NotifyObserversL( ERadioAudioAutoResumeForbidden );
    }

// ---------------------------------------------------------------------------
// From class MRadioSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::ErrorCallbackL( TInt DEBUGVAR3( aError ) )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aError = %d", aError ) );
    // P&S get fail not considered as a critical issue.
    }

// ---------------------------------------------------------------------------
// From class MRadioHeadsetEventObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::HeadsetConnectedCallbackL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    NotifyObserversL( ERadioHeadsetConnected );
    }

// ---------------------------------------------------------------------------
// From class MRadioHeadsetEventObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::HeadsetDisconnectedCallbackL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    NotifyObserversL( ERadioHeadsetDisconnected );
    }

// ---------------------------------------------------------------------------
// From class MRadioAudioRoutingObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::AudioRouteChangedL( RadioEngine::TRadioAudioRoute aRoute )
    {
    LEVEL3( LOG_METHOD_AUTO );
    TRadioSystemEventType ev = ERadioAudioRouteHeadset;
    if ( aRoute != RadioEngine::ERadioHeadset )
        {
        ev = ERadioAudioRouteSpeaker;
        }

    NotifyObserversL( ev );
    }
