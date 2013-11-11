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
 * Description:  TMS AudioEffect.
 *
 */

#include <glib.h>
#include <tms.h>
#include <tmsfactory.h>
#include <tmsglobalvoleffect.h>
#include <tmsglobalgaineffect.h>
#include <tmsver.h>
#include "cpeaudioeffect.h"
#include "pepanic.pan"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Static constructor
// ---------------------------------------------------------------------------
//
CPEAudioEffect* CPEAudioEffect::NewL(TMSEffectObserver& aObserver)
    {
    CPEAudioEffect* self = CPEAudioEffect::NewLC(aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Static constructor
// ---------------------------------------------------------------------------
//
CPEAudioEffect* CPEAudioEffect::NewLC(TMSEffectObserver& aObserver)
    {
    CPEAudioEffect* self = new (ELeave) CPEAudioEffect();
    CleanupStack::PushL(self);
    self->ConstructL(aObserver);
    return self;
    }

// ---------------------------------------------------------------------------
// CPEAudioEffect::CPEAudioEffect
// ---------------------------------------------------------------------------
//
CPEAudioEffect::CPEAudioEffect()
    {
    }

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CPEAudioEffect::ConstructL(TMSEffectObserver& aObserver)
    {
    TInt err(KErrNotFound);
    TMSVer v(2,0,0);
    TMSFactory::CreateFactory(iFactory, v);
    if (iFactory)
        {
        err = iFactory->CreateEffect(TMS_EFFECT_GLOBAL_GAIN, iGlobalGain);
        err |= iFactory->CreateEffect(TMS_EFFECT_GLOBAL_VOL, iGlobalVol);
        }
    if (err)
        {
        User::Leave(KErrNotFound);
        }
    err = static_cast<TMSGlobalVolEffect*>(iGlobalVol)->AddObserver(aObserver,
            NULL);
    User::LeaveIfError(err);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPEAudioEffect::~CPEAudioEffect()
    {
    if ( iFactory != NULL )
        {
        iFactory->DeleteEffect ( iGlobalGain );
        iFactory->DeleteEffect ( iGlobalVol );
    }
    delete iFactory;
    }

// ---------------------------------------------------------------------------
// CPEAudioEffect::SetMuteState(TBool aMute)
// ---------------------------------------------------------------------------
//
TInt CPEAudioEffect::SetMuteState(TBool aMute)
    {
    __ASSERT_DEBUG(iGlobalGain, Panic(EPEPanicBadHandle));
    guint value(0);
    TInt err(KErrNotFound);
    if (aMute)
        {
        err = static_cast<TMSGlobalGainEffect*>(iGlobalGain)->SetLevel(0);
        }
    else
        {
        err = static_cast<TMSGlobalGainEffect*>(iGlobalGain)->GetMaxLevel(
                value);
        if (err == TMS_RESULT_SUCCESS)
            {
            err = static_cast<TMSGlobalGainEffect*>(iGlobalGain)->SetLevel(
                    value);
            }
        }
    return err;
    }

// ---------------------------------------------------------------------------
// CPEAudioEffect::SetUnmuted
// ---------------------------------------------------------------------------
//
TBool CPEAudioEffect::MuteState() const
    {
    __ASSERT_DEBUG(iGlobalGain, Panic(EPEPanicBadHandle));
    guint value;
    static_cast<TMSGlobalGainEffect*>(iGlobalGain)->GetLevel(value);
    return (value == 0) ? ETrue : EFalse;
    }

// ---------------------------------------------------------------------------
// CPEAudioEffect::SetVolume
// ---------------------------------------------------------------------------
//
void CPEAudioEffect::SetVolume(TInt aVolume)
    {
    __ASSERT_DEBUG(iGlobalVol, Panic(EPEPanicBadHandle));
    static_cast<TMSGlobalVolEffect*>(iGlobalVol)->SetLevel(aVolume);
    }

// ---------------------------------------------------------------------------
// CPEAudioEffect::Volume
// ---------------------------------------------------------------------------
//
TInt CPEAudioEffect::Volume() const
    {
    __ASSERT_DEBUG(iGlobalVol, Panic(EPEPanicBadHandle));
    guint value(0);
    static_cast<TMSGlobalVolEffect*>(iGlobalVol)->GetLevel(value);
    return value;
    }

//  End of File
