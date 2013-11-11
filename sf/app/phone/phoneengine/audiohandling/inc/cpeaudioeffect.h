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
 * Description: Audio Effects handler
 *
 */

#ifndef CPEAUDIOEFFECT_H
#define CPEAUDIOEFFECT_H

#include <e32base.h>
#include <tmseffectobsrvr.h>

namespace TMS {
//FORWARD DECLARATIONS
class TMSFactory;
} //namespace TMS

using namespace TMS;

/**
 *  Starts and stops TMS audio streams.
 *  If the activation fails then there is a retry timer which
 *  will try the activation later.
 */
NONSHARABLE_CLASS(CPEAudioEffect) : public CBase
    {
public:

    /**
     * Two-phased constructing.
     */
    static CPEAudioEffect* NewL(TMSEffectObserver& aObserver);

    /**
     * Two-phased constructing.
     */
    static CPEAudioEffect* NewLC(TMSEffectObserver& aObserver);

    /**
     * C++ default destructor.
     */
    virtual ~CPEAudioEffect();

    /**
     * Set muted.
     */
    TInt SetMuteState(TBool aMute);
    /**
     * Get mute.
     */
    TBool MuteState() const;

    /**
     * Set volume.
     */
    void SetVolume(TInt aVolume);

    /**
     * Get volume.
     */
    TInt Volume() const;

public:
    // Functions from base classes

private:

    /**
     * C++ default constructor.
     */
    CPEAudioEffect();

    void ConstructL(TMSEffectObserver& aObserver);

private:
    // data
    TMSFactory* iFactory;
    TMSEffect* iGlobalVol;
    TMSEffect* iGlobalGain;
    };

#endif // CPEAUDIOEFFECT_H
