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
* Description: 
*     Retrieving maptile path from lookup db
*
*/

#ifndef __MAPTILEDBLOOKUPTABLE_H__
#define __MAPTILEDBLOOKUPTABLE_H__

// DLL_EXPORT macro definition
#ifdef  MAPTILESERVICEDLL
#define MAPTILESERVICE_EXPORT Q_DECL_EXPORT
#else
#define MAPTILESERVICE_EXPORT Q_DECL_IMPORT
#endif

#include <locationservicedefines.h>

// forward declarations
class QSqlDatabase;



/**
 * LookupMapTileDatabase class.
 * This class handles all the operations related to maptile lookup database.
 *
 */
class MAPTILESERVICE_EXPORT LookupMapTileDatabase : public QObject
{
public:
 
    /**
    * Constructor
    */
    LookupMapTileDatabase( QObject *parent = 0 );	

    /**
    * Destructor
    */
    ~LookupMapTileDatabase();  

    /**
    * Opens the lookup database.
    */
    bool open();

    /**
    * Closes the lookup database.
    */
    void close();
 

public:


    /**
    * Creates an entry in the lookup table.
    * @param[in] aLookupItem The lookup item to be created in the database.
    */
    void createEntry( const MaptileLookupItem& aLookupItem );

    /**
    * Updates an entry in the lookup table.
    * @param[in] aLookupItem The lookup item to be updated in the database.
    */
    void updateEntry( const MaptileLookupItem& aLookupItem );

    /**
    * Updates the user setting of an calendar entry in the lookup table.
    * @param[in] aLookupItem The lookup item to be updated in the database.
    */
    void updateUserSetting( const MaptileLookupItem& aLookupItem );
    /**
    * Deletes an entry from the lookup table.
    * @param[in] aLookupItem The lookup item to be deleted from the database.
    */
    void deleteEntry( MaptileLookupItem& aLookupItem );
    
    /**
    * Deletes maptile from the repository if the ref to that maptile in the db reaches one
    * @param[in] aLookupItem The lookup item containing the path to the maptile
    */
    void deleteMapTile( const MaptileLookupItem& aLookupItem );
    

   /**
    * Gets an entry from the lookup table.
    * @param[in/out] aLookupItem The lookup item to be fetched from the database.
    * The source iUid and type is passed in the lookup item. If the item is found the aLookupItem remaining fields are updated.
    * @return true if the item is found.
    */
    bool getEntry( MaptileLookupItem& aLookupItem  );

    /**
    * Gets the list of calendar ids.
    * @param[out] aIdArray Calendar id list
    */
    void getAllCalendarIds( QList<quint32>& aIdArray );
    
    /**
    * ReSet the entry in the lookup table , with null value 
    * @param aLookupItem The lookup item entry to be reseted in the database,
    * get the used maptile path as part of lookupItem filepath , to free the resource 
    */
    void resetEntry( MaptileLookupItem &aLookupItem );
  
   /**
    * Finds an entry in the lookup table.
    * @param[in] aLookupItem The lookup item to be found in the database.
    * The source iUid and type is passed in the lookup item
    * @return true if the item is found.
    */
    bool findEntry( const MaptileLookupItem& aLookupItem );
    
    /**
     * Finds the number of address present in db associated with the aId. 
     * @param id, entry (calendar/contact) uid.
     * @return Number of address a contact has.
     */
    int findNumberOfAddress( int& aId );

    /**
    * Finds an entry in the lookup table for the given maptile name.
    * @param[in] aFilePath The lookup item to be found in the database. maptile file path is passed
    * @return ETrue if found, else EFalse
    */
    bool findEntryByFilePath(const QString& aFilePath);

    /**
    * Get all the entries in the lookup table for a given fetching status.
    * @param[in] aFetchingState The fetching status to be found in the database.
    * @param[out] aLookupItemArray List of lookup items having the fetching status as aFetchingState.
    */
    void findEntriesByMapTileFetchingState(const quint32 aFetchingState,
            QList<MaptileLookupItem>& aLookupItemArray);

private:
    
    // Handle to the items database
    QSqlDatabase *mDb;

                
    // Flag to indicate if db is open
    bool mDbOpen;

};


#endif  // __MAPTILEDBLOOKUPTABLE_H__`

// End of file

