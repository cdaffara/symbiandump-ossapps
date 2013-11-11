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
* Description: MMCScBkupSBEUtils implementation
*
*
*/

#include "MMCScBkupSBEUtils.h"

// System includes
#include <e32property.h>
#include <connect/sbdefs.h>
#include "CMMCScBkupDataOwnerInfo.h"

// Namespaces
using namespace conn;



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// MMCScBkupSBEUtils::SecureIdFromGenericL()
// 
// 
// ---------------------------------------------------------------------------
TSecureId MMCScBkupSBEUtils::SecureIdFromGenericL(const CSBGenericDataType& aGeneric)
    {
    TSecureId ret = 0;
    CSBGenericDataType* nonConstOriginal = const_cast<CSBGenericDataType*>(&aGeneric);
    const TSBDerivedType type = aGeneric.DerivedTypeL();
    //
    switch(type)
        {
    case ESIDDerivedType:
        {
        CSBSecureId* temp = CSBSecureId::NewL( nonConstOriginal );
        CleanupStack::PushL( temp );
        ret = temp->SecureIdL();
        CleanupStack::PopAndDestroy( temp );
        break;
        }
    case ESIDTransferDerivedType:
        {
        // First make a generic transfer type object...
        CSBGenericTransferType* transferTypeTemp = CSBGenericTransferType::NewL( nonConstOriginal->Externalise() );
        CleanupStack::PushL(transferTypeTemp);

        // Now make instance transfer type
        CSBSIDTransferType* temp = CSBSIDTransferType::NewL( transferTypeTemp );
        CleanupStack::PushL(temp);

        // Now get the result
        ret = temp->SecureIdL();

        // Tidy up
        CleanupStack::PopAndDestroy(2, transferTypeTemp);
        break;
        }
    case EPackageDerivedType:
        {
        CSBPackageId* temp = CSBPackageId::NewL( nonConstOriginal );
        CleanupStack::PushL( temp );
        ret = temp->SecureIdL();
        CleanupStack::PopAndDestroy( temp );
        break;
        }
    case EPackageTransferDerivedType:
    case EJavaDerivedType:
    case EJavaTransferDerivedType:
    default:
        __BREAKPOINT();
        User::Leave(KErrNotSupported);
        break;
       }
    //
    return ret;
    }


// ---------------------------------------------------------------------------
// MMCScBkupSBEUtils::PackageIdFromGenericL()
// 
// 
// ---------------------------------------------------------------------------
TUid MMCScBkupSBEUtils::PackageIdFromGenericL(const CSBGenericDataType& aGeneric)
    {
    TUid ret = KNullUid;
    CSBGenericDataType* nonConstOriginal = const_cast<CSBGenericDataType*>(&aGeneric);
    const TSBDerivedType type = aGeneric.DerivedTypeL();
    //
    switch(type)
        {
    case EPackageDerivedType:
        {
        CSBPackageId* temp = CSBPackageId::NewL( nonConstOriginal );
        CleanupStack::PushL(temp);
        ret = temp->PackageIdL();
        CleanupStack::PopAndDestroy(temp);
        break;
        }
    case EPackageTransferDerivedType:
        {
        // First make a generic transfer type object...
        CSBGenericTransferType* transferTypeTemp = CSBGenericTransferType::NewL( nonConstOriginal->Externalise() );
        CleanupStack::PushL(transferTypeTemp);

        // Now make instance transfer type
        CSBPackageTransferType* temp = CSBPackageTransferType::NewL( transferTypeTemp );
        CleanupStack::PushL(temp);

        // Now get the result
        ret = temp->PackageIdL();

        // Tidy up
        CleanupStack::PopAndDestroy(2, transferTypeTemp);
        break;
        }
    case ESIDDerivedType:
    case ESIDTransferDerivedType:
    case EJavaDerivedType:
    case EJavaTransferDerivedType:
    default:
        __BREAKPOINT();
        User::Leave(KErrNotSupported);
        break;
       }
    //
    return ret;
    }


