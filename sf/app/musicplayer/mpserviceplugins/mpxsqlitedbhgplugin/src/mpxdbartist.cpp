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
* Description:  Responsible for interation with the Artist table
*
*
*/


// INCLUDE FILES
#include <sqldb.h>

#include <mpxlog.h>

#include "mpxdbcommonutil.h"
#include "mpxdbcommondef.h"
#include "mpxmediamusicdefs.h"
#include "mpxdbmanager.h"

#include "mpxcollectiondbdef.h"
#include "mpxdbpluginqueries.h"
#include "mpxdbutil.h"
#include "mpxdbartist.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbArtist* CMPXDbArtist::NewL(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory)
    {
    MPX_FUNC("CMPXDbArtist::NewL");

    CMPXDbArtist* self = CMPXDbArtist::NewLC(aDbManager, aCategory);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbArtist* CMPXDbArtist::NewLC(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory)
    {
    MPX_FUNC("CMPXDbArtist::NewLC");

    CMPXDbArtist* self = new (ELeave) CMPXDbArtist(aDbManager, aCategory);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXDbArtist::~CMPXDbArtist()
    {
    MPX_FUNC("CMPXDbArtist::~CMPXDbArtist");
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CMPXDbArtist::CMPXDbArtist(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory) :
    CMPXDbCategory(aDbManager, aCategory)
    {
    MPX_FUNC("CMPXDbArtist::CMPXDbArtist");
    }

// ----------------------------------------------------------------------------
// Second phase constructor.
// ----------------------------------------------------------------------------
//
void CMPXDbArtist::ConstructL()
    {
    MPX_FUNC("CMPXDbArtist::ConstructL");

    BaseConstructL();
    }

// ----------------------------------------------------------------------------
// CMPXDbArtist::AddItemL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbArtist::AddItemL(
    TMPXGeneralCategory /*aCategory*/,        
    const CMPXMedia& aMedia,
    TInt aDriveId,
    TBool& aNewRecord,
    TBool aCaseSensitive)
    {
    MPX_FUNC("CMPXDbArtist::AddItemL");

    TPtrC artist(KNullDesC);
    TPtrC albumArt(KNullDesC);
    
    if (aMedia.IsSupported(KMPXMediaMusicArtist))
        {
        artist.Set(aMedia.ValueText(KMPXMediaMusicArtist).Left(KMCMaxTextLen));
        }
    
    if (aMedia.IsSupported(KMPXMediaMusicAlbumArtFileName))
        {
        albumArt.Set(aMedia.ValueText(KMPXMediaMusicAlbumArtFileName).Left(KMCMaxTextLen));
        }
       
    // try to find the item first
    TUint32 rowId(MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), iCategory,
        artist, aCaseSensitive));

    HBufC* query = PreProcessStringLC(KQueryCategoryItem);
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(aDriveId, *query, rowId));
    CleanupStack::PopAndDestroy(query);
    CleanupClosePushL(recordset);
    
    TInt result(recordset.Next());
    if (result == KSqlAtEnd)
        {
        aNewRecord = result;
        
        // insert new
        HBufC* name = MPXDbCommonUtil::ProcessSingleQuotesLC(artist);
        HBufC* art = MPXDbCommonUtil::ProcessSingleQuotesLC(albumArt);

        iDbManager.ExecuteQueryL(aDriveId, KQueryArtistInsert, rowId, name, 1, 0, art);

        CleanupStack::PopAndDestroy(art);
        CleanupStack::PopAndDestroy(name);
        }
    else if (result == KSqlAtRow)
        {
        // retrieve Art from Artist table
        TPtrC art(KNullDesC);
        art.Set(MPXDbCommonUtil::GetColumnTextL(recordset, EArtistArt));

        // the current one is Unknown and the new one is Not Unknown
        if ( art == KNullDesC && albumArt != KNullDesC )
            {
            HBufC* artReplaceSingleQuote = 
                MPXDbCommonUtil::ProcessSingleQuotesLC( albumArt );
            _LIT( KFormatArt, "Art=\'%S\'" );
            HBufC* setStr = HBufC::NewLC(KMaxFileName);
            setStr->Des().Format( KFormatArt, artReplaceSingleQuote );

            iDbManager.ExecuteQueryL(aDriveId, KQueryArtistUpdate, setStr, rowId);
            CleanupStack::PopAndDestroy(setStr);
            CleanupStack::PopAndDestroy(artReplaceSingleQuote);
            }

        // increment the number of songs for the category
        query = PreProcessStringLC(KQueryCategoryIncrementSongCount);
        iDbManager.ExecuteQueryL(aDriveId, *query, rowId);
        CleanupStack::PopAndDestroy(query);
        }
    else
        {
        MPX_DEBUG2("SQL error %d", result);
        User::Leave(result);
        }
    
    CleanupStack::PopAndDestroy(&recordset);
    
    return rowId;
    }
    
