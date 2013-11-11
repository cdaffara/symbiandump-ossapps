/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*     CMsgErrorDiskSpaceObserver implementation file
*
*/



// INCLUDE FILES
#include    <e32base.h>

#include    "msgerrorwatcher_p.h"
#include    "msgerrordiskspaceobserver.h"
#include    "debugtraces.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgErrorDiskSpaceObserver::CMsgErrorDiskSpaceObserver
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgErrorDiskSpaceObserver::CMsgErrorDiskSpaceObserver( 
    CMsgErrorWatcherPrivate& aWatcher, 
        CMsvSession& aSession, 
        RFs& aFs )
    : CActive( CActive::EPriorityStandard ),
    iWatcher( aWatcher ),
    iSession( aSession ),
    iFs( aFs ),
    iLimit( 0 )
    {
    QDEBUG_WRITE("CMsgErrorDiskSpaceObserver:CMsgErrorDiskSpaceObserver : Enter");
    CActiveScheduler::Add( this );
    QDEBUG_WRITE("CMsgErrorDiskSpaceObserver:CMsgErrorDiskSpaceObserver : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorDiskSpaceObserver::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgErrorDiskSpaceObserver* CMsgErrorDiskSpaceObserver::NewL( 
    CMsgErrorWatcherPrivate& aWatcher, 
        CMsvSession& aSession, 
        RFs& aFs )
    {
    QDEBUG_WRITE("CMsgErrorDiskSpaceObserver:NewL : Enter");
    CMsgErrorDiskSpaceObserver* self = new ( ELeave )
        CMsgErrorDiskSpaceObserver( aWatcher, aSession, aFs );
    QDEBUG_WRITE("CMsgErrorDiskSpaceObserver:NewL : Exit");
    return self;
    }

    
// ---------------------------------------------------------
// CMsgErrorDiskSpaceObserver::~CMsgErrorDiskSpaceObserver
//
// Destructor
// ---------------------------------------------------------
//
CMsgErrorDiskSpaceObserver::~CMsgErrorDiskSpaceObserver()
    {
    QDEBUG_WRITE("CMsgErrorDiskSpaceObserver:~CMsgErrorDiskSpaceObserver : Enter");
    Cancel();
    QDEBUG_WRITE("CMsgErrorDiskSpaceObserver:~CMsgErrorDiskSpaceObserver : Exit");
    }


// ---------------------------------------------------------
// CMsgErrorDiskSpaceObserver::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorDiskSpaceObserver::DoCancel()
    {
    QDEBUG_WRITE("CMsgErrorDiskSpaceObserver:DoCancel : Enter");
    iFs.NotifyDiskSpaceCancel( iStatus );
    QDEBUG_WRITE("CMsgErrorDiskSpaceObserver:DoCancel : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorDiskSpaceObserver::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorDiskSpaceObserver::RunL()
    {
    QDEBUG_WRITE("CMsgErrorDiskSpaceObserver:RunL : Enter");
    QDEBUG_WRITE_FORMAT("DiskSpace: RunL, iStatus: ", iStatus.Int() );
    TInt status = iStatus.Int();
    if ( status < 0 )
        {
        //Reset the observer.
        TRAP_IGNORE ( SetLimitAndActivateL( iLimit ) );
        return;
        }
    
    //We shouldn't have to check whether we're above the limit,
    //because we were below when we activated and the event comes
    //when we cross the limit

    TRAP_IGNORE( iWatcher.HandleDiskSpaceEventL() );
    QDEBUG_WRITE("CMsgErrorDiskSpaceObserver:RunL : Exit");
    }

// ---------------------------------------------------------
// CMsgErrorDiskSpaceObserver::SetLimitAndActivateL
//
// Sets the disk space trigger level for active object
// and activates it.
// ---------------------------------------------------------
//
void CMsgErrorDiskSpaceObserver::SetLimitAndActivateL( TInt32 aLimit )
    {
    QDEBUG_WRITE("CMsgErrorDiskSpaceObserver:SetLimitAndActivateL : Enter");
    QDEBUG_WRITE_FORMAT("DiskSpace: aLimit ", aLimit );
    __ASSERT_DEBUG( aLimit >= 0, User::Leave( KErrArgument ) );
    if ( aLimit < 0 )
        {
        return;
        }
    if ( iLimit == 0 || iLimit > aLimit )
        {
        Cancel();
        iLimit = aLimit;

        TInt currentDrive = TInt( iSession.CurrentDriveL() );
        TVolumeInfo volInfo;
        User::LeaveIfError( iFs.Volume( volInfo, currentDrive ) );
        QDEBUG_WRITE_FORMAT("DiskSpace: free/hi : ", I64HIGH( volInfo.iFree ) );
        QDEBUG_WRITE_FORMAT("DiskSpace: free/lo : ", I64LOW( volInfo.iFree ) );
        //check if we're already above the limit
        if ( I64HIGH( volInfo.iFree ) != 0 ||
            I64LOW( volInfo.iFree ) > (TUint)iLimit ) //iLimit is always positive.
            {
            iWatcher.HandleDiskSpaceEventL();
            return;
            }
        else
            {
            QDEBUG_WRITE( "notify activated" );
            TInt64 treshold = iLimit;
            iFs.NotifyDiskSpace( treshold, currentDrive, iStatus );
            // No need to check IsActive since object is cancelled 
            // anyway above
            SetActive();
            }
        }
    QDEBUG_WRITE("CMsgErrorDiskSpaceObserver:SetLimitAndActivateL : Exit");
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  

