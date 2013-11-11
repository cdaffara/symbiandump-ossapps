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
* Description:  Mmc monitor
*
*/


// INCLUDE FILES
#include "mmcmonitor.h"
#include "harvesterserverlogger.h"

#include <pathinfo.h>
#include <s32file.h>
#include <f32file.h> // TDriveNumber
#include <driveinfo.h> // TDriveInfo

#include <UikonInternalPSKeys.h>

// CONSTANTS
// 

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMMCMonitorUtil::NewL
// -----------------------------------------------------------------------------
//
CMMCMonitorUtil* CMMCMonitorUtil::NewL( MMMCEventObserver* aObserver)
    {
    CMMCMonitorUtil* self = new ( ELeave ) CMMCMonitorUtil( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CMMCMonitorUtil::CMMCMonitorUtil
// -----------------------------------------------------------------------------
//
CMMCMonitorUtil::CMMCMonitorUtil( MMMCEventObserver* aObserver )
    : CActive( CActive::EPriorityStandard ),
      iObserver( aObserver )
    {
    CPIXLOGSTRING("ENTER CMMCMonitorUtil::CMMCMonitorUtil");
    CActiveScheduler::Add( this );
    CPIXLOGSTRING("END CMMCMonitorUtil::CMMCMonitorUtil");
    }


// -----------------------------------------------------------------------------
// CMMCMonitorUtil::~CMMCMonitorUtil
// -----------------------------------------------------------------------------
//
CMMCMonitorUtil::~CMMCMonitorUtil()
    {
    CPIXLOGSTRING("ENTER ~CMMCMonitorUtil");
    Cancel();
    iProperty.Close();
    CPIXLOGSTRING("END ~CMMCMonitorUtil");
    }


// -----------------------------------------------------------------------------
// CMMCMonitorUtil::ConstructL
// -----------------------------------------------------------------------------
//
void CMMCMonitorUtil::ConstructL()
    {
    CPIXLOGSTRING("ENTER CMMCMonitorUtil::ConstructL Foobar");
    User::LeaveIfError(iFsSession.Connect());    
    TInt error = iProperty.Attach( KPSUidUikon, KUikMMCInserted );
    if ( error != KErrNone ) CPIXLOGSTRING("END CMMCMonitorUtil::Attach to MMCInserted failed");
    
    error = iProperty.Get( KPSUidUikon, KUikMMCInserted, iMmcStatus );
    if ( error != KErrNone ) 
    	{
		CPIXLOGSTRING("CMMCMonitorUtil::Get MMCInserted failed");
    	} 
    else if ( iMmcStatus ) 	 
    	{
    	CPIXLOGSTRING("CMMCMonitorUtil::MMC card is in");
    	}
    else 
    	{
    	CPIXLOGSTRING("CMMCMonitorUtil::no MMC card");
    	}
    CPIXLOGSTRING("END CMMCMonitorUtil::ConstructL");
    }


// -----------------------------------------------------------------------------
// CMMCMonitorUtil::StartMonitoring
// -----------------------------------------------------------------------------
//
TBool CMMCMonitorUtil::StartMonitoring()
    {
    CPIXLOGSTRING("ENTER CMMCMonitorUtil::StartMonitoring");
    //Subscribe for property value change once modified get update      
    iStatus = KRequestPending;
    iProperty.Subscribe( iStatus );
    SetActive();
    CPIXLOGSTRING("END CMMCMonitorUtil::StartMonitoring");
    return ETrue;
    }


// -----------------------------------------------------------------------------
// CMMCMonitorUtil::MmcStatus
// -----------------------------------------------------------------------------
//
TBool CMMCMonitorUtil::MmcStatus( TInt aDriveNumber )
    {
    TBool isMmcPresent(EFalse);

    if ( iFsSession.IsValidDrive( aDriveNumber ) )
        {
        TUint drvStatus( 0 );
        TInt err = DriveInfo::GetDriveStatus( iFsSession, aDriveNumber, drvStatus );
        if ( err )
            {
            return EFalse;
            }
        // MMC drives are removable and user visible
        if ( ( drvStatus & DriveInfo::EDriveRemovable ) &&
             ( drvStatus & DriveInfo::EDriveUserVisible ) )
                {
                CPIXLOGSTRING2("CMMCMonitorUtil::MmcStatus Drive Number %d", aDriveNumber);
                isMmcPresent = ETrue;
                }
        }
    return isMmcPresent;
    }


// -----------------------------------------------------------------------------
// CMMCMonitorUtil::RunError
// -----------------------------------------------------------------------------
//
TInt CMMCMonitorUtil::RunError( TInt aError )
    {
    CPIXLOGSTRING2("CMMCMonitorUtil::RunError Error:",aError);
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMMCMonitorUtil::DoCancel
// -----------------------------------------------------------------------------
//
void CMMCMonitorUtil::DoCancel()
    {
    CPIXLOGSTRING("ENTER CMMCMonitorUtil::DoCancel");
    iProperty.Cancel();
    CPIXLOGSTRING("END CMMCMonitorUtil::DoCancel");
    }


// -----------------------------------------------------------------------------
// CMMCMonitorUtil::RunL
// -----------------------------------------------------------------------------
//
void CMMCMonitorUtil::RunL()
    {
    CPIXLOGSTRING("ENTER CMMCMonitorUtil::RunL");
    iProperty.Subscribe( iStatus );
    SetActive();
    User::LeaveIfError( iProperty.Get( KPSUidUikon, KUikMMCInserted, iMmcStatus ) );
    
    if ( iMmcStatus )
    	{
    	CPIXLOGSTRING("CMMCMonitorUtil::MMC card is in");
    	}
    else 
    	{
		CPIXLOGSTRING("CMMCMonitorUtil::no MMC card");
    	}

    for ( TInt driveNumber = EDriveA; driveNumber <= EDriveZ; driveNumber++ )
        {
        const TBool foundMmc = MmcStatus( driveNumber );
        if ( !foundMmc )
            {
            continue;
            }

        // This drive has been recognized as MMC. 
        TDriveNumber drv = TDriveNumber( driveNumber );

        TUint drvStatus( 0 );

        const TInt err = DriveInfo::GetDriveStatus( iFsSession, driveNumber, drvStatus );
        if ( err ) 
            {
            continue;  // should not happen
            }

        if ( drvStatus & DriveInfo::EDrivePresent )
            {
            CPIXLOGSTRING("CMMCMonitorUtil::RunL insert event");
            // Notify observer for handleing event
            iObserver->HandleMMCEventL(drv,ETrue);
            }
        else
            {
            CPIXLOGSTRING("CMMCMonitorUtil::RunL eject event");
            // If the MMC has been ejected, then need to dismount 
            // Notify observer for handleing event
            iObserver->HandleMMCEventL(drv,EFalse);
            }
        }
    CPIXLOGSTRING("END CMMCMonitorUtil::RunL");
    }

// End Of File
