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
* Description:  Responsible for interation with the category tables:
*                Artist, Album, Genre and Composer
*
*/


// INCLUDE FILES
#include <sqldb.h>

#include <mpxlog.h>

#include "mpxdbcommonutil.h"
#include "mpxdbcommondef.h"
#include "mpxdbmanager.h"

#include "mpxcollectiondbdef.h"
#include "mpxdbpluginqueries.h"
#include "mpxdbutil.h"
#include "mpxdbcategory.h"
#include "mpxmediamusicdefs.h"

// CONSTANTS

// maximum number of table name entries per query
const TInt KMaxTableNameCount = 2;

// ============================ MEMBER FUNCTIONS ==============================


// ----------------------------------------------------------------------------
// Destructor
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
// Second phase constructor.
// ----------------------------------------------------------------------------
//
void CMPXDbCategory::BaseConstructL()
    {
    MPX_FUNC("CMPXDbCategory::BaseConstructL");

    CMPXDbTable::BaseConstructL();
    iTableName = MPXDbUtil::TableNameForCategoryL(iCategory).AllocL();
    }

// ----------------------------------------------------------------------------
// CMPXDbCategory::AddItemL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbCategory::AddItemL(
    TMPXGeneralCategory aCategory,        
    const CMPXMedia& aMedia,
    TInt aDriveId,
    TBool& aNewRecord,
    TBool aCaseSensitive)
    {
    MPX_FUNC("CMPXDbCategory::AddItemL");

    TPtrC itemName(ItemNameL(aCategory, aMedia));
    
    // try to find the item first
    TUint32 rowId(MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), iCategory,
        itemName, aCaseSensitive));
    aNewRecord = !CategoryItemExistsL(aDriveId, rowId);

    if (aNewRecord)
        {
        // insert new
        HBufC* query = PreProcessStringLC(KQueryCategoryInsert);
        HBufC* name = MPXDbCommonUtil::ProcessSingleQuotesLC(itemName);

        iDbManager.ExecuteQueryL(aDriveId, *query, rowId, name, 1);

        CleanupStack::PopAndDestroy(name);
        CleanupStack::PopAndDestroy(query);
        }
    else
        {
        // increment the number of songs for the category
        HBufC* query = PreProcessStringLC(KQueryCategoryIncrementSongCount);
        iDbManager.ExecuteQueryL(aDriveId, *query, rowId);
        CleanupStack::PopAndDestroy(query);
        }

    return rowId;
    }
		
// ----------------------------------------------------------------------------
// CMPXDbCategory::GetNameL
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
// CMPXDbCategory::CountL
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
// CMPXDbCategory::FindAllL
// ----------------------------------------------------------------------------
//
void CMPXDbCategory::FindAllL(
    const CMPXMedia& aCriteria,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbCategory::FindAllL");

    TMPXGeneralType type = aCriteria.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType);

    const TArray<TMPXAttribute> criteria = aCriteria.Attributes();
    TInt criteriaCount(criteria.Count());

    // process the criteria and construct the criteria string
    CDesCArrayFlat* criteriaArray = new (ELeave) CDesCArrayFlat(criteriaCount + 1);
    CleanupStack::PushL(criteriaArray);

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
#ifdef RD_MPX_COLLECTION_CACHE

            if (aCriteria.ValueText(KMPXMediaGeneralTitle).Length() <= 0)
                {
                TUint32 itemId = MPXDbCommonUtil::GenerateUniqueIdL(
                                    iDbManager.Fs(), iCategory, KNullDesC, EFalse);
                HBufC* critStr = PreProcessStringLC(KCriterionCategoryUniqueId);
                MPXDbCommonUtil::AddSqlCriterionL(*criteriaArray, *critStr, itemId);
                CleanupStack::PopAndDestroy(critStr);
                }
            else
                {

#endif //RD_MPX_COLLECTION_CACHE
                HBufC* critStr = PreProcessStringLC(KCriterionCategoryName);
                HBufC* title = MPXDbCommonUtil::ProcessPatternCharsLC(
                    aCriteria.ValueText(KMPXMediaGeneralTitle));
                MPXDbCommonUtil::AddSqlCriterionL(*criteriaArray, *critStr, *title);
                CleanupStack::PopAndDestroy(2, critStr); // title & critStr
#ifdef RD_MPX_COLLECTION_CACHE
                }
