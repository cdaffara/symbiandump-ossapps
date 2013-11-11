/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Manager of media item cache
*
*/




#include "glxcachemanager.h"

#include <mpxcollectionutility.h>
#include <glxtracer.h>
#include <glxthumbnailattributeinfo.h>
#include <glxthumbnail.h>
#include <glxsingletonstore.h>
#include <glximageviewermanager.h>

#include "glxcache.h"
#include "glxerrormanager.h"
#include "glxgarbagecollector.h"
#include "glxmedialist.h"
#include <hal.h>
#include <glxcollectionpluginimageviewer.hrh>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxmediadrmdefs.h>
#include <f32file.h>
#include <lbsposition.h>
#include <apgcli.h>
#include <caf/content.h>
#include <caf/attributeset.h>
#include <DRMHelper.h>

#ifdef USE_S60_TNM
#include <thumbnaildata.h>
const TInt KMaxGridThumbnailWidth = 200;
_LIT(KFileIdentifier, ":\\");
#endif

_LIT(KDefaultType, "image/jpg");
/// How long to wait before rechecking for cleared temporary errors
/// @todo Find optimal value for this
const TInt KGlxTemporaryErrorRecheckPeriodInSeconds = 5;

const TInt KGlxLowerMemoryLimitForCleanUp = 15000000;

const TInt KGlxUpperMemoryLimitForCleanUp = 25000000;