// ----------------------------------------------------------------------------
// CMPXDbArtist::DecrementSongsForCategoryL
// ----------------------------------------------------------------------------
//
void CMPXDbArtist::DecrementSongsForCategoryL(
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
    MPX_FUNC("CMPXDbArtist::DecrementSongsForCategoryL");
    
    TInt songsCount(0);
    TInt albumsCount(0);
    
    GetSongsAndAlbumsCountL(aDriveId, aId, songsCount, albumsCount); 
            
    MPX_DEBUG4("id %d, songsCount %d, albumsCount %d", aId, songsCount, albumsCount);
    
    // if just one song uses this category. Use <= just in case
    if (songsCount <= 1 && albumsCount == 0)  
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
    else if (songsCount > 0)
        {
        aItemExist = ETrue;
        // decrement the number of songs for the category
        HBufC* query = PreProcessStringLC(KQueryCategoryDecrementSongCount);
        iDbManager.ExecuteQueryL(aDriveId, *query, aId);
        CleanupStack::PopAndDestroy(query);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbArtist::UpdateMediaL
// ----------------------------------------------------------------------------
//
void CMPXDbArtist::UpdateMediaL(
    RSqlStatement& aRecord,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbArtist::UpdateMediaL");

    TInt count(aAttrs.Count());
    for (TInt i = 0; i < count; ++i)
        {
        TInt contentId(aAttrs[i].ContentId());
        TUint attributeId(aAttrs[i].AttributeId());

        if (contentId == KMPXMediaIdGeneral)
            {
            if (attributeId & EMPXMediaGeneralId)
                {
                MPX_DEBUG1("	EMPXMediaGeneralId");

                aMedia.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId,
                    aRecord.ColumnInt64(EArtistUniqueId));
				MPX_DEBUG2("	Id[%d]", aRecord.ColumnInt64(EArtistUniqueId));
                }
            if (attributeId & EMPXMediaGeneralTitle)
                {
                MPX_DEBUG1("	EMPXMediaGeneralTitle");

                TPtrC artist( MPXDbCommonUtil::GetColumnTextL(aRecord, EArtistName) );
                aMedia.SetTextValueL(KMPXMediaGeneralTitle,
                    MPXDbCommonUtil::GetColumnTextL(aRecord, EArtistName));

				MPX_DEBUG2("	Artist[%S]", &artist);
                }
            if (attributeId & EMPXMediaGeneralCount)
                {
                // get songs count from all drives
                TInt songCount = GetSongsCountL( KDbManagerAllDrives,
                                    aRecord.ColumnInt64( EArtistUniqueId ) );
				aMedia.SetTObjectValueL<TInt>(KMPXMediaGeneralCount, songCount); // mod by anjokela

                MPX_DEBUG1("	EMPXMediaGeneralCount");
				MPX_DEBUG2("	SongCount[%d]", songCount);
                }
            } // end if contentId == KMPXMediaIdGeneral
        else if ( contentId == KMPXMediaIdMusic )
            {
            if (attributeId & EMPXMediaMusicAlbumArtFileName)
                {
                MPX_DEBUG1("    EMPXMediaMusicAlbumArtFileName");
                TPtrC art(KNullDesC);
                art.Set(MPXDbCommonUtil::GetColumnTextL(aRecord, EArtistArt));
                aMedia.SetTextValueL(KMPXMediaMusicAlbumArtFileName, art);
                MPX_DEBUG2("    Art[%S]", &art);
                }
            }
		} // end for

    aMedia.SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    aMedia.SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, iCategory);
    }

