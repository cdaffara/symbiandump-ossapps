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
* Description: Stub class for mpsongscanner unittest.
*
*/

#include "stub/inc/mpmpxharvesterframeworkwrapper.h"

/*!
 Stub function.
 */
MpMpxHarvesterFrameworkWrapper::MpMpxHarvesterFrameworkWrapper( quint32 clientSecureId, QObject *parent )
    : QObject( parent ),
      mScanRequested( false ),
      mScanning( false ),
      mScanCount( -1 ),
      mCheckSystemEventsRequested( false )
{
    Q_UNUSED( clientSecureId );
}


/*!
 Stub function.
 */
MpMpxHarvesterFrameworkWrapper::~MpMpxHarvesterFrameworkWrapper()
{
}

/*!
 Stub function.
 */
void MpMpxHarvesterFrameworkWrapper::scan()
{
    mScanRequested = true;
}

/*!
 Stub function.
 */
void MpMpxHarvesterFrameworkWrapper::cancelScan()
{
    mScanRequested = false;
}

/*!
 Stub function.
 */
void MpMpxHarvesterFrameworkWrapper::checkForSystemEvents()
{
    mCheckSystemEventsRequested = true;
}

/*!
 Stub function.
 */
void MpMpxHarvesterFrameworkWrapper::emitScanStarted()
{
    mScanning = true;
    emit scanStarted();
}

/*!
 Stub function.
 */
void MpMpxHarvesterFrameworkWrapper::emitScanEnded( int count, int error )
{
    mScanning = false;
    emit scanEnded( count, error );
}

/*!
 Stub function.
 */
void MpMpxHarvesterFrameworkWrapper::emitScanCountChanged( int count )
{
    emit scanCountChanged( count );
}

/*!
 Stub function.
 */
void MpMpxHarvesterFrameworkWrapper::emitDiskEvent()
{
    emit diskEvent( DiskInserted );
}
