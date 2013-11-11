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
* Description: MpEngineFactory stub for testing mpplaybackview
*
*/


#include "stub/inc/mpenginefactory.h"
#include "stub/inc/mpplaybackdata.h"
#include "stub/inc/mpengine.h"



/*!
 *  Stub function
 */

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
