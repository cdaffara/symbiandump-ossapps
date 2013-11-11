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
* Description:
*
*/

#include <e32def.h>
#include <e32cmn.h>

#include "enginewrapper.h"


// ---------------------------------------------------------------------------

EngineWrapper::EngineWrapper() : mSettings(iSettings)
{
}

// ---------------------------------------------------------------------------

EngineWrapper::~EngineWrapper()
{
    finalize();
}

// ---------------------------------------------------------------------------

bool EngineWrapper::initialize()
{
    TInt err = KErrNone;

    TRAP(err, ConstructL());
    if (err != KErrNone)
        return false;

    TRAP(err, ActivateEngineL());
    if (err != KErrNone)
        return false;

    createSampleEntriesArray();

    return true;
}

// ---------------------------------------------------------------------------

int EngineWrapper::SampleEntryPosToSettingPos(int entryPos) const
{
    int settingPos(0); // return position of aSampleEntryPos in settings

    if (entryPos >= iCPU0PositionInSamples && entryPos < iRAMPositionInSamples)
    {
        settingPos = ESourceCPU;
    }
    else if (entryPos == iRAMPositionInSamples)
    {
        settingPos = ESourceRAM;
    }
    else
    {
        settingPos = ESourceC + (entryPos - iCDrivePositionInSamples);
    }

    return settingPos;
}

// ---------------------------------------------------------------------------

void EngineWrapper::createSampleEntriesArray()
{
    for (TInt i=0; i < iSampleEntryArray->Count(); i++)
    {
        mEntries.append(iSampleEntryArray->At(i));
    }
}

// ---------------------------------------------------------------------------

void EngineWrapper::finalize()
{
    TRAP_IGNORE(DeActivateEngineL());
}

// ---------------------------------------------------------------------------

bool EngineWrapper::updateSettings()
{
    TInt err = KErrNone;

    TRAP(err, SaveSettingsL());
    if (err != KErrNone)
        return false;

    TRAP(err, HandleSettingsChangeL());
    if (err != KErrNone)
        return false;

    emit settingsUpdated();
    return true;
}

// ---------------------------------------------------------------------------

void EngineWrapper::setLoggingEnabled(bool enabled)
{
    EnableLogging(enabled);    
}

// ---------------------------------------------------------------------------

void EngineWrapper::SendDrawEventToContainersL()
{
    emit samplesUpdated();
}

// ---------------------------------------------------------------------------
