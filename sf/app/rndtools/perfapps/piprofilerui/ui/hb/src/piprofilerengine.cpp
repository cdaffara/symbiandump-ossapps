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


#include "piprofilerengine.h"
#include "piprofilerengineprivate.h"

PIProfilerEngine::PIProfilerEngine()
{

}

PIProfilerEngine::~PIProfilerEngine()
{
    if (mPrivate != 0) {
        delete mPrivate;
        mPrivate = 0;
    }
}

bool PIProfilerEngine::init()
{
    mPrivate = new PIProfilerEnginePrivate(this);
    return mPrivate->Init();
}

void PIProfilerEngine::getGeneralSettings(GeneralAttributes &settings)
{
    mPrivate->GetGeneralSettings(settings);
}

void PIProfilerEngine::notifyUIReady()
{
    mPrivate->NotifyUIReady();
}

bool PIProfilerEngine::saveGeneralSettings(GeneralAttributes &settings)
{
    return mPrivate->SaveGeneralSettings(settings);

}

bool PIProfilerEngine::savePluginSettings(const PluginAttributes &pluginAttributes)
{
    return mPrivate->SavePluginSettings(pluginAttributes);
}

bool PIProfilerEngine::startProfiling()
{
    return mPrivate->StartProfiling();
}
bool PIProfilerEngine::startTimedProfiling()
{
    return mPrivate->StartTimedProfiling();
}
void PIProfilerEngine::stopProfiling()
{
    return mPrivate->StopProfiling();
}

int PIProfilerEngine::getTimeLimit()
{
    return mPrivate->GetTimeLimit();
}

void PIProfilerEngine::leaveProfilingOnAfterClosing()
{
    mPrivate->LeaveProfilingOnAfterClosing();
}

bool PIProfilerEngine::checkTraceLocationSanity(QString &location)
{
    mPrivate->CheckTraceLocationSanity(location);
}
