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
* Description: MpEngineFactory stub for testing MpNowPlayingWidget
*
*/


#include "mpenginefactory.h"


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
 Returns an instance to an isolated engine with \a mode.
 */
MpEngine *MpEngineFactory::createIsolatedEngine( MpEngine::EngineMode mode )
{
    instance()->mEngines.append( new MpEngine() );
    instance()->mEngines.last()->initialize( (MpCommon::KMusicPlayerUid + instance()->mEngines.count()), mode );
    return instance()->mEngines.last();
}

/*!
 Closes all engines created on this process.
 */
void MpEngineFactory::close()
{
    if ( instance()->mSharedEngine ) {
        delete instance()->mSharedEngine;
        instance()->mSharedEngine = 0;
        
    }
    MpEngine *ptr;
    foreach ( ptr, instance()->mEngines ) {
        delete ptr;
        ptr = 0;
    }
}

/*!
 Returns an instance to an engine with \a clientSecureId, and \a mode, if the shared engine is
 already created parameters are ignored.
 */
MpEngine *MpEngineFactory::createSharedEngine( quint32 clientSecureId , MpEngine::EngineMode mode )
{
    if ( !instance()->mSharedEngine ) {
        instance()->mSharedEngine = new MpEngine();
        instance()->mSharedEngine->initialize( clientSecureId, mode );
    }
    return instance()->mSharedEngine;
}

