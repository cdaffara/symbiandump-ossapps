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
* Description: stub for media key handler.
*
*/

#include "mpmediakeyhandler.h"

// ---------------------------------------------------------------------------
// MpMediaKeyHandler()
// ---------------------------------------------------------------------------
//
MpMediaKeyHandler::MpMediaKeyHandler( QObject *parent )
    : QObject(parent)
{

}

// ---------------------------------------------------------------------------
// ~MpMediaKeyHandler()
// ---------------------------------------------------------------------------
//
MpMediaKeyHandler::~MpMediaKeyHandler()
{

}

// ---------------------------------------------------------------------------
// setEnabled()
// ---------------------------------------------------------------------------
//
void MpMediaKeyHandler::setEnabled( bool enable )
{
    mEnable = enable;
}

