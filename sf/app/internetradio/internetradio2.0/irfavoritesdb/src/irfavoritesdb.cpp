/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#include "pspresetnotifier.h"
#include "pstransaction.h"
#include "irdebug.h"
#include "irfavoritesdb.h"
#include "irisdspreset.h"
#include "irpreset.h"
#include "irsettings.h"
#include "iruid3.hrh"

const TInt KNullId = 0;
const TInt KNineteen = 19;

const TUid KIRPreset = { UID3_IRPRESETPLUGIN_DLL_IMPLEMENTATION_UID };

namespace PresetHandler
    {
//-------------------------------------------------------------------------------
//TInt OrderByIndex( const CIRPresetImpl& aPreset1, const CIRPresetImpl& aPreset2 )
//-------------------------------------------------------------------------------
//
    //Sort algorithm
    // Orders presets by their index.
    //
    TInt OrderByIndex( const CIRPreset& aPreset1, const CIRPreset& aPreset2 )
        {
        if ( aPreset1.Index() < aPreset2.Index() )
            {
            return -1;
            }
        else if ( aPreset1.Index() > aPreset2.Index() )
            {
            return 1;
            }
        else
            {
            return 0;
            }
        }
    
    TInt OrderByPlayedTimes( const CIRPreset& aPreset1, 
                             const CIRPreset& aPreset2 )
        {
        int difference = aPreset1.GetPlayedTimes() - aPreset2.GetPlayedTimes();
        if ( difference > 0 )
            {
            return -1;
            }
        else if ( difference == 0 )
            {
            return aPreset1.Name().Compare( aPreset2.Name() );
            }
        else 
            {
            return 1;
            }
        }
    }

//---------------------------------------------------------------------------
// CIRFavoritesDb::NewL()
// standard symbian 1st phase constructor
//---------------------------------------------------------------------------
//
EXPORT_C CIRFavoritesDb* CIRFavoritesDb::NewL()
    {
    IRLOG_DEBUG( "CIRFavoritesDb::NewL" );
    CIRFavoritesDb* self = new ( ELeave ) CIRFavoritesDb;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRFavoritesDb::NewL - Exiting." );
    return self;
    }


//---------------------------------------------------------------------------
// CIRFavoritesDb::~CIRFavoritesDb()
// standard c++ destructor
//---------------------------------------------------------------------------
//
CIRFavoritesDb::~CIRFavoritesDb()
    {
    IRLOG_DEBUG( "CIRFavoritesDb::~CIRFavoritesDb" );
    delete iNotifier;
    iObservers.Close();
    //resets the preset array
    iFavPresetList.ResetAndDestroy();
    iFavPresetList.Close();
    iServ.Close();

	IRLOG_DEBUG( "CIRFavoritesDb::~CIRFavoritesDb - Exiting." );
    }


