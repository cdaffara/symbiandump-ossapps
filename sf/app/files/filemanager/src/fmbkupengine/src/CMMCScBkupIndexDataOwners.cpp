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
* Description: CMMCScBkupIndexDataOwners implementation
*
*
*/

#include "CMMCScBkupIndexDataOwners.h"

// User includes
#include "MMMCScBkupArchiveDataInterface.h"

// Constants
const TInt KMMCScBkupDataOwnerIndexGranularity = 50;


    /**
     * Registration Data
     * =================
     * 
     * This is the format of the data written by the
     * CMMCScBkupStateGetDataOwners object
     * 
     * 
     *  REG DATA for DO 0
     *  {
     *      RD for DO 0, 1st drive
     *      RD for DO 0, 2nd drive
     *      RD for DO 0, 'n'th drive
     *  },
     *  REG DATA for DO 1
     *  {
     *      RD for DO 0, 1st drive
     *      RD for DO 0, 2nd drive
     *      RD for DO 0, 'n'th drive
     *  },
     *  REG DATA for DO n
     *  {
     *      RD for DO 0, 1st drive
     *      RD for DO 0, 2nd drive
     *      RD for DO 0, 'n'th drive
     *  }
     * 
     * Registration Data Index
     * =======================
     * 
     * This is the format of the data written by this method.
     * The format allows the possibility of a future partial
     * restore (hopefully).
     * 
     * 
     *  4 bytes = count of data owners
     *
     *  FOR EACH DATA OWNER
     *  {
     *      4 bytes = secure id of data owner's process
     *      4 bytes = the number of different registration data files
     *                for this particular data owner (one for each drive that
     *                was backed up)
     *      ENTRY
     *      {
     *          1 byte = Associated TDriveNumber of the registration data 
     *          n bytes = raw drive data    
     *      }
     *  }
     *
     *
     **/



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupIndexDataOwners::CMMCScBkupIndexDataOwners()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupIndexDataOwners::CMMCScBkupIndexDataOwners()
:   CMMCScBkupIndexBase( EMMCScBkupOwnerDataTypeDataOwner ), iEntries( KMMCScBkupDataOwnerIndexGranularity )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexDataOwners::~CMMCScBkupIndexDataOwners()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupIndexDataOwners::~CMMCScBkupIndexDataOwners()
    {
    iEntries.Close();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexDataOwners::NewLC()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupIndexDataOwners* CMMCScBkupIndexDataOwners::NewLC()
    {
    CMMCScBkupIndexDataOwners* self = new(ELeave) CMMCScBkupIndexDataOwners();
    CleanupStack::PushL(self);
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexDataOwners::AddIndexRecordL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupIndexDataOwners::AddIndexRecordL( const TMMCScBkupArchiveVector& aInfo )
    {
    iEntries.AppendL( aInfo );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexDataOwners::StoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupIndexDataOwners::StoreL(MMMCScBkupDriver& aDriver)
    {
    MMMCScBkupArchiveDataInterface& archiveDataInterface = aDriver.DrvADI();
    RWriteStream& stream = archiveDataInterface.ADIWriteStreamUncompressedLC();
    //    
    stream.WriteInt32L( EStreamFormatVersion1 );
    stream.WriteInt32L( 0 ); // spare1
    stream.WriteInt32L( 0 ); // spare2
    stream.WriteInt32L( 0 ); // spare3
    //
    const TInt count = iEntries.Count();
    stream.WriteInt32L(count);
    //
    for(TInt i=0; i<count; i++)
        {
        const TMMCScBkupArchiveVector& entry = iEntries[i];
        stream << entry;
        //
        stream.WriteInt32L( 0 ); // spare1
        stream.WriteInt32L( 0 ); // spare2
        }
    //
    stream.CommitL();
    CleanupStack::PopAndDestroy(); // stream

    // Update our base class info with the offset to the index
    const TMMCScBkupArchiveVector& writeInfo = archiveDataInterface.ADICurrentArchiveVectorInfo();
    SetVector( writeInfo );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexDataOwners::RestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupIndexDataOwners::RestoreL(MMMCScBkupDriver& aDriver)
    {
    MMMCScBkupArchiveDataInterface& archiveDataInterface = aDriver.DrvADI();
    RReadStream& stream = archiveDataInterface.ADIReadStreamUncompressedLC( Vector().Offset() );
    //
    stream.ReadInt32L(); // EStreamFormatVersion1 
    stream.ReadInt32L(); // spare1
    stream.ReadInt32L(); // spare2
    stream.ReadInt32L(); // spare3
    //    
    const TInt count = stream.ReadInt32L();
    //
    for(TInt i=0; i<count; i++)
        {
        TMMCScBkupArchiveVector entry;
        stream >> entry;
        //
        stream.ReadInt32L(); // spare1
        stream.ReadInt32L(); // spare2
        //
        iEntries.AppendL( entry );
        }
    //
    CleanupStack::PopAndDestroy(); // stream
    //
    const TMMCScBkupArchiveVector& readInfo = archiveDataInterface.ADICurrentArchiveVectorInfo();
    if  ( readInfo.Length() > Vector().Length() )
        {
        // We've read too much!
        User::Leave( KErrCorrupt );
        }
    }







