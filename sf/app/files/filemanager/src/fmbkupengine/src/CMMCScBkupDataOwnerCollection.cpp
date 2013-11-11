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
* Description: CMMCScBkupDataOwnerCollection implementation
*
*
*/

#include "CMMCScBkupDataOwnerCollection.h"

// User includes
#include "MMCScBkupLogger.h"
#include "TMMCScBkupDriveAndSize.h"
#include "CMMCScBkupDataOwnerInfo.h"
#include "MMCScBkupSBEUtils.h"
#include "MMMCScBkupDriver.h"
#include "CMMCScBkupDriveDataSizeManager.h"
#include "RMMCScBkupProgressSizer.h"
#ifdef RD_FILE_MANAGER_BACKUP
#include "CMMCScBkupArchiveInfo.h"
#include "BkupEngine.hrh"
#endif

// Constants
const TInt KMMCScBkupDataOwnerGranularity = 20;




// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::CMMCScBkupDataOwnerCollection()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupDataOwnerCollection::CMMCScBkupDataOwnerCollection( MMMCScBkupDriver& aDriver, TBitFlags aCategory )
:   iDriver( aDriver ), iOwners(KMMCScBkupDataOwnerGranularity), iCategory( aCategory )
    {
    }

// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::~CMMCScBkupDataOwnerCollection()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupDataOwnerCollection::~CMMCScBkupDataOwnerCollection()
    {
    Reset();
    iOwners.Close();
    delete iRestoreSizer;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataOwnerCollection::ConstructL()
    {
    // The restore sizer data type is not relevant. It holds
    // the combined disk space requirements for all drives
    // for all data types. 
    iRestoreSizer = CMMCScBkupDriveSizer::NewLC( EMMCScBkupOwnerDataTypeAny );
    CleanupStack::Pop( iRestoreSizer );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupDataOwnerCollection* CMMCScBkupDataOwnerCollection::NewL( MMMCScBkupDriver& aDriver, TBitFlags aCategory )
    {
    CMMCScBkupDataOwnerCollection* self = new(ELeave) CMMCScBkupDataOwnerCollection( aDriver, aCategory );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::AssignL()
// 
// 
// ---------------------------------------------------------------------------
#ifdef RD_FILE_MANAGER_BACKUP
TBool CMMCScBkupDataOwnerCollection::AssignL( const CMMCScBkupDataOwnerInfo& dataOwnerInfo )
    {
    const CMMCScBkupArchiveInfo& archive = iDriver.DrvParamsBase().ArchiveInfo( Category() );
    const RArray<TSecureId>& secureIds = archive.SIDs( Category() );
    const RArray<TSecureId>& excludedSecureIds = archive.ExcludedSIDs( Category() );
    TBitFlags flags = archive.SpecialFlags();
    TBitFlags excludedFlags = archive.ExcludedSpecialFlags();

    // Check whether data owner belongs to this category
    if( BelongsToL( dataOwnerInfo, flags, excludedFlags, secureIds, excludedSecureIds) )
        {
        iOwners.AppendL(&dataOwnerInfo);
        return ETrue;
        }
    
    return EFalse;
    }
#else
void CMMCScBkupDataOwnerCollection::AssignL( RDataOwnerInfoArray& aArray )
    {
    const TInt count = aArray.Count();
    //
    for(TInt i=count-1; i>=0; i--)
        {
        // Ownership is immediately transferred to the backup owner info object
        // so we should remove it from the array prior to passing into NewLC
        conn::CDataOwnerInfo* sbDataOwner = aArray[i];
        aArray.Remove(i); // Ensures it won't be deleted twice
        //
        CMMCScBkupDataOwnerInfo* info = CMMCScBkupDataOwnerInfo::NewLC( sbDataOwner );
        iOwners.AppendL(info);
        CleanupStack::Pop(info);
        }
    }
#endif

// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::AppendL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataOwnerCollection::AppendL( CMMCScBkupDataOwnerInfo* aNewEntry )
    {
    iOwners.AppendL( aNewEntry );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::Count()
// 
// 
// ---------------------------------------------------------------------------
TInt CMMCScBkupDataOwnerCollection::Count() const
    {
    return iOwners.Count();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::Owner()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupDataOwnerInfo& CMMCScBkupDataOwnerCollection::Owner(TInt aIndex)
    {
    CMMCScBkupDataOwnerInfo* info = iOwners[aIndex];
    return *info;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::Owner()
// 
// 
// ---------------------------------------------------------------------------
const CMMCScBkupDataOwnerInfo& CMMCScBkupDataOwnerCollection::Owner(TInt aIndex) const
    {
    const CMMCScBkupDataOwnerInfo* info = iOwners[aIndex];
    return *info;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::OwnerL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupDataOwnerInfo& CMMCScBkupDataOwnerCollection::OwnerL( TSecureId aSID )
    {
    TInt index = KErrNotFound;
    CMMCScBkupDataOwnerInfo* ret = CMMCScBkupDataOwnerInfo::New( aSID );
    if  (ret)
        {
        TIdentityRelation<CMMCScBkupDataOwnerInfo> relation(CMMCScBkupDataOwnerInfo::CompareDataOwnerBySIDL);
        index = iOwners.Find( ret, relation );
        delete ret;
        ret = NULL;
        }
    //
    if  (index >= 0)
        {
        ret = iOwners[index];
        }
    else
        {
        User::Leave(KErrNotFound);
        }
    //
    return *ret;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::OwnerL()
// 
// 
// ---------------------------------------------------------------------------
const CMMCScBkupDataOwnerInfo& CMMCScBkupDataOwnerCollection::OwnerL( TSecureId aSID ) const
    {
    CMMCScBkupDataOwnerCollection& self = *const_cast<CMMCScBkupDataOwnerCollection*>(this);
    CMMCScBkupDataOwnerInfo& ret = self.OwnerL( aSID );
    return ret;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::OwnerL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupDataOwnerInfo& CMMCScBkupDataOwnerCollection::OwnerL( TUid aPackageId )
    {
    CMMCScBkupDataOwnerInfo* ret = NULL;
    const TInt count = iOwners.Count();
    //
    for(TInt i=0; i<count; i++)
        {
        CMMCScBkupDataOwnerInfo* entry = iOwners[ i ];
        //
        const TUid packageId = MMCScBkupSBEUtils::PackageIdFromGenericL( entry->Owner().Identifier() );
        if  ( packageId == aPackageId )
            {
            ret = entry;
            }
        }
    //
    if  ( !ret )
        {
        User::Leave(KErrNotFound);
        }
    //
    return *ret;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::OwnerL()
// 
// 
// ---------------------------------------------------------------------------
const CMMCScBkupDataOwnerInfo& CMMCScBkupDataOwnerCollection::OwnerL( TUid aPackageId ) const
    {
    CMMCScBkupDataOwnerCollection& self = *const_cast<CMMCScBkupDataOwnerCollection*>(this);
    CMMCScBkupDataOwnerInfo& ret = self.OwnerL( aPackageId );
    return ret;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::OwnerL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupDataOwnerInfo& CMMCScBkupDataOwnerCollection::OwnerL( const TDesC& aHash )
    {
    CMMCScBkupDataOwnerInfo* ret = NULL;
    const TInt count = iOwners.Count();
    //
    for(TInt i=0; i<count; i++)
        {
        CMMCScBkupDataOwnerInfo* entry = iOwners[ i ];
        
        // Check if its a java item...
        const TSBDerivedType type = entry->Owner().Identifier().DerivedTypeL();
        if  ( type == EJavaDerivedType || type == EJavaTransferDerivedType )
            {
            // Get hash
            HBufC* hash = MMCScBkupSBEUtils::JavaHashFromGenericLC( entry->Owner().Identifier() );
            const TBool foundMatch = ( *hash == aHash );
            CleanupStack::PopAndDestroy( hash );

            if  ( foundMatch )
                {
                ret = entry;
                break;
                }
            }
        }
    //
    if  ( !ret )
        {
        User::Leave(KErrNotFound);
        }
    //
    return *ret;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::OwnerL()
// 
// 
// ---------------------------------------------------------------------------
const CMMCScBkupDataOwnerInfo& CMMCScBkupDataOwnerCollection::OwnerL( const TDesC& aHash ) const
    {
    CMMCScBkupDataOwnerCollection& self = *const_cast<CMMCScBkupDataOwnerCollection*>(this);
    CMMCScBkupDataOwnerInfo& ret = self.OwnerL( aHash );
    return ret;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::Remove()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataOwnerCollection::Remove( TInt aIndex )
    {
    CMMCScBkupDataOwnerInfo* info = iOwners[aIndex];
    delete info;
    iOwners.Remove(aIndex);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::Reset()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataOwnerCollection::Reset()
    {
    iOwners.ResetAndDestroy();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::TotalOperationalSizeL()
// 
// 
// ---------------------------------------------------------------------------
TInt64 CMMCScBkupDataOwnerCollection::TotalOperationalSizeL() const
    {
    RMMCScBkupProgressSizer sizer( iDriver.DrvParamsBase().DriveAndOperations() );
    TInt64 size = 0;
    //
    const TInt count = Count();
    for(TInt i=0; i<count; i++)
        {
        const CMMCScBkupDataOwnerInfo& owner = Owner( i );
        //
        const TInt64 restoreSizeForDO = sizer.RestoreCombinedDataSizeL( owner );

        __LOG2("CMMCScBkupDataOwnerCollection::TotalOperationalSizeL() - data transfer amount required for DO: 0x%08x is: %8Ld", owner.SecureId().iId, restoreSizeForDO );

        size += restoreSizeForDO;
        }
    //
    return size;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::RebootRequired()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupDataOwnerCollection::RebootRequired() const
    {
    TBool rebootRequired = EFalse;
    //
    const TInt count = iOwners.Count();
    for(TInt i=count-1; i>=0; i--)
        {
        // We check the SBE's common settings (for each Data Owner) to identify
        // if a reboot is required. So long as one data owner requires a
        // reboot, then we must reset the machine.. hence we break
        // out of the loop immediately.
        const CMMCScBkupDataOwnerInfo& info = *iOwners[i];
        const CDataOwnerInfo& sbeDataOwnerInfo = info.Owner();
        //
        if  ( sbeDataOwnerInfo.CommonSettings() & ERequiresReboot )
            {
            rebootRequired = ETrue;
            break;
            }
        }
    //
    return rebootRequired;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::DiskSpaceRequiredForRestore()
// 
// 
// ---------------------------------------------------------------------------
TInt64 CMMCScBkupDataOwnerCollection::DiskSpaceRequiredForRestore( TDriveNumber aDrive ) const
    {
    return iRestoreSizer->Size( aDrive );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::CalculateDiskSpaceRequiredForRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataOwnerCollection::CalculateDiskSpaceRequiredForRestoreL()
    {
    iRestoreSizer->Reset();
    //
    RArray<TMMCScBkupDriveAndSize> driveSizes;
    CleanupClosePushL( driveSizes );
    //
    const TInt count = iOwners.Count();
    for(TInt i=count-1; i>=0; i--)
        {
        const CMMCScBkupDataOwnerInfo& owner = *iOwners[i];
        __LOG(" ");
        __LOG1("CMMCScBkupDataOwnerCollection::CalculateDiskSpaceRequiredForRestoreL() - owner: 0x%08x...", owner.SecureId().iId);
        //
        owner.OperationalSizesL( driveSizes );
        //
        const TInt driveSizesCount = driveSizes.Count();
        for( TInt j=0; j<driveSizesCount; j++ )
            {
            const TMMCScBkupDriveAndSize& entry = driveSizes[ j ];
            __LOG2("CMMCScBkupDataOwnerCollection::CalculateDiskSpaceRequiredForRestoreL() -    drive: %c:, size: %8Ld", entry.Drive() + 'A', entry.Size());
            //
            iRestoreSizer->AddToSizeL( entry.Size(), entry.Drive() );
            }
        }
    //
    CleanupStack::PopAndDestroy( &driveSizes );

#ifdef __MMCSCBKUPLOGGING_ENABLED__
    __LOG(" ");
    __LOG(" ");
    __LOG(" ");
    __LOG(" ");
    __LOG("CMMCScBkupDataOwnerCollection::CalculateDiskSpaceRequiredForRestoreL() - SUMMARY:");
    __LOG(" ");

    for( TInt d = EDriveA; d<=EDriveZ; d++ )
        {
        // Get the size of each data type for this drive.
        const TDriveNumber drive = static_cast< TDriveNumber >( d );
        const TInt64 size = DiskSpaceRequiredForRestore( drive );

        if  ( size > 0 )
            {
            __LOG2("CMMCScBkupDataOwnerCollection::CalculateDiskSpaceRequiredForRestoreL() - %8Ld bytes required for drive: %c:", size, drive + 'A' );
            }
        }
#endif
    }

#ifdef RD_FILE_MANAGER_BACKUP
// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::BelongsToL()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupDataOwnerCollection::BelongsToL(const CMMCScBkupDataOwnerInfo& aInfo, 
    TBitFlags aFlags, TBitFlags aExcludedFlags,
    const RArray<TSecureId> aSecureIds, const RArray<TSecureId> aExcludedSecureIds) const
    {
    // First of all check if data owner has system or java data. If it has and these
    // types have been declared as excluded data types, then skip owner in this catecory.
    // Public data is more common to data owners together with other data types meaning it
    // cannot be a reason to invalidate data owner. System's user data folder (e.g. C:\Data) files
    // are being skipped later on in public data backup phase if category is not EBUCatUserFiles
    // It would be possible to just skip excluded types per data owner, but it
    // might make data owner unstable after restore

    if( ((aExcludedFlags.Value() & EBUCatSpecSystem) && aInfo.HasSystemDataL()) ||
        ((aExcludedFlags.Value() & EBUCatSpecJava) && aInfo.HasJavaDataL()) ||
        ((aExcludedFlags.Value() & EBUCatSpecPublic) && aInfo.HasPublicDataL()) )
        {
        return EFalse;
        }
        
    // Then check whether all specified and SID not in list of excluded owners
    if( (aFlags.Value() & EBUCatSpecAll) )
        {
        TBool excluded = EFalse;
        
        for(TInt i = 0; i < aExcludedSecureIds.Count(); i++)
            {
            if(aInfo.SecureId().iId == aExcludedSecureIds[i].iId)
                {
                excluded = ETrue;
                break;
                }
            }
            
        return !excluded;
        }
    
    // Then check whether special rules apply
    if( ((aFlags.Value() & EBUCatSpecSystem) && aInfo.HasSystemDataL()) ||
        ((aFlags.Value() & EBUCatSpecJava) && aInfo.HasJavaDataL()) ||
        ((aFlags.Value() & EBUCatSpecPublic) && aInfo.HasPublicDataL()) )
        {
        return ETrue;
        }
        
    // Finally check whether SID matches
    for(TInt i = 0; i < aSecureIds.Count(); i++)
        {
        if(aInfo.SecureId().iId == aSecureIds[i].iId)
            {
            return ETrue;
            }
        }
        
    return EFalse;
    }
#endif

// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::InternalizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataOwnerCollection::InternalizeL( RReadStream& aStream )
    {
    aStream.ReadInt32L(); // EStreamFormatVersion1 
    aStream.ReadInt32L(); // spare1
    aStream.ReadInt32L(); // spare2
    aStream.ReadInt32L(); // spare3

    CMMCScBkupDriveSizer* restoreSizer = CMMCScBkupDriveSizer::NewLC( aStream );
    delete iRestoreSizer;
    iRestoreSizer = restoreSizer;
    CleanupStack::Pop( restoreSizer );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataOwnerCollection::ExternalizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataOwnerCollection::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( EStreamFormatVersion1 );
    aStream.WriteInt32L( 0 ); // spare1
    aStream.WriteInt32L( 0 ); // spare2
    aStream.WriteInt32L( 0 ); // spare3

    aStream << *iRestoreSizer;
    }




