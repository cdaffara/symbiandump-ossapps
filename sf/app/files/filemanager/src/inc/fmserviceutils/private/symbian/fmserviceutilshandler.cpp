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
* 
* Description:
*     The source file of service utils handler
*/



// INCLUDE FILES
#include "fmserviceutilshandler.h"
#include <coreapplicationuisdomainpskeys.h>
#include <coemain.h>
#include <apgwgnam.h>
#include <apgtask.h>
#include <bautils.h>
#include <tz.h>
#include <babackup.h>
#include <pathinfo.h>
#include <sysutil.h>
#include <driveinfo.h>
#include <e32property.h>
#include <centralrepository.h>

#include "fmutils.h"
#include <QSettings>
#include <XQConversions>

// CONSTANTS
const TInt KAppCloseTimeout = 1000000;

/*!
    constructor.
*/
CFmServiceUtilsHandler::CFmServiceUtilsHandler() :
    CActive( CActive::EPriorityStandard )
{
}

/*!
    Two-phased constructor.
*/
void CFmServiceUtilsHandler::ConstructL()
{
    CActiveScheduler::Add( this );
}

/*!
    Two-phased constructor.
*/
CFmServiceUtilsHandler* CFmServiceUtilsHandler::NewL()
    {
    RFs& fs( CCoeEnv::Static()->FsSession() );
    CFmServiceUtilsHandler* self =
        new( ELeave ) CFmServiceUtilsHandler();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

/*!
    destructor.
*/
CFmServiceUtilsHandler::~CFmServiceUtilsHandler()
{
    Cancel();
    if( iBSWrapper ) {
        delete iBSWrapper;
        iBSWrapper = 0;
    }
}

/*
    \sa CActive
*/
void CFmServiceUtilsHandler::DoCancel()
{
}

/*
    \sa CActive
*/ 
void CFmServiceUtilsHandler::RunL()
    {
    iWait.AsyncStop();
    TInt err( iStatus.Int() );
    }

/*
    \sa CActive
*/
TInt CFmServiceUtilsHandler::RunError( TInt /*aError*/ )
{
    iWait.AsyncStop();
    return KErrNone;
}

/*
    Call CBaBackupSessionWrapper to close apps
    this is synchronous which will take a while, please call this function in thread
*/
void CFmServiceUtilsHandler::CloseAppsL()
{
    if( iBSWrapper ) {
        delete iBSWrapper;
        iBSWrapper = 0;
    }

    iBSWrapper = CBaBackupSessionWrapper::NewL();

    TBackupOperationAttributes atts(
        MBackupObserver::EReleaseLockNoAccess,
        MBackupOperationObserver::EStart );
    iBSWrapper->NotifyBackupOperationL( atts );
    iBSWrapper->CloseAll( MBackupObserver::EReleaseLockNoAccess, iStatus );
    SetActive();

    // Memory card formatting cannot be executed if there are open files on it.
    // It has been detected, that in some cases memory card using applications 
    // have no time to close file handles before formatting is tried to be executed. 
    // To address this issue, we need to add a delay here after client-notification 
    // about pending format and real formatting procedure.
    User::After( KAppCloseTimeout );
    StartWait();
}

/*
    Call CBaBackupSessionWrapper to restart closed apps
    this is synchronous which will return quickly.
*/
void CFmServiceUtilsHandler::RestartAppsL()
{
    if ( !iBSWrapper )
        {
        return;
        }
    
    TBackupOperationAttributes atts(
        MBackupObserver::ETakeLock, MBackupOperationObserver::EEnd );
    iBSWrapper->NotifyBackupOperationL( atts );
    iBSWrapper->RestartAll();
    
    // Get rid of the wrapper instance
    delete iBSWrapper;
    iBSWrapper = 0;
}

/*
    wait till request returned in RunL
*/
void CFmServiceUtilsHandler::StartWait()
{
    if ( iWait.IsStarted() != (TInt)ETrue )
    {
        iWait.Start();
    }
}

//  End of File  
