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
* Description: CMMCScBkupIndexPassiveData implementation
*
*
*/

#include "CMMCScBkupIndexPassiveData.h"

// User includes
#include "MMMCScBkupArchiveDataInterface.h"


    /**
     * Passive Data
     * ===========
     * 
     *
     * 
     * Passive Data Index
     * =================
     * 
     * This is the format of the data written by this object.
     * The format allows the possibility of a future partial
     * restore (hopefully).
     * 
     * 
     *  4 bytes = count of passive data entries
     *
     *  FOR EACH DATA OWNER
     *  {
     *      4 bytes = secure id associated with a passive data owner
     *      4 bytes = the number of different data entries for this 
     *                particular passive data owner (one for each 
     *                drive that was backed up)
     *      ENTRY
     *      {
     *          1 byte = Associated TDriveNumber of the system data
     *          8 bytes = offset and size to passive data   
     *      }
     *  }
     *
     *
     **/



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupIndexPassiveData::CMMCScBkupIndexPassiveData()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupIndexPassiveData::CMMCScBkupIndexPassiveData()
:   CMMCScBkupIndexWithIdentifier< TSecureId >( EMMCScBkupOwnerDataTypePassiveData )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexPassiveData::NewLC()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupIndexPassiveData* CMMCScBkupIndexPassiveData::NewLC()
    {
    CMMCScBkupIndexPassiveData* self = new(ELeave) CMMCScBkupIndexPassiveData();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }







