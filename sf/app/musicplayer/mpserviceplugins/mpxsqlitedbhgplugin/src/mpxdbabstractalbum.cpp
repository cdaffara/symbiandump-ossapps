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
*                Artist, Album, Genre, Composer and AbstractAlbum
*
*
*/


// INCLUDE FILES
#include <mpxlog.h>
#include <bautils.h>
#include <mpxmediamusicdefs.h>
#include "mpxdbabstractalbum.h"
#include "mpxdbpluginqueries.h"
#include "mpxdbmanager.h"
#include "mpxdbcommonutil.h"
#include "mpxcollectiondbstd.h"
#include "mpxdbcommondef.h"
#include "mpxcollectiondbdef.h"

#ifdef RD_MPX_TNM_INTEGRATION
_LIT( KImageFileType, "image/jpeg" );
#endif //RD_MPX_TNM_INTEGRATION


// CONSTANTS

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbAbstractAlbum* CMPXDbAbstractAlbum::NewL(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory, RFs& aFs)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::NewL");

    CMPXDbAbstractAlbum* self = CMPXDbAbstractAlbum::NewLC(aDbManager, aCategory, aFs);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbAbstractAlbum* CMPXDbAbstractAlbum::NewLC(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory, RFs& aFs)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::NewLC");

    CMPXDbAbstractAlbum* self = new (ELeave) CMPXDbAbstractAlbum(aDbManager, aCategory, aFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXDbAbstractAlbum::~CMPXDbAbstractAlbum()
    {
    MPX_FUNC("CMPXDbAbstractAlbum::~CMPXDbAbstractAlbum");
#ifdef RD_MPX_TNM_INTEGRATION
    if (iTNManager)
        delete iTNManager;
#endif //RD_MPX_TNM_INTEGRATION
    }

// ----------------------------------------------------------------------------
// CMPXDbAbstractAlbum::GetDriveIdL
// ----------------------------------------------------------------------------
//
TInt CMPXDbAbstractAlbum::GetDriveIdL(
    TUint32 aAbstractAlbumId)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::GetDriveIdL");
    return MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(),
        ExecuteIntQueryL(KCriterionCategoryVolumeId, aAbstractAlbumId));
    }


// ----------------------------------------------------------------------------
// CMPXDbAbstractAlbum::AddItemL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbAbstractAlbum::AddItemL(
    const TDesC& aUri,
    const TDesC& aName,
    const TDesC& aAlbumArtist,
    TInt aDriveId,
    TBool& aNewRecord,
    TBool aCaseSensitive)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::AddItemL");

    TUint32 rowId(MPXDbCommonUtil::GenerateUniqueIdL(iDbManager.Fs(), EMPXAbstractAlbum,
        aUri, aCaseSensitive));
    aNewRecord = !CategoryItemExistsL(aDriveId, rowId);

    if ( aNewRecord )
        {
        HBufC* uri = MPXDbCommonUtil::ProcessSingleQuotesLC(aUri);
        HBufC* name = MPXDbCommonUtil::ProcessSingleQuotesLC(aName);
        HBufC* albumartist = MPXDbCommonUtil::ProcessSingleQuotesLC(aAlbumArtist);

        iDbManager.ExecuteQueryL(aDriveId, KQueryAbstractAlbumInsert, rowId, uri, name, albumartist, 0, MPXDbCommonUtil::GetVolIdMatchDriveIdL(iDbManager.Fs(), aDriveId));
        CleanupStack::PopAndDestroy(3, uri);
        }
    else
        {
        MPX_DEBUG1("CMPXDbAbstractAlbum::AddItemL, increase song count for abstractalbum");
        // increment the number of songs for the category
        HBufC* query = PreProcessStringLC(KQueryCategoryIncrementSongCount);
        iDbManager.ExecuteQueryL(aDriveId, *query, rowId);
        CleanupStack::PopAndDestroy(query);
        }

    return rowId;
    }

