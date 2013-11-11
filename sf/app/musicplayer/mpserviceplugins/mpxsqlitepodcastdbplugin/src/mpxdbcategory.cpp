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
* Description:  Implements the base class for the various lookup tables
*
*/


// INCLUDE FILES
#include <sqldb.h>

#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>
#include <mpxlog.h>

#include "mpxdbcommonutil.h"
#include "mpxdbcommondef.h"
#include "mpxdbmanager.h"

#include "mpxdbutil.h"
#include "mpxpodcastcollectiondbdef.h"
#include "mpxpodcastcollectiondbstd.h"
#include "mpxpodcastdbpluginqueries.h"
#include "mpxdbcategory.h"

// CONSTANTS

// maximum number of table name entries per query
const TInt KMaxTableNameCount = 2;

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbCategory* CMPXDbCategory::NewL(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory)
    {
    MPX_FUNC("CMPXDbCategory::NewL");

    CMPXDbCategory* self = CMPXDbCategory::NewLC(aDbManager, aCategory);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor, leave object in cleanup stack.
// ----------------------------------------------------------------------------
//
CMPXDbCategory* CMPXDbCategory::NewLC(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory)
    {
    MPX_FUNC("CMPXDbCategory::NewLC");

    CMPXDbCategory* self = new (ELeave) CMPXDbCategory(aDbManager, aCategory);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CMPXDbCategory::~CMPXDbCategory()
    {
    MPX_FUNC("CMPXDbCategory::~CMPXDbCategory");
    delete iTableName;
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CMPXDbCategory::CMPXDbCategory(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory) :
    CMPXDbTable(aDbManager),
    iCategory(aCategory)
    {
    MPX_FUNC("CMPXDbCategory::CMPXDbCategory");
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMPXDbCategory::ConstructL()
    {
    MPX_FUNC("CMPXDbCategory::ConstructL");

    BaseConstructL();
    iTableName = MPXDbUtil::TableNameForCategoryL(iCategory).AllocL();
    }

// ----------------------------------------------------------------------------
// Add a row containing the name to the lookup table
// If the name already exists in the table, increment the count
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbCategory::AddItemL(
    const TDesC& aName,
    TInt aDriveId,
    TBool& aNewRecord,
    TBool aCaseSensitive)
    {
    MPX_FUNC("CMPXDbCategory::AddItemL");

    // try to find the item first
    TUint32 rowId(MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), iCategory, aName, aCaseSensitive));
    aNewRecord = !CategoryItemExistsL(aDriveId, rowId);

    if (aNewRecord)
        {
        // insert new
        HBufC* query = PreProcessStringLC(KQueryCategoryInsert);
        HBufC* name = MPXDbCommonUtil::ProcessSingleQuotesLC(aName);

        iDbManager.ExecuteQueryL(aDriveId, *query, rowId, name, 1);

        CleanupStack::PopAndDestroy(name);
        CleanupStack::PopAndDestroy(query);
        }
    else
        {
        // increment the number of episodes for the category
        HBufC* query = PreProcessStringLC(KQueryCategoryIncrementEpisodeCount);
        iDbManager.ExecuteQueryL(aDriveId, *query, rowId);
        CleanupStack::PopAndDestroy(query);
        }

    return rowId;
    }

// ----------------------------------------------------------------------------
// Get the name of the row matching the given ID
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbCategory::GetNameL(
    TUint32 aId)
    {
    MPX_FUNC("CMPXDbCategory::GetNameL");

    RSqlStatement recordset(GetCategoryRecordL(aId));
    CleanupClosePushL(recordset);

    if (recordset.Next() != KSqlAtRow)
        {
        User::LeaveIfError(KErrNotFound);
        }

    HBufC* name = MPXDbCommonUtil::GetColumnTextL(recordset, ECategoryName).AllocL();
    CleanupStack::PopAndDestroy(&recordset);

    return name;
    }

// ----------------------------------------------------------------------------
// Count the total number of items
// ----------------------------------------------------------------------------
//
TInt CMPXDbCategory::CountL()
    {
    MPX_FUNC("CMPXDbCategory::CountL");

    HBufC* query = PreProcessStringLC(KQueryCategoryCount);
    TInt count(ExecuteSumQueryL(*query));
    CleanupStack::PopAndDestroy(query);

    return count;
    }

// ----------------------------------------------------------------------------
// Find record(s) that match(es) the selection criteria
// ----------------------------------------------------------------------------
//
void CMPXDbCategory::FindAllL(
    const CMPXMedia& aCriteria,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbCollection::FindAllL");

    // process the requested attributes
    // the UniqueId is always requested
    TBool titleRequested(EFalse);
    TBool counterRequested(EFalse);

    TInt viewingColumnCount(aAttrs.Count());
    for (TInt i = 0; i < viewingColumnCount; ++i)
        {
        TInt contentId(aAttrs[i].ContentId());

        if (contentId == KMPXMediaIdGeneral)
            {
            TUint attributeId(aAttrs[i].AttributeId());

            if (attributeId & EMPXMediaGeneralTitle)
                {
                titleRequested = ETrue;
                }
            if (attributeId & EMPXMediaGeneralCount)
                {
                counterRequested = ETrue;
                }
            }
        }

    TMPXGeneralType type = aCriteria.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType);

    const TArray<TMPXAttribute> criteria = aCriteria.Attributes();
    TInt criteriaCount(criteria.Count());

    // process the criteria and construct the criteria string
    CDesCArrayFlat* criteriaArray = new (ELeave) CDesCArrayFlat(criteriaCount + 1);
    CleanupStack::PushL(criteriaArray);

    TBool criteriaCounterSet(EFalse);
    TInt criteriaCounter(0);
    for (TInt i = 0; i < criteriaCount; ++i)
        {
        const TMPXAttribute& criterion = criteria[i];
        if ((type == EMPXItem) && (criterion == KMPXMediaGeneralId))
            {
            TUint32 itemId = (aCriteria.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId2;
            if (MPX_ITEM_CATEGORY(itemId) != iCategory)
                {
                User::Leave(KErrNotSupported);
                }

            HBufC* critStr = PreProcessStringLC(KCriterionCategoryUniqueId);
            MPXDbCommonUtil::AddSqlCriterionL(*criteriaArray, *critStr, itemId);
            CleanupStack::PopAndDestroy(critStr);
            }
        else if (criterion == KMPXMediaGeneralTitle)
            {
            HBufC* critStr = PreProcessStringLC(KCriterionCategoryName);
            HBufC* title = MPXDbCommonUtil::ProcessPatternCharsLC(
                aCriteria.ValueText(KMPXMediaGeneralTitle));
            MPXDbCommonUtil::AddSqlCriterionL(*criteriaArray, *critStr, *title);
            CleanupStack::PopAndDestroy(2, critStr); // title & Str
            }
        else if (criterion == KMPXMediaGeneralCount)
            {
            criteriaCounterSet = ETrue;
            criteriaCounter = aCriteria.ValueTObjectL<TInt>(KMPXMediaGeneralCount);
            }
        else
            {
            // ignore attribute
            }
        }

    // construct criteria string
    HBufC* criteriaStr = MPXDbCommonUtil::StringFromArrayLC(*criteriaArray, KMCAndKeyword);

    // either get all items or items filtered based on criteria
    HBufC* query = PreProcessStringLC(criteriaStr->Length() ?
        KQueryCategoryItems() : KQueryCategoryAll());
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(*query, criteriaStr));

    CleanupStack::PopAndDestroy(query);
    CleanupStack::PopAndDestroy(criteriaStr);
    CleanupStack::PopAndDestroy(criteriaArray);

    CleanupClosePushL(recordset);

    // It is possible to get multiple records with the same ID in the case where the same category
    // name is present on more than one drive (the unique ID is constructed from the name alone
    // for categories). The records have to be processed manually as the songs count cannot be
    // summed in the query.
    //
    // Unknown artist/album/genre/composer is stored in the database as NULL (name field). This
    // ensures the unknown artist/album/genre/composer to be the 1st found record if it exists.
    // This will save time in searching for the unknown record among the results and avoid
    // performing descriptor comparison.
    // If the 1st record is the unknown artist/album/genre/composer, it won't be appended to the
    // array until all other records have been put in the array.
    //
    // NOTE: putting unknown artist/album/genre/composer to the end of the array only takes place
    //       when title field is requested. normal sorting algorithm occurs if title isn't requested,
    //       i.e. client requests for Id only. The Id of the unknown record will be the 1st item in
    //       the array if it exists.
    //
    TBool firstGroup(ETrue);
    CMPXMedia* unknownMedia(NULL);
    CMPXMedia* media(NULL);
    TUint32 prevId(0);
    TUint32 currId(0);
    TUint32 accumulatedCounter(0);
    TInt err(KErrNone);

    while ((err = recordset.Next()) == KSqlAtRow)
        {
        // Setup basic info - with first record of a group
        currId = recordset.ColumnInt64(ECategoryUniqueId);

        if (currId != prevId)
            {
            // first or new set of records
            if (media)
                {
                // valid previous record set
                if (criteriaCounterSet && (criteriaCounter != accumulatedCounter))
                    {
                    // the counter criterion does not match
                    // discard the media and unknownMedia
                    CleanupStack::PopAndDestroy(media);
                    if (firstGroup && unknownMedia)
                        {
                        unknownMedia = NULL;
                        }
                    }
                else
                    {
                    if (!firstGroup || !unknownMedia)
                        {
                        // append the media to the array
                        if (counterRequested)
                            {
                            media->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, accumulatedCounter);
                            }

                        aMediaArray.AppendL(*media);
                        CleanupStack::PopAndDestroy(media);
                        }

                    firstGroup = EFalse;
                    }
                }

            // start a new media object
            media = CMPXMedia::NewL();
            CleanupStack::PushL(media);

            media->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
            media->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, iCategory);
            media->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, currId);

            if (titleRequested)
                {
                TPtrC name = MPXDbCommonUtil::GetColumnTextL(recordset, ECategoryName);
                if (firstGroup && (name.Length() == 0))
                    {
                    unknownMedia = media;
                    }
                media->SetTextValueL(KMPXMediaGeneralTitle, name);
                }

            accumulatedCounter = 0;
            prevId = currId;
            }

        accumulatedCounter += GetEpisodeCountL(KDbManagerAllDrives, currId);
        }

    // process last record
    if (media)
        {
        // valid previous record set
        if (criteriaCounterSet && (criteriaCounter != accumulatedCounter))
            {
            // the counter criterion does not match
            // discard the media and unknownMedia
            CleanupStack::PopAndDestroy(media);
            if (firstGroup && unknownMedia)
                {
                unknownMedia = NULL;
                }
            }
        else
            {
            if (!firstGroup || !unknownMedia)
                {
                // append the media to the array
                if (counterRequested)
                    {
                    media->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, accumulatedCounter);
                    }

                aMediaArray.AppendL(*media);
                CleanupStack::PopAndDestroy(media);
                }
            }
        }

    if (unknownMedia)
        {
        aMediaArray.AppendL(*unknownMedia);
        CleanupStack::PopAndDestroy(unknownMedia); // the first media that contains unknown title
        }

    CleanupStack::PopAndDestroy(&recordset);

    if (err != KSqlAtEnd)
        {
        User::LeaveIfError(err);
        }
    }

