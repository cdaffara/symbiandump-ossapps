/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Methods for CNSmlDMSyncProfile
*
*/


#include <SyncMLTransportProperties.h>
#include <centralrepository.h>
#include <featmgr.h>
#include "nsmldmsyncprivatecrkeys.h"
#include "nsmldmsyncprofilelist.h"
#include "nsmldmsyncprofile.h"
#include "nsmldmsyncdebug.h"
#include "nsmldmsyncappengine.h"
#include "nsmldmsyncutil.h"

// ============================ MEMBER FUNCTIONS ==============================
//

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfileList::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlDMSyncProfileList* CNSmlDMSyncProfileList::NewL( 
                                                CNSmlDMSyncAppEngine* aEngine )
    {
    FLOG( "[OMADM] CNSmlDMSyncProfileList::NewL:");
    
    CNSmlDMSyncProfileList* self = 
                                new(ELeave) CNSmlDMSyncProfileList( aEngine );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlDMSyncProfileList::~CNSmlDMSyncProfileList()
    {
    FLOG( "[OMADM] CNSmlDMSyncProfileList::~CNSmlDMSyncProfileList():" );
    
	iList.Close();
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfileList::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfileList::ConstructL(void)
    {
    FLOG( "[OMADM] CNSmlDMSyncProfileList::ConstructL:" );
    
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfileList::CNSmlDMSyncProfileList.
// -----------------------------------------------------------------------------
//
CNSmlDMSyncProfileList::CNSmlDMSyncProfileList( CNSmlDMSyncAppEngine* aEngine )
	{
	FLOG( "[OMADM] CNSmlDMSyncProfileList::CNSmlDMSyncProfileList:" );
	
	iEngine = aEngine;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfileList::Count
// -----------------------------------------------------------------------------
//
TInt CNSmlDMSyncProfileList::Count()
	{
	return iList.Count();
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfileList::Item
// -----------------------------------------------------------------------------
//
TNSmlDMSyncProfileItem CNSmlDMSyncProfileList::Item(TInt aIndex)
	{
	if ( ( aIndex < 0 ) || ( aIndex >= Count() ) )
	    {
		TUtil::Panic(KErrGeneral);
	    }

	return iList[aIndex];
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfileList::FindProfile
// -----------------------------------------------------------------------------
//
TInt CNSmlDMSyncProfileList::FindProfile(const TDesC& aProfileName)
	{
	FLOG( "[OMADM] CNSmlDMSyncProfileList::FindProfile:" );
	
	for ( TInt index = 0; index < Count(); index++ )
		{
		TPtrC ptr = Item(index).Name();
		if ( ptr.Compare( aProfileName ) == 0 )
			{
			return index;
			}
		}
	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// ReplaceProfileItemL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfileList::ReplaceProfileItemL( 
                                        TNSmlDMSyncProfileItem& aProfileItem )
	{
	// remove old profile with same id
	Remove( aProfileItem.iProfileId );
	// add new profile 
    User::LeaveIfError( iList.Append(aProfileItem) );
    Sort();
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncProfileList::ReadProfileItemL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfileList::ReadProfileItemL( TInt aIndex )
	{
	FLOG("[OMADM] CNSmlDMSyncProfileList::ReadProfileItemL:" );
	
    CNSmlDMSyncProfile* profile = iEngine->OpenProfileL( aIndex, ESmlOpenRead );
	
	TNSmlDMSyncProfileItem item = ReadProfileItemL( profile );
	item.iMandatoryCheck = CheckMandatoryDataL( profile );
	ReplaceProfileItemL( item );
	iEngine->CloseProfile();
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfileList::ReadProfileItemsL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfileList::ReadProfileItemsL()
	{
	ReadProfileItemsL( EFalse );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfileList::ReadProfileItemsL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfileList::ReadProfileItemsL( TBool aIncludeHidden )
	{
	FLOG( "[OMADM] CNSmlDMSyncProfileList::ReadProfileItemsL:" );
	
	RArray<TSmlProfileId> arr;
    iEngine->Session()->ListProfilesL( arr, ESmlDevMan );
   	CleanupClosePushL(arr);
    TNSmlDMSyncProfileItem item;

    TBool fotaSupported = FeatureManager::FeatureSupported( KFeatureIdSyncMlDmFota );
    TBuf<KBufSize256> fotaProfileId;

    if ( fotaSupported && !aIncludeHidden )
    	{
        CRepository* centrep = CRepository::NewL( KCRUidNSmlDMSyncApp );
        if ( centrep->Get( KNSmlDMFotaHiddenProfileIdKey, fotaProfileId ) != KErrNone )
            {
            fotaProfileId.Zero();
            }
        FTRACE( FPrint(
            _L("[OMADM] CNSmlDMSyncProfileList::ReadProfileItemsL() hidden FOTA profile id = \"%S\""),
            &fotaProfileId ) );
        delete centrep;
    	}
    
	for ( TInt index = 0; index < arr.Count(); index++ )
		{
	    TRAPD( error, iEngine->OpenProfileL( arr[index], ESmlOpenRead ) );
	    if ( error == KErrNone )
	        {
	        TRAPD( retVal, item = ReadProfileItemL( iEngine->Profile() ) );

            TBuf<KBufSize256> profileId;
            iEngine->Profile()->GetServerId( profileId );
            if ( (retVal == KErrNone)
                && ( fotaProfileId.Compare(profileId) != 0 ) )
                {
                iList.Append( item );
                }

            iEngine->CloseProfile();
	        }
		}
    CleanupStack::PopAndDestroy( &arr );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfileList::ReadProfileItemL
// -----------------------------------------------------------------------------
//
TNSmlDMSyncProfileItem CNSmlDMSyncProfileList::ReadProfileItemL( 
                                                CNSmlDMSyncProfile* aProfile )
	{
	FLOG( "[OMADM] CNSmlDMSyncProfileList::ReadProfileItemL:" );
	
	TNSmlDMSyncProfileItem item;
	item.iApplicationId = aProfile->CreatorId();
	TBuf<KBufSize> buf;
	aProfile->GetName( buf );
	item.SetName( buf );
	item.SetId( aProfile->ProfileId() );
	item.iSynced    = aProfile->IsSynced();
	item.iLastSync  = aProfile->LastSync();
	item.iLastSuccessSync = aProfile->LastSuccessSync();
	item.iBearer    = aProfile->BearerType();
    item.iActive    = EFalse;
    if ( aProfile->SASyncState() != ESASyncStateDisable )
    	{
    	item.iActive = ETrue;
    	}
	item.iDeleteAllowed = aProfile->DeleteAllowed();
	item.iProfileLocked = aProfile->ProfileLocked();
	
	// check whether all mandatory data exist
	item.iMandatoryCheck = CheckMandatoryDataL( aProfile );
	return item;	
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfileList::Reset
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfileList::Reset()
	{
	FLOG( "[OMADM] CNSmlDMSyncProfileList::Reset:" );
	
	iList.Reset();
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfileList::Remove
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfileList::Remove( TInt aProfileId )
	{
	FLOG( "[OMADM] CNSmlDMSyncProfileList::Remove:" );
	
	for ( TInt index = 0; index < Count(); index++ )
		{
		TNSmlDMSyncProfileItem item = iList[index];
		if ( item.iProfileId == aProfileId )
			{
			iList.Remove( index );
			return;
			}
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfileList::CheckMandatoryDataL
// -----------------------------------------------------------------------------
//
TInt CNSmlDMSyncProfileList::CheckMandatoryDataL( CNSmlDMSyncProfile* aProfile )
	{
	FLOG( "[OMADM] CNSmlDMSyncProfileList::CheckMandatoryDataL:" );

	TBuf<KBufSize> buf;
	TInt num;

	aProfile->GetName( buf );
	if ( TUtil::IsEmpty( buf ) )
		{
		return EMandatoryNoProfileName;
		}

	aProfile->GetHostAddress( buf, num );
	if ( TUtil::IsEmpty( buf ) )
		{
		return EMandatoryNoHostAddress;
		}

    return EMandatoryOk;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfileList::IsEmpty
// -----------------------------------------------------------------------------
//
TBool CNSmlDMSyncProfileList::IsEmpty( const TDesC& aText )
	{
	FLOG( "[OMADM] CNSmlDMSyncProfileList::IsEmpty:" );
	
	for ( TInt index = 0; index < aText.Length(); index++ )
		{
		TChar character = aText[index];
		if ( !character.IsSpace() )
			{
			return EFalse;
			}
		}
	return ETrue;
	}
	
// -----------------------------------------------------------------------------
// Sort
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfileList::Sort()
	{
	iList.Sort( TLinearOrder<TNSmlDMSyncProfileItem>(
	                                    TNSmlDMSyncProfileItem::CompareItem) );
	}

// End of File