// ----------------------------------------------------------------------------
// CMPXDbAbstractAlbum::AddUpdatedItemL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbAbstractAlbum::AddUpdatedItemL(TUint32 aOldId, const TDesC& newUri)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::AddUpdatedItemL");
    iRenameTN = ETrue;
    
    //get the existing item based on aOldId
    HBufC* query = NULL;
    query = PreProcessStringLC(KQueryCategoryItem);
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(*query, aOldId));
    CleanupStack::PopAndDestroy(query);
    
    CleanupClosePushL(recordset);
    TBool newRecord(ETrue);
    if (recordset.Next() != KSqlAtRow)
        {
        User::Leave(KErrNotFound);
        }
    //save the existing entry fields values for renaming purpose
    const TDesC& name(MPXDbCommonUtil::GetColumnTextL (recordset, EAbstractAlbumName));
    const TDesC& albumartist(MPXDbCommonUtil::GetColumnTextL (recordset, EAbstractAlbumArtist));

    TUint32 itemId = AddItemL(newUri, name, albumartist, GetDriveIdL(aOldId), newRecord, EFalse);
    CleanupStack::PopAndDestroy(&recordset);

    return itemId;
    }

// ----------------------------------------------------------------------------
// CMPXDbAbstractAlbum::DecrementSongsForCategoryL
// ----------------------------------------------------------------------------
//
void CMPXDbAbstractAlbum::DecrementSongsForCategoryL(
    const TUint32 aId,
    TInt aDriveId,
    CMPXMessageArray* aItemChangedMessages,
    TBool& aItemExist,
    TBool aMtpInUse)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::DecrementSongsForCategoryL");
    
    TInt songCount = GetSongsCountL(aDriveId, aId);
    //while MTP connection, just decrease songCount in DB, do not do deletion.
    if ( songCount > 0 )
        {
        aItemExist = ETrue;
        // decrement the number of songs for the category
        HBufC* query = PreProcessStringLC(KQueryCategoryDecrementSongCount);
        iDbManager.ExecuteQueryL(aDriveId, *query, aId);
        CleanupStack::PopAndDestroy(query);
        songCount--;
        }
    if (songCount == 0)
        {
        //get uri
        HBufC* uri = GetUriL(aId);
        CleanupStack::PushL(uri);
            
        if (!aMtpInUse)
	        {       
	        DeleteAbstractAlbumEntryL(aId, aDriveId);
	        MPX_DEBUG1("CMPXDbAbstractAlbum::DecrementSongsForCategoryL,implicitly delete AbstractAlbum Object when it has 0 references");
	        aItemExist = EFalse;
	        if (aItemChangedMessages)
	            {
	            // add the item changed message
	            MPXDbCommonUtil::AddItemChangedMessageL(*aItemChangedMessages, aId, EMPXItemDeleted,
	                EMPXAbstractAlbum, KDBPluginUid);
	            }
	        //delete .alb file from file system                
	        TInt err = BaflUtils::DeleteFile(iFs, *uri);                       
	        if(KErrNone != err)
	            {
	            MPX_DEBUG2("CMPXDbAbstractAlbum::DecrementSongsForCategoryL,File not deleted from file system with err=%d", err);
	            }   
	        }
        if(!iRenameTN)
	        {
	        HandleTNL(*uri);
	        }   
        CleanupStack::PopAndDestroy(uri);
        }  
    }

// ----------------------------------------------------------------------------
// Remove abstractalbums which have no songs associated.
// ----------------------------------------------------------------------------
//
void CMPXDbAbstractAlbum::RemoveAbstractAlbumsWithNoSongL()
    {
    MPX_FUNC("CMPXDbAbstractAlbum::RemoveAbstractAlbumsWithNoSongL");
    
    RArray<TUint32> iItemsIds;
    CleanupClosePushL(iItemsIds);
    
    //get all abstractalbum with no songs associated.
    GetAllItemsWithNoSongL(iItemsIds);
    TInt count = iItemsIds.Count();
    TInt err = KErrNone;
    if (count)
        {           	
        MPX_DEBUG2("CMPXDbAbstractAlbum::RemoveAbstractAlbumsWithNoSongL, abstractalbum count[%d] ", iItemsIds.Count());
        CMPXMessageArray* itemChangedMessages = CMPXMediaArray::NewL();
        CleanupStack::PushL(itemChangedMessages);

        //go through each one to delete
        for (TInt i=0; i< count; i++)
            {
            TRAP(err, RemoveAbstractAlbumL(iItemsIds[i], *itemChangedMessages, EFalse));
            if (err != KErrNone)
                {
                MPX_DEBUG2("CMPXDbAbstractAlbum::RemoveAbstractAlbumsWithNoSongL, error happens when delete abstractalbum, err ", err);
                }
            }
       CleanupStack::PopAndDestroy(itemChangedMessages);
       }
    CleanupStack::PopAndDestroy(&iItemsIds);
  }

