/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Drive monitor implementation.
*
*/

// INCLUDE FILES
#include <pathinfo.h>
#include <f32file.h>
#include "StoreDriveMonitor.h"
#include <coreapplicationuisdomainpskeys.h>

// ---------------------------------------------------------
// CDriveWatcher::NewL
// Second phase constructor
// ---------------------------------------------------------
//
CStoreDriveMonitor* CStoreDriveMonitor::NewL( MStoreDriveStateObserver& aDriveStateObserver )
	{
	CStoreDriveMonitor* self = CStoreDriveMonitor::NewLC(  aDriveStateObserver );
	CleanupStack::Pop(self);
	return self;
	}
	
// ---------------------------------------------------------
// CDriveWatcher::NewLC
// Second phase constructor
// ---------------------------------------------------------
//
CStoreDriveMonitor* CStoreDriveMonitor::NewLC( MStoreDriveStateObserver& aDriveStateObserver )
	{
	CStoreDriveMonitor* self = new(ELeave) CStoreDriveMonitor( aDriveStateObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;	
	}
	
// ---------------------------------------------------------
// CDriveWatcher::~CDriveWatcher
// Destructor
// ---------------------------------------------------------
//
CStoreDriveMonitor::~CStoreDriveMonitor()
	{	
	
	delete iDriveLetter;
	Cancel();
	
	iFs.Close();
	
	}

// ---------------------------------------------------------
// CDriveWatcher::CDriveWatcher
// Default constructor
// ---------------------------------------------------------
//
CStoreDriveMonitor::CStoreDriveMonitor( MStoreDriveStateObserver& aDriveStateObserver )
	: CActive( CActive::EPriorityStandard ), 
	iDriveStateObserver( aDriveStateObserver )
	{
	}

// ---------------------------------------------------------
// CDriveWatcher::ConstructL
// default Symbian OS constructor
// ---------------------------------------------------------
//
void CStoreDriveMonitor::ConstructL()
	{	
	CActiveScheduler::Add(this);
	User::LeaveIfError( iFs.Connect() );
	}


// ---------------------------------------------------------
// CDriveWatcher::WaitForChangeL()
// Request notification for disk change
// ---------------------------------------------------------
//	
void CStoreDriveMonitor::WaitForChange()
	{
	
	if ( !IsActive() )
	    {
	
	    iFs.NotifyChange( ENotifyEntry, iStatus, *iDriveLetter );

	    SetActive();
	    }
	}
	
// ---------------------------------------------------------
// CDriveWatcher::MediaPresentL()
// Call scanning if the memory card exists
// ---------------------------------------------------------
//	
void CStoreDriveMonitor::MediaPresentL()
	{
	TVolumeInfo volumeInfo;
	// Returns KErrNone, if drive exists
	TInt err=iFs.Volume( volumeInfo, iDrive );
	   
	if ( err == KErrNone )
		{
		iDriveStateObserver.DriveStateChangedL( ETrue );
		}
	else
	    {
        iDriveStateObserver.DriveStateChangedL( EFalse );
	    }
	}

// ---------------------------------------------------------
// Return true if drive is present and false otherwise.
 // ---------------------------------------------------------
 //
TBool CStoreDriveMonitor::IsDrivePresent()
    {
    
    TVolumeInfo volumeInfo;
    // Returns KErrNone, if drive exists
    TInt err=iFs.Volume( volumeInfo, iDrive );
    
    return ( KErrNone == err );
    
    }

// ---------------------------------------------------------
// CDriveWatcher::DoCancel()
// Derived from CActive, cancel request
// ---------------------------------------------------------
//
void CStoreDriveMonitor::DoCancel()
	{
	iFs.NotifyChangeCancel();
	}

// ---------------------------------------------------------
// CDriveWatcher::RunL()
// Derived from CActive, called when notification received
// Check if media is present and call new notification request
// ---------------------------------------------------------
//	
void CStoreDriveMonitor::RunL()
	{	
	if ( iStatus.Int() != KErrCancel )
	    {	    
	    WaitForChange();	   
	    MediaPresentL();
	    }
	}

// ---------------------------------------------------------
// CDriveWatcher::RunError()
// ---------------------------------------------------------
//  

TInt CStoreDriveMonitor::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

// ---------------------------------------------------------
// CStoreDriveMonitor::SetDriveL()
// ---------------------------------------------------------
//
void CStoreDriveMonitor::SetDriveL( TDriveNumber aDrive )
    {
    
    if (iDrive == aDrive)
        {
        //if drives are the same don't need to do nothing
        return;
        }
    else if ( IsActive() )
        {
        //cancel request if drive to monitor changed
        Cancel();
        }
            
    const TUint KDriveBufSize = 2;
    
    iDrive = aDrive;
    
    TChar driveAsChar = 0;
    RFs::DriveToChar( iDrive, driveAsChar );
    
    delete iDriveLetter;
    iDriveLetter = NULL;
    iDriveLetter = HBufC::NewL( KDriveBufSize );
    
    iDriveLetter->Des().Append( driveAsChar );
    iDriveLetter->Des().Append(':');   
    
    }


