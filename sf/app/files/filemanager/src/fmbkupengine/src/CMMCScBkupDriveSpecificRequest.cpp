/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: CMMCScBkupDriveSpecificRequest implementation
*
*
*/

#include "CMMCScBkupDriveSpecificRequest.h"

// User includes
#include "CMMCScBkupDriveAndOperationTypeManager.h"


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSpecificRequest::CMMCScBkupDriveSpecificRequest()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupDriveSpecificRequest::CMMCScBkupDriveSpecificRequest( const CMMCScBkupDriveAndOperationTypeManager& aDriveAndOperations, TMMCScBkupOwnerDataType aDataType, CActive::TPriority aPriority )
:   CActive(aPriority), iDriveAndOperations(aDriveAndOperations), iDataType( aDataType )
    {
    CActiveScheduler::Add(this);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSpecificRequest::~CMMCScBkupDriveSpecificRequest()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupDriveSpecificRequest::~CMMCScBkupDriveSpecificRequest( )
    {
    Cancel();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSpecificRequest::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDriveSpecificRequest::ConstructL( )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSpecificRequest::RequestL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDriveSpecificRequest::RequestL( TRequestStatus& aObserver )
    {
    // Set to -1 so that when RunL is called, the next drive will be calculated
    // as 0 == EDriveA
    iCurrentDrive = -1;
    iObserver = &aObserver;
    *iObserver = KRequestPending;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSpecificRequest::NextValidDrive()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupDriveSpecificRequest::NextValidDrive(TDriveNumber& aDrive)
    {
    // If we've already reached Z then there isn't any sense in continuing
    // as all drives have been processed.
    TBool driveAvailable = EFalse;

    // Keep checking drives until we go past Z
    while( ++iCurrentDrive <= EDriveZ )
        {
        if  ( iDriveAndOperations.DriveList()[ iCurrentDrive ] != 0 )
            {
            const TDriveNumber drive =  static_cast< TDriveNumber >( iCurrentDrive );

            // Drive is allowable according to master list.
            // But is this data type allowed to access that drive?
            if  ( iDataType == EMMCScBkupOwnerDataTypeAny )
                {
                // Found an available drive - data type filtering is not in play...
                aDrive = drive;
                driveAvailable = ETrue;
                break;
                }
            else if ( iDriveAndOperations.IsDataTypeAllowedToAccessDrive( drive, iDataType ) )
                {
                // Found an available drive - this data type is allowed to access that drive...
                aDrive = drive;
                driveAvailable = ETrue;
                break;
                }
            }
        }
    //
    return driveAvailable;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSpecificRequest::NextValidDrive()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupDriveSpecificRequest::NextValidDrive(TDriveNumber& aDrive, const TDriveList& aCrossCheckList)
    {
    TDriveNumber drive;
    TBool driveAvailable = NextValidDrive(drive);
    
    while(driveAvailable)
        {
        // Check if aCrossCheckList also supports the same drive
        if  (aCrossCheckList[drive] != 0)
            {
            aDrive = drive;
            break;
            }
        else
            {
            // Try another drive from our master list
            driveAvailable = NextValidDrive(drive);
            }
        }
    //
    return driveAvailable;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSpecificRequest::CurrentDrive()
// 
// 
// ---------------------------------------------------------------------------
TDriveNumber CMMCScBkupDriveSpecificRequest::CurrentDrive() const
    {
    return static_cast<TDriveNumber>(iCurrentDrive);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSpecificRequest::DoCancel()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDriveSpecificRequest::DoCancel()
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSpecificRequest::RunError()
// 
// 
// ---------------------------------------------------------------------------
TInt CMMCScBkupDriveSpecificRequest::RunError(TInt aError)
    {
#ifdef MMCSCBKUP_USE_BREAKPOINTS
    __BREAKPOINT();
#endif
    //
    CompleteObserverRequest(aError);
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSpecificRequest::CompleteObserverRequest()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDriveSpecificRequest::CompleteObserverRequest(TInt aCompletionCode)
    {
    __ASSERT_ALWAYS(iObserver != NULL, User::Invariant());
    User::RequestComplete(iObserver, aCompletionCode);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSpecificRequest::CompleteSelf()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDriveSpecificRequest::CompleteSelf(TInt aCompletionCode)
    {
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, aCompletionCode);
    }