//Defining number of pages to be flushed in critical low memory conditions.
const TInt KGlxNoOfPagesToFlushInCriticalLowMemory = 4;
// -----------------------------------------------------------------------------
// InstanceL
// -----------------------------------------------------------------------------
//
CGlxCacheManager* CGlxCacheManager::InstanceL()
	{
	TRACER("CGlxCacheManager::InstanceL");
	
	return CGlxSingletonStore::InstanceL(&NewL);
	}

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CGlxCacheManager* CGlxCacheManager::NewL()
    {
    TRACER("CGlxCacheManager::NewL");
    
    CGlxCacheManager* self = new( ELeave ) CGlxCacheManager();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// Close
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::Close()
	{
	TRACER("CGlxCacheManager::Close");
	
	CGlxSingletonStore::Close(this);
	}

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxCacheManager::CGlxCacheManager()
    {
    TRACER("CGlxCacheManager::Default Constructor");
    
    }
    
// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::ConstructL()
    {
    TRACER("CGlxCacheManager::ConstructL");
    
    iGarbageCollector = CGlxGarbageCollector::NewL( iCaches );
    iTempErrorTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    iMaintainCacheCallback = new ( ELeave )
	    CAsyncCallBack( TCallBack( MaintainCacheL, this ), CActive::EPriorityStandard );
	    
#ifdef USE_S60_TNM
    iTnEngine = CThumbnailManager::NewL( *this);
    iTnEngine->SetDisplayModeL( EColor64K );
#endif
    }
        
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxCacheManager::~CGlxCacheManager()
	{
	TRACER("CGlxCacheManager::Destructor");
	
 	iObserverList.ResetAndDestroy();
 	iCaches.ResetAndDestroy();
 	delete iTempThumbnail;
 	iRequestedItemIds.Reset();
 	iRequestedAttrs.Reset();
 	iRequestedItemIndexes.Reset();
    if(iReader)
        {
        delete iReader;
        }

 	delete iGarbageCollector;

    if (iTempErrorTimer)
        {
		iTempErrorTimer->Cancel();	
		delete iTempErrorTimer;
        }

#ifdef USE_S60_TNM
    GLX_DEBUG2("CGlxCacheManager::~CGlxCacheManager() TN Ids count()=%d",
										 iThumbnailRequestIds.Count());
    for (TInt i = 0; i < iThumbnailRequestIds.Count(); i++)
        {
        iTnEngine->CancelRequest(iThumbnailRequestIds[i].iId);
        }
	iThumbnailRequestIds.Reset();
    delete iTnEngine;
    delete iMPXMedia;
#endif
    
    delete iMaintainCacheCallback;
	}

// -----------------------------------------------------------------------------
// HandleWindowChangedL
// From MVieMediaItemListManager
// The list calls this function to hint the list manager, that the items 
// the list contains might not all be loaded, and it would be nice if the list
// manager could load the missing items to the cache.
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::HandleWindowChangedL(CGlxMediaList* /*aList*/) 
	{
	TRACER("CGlxCacheManager::HandleWindowChangedL");
	
	GLX_LOG_INFO("---- MGallery - CGlxCacheManager::HandleWindowChangedL()---- ");
    
    iMaintainCacheCallback->CallBack();
    // Its Better if we are not Cleaning Cache here; Instaed Can Do inside MainTainCache
    //MaintainCacheL();
    //iGarbageCollector->Cleanup();
    }

// -----------------------------------------------------------------------------
// CancelPreviousRequest
// This API cancels the pending attributes and thumbnail request  
// when the media item in focus is NULL
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::CancelPreviousRequest()
	{
	TRACER("CGlxCacheManager::CancelPreviousRequest");		
	
	// iRequestOwner is NULL, if there are no pending attribute/thumbnail request
	// If no pending request, do nothing, else cancel the the pending requests
	if(iRequestOwner)
		{		
		MMPXCollection& collection = iRequestOwner->Collection();
		// Cancel the pending attribute request
		collection.CancelRequest();	
		
#ifdef USE_S60_TNM
		GLX_DEBUG2("CGlxCacheManager::CancelPreviousRequest() iThumbnailRequestIds.Count() %d", iThumbnailRequestIds.Count());
		
		// Check if any thumbnail requests are pending and cancel the requests.		
		for (TInt i = 0; i < iThumbnailRequestIds.Count(); i++)
			{ 			
			iTnEngine->CancelRequest(iThumbnailRequestIds[i].iId);									
			}
		iThumbnailRequestIds.Reset();		
#endif
		iRequestOwner = NULL;
		}
	}

// -----------------------------------------------------------------------------
// HandleGarbageCollection
// Garbage Collection Quick or with Timer
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::HandleGarbageCollectionL(TBool aStart)
    {
    TRACER("CGlxCacheManager::HandleGarbageCollection");
    TInt freeMemory = 0;
    
    HAL::Get( HALData::EMemoryRAMFree, freeMemory );
        
    if(aStart)
        {
           if(freeMemory < KGlxLowerMemoryLimitForCleanUp)
               {
               // 2 page - 30 Items for Flush
               TInt count = 2;  
               if(freeMemory < (KGlxLowerMemoryLimitForCleanUp/2))
                   {
                   // If Memory is below this limit it's ok i can wait for 60 items to clean-up
                   count = KGlxNoOfPagesToFlushInCriticalLowMemory;
                   }
               // Cancel Clean-up before Flush Page; Clean-up will be starting just after Flush page
               iGarbageCollector->CancelCleanup(); 
               iGarbageCollector->FlushPagesL(count);
               iGarbageCollector->CleanupL();
               iCleanUpOnGoing = ETrue;         // Clean up is Started now i can call CancelClean-up to Stop Clean Up
               }
           else if((freeMemory < KGlxUpperMemoryLimitForCleanUp) && !iCleanUpOnGoing)
               {
               iGarbageCollector->CleanupL();
               iCleanUpOnGoing = ETrue;         // Clean up is Started now i can call CancelClean-up to Stop Clean Up
               }
           // This is Added to Keep Assure Clean-up is not going to Disturb normal Flow if there is Enough Memory
           // We Remove this Code After Evaluation of Use of this Code
           else if(iCleanUpOnGoing)
               {
               iGarbageCollector->CancelCleanup();
               iCleanUpOnGoing = EFalse;
               }
               
           }
       else if(freeMemory < KGlxLowerMemoryLimitForCleanUp)
           {
           // 2 page - 30 Items for Flush
           TInt count = 2;
           if(freeMemory < (KGlxLowerMemoryLimitForCleanUp/2))
               {
               // If Memory is below this limit it's ok i can wait for 60 items to clean-up
               count = KGlxNoOfPagesToFlushInCriticalLowMemory;
               }
           // Cancel Clean-up before Flush Page; Clean-up will be starting just after Flush page
           iGarbageCollector->CancelCleanup();
           iGarbageCollector->FlushPagesL(count);
        iGarbageCollector->CleanupL();
        iCleanUpOnGoing = ETrue;         // Clean up is Started now i can call CancelClean-up to Stop Clean Up
        }
    else if(iCleanUpOnGoing)
        {
        iGarbageCollector->CancelCleanup();
        iCleanUpOnGoing = EFalse;
        }
    }
		
// -----------------------------------------------------------------------------
// Match
// return ETrue if path levels match
// -----------------------------------------------------------------------------
//
TBool CGlxCacheManager::Match(const CMPXCollectionPath& aPath1, 
		const CMPXCollectionPath& aPath2)
	{
	TRACER("CGlxCacheManager::Match");
	
	TInt levels1 = aPath1.Levels();
	TInt levels2 = aPath2.Levels();
	
	// Paths must be as deep to match
	if (levels1 != levels2) 
		{
		return EFalse; 
		}
		
	// Check if all levels match
	for (TInt i = 0; i < levels1; i++) 
		{
		if (aPath1.Index(i) != aPath2.Index(i))
			{
			return EFalse;
			}
		}
		
	return ETrue;
	}
	
// -----------------------------------------------------------------------------
// HandleCollectionMediaL
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::HandleCollectionMediaL(const TGlxIdSpaceId& aIdSpaceId, const CMPXMedia& aMedia, TInt aError)
    {
    TRACER("CGlxCacheManager::HandleCollectionMediaL");
    
    iRequestOwner = NULL;

    if ( KErrNone == aError )
        {
        // Update the cache
        CGlxCache* cache = FindCacheForceCreateL(aIdSpaceId);
        cache->MediaUpdatedL(aMedia);
        }
    else
        {
        RPointerArray< MGlxMediaUser > users;
        CleanupClosePushL( users );
        
        // Record the error on all requested attributes
        TInt idCount = iRequestedItemIds.Count();
        
        for( TInt idIndex = 0; idIndex < idCount; idIndex++ )
            {
            CGlxMedia* item = MediaForceCreateL(aIdSpaceId, iRequestedItemIds[idIndex]);
            GlxErrorManager::SetAttributeErrorL(item, iRequestedAttrs, aError);
            
            // Keep track of media lists to notify later
            TInt userCount = item->UserCount();
            for ( TInt userIndex = 0; userIndex < userCount; userIndex++ )
                {
				users.InsertInAddressOrderL( &item->User( userIndex ) );
                }
            }

        // Notify all affected users of error
        TInt userCount = users.Count();
        for ( TInt i = 0; i < userCount; i++ )
            {
            users[ i ]->HandleError( aError );
            }

        CleanupStack::PopAndDestroy( &users );
        }
    
    MaintainCacheL();
    }

// -----------------------------------------------------------------------------
// FindCacheForceCreateL
// -----------------------------------------------------------------------------
//
CGlxCache* CGlxCacheManager::FindCacheForceCreateL(const TGlxIdSpaceId& aIdSpaceId)
    {
    TRACER("CGlxCacheManager::FindCacheForceCreateL");
    
    CGlxCache* cache = NULL;

    // Look for existing cache
    TInt index = iCaches.FindInOrder(aIdSpaceId, (&CacheOrderByKey));
    if ( KErrNotFound == index )
        {
        // Not found: create one
    	TLinearOrder<CGlxCache> orderer (&CacheOrderById);
        cache = new (ELeave) CGlxCache(aIdSpaceId, this);

        CleanupStack::PushL(cache);
        iCaches.InsertInOrderL(cache, orderer);
        CleanupStack::Pop(cache);
        }
    else
        {
        cache = iCaches[index];
        }

    return cache;
    }
    
// -----------------------------------------------------------------------------
// Media
// -----------------------------------------------------------------------------
//
CGlxMedia* CGlxCacheManager::Media( const TGlxIdSpaceId& aIdSpaceId, 
        const TGlxMediaId& aMediaId ) const
    {
    TRACER("CGlxCacheManager::Media");
    
    // Look for the correct subcache
    TInt index = iCaches.FindInOrder( aIdSpaceId, &CacheOrderByKey );
    
    if ( KErrNotFound == index )
        {
        // No caches exist with this space id
        return NULL;
        }
    else
        {
        return iCaches[index]->Media( aMediaId );
        }
    }
    
// -----------------------------------------------------------------------------
// MediaForceCreateL
// -----------------------------------------------------------------------------
//
CGlxMedia* CGlxCacheManager::MediaForceCreateL(const TGlxIdSpaceId& aIdSpaceId, const TGlxMediaId& aMediaId)
    {
    TRACER("CGlxCacheManager::MediaForceCreateL");
    
    CGlxCache* cache = FindCacheForceCreateL(aIdSpaceId);
    return cache->FindItemForceCreateL(aMediaId);
    }
    
// -----------------------------------------------------------------------------
// AddObserverL
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::AddObserverL(MGlxCacheObserver* aObserver)
    {
    TRACER("CGlxCacheManager::AddObserverL");
    
    if ( aObserver )
        {
        if ( KErrNotFound == iObserverList.Find( aObserver ))
            {
            iObserverList.AppendL(aObserver);
            }
        }
    }
    
// -----------------------------------------------------------------------------
// RemoveObserver
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::RemoveObserver(MGlxCacheObserver* aObserver)
    {
    TRACER("CGlxCacheManager::RemoveObserver");
    
    if ( aObserver )
        {
        TInt index = iObserverList.Find( aObserver );
        if ( KErrNotFound != index )
            {
            iObserverList.Remove(index);
            }
        }
    }

// ---------------------------------------------------------------------------
// Refresh
// ---------------------------------------------------------------------------
//
void CGlxCacheManager::RefreshL()
    {
    TRACER("CGlxCacheManager::Refresh");
    
    if ( !iRequestOwner )
        {
        // Currently idle, so trigger timer immediately to begin refresh
        iTempErrorTimer->Cancel();
        iTempErrorTimer->Start( 0, 0,
                            TCallBack( TempErrorTimerCallbackL, this ) );
        
        HandleGarbageCollectionL(ETrue);  // Clean-up is needed here; Considering Current Memory Condition
        }
    }

// -----------------------------------------------------------------------------
// MaintainCache
// -----------------------------------------------------------------------------
//
TInt CGlxCacheManager::MaintainCacheL(TAny* aPtr)
    {
    TRACER("CGlxCacheManager::MaintainCache");
    
    CGlxCacheManager* self
                    = reinterpret_cast<CGlxCacheManager*>( aPtr );
    self->MaintainCacheL();
    
    // Clean-up is needed here; Considering Current Memory Condition
    // For Every HandleWindowChangedL; We Should not do Clean-up 
    self->HandleGarbageCollectionL(ETrue);  
    return 0;
    }
    
// -----------------------------------------------------------------------------
// MaintainCacheL
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::MaintainCacheL() 
    {
    TRACER("CGlxCacheManager::MaintainCacheL");
    
	// Handle lists in priority order: all data for the highest priority list 
	// is retrieved before the data for a lower priority list. NOTE: This
	// may need to be changed, since it might not always lead to the best
	// user experienced. (It may be sensible to retrieve item properties
	// for a lower priority list before all thumbnails for a higher priority 
	// list, but this probably depends on the speed of the data sources.
	// It might be wise to decouple the contexts from the media lists

    if (!iRequestOwner)
        {
        iTempError = EFalse;
        iTempErrorTimer->Cancel();

    	RPointerArray<CGlxMediaList>& mediaLists = CGlxMediaList::MediaListsL();
     	iRequestedItemIds.Reset();
     	iRequestedAttrs.Reset();
    	iRequestedItemIndexes.Reset();
    	
    	TInt listCount = mediaLists.Count();

    	// Request attributes in the fetch contexts
        for (TInt listIndex = 0; listIndex < listCount && !iRequestOwner; listIndex++)      
            {
            CGlxMediaList* list = mediaLists[listIndex];

    		CMPXAttributeSpecs* attrSpecs = NULL;

    		list->AttributeRequestL(iRequestedItemIndexes, iRequestedItemIds, iRequestedAttrs, attrSpecs );
    		CleanupStack::PushL(attrSpecs);

            TInt itemIdsCount = iRequestedItemIds.Count();
            if (itemIdsCount > 0)
                {
                if (attrSpecs)
                    {
                    GLX_DEBUG2("CGlxCacheManager::MaintainCacheL() attrSpecs->Count() %d", attrSpecs->Count());
                    }
                
                TGlxMediaId itemId = iRequestedItemIds[0];

                CMPXCollectionPath* path = RequestAsPathLC( *list );

                // Add request for id, if does not exist already. Id is used to 
                // identify which media object the attribute belongs to when
                // it is returned
                TIdentityRelation<TMPXAttribute> match (&TMPXAttribute::Match);
                if (iRequestedAttrs.Find(KMPXMediaGeneralId, match) == KErrNotFound) 
                    {
                    iRequestedAttrs.AppendL(KMPXMediaGeneralId);
                    }

                // Create the empty bitmap, if the request is for a bitmap
                TIdentityRelation<TMPXAttribute> matchContent(&TMPXAttribute::MatchContentId);
                TMPXAttribute tnAttr(KGlxMediaIdThumbnail, 0);
                if (iRequestedAttrs.Find(tnAttr, matchContent) != KErrNotFound) 
                    {
#ifdef USE_S60_TNM
					if (iRequestedItemIndexes.Count())
						{
                        // Cancel Clean-up is needed here;
                        HandleGarbageCollectionL(EFalse);                           
						
			            TSize tnSize;
						const TMPXAttributeData sizeAttrib = { KGlxMediaIdThumbnail , KGlxAttribSpecThumbnailSize };
      					if (attrSpecs && attrSpecs->IsSupported(sizeAttrib))
                        	{
				    		tnSize = attrSpecs->ValueTObjectL<TSize>(sizeAttrib);
                        	}

						const TGlxMedia& item = list->Item( iRequestedItemIndexes[0] );
      					TBool priority = ETrue;
      					const TMPXAttributeData priorityAttrib = { KGlxMediaIdThumbnail , KGlxAttribSpecThumbnailQualityOverSpeed };
      					if (attrSpecs && attrSpecs->IsSupported(priorityAttrib))
      					    {
      					    priority = attrSpecs->ValueTObjectL<TBool>(priorityAttrib);
      					    }
      					
						TGlxIdSpaceId spaceId = list->IdSpaceId(iRequestedItemIndexes[0]);	
#ifdef MEDIA_ID_BASED_TN_FETCH_ENABLED
	               	 	GLX_DEBUG2("CGlxCacheManager::MaintainCacheL() requesting TN attribute (Medialist) itemId %d", itemId.Value());
						if (item.Uri().Find(KFileIdentifier) != KErrNotFound ||
						    item.Uri().Length() == 0 && itemId.Value())
#else
	               	 	GLX_DEBUG1("CGlxCacheManager::MaintainCacheL() requesting TN attribute (Medialist) Uri");
						if (item.Uri().Find(KFileIdentifier) != KErrNotFound)
#endif
							{
#ifdef _DEBUG
							iStartTime.HomeTime(); // Get home time
#endif							
						    if (tnSize.iWidth < KMaxGridThumbnailWidth)
						    	{
						    	iTnEngine->SetFlagsL(CThumbnailManager::ECropToAspectRatio);
							    iTnEngine->SetThumbnailSizeL(EGridThumbnailSize);
							    GLX_DEBUG1("MaintainCacheL() - Fetch TN attrib - EGridThumbnailSize");
						    	}
						    else
							    {
							    iTnEngine->SetFlagsL(CThumbnailManager::EDefaultFlags);
							    iTnEngine->SetThumbnailSizeL(EFullScreenThumbnailSize);
                                GLX_DEBUG1("MaintainCacheL() - Fetch TN attrib - EFullScreenThumbnailSize");							    
							    }
						    
						    if (priority)
						        {
						        iTnEngine->SetQualityPreferenceL(CThumbnailManager::EOptimizeForQuality);
                                GLX_DEBUG1("MaintainCacheL() - Fetch TN attrib - EOptimizeForQuality");                                
						        }
						    else
						        {
						        //iTnEngine->SetQualityPreferenceL(CThumbnailManager::EOptimizeForQualityWithPreview);
						        iTnEngine->SetQualityPreferenceL(CThumbnailManager::EOptimizeForPerformance);
                                GLX_DEBUG1("MaintainCacheL() - Fetch TN attrib - EOptimizeForQualityWithPreview");                                
                                }
                            
                            if (list->Collection().UidL().iUid == KGlxCollectionPluginImageViewerImplementationUid)
                                {
                                GLX_DEBUG1("void CGlxCacheManager::MaintainCacheL:Imageviewer TN");
                                _LIT( KPrivateFolder, "\\Private\\" );    // Platsec private folder  
                                TParsePtrC parse( item.Uri() );
                                if( parse.PathPresent() &&
                                    parse.Path().Length() > KPrivateFolder().Length() &&
                                    parse.Path().Left( KPrivateFolder().Length() ).CompareF( KPrivateFolder ) == 0 )
                                    {
                                    CGlxImageViewerManager *imageVwrMgr = CGlxImageViewerManager::InstanceL();
									CleanupClosePushL(*imageVwrMgr);
                                    GLX_DEBUG1("KGlxCollectionPluginImageViewerImplementationUid - Fetch (Private) TN!");
                                    if ( &(imageVwrMgr->ImageFileHandle()) != NULL )
                                        {
                                        //CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC(imageVwrMgr->ImageFileHandle());
                                        CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC(imageVwrMgr->ImageUri()->Des(), 0);
                                        iThumbnailRequestIds.AppendL(TLoadingTN(iTnEngine->GetThumbnailL(*source), spaceId, tnSize, itemId));
                                        CleanupStack::PopAndDestroy(source);
                                        }
                                    CleanupStack::PopAndDestroy(imageVwrMgr);	
                                    }
                                else
                                    {
                                    GLX_DEBUG1("void CGlxCacheManager::MaintainCacheL:Imageviewer In else");
                                    GLX_DEBUG2("CGlxCacheManager::MaintainCacheL: uri %S", &item.Uri());	
                                    CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC(item.Uri(), 0);
                                    iThumbnailRequestIds.AppendL(TLoadingTN(iTnEngine->GetThumbnailL(*source), spaceId, tnSize, itemId));
                                    CleanupStack::PopAndDestroy(source);
                                    }
                                }
                            else
                                {
#ifdef MEDIA_ID_BASED_TN_FETCH_ENABLED
						    iThumbnailRequestIds.AppendL(TLoadingTN(iTnEngine->GetThumbnailL(itemId.Value()), spaceId, tnSize, itemId));
#else
						    CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC(item.Uri(), 0);
                                iThumbnailRequestIds.AppendL(TLoadingTN(
                                        iTnEngine->GetThumbnailL(*source), 
                                        spaceId, tnSize, itemId));
						    CleanupStack::PopAndDestroy(source);
#endif
                                }
		                    iThumbnailId = itemId;
							}
						else
							{
		                    delete iTempThumbnail;
		                    iTempThumbnail = NULL;
		                    iTempThumbnail = new (ELeave) CFbsBitmap;
		                    User::LeaveIfError(iTempThumbnail->Create(TSize(), KGlxThumbnailDisplayMode));
		                    iThumbnailId = itemId;
	               		 	GLX_DEBUG2("CGlxCacheManager::MaintainCacheL() requesting TN (DataSourceMde) attribute itemId %d", itemId.Value());

		                    // Set bitmap handle in attribute spec
		                    if (attrSpecs)
		                        {
		                        TMPXAttribute thHandleAttrSpec(KGlxMediaIdThumbnail, KGlxAttribSpecThumbnailBitmapHandle);
		                        attrSpecs->SetTObjectValueL<TInt>(thHandleAttrSpec, iTempThumbnail->Handle());
		                        }

		                    // Cancel Clean-up is needed here;
		                    // This is Going to Highly Used Call
		                    // Can Think about an Ulternative.
		                    HandleGarbageCollectionL(EFalse);

			                GLX_DEBUG3("MGallery - CGlxCacheManager::MaintainCacheL() requesting attribute for list %x and item %d", list, itemId.Value());

			                // Use list's isolated collection
			                MMPXCollection& collection = list->Collection();

			    			// Issue the request
			    			collection.MediaL(*path, iRequestedAttrs.Array(), attrSpecs);
							}
						}
                    }
                else
                	{
	                GLX_DEBUG3("MGallery - CGlxCacheManager::MaintainCacheL() requesting attribute for list %x and item %d", list, itemId.Value());

	                // Use list's isolated collection
	                MMPXCollection& collection = list->Collection();
                    if (collection.UidL().iUid == KGlxCollectionPluginImageViewerImplementationUid)
                        {
                        CGlxImageViewerManager *imageVwrMgr = CGlxImageViewerManager::InstanceL();
						CleanupClosePushL(*imageVwrMgr);
#ifdef GLX_LOGGING_ENABLED
						TInt mediaCnt = list->Count();
#endif                        
                        TInt errInImage = KErrNone;
 
                        
                        GLX_DEBUG3("Image Viewer Collection - Attrib population! mediaCnt=%d, Media Id=%d",
                                mediaCnt, itemId.Value());
                        
                        delete iMPXMedia;
                        iMPXMedia = NULL;

                        TFileName fileName(KNullDesC);
                        //retrieve the filename as per the caller app.
                        if(imageVwrMgr->IsPrivate())
                            {
                            const TGlxMedia& item = list->Item( iRequestedItemIndexes[0] );
                            // If there is an URI available, then the request is due to EPathChanged message 
                            // due to a file save from Image Viewer, and attribs needs to be re-populated. 
                            // So, use the same URI; Oherwise getting fullname from handle might result in a crash.
                            if (item.Uri().Length())
                                {
                                fileName.Append(item.Uri());
                                }
                            else
                                {
                                // private path
                                RFile64& imageHandle = imageVwrMgr->ImageFileHandle();
                                if ( &imageHandle != NULL )
                                    {
                                    imageHandle.FullName(fileName);
                                    }
                                else
                                    {
                                    errInImage = KErrArgument;
                                    }   
                                }
                            }
                        else
                            {
                            // user data path
                            if(  imageVwrMgr->ImageUri() != NULL  )
                                {
                                fileName.Append(imageVwrMgr->ImageUri()->Des());
                                RFs fs;
                                CleanupClosePushL(fs);
                                TInt err = fs.Connect();   
                                errInImage = KErrArgument;
                                if(err == KErrNone)
                                    {
                                    if (fs.IsValidName(fileName))
                                        {
                                        errInImage = KErrNone;
                                        }
                                    }
                                CleanupStack::PopAndDestroy(&fs);
                                }
                            else
                                {
                                errInImage = KErrArgument;
                                }
                            }

                        iMPXMedia = CMPXMedia::NewL();

                        if (errInImage != KErrNone)
                            {
                            HandleGarbageCollectionL(EFalse);
                            CleanupStack::PopAndDestroy(path);
                            iRequestOwner = list;
                            CleanupStack::PopAndDestroy(attrSpecs); 
                            TGlxIdSpaceId spaceId = list->IdSpaceId(iRequestedItemIndexes[0]);
                            HandleCollectionMediaL(spaceId, *iMPXMedia, errInImage);
                            CleanupStack::PopAndDestroy(imageVwrMgr);
                            return;
                            }

                        if(!iReader)
                            {
                            TRAP(errInImage, iReader = CGlxImageReader::NewL());
                            }

                        for ( TInt i = 0; i < iRequestedAttrs.Count(); i++ )
                            {
                            if ( iRequestedAttrs[i] == KMPXMediaGeneralId )
                                {
                                iMPXMedia->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, 
                                        (TMPXItemId)itemId.Value());
                                }
                            else if ( iRequestedAttrs[i] == KMPXMediaGeneralType )
                                {
                                iMPXMedia->SetTObjectValueL(KMPXMediaGeneralType, EMPXItem);
                                }
                            else if ( iRequestedAttrs[i] == KMPXMediaGeneralCategory )
                                {
                                iMPXMedia->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXImage);
                                }                           
                            else if ( iRequestedAttrs[i] == KMPXMediaGeneralUri )
                                {
                                iMPXMedia->SetTextValueL(KMPXMediaGeneralUri, fileName);
                                }
                            else if ( iRequestedAttrs[i] == KMPXMediaGeneralTitle )
                                {
                                TParsePtrC parser(fileName);
                                iMPXMedia->SetTextValueL(KMPXMediaGeneralTitle, parser.Name());
                                }
                            else if ( iRequestedAttrs[i] == KMPXMediaGeneralDate )
                                {
                                TTime time;
                                time.HomeTime();
                                iMPXMedia->SetTObjectValueL(KMPXMediaGeneralDate, time.Int64());
                                }
                            else if ( iRequestedAttrs[i] == KGlxMediaGeneralLastModifiedDate )
                                {
                                if(errInImage == KErrNone)
                                    {
                                    RFs fs;
                                    CleanupClosePushL(fs);
                                    TInt err = fs.Connect();   
                                    if(err == KErrNone)
                                        {                                    
                                        TEntry entry;   
                                        fs.Entry(fileName,entry);    
                                        TTime time = entry.iModified;   
                                        iMPXMedia->SetTObjectValueL(
                                                KGlxMediaGeneralLastModifiedDate, 
                                                time.Int64());
                                        }
                                    else
                                        {
                                        TTime time;
                                        time.HomeTime();
                                        iMPXMedia->SetTObjectValueL(
                                                KGlxMediaGeneralLastModifiedDate, 
                                                time.Int64());
                                        }
                                    CleanupStack::PopAndDestroy(&fs);                                    
                                    }
                                else
                                    {
                                    TTime time;
                                    time.HomeTime();
                                    iMPXMedia->SetTObjectValueL(KGlxMediaGeneralLastModifiedDate, time.Int64());
                                    }
                                }
                            else if ( iRequestedAttrs[i] == KMPXMediaGeneralSize )
                                {
                                if(errInImage == KErrNone)
                                    {
                                    if(imageVwrMgr->IsPrivate())
                                        {
                                        TInt64 sz = 0;
                                        TInt err = KErrNotFound;                                      
                                        RFile64& imageHandle = imageVwrMgr->ImageFileHandle();
                                        if ( imageHandle.SubSessionHandle() != KNullHandle )
                                            {
                                            err = imageHandle.Size(sz);
                                            }
                                        if(err == KErrNone)
                                            {
                                            iMPXMedia->SetTObjectValueL(KMPXMediaGeneralSize, sz);
                                            }
                                        else
                                            {
                                            iMPXMedia->SetTObjectValueL(KMPXMediaGeneralSize, 0);                                            
                                            }
                                        }
                                    else
                                        {
                                        RFs fs;
                                        CleanupClosePushL(fs);
                                        TInt err = fs.Connect();   
                                        if(err == KErrNone)
                                            {
                                            TEntry entry;   
                                            fs.Entry(fileName,entry);    
                                            TInt sz = (TUint)entry.iSize;                                      
                                            iMPXMedia->SetTObjectValueL(KMPXMediaGeneralSize, sz);
                                            }
                                        else
                                            {
                                            iMPXMedia->SetTObjectValueL(KMPXMediaGeneralSize, 0);
                                            }
                                        CleanupStack::PopAndDestroy(&fs);
                                        }
                                    }
                                // If any error while image is being decoded by image decorder, Need to set
                                // default vaule for that image. Typical case is corrupted image.
                                else
                                    {
                                    iMPXMedia->SetTObjectValueL(KMPXMediaGeneralSize, 0);
                                    }
                                }
                            else if ( iRequestedAttrs[i] == KMPXMediaGeneralDrive )
                                {
                                TParsePtrC parser(fileName);
                                iMPXMedia->SetTextValueL(KMPXMediaGeneralDrive, parser.Drive());
                                }
                            else if ( iRequestedAttrs[i] == KMPXMediaGeneralMimeType )
                                {
                                if(errInImage == KErrNone)
                                    {
                                    TDataType dataType;
                                    GetMimeTypeL(fileName, dataType);
                                    iMPXMedia->SetTextValueL(KMPXMediaGeneralMimeType, dataType.Des());
                                    }
                                else
                                    {
                                    iMPXMedia->SetTextValueL(KMPXMediaGeneralMimeType, KDefaultType);
                                    }                                
                                }
                            else if ( iRequestedAttrs[i] == KMPXMediaGeneralDuration )
                                {
                                iMPXMedia->SetTObjectValueL(KMPXMediaGeneralDuration, 0);
                                }
                            else if ( iRequestedAttrs[i] == KGlxMediaGeneralSystemItem)
                                {
                                iMPXMedia->SetTObjectValueL(KGlxMediaGeneralSystemItem, EFalse);
                                }
                            else if ( iRequestedAttrs[i] == KGlxMediaGeneralDimensions )
                                {
                                TSize dimensions = TSize();
                                if(errInImage == KErrNone)
                                    {
                                    //need to fetch the original file dimensions
                                        dimensions = iReader->GetDimensions();
                                    iMPXMedia->SetTObjectValueL(KGlxMediaGeneralDimensions, dimensions);
                                    }
                                else
                                    {
                                    iMPXMedia->SetTObjectValueL(KGlxMediaGeneralDimensions, dimensions);
                                    }
                                }
                            else if ( iRequestedAttrs[i] == KGlxMediaGeneralFramecount )
                                {
                                TInt fcount = 1;
                                if(errInImage == KErrNone)
                                    {
                                    fcount = iReader->GetFrameCount();
                                    }
                                GLX_DEBUG2("Imageviewer Collection framecount(%d)", fcount);
                                iMPXMedia->SetTObjectValueL(KGlxMediaGeneralFramecount, fcount);
                                }
                            else if ( iRequestedAttrs[i] == KMPXMediaGeneralComment )
                                {
                                iMPXMedia->SetTextValueL(KMPXMediaGeneralComment, KNullDesC); 
                                }
                            else if (iRequestedAttrs[i] == KMPXMediaDrmProtected )
                                {
                                TBool protection = EFalse;
                                if(errInImage == KErrNone)
                                    {
                                    protection = iReader->GetDRMRightsL
                                    (ContentAccess::EIsProtected);
                                    }
                                iMPXMedia->SetTObjectValueL(KMPXMediaDrmProtected, protection);
                                }
                            else if ( iRequestedAttrs[i] == KGlxMediaGeneralDRMRightsValid )
                                {
                                TInt rightsValid = EGlxDrmRightsValidityUnknown;
                                if(errInImage == KErrNone)
                                    {
                                    TBool canView = iReader->GetDRMRightsL(ContentAccess::ECanView);
                                    rightsValid = canView ? 
                                        EGlxDrmRightsValid : EGlxDrmRightsInvalid;
                                    }
                                iMPXMedia->SetTObjectValueL(KGlxMediaGeneralDRMRightsValid,
                                                             rightsValid); 
                                }
                            else if ( iRequestedAttrs[i] == KMPXMediaGeneralCount )
                                {
                                iMPXMedia->SetTObjectValueL(KMPXMediaGeneralCount, 1);
                                }
                            else if ( iRequestedAttrs[i] == KMPXMediaColDetailSpaceId )
                                {
                                TGlxIdSpaceId spaceId = list->IdSpaceId(iRequestedItemIndexes[0]);
                                iMPXMedia->SetTObjectValueL(KMPXMediaColDetailSpaceId,
                                		 spaceId.Value());
                                }
                            else if ( iRequestedAttrs[i] == KGlxMediaGeneralSlideshowableContent )
                                {
                                iMPXMedia->SetTObjectValueL(KGlxMediaGeneralSlideshowableContent, -1);
                                }
                            else if ( iRequestedAttrs[i] == KGlxMediaGeneralLocation)
                                {
                                // Set the attribute to a TCoordinate initialised to NaN. 
                                iMPXMedia->SetTObjectValueL(KGlxMediaGeneralLocation, TCoordinate());
                                }
                            else
                                {
                                User::Leave(KErrNotSupported);
                                }
                            }
                        
                        HandleGarbageCollectionL(EFalse);
                        CleanupStack::PopAndDestroy(imageVwrMgr);
                        CleanupStack::PopAndDestroy(path);
                        iRequestOwner = list;
                        CleanupStack::PopAndDestroy(attrSpecs); 
                        TGlxIdSpaceId spaceId = list->IdSpaceId(iRequestedItemIndexes[0]);
                        HandleCollectionMediaL(spaceId, *iMPXMedia, KErrNone);
                        return;
                        }
                    else
                        {
                    // Issue the request
                    collection.MediaL(*path, iRequestedAttrs.Array(), attrSpecs);
                        }
                    }
#else // USE_S60_TNM
                    delete iTempThumbnail;
                    iTempThumbnail = NULL;
                    iTempThumbnail = new (ELeave) CFbsBitmap;
                    User::LeaveIfError(iTempThumbnail->Create(TSize(), KGlxThumbnailDisplayMode));
                    iThumbnailId = itemId;

                    // Set bitmap handle in attribute spec
                    if (attrSpecs)
                        {
                        TMPXAttribute thHandleAttrSpec(KGlxMediaIdThumbnail, KGlxAttribSpecThumbnailBitmapHandle);
                        attrSpecs->SetTObjectValueL<TInt>(thHandleAttrSpec, iTempThumbnail->Handle());
                        }
                    // Cancel Clean-up is needed here;
                    // This is Going to Highly Used Call
                    // Can Think about an Ulternative.
                    HandleGarbageCollectionL(EFalse);
                    }

                GLX_DEBUG3("MGallery - CGlxCacheManager::MaintainCacheL() requesting attribute for list %x and item %d", list, itemId.Value());

                // Use list's isolated collection
                MMPXCollection& collection = list->Collection();

    			// Issue the request
    			collection.MediaL(*path, iRequestedAttrs.Array(), attrSpecs);
#endif
    			CleanupStack::PopAndDestroy(path);
    			
    			iRequestOwner = list;
    			}		
    			
    		CleanupStack::PopAndDestroy(attrSpecs);	
    		}
    		
        if ( !iRequestOwner )
            {
            if ( iTempError )
                {
                // Reawaken in a while to check again
                StartTempErrorTimer();
                }
            }
    	}
    }

// -----------------------------------------------------------------------------
// BroadcastAttributesAvailableL
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::BroadcastAttributesAvailableL(const TGlxIdSpaceId& aIdSpaceId, 
                    const TGlxMediaId& aMediaId, 
                    const RArray<TMPXAttribute>& aAttributes, 
                    const CGlxMedia* aMedia)
   {
   TRACER("CGlxCacheManager::BroadcastAttributesAvailableL");
   
    TInt count = iObserverList.Count();
    TInt i;
    for ( i = 0; i < count; i++ )
        {
        iObserverList[i]->HandleAttributesAvailableL(aIdSpaceId, aMediaId, aAttributes, aMedia);
        }
   }

/**
 * Cleanup the media of the given media id: broadcast this to all observers
 * @param aMediaId The media id of the item
 */ 
void CGlxCacheManager::CleanupMedia(const TGlxMediaId& aMediaId)
	{
	TRACER("CGlxCacheManager::CleanupMedia");
    TInt count = iObserverList.Count();
    GLX_DEBUG2("CGlxCacheManager::CleanupMedia -  aMediaId = %d", aMediaId.Value());
    TInt i;
    for ( i = 0; i < count; i++ )
        {
        iObserverList[i]->CleanupMedia(aMediaId);
        }
   }

// -----------------------------------------------------------------------------
// Handles modification of item in a cache
// -----------------------------------------------------------------------------
void CGlxCacheManager::HandleItemModified(const TGlxIdSpaceId& aIdSpaceId, const TGlxMediaId& aMediaId, const RArray<TMPXAttribute>& aAttributes)
	{
	TRACER("CGlxCacheManager::HandleItemModified");
	
	// Look for the correct subcache
	TInt index = iCaches.FindInOrder(aIdSpaceId, (&CacheOrderByKey));
	if (index != KErrNotFound)
		{
		iCaches[index]->HandleItemModified(aMediaId, aAttributes);
		}
	}

// -----------------------------------------------------------------------------
// CacheOrderById
// -----------------------------------------------------------------------------
//
TInt CGlxCacheManager::CacheOrderById(const CGlxCache& aItem1, const CGlxCache& aItem2) 
	{
	TRACER("CGlxCacheManager::CacheOrderById");
	
	// Cannot do aItem1.IdSpaceId() - aItem2.IdSpaceId(), since IdSpaceId().Value() returns an unsigned value
	TGlxIdSpaceId id1 = aItem1.IdSpaceId();
	TGlxIdSpaceId id2 = aItem2.IdSpaceId();
	if (id1 < id2) 
		{
		return -1;
		}
		
	if (id1 > id2) 
		{
		return 1;
		}

	return 0;
	}

// -----------------------------------------------------------------------------
// CacheOrderByKey
// -----------------------------------------------------------------------------
//
TInt CGlxCacheManager::CacheOrderByKey(const TGlxIdSpaceId* aIdSpaceId, const CGlxCache& aItem2) 
	{
	TRACER("CGlxCacheManager::CacheOrderByKey");
	
	TGlxIdSpaceId id2 = aItem2.IdSpaceId();
	if (*aIdSpaceId < id2) 
		{
		return -1;
		}
		
	if (*aIdSpaceId > id2) 
		{
		return 1;
		}

	return 0;
	}

// -----------------------------------------------------------------------------
// TempThumbnail
// -----------------------------------------------------------------------------
//
CFbsBitmap* CGlxCacheManager::TempThumbnail()
    {
    TRACER("CGlxCacheManager::TempThumbnail");
    
    return iTempThumbnail;
    }
    
// -----------------------------------------------------------------------------
// TempThumbnailId
// -----------------------------------------------------------------------------
//
TGlxMediaId CGlxCacheManager::TempThumbnailId()
    {
    TRACER("CGlxCacheManager::TempThumbnailId");
    
    return iThumbnailId;
    }

// -----------------------------------------------------------------------------
// SetTempThumbnail
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::SetTempThumbnailToNull()
    {
    TRACER("CGlxCacheManager::SetTempThumbnailToNull");
    
    iTempThumbnail = NULL;
    }

// -----------------------------------------------------------------------------
// ErrorsOnRequestedItemsL
// -----------------------------------------------------------------------------
//
TBool CGlxCacheManager::ErrorsOnRequestedItemsL()
    {
    TRACER("CGlxCacheManager::ErrorsOnRequestedItemsL");
    
    TBool errorFound = EFalse;
    
    TInt itemCount = iRequestedItemIds.Count();
    TInt attrCount = iRequestedAttrs.Count();
    
    for ( TInt itemIndex = 0; itemIndex < itemCount && !errorFound; itemIndex++ )
        {
        CGlxMedia* media = Media(iRequestedItemsIdSpace, iRequestedItemIds[itemIndex]);
        
        if ( media )
            {
            for ( TInt attrIndex = 0; attrIndex < attrCount && !errorFound; attrIndex++ )
                {
                errorFound = (KErrNone != GlxErrorManager::HasAttributeErrorL(media, iRequestedAttrs[attrIndex]));
                }
            }
        }
    
    return errorFound;
    }
    
// -----------------------------------------------------------------------------
// Caches
// -----------------------------------------------------------------------------
//
const RPointerArray<CGlxCache>& CGlxCacheManager::Caches()
    {
    TRACER("CGlxCacheManager::Caches");
    
    return iCaches;
    }

// -----------------------------------------------------------------------------
// SetTemporaryErrorFlag
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::SetTemporaryErrorFlag()
    {
    TRACER("CGlxCacheManager::SetTemporaryErrorFlag");
    
    iTempError = ETrue;
    }
    
// -----------------------------------------------------------------------------
// StartTempErrorTimer
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::StartTempErrorTimer()
    {
    TRACER("CGlxCacheManager::StartTempErrorTimer");
    
    iTempErrorTimer->Cancel();
	iTempErrorTimer->Start(
		TTimeIntervalMicroSeconds32(KGlxTemporaryErrorRecheckPeriodInSeconds * 1000000),
		TTimeIntervalMicroSeconds32(KGlxTemporaryErrorRecheckPeriodInSeconds * 1000000),
		TCallBack(&CGlxCacheManager::TempErrorTimerCallbackL,(TAny *)this));
	}

// -----------------------------------------------------------------------------
// TempErrorTimerCallbackL
// -----------------------------------------------------------------------------
//
TInt CGlxCacheManager::TempErrorTimerCallbackL(TAny* aPtr)
    {
    TRACER("CGlxCacheManager::TempErrorTimerCallback");
    
	CGlxCacheManager* self = (CGlxCacheManager*) aPtr;
	
	if ( self )
	    {
    	self->TempErrorTimerCompleteL();
    	}

    return 0;
    }

// -----------------------------------------------------------------------------
// TempErrorTimerComplete
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::TempErrorTimerCompleteL()
    {
    TRACER("CGlxCacheManager::TempErrorTimerComplete");
    
    iTempErrorTimer->Cancel();
    TRAPD(err, MaintainCacheL());
    
    // If MaintainCacheL leaves, force gabage collection to start and restart timer
    if ( KErrNone != err )
        {
        if ( KErrNoMemory == err )
            {
            //iGarbageCollector->Cleanup();
            HandleGarbageCollectionL(ETrue);
            }
            
        if ( !iRequestOwner )
            {
            StartTempErrorTimer();
            }
        }
    }
    
// -----------------------------------------------------------------------------
// Create a path with the request items
// -----------------------------------------------------------------------------
//
inline CMPXCollectionPath* CGlxCacheManager::RequestAsPathLC(const CGlxMediaList& aList)
    {
    TRACER("CGlxCacheManager::RequestAsPathLC");
    
    CMPXCollectionPath* path = aList.PathLC( NGlxListDefs::EPathParent );

    RArray<TMPXItemId> mpxIds;
    CleanupClosePushL( mpxIds );

    TInt itemIdsCount = iRequestedItemIds.Count();

    // Reserve space for all items
    mpxIds.ReserveL( itemIdsCount );

    for (TInt i = 0; i < itemIdsCount; ++i)
        {
        mpxIds.AppendL( iRequestedItemIds[ i ].Value() );
        }

    path->AppendL( mpxIds.Array() );
    path->SelectAllL();

    CleanupStack::PopAndDestroy( &mpxIds );

    return path;
    }

// -----------------------------------------------------------------------------
// HandleListDeleted
// -----------------------------------------------------------------------------
void CGlxCacheManager::HandleListDeleted(CGlxMediaList* aList)
    {
    TRACER("CGlxCacheManager::HandleListDeleted");
    
    if (iRequestOwner == aList)
        {
        iRequestOwner = NULL;

        TRAP_IGNORE( MaintainCacheL() );
        }
    }

// -----------------------------------------------------------------------------
// Inform cache manager to reserve users for an item, for a particular cache
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::ReserveUsersL(const TGlxIdSpaceId& aIdSpaceId, TInt aCount)
    {
    TRACER("CGlxCacheManager::ReserveUsersL");
    
    // Look for existing cache
    TInt index = iCaches.FindInOrder( aIdSpaceId, ( &CacheOrderByKey ) );
    if (index != KErrNotFound)
        {
        iCaches[index]->ReserveUsersL( aCount );
        }
    }
//OOM 
// -----------------------------------------------------------------------------
// Start cache cleanup on Low memory event from OOM
// -----------------------------------------------------------------------------
//   
void CGlxCacheManager::ReleaseRAML(TBool aFlushOnRequest)
{
    TRACER("CGlxCacheManager::ReleaseRAM");
    if(aFlushOnRequest)
        {
        HandleGarbageCollectionL(aFlushOnRequest);        
        }
	else
	{
		iGarbageCollector->CleanupL(); 	 				
	} 	
}
// -----------------------------------------------------------------------------
// Force a cleanup on particular media id : remove all attributes
// -----------------------------------------------------------------------------
// 
void CGlxCacheManager::ForceCleanupMedia(TGlxIdSpaceId aSpaceId, 
                                          TGlxMediaId aMediaId)
    {
    TRACER("CGlxCacheManager::ForceCleanupMedia");  
    TInt spaceIdIndex = iCaches.FindInOrder( aSpaceId, &CacheOrderByKey );
    if ( KErrNotFound != spaceIdIndex )
        {
        if ( iCaches[spaceIdIndex]->Media( aMediaId ) )
            {            
            TInt mediaIdIndex = iCaches[spaceIdIndex]->FindMediaIndexInCache(aMediaId);
            if ( KErrNotFound != mediaIdIndex )
            	{
            	iCaches[spaceIdIndex]->Delete(mediaIdIndex);
            	}
            }
        }
    }

// -----------------------------------------------------------------------------
// Stop cache cleanup on good memory event from OOM
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::StopRAMReleaseL()
{
    TRACER("CGlxCacheManager::StopRAMRelease");
    iGarbageCollector->CancelCleanup(); 		

}
//OOM

#ifdef USE_S60_TNM
// -----------------------------------------------------------------------------
// CGlxCacheManager::FindLoadingById()
// -----------------------------------------------------------------------------
//
TInt CGlxCacheManager::FindLoadingById(TThumbnailRequestId aId, TBool aRemove)
    {
    TRACER("CGlxCacheManager::FindLoadingById");
    TInt index = KErrNotFound;
    for(TInt i = 0; i < iThumbnailRequestIds.Count(); ++i)
        {
        if(iThumbnailRequestIds[i].iId == aId)
            {
            index = i;
            if(aRemove)
                {
                iThumbnailRequestIds.Remove(i);
                }
            break;
            }
        }
    return index;
    }

// -----------------------------------------------------------------------------
// CGlxCacheManager::ThumbnailPreviewReady()
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::ThumbnailPreviewReady(MThumbnailData& aThumbnail,
        TThumbnailRequestId aId)
    {
    TRACER("CGlxCacheManager::ThumbnailPreviewReady");
    TInt error = KErrNotSupported;
    if (aThumbnail.Bitmap() != NULL)
         {
		 GLX_DEBUG1("CGlxCacheManager::ThumbnailPreviewReady preview aval");
         error = KErrNone;
         }
    ThumbnailReadyL(error, aThumbnail, aId, EFalse);
    }

// -----------------------------------------------------------------------------
// CGlxCacheManager::ThumbnailReady()
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::ThumbnailReady(TInt aError,
        MThumbnailData& aThumbnail, TThumbnailRequestId aId)
    {
    TRACER("CGlxCacheManager::ThumbnailReady");
    GLX_DEBUG2("CGlxCacheManager::ThumbnailReady aError=%d", aError);
    ThumbnailReadyL(aError, aThumbnail, aId, ETrue);
    }

// -----------------------------------------------------------------------------
// CGlxCacheManager::ThumbnailReadyL()
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::ThumbnailReadyL(TInt aError, MThumbnailData& aThumbnail, 
                                       TThumbnailRequestId aId, TBool aQuality)
    {
    TRACER("CGlxCacheManager::ThumbnailReadyL");
    GLX_DEBUG3("CGlxCacheManager::ThumbnailReadyL aError=%d, aQuality=%d",
                                aError, aQuality);

#ifdef _DEBUG
    iStopTime.HomeTime(); // Get home time
    GLX_DEBUG2("=>CGlxCacheManager::ThumbnailReadyL - TN Fetch took <%d> us", 
                    (TInt)iStopTime.MicroSecondsFrom(iStartTime).Int64());
#endif
    
	TInt reqIndex = FindLoadingById(aId, EFalse);
		
    if (reqIndex == KErrNotFound)
        {
		return;
        }

    delete iMPXMedia;
    iMPXMedia = NULL;
        
    iMPXMedia = CMPXMedia::NewL();
    if (aError == KErrNone)
        {
        delete iTempThumbnail;
        iTempThumbnail = NULL;
        iTempThumbnail = aThumbnail.DetachBitmap();

        CGlxThumbnailAttribute* tnAttribute = new (ELeave) CGlxThumbnailAttribute;
        CleanupStack::PushL(tnAttribute);
        tnAttribute->iDimensions = iThumbnailRequestIds[reqIndex].iSize;
	    tnAttribute->iThumbnailQuality = aQuality;
                
        iMPXMedia->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, 
						iThumbnailRequestIds[reqIndex].iThumbnailId.Value());
        TUint attributeId = GlxFullThumbnailAttributeId(aQuality, 
								iThumbnailRequestIds[reqIndex].iSize.iWidth, 
								iThumbnailRequestIds[reqIndex].iSize.iHeight);
        iMPXMedia->SetNoNewLCObjectL(
               TMPXAttribute(KGlxMediaIdThumbnail, attributeId), tnAttribute);
        CleanupStack::PopAndDestroy(tnAttribute);

        HandleCollectionMediaL(iThumbnailRequestIds[reqIndex].iSpaceId,
                                                    *iMPXMedia, aError);
        }
    else
        {
        HandleCollectionMediaL(iThumbnailRequestIds[reqIndex].iSpaceId, 
													*iMPXMedia, aError);
        }
    
    if (aQuality)
        {
        FindLoadingById(aId, ETrue);
        }   
    }
#endif

// -----------------------------------------------------------------------------
// GetMimeTypeL()
// -----------------------------------------------------------------------------
//
void CGlxCacheManager::GetMimeTypeL(TFileName& aFileName, TDataType& aMimeType)
    {
    TRACER("CGlxCacheManager::GetMimeTypeL");
    RApaLsSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL( session );

    TUid uid;
    User::LeaveIfError( session.AppForDocument( aFileName, uid, aMimeType ) );
    CleanupStack::PopAndDestroy(&session);
    }


//End of file
