/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CMMCScBkupIndexJavaDataEntry implementation
*
*
*/

#include "CMMCScBkupIndexJavaData.h"

// User includes
#include "MMMCScBkupArchiveDataInterface.h"
#include "MMCScBkupLogger.h"


    /**
     * Registration Data
     * =================
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
     * This is the format of the data written by this object.
     * The format allows the possibility of a future partial
     * restore (hopefully).
     * 
     * 
     *  4 bytes = count of registration data entries
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
     *          8 bytes = offset & length of actual registration data
     *      }
     *  }
     *
     **/








// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupIndexJavaDataEntry::CMMCScBkupIndexJavaDataEntry()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupIndexJavaDataEntry::CMMCScBkupIndexJavaDataEntry()
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexJavaDataEntry::CMMCScBkupIndexJavaDataEntry()
// 
// C++ destructor.
// ---------------------------------------------------------------------------
CMMCScBkupIndexJavaDataEntry::~CMMCScBkupIndexJavaDataEntry()
    {
    delete iHash;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexJavaDataEntry::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupIndexJavaDataEntry::ConstructL()
    {
    iHash = KNullDesC().AllocL();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexJavaDataEntry::NewLC()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupIndexJavaDataEntry* CMMCScBkupIndexJavaDataEntry::NewLC()
    {
    CMMCScBkupIndexJavaDataEntry* self = new(ELeave) CMMCScBkupIndexJavaDataEntry();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexJavaDataEntry::NewLC()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupIndexJavaDataEntry* CMMCScBkupIndexJavaDataEntry::NewLC( RReadStream& aStream )
    {
    CMMCScBkupIndexJavaDataEntry* self = new(ELeave) CMMCScBkupIndexJavaDataEntry();
    CleanupStack::PushL( self );
    aStream >> *self;
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexJavaDataEntry::SetHashL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupIndexJavaDataEntry::SetHashL( const TDesC& aHash )
    {
    HBufC* hash  = aHash.AllocL();
    delete iHash;
    iHash = hash;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexJavaDataEntry::InternalizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupIndexJavaDataEntry::InternalizeL( RReadStream& aStream )
    {
    aStream.ReadInt32L(); // EStreamFormatVersion1 
    aStream.ReadInt32L(); // spare1
    aStream.ReadInt32L(); // spare2
    aStream.ReadInt32L(); // spare3
    //
    HBufC* hash = HBufC::NewL( aStream, KMaxTInt );
    delete iHash;
    iHash = hash;
    //
    aStream >> iMidletInfo;
    aStream >> iMidletDataInfo;
    iDrive = static_cast< TDriveNumber>( aStream.ReadInt8L() );
	//
	__LOG6("CMMCScBkupIndexJavaDataEntry::InternalizeL() - hash: %S, iDrive: %c:, MIDLET[offset: %8d, length: %8d], MIDLETDATA[offset: %8d, length: %8d]", iHash, iDrive + 'A', iMidletInfo.Offset(), iMidletInfo.Length(), iMidletDataInfo.Offset(), iMidletDataInfo.Length() );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexJavaDataEntry::ExternalizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupIndexJavaDataEntry::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( EStreamFormatVersion1 );
    aStream.WriteInt32L( 0 ); // spare1
    aStream.WriteInt32L( 0 ); // spare2
    aStream.WriteInt32L( 0 ); // spare3
    //
    aStream << Hash();
    aStream << iMidletInfo;
    aStream << iMidletDataInfo;
    aStream.WriteInt8L( iDrive );
	//
	__LOG6("CMMCScBkupIndexJavaDataEntry::ExternalizeL() - hash: %S, iDrive: %c:, MIDLET[offset: %8d, length: %8d], MIDLETDATA[offset: %8d, length: %8d]", iHash, iDrive + 'A', iMidletInfo.Offset(), iMidletInfo.Length(), iMidletDataInfo.Offset(), iMidletDataInfo.Length() );
	}



















// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupIndexJavaData::CMMCScBkupIndexJavaData()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupIndexJavaData::CMMCScBkupIndexJavaData()
:   CMMCScBkupIndexBase( EMMCScBkupOwnerDataTypeJavaData )
    {
    }

// ---------------------------------------------------------------------------
// CMMCScBkupIndexJavaData::~CMMCScBkupIndexJavaData()
// 
// C++ destructor.
// ---------------------------------------------------------------------------
CMMCScBkupIndexJavaData::~CMMCScBkupIndexJavaData()
    {
    iEntries.Close();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexJavaData::NewLC()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupIndexJavaData* CMMCScBkupIndexJavaData::NewLC()
    {
    CMMCScBkupIndexJavaData* self = new(ELeave) CMMCScBkupIndexJavaData();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexJavaData::AddIndexRecordL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupIndexJavaData::AddIndexRecordL( const TMMCScBkupArchiveVector& aInfo, const TDesC& aHash, TDriveNumber aDrive, TJavaTransferType aType )
    {
    // Check if we already have an entry for this item...
    CMMCScBkupIndexJavaDataEntry* entry = EntryByHash( aHash, aDrive );
    
    if  ( !entry )
        {
		__LOG1("CMMCScBkupIndexJavaData::AddIndexRecordL() - making new entry for hash: %S", &aHash);
		//
        entry = CMMCScBkupIndexJavaDataEntry::NewLC();
        //
        entry->SetHashL( aHash );
        entry->SetDrive( aDrive );
        iEntries.AppendL( entry );
        //
        CleanupStack::Pop( entry );
        }
	else
		{
		__LOG1("CMMCScBkupIndexJavaData::AddIndexRecordL() - found existing entry for hash: %S", &aHash);
		}

    if  ( aType == EJavaMIDlet )
        {
        entry->SetMidletInfo( aInfo );
        }
    else if ( aType == EJavaMIDletData )
        {
        entry->SetMidletDataInfo( aInfo );
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexJavaData::At()
// 
// 
// ---------------------------------------------------------------------------
const CMMCScBkupIndexJavaDataEntry& CMMCScBkupIndexJavaData::At( TInt aIndex ) const
    {
    const CMMCScBkupIndexJavaDataEntry* entry = iEntries[ aIndex ];
    return *entry;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexJavaData::StoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupIndexJavaData::StoreL(MMMCScBkupDriver& aDriver)
    {
    MMMCScBkupArchiveDataInterface& archiveDataInterface = aDriver.DrvADI();
    RWriteStream stream(archiveDataInterface.ADIWriteStreamUncompressedLC());
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
        const CMMCScBkupIndexJavaDataEntry* entry = iEntries[i];
        stream << *entry;
        }
    //
    stream.CommitL();
    CleanupStack::PopAndDestroy(); // stream

    // Update our base class info with the offset to the index
    SetVector( archiveDataInterface.ADICurrentArchiveVectorInfo() );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexJavaData::RestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupIndexJavaData::RestoreL(MMMCScBkupDriver& aDriver)
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
    for(TInt i=0; i<count; i++)
        {
        CMMCScBkupIndexJavaDataEntry* entry = CMMCScBkupIndexJavaDataEntry::NewLC( stream );
        iEntries.AppendL( entry );
        CleanupStack::Pop( entry );
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


// ---------------------------------------------------------------------------
// CMMCScBkupIndexJavaData::EntryByHash()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupIndexJavaDataEntry* CMMCScBkupIndexJavaData::EntryByHash( const TDesC& aHash, TDriveNumber aDrive ) const
    {
    CMMCScBkupIndexJavaDataEntry* ret = NULL;
    const TInt count = iEntries.Count();
    //
    for(TInt i=0; i<count; i++)
        {
        CMMCScBkupIndexJavaDataEntry* entry = iEntries[ i ];
        //
        if  ( entry->Hash() == aHash && entry->Drive() == aDrive )
            {
            ret = entry;
            break;
            }
        }
    //
    return ret;
    }