//---------------------------------------------------------------------------
// CIRFavoritesDb::AddPresetL()
// exported function to add a preset
//---------------------------------------------------------------------------
//
EXPORT_C void CIRFavoritesDb::AddPresetL( CIRIsdsPreset& aPreset,
        TInt& aRetVal, TBool /*aIsLogoAvialable*/ )
    {
    IRLOG_DEBUG( "CIRFavoritesDb::AddPresetL" );
    iMoveStatus = EFalse;
    SortByIndex() ;//sorted by index
    TBool presetSaved = EFalse;

    // Check the existence of preset before checking if the list is full 
    // to avoid KErrNoMemory for a preset that exists in the list.
    TInt searchResult = 0;
    if ( aPreset.GetChannelType() == 0 )
        {
        searchResult = SearchUserDefinedPreset( aPreset.GetName(), 
                                                aPreset.GetChannelUrlAtL(0) );
        //we don't allow user to add presets with the same name
        if ( searchResult >= 0 )
            {
            ReplaceUserDefinedPresetL( aPreset );
            aRetVal = KErrAlreadyExists;
            return;
            }
        }
    else
        {
        searchResult = SearchPreset( aPreset.GetId(), KNullId );
        if( KErrNotFound !=  searchResult)
        {
            //Eventhough the preset exists in the favorites db, replace the
            //same with the new preset i.e. aPreset.  Because the db should 
            //contain the updated values of the preset.
            ReplacePresetL(aPreset);
            
            aRetVal = KErrAlreadyExists;

            return;
        }
    }

    if ( EmptyPresetCount() <= 0 )
        {
         //max limit reached no more processing
        aRetVal = KErrNoMemory;
        return;
        }
    
    // Found the next empty place or replace
    for ( TInt j = 0; j < MaxPresetCount() && !presetSaved; j++ )
        {
        CIRPreset* preset = PresetByIndex( j );
        TBool presetExisted = preset ? ETrue : EFalse;//presetExisted=1 if preset exists

        if ( !preset )
            {
            preset = CreatePresetL( j );//create if preset==NULL
            }

        if ( !presetExisted )//crete if preset did not exists
            {
            //open a transaction with the preset server
            CPSTransaction* transaction = preset->CreateTransactionLC();
            //Convert to Plugnized Preset
            //That is accepted by the preset server
            preset->CIRIsdsPresetToCIRPresetImplL( aPreset );
            //setname of CPSPresetInterface is called
            preset->SetNameL( aPreset.GetName() );
            transaction->CommitL();
            CleanupStack::PopAndDestroy( transaction );
            //added to check multiple insertion
            presetSaved = ETrue;
            }
        }

    aRetVal = KErrNone;
    IRLOG_DEBUG( "CIRFavoritesDb::AddPresetL - Exiting." );
    }

//---------------------------------------------------------------------------
// CIRFavoritesDb::AddPresetL()
// exported,overloaded function to add a preset manually
// converts the minimum data(name ,url) into an preset
//---------------------------------------------------------------------------
//
EXPORT_C void CIRFavoritesDb::AddPresetL( const TDesC& aName,
    const TDesC& aURl,TInt& aReturn, TBool /*aIsLogoAvialable*/ )
    {
    IRLOG_DEBUG( "CIRFavoritesDb::AddPresetL(des des int int)" );
    iMoveStatus = EFalse;
    CIRIsdsPreset* saveItem;
    saveItem = CIRIsdsPreset::NewLC();
    saveItem->SetNameL( aName );
    //only one url is saved for manually added channel
    saveItem->SetUrlCount( 1 );
    saveItem->SetChannelType( EUserDefined );
    _LIT( KNotAvailable, "NA" );
    //bitrate set to zero so that it is accessible to all 
    //kinds of bitrate settings
    saveItem->SetUrlL( KNotAvailable, aURl, 0 );
    AddPresetL( *saveItem, aReturn );
    CleanupStack::PopAndDestroy( saveItem );
    IRLOG_DEBUG( "CIRFavoritesDb::AddPresetL(des des int int) - Exiting." );
    }


//---------------------------------------------------------------------------
// CIRFavoritesDb::DeletePresetL()
// exported, function to delete a preset
//---------------------------------------------------------------------------
//

EXPORT_C void CIRFavoritesDb::DeletePresetL( TInt aId )
    {
    IRLOG_DEBUG( "CIRFavoritesDb::DeletePresetL" );
    iMoveStatus = EFalse;
    iServ.DeletePresetL( aId );
    TInt count = iFavPresetList.Count();
    //when we delete one preset in the server's db, we also
    //need to delete it in the favorite list. 
    for( TInt i=0; i<count; i++ )
    {      
        if( aId == iFavPresetList[i]->Id())
        {
            //before Remove, the memory should be freed ahead. 
            delete iFavPresetList[i];
            iFavPresetList.Remove(i);          
            break;
        }
    }
    IRLOG_DEBUG( "CIRFavoritesDb::DeletePresetL - Exiting." );
    }

