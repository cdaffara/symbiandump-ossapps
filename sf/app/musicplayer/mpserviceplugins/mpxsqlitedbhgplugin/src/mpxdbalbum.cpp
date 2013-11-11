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
#include "mpxmediamusicdefs.h"
#include "mpxdbpluginqueries.h"
#include "mpxdbutil.h"
#include "mpxdbalbum.h"

// CONSTANTS
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
_LIT( KAbstractAlbumExt, ".alb" );
#endif // ABSTRACTAUDIOALBUM_INCLUDED

_LIT( KAlbumAlbumArtistSeperator, "\t1");
_LIT( KAlbumArtistSeperator, "\t2");

const TInt KUnknownAlbumsGranularity = 250;

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbAlbum* CMPXDbAlbum::NewL(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory,
    MMPXDbAlbumObserver& aObserver)
    {
    MPX_FUNC("CMPXDbAlbum::NewL");

    CMPXDbAlbum* self = CMPXDbAlbum::NewLC(aDbManager, aCategory, aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbAlbum* CMPXDbAlbum::NewLC(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory,
    MMPXDbAlbumObserver& aObserver)
    {
    MPX_FUNC("CMPXDbAlbum::NewLC");

    CMPXDbAlbum* self = new (ELeave) CMPXDbAlbum(aDbManager, aCategory, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXDbAlbum::~CMPXDbAlbum()
    {
    MPX_FUNC("CMPXDbAlbum::~CMPXDbAlbum");
    
    iUnknownArtists.ResetAndDestroy();
    iUnknownAlbums.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CMPXDbAlbum::CMPXDbAlbum(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory,
    MMPXDbAlbumObserver& aObserver) :
    CMPXDbCategory(aDbManager, aCategory),
    iObserver(aObserver),
    iUnknownArtists(KUnknownAlbumsGranularity),
    iUnknownAlbums(KUnknownAlbumsGranularity)
    {
    MPX_FUNC("CMPXDbAlbum::CMPXDbAlbum");
    }

// ----------------------------------------------------------------------------
// Second phase constructor.
// ----------------------------------------------------------------------------
//
void CMPXDbAlbum::ConstructL()
    {
    MPX_FUNC("CMPXDbAlbum::ConstructL");

    BaseConstructL();
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::AddItemL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbAlbum::AddItemL(
    TMPXGeneralCategory /*aCategory*/,        
    const CMPXMedia& aMedia,
    TInt aDriveId,
    TBool& aNewRecord,
    TBool aCaseSensitive)
    {
    MPX_FUNC("CMPXDbAlbum::AddItemL");

    TPtrC album(KNullDesC);
    TPtrC albumArtist(KNullDesC);
    TPtrC artist(KNullDesC);
    TPtrC albumArt(KNullDesC);
    
    // try to find the item first
    TUint32 rowId(GenerateUniqueIdL(aMedia));
    
    // retrieve the existing record
    HBufC* query = PreProcessStringLC(KQueryCategoryItem);
    RSqlStatement recordset(
        iDbManager.ExecuteSelectQueryL(aDriveId, *query, rowId));
    CleanupStack::PopAndDestroy(query);
    CleanupClosePushL(recordset);
    
    TInt result = recordset.Next();   
    if (result == KSqlAtEnd)
        {
        aNewRecord = result;
        
        // check Album attribute
        if (aMedia.IsSupported(KMPXMediaMusicAlbum))     
            {
            album.Set(aMedia.ValueText(KMPXMediaMusicAlbum).Left(KMCMaxTextLen));
            }
        
        // check AlbumArt attirbute
        if (aMedia.IsSupported(KMPXMediaMusicAlbumArtFileName))
            {
            albumArt.Set(aMedia.ValueText(KMPXMediaMusicAlbumArtFileName).Left(KMCMaxTextLen));
            }

        // insert new
        HBufC* name = MPXDbCommonUtil::ProcessSingleQuotesLC(album);
        TUint32 artistId(0);
        HBufC* artistName = NULL;        
        if (RetrieveAlbumArtistL(aMedia, albumArtist))
            {
            artistId = iObserver.AddAlbumArtistL(albumArtist, albumArt, aDriveId);
            artistName = MPXDbCommonUtil::ProcessSingleQuotesLC(albumArtist);
            }
        else
            {
            RetrieveArtist(aMedia, artist);
            artistId = iObserver.AddAlbumArtistL(artist, albumArt, aDriveId);
            artistName = MPXDbCommonUtil::ProcessSingleQuotesLC(artist);
            }
        
        HBufC* art = MPXDbCommonUtil::ProcessSingleQuotesLC(albumArt);
                
        iDbManager.ExecuteQueryL(aDriveId, KQueryAlbumInsert, rowId, name, 1, artistId, artistName,art); 

        CleanupStack::PopAndDestroy(art);
        CleanupStack::PopAndDestroy(artistName);
        CleanupStack::PopAndDestroy(name);
        }
    else if (result == KSqlAtRow)
        {       
        // retrieve Art from Album table
        TPtrC art(KNullDesC);
        art.Set(MPXDbCommonUtil::GetColumnTextL(recordset, EAlbumArt));

        // the current one is Unknown and the new one is Not Unknown
        if ( art == KNullDesC && albumArt != KNullDesC )
            {
            HBufC* artReplaceSingleQuote = 
                MPXDbCommonUtil::ProcessSingleQuotesLC( albumArt );

            TPtrC criteria(KCriterionArt);
            HBufC* setStr = HBufC::NewLC(criteria.Length() + artReplaceSingleQuote->Length()); 
            setStr->Des().Format( KCriterionArt, artReplaceSingleQuote );

            iDbManager.ExecuteQueryL(aDriveId, KQueryAlbumUpdate, setStr, rowId);
            CleanupStack::PopAndDestroy(setStr);
            CleanupStack::PopAndDestroy(artReplaceSingleQuote);
            }

        // increment the number of songs for the category
        query = PreProcessStringLC(KQueryCategoryIncrementSongCount);
        iDbManager.ExecuteQueryL(aDriveId, *query, rowId);
        CleanupStack::PopAndDestroy(query);
        } // existing record
    else
        {
        MPX_DEBUG2("SQL error %d", result);
        User::Leave(result);
        }

    CleanupStack::PopAndDestroy(&recordset);
    
    return rowId;
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::DecrementSongsForAlbumL
// ----------------------------------------------------------------------------
//
void CMPXDbAlbum::DecrementSongsForAlbumL(
    const TUint32 aId,
    TInt aDriveId,
    CMPXMessageArray* aItemChangedMessages,
    TBool& aItemExist,
    const TDesC& aArt)       
    {
    MPX_FUNC("CMPXDbAlbum::DecrementSongsForAlbumL");

    // retrieve the existing record
    HBufC* query = PreProcessStringLC(KQueryCategoryItem);
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(*query, aId));
    CleanupStack::PopAndDestroy(query);

    CleanupClosePushL(recordset);

    if (recordset.Next() != KSqlAtRow)
        {
        User::Leave(KErrNotFound);
        }

    // if just one song uses this category. Use <= just in case
    if (recordset.ColumnInt64(EAlbumSongCount) <= 1)
        {
        aItemExist = EFalse;

        // Decrement albums count on artist       
        TUint32 albumArtistId = recordset.ColumnInt64(EAlbumArtist);
        iObserver.DeleteAlbumForArtistL(albumArtistId, aDriveId, aItemChangedMessages);
        CleanupStack::PopAndDestroy(&recordset);
                
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

        TBool itemModified = EFalse;
        		
        TPtrC art(MPXDbCommonUtil::GetColumnTextL(recordset, EAlbumArt));        
        if (NeedToUpdateArt(aArt, art))
            {
            MPX_DEBUG2("AlbumArt of the Song to be deleted is [%S]", &aArt);
            
            //need to get alternative art in the same album to display
            HBufC* newArt = AlbumartForAlbumL(aId, aArt);
            CleanupStack::PushL(newArt);

            //update Album table only if alternative albumart found
            if (newArt)
                {
                MPX_DEBUG1("CMPXDbAlbum::DecrementSongsForAlbumL, get newArt");
                HBufC* artReplaceSingleQuote = MPXDbCommonUtil::ProcessSingleQuotesLC( *newArt );
                TPtrC criteria(KCriterionArt);   
                HBufC* setStr = HBufC::NewLC(criteria.Length() + artReplaceSingleQuote->Length()); 
                setStr->Des().Format( KCriterionArt, artReplaceSingleQuote );

                iDbManager.ExecuteQueryL(aDriveId, KQueryAlbumUpdate, setStr, aId);
                CleanupStack::PopAndDestroy(setStr);
                CleanupStack::PopAndDestroy(artReplaceSingleQuote);
                itemModified = ETrue;                     
                }
            CleanupStack::PopAndDestroy(newArt);             
            }

        if (aItemChangedMessages && itemModified)
            {
            // add the item changed message
            MPXDbCommonUtil::AddItemAlbumChangedMessageL(*aItemChangedMessages, aId, EMPXItemModified,
                EMPXAlbum, KDBPluginUid, ETrue, 0 );
            }
        CleanupStack::PopAndDestroy(&recordset);
        
        // decrement the number of songs for the category
        query = PreProcessStringLC(KQueryCategoryDecrementSongCount);
        iDbManager.ExecuteQueryL(aDriveId, *query, aId);
        CleanupStack::PopAndDestroy(query);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::GetAllCategoryItemsL
// ----------------------------------------------------------------------------
//
void CMPXDbAlbum::GetAllCategoryItemsL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbAlbum::GetAllCategoryItemsL");

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryAlbumAll));

    CleanupClosePushL(recordset);
    TRAPD(err, ProcessAlbumRecordSetL(aAttrs, recordset, aMediaArray));
    if (err != KErrNone)
        {
        iUnknownArtists.ResetAndDestroy();
        iUnknownAlbums.ResetAndDestroy();

        User::LeaveIfError(err);
        }
    CleanupStack::PopAndDestroy(&recordset);
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::GetAllCategoryItemsMediaWallL
// ----------------------------------------------------------------------------
//
void CMPXDbAlbum::GetAllCategoryItemsMediaWallL(
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbAlbum::GetAllCategoryItemsL");

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryAlbumAllMediaWall));

    CleanupClosePushL(recordset);
    TRAPD(err, ProcessMediaWallAlbumRecordSetL(aAttrs, recordset, aMediaArray));
    if (err != KErrNone)
        {
        iUnknownArtists.ResetAndDestroy();
        iUnknownAlbums.ResetAndDestroy();

        User::LeaveIfError(err);
        }
    CleanupStack::PopAndDestroy(&recordset);
    }

// ----------------------------------------------------------------------------
// CMPXDbCategory::GetSubCategoryItemsL
// ----------------------------------------------------------------------------
//
void CMPXDbAlbum::GetSubCategoryItemsL(
    TMPXGeneralCategory aParentCategory,
    TUint32 aParentId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbCategory::GetSubCategoryItemsL");

    // this is only valid for albums belonging to an artist
    ASSERT((iCategory == EMPXAlbum) && (aParentCategory == EMPXArtist));

    // to handle the UREL warning
    (void)aParentCategory;

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryMusicGetAlbumsforArtist, aParentId));
    CleanupClosePushL(recordset);
    ProcessAlbumRecordSetL(aAttrs, recordset, aMediaArray);
    CleanupStack::PopAndDestroy(&recordset);
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::UpdateItemL
// ----------------------------------------------------------------------------
//
void CMPXDbAlbum::UpdateItemL(
    TUint32 aId,
    const CMPXMedia& aMedia,
    TInt aDriveId,
    CMPXMessageArray* aItemChangedMessages)
	{
	MPX_FUNC("CMPXDbAlbum::UpdateItemL");

	CDesCArrayFlat* fields = new (ELeave) CDesCArrayFlat(EAlbumFieldCount);
	CleanupStack::PushL(fields);
	CDesCArrayFlat* values = new (ELeave) CDesCArrayFlat(EAlbumFieldCount);
	CleanupStack::PushL(values);

	// process the media parameter and construct the fields and values array
	GenerateAlbumFieldsValuesL(aMedia, *fields, *values);

	// construct the SET string
	HBufC* setStr = MPXDbCommonUtil::StringFromArraysLC(*fields, *values, KMCEqualSign, KMCCommaSign);

    if (setStr->Length())
        {
        // execute the query
        iDbManager.ExecuteQueryL(aDriveId, KQueryAlbumUpdate, setStr, aId);
        MPXDbCommonUtil::AddItemAlbumChangedMessageL(*aItemChangedMessages, aId, EMPXItemModified,
        		EMPXAlbum, KDBPluginUid, ETrue, 0 );    
        }

	CleanupStack::PopAndDestroy(setStr);
	CleanupStack::PopAndDestroy(values);
	CleanupStack::PopAndDestroy(fields);
	}

// ----------------------------------------------------------------------------
// CMPXDbAlbum::GetSongsCountInAlbumMatchingArtistL
// ----------------------------------------------------------------------------
//
TInt CMPXDbAlbum::GetSongsCountInAlbumMatchingArtistL(TUint32 aArtistId, TUint32 aAlbumId)
	{
	MPX_FUNC("CMPXDbAlbum::GetSongsCountInAlbumMatchingArtistL");

	return ExecuteSumQueryL(KQuerySongsInArtistAlbum, aArtistId, aAlbumId);
	}

// ----------------------------------------------------------------------------
// CMPXDbAlbum::UpdateMediaL
// ----------------------------------------------------------------------------
//
void CMPXDbAlbum::UpdateMediaL(
    RSqlStatement& aRecord,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbAlbum::UpdateMediaL");

    TInt count(aAttrs.Count());
    for (TInt i = 0; i < count; ++i)
        {
        TInt contentId(aAttrs[i].ContentId());
        TUint attributeId(aAttrs[i].AttributeId());

        if (contentId == KMPXMediaIdGeneral)
            {
            if (attributeId & EMPXMediaGeneralId)
                {
                MPX_DEBUG1("    EMPXMediaGeneralId");

                aMedia.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId,
                    aRecord.ColumnInt64(EAlbumUniqueId));
                }
            if (attributeId & EMPXMediaGeneralTitle)
                {
                MPX_DEBUG1("    EMPXMediaGeneralTitle");

                TPtrC album( MPXDbCommonUtil::GetColumnTextL(aRecord, EAlbumName) );
                aMedia.SetTextValueL(KMPXMediaGeneralTitle,
                    MPXDbCommonUtil::GetColumnTextL(aRecord, EAlbumName));
                MPX_DEBUG2("	Album[%S]", &album );
                }
            if (attributeId & EMPXMediaGeneralCount)
                {
                MPX_DEBUG1("    EMPXMediaGeneralCount");

				TInt songCount = GetSongsCountL(KDbManagerAllDrives,
					aRecord.ColumnInt64(EAlbumUniqueId));
                aMedia.SetTObjectValueL<TInt>(KMPXMediaGeneralCount, songCount );
				MPX_DEBUG2("	SongCount[%d]", songCount );
                }
            } // end if contentId == KMPXMediaIdGeneral
		else if ( contentId == KMPXMediaIdMusic )
			{
			if (attributeId & EMPXMediaMusicArtist)
				{
				MPX_DEBUG1("	EMPXMediaMusicArtist");

				TPtrC artistName(MPXDbCommonUtil::GetColumnTextL(aRecord, EAlbumArtistName));
				aMedia.SetTextValueL(KMPXMediaMusicArtist, artistName);
				MPX_DEBUG2("	Artist[%S]", &artistName);
				}

			if (attributeId & EMPXMediaMusicAlbum)
				{
				MPX_DEBUG1("	EMPXMediaMusicAlbum");

				TPtrC albumName(KNullDesC);
                TPtrC album( MPXDbCommonUtil::GetColumnTextL(aRecord, EAlbumName) );


				// if album is unknown
				if ( album != KNullDesC)
					{
					albumName.Set(album);
					}

				aMedia.SetTextValueL(KMPXMediaMusicAlbum, albumName);
				MPX_DEBUG2("	Album[%S]", &albumName);
				}
			if (attributeId & EMPXMediaMusicAlbumArtFileName)
				{
				MPX_DEBUG1("	EMPXMediaMusicAlbumArtFileName");

				TPtrC art(MPXDbCommonUtil::GetColumnTextL(aRecord, EAlbumArt));
				aMedia.SetTextValueL(KMPXMediaMusicAlbumArtFileName, art);
				MPX_DEBUG2("	Art[%S]", &art);
				}
			}
		} // end for

    aMedia.SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    aMedia.SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, iCategory);
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::GenerateAlbumFieldsValuesL
// ----------------------------------------------------------------------------
//
void CMPXDbAlbum::GenerateAlbumFieldsValuesL(const CMPXMedia& aMedia, CDesCArray& aFields, CDesCArray& aValues)
	{
	if (aMedia.IsSupported(KMPXMediaMusicAlbumArtFileName))
		{
		const TDesC& albumArtFilename = aMedia.ValueText(KMPXMediaMusicAlbumArtFileName).Left(KMCMaxTextLen);
		MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicArt, albumArtFilename);
		}	
	}

// ----------------------------------------------------------------------------
// CMPXDbAlbum::AlbumartForAlbumL
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbAlbum::AlbumartForAlbumL(const TUint32 aId, TPtrC aArt)
    {
    return iObserver.HandleAlbumartForAlbumL(aId, aArt);
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::ProcessAlbumRecordSetL
// Unknown album or Unknown artist are stored in the database as NULL (Name field and ArtistName field). 
// The available unknown artist is saved for later and will be appended to the array 
// as the last item among items which have the same artist.
// The available unknown albums are saved for later and will be appended to the array
// as the last items after all known albums are appended.
// The unknown album and unknown artist is saved for later and will be appended to the array
// as the last item.
//
// NOTE: putting unknown album to the end of the array only takes place when title
//       field is requested. normal sorting algorithm occurs if title isn't
//       requested.
//
// Example:
//     Album X of artist A
//     Album X of artist B
//     Album X of unknown artist
//     Album Y of artist A
//     Album Y of unknown artist
//     Unknown album of artist A
//     Unknown album of artist B
//     Unknown album of unknown artist
// ----------------------------------------------------------------------------
//
void CMPXDbAlbum::ProcessAlbumRecordSetL(
    const TArray<TMPXAttribute>& aAttrs,
    RSqlStatement& aRecordset,
    CMPXMediaArray& aMediaArray)
    {
    // populate the array
    TBool firstRecord(ETrue);
    CMPXMedia* unknownAlbumUnknownArtistMedia(NULL);
    CMPXMedia* unknownMedia(NULL);
    TUint32 prevId(0);
    TUint32 albumId(0);
    TUint32 prevAlbumId(0);
    TInt count(0);
    TInt err(KErrNone);
       
    // Reset array for unknown artists
    if (iUnknownArtists.Count() > 0)
        {
        iUnknownArtists.ResetAndDestroy();
        }
    
    // Reset array for unknown albums
    if (iUnknownAlbums.Count() > 0)
        {
        iUnknownAlbums.ResetAndDestroy();
        }
        
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
        TUint32 rowId(aRecordset.ColumnInt64(EAlbumUniqueId));
        if (prevId == rowId)
            {
            continue;
            }

        prevId = rowId;
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);

        UpdateMediaL(aRecordset, aAttrs, *media);
        
        albumId = MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), EMPXAlbum,
                MPXDbCommonUtil::GetColumnTextL(aRecordset, EAlbumName), ETrue);
        
        // Append Known album and Unknown artists
        count = iUnknownArtists.Count();
        if ( prevAlbumId != albumId && count > 0)
            {
            unknownMedia = NULL;
                
            for (TInt i = 0; i < count; i++)
                {
                unknownMedia = iUnknownArtists[i];
                if (unknownMedia->IsSupported(KMPXMediaGeneralId) && pPath)
                    {
                    ids.Append(unknownMedia->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
                    }
                aMediaArray.AppendL(*unknownMedia);                 
                }
                
            iUnknownArtists.ResetAndDestroy();
            }
        
        // for Unknown album and Unknown artist
        if (firstRecord && (MPXDbCommonUtil::GetColumnTextL(aRecordset, EAlbumArtistName).Length() == 0) &&
            (MPXDbCommonUtil::GetColumnTextL(aRecordset, EAlbumName).Length() == 0))
            {
            unknownAlbumUnknownArtistMedia = media;
            }
        // for Unknown albums and Known artists
        else if (MPXDbCommonUtil::GetColumnTextL(aRecordset, EAlbumName).Length() == 0)
            {
            iUnknownAlbums.Append(media);
            CleanupStack::Pop(media);
            }
        // for Known album and Unknown artist
        else if (MPXDbCommonUtil::GetColumnTextL(aRecordset, EAlbumArtistName).Length() == 0)
            {
            iUnknownArtists.Append(media);
            CleanupStack::Pop(media);
            
            prevAlbumId = albumId;
            }
        else
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

    // Append Known artist and Unknown albums
    count = iUnknownArtists.Count();
    if ( count > 0)
        {
        unknownMedia = NULL;
        
        for (TInt i = 0; i < count; i++)
            {
            unknownMedia = iUnknownArtists[i];
            if (unknownMedia->IsSupported(KMPXMediaGeneralId) && pPath)
                {
                ids.Append(unknownMedia->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
                }
            aMediaArray.AppendL(*unknownMedia);                 
            }
        
        iUnknownArtists.ResetAndDestroy();
        }

    // Append Unknown artists to MediaArray
    count = iUnknownAlbums.Count();
    if ( count > 0)
        {
        CMPXMedia* unknownMedia = NULL;
        
        for (TInt i = 0; i < count; i++)
            {
            unknownMedia = iUnknownAlbums[i];
            if (unknownMedia->IsSupported(KMPXMediaGeneralId) && pPath)
                {
                ids.Append(unknownMedia->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
                }
            aMediaArray.AppendL(*unknownMedia);                 
            }
        
        iUnknownAlbums.ResetAndDestroy();
        }
    
    // the last item in the list
    if (unknownAlbumUnknownArtistMedia)
        {
        if (unknownAlbumUnknownArtistMedia->IsSupported(KMPXMediaGeneralId) && pPath)
            {
            ids.AppendL(unknownAlbumUnknownArtistMedia->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
            }
        aMediaArray.AppendL(*unknownAlbumUnknownArtistMedia);
        CleanupStack::PopAndDestroy(unknownAlbumUnknownArtistMedia);
        }
    
    // Append ids to the returned path
    if (pPath)
        {
        ((CMPXCollectionPath*)pPath)->AppendL(ids.Array());
        }
    CleanupStack::PopAndDestroy(&ids);
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::ProcessMediaWallAlbumRecordSetL
// Unknown album or Unknown artist are stored in the database as NULL (Name field and ArtistName field). 
// The available unknown album is saved for later and will be appended to the array 
// as the last item among items which have the same artist.
// The available unknown artists are saved for later and will be appended to the array
// as the last items after all known artists are appended.
// The unknown album and unknown artist is saved for later and will be appended to the array
// as the last item.
//
// NOTE: putting unknown album to the end of the array only takes place when title
//       field is requested. normal sorting algorithm occurs if title isn't
//       requested.
//
// Example:
//     Album X of artist A
//     Album Y of artist A
//     Unknown album of artist A
//     Album X of artist B
//     Album Z of artist B
//     Unknown album of artist B
//     Album X of unknown artist
//     Album Y of unknown artist
//     Unknown album of unknown artist
// ----------------------------------------------------------------------------
//
//
void CMPXDbAlbum::ProcessMediaWallAlbumRecordSetL(
    const TArray<TMPXAttribute>& aAttrs,
    RSqlStatement& aRecordset,
    CMPXMediaArray& aMediaArray)
    {
    // populate the array
    TBool firstRecord(ETrue);
    CMPXMedia* unknownAlbumUnknownArtistMedia(NULL);
    CMPXMedia* unknownMedia(NULL);
    TUint32 prevId(0);
    TUint32 artistId(0);
    TUint32 prevArtistId(0);
    TInt count(0);
    TInt err(KErrNone);
       
    // Reset array for unknown artists
    if (iUnknownArtists.Count() > 0)
        {
        iUnknownArtists.ResetAndDestroy();
        }
    
    // Reset array for unknown albums
    if (iUnknownAlbums.Count() > 0)
        {
        iUnknownAlbums.ResetAndDestroy();
        }
        
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
        TUint32 rowId(aRecordset.ColumnInt64(EAlbumUniqueId));
        if (prevId == rowId)
            {
            continue;
            }

        prevId = rowId;
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);

        UpdateMediaL(aRecordset, aAttrs, *media);
    
        artistId = aRecordset.ColumnInt64(EAlbumArtist);

        // Append Known artist and Unknown albums
        count = iUnknownAlbums.Count();
        if (prevArtistId != artistId && count > 0)
            {
            unknownMedia = NULL;
                
            for (TInt i = 0; i < count; i++)
                {
                unknownMedia = iUnknownAlbums[i];
                if (unknownMedia->IsSupported(KMPXMediaGeneralId) && pPath)
                    {
                    ids.Append(unknownMedia->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
                    }
                aMediaArray.AppendL(*unknownMedia);                 
                }
                
            iUnknownAlbums.ResetAndDestroy();
            }
        
        // for Unknown artist and Unknown album
        if (firstRecord && (MPXDbCommonUtil::GetColumnTextL(aRecordset, EAlbumName).Length() == 0) &&
            (MPXDbCommonUtil::GetColumnTextL(aRecordset, EAlbumArtistName).Length() == 0))
            {
            unknownAlbumUnknownArtistMedia = media;
            }
        // for Unknown artists and Known albums
        else if (MPXDbCommonUtil::GetColumnTextL(aRecordset, EAlbumArtistName).Length() == 0)
            {
            iUnknownArtists.Append(media);
            CleanupStack::Pop(media);
            }
        // for Known artist and Unknown album
        else if (MPXDbCommonUtil::GetColumnTextL(aRecordset, EAlbumName).Length() == 0)
            {
            iUnknownAlbums.Append(media);
            CleanupStack::Pop(media);
            prevArtistId = artistId;
            }
        else
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

    // Append Known artist and Unknown albums
    count = iUnknownAlbums.Count();
    if ( count > 0)
        {
        unknownMedia = NULL;
        
        for (TInt i = 0; i < count; i++)
            {
            unknownMedia = iUnknownAlbums[i];
            if (unknownMedia->IsSupported(KMPXMediaGeneralId) && pPath)
                {
                ids.Append(unknownMedia->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
                }
            aMediaArray.AppendL(*unknownMedia);                 
            }
        
        iUnknownAlbums.ResetAndDestroy();
        }

    // Append Unknown artists to MediaArray
    count = iUnknownArtists.Count();
    if ( count > 0)
        {
        CMPXMedia* unknownMedia = NULL;
        
        for (TInt i = 0; i < count; i++)
            {
            unknownMedia = iUnknownArtists[i];
            if (unknownMedia->IsSupported(KMPXMediaGeneralId) && pPath)
                {
                ids.Append(unknownMedia->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
                }
            aMediaArray.AppendL(*unknownMedia);                 
            }
        
        iUnknownArtists.ResetAndDestroy();
        }
    
    // the last item in the list
    if (unknownAlbumUnknownArtistMedia)
        {
        if (unknownAlbumUnknownArtistMedia->IsSupported(KMPXMediaGeneralId) && pPath)
            {
            ids.AppendL(unknownAlbumUnknownArtistMedia->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
            }
        aMediaArray.AppendL(*unknownAlbumUnknownArtistMedia);
        CleanupStack::PopAndDestroy(unknownAlbumUnknownArtistMedia);
        }
    
    // Append ids to the returned path
    if (pPath)
        {
        ((CMPXCollectionPath*)pPath)->AppendL(ids.Array());
        }
    CleanupStack::PopAndDestroy(&ids);
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::GenerateUniqueIdL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbAlbum::GenerateUniqueIdL(const CMPXMedia& aMedia)
    {
    TPtrC album(KNullDesC);
    TPtrC albumArtist(KNullDesC);
    TPtrC artist(KNullDesC);
    
    // check Album attirbute
    if (aMedia.IsSupported(KMPXMediaMusicAlbum))     
        {
        album.Set(aMedia.ValueText(KMPXMediaMusicAlbum).Left(KMCMaxTextLen));
        }
    
    HBufC* buf(NULL);    
    if (RetrieveAlbumArtistL(aMedia, albumArtist))
        {
        buf = HBufC::NewLC(album.Length() + albumArtist.Length() + KAlbumAlbumArtistSeperator().Length());
    
        buf->Des().Append(album);
        buf->Des().Append(KAlbumAlbumArtistSeperator);
        buf->Des().Append(albumArtist);
        }
    else
        {
        RetrieveArtist(aMedia, artist);
        
        buf = HBufC::NewLC(album.Length() + artist.Length() + KAlbumArtistSeperator().Length());
        
        buf->Des().Append(album);
        buf->Des().Append(KAlbumArtistSeperator);
        buf->Des().Append(artist);
        }
    
    // try to find the item first
    TUint32 id(MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), EMPXAlbum,
        *buf, ETrue));
    CleanupStack::PopAndDestroy(buf);
    
    return id;
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::IsUnknownAlbumL
// ----------------------------------------------------------------------------
//
TBool CMPXDbAlbum::IsUnknownAlbumL(const TUint32 aId)
    {
    HBufC* query = PreProcessStringLC(KQueryCategoryItem);
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(*query, aId));
    CleanupStack::PopAndDestroy(query);
    CleanupClosePushL(recordset);

    if (recordset.Next() != KSqlAtRow)
        {
        User::Leave(KErrNotFound);
        }
    
    TPtrC album(MPXDbCommonUtil::GetColumnTextL(recordset, EAlbumName)); 
    TPtrC artistName(MPXDbCommonUtil::GetColumnTextL(recordset, EAlbumArtistName)); 

    TBool ret = EFalse;
    if (album.Length() == 0 || artistName.Length() == 0)
        {
        ret = ETrue;   
        }
    else
        {
        ret = EFalse;
        }
    
    CleanupStack::PopAndDestroy(&recordset);
    
    return ret;
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::RetrieveAlbumArtistL
// ----------------------------------------------------------------------------
//
TBool CMPXDbAlbum::RetrieveAlbumArtistL(const CMPXMedia& aMedia, TPtrC& aName)
    {
    // check AlbumArtist attribute
    if (aMedia.IsSupported(KMPXMediaMusicAlbumArtist))
        {
        aName.Set(aMedia.ValueText(KMPXMediaMusicAlbumArtist).Left(KMCMaxTextLen));
        }
    else
        {
        aName.Set(KNullDesC);
        }
    
    if (aName.Length() > 0 )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::RetrieveArtist
// ----------------------------------------------------------------------------
//
void CMPXDbAlbum::RetrieveArtist(const CMPXMedia& aMedia, TPtrC& aName)
    {
    // check Artist attribute
    if (aMedia.IsSupported(KMPXMediaMusicArtist))
        {
        aName.Set(aMedia.ValueText(KMPXMediaMusicArtist).Left(KMCMaxTextLen));
        }
    else
        {
        aName.Set(KNullDesC);            
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::NeedToUpdateArt
// ----------------------------------------------------------------------------
//
TBool CMPXDbAlbum::NeedToUpdateArt(const TDesC& aDeletedSongArt, const TDesC& aCurrentAlbumArt)
    { 
    TBool needToUpdateArt = EFalse;
    
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
    TBool isNonEmbedded = EFalse;
    if (aDeletedSongArt.Length() > 0)
        {
        TParsePtrC parse(aDeletedSongArt);
        TPtrC ext(parse.Ext());
        //set flag to false, so .alb will not overwrite art field in album, artist table 
        // when song with embedded art
        if (ext.CompareF(KAbstractAlbumExt) == 0) 
            {     
            // Deleted song's art is Non-embedded album art
            isNonEmbedded = ETrue;
            }
        }
    if (!isNonEmbedded)
        {
#endif   // ABSTRACTAUDIOALBUM_INCLUDED
        if (aDeletedSongArt.Length() > 0 && aCurrentAlbumArt.Length() > 0 && aDeletedSongArt.CompareF(aCurrentAlbumArt) == 0)
            {
            // Deleted song's art is Embedded album art and it is the same as Album's current art
            needToUpdateArt = ETrue;
            }
#ifdef ABSTRACTAUDIOALBUM_INCLUDED            
        }
#endif // ABSTRACTAUDIOALBUM_INCLUDED
    return needToUpdateArt;
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::CreateTableL
// ----------------------------------------------------------------------------
//
void CMPXDbAlbum::CreateTableL(
    RSqlDatabase& aDatabase,
    TBool /* aCorruptTable */)
    {
    MPX_FUNC("CMPXDbCategory::CreateTableL");

    // create the table
    User::LeaveIfError(aDatabase.Exec(KAlbumCreateTable));

    // do not create an index on the Name field
    // as it only slows down the insert/update queries overall
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::CheckTableL
// ----------------------------------------------------------------------------
//
TBool CMPXDbAlbum::CheckTableL(
    RSqlDatabase& aDatabase)
    {
    MPX_FUNC("CMPXDbCategory::CheckTableL");

    TBool check(DoCheckTable(aDatabase, KAlbumCheckTable));

    return check;
    }

// End of File
