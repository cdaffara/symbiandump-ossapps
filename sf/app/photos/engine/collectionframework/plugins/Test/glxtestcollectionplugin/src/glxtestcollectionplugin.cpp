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
* Description:   Album collection plugin implementation
*
*/



// INCLUDE FILES

#include "glxtestcollectionplugin.h"
#include "glxtestcollectionplugin.hrh"
#include "glxpanic.h"
#include "glxtnthumbnailrequest.h"
#include "glxtnthumbnailcreator.h"
#include <glxtnfileinfo.h>

#include <mpxcollectionpath.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxcmn.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <fbs.h>

// CONSTANTS

// ============================ LOCAL FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// Add an item to the media array
// ----------------------------------------------------------------------------
//
LOCAL_C void AppendL(const TGlxMediaId& aId, const TDesC& aTitle,
        CMPXMediaArray& aArray, TMPXGeneralType aType,TMPXGeneralCategory aCat)
    {
    RArray<TInt> supportedIds;
    CleanupClosePushL(supportedIds);
    supportedIds.AppendL(KMPXMediaIdGeneral);
    CMPXMedia* entry = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PushL(entry);
    entry->SetTObjectValueL(KMPXMediaGeneralId, aId);
    entry->SetTextValueL(KMPXMediaGeneralTitle, aTitle);
    entry->SetTObjectValueL(KMPXMediaGeneralType, aType);
    entry->SetTObjectValueL(KMPXMediaGeneralCategory, aCat);
    aArray.AppendL(entry);
    CleanupStack::Pop(entry);
    CleanupStack::PopAndDestroy(&supportedIds);
    }
        
    
// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CGlxTestCollectionPlugin* CGlxTestCollectionPlugin::NewL(TAny* aObs)
    {
    CGlxTestCollectionPlugin* o = new (ELeave) CGlxTestCollectionPlugin(
                            static_cast<MMPXCollectionPluginObserver*>(aObs));
	CleanupStack::PushL(o);
	o->ConstructL();
	CleanupStack::Pop(o);
	return o;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CGlxTestCollectionPlugin::~CGlxTestCollectionPlugin()
    {
    iThumbnailCreator->Close(NULL);

    _iItemDBHackAlbums.ResetAndDestroy();
    _iItemDBHackContent.ResetAndDestroy();
    delete iSelf;
    delete iMedia;
    delete iOpenEntries;
    iFs.Close();

    RFbsSession::Disconnect();
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxTestCollectionPlugin::CGlxTestCollectionPlugin(
    MMPXCollectionPluginObserver* aObs)
    {
    iObs = aObs;
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CGlxTestCollectionPlugin::ConstructL()
    {
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(RFbsSession::Connect(iFs));

    iThumbnailCreator = CGlxtnThumbnailCreator::InstanceL();

    iSelf = new (ELeave) CItem;
    iSelf->iId = TGlxMediaId(KGlxTestCollectionImplementationUid);
    iSelf->SetFilenameL(KNullDesC);
    iSelf->SetTitleL(_L("Test Albums"));

    // TEMP: Create some dummy items
    iItemAddedId = 3000;
    for (TInt i = 0; i < 1; i++) 
        {
        AddItemL(KNullDesC, _L("Favourites"), _iItemDBHackAlbums);
        AddItemL(KNullDesC, _L("Barcelona"), _iItemDBHackAlbums);
        AddItemL(KNullDesC, _L("Emma's birthday"), _iItemDBHackAlbums);
        AddItemL(KNullDesC, _L("New designers"), _iItemDBHackAlbums);
        AddItemL(KNullDesC, _L("Guns N Roses gig"), _iItemDBHackAlbums);
        AddItemL(KNullDesC, _L("Kew Gardens"), _iItemDBHackAlbums);
        AddItemL(KNullDesC, _L("Helsinki festival"), _iItemDBHackAlbums);
        AddItemL(KNullDesC, _L("Yorkshire Dales"), _iItemDBHackAlbums);
        AddItemL(KNullDesC, _L("Greece holiday"), _iItemDBHackAlbums);
        }

    for (TInt i = 0; i < 1; i++) 
        {
        AddItemL(_L("c:\\newitem1.jpg"), _L("newitem1a"), _iItemDBHackContent, TTime(_L("20061105:083546.")), 123456, _L("D:"));
        AddItemL(_L("c:\\newitem2.jpg"), _L("newitem2"), _iItemDBHackContent, TTime(_L("20061105:092527.")), 125371, _L("E:"));
        AddItemL(_L("c:\\newitem3.jpg"), _L("newitem3"), _iItemDBHackContent, TTime(_L("20061105:101536.")), 267276, _L("C:"));
        AddItemL(_L("c:\\newitem4.jpg"), _L("newitem4"), _iItemDBHackContent, TTime(_L("20061106:132425.")), 57824, _L("C:"));
        AddItemL(_L("c:\\newitem5.jpg"), _L("newitem5"), _iItemDBHackContent, TTime(_L("20061108:050134.")), 274894, _L("C:"));
        AddItemL(_L("c:\\newitem6.jpg"), _L("newitem6"), _iItemDBHackContent, TTime(_L("20061127:140112.")), 1145732, _L("E:"));
        AddItemL(_L("c:\\newitem7.jpg"), _L("newitem7"), _iItemDBHackContent, TTime(_L("20070004:172506.")), 546985, _L("C:"));
        AddItemL(_L("c:\\newitem8.jpg"), _L("newitem8"), _iItemDBHackContent, TTime(_L("20070015:092633.")), 1562347, _L("C:"));
        AddItemL(_L("c:\\newitem9.jpg"), _L("newitem9"), _iItemDBHackContent, TTime(_L("20070024:132121.")), 679512, _L("E:"));
        AddItemL(_L("c:\\newitem10.jpg"), _L("newitem10"), _iItemDBHackContent, TTime(_L("20070029:165755.")), 423566, _L("E:"));
        AddItemL(_L("c:\\newitem11.jpg"), _L("newitem11"), _iItemDBHackContent, TTime(_L("20070103:111214.")), 25714, _L("C:"));
        AddItemL(_L("c:\\newitem12.jpg"), _L("newitem12"), _iItemDBHackContent, TTime(_L("20070200:101533.")), 358715, _L("C:"));
        AddItemL(_L("c:\\newitem13.jpg"), _L("newitem13"), _iItemDBHackContent, TTime(_L("20070214:144652.")), 257968, _L("C:"));
        AddItemL(_L("c:\\newitem14.jpg"), _L("newitem14"), _iItemDBHackContent, TTime(_L("20070214:152323.")), 2652125, _L("C:"));
        }
    }

// ----------------------------------------------------------------------------
// Executes a command on the selected collection
// ----------------------------------------------------------------------------
//
void CGlxTestCollectionPlugin::CommandL(TMPXCollectionCommand /*aCmd*/, TInt /*aArg*/)
    {
    }

// ----------------------------------------------------------------------------
// Executes a command on the selected collection
// ----------------------------------------------------------------------------
//
void CGlxTestCollectionPlugin::CommandL(const CMPXCommand& /*aCmd*/)
    {
    }

// ----------------------------------------------------------------------------
// Navigates to the given path
// ----------------------------------------------------------------------------
//
void CGlxTestCollectionPlugin::OpenL(const CMPXCollectionPath& aPath,
                   const TArray<TMPXAttribute>& /*aAttrs*/,
                   CMPXFilter* /*aFilter*/)
    {
    RArray<TInt> supportedIds;
    CleanupClosePushL(supportedIds);
    supportedIds.AppendL(KMPXMediaIdContainer);
    supportedIds.AppendL(KMPXMediaIdGeneral);
    iOpenEntries = CMPXMedia::NewL(supportedIds.Array());

    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);

 	if ( aPath.Levels() == 1 ) 
 		{
 		TInt count = _iItemDBHackAlbums.Count();
 		for ( TInt i = 0; i < count; i++ )
 			{
 			AppendL(_iItemDBHackAlbums[i]->iId, *_iItemDBHackAlbums[i]->iTitle, *array, EMPXGroup, EMPXAlbum);
 			}
 		}
 	else if ( aPath.Levels() == 2 ) 
 		{
 		TInt count = _iItemDBHackContent.Count();
 		for ( TInt i = 0; i < count; i++ )
 			{
 			AppendL(_iItemDBHackContent[i]->iId, *_iItemDBHackContent[i]->iTitle, *array, EMPXItem, EMPXImage);
 			}
 		}
 	else 
 		{
 		__DEBUG_ONLY(Panic(EGlxPanicDebugUnexpectedError));
 		}
    
    iOpenEntries->SetCObjectValueL(KMPXMediaArrayContents, array);
    iOpenEntries->SetTObjectValueL(KMPXMediaArrayCount, array->Count());

    CleanupStack::PopAndDestroy(array);    

	//iObs->HandleOpen(*iOpenEntries, KErrNone);
	
    AsyncNotifyL(iOpenEntries, KErrNone, CGlxAsyncNotifier::EOpen);
        
    CleanupStack::PopAndDestroy(&supportedIds);        
    }
    

// ----------------------------------------------------------------------------
// Extended properties of the current file (async)
// ----------------------------------------------------------------------------
//
void CGlxTestCollectionPlugin::MediaL(const CMPXCollectionPath& aPath,
		const TArray<TMPXAttribute>& aAttrs,
                        const TArray<TCapability>& /*aCaps*/,
                        CMPXAttributeSpecs* /*aSpecs*/)
    {
    __ASSERT_ALWAYS(!iMedia, Panic(EGlxPanicIllegalState));

    TInt error = KErrNone;
    CItem* item = NULL;
    TInt bitmapHandle = 0;

    RArray<TInt> supportedIds;
    CleanupClosePushL(supportedIds);
    iMedia = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);

    TArray<TInt> selection = aPath.Selection();
    TInt selectionCount = selection.Count();
    if (selectionCount > 1)
        {
        CMPXMediaArray* mediaArray = CMPXMediaArray::NewL();
        CleanupStack::PushL(mediaArray);

        for (TInt count = 0; count < selectionCount; ++count)
            {
            item = ItemL(aPath, selection[count]);

            if (item)
                {
                RArray<TInt> supportedIds;
                CleanupClosePushL(supportedIds);
                CMPXMedia* media = CMPXMedia::NewL(supportedIds.Array());
                CleanupStack::PopAndDestroy(&supportedIds);

                CleanupStack::PushL(media);

                (void)PopulateMediaWithItemL(media, item, aAttrs);

                mediaArray->AppendL(*media);

                CleanupStack::PopAndDestroy(media);
                }
            else
                {
                error = KErrNotFound;
                break;
                }
            }

        PopulateMediaWithArrayL(iMedia, mediaArray);

        CleanupStack::PopAndDestroy(mediaArray);
        }
    else
        {
	item = ItemL(aPath, aPath.Index());

        if (item)
            {
            bitmapHandle = PopulateMediaWithItemL(iMedia, item, aAttrs);
            }
        else
            {
            error = KErrNotFound;
            }
        }

    if ( (KErrNone == error) && bitmapHandle )
        {
        // Get thumbnail for the item
        TGlxThumbnailRequest request;
        request.iId = item->iId;
    	request.iSizeClass = TSize(60, 45);
    	request.iPriorityMode = TGlxThumbnailRequest::EPrioritizeSpeed;
    	request.iBitmapHandle = bitmapHandle;

       	iThumbnailCreator->FetchThumbnailL(request, *this);
        }
    else
        {
        /*****************************************************
        * Old synchronous code
        ******************************************************/
        // No thumbnail requested - send response now
        //iObs->HandleMedia(*iMedia, error);
        //delete iMedia;
        //iMedia = NULL;
        
        /*****************************************************
        * New Async Code. Instantiate callback mechanism
        ******************************************************/
        
    	AsyncNotifyL(iMedia, error, CGlxAsyncNotifier::EMedia);  
        }
    }

// ----------------------------------------------------------------------------
// Adds an item to the collection
// ----------------------------------------------------------------------------
//
void CGlxTestCollectionPlugin::AddL(const CMPXMedia& aNewMedia)
    {
    TInt error = KErrNotSupported;
//    TInt idAdded = iItemAddedId;

    TMPXGeneralCategory category
            = *aNewMedia.Value<TMPXGeneralCategory>(KMPXMediaGeneralCategory);

    if ( EMPXAlbum == category )
        {
        const TDesC& title = aNewMedia.ValueText(KMPXMediaGeneralTitle);

        AddItemL(KNullDesC, title, _iItemDBHackAlbums);
        error = KErrNone;
        }

    if ( iObs )
        {
        if ( KErrNone != error )
            {
            User::Leave(error);
            }
        else
            {
/*		    TMPXCollectionChangeEvent event = {TUid::Uid(KGlxTestCollectionImplementationUid), idAdded, EMcItemInserted};
		    RArray<TMPXCollectionChangeEvent> events;
		    CleanupClosePushL(events);
		    events.AppendL(event);
		    iObs->HandleChange(events.Array());
		    CleanupStack::PopAndDestroy(&events);
*/          }
        }
    }

// ----------------------------------------------------------------------------
// Remove an item or items under a group from the collection
// ----------------------------------------------------------------------------
//
void CGlxTestCollectionPlugin::RemoveL(const CMPXCollectionPath& /*aPath*/)
    {
    // Currently returns dummy data for testing only
/*    TMPXCollectionChangeEvent event = {TUid::Uid(KGlxTestCollectionImplementationUid), 0, EMcItemDeleted};
    RArray<TMPXCollectionChangeEvent> events;
    CleanupClosePushL(events);
    events.AppendL(event);
    iObs->HandleChange(events.Array());
    CleanupStack::PopAndDestroy(&events);
*/    }

// ----------------------------------------------------------------------------
// Remove an item or items under a group from the collection
// ----------------------------------------------------------------------------
//
void CGlxTestCollectionPlugin::RemoveL(const CMPXMedia& /*aMedia*/)
    {
    // Currently returns dummy data for testing only
/*    TMPXCollectionChangeEvent event = {TUid::Uid(KGlxTestCollectionImplementationUid), 0, EMcItemDeleted};
    RArray<TMPXCollectionChangeEvent> events;
    CleanupClosePushL(events);
    events.AppendL(event);
    iObs->HandleChange(events.Array());
    CleanupStack::PopAndDestroy(&events);
*/    }

// ----------------------------------------------------------------------------
// Sets/updates the media for the item
// ----------------------------------------------------------------------------
//
void CGlxTestCollectionPlugin::SetL(const CMPXMedia& /* aMedia */)
    {
/*    TMPXCollectionChangeEvent event = {TUid::Uid(KGlxTestCollectionImplementationUid), 0, EMcItemModified};
    RArray<TMPXCollectionChangeEvent> events;
    CleanupClosePushL(events);
    events.AppendL(event);
    iObs->HandleChange(events.Array());
    CleanupStack::PopAndDestroy(&events);
*/    }

// ----------------------------------------------------------------------------
// FindAllL
// ----------------------------------------------------------------------------
//
void CGlxTestCollectionPlugin::FindAllL(const CMPXMedia& /*aMedia*/, const TArray<TMPXAttribute>& /*aAttrs*/)
    {
    	// Currently not supported. Get it to complete with a not supported message
    	AsyncNotifyL(KErrNotSupported, CGlxAsyncNotifier::EFindAll);  
    }
// ----------------------------------------------------------------------------
// FindAllL
// ----------------------------------------------------------------------------
//
CMPXMedia* CGlxTestCollectionPlugin::FindAllSyncL(const CMPXMedia& /*aMedia*/, const TArray<TMPXAttribute>& /*aAttrs*/)
    {
    return NULL;
    }
// ----------------------------------------------------------------------------
// FindAllL
// ----------------------------------------------------------------------------
//
TCollectionCapability CGlxTestCollectionPlugin::GetCapabilities()
	{
	return 0;
	}
	
void CGlxTestCollectionPlugin::GenerateTitleL( TMPXGeneralCategory /*aCategory*/,
                             const TDesC& /*aBaseTitle*/ )
	{
	
	}
                             
HBufC* CGlxTestCollectionPlugin::GenerateTitleSyncL( TMPXGeneralCategory /*aCategory*/,
                                   const TDesC& /*aBaseTitle*/ )
	{
	return NULL;
	}

// -----------------------------------------------------------------------------
// Item
// -----------------------------------------------------------------------------
//
CGlxTestCollectionPlugin::CItem* CGlxTestCollectionPlugin::Item(const TGlxMediaId& aItemId) const
	{
	TInt count = _iItemDBHackAlbums.Count();
	for (TInt i = 0; i < count; i++)
		{
		CItem* item = _iItemDBHackAlbums[i];
		if (item->iId == aItemId)
			{
			return item;
			}
		}
		
	count = _iItemDBHackContent.Count();
	for (TInt i = 0; i < count; i++)
		{
		CItem* item = _iItemDBHackContent[i];
		if (item->iId == aItemId)
			{
			return item;
			}
		}
				
	return NULL; 
	}

// -----------------------------------------------------------------------------
// HandleThumbnailRequestCompleteL
// From MGlxtnThumbnailCreatorObserver
// -----------------------------------------------------------------------------
//
void CGlxTestCollectionPlugin::HandleThumbnailRequestCompleteL(const TGlxMediaId& /*aId*/, 
		TGlxThumbnailQuality /*aQuality*/, TInt aErrorCode) 
	{
    __ASSERT_ALWAYS(iMedia, Panic(EGlxPanicIllegalState));

    iMedia->SetTObjectValueL(KMPXMediaGeneralThumbnail1, 1);

    iObs->HandleMedia(iMedia, aErrorCode);
    delete iMedia;
    iMedia = NULL;
	}

// -----------------------------------------------------------------------------
// ThumbnailFetchComplete
// Notifies that a thumbnail for a given item is available, or that
// thumbnail generation failed.
// -----------------------------------------------------------------------------
//
void CGlxTestCollectionPlugin::ThumbnailFetchComplete(
    const TGlxMediaId& aItemId, TGlxThumbnailQuality aQuality, TInt aErrorCode)
    {
    TRAP_IGNORE(HandleThumbnailRequestCompleteL(aItemId, aQuality, aErrorCode));
    }

// -----------------------------------------------------------------------------
// ThumbnailDeletionComplete
// Notifies that deletion of thumbnails for a given item is complete, or
// the operation failed.
// -----------------------------------------------------------------------------
//
void CGlxTestCollectionPlugin::ThumbnailDeletionComplete(
                                const TGlxMediaId& /*aItemId*/, TInt /*aErrorCode*/)
    {
    // TO DO
    }

// -----------------------------------------------------------------------------
// FilterAvailableComplete
// Notifies that filtering items with available thumbnails from a list is
// complete, or the operation failed.
/// -----------------------------------------------------------------------------
//
void CGlxTestCollectionPlugin::FilterAvailableComplete(
                const RArray<TGlxMediaId>& /*aIdArray*/, TInt /*aErrorCode*/ )
    {
    // TO DO
    }

// -----------------------------------------------------------------------------
// FetchFileInfoL
// Called by a task to request the URI of an item.  The URI is returned
// asynchronously.
// -----------------------------------------------------------------------------
//
void CGlxTestCollectionPlugin::FetchFileInfoL(CGlxtnFileInfo* aInfo,
                        const TGlxMediaId& aItemId, TRequestStatus* aStatus)
    {
    CItem* item = Item(aItemId);
    __ASSERT_DEBUG(item != NULL, Panic(EGlxPanicLogicError));

    aInfo->SetFilePathL(*item->iFilename);

    *aStatus = KRequestPending;
    User::RequestComplete(aStatus, KErrNone);
    }

// -----------------------------------------------------------------------------
// CancelFetchUri
// Cancel the request for an URI.
// -----------------------------------------------------------------------------
//
void CGlxTestCollectionPlugin::CancelFetchUri(const TGlxMediaId& /*aItemId*/)
    {
    // No implementation
    }

// -----------------------------------------------------------------------------
// ThumbnailStorage
// Get the thumbnail storage used by this client.
// -----------------------------------------------------------------------------
//
MGlxtnThumbnailStorage* CGlxTestCollectionPlugin::ThumbnailStorage()
    {
    // TO DO
    return NULL;
    }

// -----------------------------------------------------------------------------
// RW - TEMP AddItem
// -----------------------------------------------------------------------------
//
void CGlxTestCollectionPlugin::AddItemL(const TDesC& aFileName, 
	              const TDesC& aTitle, 
	              RPointerArray<CItem>& aDatabase, 
	              TTime aDateTime , 
	              TInt  aFileSize ,
	              const TDesC& aDrive ) 
	{
	CItem* item = new (ELeave) CItem();
	CleanupStack::PushL(item);
	item->iId = TGlxMediaId(iItemAddedId++);
	item->SetFilenameL(aFileName);
	item->SetTitleL(aTitle);
	item->iDateTime = aDateTime;
	item->iFileSize = aFileSize;
	item->SetDriveL(aDrive);
	aDatabase.AppendL(item); 
	CleanupStack::Pop(); // item	
	}
	
// -----------------------------------------------------------------------------
// ItemL
// -----------------------------------------------------------------------------
CGlxTestCollectionPlugin::CItem* CGlxTestCollectionPlugin::ItemL(const CMPXCollectionPath& aPath, TInt aIndex) const
	{
	CItem* item = NULL;

	if (aPath.Levels() == 3)
		{
		item = _iItemDBHackContent[aIndex];
		__ASSERT_DEBUG(item != NULL, Panic(EGlxPanicLogicError));
		}
	else if (aPath.Levels() == 2)
		{
		item = _iItemDBHackAlbums[aIndex];
		__ASSERT_DEBUG(item != NULL, Panic(EGlxPanicLogicError));
		}
	else if (aPath.Levels() == 1)
		{
		item = iSelf;
		}
	else
		{
		__DEBUG_ONLY(Panic(EGlxPanicDebugUnexpectedError));
		User::Leave(KErrNotSupported);
		}

	return item;
	}

// -----------------------------------------------------------------------------
// PopulateMediaWithItemL
// -----------------------------------------------------------------------------
TInt CGlxTestCollectionPlugin::PopulateMediaWithItemL(CMPXMedia*& aMedia, const CItem* aItem, const TArray<TMPXAttribute>& aAttrs)
	{
	TInt bitmapHandle(0);
	if ( aItem )
	        {
		for ( TInt i = 0; i < aAttrs.Count(); i++ )
			{
			if ( aAttrs[i] == KMPXMediaGeneralId )
				{
				aMedia->SetTObjectValueL(KMPXMediaGeneralId, aItem->iId);
				}
			else if ( aAttrs[i] == KMPXMediaGeneralUri )
				{
				aMedia->SetTextValueL(KMPXMediaGeneralUri, *aItem->iFilename);
				}
			else if ( aAttrs[i] == KMPXMediaGeneralTitle )
				{
				aMedia->SetTextValueL(KMPXMediaGeneralTitle, *aItem->iTitle);
				}
			else if ( aAttrs[i] == KMPXMediaGeneralDate )
				{
				aMedia->SetTObjectValueL(KMPXMediaGeneralDate, aItem->iDateTime);
				}
			else if ( aAttrs[i] == KMPXMediaGeneralSize )
				{
				aMedia->SetTObjectValueL(KMPXMediaGeneralSize, aItem->iFileSize);
				}
			else if ( aAttrs[i] == KMPXMediaGeneralDrive )
				{
				aMedia->SetTextValueL(KMPXMediaGeneralDrive, *aItem->iDrive);
				}
			else if ( KGlxMediaIdBitmapHandle == aAttrs[i].ContentId() )
				{
				bitmapHandle = aAttrs[i].AttributeId();
				}
			}
		}

	return bitmapHandle;
	}

// -----------------------------------------------------------------------------
// PopulateMediaWithArrayL
// -----------------------------------------------------------------------------
void CGlxTestCollectionPlugin::PopulateMediaWithArrayL(CMPXMedia*& aMedia, CMPXMediaArray* aArray)
	{
	TInt arrayCount = aArray->Count();
	if (arrayCount)
		{
		aMedia->SetTObjectValueL(KMPXMediaArrayCount, arrayCount);
		aMedia->SetCObjectValueL(KMPXMediaArrayContents, aArray);
		}
	}

// End of file