//---------------------------------------------------------------------------
// CIRFavoritesDb::GetAllPresetL()
// exported,function to get the list of saved presets in 
// the array iFavPresetList
//---------------------------------------------------------------------------
//
EXPORT_C void CIRFavoritesDb::GetAllPreset()
    {
    IRLOG_DEBUG( "CIRFavoritesDb::GetAllPresetL" );
    //sort the array according to the index values
    SortByIndex();
    IRLOG_DEBUG( "CIRFavoritesDb::GetAllPresetL - Exiting." );
    }

//---------------------------------------------------------------------------
// CIRFavoritesDb::GetAllSortedPresets()
// get all presets sorted by played times
//---------------------------------------------------------------------------
//
EXPORT_C const RIRPresetArray& CIRFavoritesDb::GetAllSortedPresets()
    {
    iFavPresetList.Sort( 
            TLinearOrder<CIRPreset>( PresetHandler::OrderByPlayedTimes ) );
    return iFavPresetList;
    }
    
//---------------------------------------------------------------------------
// CIRFavoritesDb::SwapPresetsInDbL()
// returns the previous preset for a given presetId
//---------------------------------------------------------------------------
//
EXPORT_C void CIRFavoritesDb::SwapPresetsInDbL( TInt aChannelIdFrom,
    TInt /*aChannelIdTo*/, TInt /*aChannelIndexFro*/, TInt aChannelIndexTo )
    {
    IRLOG_DEBUG( "CIRFavoritesDb::SwapPresetsInDbL" );
    MovePresetL( aChannelIdFrom, aChannelIndexTo );
    IRLOG_DEBUG( "CIRFavoritesDb::SwapPresetsInDbL - Exiting." );
    }

//---------------------------------------------------------------------------
// CIRFavoritesDb::MovePresetL()
// moves a preset to destination index
//---------------------------------------------------------------------------
//
 void CIRFavoritesDb::MovePresetL( const TInt aId, 
                                   const TInt aDestinationIndex )
    {
    IRLOG_DEBUG( "CIRFavoritesDb::MovePresetL" );
    ASSERT( aDestinationIndex >= 0 && 
            aDestinationIndex < iFavPresetList.Count() );
    if( aDestinationIndex < 0 || aDestinationIndex >= iFavPresetList.Count() )
        {
        User::Leave( KErrArgument );
        }
    
    TInt fakeIndex = iFavPresetList[aDestinationIndex]->Index();
    SetMoveStatus( ETrue );
    iServ.MovePresetL( aId, fakeIndex );
    IRLOG_DEBUG( "CIRFavoritesDb::MovePresetL - Exiting." );
    }
    
//---------------------------------------------------------------------------
// CIRFavoritesDb::SetMoveStatus()
// sets the status of Move functionality
//---------------------------------------------------------------------------
//
EXPORT_C void CIRFavoritesDb::SetMoveStatus( TBool aStatus )
    {
    iMoveStatus = aStatus;
    }

//---------------------------------------------------------------------------
// CIRFavoritesDb::GetMoveStatus()
// returns the status of the Move functionality
//---------------------------------------------------------------------------
//
EXPORT_C TBool CIRFavoritesDb::GetMoveStatus()
    {
    return iMoveStatus;
    }
    
    
//---------------------------------------------------------------------------
// CIRFavoritesDb::SearchPreset()
// exported, function to find out if a preset exists in the favorites list
//---------------------------------------------------------------------------
//
EXPORT_C TInt CIRFavoritesDb::SearchPreset( const TInt aIsdsPresetId,
                              const TInt aUniqPresetId )
    {
    IRLOG_DEBUG( "CIRFavoritesDb::SearchPreset" );
    TInt count = iFavPresetList.Count();
    TInt iter;
    //for isds presets
    if( aIsdsPresetId != KNullId )
        {
        for( iter = 0; iter < count; iter++ )
            {
            if( iFavPresetList[iter]->GetId() == aIsdsPresetId )
                {
                IRLOG_DEBUG( "CIRFavoritesDb::SearchPreset - Exiting (1)." );
                return iter;
                }
            }
        }
    if( aUniqPresetId != KNullId )
    //for manually added presets
        {
        for( iter = 0; iter < count; iter++)
            {
            if( aUniqPresetId == iFavPresetList[iter]->Id() )
                {
                IRLOG_DEBUG( "CIRFavoritesDb::SearchPreset - Exiting (2)." );
                return iter;
                }
            }
        }
    // both the parameters are null implies 
    //that the preset is a non saved station
    IRLOG_DEBUG( "CIRFavoritesDb::SearchPreset - Exiting (3)." );
    return KErrNotFound;
    }