// ----------------------------------------------------------------------------
// CMPXDbArtist::GenerateArtistFieldsValuesL
// ----------------------------------------------------------------------------
//
void CMPXDbArtist::GenerateArtistFieldsValuesL(const CMPXMedia& aMedia, CDesCArray& aFields, CDesCArray& aValues)
    {
    if (aMedia.IsSupported(KMPXMediaMusicAlbumArtFileName))
        {
        const TDesC& albumArtFilename = aMedia.ValueText(KMPXMediaMusicAlbumArtFileName).Left(KMCMaxTextLen);
        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicArt, albumArtFilename);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbArtist::GetAllCategoryItemsL
// ----------------------------------------------------------------------------
//
void CMPXDbArtist::GetAllCategoryItemsL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbArtist::GetAllCategoryItemsL");
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryArtistAll()));
    CleanupClosePushL(recordset);
    ProcessRecordsetL(aAttrs, recordset, aMediaArray);
    CleanupStack::PopAndDestroy(&recordset);
    }

// ----------------------------------------------------------------------------
// CMPXDbArtist::UpdateItemL
// ----------------------------------------------------------------------------
//
void CMPXDbArtist::UpdateItemL(
    TUint32 aId,
    const CMPXMedia& aMedia,
    TInt aDriveId,
    CMPXMessageArray* aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbAlbum::UpdateItemL");

    CDesCArrayFlat* fields = new (ELeave) CDesCArrayFlat(EArtistFieldCount);
    CleanupStack::PushL(fields);
    CDesCArrayFlat* values = new (ELeave) CDesCArrayFlat(EArtistFieldCount);
    CleanupStack::PushL(values);

    // process the media parameter and construct the fields and values array
    GenerateArtistFieldsValuesL(aMedia, *fields, *values);

    // construct the SET string
    HBufC* setStr = MPXDbCommonUtil::StringFromArraysLC(*fields, *values, KMCEqualSign, KMCCommaSign);

    if (setStr->Length())
        {
        // execute the query
        iDbManager.ExecuteQueryL(aDriveId, KQueryArtistUpdate, setStr, aId);
        MPXDbCommonUtil::AddItemAlbumChangedMessageL(*aItemChangedMessages, aId, EMPXItemModified,
        		EMPXArtist, KDBPluginUid, ETrue, 0 );          
        }

    CleanupStack::PopAndDestroy(setStr);
    CleanupStack::PopAndDestroy(values);
    CleanupStack::PopAndDestroy(fields);
    }

// ----------------------------------------------------------------------------
// CMPXDbArtist::AddAlbumArtistL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbArtist::AddAlbumArtistL(
    const TDesC& aName,
    const TDesC& aArt,
    TInt aDriveId)
    {
    MPX_FUNC("CMPXDbArtist::AddAlbumArtistL");

    // try to find the item first
    TUint32 rowId(MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), iCategory,
        aName, ETrue));
    HBufC* query = PreProcessStringLC(KQueryCategoryItem);
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(aDriveId, *query, rowId));
    CleanupStack::PopAndDestroy(query);
    CleanupClosePushL(recordset);
    
    TInt result(recordset.Next());
    if (result == KSqlAtEnd)
        {
        // insert new
        HBufC* name = MPXDbCommonUtil::ProcessSingleQuotesLC(aName);
        HBufC* art = MPXDbCommonUtil::ProcessSingleQuotesLC(aArt);

        iDbManager.ExecuteQueryL(aDriveId, KQueryArtistInsert, rowId, name, 0, 1, art);

        CleanupStack::PopAndDestroy(art);
        CleanupStack::PopAndDestroy(name);
        }
    else if (result == KSqlAtRow)
        {
        // retrieve Art from Artist table
        TPtrC art(KNullDesC);
        art.Set(MPXDbCommonUtil::GetColumnTextL(recordset, EArtistArt));

        // the current one is Unknown and the new one is Not Unknown
        if ( art == KNullDesC && aArt != KNullDesC )
            {
            HBufC* artReplaceSingleQuote = 
                MPXDbCommonUtil::ProcessSingleQuotesLC( aArt );
            _LIT( KFormatArt, "Art=\'%S\'" );
            HBufC* setStr = HBufC::NewLC(KMaxFileName);
            setStr->Des().Format( KFormatArt, artReplaceSingleQuote );

            iDbManager.ExecuteQueryL(aDriveId, KQueryArtistUpdate, setStr, rowId);
            CleanupStack::PopAndDestroy(setStr);
            CleanupStack::PopAndDestroy(artReplaceSingleQuote);
            }

        // increment the number of songs for the category
        iDbManager.ExecuteQueryL(aDriveId, KQueryArtistIncrementAlbumCount, rowId);
        }
    else
        {
        MPX_DEBUG2("SQL error %d", result);
        User::Leave(result);
        }

    CleanupStack::PopAndDestroy(&recordset);
    
    return rowId;
    }

