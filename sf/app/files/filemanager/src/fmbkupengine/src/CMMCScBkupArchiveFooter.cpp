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
* Description: CMMCScBkupArchiveFooter Implementation
*
*
*/

#include "CMMCScBkupArchiveFooter.h"

// User includes
#include "CMMCScBkupIndexPublicDataFiles.h"
#include "CMMCScBkupIndexDataOwners.h"
#include "CMMCScBkupIndexJavaData.h"
#include "CMMCScBkupIndexSystemData.h"
#include "CMMCScBkupIndexActiveData.h"
#include "CMMCScBkupIndexJavaData.h"
#include "CMMCScBkupIndexPassiveData.h"
#include "MMMCScBkupArchiveDataInterface.h"
#include "CMMCScBkupDataOwnerInfo.h"
#include "MMCScBkupArchiveUtils.h"
#include "CMMCScBkupDataOwnerCollection.h"

/**

    UNDERSTANDING ARCHIVE FOOTER
    ============================

    // Indicies are:
    //
    // EMMCScBkupOwnerDataTypeDataOwner
    // EMMCScBkupOwnerDataTypeJavaData
    // EMMCScBkupOwnerDataTypePublicData
    // EMMCScBkupOwnerDataTypeSystemData
    // EMMCScBkupOwnerDataTypeActiveData
    // EMMCScBkupOwnerDataTypePassiveData
    //
    //  6 (index objects) *
    //  ( 1 byte for object type + TMMCScBkupArchiveVector::ExternalizedSize() )
    //

    // THIS IS THE SIZE OF AN INDIVIDUAL "INDEX RECORD POINTER"
    const TInt indexDataSize = 1 // index type
        + TMMCScBkupArchiveVector::ExternalizedSize() // vector
        + 4 // spare1
        + 4 // spare2
        ;

    // THIS IS THE TOTAL SIZE OF ALL "INDEX RECORD POINTERS"
    const TInt sizeOfAllIndicies = 
          4 // stream format version
        + 4 // spare1
        + 4 // spare2
        + 4 // spare3
        + 4 // spare4
        + 4 // index object count
        + ( EMMCScBkupOwnerDataTypeCount * indexDataSize);

    // WE ALSO WRITE THE AMOUNT OF DISK SPACE REQUIRED FOR EACH DRIVE
    // IN ORDER TO RESTORE THE ARCHIVE

    (not included here)
*/


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupArchiveFooter::CMMCScBkupArchiveFooter()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupArchiveFooter::CMMCScBkupArchiveFooter( MMMCScBkupArchiveDataInterface& aDataInterface, MMMCScBkupDriver& aDriver )
:   iDataInterface( aDataInterface ), iDriver( aDriver )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveFooter::~CMMCScBkupArchiveFooter()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupArchiveFooter::~CMMCScBkupArchiveFooter()
    {
    iIndicies.Close();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveFooter::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveFooter::ConstructL()
    {
    CMMCScBkupIndexBase* index = NULL;
    //
    index = CMMCScBkupIndexDataOwners::NewLC();
    iIndicies.AppendL(index);
    CleanupStack::Pop(index);
    //
    index = CMMCScBkupIndexPublicDataFiles::NewLC();
    iIndicies.AppendL(index);
    CleanupStack::Pop(index);
    //
    index = CMMCScBkupIndexSystemData::NewLC();
    iIndicies.AppendL(index);
    CleanupStack::Pop(index);
    //
    index = CMMCScBkupIndexJavaData::NewLC();
    iIndicies.AppendL(index);
    CleanupStack::Pop(index);
    //
    index = CMMCScBkupIndexActiveData::NewLC();
    iIndicies.AppendL(index);
    CleanupStack::Pop(index);
    //
    index = CMMCScBkupIndexPassiveData::NewLC();
    iIndicies.AppendL(index);
    CleanupStack::Pop(index);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveFooter::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupArchiveFooter* CMMCScBkupArchiveFooter::NewL( MMMCScBkupArchiveDataInterface& aDataInterface, MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupArchiveFooter* self = new(ELeave) CMMCScBkupArchiveFooter( aDataInterface, aDriver );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveFooter::IndexByType()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupIndexBase& CMMCScBkupArchiveFooter::IndexByType( TMMCScBkupOwnerDataType aType )
    {
    CMMCScBkupIndexBase* ret = IndexByTypeOrNull( aType );
    __ASSERT_ALWAYS(ret != NULL, User::Invariant());
    return *ret;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveFooter::IndexByTypeOrNull()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupIndexBase* CMMCScBkupArchiveFooter::IndexByTypeOrNull( TMMCScBkupOwnerDataType aType )
    {
    CMMCScBkupIndexBase* ret = NULL;
    //
    const TInt count = iIndicies.Count();
    for(TInt i=0; i<count; i++)
        {
        CMMCScBkupIndexBase* entry = iIndicies[i];
        if  (entry->Type() == aType)
            {
            ret = entry;
            break;
            }
        }
    //
    return ret;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveFooter::StoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveFooter::StoreL()
    {
    RWriteStream stream( iDataInterface.ADIWriteStreamUncompressedLC() );
    //    
    const TInt count = iIndicies.Count();
    __ASSERT_DEBUG( count == EMMCScBkupOwnerDataTypeCount, User::Invariant());

    // NEW: write index version
    stream.WriteInt32L( EStreamFormatVersion1 );

    // NEW: spare1, spare2, spare3, spare4
    stream.WriteInt32L( 0 );
    stream.WriteInt32L( 0 );
    stream.WriteInt32L( 0 );
    stream.WriteInt32L( 0 );

    // NEW: write the number of indicies...
    stream.WriteInt32L( count );

    // ...then write each index in turn...
    for(TInt i=0; i<count; i++)
        {
        // First write the index type...
        const CMMCScBkupIndexBase& index = *iIndicies[i];
        stream.WriteInt8L( index.Type() );

        // Then write its offset (and length)
        const TMMCScBkupArchiveVector& vector = index.Vector();
        stream << vector;

        // NEW: Then write spare bytes for each index record
        stream.WriteInt32L( 0 ); // per-index spare1
        stream.WriteInt32L( 0 ); // per-index spare2
        }

    // NEW: write the amount of disk space required for each drive as
    // part of the footer information
    CMMCScBkupDataOwnerCollection& dataOwners = iDriver.DrvDataOwners();
    dataOwners.CalculateDiskSpaceRequiredForRestoreL();
    stream << dataOwners;
    
    // Tidy up...
    stream.CommitL();
    CleanupStack::PopAndDestroy(); // stream

#ifdef RD_FILE_MANAGER_BACKUP
    // Disable crc-calculation for header data. Header crc'd separately
    iDataInterface.ADIActivateCrcCalculation(EFalse);
#endif
    
    // We can now update the header with the total length of the footer, validation bits and crcs
    const TMMCScBkupArchiveVector& finalWriteInfo = iDataInterface.ADICurrentArchiveVectorInfo();
    MMCScBkupArchiveUtils::SetFooterLengthL( iDataInterface.ADIRawArchiveFile(), finalWriteInfo.Length() );
    MMCScBkupArchiveUtils::SetArchiveContentAsValidL( iDataInterface.ADIRawArchiveFile() );
#ifdef RD_FILE_MANAGER_BACKUP
    MMCScBkupArchiveUtils::SetArchiveCrcsL( iDataInterface.ADIRawArchiveFile(), iDataInterface.ADIArchiveCrc() );
#endif
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveFooter::RestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveFooter::RestoreL( TInt aCalculatedFooterOffsetWithinArchive )
    {
    RReadStream stream( iDataInterface.ADIReadStreamUncompressedLC( aCalculatedFooterOffsetWithinArchive ) );

    // NEW: Read stream format
    stream.ReadInt32L(); // EStreamFormatVersion1 

    // NEW: spare1, spare2, spare3, spare4
    stream.ReadInt32L();
    stream.ReadInt32L();
    stream.ReadInt32L();
    stream.ReadInt32L();
    
    // NEW: Read written index count
    const TInt inMemoryIndexCount = iIndicies.Count();
    const TInt archiveIndexCount = stream.ReadInt32L();
    if  ( archiveIndexCount > inMemoryIndexCount )
        {
        // Something wrong with file format version info?
        User::Leave( KErrCorrupt );
        }

    // Read index offsets
    for(TInt i=0; i<archiveIndexCount; i++)
        {
        // We only read the offset & length information. 
        // The actual respective index restoration is done by other
        // objects as a result of other states
        const TMMCScBkupOwnerDataType type = static_cast< TMMCScBkupOwnerDataType >( stream.ReadInt8L() );
        //
        if  ( type < 0 || type >= EMMCScBkupOwnerDataTypeCount )
            {
            User::Leave( KErrCorrupt );
            }
        else
            {
            // Always read the vector info (in order to keep the stream
            // position inline with the externalised representation)
            TMMCScBkupArchiveVector vector;
            stream >> vector;
            
            // NEW: Then read spare bytes for each index record
            stream.ReadInt32L(); // per-index spare1
            stream.ReadInt32L(); // per-index spare2

            // But only prepare the index if we have a similar type..
            CMMCScBkupIndexBase* index = IndexByTypeOrNull( type );
            //
            if  ( index )
                {
                index->SetVector( vector );
                }

            }
        }

    // NEW: read the amount of disk space required for each drive as
    // part of the footer information
    CMMCScBkupDataOwnerCollection& dataOwners = iDriver.DrvDataOwners();
    stream >> dataOwners;

    //
    CleanupStack::PopAndDestroy(); // stream
    }






