//---------------------------------------------------------------------------
// CIRFavoritesDb::GetPreviousPreset()
// exported, function to find out if a preset exists in the favorites list
//---------------------------------------------------------------------------
//
EXPORT_C TInt CIRFavoritesDb::GetNextPreset( TInt aIndex )
    {
    if ( aIndex == ( iFavPresetList.Count() - 1 )|| aIndex == KNineteen )
        {
        return 0;
        }
    else
        {
        return aIndex+1;
        }
    }

//---------------------------------------------------------------------------
// CIRFavoritesDb::GetNextPreset()
// exported, function to find out if a preset exists in the favorites list
//---------------------------------------------------------------------------
//
EXPORT_C TInt CIRFavoritesDb::GetPreviousPreset( TInt aIndex )
    {
    if ( aIndex==0 )
        {
        TInt val = ( iFavPresetList.Count() - 1 );
        return val;
        }
    else
        {
        return aIndex-1;
        }
    }

//---------------------------------------------------------------------------
// CIRFavoritesDb::ReplacePresetL()
// replaces a preset with a new preset
// for presetSync
//---------------------------------------------------------------------------
//
EXPORT_C void CIRFavoritesDb::ReplacePresetL( CIRIsdsPreset& aNewPreset )
    {
    IRLOG_DEBUG( "CIRFavoritesDb::ReplacePresetL" );
    iMoveStatus = EFalse;
    CIRPreset* preset;
    TInt index = SearchPreset( aNewPreset.GetId(), KNullId );
    //actual index
    if ( index >= 0 )
        {
        ASSERT( index >= 0 || index < iFavPresetList.Count() );
        index = iFavPresetList[index]->Index();
        preset = PresetByIndex( index );
        if( preset )
            {
            CPSTransaction* transaction = preset->CreateTransactionLC();
            //change the preset data and commit
            //update 'name' only if this station has not been renamed before
            if ( preset->GetRenamed() )
                {
                aNewPreset.SetNameL( preset->Name() );
                }
            preset->CIRIsdsPresetToCIRPresetImplL( aNewPreset );
            preset->SetChannelType( EIsdsPreset );
            transaction->CommitL();
            CleanupStack::PopAndDestroy( transaction );
            }
        }
    IRLOG_DEBUG( "CIRFavoritesDb::ReplacePresetL - Exiting." );
    }

//---------------------------------------------------------------------------
// CIRFavoritesDb::ReplaceUserDefinedPresetL()
// replaces a userdefined preset with a new userdefiend preset
// for presetSync
//---------------------------------------------------------------------------
//
EXPORT_C void CIRFavoritesDb::ReplaceUserDefinedPresetL( 
                                   CIRIsdsPreset& aNewPreset )
    {
    IRLOG_DEBUG( "CIRFavoritesDb::ReplaceUserDefinedPresetL" );
    iMoveStatus = EFalse;
    TInt index = SearchUserDefinedPreset( aNewPreset.GetName(), 
                                    aNewPreset.GetChannelUrlAtL(0) );
    
    if ( index >= 0 && index < iFavPresetList.Count() )
        {
        CIRPreset* preset = iFavPresetList[index];
        if ( preset )
            {
            CPSTransaction* transaction = preset->CreateTransactionLC();
            /**
             * change the preset data and commit
             */
            preset->CIRIsdsPresetToCIRPresetImplL( aNewPreset );
            preset->SetChannelType( EUserDefined );
            preset->SetNameL( aNewPreset.GetName() );
            preset->SetDescriptionL( aNewPreset.GetShortDescription() );
            transaction->CommitL();
            CleanupStack::PopAndDestroy( transaction );
            }
        }
    IRLOG_DEBUG( "CIRFavoritesDb::ReplaceUserDefinedPresetL - Exiting." );    
    }