// ----------------------------------------------------------------------------
// CMPXDbArtist::DecrementAlbumsForArtistL
// ----------------------------------------------------------------------------
//
void CMPXDbArtist::DecrementAlbumsForArtistL(
    const TUint32 aId,
    TInt aDriveId,
    CMPXMessageArray* aItemChangedMessages
    )
    {
    MPX_FUNC("CMPXDbArtist::DecrementAlbumsForCategoryL");
    
    TInt songsCount(0);
    TInt albumsCount(0);
    
    GetSongsAndAlbumsCountL(aDriveId, aId, songsCount, albumsCount);
    
    MPX_DEBUG4("id %d, songsCount %d, albumsCount %d", aId, songsCount, albumsCount);
    
    // if just one album uses this category. Use <= just in case
    if (songsCount == 0 && albumsCount <= 1)
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
    else if (albumsCount > 0)
        {
        // decrement the number of albums for the category
        iDbManager.ExecuteQueryL(aDriveId, KQueryArtistDecrementAlbumCount, aId);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbArtist::GetSongsAndAlbumsCountL
// ----------------------------------------------------------------------------
//
void CMPXDbArtist::GetSongsAndAlbumsCountL(
    TInt aDriveId,
    TUint32 aId, 
    TInt& aSongCount,
    TInt& aAlbumCount)
    {
    MPX_FUNC("CMPXDbArtist::GetSongsAndAlbumsCountL");

    RSqlStatement recordset(
        iDbManager.ExecuteSelectQueryL(aDriveId, KQueryArtistGetSongCountAndAlbumCount, aId));
    CleanupClosePushL(recordset);

    TInt err(KErrNone);
    while ((err = recordset.Next()) == KSqlAtRow)
        {
        aSongCount += recordset.ColumnInt(KMPXTableDefaultIndex);
        aAlbumCount += recordset.ColumnInt(KMPXTableDefaultIndex+1);
        }

    if (err != KSqlAtEnd)
        {
        User::Leave(err);
        }

    CleanupStack::PopAndDestroy(&recordset);
    }

// ----------------------------------------------------------------------------
// CMPXDbArtist::CreateTableL
// ----------------------------------------------------------------------------
//
void CMPXDbArtist::CreateTableL(
    RSqlDatabase& aDatabase,
    TBool /* aCorruptTable */)
    {
    MPX_FUNC("CMPXDbArtist::CreateTableL");

    // create the table
    User::LeaveIfError(aDatabase.Exec(KArtistCreateTable));

    // do not create an index on the Name field
    // as it only slows down the insert/update queries overall
    }

// ----------------------------------------------------------------------------
// CMPXDbArtist::CheckTableL
// ----------------------------------------------------------------------------
//
TBool CMPXDbArtist::CheckTableL(
    RSqlDatabase& aDatabase)
    {
    MPX_FUNC("CMPXDbArtist::CheckTableL");

    TBool check(DoCheckTable(aDatabase, KArtistCheckTable));

    return check;
    }

// End of File