// ---------------------------------------------------------------------------
// MMCScBkupSBEUtils::JavaHashFromGenericLC()
// 
// 
// ---------------------------------------------------------------------------
HBufC* MMCScBkupSBEUtils::JavaHashFromGenericLC(const CSBGenericDataType& aGeneric)
    {
    HBufC* ret = NULL;
    CSBGenericDataType* nonConstOriginal = const_cast<CSBGenericDataType*>(&aGeneric);
    const TSBDerivedType type = aGeneric.DerivedTypeL();
    //
    switch(type)
        {
    case EJavaDerivedType:
        {
        CSBJavaId* temp = CSBJavaId::NewL( nonConstOriginal );
        CleanupStack::PushL(temp);
        ret = temp->SuiteHashL().AllocL();
        CleanupStack::PopAndDestroy(temp);
        CleanupStack::PushL( ret );
        break;
        }
    case EJavaTransferDerivedType:
        {
        // First make a generic transfer type object...
        CSBGenericTransferType* transferTypeTemp = CSBGenericTransferType::NewL( nonConstOriginal->Externalise() );
        CleanupStack::PushL(transferTypeTemp);

        // Now make a instance transfer type
        CSBJavaTransferType* temp = CSBJavaTransferType::NewL( transferTypeTemp );
        CleanupStack::PushL(temp);

        // Now get the result
        ret = temp->SuiteHashL().AllocL();

        // Tidy up
        CleanupStack::PopAndDestroy(2, transferTypeTemp);
        CleanupStack::PushL( ret );
        break;
        }
    case ESIDDerivedType:
    case ESIDTransferDerivedType:
    case EPackageDerivedType:
    case EPackageTransferDerivedType:
    default:
        __BREAKPOINT();
        User::Leave(KErrNotSupported);
        break;
       }
    //
    if  ( ret == NULL )
        {
        ret = KNullDesC().AllocLC();
        }
    //
    return ret;
    }


// ---------------------------------------------------------------------------
// MMCScBkupSBEUtils::CopyLC()
// 
// 
// ---------------------------------------------------------------------------
CSBGenericDataType* MMCScBkupSBEUtils::CopyLC(const CSBGenericDataType& aToBeCopied)
    {
    CSBGenericDataType* ret = NULL;

    // Have to do this through poor implementation of SBE API 
    CSBGenericDataType* nonConstOriginal = const_cast<CSBGenericDataType*>(&aToBeCopied);
    //
    const TSBDerivedType type = nonConstOriginal->DerivedTypeL();
    switch(type)
        {
    case ESIDDerivedType:
        ret = CSBSecureId::NewL(nonConstOriginal);
        break;
    case EPackageDerivedType:
        ret = CSBPackageId::NewL(nonConstOriginal);
        break;
    case EJavaDerivedType:
        ret = CSBJavaId::NewL(nonConstOriginal);
        break;
    case ESIDTransferDerivedType:
    case EPackageTransferDerivedType:
    case EJavaTransferDerivedType:
        {
        // Must first create a generic transfer type object... sigh...
        CSBGenericTransferType* transferType = CSBGenericTransferType::NewL( nonConstOriginal->Externalise() );
        CleanupStack::PushL( transferType ); 
        if ( ESIDTransferDerivedType == type )
            {
            ret = CSBSIDTransferType::NewL( transferType );
            }
        else if ( EPackageTransferDerivedType == type )
            {
            ret = CSBPackageTransferType::NewL( transferType );
            }
        // Type is EJavaTransferDerivedType
        else
            {
            ret = CSBJavaTransferType::NewL( transferType );
            }
        CleanupStack::PopAndDestroy( transferType );
        }
    default:
        break;
       }
    //
    if  (!ret)
        {
        __BREAKPOINT();
        User::Leave(KErrNotSupported);
        }
    CleanupStack::PushL(ret);
    return ret;
    }