//---------------------------------------------------------------------------
// CIRFavoritesDb::MakePresetUserDefinedL()
// for a favorite preset that has been removed from the isds.
// it is made a user defined preset by changing the type to 0.
// index value is preseved so that the relative positions in the saved
// stations view remains the same.
//---------------------------------------------------------------------------
//
EXPORT_C void CIRFavoritesDb::MakePresetUserDefinedL( TInt aChannelId,
    TInt /*aUserDefinedChannelId*/ )
    {
    IRLOG_DEBUG( "CIRFavoritesDb::MakePresetUserDefinedL" );
    iMoveStatus = EFalse;
    //to match the function signature
    TInt index = SearchPreset( aChannelId, KNullId );
    CIRPreset* preset;
    //actual index
    if ( index >= 0 )
        {
        ASSERT( index >= 0 || index < iFavPresetList.Count() );
        index = iFavPresetList[index]->Index();
        preset = PresetByIndex(index);
        if( preset )
            {
            CPSTransaction* transaction = preset->CreateTransactionLC();
            //convert the flag to user defined and commit
            preset->SetChannelType( EUserDefined );
            //assign null id
            preset->SetId( KNullId );

            //make logo as null;;;; added for 2.0
            _LIT8( KEmpty, "" );
            RBuf8 tempLogo;
            CleanupClosePushL(tempLogo);
            tempLogo.CreateL( KEmpty );
            preset->SetLogoDataL( tempLogo );
            _LIT( KEmptyString, "" );
            RBuf tempString;
            CleanupClosePushL(tempString);
            tempString.CreateL( KEmptyString );
            //set imgurl to null
            preset->SetImgUrlL( tempString );
            //set genre to null
            preset->SetGenreInfoL( tempString,tempString );
            //set language to null
            preset->SetLangL( tempString );
            //set country to null
            preset->SetCountryNameL( tempString );
            //set musicStoreEnabled filed to "no";;;;added for 2.0
            _LIT( KNo, "no" );
            RBuf tempMusicStoreEnabled;
            CleanupClosePushL(tempMusicStoreEnabled);
            tempMusicStoreEnabled.CreateL( KNo );
            preset->SetMusicStoreStatusL( tempMusicStoreEnabled );
            CleanupStack::PopAndDestroy(3);
            
            transaction->CommitL();
            CleanupStack::PopAndDestroy( transaction );
            }
        }
    IRLOG_DEBUG( "CIRFavoritesDb::MakePresetUserDefinedL - Exiting." );
    }

//---------------------------------------------------------------------------
// const RVRPresetArray& CVRPresetHandler::Presets() const
// RIRPresetArray& an array containing all Internet Radio presets.
//---------------------------------------------------------------------------
//
EXPORT_C const RIRPresetArray& CIRFavoritesDb::Presets() const
    {
    IRLOG_DEBUG( "CIRFavoritesDb::Presets" );
    //sort by 
    return iFavPresetList;
    }

//---------------------------------------------------------------------------
// CIRFavoritesDb::IncreasePlayedTimesL()
// Increase the played times of a channel if it has been in the favorites.
// return : KErrNone if success
//          KErrNotFound if the preset is not in the favorites
//---------------------------------------------------------------------------
//
EXPORT_C TInt CIRFavoritesDb::IncreasePlayedTimesL(
                           const CIRIsdsPreset &aIsdsPreset )
    {
    //step 1 : search the preset in favorites
    SortByIndex();
    TInt index = 0;
    if ( aIsdsPreset.GetChannelType() == 0 )
        {
        index = SearchUserDefinedPreset( aIsdsPreset.GetName(), 
                    aIsdsPreset.GetChannelUrlAtL(0) );
        }
    else
        {
        index = SearchPreset( aIsdsPreset.GetId(), KNullId );
        }
    
    if ( KErrNotFound == index )
        {
        return KErrNotFound;
        }
    
    //step 2 : update the played times of the preset
    CIRPreset *irPreset = iFavPresetList[index];
    //open a transaction with the preset server
    CPSTransaction* transaction = irPreset->CreateTransactionLC();
    irPreset->SetPlayedTimes( irPreset->GetPlayedTimes() + 1 );
    transaction->CommitL();
    CleanupStack::PopAndDestroy( transaction );
    return KErrNone;
    }
    