// ----------------------------------------------------------------------------
// Remove .alb entry from AbstractAlnum table, TN table if .alb files deleted 
// from file manager when refresh library
// ----------------------------------------------------------------------------
//
void CMPXDbAbstractAlbum::AbstractAlbumCleanUpL()
    {
    MPX_FUNC("CMPXDbAbstractAlbum::AbstractAlbumCleanUpL");
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL(attributes);
    attributes.AppendL(KMPXMediaGeneralUri);
    CMPXMediaArray* mediaArray = CMPXMediaArray::NewL();
    CleanupStack::PushL(mediaArray);
    
    GetAllCategoryItemsL(attributes.Array(), *mediaArray);

    TInt count(mediaArray->Count());
    if (count)
        {      
        CMPXMessageArray* itemChangedMessages = CMPXMediaArray::NewL();
        CleanupStack::PushL(itemChangedMessages);
        for (TInt i = 0; i < count; i++)
            {
            CMPXMedia* element = mediaArray->AtL(i);
            const TDesC& uri = element->ValueText(KMPXMediaGeneralUri);                                 

            //check if the file exists in file system
            if (!(BaflUtils::FileExists(iFs, uri)))
                {
                //generate abstractalbum UID with the Uri
                TUint32 abstractAlbumId(MPXDbCommonUtil::GenerateUniqueIdL(iFs, EMPXAbstractAlbum, uri, EFalse));
                RemoveAbstractAlbumL(abstractAlbumId, *itemChangedMessages, ETrue);             
                }
            }
            CleanupStack::PopAndDestroy(itemChangedMessages);
        }
        CleanupStack::PopAndDestroy(mediaArray);       
        CleanupStack::PopAndDestroy(&attributes);     
    }

// ----------------------------------------------------------------------------
// CMPXDbAbstractAlbum::RemoveAbstractAlbumL
// ----------------------------------------------------------------------------
//
void CMPXDbAbstractAlbum::RemoveAbstractAlbumL(TUint32 aAbstractAlbumId, 
    CMPXMessageArray& aItemChangedMessages, TBool aFileDeleted)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::RemoveAbstractAlbumL");
    HBufC* uri = GetUriL(aAbstractAlbumId);
    CleanupStack::PushL(uri);
    
    DeleteAbstractAlbumEntryL(aAbstractAlbumId);
    HandleTNL(*uri);
    if (!aFileDeleted)
        {             
        TInt err = BaflUtils::DeleteFile(iFs, *uri);                       
        if(KErrNone != err)
            {
            MPX_DEBUG2("CMPXDbAbstractAlbum::RemoveAbstractAlbumL,File not deleted from file system with err=%d", err);
            }   
        }   
    MPXDbCommonUtil::AddItemChangedMessageL(aItemChangedMessages, aAbstractAlbumId, EMPXItemDeleted, EMPXAbstractAlbum, KDBPluginUid);            
    CleanupStack::PopAndDestroy(uri);
    }