#endif //RD_MPX_COLLECTION_CACHE
            }
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        else if (criterion == KMPXMediaGeneralUri)
            {
            //only Genre and  AbstractAlbum are not case sensitive
            TBool caseSensitive = ETrue;            
            if ((iCategory == EMPXGenre) || (iCategory == EMPXAbstractAlbum))
                 caseSensitive = EFalse;
                      
            TUint32 itemId(MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), iCategory,
                 aCriteria.ValueText(KMPXMediaGeneralUri), caseSensitive));   
            HBufC* critStr = PreProcessStringLC(KCriterionCategoryUniqueId);
            MPXDbCommonUtil::AddSqlCriterionL(*criteriaArray, *critStr, itemId);
            CleanupStack::PopAndDestroy(critStr);
            }
        else if (criterion == KMPXMediaGeneralDrive)
            {
            const TDesC& drive(aCriteria.ValueText(KMPXMediaGeneralDrive));
            TDriveUnit driveUnit(drive);
            MPXDbCommonUtil::AddSqlCriterionL(*criteriaArray, KCriterionAbstractAlbumVolumeId,
                MPXDbCommonUtil::GetVolIdMatchDriveIdL(iDbManager.Fs(), driveUnit));
            }
#endif // ABSTRACTAUDIOALBUM_INCLUDED
        else
            {
            // ignore attribute
            }
        }

    // construct criteria string
    HBufC* criteriaStr = NULL;
    if (criteriaArray->Count() > 0)
        {
        criteriaStr = MPXDbCommonUtil::StringFromArrayLC(*criteriaArray, KMCAndKeyword);
        }
    else
        {
        _LIT(KCriteriaTrue, "1");
        criteriaStr = KCriteriaTrue().AllocLC();
        }
        
    RSqlStatement recordset;
    if (iCategory == EMPXArtist)
        {
        recordset = iDbManager.ExecuteSelectQueryL(KQueryArtistItems, criteriaStr);
        CleanupStack::PopAndDestroy(2, criteriaArray);  //criteriaStr, criteriaArray
        }
    else 
        {
        HBufC* query = PreProcessStringLC(KQueryCategoryItems);
        recordset = iDbManager.ExecuteSelectQueryL(*query, criteriaStr);
        CleanupStack::PopAndDestroy(3, criteriaArray);  //query, criteriaStr, criteriaArray 
        }

    CleanupClosePushL(recordset);

    // process the results
    ProcessRecordsetL(aAttrs, recordset, aMediaArray);
    CleanupStack::PopAndDestroy(&recordset);
    }
        