//---------------------------------------------------------------------------
// CIRFavoritesDb::RenamePresetL()
// renames a preset with a new name
//---------------------------------------------------------------------------
//
EXPORT_C TInt CIRFavoritesDb::RenamePresetL(const CIRIsdsPreset &aIsdsPreset, 
	                                          const TDesC &aNewName)
    {
    TInt index = 0;
    if (aIsdsPreset.GetChannelType() == 0)
        {
        index = SearchUserDefinedPreset( aNewName, aIsdsPreset.GetChannelUrlAtL( 0 ) );
        if ( KErrNotFound != index )
            {
            //there is already a user-defined station with same name and url
            return KErrAlreadyExists;
            }
        else
            {
            index = SearchUserDefinedPreset( aIsdsPreset.GetName(),
                                              aIsdsPreset.GetChannelUrlAtL( 0 ) );
            }
        }
    else
        {
        index = SearchPreset( aIsdsPreset.GetId(), KNullId );
        }
    
    if ( KErrNotFound == index )
        {
        return KErrNotFound;
        }
    
    CIRPreset *irPreset = iFavPresetList[index];
    //open a transaction with the preset server
    CPSTransaction* transaction = irPreset->CreateTransactionLC();
    irPreset->SetNameL( aNewName );
    irPreset->SetRenamed();
    transaction->CommitL();
    CleanupStack::PopAndDestroy( transaction );
    
    return KErrNone;
    }

//---------------------------------------------------------------------------
// CIRFavoritesDb::ConstructL()
// Standard 2nd phase construction
//---------------------------------------------------------------------------
//
void CIRFavoritesDb::ConstructL()
	{
	IRLOG_DEBUG( "CIRFavoritesDb::ConstructL" );
	//cenrep handle
    CIRSettings *settings = CIRSettings::OpenL();
    iMaxPresetCount=settings->MaxPresetCount();
    settings->Close();
    
	User::LeaveIfError( iServ.Connect() );
	//a session to the client of the preset server
    iServ.GetPresetsL( iFavPresetList, KIRPreset );
    //notifier
    iNotifier = CPSPresetNotifier::NewL( iServ, *this );
    
    //initialization of the list for UI use
    //iFavPresetList=new(ELeave)CArrayPtrFlat<CIRPreset>(KGranularity)
    iMoveStatus = EFalse;
    IRLOG_DEBUG( "CIRFavoritesDb::ConstructL - Exiting." );
    }


//---------------------------------------------------------------------------
// CIRFavoritesDb::SortByIndex()
// sorts the preset list by index
//---------------------------------------------------------------------------
//
void CIRFavoritesDb::SortByIndex()
    {
    IRLOG_DEBUG( "CIRFavoritesDb::SortByIndex" );
    //The sort order is determined by an algorithm supplied by the caller
    //and packaged as a TLinerOrder<T>
    iFavPresetList.Sort( TLinearOrder<CIRPreset>( PresetHandler::OrderByIndex ) );
    IRLOG_DEBUG( "CIRFavoritesDb::SortByIndex - Exiting." );
    }

//---------------------------------------------------------------------------
// EXPORT_C TInt CIRFavoritesDb::MaxPresetCount()
// returns the maximum number of presets that can be stored in the favorites list
//---------------------------------------------------------------------------
//
EXPORT_C TInt CIRFavoritesDb::MaxPresetCount()
    {
    IRLOG_DEBUG( "CIRFavoritesDb::MaxPresetCount" );
    return iMaxPresetCount;
    }