// ----------------------------------------------------------------------------
// Decrement the number of episodes in the row. If the count gets to 0, remove the row.
// ----------------------------------------------------------------------------
//
void CMPXDbCategory::DecrementEpisodesForCategoryL(
    const TUint32 aId,
    TInt aDriveId,
    CMPXMessageArray* aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbCategory::DecrementEpisodesForCategoryL");

    // if just one episode uses this category, use <= just in case
    if (GetEpisodeCountL(aDriveId, aId) <= 1)
        {
        // delete the category
        DeleteCategoryL(aId, aDriveId);

        if (aItemChangedMessages)
            {
            // add the item changed message
            MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages, aId, EMPXItemDeleted,
                iCategory, KDBPluginUid);
            }
        }
    else
        {
        // decrement the number of episodes for the category
        HBufC* query = PreProcessStringLC(KQueryCategoryDecrementEpisodeCount);
        iDbManager.ExecuteQueryL(aDriveId, *query, aId);
        CleanupStack::PopAndDestroy(query);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbCategory::DeleteCategoryL
// ----------------------------------------------------------------------------
//
void CMPXDbCategory::DeleteCategoryL(
    TUint32 aId,
    TInt aDriveId)
    {
    MPX_FUNC("CMPXDbCategory::DeleteCategoryL");

    HBufC* query = PreProcessStringLC(KQueryCategoryDelete);
    iDbManager.ExecuteQueryL(aDriveId, *query, aId);
    CleanupStack::PopAndDestroy(query);
    }

// ----------------------------------------------------------------------------
// CMPXDbCategory::GetCategoryItemL
// ----------------------------------------------------------------------------
//
void CMPXDbCategory::GetCategoryItemL(
    TUint32 aId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbCategory::GetCategoryItemL");

    HBufC* query = PreProcessStringLC(KQueryCategoryItem);
    ExecuteMediaQueryL(aAttrs, aMedia, *query, aId);
    CleanupStack::PopAndDestroy(query);
    }

// ----------------------------------------------------------------------------
// CMPXDbCategory::GetCategoryItemsL
// ----------------------------------------------------------------------------
//
void CMPXDbCategory::GetCategoryItemsL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbCategory::GetCategoryItemsL");

    // have to run one query to get all items as opposed to individual queries
    // because of the sorting

    // construct the unique ID criteria string
    // (UniqueId = %u OR UniqueId = %u ...)
    TInt itemCount(aMediaArray.Count());
    HBufC* criteria = HBufC::NewLC((2 * KCriterionCategoryUniqueId().Length() +
        KMCIntegerLen + KMCOrKeyword().Length() + 2) * itemCount);
    TPtr ptr(criteria->Des());
    ptr.Append(KMCOpenBracket);
    for (TInt index = 0; index < itemCount; ++index)
        {
        CMPXMedia* media = aMediaArray[index];

        HBufC* critStr = PreProcessStringLC(KCriterionCategoryUniqueId);
        HBufC* criterion = MPXDbCommonUtil::SqlCriterionLC(*critStr,
            media->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
        ptr.Append(*criterion);
        CleanupStack::PopAndDestroy(criterion);
        CleanupStack::PopAndDestroy(critStr);

        if (index < itemCount - 1)
            {
            ptr.Append(KMCOrKeyword);
            }
        }
    ptr.Append(KMCCloseBracket);

    // the array has to be reset as the items have to be returned in a different sort order
    aMediaArray.Reset();

    HBufC* query = PreProcessStringLC(KQueryCategoryItems);
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(*query, criteria));

    CleanupStack::PopAndDestroy(query);
    CleanupStack::PopAndDestroy(criteria);

    CleanupClosePushL(recordset);
    ProcessRecordsetL(aAttrs, recordset, aMediaArray);
    CleanupStack::PopAndDestroy(&recordset);
    }

