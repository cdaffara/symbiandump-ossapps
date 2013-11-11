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
* Description:    Cache implementation for media items sharing the same Id space
*
*/




#include "glxcache.h"

#include <glxassert.h>
#include <mpxmediadrmdefs.h>			// for DRM attributes
#include <mpxmediacontainerdefs.h>
#include <mpxmediacollectiondetaildefs.h>
#include <glxthumbnailattributeinfo.h>
#include <glxtracer.h>
#include <lbsposition.h>				//for TCoordinate

#include "glxcachemanager.h"
#include "glxmedialist.h"

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxCache::CGlxCache( const TGlxIdSpaceId& aIdSpaceId, CGlxCacheManager* aCacheManager )
        : iIdSpaceId( aIdSpaceId ), iCacheManager(aCacheManager)
    {
    TRACER("CGlxCache::CGlxCache");
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxCache::~CGlxCache()
    {
    TRACER("CGlxCache::Destructor");
    iItemPool.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// IdSpaceId
// -----------------------------------------------------------------------------
//
TGlxIdSpaceId CGlxCache::IdSpaceId() const
    {
    TRACER("CGlxCache::IdSpaceId");
    
    return iIdSpaceId;
    }
    
// -----------------------------------------------------------------------------
// return a media object or null
// -----------------------------------------------------------------------------
//
CGlxMedia* CGlxCache::Media( const TGlxMediaId& aId ) const
	{
	TRACER("CGlxCache::Media");
	
    TInt index = iItemPool.FindInOrder(aId, (&MediaItemOrderByKey));
    if ( KErrNotFound != index )
        {
        return iItemPool[index];
        }
        
    return NULL;
    }

// -----------------------------------------------------------------------------
// FindItemForceCreateL
// -----------------------------------------------------------------------------
//
CGlxMedia* CGlxCache::FindItemForceCreateL( const TGlxMediaId& aId )
    {
    TRACER("CGlxCache::FindItemForceCreateL");
    
    CGlxMedia* media = Media( aId );

    if ( !media )
        {
        media = CreateItemL( aId );
        }

    return media;	
    }

// -----------------------------------------------------------------------------
// CreateItemL
// -----------------------------------------------------------------------------
//
CGlxMedia* CGlxCache::CreateItemL(const TGlxMediaId& aId)
    {
    TRACER("CGlxCache::CreateItemL");
    
    CGlxMedia* item = NULL;
    
    iItemPool.ReserveL( iItemPool.Count() + 1 );
    item = new (ELeave) CGlxMedia(aId);
    TLinearOrder<CGlxMedia> orderer (&MediaItemOrderById);
    iItemPool.InsertInOrder(item, orderer);

	// Safe to fail, since if the media objects is not used by any lists,
	// garbage collection will simply delete it
	RPointerArray<CGlxMediaList>& mediaLists = CGlxMediaList::MediaListsL();

	TInt count = mediaLists.Count();
	item->ReserveUsersL(count);
	for (TInt i = 0; i < count; i++) 
		{
		mediaLists[i]->OfferMedia(iIdSpaceId, item);
		}

    return item;    
    }

// -----------------------------------------------------------------------------
// MediaUpdatedL
// -----------------------------------------------------------------------------
//
void CGlxCache::MediaUpdatedL(const CMPXMedia& aMedia)
    {
    TRACER("CGlxCache::MediaUpdatedL");
    
    if (aMedia.IsSupported(KMPXMediaArrayContents))
        {
        CMPXMediaArray* mediaArray = aMedia.ValueCObjectL<CMPXMediaArray>(KMPXMediaArrayContents);
        CleanupStack::PushL(mediaArray);

        TInt arrayCount = mediaArray->Count();
		GLX_DEBUG2("CGlxCache::MediaUpdatedL() arrayCount=%d", arrayCount);
        for (TInt count = 0; count < arrayCount; ++count)
            {
            UpdateMediaL(*((*mediaArray)[count]));
            }

        CleanupStack::PopAndDestroy(mediaArray);
        }
    else
        {
        UpdateMediaL(aMedia);
        }
    }

// -----------------------------------------------------------------------------
// UpdateMediaL
// -----------------------------------------------------------------------------
void CGlxCache::UpdateMediaL(const CMPXMedia& aMedia)
    {
    TRACER("CGlxCache::UpdateMediaL");
    
    if ( !aMedia.IsSupported(KMPXMediaGeneralId) )
        {
        return;
        }
	// Copy/synchronize attributes
    TGlxMediaId id ((TUint32)aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
	GLX_DEBUG2("CGlxCache::UpdateMediaL() id=%d", id.Value());	
	CGlxMedia* item = Media( id );

	RArray<TMPXAttribute> newAttributes;
	CleanupClosePushL(newAttributes);
	
	if ( item )
		{
		GLX_DEBUG2("MGallery - CGlxCacheManager::HandleCollectionMediaL() existing item for item id %d", id.Value());
		CopyNewAndModifiedL(*item, aMedia, newAttributes);
    	}
    else 
    	{
		GLX_DEBUG2("MGallery - CGlxCacheManager::HandleCollectionMediaL() new item for item id %d", id.Value());

    	// This is a new item. Create a media object and offer it to all lists.
    	// If the item is no longer needed by any list, then this is ok, since
    	// garbage collection will delete the item
    	// Add in id order.        	
    	item = CreateItemL(id);
    	
		CopyNewAndModifiedL(*item, aMedia, newAttributes);
    	}
		
	// Broadcast the new attributes to all observers of the item and the cache
	if (newAttributes.Count() > 0)
		{
		TInt count = item->UserCount();
		for (TInt i = 0; i < count; i++)
			{
			item->User( i ).HandleAttributesAvailableL( item->IndexInUser( i ), newAttributes );
			}
			
	    // Broadcast to cache observers
		// iCacheManager->BroadcastAttributesAvailableL(iIdSpaceId, id, newAttributes, item);
		}

	CleanupStack::PopAndDestroy(&newAttributes);
    }
// -----------------------------------------------------------------------------
// Deletes the media item
// -----------------------------------------------------------------------------
void CGlxCache::CleanupMedia(const TGlxMediaId& aMediaId)
	{
	TRACER("CGlxCache::CleanupMedia");
    GLX_DEBUG2("CGlxCache::CleanupMedia -  aMediaId = %d", aMediaId.Value());
    if(iCacheManager)
        {
        iCacheManager->CleanupMedia(aMediaId);
        }
	}
    
// -----------------------------------------------------------------------------
// Handles modifications of item in cache
// -----------------------------------------------------------------------------
void CGlxCache::HandleItemModified(const TGlxMediaId& aId, const RArray<TMPXAttribute>& aAttributes)
	{
	TRACER("CGlxCache::HandleItemModified");
	
	TInt index = iItemPool.FindInOrder(aId, (&MediaItemOrderByKey));
	if (index != KErrNotFound)
		{
		iItemPool[index]->HandleModified(aAttributes);
		}
	}

// -----------------------------------------------------------------------------
// Copies new and modified attributes from the provided media object
// -----------------------------------------------------------------------------
//
void CGlxCache::CopyNewAndModifiedL(CGlxMedia& aTarget, const CMPXMedia& aSource, 
        RArray<TMPXAttribute>& aNewAttributes)
    {
    TRACER("CGlxCache::CopyNewAndModifiedL");
    CleanupClosePushL(aNewAttributes);
    
    /// @todo This is all temporary until global chunk based CMPXMedia is available
    TInt count = aSource.Count();
	GLX_DEBUG2("CGlxCache::CopyNewAndModifiedL() Attribs count=%d", count);
    aNewAttributes.ReserveL( count );

    for (TInt i = 0; i < count; i++)
        {
        const TMPXAttribute& attrib = aSource.Attribute(i);

        if (attrib == KMPXMediaGeneralId)
            {
            // Ignore id
            __ASSERT_DEBUG((TUint32)aSource.ValueTObjectL<TMPXItemId>(attrib) == aTarget.Id().Value(), 
                           Panic(EGlxPanicIllegalArgument)); // aMedia relates to another media object than this
            }
        else 
            {
            if (!aTarget.IsSupported(attrib)) 
                {
                TMPXAttributeType type = aSource.Type(i);

                if (type == EMPXTypeText) 
                    {
                    aTarget.SetTextValueL(attrib, aSource.ValueText(attrib));
                    }
                else if (type == EMPXTypeCObject) 
                    {
                    if ( KGlxMediaIdThumbnail == attrib.ContentId() )
                        {
                        // This is a thumbnail
                        CFbsBitmap* thumbnail = iCacheManager->TempThumbnail();

                        if ( thumbnail && iCacheManager->TempThumbnailId() == aTarget.Id() )
                            {
                            // Add new thumbnail attribute
                            CGlxThumbnailAttribute* value =
                                aSource.ValueNoNewLCObjectL<CGlxThumbnailAttribute>(attrib);
                            CleanupStack::PushL(value);
                            value->iBitmap = thumbnail;
                            GLX_DEBUG3("CGlxCache::CopyNewAndModifiedL() TN Attrib w(%d) h(%d)", 
					        	thumbnail->SizeInPixels().iWidth, thumbnail->SizeInPixels().iHeight);
                            iCacheManager->SetTempThumbnailToNull();
                            aTarget.SetCObjectValueL(attrib, value);
                            CleanupStack::Pop(value);
                            }
                        }
                    else
                        {
                        __DEBUG_ONLY(Panic(EGlxPanicNotImplemented)); // Add support for the attribute here
                        }
                    }
                else if (type == EMPXTypeTObject) 
                    {
                    if (attrib == KMPXMediaGeneralThumbnail1)
                        {
                        // Old thumbnail attribute - ignore
                        }
                    else if (attrib == KMPXMediaGeneralDate)
                        {
                        aTarget.SetTObjectValueL(attrib, aSource.ValueTObjectL<TInt64>(attrib));
                        }
                    else if (attrib == KGlxMediaGeneralLastModifiedDate)
                        {
                        TTime modified = aSource.ValueTObjectL<TInt64>(attrib);
                        TTimeIntervalSeconds utcOffset = User::UTCOffset();
                        modified += utcOffset;
                        aTarget.SetTObjectValueL(attrib, modified);
                        }
                    else if (attrib == KMPXMediaGeneralSize)
                        {
                        aTarget.SetTObjectValueL(attrib, aSource.ValueTObjectL<TUint>(attrib));
                        }
                    else if (attrib == KMPXMediaColDetailSpaceId)
                        {
                        aTarget.SetTObjectValueL(attrib, aSource.ValueTObjectL<TGlxIdSpaceId>(attrib));
                        }
                    else if (attrib == KMPXMediaGeneralType)
                        {
                        aTarget.SetTObjectValueL(attrib, aSource.ValueTObjectL<TInt>(attrib));
                        }
                    else if (attrib == KMPXMediaGeneralCategory)
                        {
                        aTarget.SetTObjectValueL(attrib, aSource.ValueTObjectL<TInt>(attrib));
                        }
                    else if (attrib == KMPXMediaGeneralCount)
                        {
                        aTarget.SetTObjectValueL(attrib, aSource.ValueTObjectL<TInt>(attrib));
                        }
                    else if (attrib == KGlxMediaGeneralDimensions)
                        {
                        aTarget.SetTObjectValueL(attrib, aSource.ValueTObjectL<TSize>(attrib));
                        }
                    else if (attrib == KGlxMediaGeneralLocation)
                        {
                        aTarget.SetTObjectValueL(attrib, aSource.ValueTObjectL<TCoordinate>(attrib));
                        }
                    else if (attrib == KMPXMediaGeneralDuration)
                        {
                        aTarget.SetTObjectValueL(attrib, aSource.ValueTObjectL<TInt>(attrib));
                        }  
                    else if (attrib == KMPXMediaDrmProtected)
                        {
                        aTarget.SetTObjectValueL(attrib, aSource.ValueTObjectL<TBool>(attrib));
                        }                         
                    else if (attrib == KGlxMediaGeneralDRMRightsValid)
                        {
                        aTarget.SetTObjectValueL(attrib, aSource.ValueTObjectL<TBool>(attrib));
                        }                         
                    else if (attrib == KGlxMediaGeneralSystemItem)
                        {
                        aTarget.SetTObjectValueL(attrib, aSource.ValueTObjectL<TBool>(attrib));
                        }
                    else if (attrib == KGlxMediaGeneralFramecount)
                        {
                        aTarget.SetTObjectValueL(attrib, aSource.ValueTObjectL<TInt>(attrib));
                        }				         				
                    else if(attrib == KGlxMediaGeneralSlideshowableContent)
                        {
                        aTarget.SetTObjectValueL(attrib, aSource.ValueTObjectL<TInt>(attrib));
                        }        				
                    else
                        {
                        __DEBUG_ONLY(Panic(EGlxPanicNotImplemented)); // Add support for the attribute here
                        }
                    }
                else 
                    {
                    __DEBUG_ONLY(Panic(EGlxPanicNotImplemented)); 
                    }

                aNewAttributes.AppendL(attrib);
                }
            }
        }
    CleanupStack::Pop(&aNewAttributes);
    }

// -----------------------------------------------------------------------------
// Delete
// -----------------------------------------------------------------------------
//
void CGlxCache::Delete( TInt aIndex ) 
    {
    TRACER("CGlxCache::Delete");
    
    GLX_ASSERT_DEBUG( 0 <= aIndex && aIndex < iItemPool.Count(), 
        Panic(EGlxPanicIllegalArgument), "deleting attribute out of bounds");
        
    CGlxMedia* media = iItemPool[aIndex];
    
    // Cleanup the media from all users, example texture manager
    CleanupMedia(media->Id());
    
    GLX_DEBUG2("MGallery - CGlxCache::Delete() for item id %d", media->Id().Value());
    // notify users that the media object is about to be deleted. This allows
    // users to remove their references
    // (loop does not t iterate indexes, since the contents of the User array 
    // will change during calls to RemoveReferences, as users are removed)
    
    // safety check that RemoveReference actually removed the reference
    __DEBUG_ONLY( TInt _maxLoopCount = 1000 );
    
    while ( media->UserCount() > 0 )
        {
        // The user must remove a reference and that will decrement the 
        // UserCount()
        media->User( 0 ).RemoveReference( media->IndexInUser( 0 ) );
        
        // protect against a bug that would cause hard-to-find infinite loop
        __DEBUG_ONLY( _maxLoopCount-- );
        GLX_ASSERT_DEBUG( _maxLoopCount != 0, Panic( EGlxPanicLogicError ),
            "infinite loop when removing a media object" );
        }
    
    delete media;
    iItemPool.Remove( aIndex );
    }
    
// -----------------------------------------------------------------------------
// Reserve space for a number of users, for all items in the cache
// -----------------------------------------------------------------------------
//
void CGlxCache::ReserveUsersL(TInt aCount)
    {
    TRACER("CGlxCache::ReserveUsersL");
    
    TInt count = iItemPool.Count();
    for ( TInt i = 0; i < count ; ++i )
        {
        iItemPool[i]->ReserveUsersL( aCount );
        }
    }

// -----------------------------------------------------------------------------
// MediaItemOrderById
// -----------------------------------------------------------------------------
//
TInt CGlxCache::MediaItemOrderById(const CGlxMedia& aItem1, const CGlxMedia& aItem2) 
	{
	TRACER("CGlxCache::MediaItemOrderById");
	
	// Cannot do aItem1.Id() - aItem2.Id(), since Id().Value() returns an unsigned value
	TGlxMediaId id1 = aItem1.Id();
	return MediaItemOrderByKey( &id1, aItem2 );
	}

// -----------------------------------------------------------------------------
// MediaItemOrderByKey
// -----------------------------------------------------------------------------
//
TInt CGlxCache::MediaItemOrderByKey(const TGlxMediaId* aMediaId, const CGlxMedia& aItem2) 
	{
	TRACER("CGlxCache::MediaItemOrderByKey");
	
	TGlxMediaId id2 = aItem2.Id();
	if (*aMediaId < id2) 
		{
		return -1;
		}
		
	if (*aMediaId > id2) 
		{
		return 1;
		}

	return 0;
	}

// -----------------------------------------------------------------------------
// Return count of media objects
// -----------------------------------------------------------------------------
//
TInt CGlxCache::Count()
    {
    TRACER("CGlxCache::Count");
    
    return iItemPool.Count();
    }
    
// -----------------------------------------------------------------------------
// Return media object by index
// -----------------------------------------------------------------------------
//
CGlxMedia& CGlxCache::Media( TInt aIndex )
    {
    TRACER("CGlxCache::Media");
    
    return *iItemPool[aIndex];
    }

// -----------------------------------------------------------------------------
// Return media index
// -----------------------------------------------------------------------------
//
TInt CGlxCache::FindMediaIndexInCache(TGlxMediaId aMediaId)
	{
	TRACER("CGlxCache::FindMediaIndexInCache");
	 
	return iItemPool.FindInOrder(aMediaId, (&MediaItemOrderByKey));
	}