// ----------------------------------------------------------------------------
// CMPXDbAbstractAlbum::HandleTNL
// ----------------------------------------------------------------------------
//
void CMPXDbAbstractAlbum::HandleTNL( const TDesC& aOldPath,
        const TDesC& aNewPath, TInt aPriority )
  {
  MPX_FUNC("CMPXDbAbstractAlbum::HandleTNL");
  if (!iTNManager)
      {
      CreateTNMSessionL();
      }
  if (aNewPath.Compare(KNullDesC)==0 && aOldPath.Compare(KNullDesC)!=0)
      {         
      // remove from thumbnail database table
      MPX_DEBUG1("CMPXDbAbstractAlbum::HandleTNL, delete TN");  
      CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC(
               aOldPath, KImageFileType );
      iTNManager->DeleteThumbnails( *source );
          
      CleanupStack::PopAndDestroy( source );
      }
   else if (aNewPath.Compare(KNullDesC)!=0)
      {
      //rename thumbnail
      MPX_DEBUG1("CMPXDbAbstractAlbum::HandleTNL, rename Thumbnail entry in Thumbnail table");
    
      iTNManager->RenameThumbnailsL( aOldPath,  aNewPath, aPriority );  
      //after rename, reset flag
      iRenameTN = EFalse;
      }
 }


// ----------------------------------------------------------------------------
// CMPXDbAbstractAlbum::UpdateItemL
// ----------------------------------------------------------------------------
//
void CMPXDbAbstractAlbum::UpdateItemL(
    TUint32 aId,
    const CMPXMedia& aMedia,
    TInt aDriveId,
    CMPXMessageArray* aItemChangedMessages)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::UpdateItemL");

    CDesCArrayFlat* fields = new (ELeave) CDesCArrayFlat(EAbstractAlbumFieldCount);
    CleanupStack::PushL(fields);
    CDesCArrayFlat* values = new (ELeave) CDesCArrayFlat(EAbstractAlbumFieldCount);
    CleanupStack::PushL(values);

    // process the media parameter and construct the fields and values array
    GenerateAbstractAlbumFieldsValuesL(aMedia, *fields, *values);

    // construct the SET string
    HBufC* setStr = MPXDbCommonUtil::StringFromArraysLC(*fields, *values, KMCEqualSign, KMCCommaSign);

    if (setStr->Length())
        {
        // execute the query
        iDbManager.ExecuteQueryL(aDriveId, KQueryAbstractAlbumUpdate, setStr, aId);
        MPXDbCommonUtil::AddItemAlbumChangedMessageL(*aItemChangedMessages, aId, EMPXItemModified,
                EMPXAbstractAlbum, KDBPluginUid, ETrue, 0 );
        }
    CleanupStack::PopAndDestroy(setStr);
    CleanupStack::PopAndDestroy(values);
    CleanupStack::PopAndDestroy(fields);
    }


// ----------------------------------------------------------------------------
// CMPXDbAbstractAlbum::GetUriL
// ----------------------------------------------------------------------------
//
HBufC* CMPXDbAbstractAlbum::GetUriL(
    TUint32 aId)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::GetUriL");

    RSqlStatement recordset(GetCategoryRecordL(aId));
    CleanupClosePushL(recordset);
    HBufC* uri = NULL;
    if (recordset.Next() == KSqlAtRow)
        {
        uri = MPXDbCommonUtil::GetColumnTextL(recordset, EAbstractAlbumUri).AllocL();     
        }
    else
        {
        //entry is deleted due to garbage collection 
        MPX_DEBUG1("CMPXDbAbstractAlbum::GetUriL, no uri, entry is deleted due to garbage collection"); 
        TPtrC nullUri(KNullDesC);
        uri = nullUri.AllocL();
        }
    CleanupStack::PopAndDestroy(&recordset);    
    return uri;
    }


