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
* Description: Wrapper for mpx framework utilities.
*
*/

#include "mpengine.h"

#include "mptrace.h"


MpEngine::MpEngine()
    : mActivePreset(0),
      mApplyPresetCount(0),
      mDisableEquqlizerCount(0)
{
    TX_LOG_ARGS("Stub")
}

/*!
 Destructs music player engine.
 */
MpEngine::~MpEngine()
{
    TX_ENTRY_ARGS("Stub")
}


/*!
    Apply the preset by giving preset index. The index is starting with 0
    which if the "Off". The command then relays to its private implementation.
 */
void MpEngine::applyPreset( int preset )
{
    mApplyPresetCount++;
    mActivePreset = preset;
}

/*!
    Disabling the eqaulizer. The command then relays to its private implementation.
 */
void MpEngine::disableEqualizer()
{
    mDisableEquqlizerCount++;
    mActivePreset = 0;
}

/*!
    Returning currectly activated preset. The command then relays to its private 
    implementation. -1 will be returned if the adaptation is not ready or
    no available preset.
 */
int MpEngine::activePreset()
{
    return mActivePreset;
}

/*!
    Returning the list of availale preset names. The command then relays to its 
    private implementation. 
 */
QStringList MpEngine::presetNames()
{
    QStringList presets;
    presets << "Bass Booster" << "Classic" << "Jazz" << "Pop" << "Rock" << "New Preset";
    return presets;
}
