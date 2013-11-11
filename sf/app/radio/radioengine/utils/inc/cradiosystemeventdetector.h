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

#ifndef CRADIOSYSTEMEVENTDETECTOR_H
#define CRADIOSYSTEMEVENTDETECTOR_H

#include <e32base.h>

#include "cradiopropertyobserver.h"

class MRadioSystemEventDetectorObserver;

/**
* Catches notification of the call and network state changes.
*
* Class provides implementation to detect system events and notify UI components of the event,
* Class keeps several instances of CVPropertyObserver, and those instances send event notifications
* about the particular system component state change.
*/
NONSHARABLE_CLASS( CRadioSystemEventDetector ) : public CBase
                                               , public MRadioPropertyChangeObserver
    {
public:

    static CRadioSystemEventDetector* NewL( MRadioSystemEventDetectorObserver&  aObserver );

    virtual ~CRadioSystemEventDetector();

    /**
     * Returns network coverage state ( wlan or mobile ).
     */
    TBool IsNetworkCoverage() const;

    /**
     * Returns mobile network coverage state.
     */
    TBool IsMobileNetworkCoverage() const;

    /**
     * Returns current call state.
     */
    TBool IsCallActive() const;

    /**
     * Returns current audio resources state.
     * @return audio resources state
     */
    TBool IsAudioResourcesAvailable() const;

    /**
     * Returns current Voice UI state.
     * @return ETrue if Voice UI is active, EFalse otherwise
     */
    TBool IsVoiceUiActive() const;

protected:

// from base class MRadioPropertyChangeObserver

    void HandlePropertyChangeL( const TUid& aCategory, const TUint aKey, const TInt aValue );
    void HandlePropertyChangeL( const TUid& aCategory, const TUint aKey, const TDesC8& aValue );
    void HandlePropertyChangeL( const TUid& aCategory, const TUint aKey, const TDesC& aValue );
    void HandlePropertyChangeErrorL( const TUid& aCategory, const TUint aKey, TInt aError );

private:

    void ConstructL();

    CRadioSystemEventDetector( MRadioSystemEventDetectorObserver&  aObserver );

private: // data

    /** Observer which will be notified of the call and network state changes*/
    MRadioSystemEventDetectorObserver&  iObserver;

    /** handler to Publish & Subscribe interface*/
    CRadioPropertyObserver*     iCallStatusObserver;
    /** handler to Publish & Subscribe interface*/
    CRadioPropertyObserver*     iNetworkStatusObserver;
    /** handler to Publish & Subscribe interface*/
    CRadioPropertyObserver*     iAudioPolicyObserver;

    /**Indicator for the mobile network status*/
    TBool                       iIsMobileNetworkCoverage;

    /**Indicator for the call status*/
    TBool                       iIsCallActive;

    /** Array of audio policy categories that radio must not resume after. */
    RArray<TInt>                iNoAutoResumeAudioCategories;

    /** Indicator for audio resource status. */
    TBool                       iIsAudioResourcesAvailable;
    
    };

#endif // CRADIOSYSTEMEVENTDETECTOR_H