//---------------------------------------------------------------------------
// TInt CIRFavoritesDb::EmptyPresetCount() const
// Returns the number of empty presets
//---------------------------------------------------------------------------
//
EXPORT_C TInt CIRFavoritesDb::EmptyPresetCount() const
    {
    IRLOG_DEBUG( "CIRFavoritesDb::EmptyPresetCount" );
    return iMaxPresetCount - iFavPresetList.Count();
    }

//---------------------------------------------------------------------------
// TInt CIRFavoritesDb::AddObserver( MPSPresetObserver& aObserver )
// Adds an observer that is notified upon changes in presets.
//---------------------------------------------------------------------------
//
EXPORT_C TInt CIRFavoritesDb::AddObserver( 
                                const MPSPresetObserver& aObserver )
    {
    IRLOG_DEBUG( "CIRFavoritesDb::AddObserver" );
    return iObservers.InsertInAddressOrder( &aObserver );
    }


//---------------------------------------------------------------------------
// TInt CIRPresetHandler::RemoveObserver( MPSPresetObserver& aObserver )
// Removes an observer for a preset.
//---------------------------------------------------------------------------
//
EXPORT_C void CIRFavoritesDb::RemoveObserver(
                    const MPSPresetObserver& aObserver )
    {
    TInt idx = iObservers.FindInAddressOrder( &aObserver );
    if ( idx >= 0 )
        {
        iObservers.Remove( idx );
        iObservers.Compress();
        }
    }

//---------------------------------------------------------------------------
//TInt CIRFavoritesDb::MatchingPresetId( TInt aId )
//---------------------------------------------------------------------------
//
TInt CIRFavoritesDb::MatchingPresetId( TInt aId )
    {
    TInt indx = KErrNotFound;
    SortByIndex() ;
    for ( TInt i = 0; i < iFavPresetList.Count(); i++ )
        {
          if ( iFavPresetList[i]->Id() == aId )
            {
            indx = iFavPresetList[i]->Index();
          /*lint -save -e960 Note -- Violates MISRA Required Rule 58,
          non-switch break used*/
            break;
          /*lint -restore */
            }
        }
    return indx;
    }

//---------------------------------------------------------------------------
// TInt CIRFavoritesDb::CreatePresetL( TInt aIndex )
// creates a preset by the index aIndex
//---------------------------------------------------------------------------
//
CIRPreset* CIRFavoritesDb::CreatePresetL( TInt aIndex )
    {
    IRLOG_DEBUG( "CIRFavoritesDb::CreatePresetL" );
    CIRPreset* preset = static_cast<CIRPreset*>( iServ.CreatePresetL(
                            aIndex, KIRPreset) );
    CleanupStack::PushL( preset );
    iFavPresetList.AppendL( preset );
    CleanupStack::Pop( preset );
    IRLOG_DEBUG( "CIRFavoritesDb::CreatePresetL - Exiting." );
    return preset;
    }

//---------------------------------------------------------------------------
// TInt CIRFavoritesDb::SearchUserDefinedPreset
// Search a user defined preset by name and url
//---------------------------------------------------------------------------
//
TInt CIRFavoritesDb::SearchUserDefinedPreset( const TDesC &aName, 
                                              const TDesC &aUrl )
    {
    TInt count = iFavPresetList.Count();
    for ( TInt index = 0; index < count; ++index )
        {
        CIRPreset *preset = iFavPresetList[index];
        const TDesC &name = preset->Name();
        TInt urlCount = preset->GetUrlCount();
        if( preset->GetUrlCount()>0 )
            {
            const TDesC &url = preset->GetChannelUrlAt( 0 );
            if ( preset->GetChannelType() == 0 &&
                 name == aName &&
                 url  == aUrl )
                {
                return index;
                }
            }
        else
            {
            if ( preset->GetChannelType() == 0 &&
                 name == aName &&
                 aUrl == KNullDesC )
                {
                return index;
                }                
            }
        }
    return KErrNotFound;
    }

