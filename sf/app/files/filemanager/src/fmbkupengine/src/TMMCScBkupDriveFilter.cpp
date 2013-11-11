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
* Description: TMMCScBkupDriveFilter implementation
*
*
*/

#include "TMMCScBkupDriveFilter.h"

// Constants
const TInt KMMCScBkupInitialDriveIndex = -1;



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// TMMCScBkupDriveFilter::Reset()
// 
// 
// ---------------------------------------------------------------------------
void TMMCScBkupDriveFilter::Reset()
    {
    iCurrentDrive = KMMCScBkupInitialDriveIndex;
    }


// ---------------------------------------------------------------------------
// TMMCScBkupDriveFilter::SetPrimaryDriveFilter()
// 
// 
// ---------------------------------------------------------------------------
void TMMCScBkupDriveFilter::SetPrimaryDriveFilter( const TDriveList& aDriveList )
    {
    iPrimaryDriveList = aDriveList;
    }


// ---------------------------------------------------------------------------
// TMMCScBkupDriveFilter::SetSecondaryDriveFilter()
// 
// 
// ---------------------------------------------------------------------------
void TMMCScBkupDriveFilter::SetSecondaryDriveFilter( const TDriveList& aDriveList )
    {
    iSecondaryDriveList = aDriveList;
    iHaveSecondaryList = ETrue;
    }


// ---------------------------------------------------------------------------
// TMMCScBkupDriveFilter::NextValidDrive()
// 
// 
// ---------------------------------------------------------------------------
TBool TMMCScBkupDriveFilter::NextValidDrive( TDriveNumber& aDrive )
    {
    // Always move to the next drive before checking its availability.
    // FindValidDrive takes care of ensuring the value falls within A-Z
    // range.
    ++iCurrentDrive;
    
    // Do we have an available primary drive?
    TBool driveAvailable = EFalse;
    while( iCurrentDrive >= EDriveA && iCurrentDrive <= EDriveZ )
        {
        TDriveNumber drive;
        driveAvailable = FindValidDrive( iPrimaryDriveList, drive );
        if  ( driveAvailable && iHaveSecondaryList )
            {
            // Verify against secondary drive list also...
            const TBool makeOnlyOneDirectAttempt = ETrue;
            driveAvailable = FindValidDrive( iSecondaryDriveList, drive, makeOnlyOneDirectAttempt );
            }
        else if (!driveAvailable)
            {
            // No more primary drives left, so we're done here.
            break;
            }

        // Did we find a match on (just) the primary drive, or then
        // the primary and secondary drive lists combined?
        if  (driveAvailable)
            {
            // Yes, match found - we're finished here.
            aDrive = drive;
            break;
            }
        else
            {
            // Try to search for the next drive that matches both the
            // primary and secondary lists (if supported)
            ++iCurrentDrive;
            }
        }
    //
    return driveAvailable;
    }


// ---------------------------------------------------------------------------
// TMMCScBkupDriveFilter::CurrentDrive()
// 
// 
// ---------------------------------------------------------------------------
TDriveNumber TMMCScBkupDriveFilter::CurrentDrive() const
    {
    return static_cast< TDriveNumber >( iCurrentDrive );
    }


// ---------------------------------------------------------------------------
// TMMCScBkupDriveFilter::FindValidDrive()
// 
// 
// ---------------------------------------------------------------------------
TBool TMMCScBkupDriveFilter::FindValidDrive( const TDriveList& aList, TDriveNumber& aDrive, TBool aOneAttemptOnly )
    {
    // If we've already reached Z then there isn't any sense in continuing
    // as all drives have been processed.
    TBool driveAvailable = EFalse;

    // Keep checking drives until we go past Z
    while( iCurrentDrive >= EDriveA && iCurrentDrive <= EDriveZ )
        {
        if  ( aList[ iCurrentDrive ] != 0 )
            {
            // Found an available drive
            aDrive = static_cast< TDriveNumber >( iCurrentDrive );
            driveAvailable = ETrue;
            break;
            }
        else if ( aOneAttemptOnly )
            {
            break;
            }
            
        // Try next drive
        ++iCurrentDrive;
        }
    //
    return driveAvailable;
    }




