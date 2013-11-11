/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Database manager implementation 
*
*/

#ifndef __MYLOCATIONSDATABASEMANAGER_H__
#define __MYLOCATIONSDATABASEMANAGER_H__


#include <EPos_CPosLandmark.h>

#include <locationservicedefines.h>
// lookup database
#include <locationdatalookupdb.h>


class CPosLandmarkDatabase;
class CPosLmCategoryManager;


/**
 * CMyLocationsDatabaseManager class.
 *  This class handles all the operations related to mylocations landmark database.
 *
 */
class  CMyLocationsDatabaseManager : public CBase
    {
    public:

        // Constructors and destructor
    
        /**
         * CMyLocationsDatabaseManager.
         * C++ default constructor. 
         */
        IMPORT_C CMyLocationsDatabaseManager();

        /**
         * ConstructL.
         * 2nd phase constructor.
         */
        IMPORT_C void ConstructL();
        
        /**
         * ~CMyLocationsDatabaseManager.
         * Virtual Destructor.
         */
        virtual ~CMyLocationsDatabaseManager();
        
    public:
        /**
         * UpdateDatabaseL.
         * Updates the location into the landmark database and lookup table. 
         * Based on the entry source type and the entry change type the updation can be 
         * addition/modification/deletion.
         * 
         * @param[in] aLandmark Landmark object to be updated in the database.
         * @param[in] aUid Uid of the changed source entry.
         * @param[in] aSourceType Source type of the aUid.
         * @param[in] aChangeType Type of change to the entry ( added/modified/deleted )
         */
        IMPORT_C void UpdateDatabaseL( CPosLandmark* aLandmark, const TUint32 aUid, 
                              const TUint32 aSourceType, const TUint32 aChangeType );
        
        /** Gets the full address from a given landmark object
         *
         * @param[out] aLandmarkAddress comma separated landmark's full address
         * @param[in] aLandmark landmark object whose address needs to be found.
         */      
        IMPORT_C void GetLandmarkFullAddress( TBuf<255>& aLandmarkAddress, 
                                     const CPosLandmark* aLandmark );
       
	    /** Update the maptile path to mylocation lookup table
         *
         * @param[in] aSourceId Uid of the changed source entry.
		 * @param[in] aSourceType Source type of the aSourceId.
         * @param[in] aFilePath Maptile file path.
         */                               
        IMPORT_C void UpdateMapTilePath( TUint32 aSourceId, TUint32 aSourceType, 
                                            TFileName& aFilePath );

		/** Update the maptile path to mylocation lookup table
		 *
		 * @param[in] aSourceId Uid of the changed source entry.
		 * @param[in] aSourceType Source type of the aSourceId.
		 * @param[in] aName entry name.
		 */ 				 

		IMPORT_C void UpdateEntryName( TUint32 aSourceId, TUidSourceType aSourceType, 
                                            const TDesC& aName );
		
        /** Compare the address details to lplookupaddres  table
         *
         * @param aLandmarks , address formed as landmark object
         * @parama Id , id of entry to be checked in db table
         * @param aAddressType , type of entry.
         * @return , true for match case .
         */  
        IMPORT_C TBool CheckIfAddressChanged(const CPosLandmark& aLandmarks,
                const TUint32 aId, const TUidSourceType aAddressType);
        
        /** Compare the address details to lplookupaddres  table
         *
         * @param aAddress , one line address .
         * @parama Id , id of entry to be checked in db table
         * @param aAddressType , type of entry.
         * @return , true for match case .
         */
        IMPORT_C TBool CheckIfAddressChanged(const TDesC& aAddress,
                const TUint32 aId, const TUidSourceType aAddressType);        
        
    /**
       * Updates the calendar entry  
       * @param aId , id of the caleentry
       * @param aChangeType , changetype of the calendar entry
       * @param aLocation , location details of the calendar entry 
       */
        IMPORT_C void UpdateCalendarLocationById(const TUint32 aUid, const TUint32 aChangeType,const TDesC& aLocation);    
      
      /**
       * gets all the calendar entry  
       * @param aCalEntryArray , refference to all entry
       */
        IMPORT_C void GetAllCalendarEntry(QList<QCalendarLocationDetails>& aCalEntryArray);

#ifdef LOCATION_DATA_HARVESTER_UNIT_TEST
public:
#else
private:
#endif
        
        /**
         * AddMylocationsCategory.
         * Adds the category to landmarks database. 
         * Used to add location picker specific categories.
         * @param[in] aCategoryName defines the category name to be added.
         * @returns The category id. 
         */
        TUint32 AddMylocationsCategoryL(  const TDesC&  aCategoryName  );

        /**
         * CheckIfDuplicateExistsL.
         * Checks if this landmark is already present in database. 
         * If present returns the landmark id, else 0.
         * @param[in] aLandmark A landmark object to be checked for.
         * @returns If no duplicate is found in database returns 0 
         * else returns the id of the found landmark.
         */
        TPosLmItemId CheckIfDuplicateExistsL( const CPosLandmark* aLandmark );
        
        /**
         * CompareLandmarks.
         * Compares two landmarks. Only the text fields, landmark name, 
         * street, city, state country and postal code are compared. 
         * @param[in] aLandmark1 First landmark object to be compared.
         * @param[in] aLandmark2 Second landmark object to be compared.
         * @returns ETrue if the landmarks are same, else returns EFalse.
         */
       TBool CompareLandmarks( const CPosLandmark* alandmark1, 
                               const CPosLandmark* alandmark2 );
        
       /**
         * HandleEntryAdditionL.
         * Handles the entry addition in lookup table and landmarks db
         * @param[in] aLandmark Landmark object added.
         * @param[in] aUid Uid of the source entry corresponding to aLandmark..
         * @param[in] aSourceType source type of the aUid passed.
        */
       void HandleEntryAdditionL( CPosLandmark*  aLandmark, const TUint32 aUid,
                                  const TUint32 aSourceType );

       /**
         * HandleEntryModificationL.
         * Handles the entry modification in lookup table and landmarks db
         * @param[in] aLandmark Landmark object modified.
         * @param[in] aUid Uid of the source entry corresponding to aLandmark..
         * @param[in] aSourceType source type of the aUid passed.
        */
       void HandleEntryModificationL( CPosLandmark*  aLandmark, const TUint32 aUid,
                                      const TUint32 aSourceType );

       /**
         * HandleEntryDeletionL.
         * Handles the entry deletion in lookup table and landmarks db
         * @param[in] aUid Uid of the source entry corresponding to aLandmark..
         * @param[in] aSourceType source type of the aUid passed.
        */
       void HandleEntryDeletionL( const TUint32 aUid, const TUint32 aSourceType );

       /**
         * HandleLandmarkModificationL.
         * Handles a landmark modification
         * @param[in] aLandmark Landmark object modified.
         * @param[in] aUid Uid of the source entry corresponding to aLandmark..
        */
       void HandleLandmarkModificationL( CPosLandmark* aLandmark, const TUint32 aUid );
       /**
          * CheckAndReadLandmarkL.
          * Checks whether a category is available in database pointed by category manager.
          * @param[in] CPosLandmarkDatabase handle to the database manager
          * @param[in] aLmId Id of the landmark
          * @returns landmark object if found else NULL
         */
       CPosLandmark* CheckAndReadLandmarkL( 
                       CPosLandmarkDatabase* aDb, const TUint32 aLmId );
        /**
          * FillLookupItemAddressDetails.
          * Fills address details into QLookupItem from CPosLandmark.
          * @param[in] aLandmark a landmark object
          * @param[out] aLookupItem, a lookup item in which the address details are filled
         */
       void FillLookupItemAddressDetails( CPosLandmark* aLandmark, QLookupItem &aLookupItem );

        /**
          * UnsetDuplicateNextCalEntry.
          * Finds next calendar lookup entry whose detination id  is aLandmarkId
		  * and unsets it duplcate flag.
          * @param[in] aCurrentCalUid current calendar entry id.
          * @param[in] aLandmarkId a landmark id.
         */
       void UnsetDuplicateNextCalEntry( quint32 aCurrentCalUid, quint32 aLandmarkId );

        /**
          * IsDuplicateCalEntry.
          * Checks if there is a duplicate entry present whose destination id 
		  * is aLandmarkId
          * @param[in] aLandmarkId a landmark id.
         */
       bool IsDuplicateEntry( quint32 aLandmarkId );

        /**
          * CreateLandmarkItemLC.
          * Creates a landmark from a QLookupItem
          * @param[in] aLookupItem a lookup item.
          * @returns CPosLandmark a newly created landmark.
         */
       CPosLandmark* CreateLandmarkItemLC( const QLookupItem &aLookupItem );
       
#ifdef LOCATION_DATA_HARVESTER_UNIT_TEST
public:
#else
private:
#endif
        // Handle to the landmark database
        CPosLandmarkDatabase* iLandmarkDb;

        // Calendar category to be created for calendar related location entries in landmark database
        TPosLmItemId iLmCalendarCatId;  
        // Contacts category to be created for contacts related location entries in landmark database
        TPosLmItemId iLmContactsCatId;  
       
        // handle to the location app lookup database.
        LocationDataLookupDb*  iLocationAppLookupDb;

        // handle to landmarks category manager
        CPosLmCategoryManager* iLandmarksCatManager;

        // handle to the file session
        RFs iFsSession;
    };
#endif  // __MYLOCATIONSDATABASEMANAGER_H__
