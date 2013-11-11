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
* Description: Music Player application monitor utility - private implementation.
*
*/

#include <apgtask.h>
#include <AknTaskList.h>

#include "mpapplicationmonitor_p.h"
#include "mpapplicationmonitor.h"
#include "mptrace.h"

// -1 means the window will never be visible.
const TInt KWindowGroupPriority = -1;

/*!
    \class MpApplicationMonitorPrivate
    \brief Music Player application monitor utility - private implementation.


*/

/*!
 Constructor
 */
MpApplicationMonitorPrivate::MpApplicationMonitorPrivate( MpApplicationMonitor *qq )
    : CActive( CActive::EPriorityStandard ),
      q_ptr( qq )
{
    TX_ENTRY_ARGS( "Q pointer=" << ( void * )qq )
    CActiveScheduler::Add( this );
    TX_EXIT
}

/*!
 Destructor
 */
MpApplicationMonitorPrivate::~MpApplicationMonitorPrivate()
{
    TX_LOG
    // Cancel any outstanding request
    Cancel();

    iWindowGroup.Close();
    iWsSession.Close();
    TX_EXIT
}

/*!
 \internal
 */
void MpApplicationMonitorPrivate::init( quint32 clientSecureId )
{
    TX_ENTRY
    iHostUid = TUid::Uid( clientSecureId );
    TRAPD( err, DoInitL() );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
    TX_EXIT
}

/*!
 Request if music player application is running.
 */
bool MpApplicationMonitorPrivate::isApplicationRunning()
{
    TX_ENTRY
    bool running = false;
    TRAPD( err, running = DoIsApplicationRunningL() );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
    TX_EXIT
    return running;
}

/*!
 \internal
 Checks if the chosen app is launched
 */
void MpApplicationMonitorPrivate::RunL()
    {
    // Get the status of this object.
    const TInt errCode( iStatus.Int() );
    TX_ENTRY_ARGS("Status=" << errCode);

    if ( KErrNone == errCode ) {
        TWsEvent event;
        iWsSession.GetEvent( event );
        if ( event.Type() == EEventWindowGroupsChanged ) {
            emit q_ptr->applicationStatusChanged( isApplicationRunning() );
        }
        TX_LOG_ARGS("OK, renewing request");
        StartMonitor();
    }
    else if ( KErrCancel != errCode ) {
        TX_LOG_ARGS("Renewing request after error: " << errCode);
        StartMonitor();
    }
    else {
        TX_LOG_ARGS("cancelled");
    }
    TX_EXIT
}

/*!
 \internal
 Cancel the outstanding request.
 */
void MpApplicationMonitorPrivate::DoCancel()
{
    TX_ENTRY
    iWsSession.EventReadyCancel();
    TX_EXIT
}

/*!
 \internal
 */
void MpApplicationMonitorPrivate::DoInitL()
{
    TX_ENTRY
    CreateWindowGroupL();
    StartMonitor();
    TX_EXIT
}

/*!
 \internal
 */
bool MpApplicationMonitorPrivate::DoIsApplicationRunningL()
{
    TX_ENTRY
    bool taskExists = false;
    CAknTaskList* taskList = CAknTaskList::NewL( iWsSession );
    TApaTask task = taskList->FindRootApp( iHostUid );
    delete taskList;
    taskExists = task.Exists();
    TX_LOG_ARGS("taskExists=" << taskExists);
    return taskExists;
}

/*!
 \internal
 Creates a window group and hides it from the UI.
 */
void MpApplicationMonitorPrivate::CreateWindowGroupL()
{
    TX_ENTRY
    User::LeaveIfError( iWsSession.Connect() );
    RWindowGroup wg( iWsSession );
    // Make a handle from the address of window group object
    User::LeaveIfError( wg.Construct( reinterpret_cast<TUint32>( &wg ) ) );
    User::LeaveIfError( wg.EnableGroupChangeEvents() );

    // Get the ordinal position of this window group
    const TInt ordPos( wg.OrdinalPosition() );
    // Set priority so it's hidden in the UI
    wg.SetOrdinalPosition( ordPos, KWindowGroupPriority );

    // Needs to be a member variable. Must be closed in destructor.
    // Otherwise can't monitor what has been opened.
    iWindowGroup = wg;

    iWsSession.Flush();
    TX_EXIT
}

/*!
 \internal
 Starts event monitoring.
 */
void MpApplicationMonitorPrivate::StartMonitor()
{
    TX_ENTRY
    Cancel();

    iWsSession.EventReady( &iStatus );
    SetActive();
    TX_EXIT
}