// ---------------------------------------------------------------------------
// MMCScBkupSBEUtils::TransferTypeLC()
// 
// 
// ---------------------------------------------------------------------------
CSBGenericTransferType* MMCScBkupSBEUtils::TransferTypeLC( const CSBGenericDataType& aDT, TDriveNumber aDrive, 
    TTransferDataType aTransferType, TInt aVersion )
    {
    CSBGenericTransferType* ret = NULL;

    // Have to do this through poor implementation of SBE API 
    CSBGenericDataType* nonConstOriginal = const_cast< CSBGenericDataType* >( &aDT );
    //
    const TSBDerivedType type = nonConstOriginal->DerivedTypeL();
    switch( type )
        {
    case ESIDDerivedType:
    case ESIDTransferDerivedType:
        {
        const TSecureId secureId = SecureIdFromGenericL( aDT );
        ret = CSBSIDTransferType::NewL( secureId, aDrive, aTransferType );
        break;
        }

    case EPackageDerivedType:
        {
        TSecureId secureId = SecureIdFromGenericL( aDT );
        if( secureId.iId == KNullUid.iUid && aVersion > CMMCScBkupDataOwnerInfo::EStreamFormatVersionFirst )
            {
            secureId = MMCScBkupSBEUtils::PackageIdFromGenericL( aDT );
            }
        ret = CSBSIDTransferType::NewL( secureId, aDrive, aTransferType );
        break;
        }

    default:
    case EPackageTransferDerivedType:
    case EJavaDerivedType:
    case EJavaTransferDerivedType:
        __BREAKPOINT();
        User::Leave( KErrNotSupported );
        break;
        }
    //
    CleanupStack::PushL( ret );
    return ret;
    }


// ---------------------------------------------------------------------------
// MMCScBkupSBEUtils::TransferTypeLC()
// 
// 
// ---------------------------------------------------------------------------
CSBGenericTransferType* MMCScBkupSBEUtils::TransferTypeLC(const CSBGenericDataType& aDT, TDriveNumber aDrive, 
    TPackageDataType aDataType, TInt /*aVersion*/)
    {
    CSBGenericTransferType* ret = NULL;

    // Have to do this through poor implementation of SBE API 
    CSBGenericDataType* nonConstOriginal = const_cast<CSBGenericDataType*>(&aDT);
    //
    const TSBDerivedType type = nonConstOriginal->DerivedTypeL();
    switch(type)
        {
    case EPackageDerivedType:
    case EPackageTransferDerivedType:
        {
        const TUid packageId = PackageIdFromGenericL(aDT);
        ret = CSBPackageTransferType::NewL(packageId, aDrive, aDataType);
        break;
        }

    default:
    case ESIDDerivedType:
    case ESIDTransferDerivedType:
    case EJavaDerivedType:
    case EJavaTransferDerivedType:
        __BREAKPOINT();
        User::Leave(KErrNotSupported);
        break;
        }
    //
    CleanupStack::PushL(ret);
    return ret;
    }


// ---------------------------------------------------------------------------
// MMCScBkupSBEUtils::TransferTypeLC()
// 
// 
// ---------------------------------------------------------------------------
CSBGenericTransferType* MMCScBkupSBEUtils::TransferTypeLC(const CSBGenericDataType& aDT, TDriveNumber aDrive, 
    TJavaTransferType aDataType, TInt /*aVersion*/)
    {
    CSBGenericTransferType* ret = NULL;

    // Have to do this through poor implementation of SBE API 
    CSBGenericDataType* nonConstOriginal = const_cast<CSBGenericDataType*>(&aDT);
    //
    const TSBDerivedType type = nonConstOriginal->DerivedTypeL();
    switch(type)
        {
    case EJavaDerivedType:
    case EJavaTransferDerivedType:
        {
        HBufC* hash = JavaHashFromGenericLC( aDT );
        ret = CSBJavaTransferType::NewL( *hash, aDrive, aDataType );
        CleanupStack::PopAndDestroy( hash );
        break;
        }

    default:
    case ESIDDerivedType:
    case ESIDTransferDerivedType:
    case EPackageDerivedType:
    case EPackageTransferDerivedType:
        __BREAKPOINT();
        User::Leave(KErrNotSupported);
        break;
        }
    //
    CleanupStack::PushL(ret);
    return ret;
    }


// ---------------------------------------------------------------------------
// MMCScBkupSBEUtils::PhoneIsInBackupOrRestoreModeL()
// 
// 
// ---------------------------------------------------------------------------
TBool MMCScBkupSBEUtils::PhoneIsInBackupOrRestoreModeL()
    {
    TBool backupInProgress = EFalse;
    //
    TInt value = 0;
    const TInt error = RProperty::Get( KUidSystemCategory, KUidBackupRestoreKey, value );
    User::LeaveIfError( error );
    //
    const TBURPartType partType = static_cast< TBURPartType >( value & KBURPartTypeMask );
    if  (value != 0)
        {
        switch(partType)
            {
        case EBURUnset:
        case EBURNormal:
            break;
        case EBURBackupFull:
        case EBURBackupPartial:
        case EBURRestoreFull:
        case EBURRestorePartial:
            backupInProgress = ETrue;
            break;
            }
        }
    //
    return backupInProgress;
    }


