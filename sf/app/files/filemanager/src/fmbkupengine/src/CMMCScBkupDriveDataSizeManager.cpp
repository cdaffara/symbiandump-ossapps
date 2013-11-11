/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CMMCScBkupDriveSizer implementation
*
*
*/

#include "CMMCScBkupDriveDataSizeManager.h"




// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupDriveSizer::CMMCScBkupDriveSizer()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupDriveSizer::CMMCScBkupDriveSizer( TMMCScBkupOwnerDataType aType )
:   iDataType( aType )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSizer::~CMMCScBkupDriveSizer()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupDriveSizer::~CMMCScBkupDriveSizer()
    {
    iEntries.Close();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSizer::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDriveSizer::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSizer::NewLC()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupDriveSizer* CMMCScBkupDriveSizer::NewLC( TMMCScBkupOwnerDataType aType )
    {
    CMMCScBkupDriveSizer* self = new(ELeave) CMMCScBkupDriveSizer( aType );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSizer::NewLC()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupDriveSizer* CMMCScBkupDriveSizer::NewLC( RReadStream& aStream )
    {
    CMMCScBkupDriveSizer* self = new(ELeave) CMMCScBkupDriveSizer( EMMCScBkupOwnerDataTypeAny );
    CleanupStack::PushL(self);
    aStream >> *self;
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSizer::AddToSizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDriveSizer::AddToSizeL( TInt64 aAmount, TDriveNumber aDrive )
    {
    TMMCScBkupDriveAndSize* entry = EntryByDrive( aDrive );
    //
    if  ( entry == NULL )
        {
        if  ( aAmount > 0 )
            {
            TMMCScBkupDriveAndSize newEntry( aDrive, aAmount );
            iEntries.AppendL( newEntry );
            }
        }
    else
        {
        entry->AddToSize( aAmount );
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSizer::Reset()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDriveSizer::Reset()
    {
    const TInt count = iEntries.Count();
    for( TInt i=0; i<count; i++ )
        {
        TMMCScBkupDriveAndSize& entry = iEntries[ i ];
        entry.SetSize( 0 );
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSizer::Reset()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDriveSizer::Reset( TDriveNumber aDrive )
    {
    TMMCScBkupDriveAndSize* entry = EntryByDrive( aDrive );
    //
    if  ( entry != NULL )
        {
        entry->SetSize( 0 );
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSizer::Size()
// 
// 
// ---------------------------------------------------------------------------
TInt64 CMMCScBkupDriveSizer::Size() const
    {
    TInt64 size = 0;
    //
    const TInt count = iEntries.Count();
    for( TInt i=0; i<count; i++ )
        {
        const TMMCScBkupDriveAndSize& entry = iEntries[ i ];
        size += entry.Size();
        }
    //
    return size;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSizer::Size()
// 
// 
// ---------------------------------------------------------------------------
TInt64 CMMCScBkupDriveSizer::Size( TDriveNumber aDrive ) const
    {
    TInt64 size = 0;
    TMMCScBkupDriveAndSize* entry = const_cast< CMMCScBkupDriveSizer* >( this )->EntryByDrive( aDrive );
    //
    if  ( entry != NULL )
        {
        size = entry->Size();
        }
    //
    return size;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSizer::InternalizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDriveSizer::InternalizeL(RReadStream& aStream)
    {
    aStream.ReadInt32L(); // EStreamFormatVersion1 
    aStream.ReadInt32L(); // spare1
    aStream.ReadInt32L(); // spare2
    //
    iDataType = static_cast< TMMCScBkupOwnerDataType >( aStream.ReadInt8L() );
    const TInt count = aStream.ReadInt8L();
    for( TInt i=0; i<count; i++ )
        {
        TMMCScBkupDriveAndSize entry;
        aStream >> entry;
        iEntries.AppendL( entry );
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSizer::ExternalizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDriveSizer::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteInt32L( EStreamFormatVersion1 );
    aStream.WriteInt32L( 0 ); // spare1
    aStream.WriteInt32L( 0 ); // spare2
    //
    aStream.WriteInt8L( iDataType );

    // Work out how many non-zero entries we need to write.
    TInt i = 0;
    const TInt count = iEntries.Count();
    TInt numberOfExternalisedEntries = 0;
    //    
    for( i=0; i<count; i++ )
        {
        const TMMCScBkupDriveAndSize& entry = iEntries[ i ];
        if  ( entry.Size() > 0 )
            {
            ++numberOfExternalisedEntries;
            }
        }
    
    // Now really write the entries...
    aStream.WriteInt8L( numberOfExternalisedEntries );
    if  ( numberOfExternalisedEntries > 0 )
        {
        for( i=0; i<count; i++ )
            {
            const TMMCScBkupDriveAndSize& entry = iEntries[ i ];
            if  ( entry.Size() > 0 )
                {
                aStream << entry;
                }
            }
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveSizer::EntryByDrive()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupDriveAndSize* CMMCScBkupDriveSizer::EntryByDrive( TDriveNumber aDrive )
    {
    TMMCScBkupDriveAndSize* ret = NULL;
    //
    const TInt count = iEntries.Count();
    for( TInt i=0; i<count; i++ )
        {
        TMMCScBkupDriveAndSize& entry = iEntries[ i ];
        //
        if  ( entry.Drive() == aDrive )
            {
            ret = &entry;
            break;
            }
        }
    //
    return ret;
    }










































// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupDataTypeSizer::CMMCScBkupDataTypeSizer()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupDataTypeSizer::CMMCScBkupDataTypeSizer()
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataTypeSizer::~CMMCScBkupDataTypeSizer()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupDataTypeSizer::~CMMCScBkupDataTypeSizer()
    {
    iSizers.Close();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataTypeSizer::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataTypeSizer::ConstructL()
    {
    iSizers.AppendL( CMMCScBkupDriveSizer::NewLC( EMMCScBkupOwnerDataTypeDataOwner ) );
    CleanupStack::Pop(); // sizer
    iSizers.AppendL( CMMCScBkupDriveSizer::NewLC( EMMCScBkupOwnerDataTypeJavaData ) );
    CleanupStack::Pop(); // sizer
    iSizers.AppendL( CMMCScBkupDriveSizer::NewLC( EMMCScBkupOwnerDataTypePublicData ) );
    CleanupStack::Pop(); // sizer
    iSizers.AppendL( CMMCScBkupDriveSizer::NewLC( EMMCScBkupOwnerDataTypeSystemData ) );
    CleanupStack::Pop(); // sizer
    iSizers.AppendL( CMMCScBkupDriveSizer::NewLC( EMMCScBkupOwnerDataTypeActiveData ) );
    CleanupStack::Pop(); // sizer
    iSizers.AppendL( CMMCScBkupDriveSizer::NewLC( EMMCScBkupOwnerDataTypePassiveData ) );
    CleanupStack::Pop(); // sizer
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataTypeSizer::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupDataTypeSizer* CMMCScBkupDataTypeSizer::NewL()
    {
    CMMCScBkupDataTypeSizer* self = new(ELeave) CMMCScBkupDataTypeSizer();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataTypeSizer::NewLC()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupDataTypeSizer* CMMCScBkupDataTypeSizer::NewLC( RReadStream& aStream )
    {
    CMMCScBkupDataTypeSizer* self = new(ELeave) CMMCScBkupDataTypeSizer();
    CleanupStack::PushL(self);
    aStream >> *self;
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataTypeSizer::AddToSizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataTypeSizer::AddToSizeL( TMMCScBkupOwnerDataType aDataType, TInt64 aAmount, TDriveNumber aDrive )
    {
    CMMCScBkupDriveSizer* sizer = SizerByDataType( aDataType );
    ASSERT( sizer != NULL );
    sizer->AddToSizeL( aAmount, aDrive );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataTypeSizer::Reset()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataTypeSizer::Reset( TMMCScBkupOwnerDataType aDataType )
    {
    CMMCScBkupDriveSizer* sizer = SizerByDataType( aDataType );
    ASSERT( sizer != NULL );
    sizer->Reset();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataTypeSizer::Reset()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataTypeSizer::Reset( TMMCScBkupOwnerDataType aDataType, TDriveNumber aDrive )
    {
    CMMCScBkupDriveSizer* sizer = SizerByDataType( aDataType );
    ASSERT( sizer != NULL );
    sizer->Reset( aDrive );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataTypeSizer::Size()
// 
// 
// ---------------------------------------------------------------------------
TInt64 CMMCScBkupDataTypeSizer::Size( TMMCScBkupOwnerDataType aDataType ) const
    {
    CMMCScBkupDriveSizer* sizer = const_cast< CMMCScBkupDataTypeSizer* >( this )->SizerByDataType( aDataType );
    ASSERT( sizer != NULL );
    const TInt64 size = sizer->Size();
    return size;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataTypeSizer::Size()
// 
// 
// ---------------------------------------------------------------------------
TInt64 CMMCScBkupDataTypeSizer::Size( TMMCScBkupOwnerDataType aDataType, TDriveNumber aDrive ) const
    {
    CMMCScBkupDriveSizer* sizer = const_cast< CMMCScBkupDataTypeSizer* >( this )->SizerByDataType( aDataType );
    ASSERT( sizer != NULL );
    const TInt64 size = sizer->Size( aDrive );
    return size;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataTypeSizer::GetSizesL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataTypeSizer::GetSizesL( RArray<TMMCScBkupDriveAndSize>& aSizes ) const
    {
    aSizes.Reset();
    //
    for( TInt d = EDriveA; d<=EDriveZ; d++ )
        {
        // Get the size of each data type for this drive.
        const TDriveNumber drive = static_cast< TDriveNumber >( d );
        TMMCScBkupDriveAndSize driveAndSize( drive, 0 );

        const TInt count = iSizers.Count();
        for( TInt i=0; i<count; i++ )
            {
            const CMMCScBkupDriveSizer* sizer = iSizers[ i ];
            const TInt sizeOfDataForDrive = sizer->Size( drive );
            //
            driveAndSize.AddToSize( sizeOfDataForDrive );
            }

        // Add the entry if the size is more than 0
        if  ( driveAndSize.Size() > 0 )
            {
            aSizes.AppendL( driveAndSize );
            }
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataTypeSizer::CombinedSize()
// 
// 
// ---------------------------------------------------------------------------
TInt64 CMMCScBkupDataTypeSizer::CombinedSize( TDriveNumber aDrive ) const
    {
    TInt64 size = 0;
    //
    const TInt count = iSizers.Count();
    for( TInt i=0; i<count; i++ )
        {
        const CMMCScBkupDriveSizer* sizer = iSizers[ i ];
        size += sizer->Size( aDrive );
        }
    //
    return size;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataTypeSizer::InternalizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataTypeSizer::InternalizeL(RReadStream& aStream)
    {
    aStream.ReadInt32L(); // EStreamFormatVersion1 
    aStream.ReadInt32L(); // spare1
    aStream.ReadInt32L(); // spare2
    //
    const TInt count = aStream.ReadInt8L();
    for( TInt i=0; i<count; i++ )
        {
        CMMCScBkupDriveSizer* sizer = CMMCScBkupDriveSizer::NewLC( aStream );
        iSizers.AppendL( sizer );
        CleanupStack::Pop( sizer );
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataTypeSizer::ExternalizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataTypeSizer::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteInt32L( EStreamFormatVersion1 );
    aStream.WriteInt32L( 0 ); // spare1
    aStream.WriteInt32L( 0 ); // spare2
    //
    const TInt count = iSizers.Count();
    aStream.WriteInt8L( count );
    //
    for( TInt i=0; i<count; i++ )
        {
        CMMCScBkupDriveSizer* sizer = iSizers[ i ];
        aStream << *sizer;
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataTypeSizer::SizerByDataType()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupDriveSizer* CMMCScBkupDataTypeSizer::SizerByDataType( TMMCScBkupOwnerDataType aDataType )
    {
    CMMCScBkupDriveSizer* ret = NULL;
    //
    const TInt count = iSizers.Count();
    for( TInt i=0; i<count; i++ )
        {
        CMMCScBkupDriveSizer* sizer = iSizers[ i ];
        //
        if  ( sizer->DataType() == aDataType )
            {
            ret = sizer;
            break;
            }
        }
    //
    return ret;
    }