// ----------------------------------------------------------------------------
// CMPXDbAbstractAlbum::UpdateMediaL
// ----------------------------------------------------------------------------
//
void CMPXDbAbstractAlbum::UpdateMediaL(
    RSqlStatement& aRecord,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::UpdateMediaL");

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
                       aRecord.ColumnInt64(EAbstractAlbumUniqueId));
                   }
               if (attributeId & EMPXMediaGeneralTitle)
                   {
                   MPX_DEBUG1("    EMPXMediaGeneralTitle");
                   TPtrC title(MPXDbCommonUtil::GetColumnTextL(aRecord, EAbstractAlbumName));
                   aMedia.SetTextValueL(KMPXMediaGeneralTitle, title);
                   MPX_DEBUG2("    AbstractAlbumName[%S]", &title);
                   }
               if (attributeId & EMPXMediaGeneralCount)
                   {
                   MPX_DEBUG1("    EMPXMediaGeneralCount");
                   TInt songCount = GetSongsCountL(KDbManagerAllDrives,
                                aRecord.ColumnInt64(EAbstractAlbumUniqueId));
                   aMedia.SetTObjectValueL<TInt>(KMPXMediaGeneralCount, songCount );
                   }
               if (attributeId & EMPXMediaGeneralDrive ||
                    (attributeId & EMPXMediaGeneralUri))
                   {                       
                   TUint32 volId(aRecord.ColumnInt64(EAbstractAlbumVolumeId));
                   TInt driveId = MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(), volId);
                   MPX_DEBUG3("volId = %d, driveId = %d", volId, driveId);

                   // handle possibly delay from framework notification
                   if (driveId < 0)
                       {
                       MPX_DEBUG1("invalid driveId, leave with KErrNotReady");
                       User::Leave(KErrNotReady);
                       }
                   TDriveUnit driveUnit(driveId);
                   if (attributeId & EMPXMediaGeneralUri)
                       {                              
                       TPtrC uri(MPXDbCommonUtil::GetColumnTextL(aRecord, EAbstractAlbumUri));
                       aMedia.SetTextValueL(KMPXMediaGeneralUri, uri);
                       MPX_DEBUG2("    AbstractAlbum URI[%S]", &uri);
                       }
                  if (attributeId & EMPXMediaGeneralDrive)
                       {
                       aMedia.SetTextValueL(KMPXMediaGeneralDrive, driveUnit.Name());
                       }
                   }
               } // end if contentId == KMPXMediaIdGeneral

           else if ( contentId == KMPXMediaIdMusic)
               {
               if (attributeId & EMPXMediaMusicAlbumArtist)
                   {
                   MPX_DEBUG1("    EMPXMediaMusicAlbumArtist");
                   TPtrC albumartist(MPXDbCommonUtil::GetColumnTextL(aRecord, EAbstractAlbumArtist));
                   aMedia.SetTextValueL(KMPXMediaMusicAlbumArtist, albumartist);
                   MPX_DEBUG2("    albumartist[%S]", &albumartist);
                   }
               }

           } // end for
       aMedia.SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
       aMedia.SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXAbstractAlbum);
    }

// ----------------------------------------------------------------------------
// CMPXDbAbstractAlbum::GetAllItemsWithNoSongL
// ----------------------------------------------------------------------------
//
void CMPXDbAbstractAlbum::GetAllItemsWithNoSongL(RArray<TUint32>& aItemsIds)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::GetAllItemsWithNoSongL");
    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(KQueryAbstractAlbumsWithNoSong));
    CleanupClosePushL(recordset);

    TInt err(KErrNone);
    while ((err = recordset.Next()) == KSqlAtRow)
        {
        TUint32 uniqueID(recordset.ColumnInt64(EAbstractAlbumUniqueId));
        aItemsIds.AppendL(uniqueID);
        }
    CleanupStack::PopAndDestroy(&recordset);
    if (err!= KSqlAtEnd)
        {
        User::Leave(KErrCorrupt);
        } 	
    }


// ----------------------------------------------------------------------------
// CMPXDbAbstractAlbum::GenerateAbstractAlbumFieldsValuesL
// ----------------------------------------------------------------------------
//
void CMPXDbAbstractAlbum::GenerateAbstractAlbumFieldsValuesL(const CMPXMedia& aMedia, CDesCArray& aFields, CDesCArray& aValues)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::GenerateAbstractAlbumFieldsValuesL");
    //support Winlogo use case
    if (aMedia.IsSupported(KMPXMediaGeneralTitle))
        {
        TPtrC truncatedName(aMedia.ValueText(KMPXMediaGeneralTitle).Left(KMCMaxTextLen));
        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCCategoryName, truncatedName);
        }
    //support Winlogo use case
    if (aMedia.IsSupported(KMPXMediaMusicAlbumArtist))
        {
        TPtrC truncatedAlbumArtist(aMedia.ValueText(KMPXMediaMusicAlbumArtist).Left(KMCMaxTextLen));
        MPXDbCommonUtil::AppendValueL(aFields, aValues, KMCMusicAlbumArtist, truncatedAlbumArtist);
        }
     }