// ---------------------------------------------------------------------------
// MMCScBkupSBEUtils::EndBackupOrRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void MMCScBkupSBEUtils::EndBackupOrRestoreL( CSBEClient& aSBEClient )
    {
    TDriveList nullDriveList;
    aSBEClient.SetBURModeL( nullDriveList, EBURNormal, ENoBackup);
    }


// ---------------------------------------------------------------------------
// MMCScBkupSBEUtils::HasSystemDataL()
// 
// 
// ---------------------------------------------------------------------------
TBool MMCScBkupSBEUtils::HasSystemDataL( const CDataOwnerInfo& aDataOwner )
    {
    const TSBDerivedType type = aDataOwner.Identifier().DerivedTypeL();
    const TBool systemDataType = ( type == EPackageDerivedType || 
                                   type == EPackageTransferDerivedType );
    const TBool hasSystemData = ( aDataOwner.CommonSettings() & EHasSystemFiles );
    //
    return ( systemDataType && hasSystemData );
    }


// ---------------------------------------------------------------------------
// MMCScBkupSBEUtils::HasJavaDataL()
// 
// 
// ---------------------------------------------------------------------------
TBool MMCScBkupSBEUtils::HasJavaDataL( const CDataOwnerInfo& aDataOwner )
    {
    const TSBDerivedType type = aDataOwner.Identifier().DerivedTypeL();
    const TBool javaDataType = ( type == EJavaDerivedType || 
                                 type == EJavaTransferDerivedType );
    const TBool hasSystemData = ( aDataOwner.CommonSettings() & EHasSystemFiles );

    // Java abuses the "has system files flag" - this has been confirmed
    // with Symbian.
    return ( javaDataType && hasSystemData );
    }


// ---------------------------------------------------------------------------
// MMCScBkupSBEUtils::HasPassiveDataL()
// 
// 
// ---------------------------------------------------------------------------
TBool MMCScBkupSBEUtils::HasPassiveDataL( const CDataOwnerInfo& aDataOwner )
    {
    const TSBDerivedType type = aDataOwner.Identifier().DerivedTypeL();
    const TBool passiveDataType = ( type == ESIDDerivedType || 
                                    type == ESIDTransferDerivedType ||
                                    type == EPackageDerivedType || 
                                    type == EPackageTransferDerivedType );
    const TBool hasPassiveData = ( aDataOwner.CommonSettings() & EPassiveBUR );
    //
    return ( passiveDataType && hasPassiveData );
    }


// ---------------------------------------------------------------------------
// MMCScBkupSBEUtils::HasActiveDataL()
// 
// 
// ---------------------------------------------------------------------------
TBool MMCScBkupSBEUtils::HasActiveDataL( const CDataOwnerInfo& aDataOwner )
    {
    const TSBDerivedType type = aDataOwner.Identifier().DerivedTypeL();
    const TBool activeDataType = ( type == ESIDDerivedType || 
                                   type == ESIDTransferDerivedType ||
                                   type == EPackageDerivedType || 
                                   type == EPackageTransferDerivedType );
    const TBool hasActiveData = ( aDataOwner.CommonSettings() & EActiveBUR );
    //
    return ( activeDataType && hasActiveData );
    }


// ---------------------------------------------------------------------------
// MMCScBkupSBEUtils::HasPublicDataL()
// 
// 
// ---------------------------------------------------------------------------
TBool MMCScBkupSBEUtils::HasPublicDataL( const CDataOwnerInfo& aDataOwner )
    {
    const TSBDerivedType type = aDataOwner.Identifier().DerivedTypeL();
    const TBool publicDataType = ( type == ESIDDerivedType || 
                                   type == ESIDTransferDerivedType ||
                                   type == EPackageDerivedType || 
                                   type == EPackageTransferDerivedType );
    const TBool hasPublicData = ( aDataOwner.PassiveSettings() & EHasPublicFiles );
    //
    return ( hasPublicData && publicDataType );
    }

