/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is responsible for all category tables (Artist,
*                Album, Genre, Composer).
*
*
*/


#ifndef MPXDBCATEGORY_H
#define MPXDBCATEGORY_H

// INCLUDES
#include "mpxdbtable.h"
#include "mpxmediageneraldefs.h"
#include "mpxmessage2.h"

// CLASS FORWARDS
class CMPXMedia;
class CMPXMediaArray;
class RSqlStatement;

// CLASS DECLARATION

/**
* Responsible for managing all music databases
*
* @lib MPXDbPlugin.lib
*/
class CMPXDbCategory :
    public CMPXDbTable
    {
    protected:
        /**
        * Destructor
        */
        virtual ~CMPXDbCategory();


    public:

        /**
        * Add a category item. If the record already exists, its counter will
        * be incremented.
        * @param aCategory category type
        * @param aMedia: This is the media object to be entered into the row
        * @param aDriveId: The Drive Id the name (category) belongs
        * @param aNewRecord: indicates to the caller if a new record is created.
        *        ETrue if a new row is created in the table; otherwise EFalse.
        * @param aCaseSensitive indicates whether case sensitivity should be taken
        *        into consideration when generating the unique row id
        * @return The unique id of the row added.
        */
        virtual TUint32 AddItemL(TMPXGeneralCategory aCategory, const CMPXMedia& aMedia, TInt aDriveId, TBool& aNewRecord,
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
        * Decrement the number of songs for the item. If the count gets to 0, remove
        * the item.
        * @param aId: The ID of the category to delete.
        * @param aDriveId: The drive Id the name (category) belongs to. Songs on different
        *                  drives may belong to the same album or artist; consequently, one
        *                  row for each artist/album id and drive ID pair will exist in the
        *                  lookup table with a count existing for each row that tracks the
        *                  number number of songs on that drive with that album/artist.
        * @param aItemChangedMessages if valid on return contains a deleted message if the
        *                  category was deleted
        * @param aItemExist Out parameter, ETrue if the category is not deleted after the delete,
        *        EFalse otherwise
        */
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        virtual void DecrementSongsForCategoryL(TUint32 aId, TInt aDriveId,
            CMPXMessageArray* aItemChangedMessages, TBool& aItemExist, TBool aMtpInUse = EFalse);
#else
        virtual void DecrementSongsForCategoryL(TUint32 aId, TInt aDriveId,
            CMPXMessageArray* aItemChangedMessages, TBool& aItemExist);
#endif // ABSTRACTAUDIOALBUM_INCLUDED
        /**
        * Deletes a category.
        * @param aId identifies the category
        * @param aDriveId category drive
        */
        void DeleteCategoryL(TUint32 aId, TInt aDriveId);

        /**
        * Gets the details for multiple category items.
        * @param aAttrs attributes to be returned
        * @param aMediaArray inout parameter, on input contains the IDs of the
        *       category items to be returned, on exit contains the requested
        *       attributes for these items
        */
        void GetCategoryItemsL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Gets the details for all category items.
        * @param aAttrs attributes to be returned
        * @param aMediaArray returns the requested attributes for all items
        */
        void GetAllCategoryItemsL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Gets details for one category item.
        * @param aId identifies the category item
        * @param aAttrs attributes to be returned
        * @param aMedia returns the requested attributes for te item
        * @leave KErrCorrupt if there are multiple items with the same ID
        *        KErrNotFound if there is no item with the specified ID
        */
        void GetCategoryItemL(TUint32 aId, const TArray<TMPXAttribute>& aAttrs,
            CMPXMedia& aMedia);

        /**
        * Gets details for the category items under a parent category item.
        * This applies to albums for a given artist. The method panics if the
        * category or parent category are invalid.
        * @param aParentCategory identifies the parent category
        * @param aParentId identifies the parent item
        * @param aAttrs attributes to be returned
        * @param aMediaArray returns the requested attributes for all items
        */
        virtual void GetSubCategoryItemsL(TMPXGeneralCategory aParentCategory, TUint32 aParentId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Checks if the specified category item exists.
        * @param aDriveId drive to check or KDbManagerAllDrives
        * @param aId identifies the category item
        * @return ETrue if it exists, EFalse otherwise
        */
        TBool CategoryItemExistsL(TInt aDriveId, TUint32 aId);

        /**
        * Gets the CountSongs field for a category item
        * @param aDriveId drive to check or KDbManagerAllDrives
        * @param aId identifies the category item
        * @return value of the CountSongs field.
        */
        TInt GetSongsCountL(TInt aDriveId, TUint32 aId);

        /**
        * Update a category item.
        * @param aId: The ID of the category to update
        * @param aMedia: The media data
        * @param aDriveId: The Drive Id the name (category) belongs
        * @param aItemChangedMessages: if valid on return contains a updated message if the
        * category was updated
        */       
        virtual void UpdateItemL(TUint32 aId, const CMPXMedia& aMedia, TInt aDriveId, CMPXMessageArray* aItemChangedMessages);

    protected:

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

    private:
        /**
        * Retrieve name from media object based on category
        * @param aCategory the category to retrieve name
        * @param aMedia media object
        * @return name
        */
        TPtrC ItemNameL(TMPXGeneralCategory aCategory, const CMPXMedia& aMedia);
        
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

    protected:

        /**
        * C++ constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        */
        CMPXDbCategory(CMPXDbManager& aDbManager, TMPXGeneralCategory aCategory);

        /**
        * Second phase constructor.
        */
        void BaseConstructL();

    private:

        /**
        * Column indexes in the category tables
        */
        enum TCategoryColumns
            {
            ECategoryUniqueId = KMPXTableDefaultIndex,
            ECategoryName,
            ECategorySongCount
            };

    protected:

        HBufC* iTableName;              // owned
        TMPXGeneralCategory iCategory;

    };

#endif // MPXDBCATEGORY_H

// End of File