// ----------------------------------------------------------------------------
// CMPXDbAbstractAlbum::DeleteAbstractAlbumEntryL
// ----------------------------------------------------------------------------
//
void CMPXDbAbstractAlbum::DeleteAbstractAlbumEntryL(
    TUint32 aAbstractAlbumId, TInt aDriveId)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::DeleteAbstractAlbumEntryL");

    //delete entry from table        
    if (aDriveId)
        DeleteCategoryL(aAbstractAlbumId, aDriveId);
    else
        {
        TInt drive = MPXDbCommonUtil::GetDriveIdMatchVolIdL(iDbManager.Fs(),
        ExecuteIntQueryL(KCriterionCategoryVolumeId, aAbstractAlbumId));
        DeleteCategoryL(aAbstractAlbumId, drive);
        }
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::CreateTableL
// ----------------------------------------------------------------------------
//
void CMPXDbAbstractAlbum::CreateTableL(
    RSqlDatabase& aDatabase,
    TBool /* aCorruptTable */)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::CreateTableL");

    // create the table
    HBufC* query = PreProcessStringLC(KAbstractAlbumCreateTable);
    User::LeaveIfError(aDatabase.Exec(*query));
    CleanupStack::PopAndDestroy(query);

    // do not create an index on the Name field
    // as it only slows down the insert/update queries overall
    }

// ----------------------------------------------------------------------------
// CMPXDbAlbum::CheckTableL
// ----------------------------------------------------------------------------
//
TBool CMPXDbAbstractAlbum::CheckTableL(
    RSqlDatabase& aDatabase)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::CheckTableL");

    HBufC* query = PreProcessStringLC(KAbstractAlbumCheckTable);
    TBool check(DoCheckTable(aDatabase, *query));
    CleanupStack::PopAndDestroy(query);

    return check;
    }


// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CMPXDbAbstractAlbum::CMPXDbAbstractAlbum(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory, RFs& aFs) :
    CMPXDbCategory(aDbManager, aCategory),
    iTNManager(NULL),
    iFs(aFs)
    {
    MPX_FUNC("CMPXDbAbstractAlbum::CMPXDbAbstractAlbum");
    }

// ----------------------------------------------------------------------------
// Second phase constructor.
// ----------------------------------------------------------------------------
//
void CMPXDbAbstractAlbum::ConstructL()
    {
    MPX_FUNC("CMPXDbAbstractAlbum::ConstructL");

    BaseConstructL();
    iRenameTN = EFalse;
    }



// ---------------------------------------------------------------------------
// CMPXDbAbstractAlbum::ThumbnailReady
// Callback but not used here
// ---------------------------------------------------------------------------
void CMPXDbAbstractAlbum::ThumbnailPreviewReady(
        MThumbnailData& /*aThumbnail*/, TThumbnailRequestId /*aId*/ )
    {
    }


// ---------------------------------------------------------------------------
// CMPXDbAbstractAlbum::ThumbnailReady
// Callback but not used here
// ---------------------------------------------------------------------------
void CMPXDbAbstractAlbum::ThumbnailReady( TInt /*aError*/,
        MThumbnailData& /*aThumbnail*/, TThumbnailRequestId /*aId*/ )
    {
    }

// ---------------------------------------------------------------------------
// CMPXDbAbstractAlbum::CreateTNMSessionL
// Create thumbnail session
// ---------------------------------------------------------------------------    
void CMPXDbAbstractAlbum::CreateTNMSessionL()
    {
    if(!iTNManager)
        {
        iTNManager = CThumbnailManager::NewL( *this );
        }
    }
// End of File
