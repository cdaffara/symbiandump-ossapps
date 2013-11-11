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
* Description: Stub for MpEngine for testing mpsettingsaudioeffecswidget
*
*/

#include "mpengine.h"
#include "mptrace.h"


/*!
 Stub
 */
MpEngine::MpEngine()
    : mBalance(0),
    mLoudness(0)
{
    TX_LOG_ARGS("Stub")
}

/*!
 Stub
 */
MpEngine::~MpEngine()
{
    TX_ENTRY_ARGS("Stub")
}

// Audio Effects related
/*!
 Stub
 */
int MpEngine::balance()
{
    return mBalance;
}

/*!
Stub
 */
bool MpEngine::loudness()
{
    return mLoudness;
}


/*!
Stub
 */
void MpEngine::setBalance( int balance )
{
     mBalance=balance;
}

/*!
Stub
 */
void MpEngine::setLoudness( bool mode )
{
    mLoudness=mode;
}
