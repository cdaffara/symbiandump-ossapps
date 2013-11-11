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
* Description: Music Player song scanner.
*
*/

#include "mpsongscanner.h"
#include "mpmpxharvesterframeworkwrapper.h"
#include "mptrace.h"

/*!
    \class MpSongScanner
    \brief Music Player song scanner.

    Song scanner interfaces with MPX Harvesting Framework to harvest
    music files in the device.
*/

/*!
 \fn void scanStarted()

 This signal is emitted when scanning has started.

 */

/*!
 \fn void scanCountChanged( int count )

 This signal is emitted to notify that scan count has changed.

 */

/*!
 \fn void scanFinished( int error, int itemsAdded )

 Emitted when scanning has finished.

 */

/*!
 Constructs the song scanner.
 */
MpSongScanner::MpSongScanner( MpMpxHarvesterFrameworkWrapper *wrapper, QObject *parent )
    : QObject( parent ),
      mMpxHarvesterWrapper(wrapper),
      mScanning( false ),
      mAutomaticScan( true )
{
    TX_ENTRY
    connect( mMpxHarvesterWrapper, SIGNAL( scanStarted() ),
            this, SLOT( handleScanStarted() ), Qt::QueuedConnection );
    connect( mMpxHarvesterWrapper, SIGNAL( scanEnded( int, int ) ),
            this, SLOT( handleScanEnded( int, int ) ), Qt::QueuedConnection );
    connect( mMpxHarvesterWrapper, SIGNAL( scanCountChanged( int ) ), 
            this, SIGNAL( scanCountChanged( int ) ), Qt::QueuedConnection );
    // Disk events should be handled asap in order to stop the scan on time
    connect( mMpxHarvesterWrapper, SIGNAL( diskEvent( MpxDiskEvents ) ),
            this, SLOT( handleDiskEvent(MpxDiskEvents) ) );
    TX_EXIT
}

/*!
 Destructs the song scanner.
 */
MpSongScanner::~MpSongScanner()
{
    TX_LOG
}

/*!
 Initiates song scanning.
 */
void MpSongScanner::scan( bool automaticScan )
{
    TX_ENTRY
    if ( !mScanning ) {
        mScanning = true;
        mAutomaticScan = automaticScan;
        mMpxHarvesterWrapper->scan();
    }
    TX_EXIT
}

/*!
 Returns mAutomaticScan value.
 */
bool MpSongScanner::isAutomaticScan()
{
    return mAutomaticScan;
}

/*!
 Cancels ongoing song scanning, if any.

 \sa scan()
 */
void MpSongScanner::cancelScan()
{
    TX_ENTRY
    if ( mScanning ) {
        mScanning = false;
        mMpxHarvesterWrapper->cancelScan();
    }
    TX_EXIT
}

/*!
 Slot called upon notification from MPX Harvesting FW indicating start of
 scanning process.
 */
void MpSongScanner::handleScanStarted()
{
    TX_ENTRY
    mScanning = true;
    emit scanStarted();
    TX_EXIT
}

/*!
 Slot called upon notification from MPX Harvesting FW indicating end of
 scanning process.
 */
void MpSongScanner::handleScanEnded( int numItemsAdded, int error )
{
    TX_ENTRY_ARGS("error=" << error << ", numItemsAdded=" << numItemsAdded);
    if (error == KErrDiskFull) {
        emit scanFinished( ScanErrorDiskFull, 0 );
        mScanning = false;
    }
    else{
        if( error < 0) {
            emit scanFinished( ScanGeneralError, numItemsAdded );
        }
        else if ( mScanning ) {
            emit scanFinished( ScanErrorNone, numItemsAdded );
        }
        else {
            // Scan canceled
            emit scanFinished( ScanGeneralError, numItemsAdded );
        }
        mScanning = false;
    }
    TX_EXIT
}

/*!
 Slot to be called when disk event is received from MPX framework.
 */
void MpSongScanner::handleDiskEvent( MpxDiskEvents event )
{
    TX_ENTRY
    Q_UNUSED( event );
    if ( mScanning ) {
        emit scanFinished( ScanInterrupted, 0 );
        mScanning = false;
    }
    TX_EXIT
}

