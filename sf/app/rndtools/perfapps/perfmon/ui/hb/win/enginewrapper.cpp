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

#include "enginewrapper.h"


// ---------------------------------------------------------------------------

EngineWrapper::EngineWrapper()
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

    SampleEntry cpu;
    cpu.mDescription = "CPU";
    cpu.mDriveNumber = -1;
    cpu.mGraphColor.setNamedColor("yellow");

    SampleEntry ram;
    ram.mDescription = "RAM";
    ram.mUnitShort = "b";
    ram.mUnitLong = "bytes";
    ram.mDriveNumber = -1;
    ram.mGraphColor.setNamedColor("green");

    SampleEntry cdrive;
    cdrive.mDescription = "C:";
    cdrive.mUnitShort = "b";
    cdrive.mUnitLong = "bytes";
    cdrive.mDriveNumber = 0;
    cdrive.mGraphColor.setNamedColor("cyan");

    SampleEntry ddrive;
    ddrive.mDescription = "D:";
    ddrive.mUnitShort = "b";
    ddrive.mUnitLong = "bytes";
    ddrive.mDriveNumber = 0;
    ddrive.mGraphColor.setNamedColor("blue");

    mEntries << cpu << ram << cdrive << ddrive;

    connect(&mTimer, SIGNAL(timeout()), this, SLOT(update()));
    mStartTime = QTime::currentTime();
    mTimer.start(600);

    return true;
}

// ---------------------------------------------------------------------------

void EngineWrapper::finalize()
{
}

// ---------------------------------------------------------------------------

bool EngineWrapper::updateSettings()
{
    emit settingsUpdated();
    return true;
}

void EngineWrapper::update()
{
    // do some updates :)
    for (int i=0; i<mEntries.length(); i++)
    {
        SampleEntry & entry = mEntries[i];
        if (entry.mSampleData.length() > 0)
        {
            SampleData data = entry.mSampleData.at(0);
            data.mFree += 10000 * (qrand() % 1000 - 500);
            data.mTimeFromStart = mStartTime.msecsTo(QTime::currentTime()) * 1000;
            entry.mSampleData.prepend(data);
        }
        else
        {
            SampleData data = {100000000L, 200000000L, mStartTime.msecsTo(QTime::currentTime()) * 1000};
            entry.mSampleData.prepend(data);
        }

        if (entry.mSampleData.length() > 64)
        {
            entry.mSampleData.removeLast();
        }
    }

    emit samplesUpdated();
}