// ----------------------------------------------------------------------------
// CMPXDbCategory::DecrementSongsForCategoryL
// ----------------------------------------------------------------------------
//
void CMPXDbCategory::DecrementSongsForCategoryL(
    const TUint32 aId,
    TInt aDriveId,
    CMPXMessageArray* aItemChangedMessages,
    TBool& aItemExist
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
    ,
    TBool /*aMTPInUse*/
#endif //ABSTRACTAUDIOALBUM_INCLUDED
)
    {
    MPX_FUNC("CMPXDbCategory::DecrementSongsForCategoryL");

    // if just one song uses this category. Use <= just in case
    if (GetSongsCountL(aDriveId, aId) <= 1)
        {
        aItemExist = EFalse;
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
        aItemExist = ETrue;
        // decrement the number of songs for the category
        HBufC* query = PreProcessStringLC(KQueryCategoryDecrementSongCount);
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
    TInt count(aMediaArray.Count());
    HBufC* criteria = HBufC::NewLC((2 * KCriterionCategoryUniqueId().Length() +
        KMCIntegerLen + KMCOrKeyword().Length() + 2) * count);
    TPtr ptr(criteria->Des());
    ptr.Append(KMCOpenBracket);
    for (TInt index = 0; index < count; ++index)
        {
        CMPXMedia* media = aMediaArray[index];

        HBufC* critStr = PreProcessStringLC(KCriterionCategoryUniqueId);
        HBufC* criterion = MPXDbCommonUtil::SqlCriterionLC(*critStr,
            (media->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)).iId1);
        ptr.Append(*criterion);
        CleanupStack::PopAndDestroy(criterion);
        CleanupStack::PopAndDestroy(critStr);

        if (index < (count - 1))
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
// CMPXDbCategory::GetSubCategoryItemsL
// ----------------------------------------------------------------------------
//
void CMPXDbCategory::GetSubCategoryItemsL(
    TMPXGeneralCategory aParentCategory,
    TUint32 aParentId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbCategory::GetSubCategoryItemsL");

    }

// ----------------------------------------------------------------------------
// CMPXDbCategory::CategoryItemExistsL
// The category records must be in the same database as the corresponding
// Music record, otherwise when adding a duplicate of a song on a
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
TInt CMPXDbCategory::GetSongsCountL(
    TInt aDriveId,
    TUint32 aId)
    {
    MPX_FUNC("CMPXDbCategory::GetSongsCountL");

    HBufC* query = PreProcessStringLC(KQueryCategoryGetSongCount);
    RSqlStatement recordset(
        iDbManager.ExecuteSelectQueryL(aDriveId, *query, aId));
    CleanupClosePushL(recordset);

    TInt err(KErrNone);
    TInt ret(0);
    while ((err = recordset.Next()) == KSqlAtRow)
        {
        ret += recordset.ColumnInt(KMPXTableDefaultIndex);
        }

    if (err != KSqlAtEnd)
        {
        User::Leave(err);
        }

    CleanupStack::PopAndDestroy(&recordset);
    CleanupStack::PopAndDestroy(query);

    return ret;
    }

void CMPXDbCategory::UpdateItemL(
    TUint32 /*aId*/,
    const CMPXMedia& /*aMedia*/,
    TInt /*aDriveId*/,
    CMPXMessageArray* /*aItemChangedMessages*/)
	{
    MPX_FUNC("CMPXDbCategory::UpdateItemL");
    
	// do nothing
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

    TInt count(aAttrs.Count());
    for (TInt i = 0; i < count; ++i)
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
                    GetSongsCountL(KDbManagerAllDrives,
                    aRecord.ColumnInt64(ECategoryUniqueId)));
                }
            } // end if contentId == KMPXMediaIdGeneral
        } // end for

    aMedia.SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    aMedia.SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, iCategory);
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

    HBufC* query = HBufC::NewLC(aQuery.Length() + KMaxTableNameCount * (iTableName->Length() +
        KCategoryTablePlaceholder().Length()));
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
// Unknown item is stored in the database as empty string (name field). This ensures the
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
    // populate the array
    TBool firstRecord(ETrue);
    CMPXMedia* unknownMedia(NULL);
    TInt prevId(0);
    TInt err(KErrNone);

    TInt pPath(0);
    if (aMediaArray.Count())
        {
        CMPXMedia* pMedia = aMediaArray[0];
        if (pMedia->IsSupported(KMPXMediaGeneralValue))
            { // Query excuted by OpenL
            pPath = pMedia->ValueTObjectL<TInt>(KMPXMediaGeneralValue);
            MPX_ASSERT(pPath);
            }
        }
    RArray<TMPXItemId> ids;
    CleanupClosePushL(ids);

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

        if (firstRecord &&
            (MPXDbCommonUtil::GetColumnTextL(aRecordset, ECategoryName).Length() == 0))
            {
            unknownMedia = media;
            }

        if (!firstRecord || !unknownMedia)
            {
            if (media->IsSupported(KMPXMediaGeneralId) && pPath)
                {
                ids.AppendL(media->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
                }
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
        if (unknownMedia->IsSupported(KMPXMediaGeneralId) && pPath)
            {
            ids.AppendL(unknownMedia->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
            }
        aMediaArray.AppendL(*unknownMedia);
        CleanupStack::PopAndDestroy(unknownMedia);
        }

    // Append ids to the returned path
    if (pPath)
        {
        ((CMPXCollectionPath*)pPath)->AppendL(ids.Array());
        }
    CleanupStack::PopAndDestroy(&ids);
    }

// ----------------------------------------------------------------------------
// CMPXDbCategory::ItemNameL
// ----------------------------------------------------------------------------
//
TPtrC CMPXDbCategory::ItemNameL(
    TMPXGeneralCategory aCategory,        
    const CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbCategory::ItemNameL");
    
    TMPXAttribute attribute;
    switch(aCategory)
        {
        case EMPXGenre:
            {
            attribute = KMPXMediaMusicGenre;
            break;
            }
        case EMPXComposer:
            {
            attribute = KMPXMediaMusicComposer;
            break;
            }
        default:
            {
            User::Leave(KErrArgument);
            break;
            }
        }
    
    if (aMedia.IsSupported(attribute))
        {
        return aMedia.ValueText(attribute).Left(KMCMaxTextLen);
        }
    else
        {
        return KNullDesC();
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

    // do not create an index on the Name field
    // as it only slows down the insert/update queries overall
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