// ----------------------------------------------------------------------------
// CMPXDbCategory::GetAllCategoryItemsL
// ----------------------------------------------------------------------------
//
void CMPXDbCategory::GetAllCategoryItemsL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbCategory::GetAllCategoryItemsL");

    HBufC* query = PreProcessStringLC(KQueryCategoryAll);
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(*query));
    CleanupStack::PopAndDestroy(query);

    CleanupClosePushL(recordset);
    ProcessRecordsetL(aAttrs, recordset, aMediaArray);
    CleanupStack::PopAndDestroy(&recordset);
    }

// ----------------------------------------------------------------------------
// CMPXDbCategory::CategoryItemExistsL
// The category records must be in the same database as the corresponding
// Podcast record, otherwise when adding a duplicate of a song on a
// different drive this method will return true and the category record won't
// be created.
// ----------------------------------------------------------------------------
//
TBool CMPXDbCategory::CategoryItemExistsL(
    TInt aDriveId,
    TUint32 aId)
    {
    MPX_FUNC("CMPXDbCategory::CategoryItemExistsL");

    HBufC* query = PreProcessStringLC(KQueryCategoryItem);
    RSqlStatement recordset(
        iDbManager.ExecuteSelectQueryL(aDriveId, *query, aId));

    TBool exists(recordset.Next() == KSqlAtRow);

    recordset.Close();
    CleanupStack::PopAndDestroy(query);

    return exists;
    }

