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
#include "cmmcmonitor.h"
#include "cfileplugin.h"
#include "harvesterserverlogger.h"

#include <pathinfo.h>
#include <s32file.h>
#include <f32file.h> // TDriveNumber
#include <driveinfo.h> // TDriveInfo

#include <UikonInternalPSKeys.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmcmonitorTraces.h"
#endif


// CONSTANTS
// 

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMMCMonitor::NewL
// -----------------------------------------------------------------------------
//
CMMCMonitor* CMMCMonitor::NewL( CFilePlugin& aFilePlugin, RFs* aFsSession )
    {
    CMMCMonitor* self = new ( ELeave ) CMMCMonitor( aFilePlugin );
    CleanupStack::PushL( self );
    self->ConstructL( aFsSession );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CMMCMonitor::CMMCMonitor
// -----------------------------------------------------------------------------
//
CMMCMonitor::CMMCMonitor( CFilePlugin& aFilePlugin )
    : CActive( CActive::EPriorityStandard ),
      iFilePlugin( aFilePlugin )
    {
    OstTraceFunctionEntry0( CMMCMONITOR_CMMCMONITOR_ENTRY );
    CPIXLOGSTRING("ENTER CMMCMonitor::CMMCMonitor");
    CActiveScheduler::Add( this );
    CPIXLOGSTRING("END CMMCMonitor::CMMCMonitor");
    OstTraceFunctionExit0( CMMCMONITOR_CMMCMONITOR_EXIT );
    }


// -----------------------------------------------------------------------------
// CMMCMonitor::~CMMCMonitor
// -----------------------------------------------------------------------------
//
CMMCMonitor::~CMMCMonitor()
    {
    CPIXLOGSTRING("ENTER ~CMMCMonitor");

    Cancel();
    iProperty.Close();
    CPIXLOGSTRING("END ~CMMCMonitor");
    }


// -----------------------------------------------------------------------------
// CMMCMonitor::ConstructL
// -----------------------------------------------------------------------------
//
void CMMCMonitor::ConstructL( RFs* aFsSession )
    {
    OstTraceFunctionEntry0( CMMCMONITOR_CONSTRUCTL_ENTRY );
    CPIXLOGSTRING("ENTER CMMCMonitor::ConstructL Foobar");
    iFsSession = aFsSession;
    
    TInt error = iProperty.Attach( KPSUidUikon, KUikMMCInserted );
    if ( error != KErrNone ) 
        {
        OstTrace0( TRACE_NORMAL, CMMCMONITOR_CONSTRUCTL, "END CMMCMonitor::Attach to MMCInserted failed" );
        CPIXLOGSTRING("END CMMCMonitor::Attach to MMCInserted failed");
        }
    
    error = iProperty.Get( KPSUidUikon, KUikMMCInserted, iMmcStatus );
    if ( error != KErrNone ) 
    	{
		OstTrace0( TRACE_NORMAL, DUP1_CMMCMONITOR_CONSTRUCTL, "CMMCMonitor::Get MMCInserted failed" );
		CPIXLOGSTRING("CMMCMonitor::Get MMCInserted failed");
    	} 
    else if ( iMmcStatus ) 	 
    	{
    	OstTrace0( TRACE_NORMAL, DUP2_CMMCMONITOR_CONSTRUCTL, "CMMCMonitor::MMC card is in" );
    	CPIXLOGSTRING("CMMCMonitor::MMC card is in");
    	}
    else 
    	{
    	OstTrace0( TRACE_NORMAL, DUP3_CMMCMONITOR_CONSTRUCTL, "CMMCMonitor::no MMC card" );
    	CPIXLOGSTRING("CMMCMonitor::no MMC card");
    	}

    // The CFilePlugin::StartMonitoring() will call
    // CMMCMonitor::StartMonitoring() which will call
    // CMMCMonitor::RunL().
    //
    CPIXLOGSTRING("END CMMCMonitor::ConstructL");
    OstTraceFunctionExit0( CMMCMONITOR_CONSTRUCTL_EXIT );
    }


// -----------------------------------------------------------------------------
// CMMCMonitor::StartMonitoring
// -----------------------------------------------------------------------------
//
TBool CMMCMonitor::StartMonitoring()
    {
    OstTraceFunctionEntry0( CMMCMONITOR_STARTMONITORING_ENTRY );
    CPIXLOGSTRING("ENTER CMMCMonitor::StartMonitoring");
    iProperty.Subscribe( iStatus );
    SetActive();
    User::LeaveIfError( iProperty.Get( KPSUidUikon, KUikMMCInserted, iMmcStatus ) );
    
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

        const TInt err = DriveInfo::GetDriveStatus( *iFsSession, driveNumber, drvStatus );
        if ( err ) 
            {
            continue;  // should not happen
            }

        if ( drvStatus & DriveInfo::EDrivePresent )
            {            
            iFilePlugin.MountL(drv, EFalse);
            }
        }
    CPIXLOGSTRING("END CMMCMonitor::StartMonitoring");
    OstTraceFunctionExit0( CMMCMONITOR_STARTMONITORING_EXIT );
    return ETrue;
    }


