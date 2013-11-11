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
* Description: CMMCScBkupStateGetDataOwners implementation
*
*
*/

#include "CMMCScBkupStateGetDataOwners.h"

#include <bldvariant.hrh>
// User includes
#include "MMCScBkupLogger.h"
#include "CMMCScBkupDataOwnerCollection.h"
#include "MMCScBkupSBEUtils.h"

// Constants
const TInt KMMCScBkupDataOwnerGranularity = 20;



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupStateGetDataOwners::CMMCScBkupStateGetDataOwners()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateGetDataOwners::CMMCScBkupStateGetDataOwners( MMMCScBkupDriver& aDriver )
:   CMMCScBkupState( aDriver )
    {
    __LOG1("CMMCScBkupStateGetDataOwners::CMMCScBkupStateGetDataOwners() - 0x%08x", StateId().iUid );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateGetDataOwners::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateGetDataOwners* CMMCScBkupStateGetDataOwners::NewL( MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupStateGetDataOwners* self = new(ELeave) CMMCScBkupStateGetDataOwners( aDriver );
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateGetDataOwners::StateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateGetDataOwners::StateId() const
    {
    return KMMCScBkupStateIdGetDataOwners;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateGetDataOwners::NextStateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateGetDataOwners::NextStateId() const
    {
    // This is the same for backup & restore
    return KMMCScBkupStateIdSetPhoneMode;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateGetDataOwners::PerformStateInitL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateGetDataOwners::PerformStateInitL()
    {
    // Query asynchronously
    CompleteSelf();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateGetDataOwners::PerformAsynchronousStateStepL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateGetDataOwners::PerformAsynchronousStateStepL()
    {
    RMMCScBkupPointerArray<CDataOwnerInfo> dataOwners(KMMCScBkupDataOwnerGranularity);
    CleanupClosePushL( dataOwners );

    // Get data owner listing from secure backup engine
    Driver().DrvSecureBackupClient().ListOfDataOwnersL(dataOwners);

    // Inform driver of result
#ifdef RD_FILE_MANAGER_BACKUP
    RPointerArray<CMMCScBkupDataOwnerCollection>& dataOwnerLists = Driver().DrvDataOwnersAll();
    TInt count = dataOwners.Count();
    
    // Construct an info array to get introduced for every category
    for(TInt i = count-1; i >= 0; i--)
        {
        // Ownership is immediately transferred to the backup owner info object
        // so we should remove it from the array prior to passing into NewLC
        conn::CDataOwnerInfo* sbDataOwner = dataOwners[i];
        
        dataOwners.Remove( i ); // Ensures it won't be deleted twice
        
        const TSBDerivedType type = sbDataOwner->Identifier().DerivedTypeL();
        if  ( type == EJavaDerivedType || type == EJavaTransferDerivedType )
            {
            HBufC* hash = MMCScBkupSBEUtils::JavaHashFromGenericLC( sbDataOwner->Identifier() );
            __LOG1("CMMCScBkupStateGetDataOwners::PerformAsynchronousStateStepL() - Assigning java owner with hash %S for categories", 
                hash );
            CleanupStack::PopAndDestroy( hash );
            }
        else
            {
            __LOG1("CMMCScBkupStateGetDataOwners::PerformAsynchronousStateStepL() - Assigning data owner 0x%08x for categories", 
                MMCScBkupSBEUtils::SecureIdFromGenericL( sbDataOwner->Identifier() ).iId );
            }

        // We have to create copy of original data owner for each category, so we have to externalize it
        RPointerArray<CDataOwnerInfo> sbDataOwnerCopies(dataOwnerLists.Count());
        CleanupClosePushL( sbDataOwnerCopies );
        HBufC8* dataOwner = sbDataOwner->ExternaliseL();
        CleanupStack::PushL( dataOwner );
        // Delete original data owner instance 
        delete sbDataOwner;
        
        // Create copies for each category (even if they do not eventually end up to those lists)
        for(TInt j = 0; j < dataOwnerLists.Count(); j++)
            {
            conn::CDataOwnerInfo* sbDataOwnerCopy = conn::CDataOwnerInfo::NewL(*dataOwner);
            CleanupStack::PushL( sbDataOwnerCopy );
            sbDataOwnerCopies.AppendL(sbDataOwnerCopy);
            CleanupStack::Pop( sbDataOwnerCopy );
            }
        CleanupStack::PopAndDestroy( dataOwner );
        
        // Loop through all lists and assign copied data owners there
        for(TInt j = 0; j < dataOwnerLists.Count(); j++)
            {
            // Again we have to create instances for each category, 
            // because each has to have own copy of data owner
            CMMCScBkupDataOwnerInfo* info = CMMCScBkupDataOwnerInfo::NewLC( sbDataOwnerCopies[j] );
            
            if(dataOwnerLists[j]->AssignL( *info ))
                {
                CleanupStack::Pop( info );
                }
            else
                {
                CleanupStack::PopAndDestroy( info );
                }
            }
            
            CleanupStack::PopAndDestroy( &sbDataOwnerCopies );
        }
    
#if defined(__MMCSCBKUPLOGGING_ENABLED__)
    __LOG("CMMCScBkupStateGetDataOwners::PerformAsynchronousStateStepL() - Data owners per categories:");
    
    for(TInt i = 0; i < dataOwnerLists.Count(); i++)
        {
        __LOG2("CMMCScBkupStateGetDataOwners::PerformAsynchronousStateStepL() - Category 0x%x contains %d data owners", 
            dataOwnerLists[i]->Category().iFlags, dataOwnerLists[i]->Count() );
        
        for(TInt j = 0; j < dataOwnerLists[i]->Count(); j++)
            {
            CMMCScBkupDataOwnerInfo* entry = &dataOwnerLists[i]->Owner(j);
            
            const TSBDerivedType type = entry->Owner().Identifier().DerivedTypeL();
            if  ( type == EJavaDerivedType || type == EJavaTransferDerivedType )
                {
                HBufC* hash = MMCScBkupSBEUtils::JavaHashFromGenericLC( entry->Owner().Identifier() );
                __LOG1("CMMCScBkupStateGetDataOwners::PerformAsynchronousStateStepL() - java data owner with hash %S", 
                    hash );
                CleanupStack::PopAndDestroy( hash );
                }
            else
                {
                __LOG1("CMMCScBkupStateGetDataOwners::PerformAsynchronousStateStepL() - data owner 0x%08x", 
                    MMCScBkupSBEUtils::SecureIdFromGenericL( entry->Owner().Identifier() ).iId );
                }
            }
        }
#endif

#else // RD_FILE_MANAGER_BACKUP
    Driver().DrvDataOwners().AssignL( dataOwners );
#endif // RD_FILE_MANAGER_BACKUP
    
    CleanupStack::PopAndDestroy( &dataOwners );
    }
