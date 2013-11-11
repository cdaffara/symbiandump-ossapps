/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class provides the functions to manipulate the a category
*                table in the podcast collection database.
*
*/


#ifndef MPXDBCATEGORY_H
#define MPXDBCATEGORY_H

// INCLUDES
#include "mpxdbtable.h"
#include "mpxmediageneraldefs.h"
#include "mpxpodcastdefs.h"

// FORWARD DECLARATIONS
class CMPXMediaArray;

// CLASS DECLARATION

/**
* Responsible for managing all category tables
*
* @lib mpxpodcastdbplugin.lib
*/
class CMPXDbCategory :
    public CMPXDbTable
    {
    public:

        /**
        * Two-phased constructor. Leaves on failure. Places the instance on the
        * cleanup stack.
        * @param aDbManager Database manager
        * @param aCategory Category to identify the table
        * @return New CMPXDbCategory instance
        */
        static CMPXDbCategory* NewLC(CMPXDbManager& aDbManager,
            TMPXGeneralCategory aCategory);

        /**
        * Two-phased constructor. Leaves on failure.
        * @param aDbManager Database manager
        * @param aCategory Category to identify the table
        * @return New CMPXDbCategory instance
        */
        static CMPXDbCategory* NewL(CMPXDbManager& aDbManager,
            TMPXGeneralCategory aCategory);

        /**
        * Destructor.
        */
        virtual ~CMPXDbCategory();

    public:

        /**
        * Add a row to the lookup table
        * @param aName This is the name to be entered into the row
        * @param aDriveId The Drive Id the name (category) belongs
        * @param aNewRecord indicates to the caller if a new record is created. ETrue
        *        if a new row is created in the table; otherwise EFalse.
        * @param aCaseSensitive indicates whether case sensitivity should be taken
        *        into consideration when generating the unique row id
        * @return The unique id of the row added.
        */
        TUint32 AddItemL(const TDesC& aName, TInt aDriveId, TBool& aNewRecord,
            TBool aCaseSensitive = ETrue);

        /**
        * Get the name field for a given ID.
        * @param aId identifies the category item
        * @return name that matches aId. Ownership is abandoned.
        */
        HBufC* GetNameL(TUint32 aId);

        /**
        * Gets the total number of items in the category table.
        * @return number of items
        */
        TInt CountL();

        /**
        * Find record(s) matching the criteria
        * @param aCriteria, selection criteria
        * @param aAttrs, attributes for the matching record(s)
        * @param aMediaArray returns the matching record(s)
        */
        void FindAllL(const CMPXMedia& aCriteria, const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Decrement the number of episodes in the row. If the count gets to 0, remove the row.
        * @param aId The ID of the row to decrement.
        * @param aDriveId The drive Id the name (category) belongs to. Episodes on different
        *                  drives may belong to the same podcast title or composer; consequently,
        *                  one row for each podcast title/composer id and drive ID pair will exist
        *                  in the look up table with a count existing for each row
        *                  that tracks the number number of episodes on that drive with that
        *                  podcast title/composer.
        * @param aItemChangedMessages Returns the changed messages if valid
        */
        void DecrementEpisodesForCategoryL(const TUint32 aId, TInt aDriveId,
            CMPXMessageArray* aItemChangedMessages);

        /**
        * Deletes a category.
        * @param aId identifies the category
        * @param aDriveId category drive
        */
        void DeleteCategoryL(TUint32 aId, TInt aDriveId);

        /**
        * Gets details for one category item.
        * @param aId identifies the category item
        * @param aAttrs attributes to be returned
        * @param aMedia returns the requested attributes for te item
        * @leave KErrNotFound if there is no item with the specified ID
        */
        void GetCategoryItemL(TUint32 aId, const TArray<TMPXAttribute>& aAttrs, CMPXMedia& aMedia);

        /**
        * Gets the details for multiple category items.
        * @param aAttrs attributes to be returned
        * @param aMediaArray inout parameter, on input contains the IDs of the category items to
        *       be returned, on exit contains the requested attributes for these items
        */
        void GetCategoryItemsL(const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Gets the details for all category items.
        * @param aAttrs attributes to be returned
        * @param aMediaArray returns the requested attributes for all items
        */
        void GetAllCategoryItemsL(const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Checks if the specified category item exists.
        * @param aDriveId drive to check or KDbManagerAllDrives
        * @param aId identifies the category item
        * @return ETrue if it exists, EFalse otherwise
        */
        TBool CategoryItemExistsL(TInt aDriveId, TUint32 aId);

        /**
        * Gets the number of episodes for a category item
        * @param aDriveId drive to check or KDbManagerAllDrives
        * @param aId identifies the category item
        * @return number of episodes
        */
        TInt GetEpisodeCountL(TInt aDriveId, TUint32 aId);

    private:

        /**
        * Updates the media with information from the table
        * @param aRecord record containing the source fields
        * @param aAttrs attributes to be returned
        * @param aMedia returns the requested attributes
        */
        void UpdateMediaL(RSqlStatement& aRecord, const TArray<TMPXAttribute>& aAttrs,
            CMPXMedia& aMedia);

        /**
        * Returns the category record for a given ID
        * @param aId identifies the category item
        * @return rowset
        */
        RSqlStatement GetCategoryRecordL(TUint32 aId);

        /**
        * Replaces the table placeholder in the string parameter with the
        * category table name.
        * @param aQuery raw query string
        * @return processed query string, ownership is transferred.
        */
        HBufC* PreProcessStringLC(const TDesC& aQuery);

        /**
        * Processes a recordset by constructing media instances and adding them
        * to the specified array.
        * @param aAttrs attributes to be returned
        * @param aRecordset recordset to be processed
        * @param aMediaArray returns the media instances
        */
        void ProcessRecordsetL(const TArray<TMPXAttribute>& aAttrs,
            RSqlStatement& aRecordset, CMPXMediaArray& aMediaArray);

    private:    // from MMPXTable

        /**
        * @see MMPXTable
        */
        virtual void CreateTableL(RSqlDatabase& aDatabase, TBool aCorruptTable);

        /**
        * @see MMPXTable
        */
        virtual void DropTableL(RSqlDatabase& aDatabase);

        /**
        * @see MMPXTable
        */
        virtual TBool CheckTableL(RSqlDatabase& aDatabase);

    private:

        /**
        * C++ constructor.
        * @param aDbManager Database manager insteance to be used
        * @param aCategory category for this instance
        */
        CMPXDbCategory(CMPXDbManager& aDbManager, TMPXGeneralCategory aCategory);

        /**
        * Second-phase constructor.
        */
        void ConstructL();

    private:

        /**
        * Columns in the category table
        */
        enum TCategoryColumns
            {
            ECategoryUniqueId = KMPXTableDefaultIndex,
            ECategoryName,
            ECategoryEpisodeCount
            };

    protected:      // Data

        HBufC* iTableName;                  // owned
        TMPXGeneralCategory iCategory;      // identifies this table
    };

#endif  // CMPXDbCategory_H

// End of File