// -----------------------------------------------------------------------------
// CMMCMonitor::MmcStatus
// -----------------------------------------------------------------------------
//
TBool CMMCMonitor::MmcStatus( TInt aDriveNumber )
    {
    TBool isMmcPresent(EFalse);

    if ( iFsSession->IsValidDrive( aDriveNumber ) )
        {
        TUint drvStatus( 0 );
        TInt err = DriveInfo::GetDriveStatus( *iFsSession, aDriveNumber, drvStatus );
        if ( err )
            {
            return EFalse;
            }
        // MMC drives are removable and user visible
        if ( ( drvStatus & DriveInfo::EDriveRemovable ) &&
             ( drvStatus & DriveInfo::EDriveUserVisible ) )
                {
                OstTrace1( TRACE_NORMAL, CMMCMONITOR_MMCSTATUS, "CMMCMonitor::MmcStatus;Drive Number=%d", aDriveNumber );
                CPIXLOGSTRING2("CMMCMonitor::MmcStatus Drive Number %d", aDriveNumber);
                isMmcPresent = ETrue;
                }
        }
    return isMmcPresent;
    }


// -----------------------------------------------------------------------------
// CMMCMonitor::RunError
// -----------------------------------------------------------------------------
//
TInt CMMCMonitor::RunError( TInt aError )
    {
    OstTrace1( TRACE_NORMAL, CMMCMONITOR_RUNERROR, "CMMCMonitor::RunError;aError=%d", aError );
    CPIXLOGSTRING2("CMMCMonitor::RunError Error:",aError);
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMMCMonitor::DoCancel
// -----------------------------------------------------------------------------
//
void CMMCMonitor::DoCancel()
    {
    OstTraceFunctionEntry0( CMMCMONITOR_DOCANCEL_ENTRY );
    CPIXLOGSTRING("ENTER CMMCMonitor::DoCancel");
    iProperty.Cancel();
    CPIXLOGSTRING("END CMMCMonitor::DoCancel");
    OstTraceFunctionExit0( CMMCMONITOR_DOCANCEL_EXIT );
    }


// -----------------------------------------------------------------------------
// CMMCMonitor::RunL
// -----------------------------------------------------------------------------
//
void CMMCMonitor::RunL()
    {
    OstTraceFunctionEntry0( CMMCMONITOR_RUNL_ENTRY );
    CPIXLOGSTRING("ENTER CMMCMonitor::RunL");
    iProperty.Subscribe( iStatus );
    SetActive();
    User::LeaveIfError( iProperty.Get( KPSUidUikon, KUikMMCInserted, iMmcStatus ) );
    
    if ( iMmcStatus )
    	{
    	OstTrace0( TRACE_NORMAL, CMMCMONITOR_RUNL, "CMMCMonitor::MMC card is in" );
    	CPIXLOGSTRING("CMMCMonitor::MMC card is in");
    	}
    else 
    	{
		OstTrace0( TRACE_NORMAL, DUP1_CMMCMONITOR_RUNL, "CMMCMonitor::no MMC card" );
		CPIXLOGSTRING("CMMCMonitor::no MMC card");
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

        const TInt err = DriveInfo::GetDriveStatus( *iFsSession, driveNumber, drvStatus );
        if ( err ) 
            {
            continue;  // should not happen
            }

        if ( drvStatus & DriveInfo::EDrivePresent )
            {
            OstTrace0( TRACE_NORMAL, DUP2_CMMCMONITOR_RUNL, "CMMCMonitor::RunL insert event" );
            CPIXLOGSTRING("CMMCMonitor::RunL insert event");
            // Mount MMC and force reharvest
            iFilePlugin.MountL(drv, ETrue);
            }
        else
            {
            OstTrace0( TRACE_NORMAL, DUP3_CMMCMONITOR_RUNL, "CMMCMonitor::RunL eject event" );
            CPIXLOGSTRING("CMMCMonitor::RunL eject event");
            // If the MMC has been ejected, then need to dismount 
            // and undefine the volume and also remove from queue forever
            iFilePlugin.UnMount(drv, ETrue,ETrue);
            }
        }
    CPIXLOGSTRING("END CMMCMonitor::RunL");
    OstTraceFunctionExit0( CMMCMONITOR_RUNL_EXIT );
    }

// End Of File
