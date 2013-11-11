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
* Description: MpSettingsManager stub for testing MpMtpInfoLink.
*
*/

#include "stub/inc/mpsettingsmanager.h"


/*!
 Constructs the MP Settings Manager.
 */
MpSettingsManager::MpSettingsManager() :
    mShuffle(true),
    mRepeat(true)
{
}

/*!
 Destructs the settings manager.
 */
MpSettingsManager::~MpSettingsManager()
{
}

/*!
 Returns the singleton instance to the settings manager.
 */
MpSettingsManager * MpSettingsManager::instance()
{
    static MpSettingsManager instance;
    return &instance;
}

/*!
 Stub function.
 */
void MpSettingsManager::setShuffle( bool shuffle )
{
    instance()->mShuffle = shuffle;
}

/*!
 Stub function.
 */
void MpSettingsManager::setRepeat( bool repeat )
{
    instance()->mRepeat = repeat;
}

/*!
 Stub function.
 */
bool MpSettingsManager::shuffle()
{
    return instance()->mShuffle;
}

/*!
 Stub function.
 */
bool MpSettingsManager::repeat()
{
    return instance()->mRepeat;
}

