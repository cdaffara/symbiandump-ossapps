/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Backup/restore observer implementation.
*
*/



// ========
// INCLUDES
// ========

#include <connect/sbdefs.h>
#include <e32property.h>

#include "BackupRestoreObserver.h"

// =========
// CONSTANTS
// =========

// A backup is in progress if any of these bits are set.
const TUint KBackupInProgressMask = conn::EBURBackupFull | 
				 			        conn::EBURBackupPartial;

// A restore is in progress if any of these bits are set.
const TUint KRestoreInProgressMask = conn::EBURRestoreFull | 
									 conn::EBURRestorePartial;
									 
// ======================
// METHOD IMPLEMENTATIONS
// ======================

// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
CBackupRestoreNotifier* CBackupRestoreNotifier::NewL( MBackupRestoreObserver& aObserver )
	{
    CBackupRestoreNotifier* self = new( ELeave ) CBackupRestoreNotifier( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
	} // end NewL

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CBackupRestoreNotifier::CBackupRestoreNotifier( MBackupRestoreObserver& aObserver ) :
	CActive( EPriorityNormal ),
	iObserver( aObserver ),
	iInProgress( EFalse )
	{
	__LOG_CONSTRUCT( "msg", "CBackupRestoreNotifier" )
	} // end constructor
	
// ==========================================================================
// FUNCTION: ConstructL
// ==========================================================================
void CBackupRestoreNotifier::ConstructL()
	{
	__LOG_ENTER( "ConstructL" );
		
	User::LeaveIfError( iProperty.Attach( KUidSystemCategory, conn::KUidBackupRestoreKey ) );
	CActiveScheduler::Add(this);
	
	// Initial subscription and process current property value.
	RunL();	
	
	__LOG_EXIT
	} // end ConstructL
	
// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CBackupRestoreNotifier::~CBackupRestoreNotifier()
	{
	Cancel();
	iProperty.Close();	
	__LOG_DESTRUCT	
	} // end destructor
	
// ==========================================================================
// FUNCTION: DoCancel
// ==========================================================================
void CBackupRestoreNotifier::DoCancel()
	{
	iProperty.Cancel();		
	} // end DoCancel
	
// ==========================================================================
// FUNCTION: RunL
// ==========================================================================
void CBackupRestoreNotifier::RunL()
	{
	__LOG_ENTER( "RunL" );
	
	// resubscribe before processing new value to prevent missing updates
	iProperty.Subscribe( iStatus );
	SetActive();

	TInt propertyValue = 0;
	
	iProperty.Get( propertyValue );
	
	ProcessEvent( static_cast<TUint>(propertyValue) );
	
	__LOG_EXIT		
	} // end RunL

// ==========================================================================
// FUNCTION: ProcessEventL
// ==========================================================================
void CBackupRestoreNotifier::ProcessEvent( TUint aEvent )
    {
    __LOG_ENTER( "ProcessEventL" )
    __LOG_WRITE8_FORMAT1_INFO( "event=%i", aEvent )
    
	TBool newInProgress;
	TBool isARestore = EFalse;
	
	// If the property exists & one of the backup/restore bits are set...
	if( (aEvent & KBackupInProgressMask) > 0 )
		{
		__LOG_WRITE_INFO( "backup in progress" );
		newInProgress = ETrue;
		}
	else if( (aEvent & KRestoreInProgressMask) > 0 )
		{
		__LOG_WRITE_INFO( "restore in progress" );
		newInProgress = ETrue;
		isARestore    = ETrue;
		}
	else
		{
		__LOG_WRITE_INFO( "backup or restore not in progress" );
		newInProgress = EFalse;
		}
		
	if( newInProgress )
		{
		__LOG_WRITE_INFO( "notifying observer" );
		iObserver.BackupOrRestoreInProgress( isARestore );		
		}
	else if( !newInProgress && iInProgress )
		{
		__LOG_WRITE_INFO( "notifying observer" );
		iObserver.BackupOrRestoreCompleted();
		} // end if
		
	iInProgress = newInProgress;
	
	__LOG_EXIT
    } // end ProcessEvent
