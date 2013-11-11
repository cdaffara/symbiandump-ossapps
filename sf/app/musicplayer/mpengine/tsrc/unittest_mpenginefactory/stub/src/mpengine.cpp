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
* Description: Wrapper for mpengine.
*
*/

#include "mpengine.h"

MpEngine::MpEngine()
{

}

/*!
 Destructs music player engine.
 */
MpEngine::~MpEngine()
{

}

/*!
 Initialize engine
 */
void MpEngine::initialize( quint32 clientSecureId, EngineMode mode )
{
    Q_UNUSED( clientSecureId );
    Q_UNUSED( mode );
}
