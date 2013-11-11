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


#ifndef  IRFAVORITESDB_H
#define  IRFAVORITESDB_H

#include "pspresetobserver.h"
#include "psserv.h"

class CIRIsdsPreset;
class CIRPreset;
class CPSPresetNotifier;


//typedefed to array of pointer array
typedef RPointerArray<CIRPreset> RIRPresetArray;

//this class is an handle to the presetclient
//provides apis to access the presetserver functionalities

NONSHARABLE_CLASS( CIRFavoritesDb ) : public CBase, 
                                      public MPSPresetObserver
    {
public:
    /**CIRFavoritesDb::NewL()
    *standard symbian 1st phase constructor
    *@return CIRFavoritesDb* ,db instance
    */
    IMPORT_C static CIRFavoritesDb* NewL();


    /**CIRFavoritesDb::~CIRFavoritesDb()
    *standard c++ destructor
    */
    ~CIRFavoritesDb();


    /**CIRFavoritesDb::AddPresetL()
    *exported function to add a preset
    *@param CIRIsdsPreset& the isds preset,TInt index(not in use for 3.2)
    *@return KerrNone on success,KErrNoMemory if the no of presets is 20
    */
   IMPORT_C void AddPresetL( CIRIsdsPreset& aPreset,
                             TInt& aIndex, TBool aIsLogoAvialable = EFalse );


    /**CIRFavoritesDb::AddPresetL()
    *exported,overloaded function to add a preset manually
    *@param TDesC name,TDesC url of the preset,
    *@param TInt,TIntindex and id not in use for 3.2
    *@return KerrNone on success,KErrNoMemory if the no of presets is 20
    */
    IMPORT_C void AddPresetL( const TDesC& aName, const TDesC& aURl,
        TInt& aReturn, TBool aIsLogoAvialable = EFalse );


    /**CIRFavoritesDb::DeletePresetL()
    *exported, function to delete a preset
    *@param TInt is the id of the preset to be deleted
    *@return KerrNone on success,
    */
   IMPORT_C void DeletePresetL( TInt aUniqId );


    /**CIRFavoritesDb::GetAllPresetL()
    *exported,function to get the list of saved presets in the array iFavPresetList
    */
   IMPORT_C void GetAllPreset();

    /*
     * get all presets sorted by played times
     */
    IMPORT_C const RIRPresetArray& GetAllSortedPresets();
    
    /**CIRFavoritesDb::SearchPreset()
    *exported, function to find out if a preset exists in the favorites list
    *@param TInt the id of the preset to search.
    *@return the index number of the preset being searched(if found) 
    *@else KErrNotFound
    */
   IMPORT_C  TInt SearchPreset( const TInt aUniqPresetId,
                              const TInt aIsdsPresetId );


    /**CIRFavoritesDb::GetPreviousPreset()
    *exported, function to find out if a preset exists in the favorites list
    *@param TInt the id of the preset .
    *@return the index number of the previous preset
    */
   IMPORT_C TInt GetPreviousPreset( TInt aIndex );


    /**CIRFavoritesDb::GetNextPreset()
    *exported, function to find out if a preset exists in the favorites list
    *@param TInt the id of the preset .
    *@return the index number of the next preset
    */
   IMPORT_C TInt GetNextPreset(TInt aIndex);



    /**CIRFavoritesDb::SwapPresetsInDbL()
    *returns the previous preset for a given presetId
    *@param TInt,TInt,TInt,TIntswaps the presets between two
    *channelIds for given channel Indices
    *@return TInt system wode error code
    **/
   IMPORT_C void SwapPresetsInDbL( TInt aChannelIdFrom,TInt aChannelIdTo,
               TInt aChannelIndexFro,TInt aChannelIndexTo );


    /**CIRFavoritesDb::ReplacePresetL()
    *replaces a preset with a new preset
    *@param CIRIsdsPreset instance
    *for presetSync
    *@return TInt system wode error code
    **/
   IMPORT_C void ReplacePresetL( CIRIsdsPreset& aNewPreset );


   /**CIRFavoritesDb::ReplaceUserDefinedPresetL()
    *replaces a userdefined preset with a new userdefined preset
    *@param CIRIsdsPreset instance
    *for presetSync
    *@return TInt system wide error code
    **/
   IMPORT_C void ReplaceUserDefinedPresetL( CIRIsdsPreset& aNewPreset );
   

    /**CIRFavoritesDb::MakePresetUserDefinedL()
    *for a favorite preset that has been removed from the isds.
    *it is made a user defined preset by changing the type to 0.
    *index value is preseved so that the relative positions in the saved
    *stations view remains the same.
    *@param TInt,TInt
    *@return TInt
    **/
   IMPORT_C void MakePresetUserDefinedL( TInt aChannelId,
                                         TInt aUserDefinedChannelId );

    /**
    *const RVRPresetArray& CVRPresetHandler::Presets() const
    * @return RIRPresetArray& an array containing all Internet Radio presets.
    */
    IMPORT_C const RIRPresetArray& Presets() const;
    /**
    *TInt CIRFavoritesDb::EmptyPresetCount() const
    *Returns the number of empty presets
    *@return TInt the number of empty presets
    */
    IMPORT_C TInt EmptyPresetCount() const;

   /**
    *TInt CIRFavoritesDb::AddObserver( MPSPresetObserver& aObserver )
    *Adds an observer that is notified upon changes in presets.
    *@param MPSPresetObserver&
    */
    IMPORT_C TInt AddObserver( const MPSPresetObserver& aObserver );

   /**
    *TInt CIRPresetHandler::RemoveObserver( MPSPresetObserver& aObserver )
    *Removes an observer for a preset.
    */
    IMPORT_C void RemoveObserver(const MPSPresetObserver& aObserver );

    /**CIRFavoritesDb::MaxPresetCount()
    *returns the maximum number of presets that 
    *can be stored in the favorites list
    *@return TInt maximum number of presets 
    *that can be stored in the favorites list
    **/
    IMPORT_C TInt MaxPresetCount();
    
    /** CIRFavoritesDb::SetMoveStatus(TBool aStatus)
    * @aStatus, sets the status of the Move functionality progression
    **/
    IMPORT_C void SetMoveStatus( TBool aStatus );
    
    /** CIRFavoritesDb::GetMoveStatus()
    * returns the status of the Move functionality
    **/
    IMPORT_C TBool GetMoveStatus();

    /*
     * Increase the played times of a channel if it has been in the favorites
     * return : KErrNone if success
     *          KErrNotFound if the preset is not in the favorites
     */
    IMPORT_C TInt IncreasePlayedTimesL( const CIRIsdsPreset &aIsdsPreset );
    
    /**CIRFavoritesDb::RenamePresetL()
    *renames a preset with a new name
    *@param CIRIsdsPreset instance
    *@param a new name
    **/
    IMPORT_C TInt RenamePresetL( const CIRIsdsPreset &aIsdsPreset, 
                                      const TDesC &aNewName );
                                      
private:
    /**CIRFavoritesDb::HandlePresetChangedL()
    *function to notify a change in saved presets
    *@param TInt,TUid,TPSReason the id of the preset,the id of the
    *preset handler i.e CIRPreset,the reason of change.
    */
    void HandlePresetChangedL( TInt aId, TUid aDataHandler, 
                               MPSPresetObserver::TPSReason aReason );

      
    /**CIRFavoritesDb::MovePresetL()
    *moves a preset to destination index
    *@param TInt,TInt,the preset id ,the destination index
    **/
    void MovePresetL( const TInt aId, const TInt aDestinationIndex );


    /**CIRFavoritesDb::ConstructL()
    *Standard 2nd phase construction
    **/
    void ConstructL();


    /**CIRFavoritesDb::SortByIndex()
    *sorts the preset list by index
    **/
    void SortByIndex();

    /**
    *TInt CIRFavoritesDb::MatchingPresetId( TInt aId )
    *returns the index of the preset whose id is aId
    *@return TInt the index of the preset whose id is aId
    */
    TInt MatchingPresetId( TInt aId );


    /**
    *TInt CIRFavoritesDb::CreatePresetL( TInt aIndex )
    *creates a preset by the index aIndex
    *@param TInt aIndex,index of the new preset
    *@return CIRPreset*
    */
    CIRPreset* CreatePresetL( TInt aIndex );

    /*
     * Search a user defined preset by name and url
     */
    TInt SearchUserDefinedPreset( const TDesC &aName, const TDesC &aUrl );
    
public:
    /**
     *CIRFavoritesDb::PresetByIndex(TInt aIndex)
     * Returns a preset by its id.
     * @param   aId     Id of the preset.
     * @return  The preset matching the id or <code>NULL</code> if no such preset exists.
     */
    CIRPreset* PresetByIndex( TInt aIndex );


    /**
    * CIRFavoritesDb::PresetById(TInt aIndex)
    * Returns a preset by its id.
    * @param   aId     Id of the preset.
    * @return  The preset matching the id or <code>NULL</code> if no such preset exists.
    */
    IMPORT_C CIRPreset* PresetById( TInt aId );


    /**the preset list accessible to ui*/
    /** Internet Radio presets currently stored in the preset server. */
    RIRPresetArray iFavPresetList;
private:

    /** Session with the preset server. */
    RPSServ iServ;
    
    /** Notifier that informs about changes in presets. */
    CPSPresetNotifier* iNotifier;
    /** Observers to notify when presets change. */
    RPointerArray<MPSPresetObserver> iObservers;

    /** The maximum number of supported presets. */
    TInt iMaxPresetCount;

    /**type of presets(source)*/
    enum TChannelType
        {
        EUserDefined,/**added by user*/
        EIsdsPreset,/**fetched from isds*/
        };
    
    /** iMoveStatus, to know the status of the Move funcitionality */
    TBool iMoveStatus;
};

#endif  //end IRFAVORITESDB_H
