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
* Description:   Media lists test collection plugin implementation
*
*/



// INCLUDE FILES

#include "glxmedialiststestcollectionplugin.h"
#include "glxmedialiststestcollectionplugin.hrh"
#include "glxpanic.h"
#include "glxtnthumbnailrequest.h"
#include "glxtnthumbnailcreator.h"
#include <glxtnfileinfo.h>

#include <e32math.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxcmn.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionmessagedefs.h>
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
    CMPXMedia* entry=CMPXMedia::NewL(supportedIds.Array());
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
CGlxMediaListsTestCollectionPlugin* CGlxMediaListsTestCollectionPlugin::NewL(TAny* aObs)
    {
    CGlxMediaListsTestCollectionPlugin* o = new (ELeave) CGlxMediaListsTestCollectionPlugin(
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
CGlxMediaListsTestCollectionPlugin::~CGlxMediaListsTestCollectionPlugin()
    {
    delete iTimer;

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
CGlxMediaListsTestCollectionPlugin::CGlxMediaListsTestCollectionPlugin(
    MMPXCollectionPluginObserver* aObs)
    {
    iObs = aObs;
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CGlxMediaListsTestCollectionPlugin::ConstructL()
    {
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(RFbsSession::Connect(iFs));

    iThumbnailCreator = CGlxtnThumbnailCreator::InstanceL();
    iTimer = CTestTimer::NewL(this);

    iSelf = new (ELeave) CItem;
    iSelf->iId = TGlxMediaId(KGlxMediaListsTestCollectionImplementationUid);
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
void CGlxMediaListsTestCollectionPlugin::CommandL(TMPXCollectionCommand /*aCmd*/, TInt /*aArg*/)
    {
    }

// ----------------------------------------------------------------------------
// Executes a command on the selected collection
// ----------------------------------------------------------------------------
//
void CGlxMediaListsTestCollectionPlugin::CommandL(const CMPXCommand& /*aCmd*/)
    {
    }

// ----------------------------------------------------------------------------
// Navigates to the given path
// ----------------------------------------------------------------------------
//
void CGlxMediaListsTestCollectionPlugin::OpenL(const CMPXCollectionPath& aPath,
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

		StartEvents();
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
void CGlxMediaListsTestCollectionPlugin::MediaL(const CMPXCollectionPath& aPath,
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
void CGlxMediaListsTestCollectionPlugin::AddL(const CMPXMedia& aNewMedia)
    {
    TInt error = KErrNotSupported;
    TInt idAdded = iItemAddedId;

    TMPXGeneralCategory category
            = *aNewMedia.Value<TMPXGeneralCategory>(KMPXMediaGeneralCategory);

    if ( EMPXAlbum == category )
        {
        const TDesC& title = aNewMedia.ValueText(KMPXMediaGeneralTitle);

	InsertItemL(KNullDesC, title, _iItemDBHackAlbums);
        error = KErrNone;
        }
    else if ( EMPXImage == category )
	{
	InsertItemL(_L("c:\\newitem1.jpg"), _L("newitem"), _iItemDBHackContent, TTime(_L("20061105:092527.")), 125371, _L("E:"));
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
		CMPXMessage* message = CMPXMessage::NewL();
		CleanupStack::PushL(message);

		message->SetTObjectValueL<TMPXMessageId>(KMPXMessageGeneralId, KMPXMessageIdItemChanged);
		message->SetTObjectValueL<TUid>(KMPXMessageCollectionId, TUid::Uid(KGlxMediaListsTestCollectionImplementationUid));
		message->SetTObjectValueL<TMPXChangeEventType>(KMPXMessageChangeEventType, EMPXItemInserted);
		message->SetTObjectValueL<TMPXGeneralCategory>(KMPXMessageMediaGeneralCategory, category);
		message->SetTObjectValueL<TMPXItemId>(KMPXMessageMediaGeneralId, TMPXItemId(idAdded));

		iObs->HandleMessage(*message);

		CleanupStack::PopAndDestroy(message);

/*		    TMPXCollectionChangeEvent event = {TUid::Uid(KGlxMediaListsTestCollectionImplementationUid), idAdded, EMcItemInserted};
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
void CGlxMediaListsTestCollectionPlugin::RemoveL(const CMPXCollectionPath& /*aPath*/)
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
void CGlxMediaListsTestCollectionPlugin::RemoveL(const CMPXMedia& aOldMedia)
    {
    TInt error = KErrNotSupported;
    TInt idRemoved = iItemAddedId - 1;

    TMPXGeneralCategory category
            = *aOldMedia.Value<TMPXGeneralCategory>(KMPXMediaGeneralCategory);

    if ( EMPXAlbum == category )
        {
        idRemoved = *aOldMedia.Value<TMPXGeneralCategory>(KMPXMediaGeneralId);

        RemoveItemL(TGlxMediaId(idRemoved), _iItemDBHackAlbums);
        error = KErrNone;
        }
    else if ( EMPXImage == category )
	{
	RemoveItemL(TGlxMediaId(idRemoved), _iItemDBHackContent);
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
		CMPXMessage* message = CMPXMessage::NewL();
		CleanupStack::PushL(message);

		message->SetTObjectValueL<TMPXMessageId>(KMPXMessageGeneralId, KMPXMessageIdItemChanged);
		message->SetTObjectValueL<TUid>(KMPXMessageCollectionId, TUid::Uid(KGlxMediaListsTestCollectionImplementationUid));
		message->SetTObjectValueL<TMPXChangeEventType>(KMPXMessageChangeEventType, EMPXItemDeleted);
		message->SetTObjectValueL<TMPXGeneralCategory>(KMPXMessageMediaGeneralCategory, category);
		message->SetTObjectValueL<TMPXItemId>(KMPXMessageMediaGeneralId, TMPXItemId(idRemoved));

		iObs->HandleMessage(*message);

		CleanupStack::PopAndDestroy(message);
            }
        }

    // Currently returns dummy data for testing only
/*    TMPXCollectionChangeEvent event = {TUid::Uid(KGlxMediaListsTestCollectionImplementationUid), 0, EMcItemDeleted};
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
void CGlxMediaListsTestCollectionPlugin::SetL(const CMPXMedia& /* aMedia */)
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
void CGlxMediaListsTestCollectionPlugin::FindAllL(const CMPXMedia& /*aMedia*/, const TArray<TMPXAttribute>& /*aAttrs*/)
    {
    	// Currently not supported. Get it to complete with a not supported message
    	AsyncNotifyL(KErrNotSupported, CGlxAsyncNotifier::EFindAll);  
    }
// ----------------------------------------------------------------------------
// FindAllL
// ----------------------------------------------------------------------------
//
CMPXMedia* CGlxMediaListsTestCollectionPlugin::FindAllSyncL(const CMPXMedia& /*aMedia*/, const TArray<TMPXAttribute>& /*aAttrs*/)
    {
    return NULL;
    }
// ----------------------------------------------------------------------------
// FindAllL
// ----------------------------------------------------------------------------
//
TCollectionCapability CGlxMediaListsTestCollectionPlugin::GetCapabilities()
	{
	return 0;
	}
	
void CGlxMediaListsTestCollectionPlugin::GenerateTitleL( TMPXGeneralCategory /*aCategory*/,
                             const TDesC& /*aBaseTitle*/ )
	{
	
	}
                             
HBufC* CGlxMediaListsTestCollectionPlugin::GenerateTitleSyncL( TMPXGeneralCategory /*aCategory*/,
                                   const TDesC& /*aBaseTitle*/ )
	{
	return NULL;
	}

// -----------------------------------------------------------------------------
// Item
// -----------------------------------------------------------------------------
//
CGlxMediaListsTestCollectionPlugin::CItem* CGlxMediaListsTestCollectionPlugin::Item(const TGlxMediaId& aItemId) const
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
void CGlxMediaListsTestCollectionPlugin::HandleThumbnailRequestCompleteL(const TGlxMediaId& /*aId*/, 
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
void CGlxMediaListsTestCollectionPlugin::ThumbnailFetchComplete(
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
void CGlxMediaListsTestCollectionPlugin::ThumbnailDeletionComplete(
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
void CGlxMediaListsTestCollectionPlugin::FilterAvailableComplete(
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
void CGlxMediaListsTestCollectionPlugin::FetchFileInfoL(CGlxtnFileInfo* aInfo,
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
void CGlxMediaListsTestCollectionPlugin::CancelFetchUri(const TGlxMediaId& /*aItemId*/)
    {
    // No implementation
    }

// -----------------------------------------------------------------------------
// ThumbnailStorage
// Get the thumbnail storage used by this client.
// -----------------------------------------------------------------------------
//
MGlxtnThumbnailStorage* CGlxMediaListsTestCollectionPlugin::ThumbnailStorage()
    {
    // TO DO
    return NULL;
    }

// -----------------------------------------------------------------------------
// RW - TEMP AddItem
// -----------------------------------------------------------------------------
//
void CGlxMediaListsTestCollectionPlugin::AddItemL(const TDesC& aFileName, 
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
CGlxMediaListsTestCollectionPlugin::CItem* CGlxMediaListsTestCollectionPlugin::ItemL(const CMPXCollectionPath& aPath, TInt aIndex) const
	{
	CItem* item = NULL;
	TIdentityRelation<CItem> match(&MatchById);

	if (aPath.Levels() == 3)
		{
		// Find item
		CItem* mediaToCompare = new (ELeave) CItem();
		CleanupStack::PushL(mediaToCompare);

		mediaToCompare->iId = TGlxMediaId(aPath.IdOfIndex(aIndex));
		TInt index = _iItemDBHackContent.Find(mediaToCompare, match);

		CleanupStack::PopAndDestroy(mediaToCompare);

		if (index != KErrNotFound)
			{
			item = _iItemDBHackContent[index];
			__ASSERT_DEBUG(item != NULL, Panic(EGlxPanicLogicError));
			}
		}
	else if (aPath.Levels() == 2)
		{
		// Find item
		CItem* mediaToCompare = new (ELeave) CItem();
		CleanupStack::PushL(mediaToCompare);

		mediaToCompare->iId = TGlxMediaId(aPath.IdOfIndex(aIndex));
		TInt index = _iItemDBHackAlbums.Find(mediaToCompare, match);

		CleanupStack::PopAndDestroy(mediaToCompare);

		if (index != KErrNotFound)
			{
			item = _iItemDBHackAlbums[index];
			__ASSERT_DEBUG(item != NULL, Panic(EGlxPanicLogicError));
			}
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
TInt CGlxMediaListsTestCollectionPlugin::PopulateMediaWithItemL(CMPXMedia*& aMedia, const CItem* aItem, const TArray<TMPXAttribute>& aAttrs)
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
void CGlxMediaListsTestCollectionPlugin::PopulateMediaWithArrayL(CMPXMedia*& aMedia, CMPXMediaArray* aArray)
	{
	TInt arrayCount = aArray->Count();
	if (arrayCount)
		{
		aMedia->SetTObjectValueL(KMPXMediaArrayCount, arrayCount);
		aMedia->SetCObjectValueL(KMPXMediaArrayContents, aArray);
		}
	}

void CGlxMediaListsTestCollectionPlugin::InsertItemL(const TDesC& aFileName, 
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
	TInt databaseCount = aDatabase.Count();
	TInt position = Math::Random() % (databaseCount + 1);
	aDatabase.InsertL(item, position); 
	CleanupStack::Pop(); // item	
	}

void CGlxMediaListsTestCollectionPlugin::RemoveItemL(const TGlxMediaId& aId, RPointerArray<CItem>& aDatabase)
	{
	TIdentityRelation<CItem> match(&MatchById);
	CItem* mediaToCompare = new (ELeave) CItem();
	CleanupStack::PushL(mediaToCompare);
	mediaToCompare->iId = TGlxMediaId(aId);
	TInt index = aDatabase.Find(mediaToCompare, match);
	delete aDatabase[index];
	aDatabase.Remove(index);
	CleanupStack::PopAndDestroy(mediaToCompare);
	}

TBool CGlxMediaListsTestCollectionPlugin::MatchById(const CItem& aMedia1, const CItem& aMedia2)
	{
	return aMedia1.iId == aMedia2.iId;
	}

void CGlxMediaListsTestCollectionPlugin::StartEvents()
	{
	if (!iTimer->IsActive())
		{
		iTimer->After(2000000);
		}
	}

CGlxMediaListsTestCollectionPlugin::CTestTimer* CGlxMediaListsTestCollectionPlugin::CTestTimer::NewL(CMPXCollectionPlugin* aPlugin)
	{
	CTestTimer* self = new (ELeave) CTestTimer(aPlugin);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CGlxMediaListsTestCollectionPlugin::CTestTimer::CTestTimer(CMPXCollectionPlugin* aPlugin) :
	CTimer(CActive::EPriorityStandard),
	iPlugin(aPlugin),
	iNextEvent(EAddItem)
	{
	}

CGlxMediaListsTestCollectionPlugin::CTestTimer::~CTestTimer()
	{
	Cancel();
	}

void CGlxMediaListsTestCollectionPlugin::CTestTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CGlxMediaListsTestCollectionPlugin::CTestTimer::RunL()
	{
	CMPXMedia* media = CMPXMedia::NewL();
	CleanupStack::PushL(media);

	media->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXImage);

	switch (iNextEvent)
		{
		case EAddItem:
			iPlugin->AddL(*media);
			iNextEvent = ERemoveItem;
			break;
		case ERemoveItem:
			iPlugin->RemoveL(*media);
			iNextEvent = EAddItem;
			break;
		default:
			break;
		}

	CleanupStack::PopAndDestroy(media);

	After(2000000);
	}

void CGlxMediaListsTestCollectionPlugin::CTestTimer::DoCancel()
	{
	}

TInt CGlxMediaListsTestCollectionPlugin::CTestTimer::RunError(TInt aError)
	{
	return aError;
	}

// End of file
