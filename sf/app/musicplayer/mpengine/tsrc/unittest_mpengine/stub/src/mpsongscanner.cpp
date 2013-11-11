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
* Description: stub for song scanner.
*
*/

#include "mpsongscanner.h"
#include "mpmpxharvesterframeworkwrapper.h"

/*!
 Constructs the song scanner.
 */
MpSongScanner::MpSongScanner( MpMpxHarvesterFrameworkWrapper */*wrapper*/, QObject *parent )
    : QObject( parent )
{

}

/*!
 Destructs the song scanner.
 */
MpSongScanner::~MpSongScanner()
{
}

/*!
 Initiates song scanning.
 */
void MpSongScanner::scan( bool /*automaticScan*/ )
{

}
