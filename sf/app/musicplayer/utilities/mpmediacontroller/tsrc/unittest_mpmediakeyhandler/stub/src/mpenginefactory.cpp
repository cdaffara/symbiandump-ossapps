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
* Description: MpEngineFactory stub for testing mpmediakeyhandler
*
*/


#include "stub/inc/mpenginefactory.h"

/*!
 *  Stub function
 */
MpEngine::MpEngine() 
    : iMediaCommandReceived( CmdNone ),
      iMediaPropertyRequested( PropNone )
{
}


/*!
 *  Stub function
 */

void MpEngine::play()
{
    iMediaCommandReceived = CmdPlay;
}

/*!
 *  Stub function
 */
void MpEngine::pause()
{
    iMediaCommandReceived = CmdPause;
}

/*!
 *  Stub function
 */
void MpEngine::playPause()
{
    iMediaCommandReceived = CmdPlayPause;
}

/*!
 *  Stub function
 */
void MpEngine::stop()
{
    iMediaCommandReceived = CmdStop;
}

/*!
 *  Stub function
 */
void MpEngine::skipForward()
{
    iMediaCommandReceived = CmdSkipForward;
}

/*!
 *  Stub function
 */
void MpEngine::startSeekForward()
{
    iMediaCommandReceived = CmdStartSeekForward;
}

/*!
 *  Stub function
 */
void MpEngine::stopSeeking()
{
    iMediaCommandReceived = CmdStopSeeking;
}

/*!
 *  Stub function
 */
void MpEngine::skipBackward()
{
    iMediaCommandReceived = CmdSkipBackward;
}

/*!
 *  Stub function
 */
void MpEngine::startSeekBackward()
{
    iMediaCommandReceived = CmdStartSeekBackward;
}

/*!
 *  Stub function
 */
void MpEngine::getVolumeLevelMax( )
{
    iMediaPropertyRequested = PropVolumeLevelMax;
}

/*!
 *  Stub function
 */
void MpEngine::getVolumeLevel( )
{
    iMediaPropertyRequested = PropVolumeLevel;
}

/*!
 *  Stub function
 */
void MpEngine::increaseVolume()
{
    iMediaCommandReceived = CmdIncreaseVolume;
}

/*!
 *  Stub function
 */
void MpEngine::decreaseVolume()
{
    iMediaCommandReceived = CmdDecreaseVolume;
}

/*!
 *  Stub function
 */
void MpEngine::setVolumeLevel( int /*value*/ )
{
    iMediaCommandReceived = CmdSetVolumeLevel;
}

/*!
 *  Stub function
 */
void MpEngine::getVolumeMuteState( )
{
    iMediaPropertyRequested = PropVolumeMuteState;
}

/*!
 *  Stub function
 */
void MpEngine::muteVolume()
{
    iMediaCommandReceived = CmdMuteVolume;
}

/*!
 *  Stub function
 */
void MpEngine::unmuteVolume()
{
    iMediaCommandReceived = CmdUnmuteVolume;
}


//============= MpEngineFactory =============

/*!
 *  Stub function
 */
MpEngineFactory::MpEngineFactory()
    : mSharedEngine( 0 )
{
}

/*!
 *  Stub function
 */
MpEngineFactory::~MpEngineFactory()
{
    delete mSharedEngine;
    mSharedEngine = 0;
}

/*!
 *  Stub function
 */
MpEngineFactory * MpEngineFactory::instance()
{
    static MpEngineFactory instance;
    return &instance;
}

/*!
 *  Stub function
 */
MpEngine *MpEngineFactory::sharedEngine()
{
    if ( !instance()->mSharedEngine ) {
        instance()->mSharedEngine = new MpEngine();
    }
    return instance()->mSharedEngine;
}

/*!
 *  Stub function
 */
void MpEngineFactory::close()
{
    if ( instance()->mSharedEngine ) {
        delete instance()->mSharedEngine;
        instance()->mSharedEngine = 0;
    }
}
