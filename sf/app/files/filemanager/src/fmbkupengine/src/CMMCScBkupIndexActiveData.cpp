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
* Description: CMMCScBkupIndexActiveData implementation
*
*
*/

#include "CMMCScBkupIndexActiveData.h"

// User includes
#include "MMMCScBkupArchiveDataInterface.h"


    /**
     * Active Data
     * ===========
     * 
     * This is the format of the data written by the
     * CMMCScBkupStateArchiveOpActiveData object
     * 
     * E.g.:
     * 
     *  ACTIVE DATA for DO 0
     *  {
     *      AD for DO 0, 1st drive
     *      AD for DO 0, 2nd drive
     *      AD for DO 0, 'n'th drive
     *  },
     *  ACTIVE DATA for DO 1
     *  {
     *      AD for DO 0, 1st drive - (AD is only on
     *      AD for DO 0, 2nd drive    two drives)
     *  },
     *  ACTIVE DATA for DO n
     *  {
     *      AD for DO 0, 1st drive - (AD is only on one drive)
     *  }
     * 
     * Active Data Index
     * =================
     * 
     * This is the format of the data written by this object.
     * The format allows the possibility of a future partial
     * restore (hopefully).
     * 
     * 
     *  4 bytes = count of active data entries
     *
     *  FOR EACH DATA OWNER
     *  {
     *      4 bytes = secure id associated with a active data owner
     *      4 bytes = the number of different active data packages
     *                for this particular active data owner (one for each 
     *                drive that was backed up)
     *      ENTRY
     *      {
     *          1 byte = Associated TDriveNumber of the active data
     *          8 bytes = offset and length of raw data
     *      }
     *  }
     *
     *
     **/


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupIndexActiveData::CMMCScBkupIndexActiveData()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupIndexActiveData::CMMCScBkupIndexActiveData()
:   CMMCScBkupIndexWithIdentifier< TSecureId >( EMMCScBkupOwnerDataTypeActiveData )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexActiveData::NewLC()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupIndexActiveData* CMMCScBkupIndexActiveData::NewLC()
    {
    CMMCScBkupIndexActiveData* self = new(ELeave) CMMCScBkupIndexActiveData();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }




