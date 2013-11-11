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
* Description: Music Player media key handler.
*
*/

#include "mpmediakeyhandler.h"
#include "mpmediakeyhandler_p.h"
#include "mptrace.h"

/*!
 Constructs MpMediaKeyHandler.
 */
MpMediaKeyHandler::MpMediaKeyHandler( QObject *parent )
    : QObject(parent)
{
    TX_ENTRY
    d_ptr = new MpMediaKeyHandlerPrivate(this);
    d_ptr->init();
    TX_EXIT
}

/*!
 Destructs MpMediaKeyHandler.
 */
MpMediaKeyHandler::~MpMediaKeyHandler()
{
    TX_ENTRY
    delete d_ptr;
    TX_EXIT
}

