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
* Description: CMMCScBkupStateFactory implementation
*
*
*/

#include "CMMCScBkupStateFactory.h"

// System includes
#include <e32err.h>

// User includes
#include "MMCScBkupLogger.h"
#include "CMMCScBkupStateGetDataOwners.h"
#include "CMMCScBkupStateSetPhoneMode.h"
#include "CMMCScBkupStateRequestSizeOfBackupData.h"
#include "CMMCScBkupStateRequestListOfPublicFiles.h"
#include "CMMCScBkupStateGetDataOwnerStatuses.h"
#include "CMMCScBkupStateNotifyAllSnapshotsSupplied.h"
//
#include "CMMCScBkupStateArchiveOpSystemData.h"
#include "CMMCScBkupStateArchiveOpDataOwners.h"
#include "CMMCScBkupStateArchiveOpPublicDataFiles.h"
#include "CMMCScBkupStateArchiveOpActiveData.h"
#include "CMMCScBkupStateArchiveOpJavaData.h"
#include "CMMCScBkupStateArchiveOpPassiveData.h"
#include "CMMCScBkupStateArchiveOpArchiveHeader.h"
#include "CMMCScBkupStateArchiveOpArchiveFooter.h"
#include "CMMCScBkupStateValidateDiskSpace.h"



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupStateFactory::CMMCScBkupStateFactory()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateFactory::CMMCScBkupStateFactory()
    {
    }

// ---------------------------------------------------------------------------
// CMMCScBkupStateFactory::FactoryByOperationTypeLC()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateFactory* CMMCScBkupStateFactory::FactoryByOperationTypeLC( TMMCScBkupOperationType aOperationType )
    {
    CMMCScBkupStateFactory* factory = NULL;
    //
    switch(aOperationType)
        {
    case EMMCScBkupOperationTypeFullBackup:
        factory = CMMCScBkupStateFactoryBackup::NewL();
        break;
    case EMMCScBkupOperationTypeFullRestore:
        factory = CMMCScBkupStateFactoryRestore::NewL();
        break;
    case EMMCScBkupOperationTypePartialBackup:
    case EMMCScBkupOperationTypePartialRestore:
    default:
        __ASSERT_ALWAYS(EFalse, User::Invariant());
        break;
        }
    //
    CleanupStack::PushL(factory);
    return factory;
    }













// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupStateFactoryBackup::CMMCScBkupStateFactoryBackup()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateFactoryBackup::CMMCScBkupStateFactoryBackup()
    {
    }

// ---------------------------------------------------------------------------
// CMMCScBkupStateFactoryBackup::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateFactoryBackup* CMMCScBkupStateFactoryBackup::NewL()
    {
    CMMCScBkupStateFactoryBackup* self = new(ELeave) CMMCScBkupStateFactoryBackup();
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateFactoryBackup::GetStateLC()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupState* CMMCScBkupStateFactoryBackup::GetStateLC( TMMCScBkupStateId aRequiredType, MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupState* state = NULL;
    //
    switch(aRequiredType.iUid)
        {
    case KMMCScBkupStateIdValueArchiveOpArchiveHeader:
        state = CMMCScBkupStateArchiveOpArchiveHeader::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueGetDataOwners:
        state = CMMCScBkupStateGetDataOwners::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueSetPhoneMode:
        state = CMMCScBkupStateSetPhoneMode::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueNotifyAllSnapshotsSupplied:
        state = CMMCScBkupStateNotifyAllSnapshotsSupplied::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueRequestSizeOfBackupData:
        state = CMMCScBkupStateRequestSizeOfBackupData::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueArchiveOpDataOwners:
        state = CMMCScBkupStateArchiveOpDataOwners::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueRequestListOfPublicFiles:
        state = CMMCScBkupStateRequestListOfPublicFiles::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueArchiveOpPublicDataFiles:
        state = CMMCScBkupStateArchiveOpPublicDataFiles::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueArchiveOpSystemData:
        state = CMMCScBkupStateArchiveOpSystemData::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueArchiveOpJavaData:
        state = CMMCScBkupStateArchiveOpJavaData::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueGetDataOwnerStatuses:
        state = CMMCScBkupStateGetDataOwnerStatuses::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueArchiveOpActiveData:
        state = CMMCScBkupStateArchiveOpActiveData::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueArchiveOpPassiveData:
        state = CMMCScBkupStateArchiveOpPassiveData::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueArchiveOpArchiveFooter:
        state = CMMCScBkupStateArchiveOpArchiveFooter::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueValidateDiskSpace:
        state = CMMCScBkupStateValidateDiskSpace::NewL( aDriver );
        break;
    default:
    case KMMCScBkupStateIdValueSupplyDataSnapshots:
        ASSERT( EFalse );
        User::Leave(KErrNotSupported);
        break;
        }
    //
    CleanupStack::PushL(state);
    return state;
    }




























// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupStateFactoryRestore::CMMCScBkupStateFactoryRestore()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateFactoryRestore::CMMCScBkupStateFactoryRestore()
    {
    }

// ---------------------------------------------------------------------------
// CMMCScBkupStateFactoryRestore::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateFactoryRestore* CMMCScBkupStateFactoryRestore::NewL()
    {
    CMMCScBkupStateFactoryRestore* self = new(ELeave) CMMCScBkupStateFactoryRestore();
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateFactoryRestore::GetStateLC()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupState* CMMCScBkupStateFactoryRestore::GetStateLC( TMMCScBkupStateId aRequiredType, MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupState* state = NULL;
    //
    switch(aRequiredType.iUid)
        {
    case KMMCScBkupStateIdValueArchiveOpArchiveHeader:
        state = CMMCScBkupStateArchiveOpArchiveHeader::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueArchiveOpArchiveFooter:
        state = CMMCScBkupStateArchiveOpArchiveFooter::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueArchiveOpDataOwners:
        state = CMMCScBkupStateArchiveOpDataOwners::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueSetPhoneMode:
        state = CMMCScBkupStateSetPhoneMode::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueArchiveOpSystemData:
        state = CMMCScBkupStateArchiveOpSystemData::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueNotifyAllSnapshotsSupplied:
        state = CMMCScBkupStateNotifyAllSnapshotsSupplied::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueArchiveOpJavaData:
        state = CMMCScBkupStateArchiveOpJavaData::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueArchiveOpPassiveData:
        state = CMMCScBkupStateArchiveOpPassiveData::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueGetDataOwnerStatuses:
        state = CMMCScBkupStateGetDataOwnerStatuses::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueArchiveOpActiveData:
        state = CMMCScBkupStateArchiveOpActiveData::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueArchiveOpPublicDataFiles:
        state = CMMCScBkupStateArchiveOpPublicDataFiles::NewL( aDriver );
        break;
    case KMMCScBkupStateIdValueValidateDiskSpace:
        state = CMMCScBkupStateValidateDiskSpace::NewL( aDriver );
        break;
    default:
        ASSERT( EFalse );
        User::Leave(KErrNotSupported);
        break;
        }
    //
    CleanupStack::PushL(state);
    return state;
    }









































