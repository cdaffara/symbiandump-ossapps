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
* Description: CMMCScBkupIndexSystemData implementation
*
*
*/

#include "CMMCScBkupIndexSystemData.h"

// User includes
#include "MMMCScBkupArchiveDataInterface.h"


    /**
     * System Data
     * ===========
     * 
     * This is the format of the data written by the
     * CMMCScBkupStateArchiveOpSystemData object
     * 
     * 
     *  SYSTEM DATA for DO 0
     *  {
     *      SD for DO 0, 1st drive
     *      SD for DO 0, 2nd drive
     *      SD for DO 0, 'n'th drive
     *  },
     *  SYSTEM DATA for DO 1
     *  {
     *      SD for DO 0, 1st drive - (System data is only on
     *      SD for DO 0, 2nd drive    two drives)
     *  },
     *  SYSTEM DATA for DO n
     *  {
     *      SD for DO 0, 1st drive - (System Data is only on one drive)
     *  }
     * 
     * System Data Index
     * =================
     * 
     * This is the format of the data written by this object.
     * The format allows the possibility of a future partial
     * restore (hopefully).
     * 
     * 
     *  4 bytes = count of system data entries
     *
     *  FOR EACH DATA OWNER
     *  {
     *      4 bytes = package id associated with a system data owner
     *      4 bytes = the number of different system data packages
     *                for this particular system data owner (one for each 
     *                drive that was backed up)
     *      ENTRY
     *      {
     *          1 byte = Associated TDriveNumber of the system data
     *          8 bytes = offset & length of actual registration data
     *      }
     *  }
     *
     *
     **/




// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupIndexSystemData::CMMCScBkupIndexSystemData()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupIndexSystemData::CMMCScBkupIndexSystemData()
:   CMMCScBkupIndexWithIdentifier< TInt32 >( EMMCScBkupOwnerDataTypeSystemData )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexSystemData::NewLC()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupIndexSystemData* CMMCScBkupIndexSystemData::NewLC()
    {
    CMMCScBkupIndexSystemData* self = new(ELeave) CMMCScBkupIndexSystemData();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }







