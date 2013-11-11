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

#include "glxalbumcollectionplugin.h"

#include <fbs.h>
#include <glxpanic.h>
#include <glxtnfileinfo.h>
#include <glxtnthumbnailcreator.h>
#include <glxtnthumbnailrequest.h>
#include <mpxcmn.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxmessageprogressdefs.h>
#include <mpxmessageprogressdefs.h>

#include "glxalbumcollectionplugin.hrh"

class CGlxAcpProgressNotifier : public CBase
    {
public:
    CGlxAcpProgressNotifier(TInt aTotalCount, TUint32 aSessionId, MMPXCollectionPluginObserver* aObs) 
            : iTotalCount(aTotalCount), iCount(1), iSessionId(aSessionId), iObs(aObs)
        {      
        }
        
    ~CGlxAcpProgressNotifier() 
        {
        delete iPeriodic;
        }
        
    void ConstructL() 
        {
        iPeriodic = CPeriodic::NewL(EPriorityLow);
        iPeriodic->Start(10000, 10000, TCallBack(CGlxAcpProgressNotifier::Callback, this));
        }
        
    static TInt Callback(TAny* aPtr) 
        {
        static_cast<CGlxAcpProgressNotifier*>(aPtr)->Callback();
        return KErrNone;
        }
        
    void Callback() 
        {
        CMPXMessage* msg = CMPXMessage::NewL();
        CleanupStack::PushL(msg);
        // message id
        msg->SetTObjectValueL<TInt>(KMPXMessageGeneralId, KMPXMessageContentIdProgress); 
        // current count
        msg->SetTObjectValueL<TInt>(
            TMPXAttribute(KMPXMessageContentIdProgress, EMPXMessageProgressCurrentCount),
            iCount); 
        // total count
        msg->SetTObjectValueL<TInt>(
            TMPXAttribute(KMPXMessageContentIdProgress, EMPXMessageProgressTotalCount),
            iTotalCount); 
        // session id
        msg->SetTObjectValueL<TUint32>(KMPXCommandGeneralSessionId, iSessionId); 

        // Enable this to send an error back
/*        if (iCount == 20) 
            {
            msg->SetTObjectValueL<TUint32>(
                TMPXAttribute(KMPXMessageContentIdProgress, EMPXMessageError),
                KErrAlreadyExists); 
            iPeriodic->Cancel();
            }*/
            
        iObs->HandleMessage(*msg);
        
        CleanupStack::PopAndDestroy(msg);
        
        iCount++;
        if (iCount > iTotalCount) 
            {
            iPeriodic->Cancel();
            }        
        }
    
    TInt iTotalCount;
    TInt iCount;
    TUint32 iSessionId;
    MMPXCollectionPluginObserver* iObs;
    CPeriodic* iPeriodic;
    };


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
    CMPXMedia* entry= CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PushL(entry);
    entry->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, aId.Value());
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
CGlxAlbumCollectionPlugin* CGlxAlbumCollectionPlugin::NewL(TAny* aObs)
    {
    CGlxAlbumCollectionPlugin* o = new (ELeave) CGlxAlbumCollectionPlugin(
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
CGlxAlbumCollectionPlugin::~CGlxAlbumCollectionPlugin()
    {
    delete iProgressNotifier;
    if (iThumbnailCreator) 
        {
        iThumbnailCreator->Close(NULL);
        }

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
CGlxAlbumCollectionPlugin::CGlxAlbumCollectionPlugin(
    MMPXCollectionPluginObserver* aObs)
    {
    iObs = aObs;
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CGlxAlbumCollectionPlugin::ConstructL()
    {
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(RFbsSession::Connect(iFs));

    iThumbnailCreator = CGlxtnThumbnailCreator::InstanceL();

    iSelf = new (ELeave) CItem;
    iSelf->iId = TGlxMediaId(KGlxAlbumCollectionImplementationUid);
    iSelf->SetFilenameL(KNullDesC);
    iSelf->SetTitleL(_L("Albums"));

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
        AddItemL(_L("c:\\newitem1.jpg"), _L("newitem1"), _iItemDBHackContent, TTime(_L("20061105:083546.")), 123456, _L("C:"));
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
void CGlxAlbumCollectionPlugin::CommandL(TMPXCollectionCommand /*aCmd*/, TInt /*aArg*/)
    {
    }
// ----------------------------------------------------------------------------
// Executes a command on the selected collection
// ----------------------------------------------------------------------------
//
void CGlxAlbumCollectionPlugin::CommandL(CMPXCommand& aCmd)
    {
    TInt cmdId = *aCmd.Value<TInt>(KMPXCommandGeneralId);
    TUint32 sessionId = *aCmd.Value<TUint32>(KMPXCommandGeneralSessionId);
            
    // Send progress back
    delete iProgressNotifier;
    iProgressNotifier = NULL;
    iProgressNotifier = new (ELeave) CGlxAcpProgressNotifier(100, sessionId, iObs);
    iProgressNotifier->ConstructL();
    }


// ----------------------------------------------------------------------------
// Navigates to the given path
// ----------------------------------------------------------------------------
//
void CGlxAlbumCollectionPlugin::OpenL(const CMPXCollectionPath& aPath,
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

	//iObs->HandleOpen(*entries, KErrNone);

    AsyncNotifyL(iOpenEntries, KErrNone, CGlxAsyncNotifier::EOpen);

	// entries will be destroyed after the async call completes
    CleanupStack::PopAndDestroy(&supportedIds);        
    }

// ----------------------------------------------------------------------------
// Extended properties of the current file (async)
// ----------------------------------------------------------------------------
//
void CGlxAlbumCollectionPlugin::MediaL(const CMPXCollectionPath& aPath,
		const TArray<TMPXAttribute>& aAttrs,
                        const TArray<TCapability>& /*aCaps*/,
                        CMPXAttributeSpecs* /*aSpecs*/)
    {
    __ASSERT_ALWAYS(!iMedia, Panic(EGlxPanicIllegalState));

    CItem* item = NULL;
 	if (aPath.Levels() == 3) 
 		{
	    // Find item 
	    item = _iItemDBHackContent[aPath.Index()];
	    __ASSERT_DEBUG(item != NULL, Panic(EGlxPanicLogicError));
 		}
 	else if (aPath.Levels() == 2)
 		{
	    // Find item 
	    item = _iItemDBHackAlbums[aPath.Index()];
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

    TInt error = KErrNone;
    TInt bitmapHandle = 0;
    RArray<TInt> supportedIds;
    CleanupClosePushL(supportedIds);
    iMedia = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);

    if ( item )
        {
        for ( TInt i = 0; i < aAttrs.Count(); i++ )
            {
            if ( aAttrs[i] == KMPXMediaGeneralId )
                {
                iMedia->SetTObjectValueL(KMPXMediaGeneralId, item->iId);
                }
            else if ( aAttrs[i] == KMPXMediaGeneralUri )
                {
                iMedia->SetTextValueL(KMPXMediaGeneralUri, *item->iFilename);
                }
            else if ( aAttrs[i] == KMPXMediaGeneralTitle )
                {
                iMedia->SetTextValueL(KMPXMediaGeneralTitle, *item->iTitle);
                }
            else if ( KGlxMediaIdBitmapHandle == aAttrs[i].ContentId() )
                {
                bitmapHandle = aAttrs[i].AttributeId();
                }
            else if ( aAttrs[i] == KMPXMediaGeneralDate )
                {
                iMedia->SetTObjectValueL(KMPXMediaGeneralDate, item->iDateTime);
                }
            else if ( aAttrs[i] == KMPXMediaGeneralSize )
                {
                iMedia->SetTObjectValueL(KMPXMediaGeneralSize, item->iFileSize);
                }
            else if ( aAttrs[i] == KMPXMediaGeneralDrive )
                {
                iMedia->SetTextValueL(KMPXMediaGeneralDrive, *item->iDrive);
                }
            else
                {
//                error = KErrNotSupported;
                }
            }
        }
    else
        {
        error = KErrNotFound;
        }

    if ( (KErrNone == error) && bitmapHandle )
        {
        // Get thumbnail for the item
        TGlxThumbnailRequest request;
        request.iId = item->iId;
    	request.iSizeClass = TSize(60, 45);
    	request.iPriorityMode = TGlxThumbnailRequest::EPrioritizeSpeed;
    	request.iBitmapHandle = bitmapHandle;
        iThumbnailCreator->FetchThumbnailL(request,
                                           *this);	
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
void CGlxAlbumCollectionPlugin::AddL(const CMPXMedia& aNewMedia)
    {
    TInt error = KErrNotSupported;
    //TInt idAdded = iItemAddedId;

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
/*		    TMPXCollectionChangeEvent event = {TUid::Uid(KGlxAlbumCollectionImplementationUid), idAdded, EMcItemInserted};
		    RArray<TMPXCollectionChangeEvent> events;
		    CleanupClosePushL(events);
		    events.AppendL(event);
		    iObs->HandleChange(events.Array());
		    CleanupStack::PopAndDestroy(&events);
*/            }
        }
    }

// ----------------------------------------------------------------------------
// Remove an item or items under a group from the collection
// ----------------------------------------------------------------------------
//
void CGlxAlbumCollectionPlugin::RemoveL(const CMPXCollectionPath& /*aPath*/)
    {
    }

// ----------------------------------------------------------------------------
// Remove an item or items under a group from the collection
// ----------------------------------------------------------------------------
//
void CGlxAlbumCollectionPlugin::RemoveL(const CMPXMedia& /*aMedia*/)
    {
    }

// ----------------------------------------------------------------------------
// Sets/updates the media for the item
// ----------------------------------------------------------------------------
//
void CGlxAlbumCollectionPlugin::SetL(
    const CMPXMedia& aMedia)
    {
    (void)aMedia;
    }

// ----------------------------------------------------------------------------
// FindAllL
// ----------------------------------------------------------------------------
//
void CGlxAlbumCollectionPlugin::FindAllL(const CMPXMedia& /*aMedia*/, const TArray<TMPXAttribute>& /*aAttrs*/)
    {
    	// Currently not supported. Get it to complete with a not supported message
    	AsyncNotifyL(KErrNotSupported, CGlxAsyncNotifier::EFindAll);  
    }
// ----------------------------------------------------------------------------
// FindAllL
// ----------------------------------------------------------------------------
//
CMPXMedia* CGlxAlbumCollectionPlugin::FindAllSyncL(const CMPXMedia& /*aMedia*/, const TArray<TMPXAttribute>& /*aAttrs*/)
    {
    return NULL;
    }
// ----------------------------------------------------------------------------
// FindAllL
// ----------------------------------------------------------------------------
//
TCollectionCapability CGlxAlbumCollectionPlugin::GetCapabilities()
	{
	return 0;
	}

void CGlxAlbumCollectionPlugin::GenerateTitleL( TMPXGeneralCategory /*aCategory*/,
                             const TDesC& /*aBaseTitle*/ )
	{
	
	}
                             
HBufC* CGlxAlbumCollectionPlugin::GenerateTitleSyncL( TMPXGeneralCategory /*aCategory*/,
                                   const TDesC& /*aBaseTitle*/ )
	{
	return NULL;
	}

// -----------------------------------------------------------------------------
// Item
// -----------------------------------------------------------------------------
//
CGlxAlbumCollectionPlugin::CItem* CGlxAlbumCollectionPlugin::Item(const TGlxMediaId& aItemId) const
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
void CGlxAlbumCollectionPlugin::HandleThumbnailRequestCompleteL(const TGlxMediaId& /*aId*/, 
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
void CGlxAlbumCollectionPlugin::ThumbnailFetchComplete(
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
void CGlxAlbumCollectionPlugin::ThumbnailDeletionComplete(
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
void CGlxAlbumCollectionPlugin::FilterAvailableComplete(
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
void CGlxAlbumCollectionPlugin::FetchFileInfoL(CGlxtnFileInfo* aInfo,
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
void CGlxAlbumCollectionPlugin::CancelFetchUri(const TGlxMediaId& /*aItemId*/)
    {
    // No implementation
    }

// -----------------------------------------------------------------------------
// ThumbnailStorage
// Get the thumbnail storage used by this client.
// -----------------------------------------------------------------------------
//
MGlxtnThumbnailStorage* CGlxAlbumCollectionPlugin::ThumbnailStorage()
    {
    // TO DO
    return NULL;
    }

// -----------------------------------------------------------------------------
// RW - TEMP AddItem
// -----------------------------------------------------------------------------
//
void CGlxAlbumCollectionPlugin::AddItemL(const TDesC& aFileName, 
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

// End of file