//---------------------------------------------------------------------------
// CIRFavoritesDb::PresetByIndex(TInt aIndex)
// Returns a preset by its id.
//---------------------------------------------------------------------------
//
EXPORT_C CIRPreset* CIRFavoritesDb::PresetByIndex( TInt aIndex )
    {
    IRLOG_DEBUG( "CIRFavoritesDb::PresetByIndex" );

    CIRPreset* preset = NULL;

    for ( TInt i = 0; i < iFavPresetList.Count(); i++ )
        {
        if ( iFavPresetList[i]->Index() == aIndex )
            {
            preset = iFavPresetList[i];
            /*lint -save -e960 (Note -- Violates MISRA Required Rule 58, non-switch break used)*/
            break;
            /*lint -restore*/
            }
        }
    IRLOG_DEBUG( "CIRFavoritesDb::PresetByIndex - Exiting." );
    return preset;
    }

//---------------------------------------------------------------------------
// CIRFavoritesDb::PresetById(TInt aIndex)
// Returns a preset by its id.
//---------------------------------------------------------------------------
//
EXPORT_C CIRPreset* CIRFavoritesDb::PresetById( TInt aId )
    {
    IRLOG_DEBUG( "CIRFavoritesDb::PresetById" );

    CIRPreset* preset = NULL;

    for ( TInt i = 0; i < iFavPresetList.Count(); i++ )
        {
        if ( iFavPresetList[i]->Id() == aId )
            {
            preset = iFavPresetList[i];
            /*lint -save -e960 Note -- Violates MISRA Required Rule 58,
            non-switch break used*/
            break;
            /*lint -restore */
            }
        }
    IRLOG_DEBUG( "CIRFavoritesDb::PresetById - Exiting." );

    return preset;
    }


//---------------------------------------------------------------------------
//CIRFavoritesDb::HandlePresetChangedL()
//function to notify a change in saved presets
//preset handler i.e CIRPreset,the reason of change.
//---------------------------------------------------------------------------
//
EXPORT_C void CIRFavoritesDb::HandlePresetChangedL( TInt aId,
    TUid aDataHandler, MPSPresetObserver::TPSReason aReason )
    {
    IRLOG_DEBUG( "CIRFavoritesDb::HandlePresetChangedL" );
    if ( aDataHandler == KIRPreset )
        {
        switch ( aReason )
            {
            case MPSPresetObserver::EPSCreated:
                {
                //call back from the preset client
                //to append a preset when the preset is added
                CPSPresetInterface* preset = NULL;
                preset = PresetById( aId );
                if ( !preset )
                    {
                    preset = iServ.OpenPresetL( aId );
                    CleanupStack::PushL( preset );
                    iFavPresetList.AppendL( 
                            static_cast<CIRPreset*>( preset ) );
                    CleanupStack::Pop( preset );
                    }

                break;
                }
            case MPSPresetObserver::EPSDeleted:
                {
                //call back from the preset client
                //to remove a preset when the preset is deleted
                for ( TInt i = 0; i < iFavPresetList.Count(); i++ )
                    {
                    if ( iFavPresetList[i]->Id() == aId )
                        {
                        delete iFavPresetList[i];
                        iFavPresetList.Remove( i );
                        /*lint -save -e960 (Note -- Violates MISRA Required Rule 58, non-switch break used)*/
                        break;
                        /*lint -restore*/
                        }
                    }
                break;
                }
            case MPSPresetObserver::EPSModified:
                break;
            default:
                break;
            }

        for ( TInt i = 0; i < iObservers.Count(); i++ )
            {
            iObservers[i]->HandlePresetChangedL( aId, aDataHandler, aReason );
            }
            
        }
    IRLOG_DEBUG( "CIRFavoritesDb::HandlePresetChangedL - Exiting." );
    }

// end of file
