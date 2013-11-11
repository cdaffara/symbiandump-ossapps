/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements CPEAudioToneUtility interface
*
*/

#ifndef CPEAUDIOTONEUTILITYIMPL_H
#define CPEAUDIOTONEUTILITYIMPL_H

#include <e32base.h>

#include "cpeaudiotoneutility.h"

class MMdaAudioToneObserver;
class CMdaAudioToneUtility;

/**
 *  Wraps CMdaAudioToneUtility.
 *  To ease module testing.
 *
 *  @lib AudioHandling
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CPEAudioToneUtilityImpl ) : public CPEAudioToneUtility
    {
public:

    static CPEAudioToneUtilityImpl* NewL(
        MMdaAudioToneObserver& aToneObserver );  

    /**
    * Destructor.
    */
    virtual ~CPEAudioToneUtilityImpl();

// from base class CPEAudioToneUtility

    /**
     * From CPEAudioToneUtility
     * @see CMdaAudioToneUtility
     *
     * @since S60 5.0
     */
    void PrepareToPlayDesSequence( const TDesC8& aSequence );
    
    /**
     * From CPEAudioToneUtility
     * @see CMdaAudioToneUtility
     *
     * @since S60 5.0
     */
    TMdaAudioToneUtilityState State();
    
    /**
     * From CPEAudioToneUtility
     * @see CMdaAudioToneUtility
     *
     * @since S60 5.0
     */
    void CancelPlay();
     
     /**
     * From CPEAudioToneUtility
     * @see CMdaAudioToneUtility
     *
     * @since S60 5.0
     */
    TInt MaxVolume();
    
     /**
     * From CPEAudioToneUtility
     * @see CMdaAudioToneUtility
     *
     * @since S60 5.0
     */
    void SetVolume( TInt aVolume );
     
     /**
     * From CPEAudioToneUtility
     * @see CMdaAudioToneUtility
     *
     * @since S60 5.0
     */
    void SetRepeats(
        TInt aRepeatNumberOfTimes, 
        const TTimeIntervalMicroSeconds& aTrailingSilence );
        
    /**
     * From CPEAudioToneUtility
     * @see CMdaAudioToneUtility
     *
     * @since S60 5.0
     */
    void SetPriority(
        TInt aPriority, 
        TMdaPriorityPreference aPref);
    
    /**
     * From CPEAudioToneUtility
     * @see CMdaAudioToneUtility
     *
     * @since S60 5.0
     */
    void SetVolumeRamp( 
        const TTimeIntervalMicroSeconds& aRampDuration );
    
    /**
     * From CPEAudioToneUtility
     * @see CMdaAudioToneUtility
     *
     * @since S60 5.0
     */
    void Play();
    
    /**
     * From CPEAudioToneUtility
     * @see CMdaAudioToneUtility
     */
    void CancelPrepare();
    
private:

    CPEAudioToneUtilityImpl();

    void ConstructL( MMdaAudioToneObserver& aToneObserver );

private: // data

    /**
     * Own.
     * For playing tones.
     */
    CMdaAudioToneUtility* iToneUtility;

    };

#endif // CPEAUDIOTONEUTILITYIMPL_H