// ----------------------------------------------------------------------------
// CMPXDbCategory::GetSongsCountL
// ----------------------------------------------------------------------------
//
TInt CMPXDbCategory::GetEpisodeCountL(
    TInt aDriveId,
    TUint32 aId)
    {
    MPX_FUNC("CMPXDbCategory::GetEpisodeCountL");

    HBufC* query = PreProcessStringLC(KQueryCategoryGetEpisodeCount);
    RSqlStatement recordset(
        iDbManager.ExecuteSelectQueryL(aDriveId, *query, aId));
    CleanupClosePushL(recordset);

    TInt err(KSqlAtEnd);
    TInt ret(0) ;
    while((err = recordset.Next()) == KSqlAtRow)
        {
        ret += recordset.ColumnInt(KMPXTableDefaultIndex);
        }
    if(err != KSqlAtEnd)
        {
        User::Leave(KErrNotFound);
        }
    MPX_DEBUG2("CMPXDbCategory::GetEpisodeCountL Count %d" , ret);
    CleanupStack::PopAndDestroy(&recordset);
    CleanupStack::PopAndDestroy(query);

    return ret;
    }

// ----------------------------------------------------------------------------
// CMPXDbCategory::UpdateMediaL
// ----------------------------------------------------------------------------
//
void CMPXDbCategory::UpdateMediaL(
    RSqlStatement& aRecord,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbCategory::UpdateMediaL");

    TInt attrCount(aAttrs.Count());
    for (TInt i = 0; i < attrCount; ++i)
        {
        TInt contentId(aAttrs[i].ContentId());
        TUint attributeId(aAttrs[i].AttributeId());

        if (contentId == KMPXMediaIdGeneral)
            {
            if (attributeId & EMPXMediaGeneralId)
                {
                aMedia.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId,
                    aRecord.ColumnInt64(ECategoryUniqueId));
                }
            if (attributeId & EMPXMediaGeneralTitle)
                {
                aMedia.SetTextValueL(KMPXMediaGeneralTitle,
                    MPXDbCommonUtil::GetColumnTextL(aRecord, ECategoryName));
                }
            if (attributeId & EMPXMediaGeneralCount)
                {
                aMedia.SetTObjectValueL<TInt>(KMPXMediaGeneralCount,
                    GetEpisodeCountL(KDbManagerAllDrives,
                    aRecord.ColumnInt64(ECategoryUniqueId)));
                }
            } // end if contentId == KMPXMediaIdGeneral
        } // end for

    aMedia.SetTObjectValueL(KMPXMediaGeneralType, EMPXItem);
    aMedia.SetTObjectValueL(KMPXMediaGeneralCategory, iCategory);
    // Fix for EXAL-79ZC9M set the right Type and Cat for each item
    if (iCategory == EMPXAlbum)
        {
        aMedia.SetTObjectValueL(KMPXMediaPodcastType, EMPXPodcastGroup);
        aMedia.SetTObjectValueL(KMPXMediaPodcastCategoryGroup, EMPXTitle);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbCategory::GetCategoryRecordL
// ----------------------------------------------------------------------------
//
RSqlStatement CMPXDbCategory::GetCategoryRecordL(
    TUint32 aId)
    {
    MPX_FUNC("CMPXDbCategory::GetCategoryRecordL");

    HBufC* query = PreProcessStringLC(KQueryCategoryItem);
    RSqlStatement statement(iDbManager.ExecuteSelectQueryL(*query, aId));
    CleanupStack::PopAndDestroy(query);

    return statement;
    }

// ----------------------------------------------------------------------------
// CMPXDbCategory::PreProcessStringLC
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbCategory::PreProcessStringLC(
    const TDesC& aQuery)
    {
    MPX_FUNC("CMPXDbCategory::PreProcessStringLC");

    HBufC* query = HBufC::NewLC(aQuery.Length() + KMaxTableNameCount *
        (iTableName->Length() + KCategoryTablePlaceholder().Length()));
    TPtr queryPtr(query->Des());

    // copy the query string
    queryPtr = aQuery;

    // replace all instances of the placeholder with the actual table name
    TInt index(0);
    while ((index = queryPtr.Find(KCategoryTablePlaceholder)) != KErrNotFound)
        {
        queryPtr.Replace(index, KCategoryTablePlaceholder().Length(), *iTableName);
        }

    return query;
    }

// ----------------------------------------------------------------------------
// CMPXDbCategory::ProcessRecordsetL
// Unknown item is stored in the database as NULL (name field). This ensures the
// unknown item to be the 1st found record if it exists. This will save time in
// searching for the unknown record among the results and avoid performing
// descriptor comparison. If the 1st record is the unknown item, it won't be
// appended to the array until all other records have been put in the array.
//
// NOTE: putting unknown item to the end of the array only takes place when title
//       field is requested. normal sorting algorithm occurs if title isn't
//       requested.
// ----------------------------------------------------------------------------
//
void CMPXDbCategory::ProcessRecordsetL(
    const TArray<TMPXAttribute>& aAttrs,
    RSqlStatement& aRecordset,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbCategory::ProcessRecordsetL");

    // populate the array
    TBool firstRecord(ETrue);
    CMPXMedia* unknownMedia(NULL);
    TInt prevId(0);
    TInt err(KErrNone);

    while ((err = aRecordset.Next()) == KSqlAtRow)
        {
        TUint32 rowId(aRecordset.ColumnInt64(ECategoryUniqueId));
        if (prevId == rowId)
            {
            continue;
            }

        prevId = rowId;
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);

        UpdateMediaL(aRecordset, aAttrs, *media);

        if (firstRecord && (MPXDbCommonUtil::GetColumnTextL(aRecordset, ECategoryName).Length() == 0))
            {
            unknownMedia = media;
            }

        if (!firstRecord || !unknownMedia)
            {
            aMediaArray.AppendL(*media);
            CleanupStack::PopAndDestroy(media);
            }

        firstRecord = EFalse;
        } // end while

    if (err != KSqlAtEnd)
        {
        User::LeaveIfError(err);
        }

    if (unknownMedia)
        {
        aMediaArray.AppendL(*unknownMedia);
        CleanupStack::PopAndDestroy(unknownMedia);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbCategory::CreateTableL
// ----------------------------------------------------------------------------
//
void CMPXDbCategory::CreateTableL(
    RSqlDatabase& aDatabase,
    TBool /* aCorruptTable */)
    {
    MPX_FUNC("CMPXDbCategory::CreateTableL");

    // create the table
    HBufC* query = PreProcessStringLC(KCategoryCreateTable);
    User::LeaveIfError(aDatabase.Exec(*query));
    CleanupStack::PopAndDestroy(query);

    // create the Name index
    query = PreProcessStringLC(KCategoryNameIndex);
    User::LeaveIfError(aDatabase.Exec(*query));
    CleanupStack::PopAndDestroy(query);
    }

// ----------------------------------------------------------------------------
// CMPXDbCategory::DropTableL
// ----------------------------------------------------------------------------
//
void CMPXDbCategory::DropTableL(
    RSqlDatabase& aDatabase)
    {
    MPX_FUNC("CMPXDbCategory::DropTableL");

    HBufC* query = PreProcessStringLC(KCategoryDropTable);
    User::LeaveIfError(aDatabase.Exec(*query));
    CleanupStack::PopAndDestroy(query);
    }

// ----------------------------------------------------------------------------
// CMPXDbCategory::CheckTableL
// ----------------------------------------------------------------------------
//
TBool CMPXDbCategory::CheckTableL(
    RSqlDatabase& aDatabase)
    {
    MPX_FUNC("CMPXDbCategory::CheckTableL");

    HBufC* query = PreProcessStringLC(KCategoryCheckTable);
    TBool check(DoCheckTable(aDatabase, *query));
    CleanupStack::PopAndDestroy(query);

    return check;
    }

// End of File
