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
* Description:
*
*/
#include "irqphononadapter.h"

// ---------------------------------------------------------------------------
//  IRQPHONONAdapter::IRQMMFAdapter
//  Constructor
//  Initialize viriants
// ---------------------------------------------------------------------------
//
IRQPHONONAdapter::IRQPHONONAdapter()
{
    iPlayState = EStopped;
}

// ---------------------------------------------------------------------------
//  IRQPHONONAdapter::~IRQMMFAdapter
//  Destructor
//  Clean up resources
// ---------------------------------------------------------------------------
//
IRQPHONONAdapter::~IRQPHONONAdapter()
{
}

// ---------------------------------------------------------------------------
//  IRQPHONONAdapter::playStation
//  IRQPlayerAdapterInterface method
//  Play url via specific access point id
// ---------------------------------------------------------------------------
//
void IRQPHONONAdapter::playStation(const QString &aUrl, int aApId)
{
    Q_UNUSED(aUrl);
    Q_UNUSED(aApId);
}

// ---------------------------------------------------------------------------
//  IRQPHONONAdapter::stop
//  IRQPlayerAdapterInterface method
//  Stop playback, call Close() to clean up allocated resources
// ---------------------------------------------------------------------------
//
void IRQPHONONAdapter::stop()
{
}

// ---------------------------------------------------------------------------
//  IRQPHONONAdapter::setVolume
//  IRQPlayerAdapterInterface method
//  Set volume to player
// ---------------------------------------------------------------------------
//
void IRQPHONONAdapter::setVolume(int aVolume)
{
    Q_UNUSED(aVolume);
}

// ---------------------------------------------------------------------------
//  IRQPHONONAdapter::getVolume
//  IRQPlayerAdapterInterface method
//  Get current volume value from player
// ---------------------------------------------------------------------------
//
int IRQPHONONAdapter::getVolume()
{
    return 0;
}

// ---------------------------------------------------------------------------
//  IRQPHONONAdapter::getPlayerInstance
//  IRQPlayerAdapterInterface method
//  Get audio player instance
// ---------------------------------------------------------------------------
//
void* IRQPHONONAdapter::getPlayerInstance()
{
    return NULL;
}

void IRQPHONONAdapter::enableStereoEffect()
{
    
}

void IRQPHONONAdapter::disableStereoEffect()
{
    
}
