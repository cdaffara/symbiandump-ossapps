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
* Description: CMMCScBkupDataOwnerInfo implementation
*
*
*/

#include "CMMCScBkupDataOwnerInfo.h"

// System includes
#include <s32strm.h>

// User includes
#include "MMCScBkupLogger.h"
#include "MMCScBkupSBEUtils.h"
#include "MMMCScBkupArchiveDataInterface.h"
#include "CMMCScBkupDriveDataSizeManager.h"

// Constants
_LIT_SECURE_ID(KMMCScBkupUninitialisedSecureId, 0);


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::CMMCScBkupDataOwnerInfo()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupDataOwnerInfo::CMMCScBkupDataOwnerInfo( CDataOwnerInfo* aOwner )
:   iDataOwner(aOwner), iVersion(EStreamFormatVersionLatest-1)
    {
    SetStatus(EUnset);
    //
    iSecureId = KMMCScBkupUninitialisedSecureId;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::~CMMCScBkupDataOwnerInfo()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupDataOwnerInfo::~CMMCScBkupDataOwnerInfo()
    {
    delete iDataOwner;
    delete iOperationalSize;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataOwnerInfo::ConstructL()
    {
    if  ( iOperationalSize == NULL )
        {
        iOperationalSize = CMMCScBkupDataTypeSizer::NewL();
        }

    // Try and extract the SID if its a package or a SID type
    const TSBDerivedType type = iDataOwner->Identifier().DerivedTypeL();
    //
    if  (type == ESIDDerivedType || type == EPackageDerivedType)
        {
        iSecureId = MMCScBkupSBEUtils::SecureIdFromGenericL(iDataOwner->Identifier());
        if( iSecureId.iId == KNullUid.iUid && iVersion > EStreamFormatVersion1 )
            {
            iSecureId = MMCScBkupSBEUtils::PackageIdFromGenericL(iDataOwner->Identifier());
            }
        }

    // The following code sets the completion status to ETrue
    // for any irrelevant items associated with a data owner. I.e. if
    // a data owner doesn't support active backup, then it sets the completion
    // of active backup to ETrue. This makes writing the logic inside the
    // active/passive/system backup items a little easier (active in particular)
    const TBool hasJavaData = HasJavaDataL();
    const TBool hasActiveData = HasActiveDataL();
    const TBool hasPassiveData = HasPassiveDataL();
    const TBool hasPublicData = HasPublicDataL();
    const TBool hasSystemData = HasSystemDataL();

    // Update completion statuses (i.e. "what work must be done")
    SetCompletionStatus( EMMCScBkupOwnerDataTypeJavaData, !hasJavaData );
    SetCompletionStatus( EMMCScBkupOwnerDataTypeActiveData, !hasActiveData );
    SetCompletionStatus( EMMCScBkupOwnerDataTypePassiveData, !hasPassiveData );
    SetCompletionStatus( EMMCScBkupOwnerDataTypePublicData, !hasPublicData );
    SetCompletionStatus( EMMCScBkupOwnerDataTypeSystemData, !hasSystemData );
    
    // Be aware that CMMCScBkupStateRequestSizeOfDataOwner::GetJavaDataSizesL() can
    // manipulate the passive data completion status depending on whether the midlet
    // has associated data files.
    
    // If we're logging, then we write out some useful debugging info that shows
    // which operations must be performed for each D.O.
#ifdef __MMCSCBKUPLOGGING_ENABLED__
    TBuf<256> drivePrint;
    const TDriveList& driveList = iDataOwner->DriveList();
    
    for(TInt i=0; i<KMaxDrives; i++)
        {
        if  (driveList[i] != 0)
            {
            const TDriveUnit driveUnit(i);
            const TDriveName name(driveUnit.Name());
            drivePrint.Append(name);
            if  (i < KMaxDrives - 1)
                {
                drivePrint.Append(_L(", "));
                }
            }
        }

    if ( hasJavaData )
        {
        HBufC* hash = MMCScBkupSBEUtils::JavaHashFromGenericLC( iDataOwner->Identifier() );
        __LOG7("CMMCScBkupDataOwnerInfo::ConstructL() - PubD: %d, ActD: %d, PasD: %d, JavD: %d, SysD: %d, drives: %S, hash: %S", hasPublicData, hasActiveData, hasPassiveData, hasJavaData, hasSystemData, &drivePrint, hash);
        CleanupStack::PopAndDestroy( hash );
        }
    else
        {
        __LOG8("CMMCScBkupDataOwnerInfo::ConstructL() - PubD: %d, ActD: %d, PasD: %d, JavD: %d, SysD: %d, drives: %S, SID : 0x%08x %S", hasPublicData, hasActiveData, hasPassiveData, hasJavaData, hasSystemData, &drivePrint, iSecureId.iId, &MMCScBkupLogger::FriendlyNameForSID( iSecureId ));
        }
#endif
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::New()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupDataOwnerInfo* CMMCScBkupDataOwnerInfo::New( TSecureId aSecureId )
    {
    CMMCScBkupDataOwnerInfo* self = new CMMCScBkupDataOwnerInfo( NULL );
    if  (self)
        {
        self->iSecureId = aSecureId;
        }
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::NewLC()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupDataOwnerInfo* CMMCScBkupDataOwnerInfo::NewLC( CDataOwnerInfo* aOwner )
    {
    CleanupStack::PushL(aOwner);
    CMMCScBkupDataOwnerInfo* self = new(ELeave) CMMCScBkupDataOwnerInfo( aOwner );
    CleanupStack::Pop(aOwner);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::NewLC()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupDataOwnerInfo* CMMCScBkupDataOwnerInfo::NewLC( RReadStream& aStream )
    {
    CMMCScBkupDataOwnerInfo* self = new(ELeave) CMMCScBkupDataOwnerInfo();
    CleanupStack::PushL(self);
    aStream >> *self;
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::Owner()
// 
// 
// ---------------------------------------------------------------------------
CDataOwnerInfo& CMMCScBkupDataOwnerInfo::Owner()
    {
    return *iDataOwner;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::Owner()
// 
// 
// ---------------------------------------------------------------------------
const CDataOwnerInfo& CMMCScBkupDataOwnerInfo::Owner() const
    {
    return *iDataOwner;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::Status()
// 
// 
// ---------------------------------------------------------------------------
TDataOwnerStatus CMMCScBkupDataOwnerInfo::Status() const
    {
    return iStatus;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::SetStatus()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataOwnerInfo::SetStatus(TDataOwnerStatus aStatus)
    {
    iStatus = aStatus;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::AddToOperationalSizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataOwnerInfo::AddToOperationalSizeL( TMMCScBkupOwnerDataType aType, TDriveNumber aDrive, TInt64 aSize )
    {
    iOperationalSize->AddToSizeL( aType, aSize, aDrive );

#if defined(__MMCSCBKUPLOGGING_ENABLED__)
    const TInt64 newSize = iOperationalSize->Size( aType, aDrive );
    __LOG5("CMMCScBkupDataOwnerInfo::AddToOperationalSizeL() - [0x%08x] - aSize: %8Ld, newSize: %8Ld, aDrive: %c:, aType: %S", iSecureId.iId, aSize, newSize, aDrive + 'A', &MMCScBkupLogger::DataType( aType ) );
#endif
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::SetOperationalSizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataOwnerInfo::SetOperationalSizeL( TMMCScBkupOwnerDataType aType, TDriveNumber aDrive, TInt64 aSize )
    {
    iOperationalSize->Reset( aType, aDrive );
    iOperationalSize->AddToSizeL( aType, aSize, aDrive );

    __LOG4("CMMCScBkupDataOwnerInfo::SetOperationalSizeL()   - [0x%08x] - aSize: %8Ld, aDrive: %c:, aType: %S", iSecureId.iId, aSize, aDrive + 'A', &MMCScBkupLogger::DataType( aType ) );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::ResetOperationalSize()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataOwnerInfo::ResetOperationalSize( TMMCScBkupOwnerDataType aType )
    {
    iOperationalSize->Reset( aType );
    __LOG2("CMMCScBkupDataOwnerInfo::ResetOperationalSize()  - [0x%08x] - aType: %S", iSecureId.iId, &MMCScBkupLogger::DataType( aType ) );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::OperationalSize()
// 
// 
// ---------------------------------------------------------------------------
TInt64 CMMCScBkupDataOwnerInfo::OperationalSize( TMMCScBkupOwnerDataType aType, TDriveNumber aDrive ) const
    {
    const TInt64 size = iOperationalSize->Size( aType, aDrive );

    __LOG4("CMMCScBkupDataOwnerInfo::OperationalSize()       - [0x%08x] - size: %8Ld, aDrive: %c:, aType: %S", iSecureId.iId, size, aDrive + 'A', &MMCScBkupLogger::DataType( aType ) );

    return size;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::OperationalSize()
// 
// 
// ---------------------------------------------------------------------------
TInt64 CMMCScBkupDataOwnerInfo::OperationalSize( TMMCScBkupOwnerDataType aType ) const
    {
    const TInt64 size = iOperationalSize->Size( aType );

    //__LOG3("CMMCScBkupDataOwnerInfo::OperationalSize()       - [0x%08x] - size: %8Ld, aType: %S", iSecureId.iId, size, &MMCScBkupLogger::DataType( aType ) );

    return size;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::OperationalSize()
// 
// 
// ---------------------------------------------------------------------------
TInt64 CMMCScBkupDataOwnerInfo::OperationalSize( TDriveNumber aDrive ) const
    {
    const TInt64 size = iOperationalSize->CombinedSize( aDrive );

    //__LOG3("CMMCScBkupDataOwnerInfo::OperationalSize()       - [0x%08x] - size: %8Ld, aDrive: %c:", iSecureId.iId, size, aDrive + 'A' );

    return size;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::OperationalSizesL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataOwnerInfo::OperationalSizesL( RArray<TMMCScBkupDriveAndSize>& aSizes ) const
    {
    iOperationalSize->GetSizesL( aSizes );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::SetCompletionStatus()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataOwnerInfo::SetCompletionStatus( TMMCScBkupOwnerDataType aType, TBool aCompleted )
    {
    iCompletionStatus[aType] = aCompleted;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::CompletionStatus()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupDataOwnerInfo::CompletionStatus( TMMCScBkupOwnerDataType aType ) const
    {
    return iCompletionStatus[aType];
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::ActiveDataRetryCount()
// 
// 
// ---------------------------------------------------------------------------
TInt CMMCScBkupDataOwnerInfo::ActiveDataRetryCount() const
    {
    return iActiveDataRetryCount;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::SetActiveDataRetryCount()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataOwnerInfo::SetActiveDataRetryCount(TInt aCount)
    {
    iActiveDataRetryCount = aCount;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::InternalizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataOwnerInfo::InternalizeL(RReadStream& aStream)
    {
    iVersion = aStream.ReadInt32L(); // EStreamFormatVersionX
    aStream.ReadInt32L(); // spare1
    aStream.ReadInt32L(); // spare2
    aStream.ReadInt32L(); // spare3

    // Secure ID
    iSecureId = aStream.ReadInt32L();
    
    // Data sizes
    CMMCScBkupDataTypeSizer* operationalSize = CMMCScBkupDataTypeSizer::NewLC( aStream );
    delete iOperationalSize;
    iOperationalSize = operationalSize;
    CleanupStack::Pop( operationalSize );
   
    // Read data owner buffer length
    TCardinality dataOwnerLength;
    aStream >> dataOwnerLength;

    // Internalize the data owner (as raw descriptor)
    HBufC8* dataOwnerBuffer = HBufC8::NewLC( dataOwnerLength );
    TPtr8 pDataOwnerBuffer( dataOwnerBuffer->Des() );
    aStream.ReadL( pDataOwnerBuffer, dataOwnerLength );
    //
    CDataOwnerInfo* dataOwner = CDataOwnerInfo::NewL( *dataOwnerBuffer );
    delete iDataOwner;
    iDataOwner = dataOwner;
    CleanupStack::PopAndDestroy( dataOwnerBuffer );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::ExternalizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataOwnerInfo::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteInt32L( EStreamFormatVersion2 );
    aStream.WriteInt32L( 0 ); // spare1
    aStream.WriteInt32L( 0 ); // spare2
    aStream.WriteInt32L( 0 ); // spare3

    // Always write secure id
    aStream.WriteInt32L( iSecureId );

    // ... and always write the data sizes
    aStream << *iOperationalSize;

    // Externalize the data owner to an HBufC8
    HBufC8* dataOwnerBuffer = iDataOwner->ExternaliseL();
    CleanupStack::PushL( dataOwnerBuffer );

    // Write leading byte count - helps us when it comes to internalising
    const TCardinality dataOwnerLength( dataOwnerBuffer->Length() );
    aStream << dataOwnerLength;
    
    // Now write the data owner buffer - not using chevrons just in case it
    // does funny unicode compression stuff...
    aStream.WriteL( *dataOwnerBuffer );

    CleanupStack::PopAndDestroy(dataOwnerBuffer);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::CompareDataOwnerBySIDL()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupDataOwnerInfo::CompareDataOwnerBySIDL(const CMMCScBkupDataOwnerInfo& aLeft, const CMMCScBkupDataOwnerInfo& aRight)
    {
    TBool same = EFalse;
    
    // If we have an owner object, then we first compare the derived types.
    // If not, we just directly compare the SIDs
    if  ( aLeft.iDataOwner && aRight.iDataOwner )
        {
        const TSBDerivedType leftType = aLeft.Owner().Identifier().DerivedTypeL();
        const TSBDerivedType rightType = aRight.Owner().Identifier().DerivedTypeL();
        //
        same = ((leftType == rightType) || 
                (leftType == ESIDDerivedType && rightType == EPackageDerivedType) || 
                (leftType == EPackageDerivedType && rightType == ESIDDerivedType)
               );
        }
    else if ( aRight.iDataOwner )
        {
        // Check that it is not Java data owner in question. Java DOs all have
        // secure id 0x0, but it could also be the case for certain package
        // data owners (see for KDummyId in SBE implementation).
        if( !MMCScBkupSBEUtils::HasJavaDataL( aRight.Owner() ) )
            {
            // ..set to ETrue to trip next if-statement
            same = ETrue;
            }
        }

    // Now compare the SID's themselves 
    if  ( same )
        {
        same = (aLeft.SecureId() == aRight.SecureId());
        }
    //
    return same;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::HasJavaDataL()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupDataOwnerInfo::HasJavaDataL() const
    {
    const TBool hasJavaData = MMCScBkupSBEUtils::HasJavaDataL( *iDataOwner );
    const TInt64 opSize = OperationalSize( EMMCScBkupOwnerDataTypeJavaData );
    //
    return hasJavaData || (opSize > 0 );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::HasActiveDataL()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupDataOwnerInfo::HasActiveDataL() const
    {
    const TBool hasActiveData = MMCScBkupSBEUtils::HasActiveDataL( *iDataOwner );
    const TInt64 opSize = OperationalSize( EMMCScBkupOwnerDataTypeActiveData );
    //
    return hasActiveData || (opSize > 0 );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::HasPassiveDataL()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupDataOwnerInfo::HasPassiveDataL() const
    {
    const TBool hasPassiveData = MMCScBkupSBEUtils::HasPassiveDataL( *iDataOwner );
    const TInt64 opSize = OperationalSize( EMMCScBkupOwnerDataTypePassiveData );
    //
    return hasPassiveData || (opSize > 0 );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::HasPublicDataL()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupDataOwnerInfo::HasPublicDataL() const
    {
    const TBool hasPublicData = MMCScBkupSBEUtils::HasPublicDataL( *iDataOwner );
    const TInt64 opSize = OperationalSize( EMMCScBkupOwnerDataTypePublicData );
    //
    return hasPublicData || (opSize > 0 );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerInfo::HasSystemDataL()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupDataOwnerInfo::HasSystemDataL() const
    {
    const TBool hasSystemData = MMCScBkupSBEUtils::HasSystemDataL( *iDataOwner );
    const TInt64 opSize = OperationalSize( EMMCScBkupOwnerDataTypeSystemData );
    //
    return hasSystemData || (opSize > 0 );
    }




