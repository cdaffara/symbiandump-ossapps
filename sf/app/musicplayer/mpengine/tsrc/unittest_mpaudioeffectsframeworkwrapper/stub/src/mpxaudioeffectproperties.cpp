/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  audio effect properties stub for testing mpmpxasframeworkwrapper
*
*/

#include "mptrace.h"
#include "stub/inc/mpxaudioeffectproperties.h"

/*!
 Stub function.
*/
CMPXAudioEffectProperties::CMPXAudioEffectProperties():iBalance(0),
                                                       iLoudness(EFalse),
                                                       iLoadFromFile(EFalse),
                                                       iSaveToFile(EFalse),
                                                       iLeave(EFalse)
{
}

/*!
 Stub function.
*/
CMPXAudioEffectProperties::~CMPXAudioEffectProperties()
{
}

/*!
 Stub function.
*/
TInt CMPXAudioEffectProperties::Balance()
{
    return iBalance;
}

/*!
 Stub function.
*/
TBool CMPXAudioEffectProperties::Loudness()
{
    return iLoudness;
}

/*!
 Stub function.
*/
void CMPXAudioEffectProperties::SetBalance(TInt aBalance)
{
    iBalance = aBalance;
}

/*!
 Stub function.
*/
void CMPXAudioEffectProperties::SetLoudness(TBool aLoudness)
{
    iLoudness = aLoudness;
}

/*!
 Stub function.
*/
void CMPXAudioEffectProperties::Reset()
{
    TX_ENTRY
    iBalance = 0;
    iLoudness = EFalse;
    iLeave = EFalse;
    iLoadFromFile = EFalse;
    TX_EXIT
}

/*!
 Stub function.
*/
void CMPXAudioEffectProperties::SaveToFileL()
{
    TX_ENTRY
    if (iLeave) {
        iSaveToFile = EFalse;
        User::Leave(KErrNotFound);
    }
    else {
        iSaveToFile = ETrue;
    }
    TX_EXIT
}

/*!
 Stub function.
*/
void CMPXAudioEffectProperties::LoadFromFileL()
{
    TX_ENTRY
    if (iLeave) {
        iLoadFromFile = EFalse;
        User::Leave(KErrNotFound);
    }
    else {
        iLoadFromFile = ETrue;
    }
    TX_EXIT
}

//end of file
