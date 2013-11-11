/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Categories related functionality*
*/




// INCLUDE FILES
#include <mpxlog.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxmessagecontainerdefs.h>
#include "vcxmyvideoscollectionplugin.h"
#include "vcxmyvideoscategories.h"
#include "vcxmyvideoscollectionutil.h"
#include "vcxmyvideosvideocache.h"
#include "vcxmyvideosmessagelist.h"

_LIT(KVcxMyVideosAllVideosCategoryText, "All videos");
_LIT(KVcxMyVideosDownloadsCategoryText, "Downloads");
_LIT(KVcxMyVideosTvRecordingsCategoryText, "TV recordings");
_LIT(KVcxMyVideosCapturedVideosCategoryText, "Captured videos");
_LIT(KVcxMyVideosOtherCategoryText, "Other");

const TInt KVcxMyVideosAllVideosCategoryIndex    = 0;
const TInt KVcxMyVideosDownloadsCategoryIndex    = 1;
const TInt KVcxMyVideosTvRecordingsCategoryIndex = 2;
const TInt KVcxMyVideosCapturedCategoryIndex     = 3;
const TInt KVcxMyVideosOtherCategoryIndex        = 4;


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosCategories* CVcxMyVideosCategories::NewL(
    CVcxMyVideosCollectionPlugin& aCollection )
    {
    MPX_FUNC("CVcxMyVideosCategories::NewL");

    CVcxMyVideosCategories* self = new (ELeave) CVcxMyVideosCategories( aCollection );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosCategories::~CVcxMyVideosCategories()
    {
    MPX_FUNC("CVcxMyVideosCategories::~CVcxMyVideosCategories");
    
    delete iList;
    iModifiedCategories.Close();
    }

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosCategories::CVcxMyVideosCategories( CVcxMyVideosCollectionPlugin& aCollection )
: iCollection( aCollection )
    {
    MPX_FUNC("CVcxMyVideosCategories::CVcxMyVideosCategories");
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::ConstructL ()
    {
    MPX_FUNC("CVcxMyVideosCategories::ConstructL");
    iList      = CreateCategoryListL();
    iListArray = iList->Value<CMPXMediaArray>( KMPXMediaArrayContents );   

    TInt categoryCount = iListArray->Count();
    for ( TInt i = 0; i < categoryCount; i++ )
        {
        iModifiedCategories.Append( EFalse );
        }

    }
    
// ----------------------------------------------------------------------------------------------------------
// CVcxMyVideosCategories::CreateCategoryListL
// ----------------------------------------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosCategories::CreateCategoryListL()
    {
    //first level

    CMPXMedia* entries = TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
    CleanupStack::PushL( entries ); // 1->
    entries->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, 0 /*aPath.Id( 0 )*/);
    CMPXMediaArray* entryArray = entries->Value<CMPXMediaArray>( KMPXMediaArrayContents );

    AppendCategoryToArrayL( KVcxMyVideosAllVideosCategoryText,
            TMPXItemId( KVcxMvcCategoryIdAll, 1), *entryArray );

    AppendCategoryToArrayL( KVcxMyVideosDownloadsCategoryText,
            TMPXItemId( KVcxMvcCategoryIdDownloads, 1 ), *entryArray );

    AppendCategoryToArrayL( KVcxMyVideosTvRecordingsCategoryText,
            TMPXItemId( KVcxMvcCategoryIdTvRecordings, 1 ), *entryArray );

    AppendCategoryToArrayL( KVcxMyVideosCapturedVideosCategoryText,
            TMPXItemId( KVcxMvcCategoryIdCaptured, 1), *entryArray );

    AppendCategoryToArrayL( KVcxMyVideosOtherCategoryText,
            TMPXItemId( KVcxMvcCategoryIdOther, 1 ), *entryArray );

    entries->SetTObjectValueL( KMPXMediaArrayCount, entryArray->Count() );

    CleanupStack::Pop( entries ); // <-1

    return entries;
    }

// ----------------------------------------------------------------------------------------------------------
// CVcxMyVideosCategories::AppendCategoryToArrayL
// ----------------------------------------------------------------------------------------------------------
//
void CVcxMyVideosCategories::AppendCategoryToArrayL( const TDesC& aTitle, TMPXItemId aId,
        CMPXMediaArray& aCategoryArray )
    {
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds ); // 1->
    supportedIds.AppendL( KMPXMediaIdGeneral );

    CMPXMedia* category = CMPXMedia::NewL( supportedIds.Array() );
    CleanupStack::PushL( category ); // 2->
    category->SetTextValueL( KMPXMediaGeneralTitle, aTitle );
    category->SetTObjectValueL( KMPXMediaGeneralId, aId );
    category->SetTObjectValueL( KMPXMediaGeneralType, EMPXGroup );
    category->SetTObjectValueL( KMPXMediaGeneralCategory, EMPXNoCategory );
    category->SetTObjectValueL( KVcxMediaMyVideosCategoryItemCount, 0 );
    category->SetTObjectValueL( KVcxMediaMyVideosCategoryNewItemCount, 0 );
    aCategoryArray.AppendL( category );
    CleanupStack::Pop( category );                // <-2
    CleanupStack::PopAndDestroy( &supportedIds ); // <-1
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::UpdateCategoriesL
// Called by iCollection when fetching the video list from mds incrementally.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::UpdateCategoriesL( CMPXMedia& aVideoList,
        TInt aNewItemsStartIndex )
    {    
    CMPXMediaArray* categoryArray = iList->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    TInt categoryCount            = categoryArray->Count();

    RArray<TInt> videosIncrements;
    videosIncrements.Reset();
    CleanupClosePushL( videosIncrements ); // 1->

    RArray<TInt> newVideosIncrements;
    newVideosIncrements.Reset();
    CleanupClosePushL( newVideosIncrements ); // 2->

    TInt i;
    videosIncrements.ReserveL( categoryCount );
    newVideosIncrements.ReserveL( categoryCount );
    for ( i = 0; i < categoryCount; i++ )
        {
        videosIncrements.AppendL( 0 );
        newVideosIncrements.Append( 0 );
        }

    CMPXMediaArray* videoListArray = aVideoList.Value<CMPXMediaArray>( KMPXMediaArrayContents );
    TInt videoListCount            = videoListArray->Count();

    videosIncrements[KVcxMyVideosAllVideosCategoryIndex] = videoListCount - aNewItemsStartIndex;
    
    CMPXMedia* media;
    for ( i = aNewItemsStartIndex; i < videoListCount; i++ )
        {
        TUint8 origin = EVcxMyVideosOriginOther;
        
        media = videoListArray->AtL( i );
        
        origin = TVcxMyVideosCollectionUtil::OriginL( *media );
                
        TBool newVideo = EFalse;
        TUint32 flags  = 0;
        
        if ( media->IsSupported( KMPXMediaGeneralFlags ) )
            {
            flags = media->ValueTObjectL<TUint32>( KMPXMediaGeneralFlags );
            if ( flags & EVcxMyVideosVideoNew )
                {
                newVideo = ETrue;
                newVideosIncrements[KVcxMyVideosAllVideosCategoryIndex]++;
                }
            }
        
        switch ( origin )
            {
            case EVcxMyVideosOriginDownloaded:
                videosIncrements[KVcxMyVideosDownloadsCategoryIndex]++;
                if ( newVideo )
                    {
                    newVideosIncrements[KVcxMyVideosDownloadsCategoryIndex]++;
                    }
                break;
                
            case EVcxMyVideosOriginCapturedWithCamera:
                videosIncrements[KVcxMyVideosCapturedCategoryIndex]++;
                if ( newVideo )
                    {
                    newVideosIncrements[KVcxMyVideosCapturedCategoryIndex]++;
                    }
                break;
                                
            case EVcxMyVideosOriginTvRecording:
                videosIncrements[KVcxMyVideosTvRecordingsCategoryIndex]++;
                if ( newVideo )
                    {
                    newVideosIncrements[KVcxMyVideosTvRecordingsCategoryIndex]++;
                    }
                break;

            case EVcxMyVideosOriginSideLoaded:                
            case EVcxMyVideosOriginOther:
            default:
                videosIncrements[KVcxMyVideosOtherCategoryIndex]++;
                if ( newVideo )
                    {
                    newVideosIncrements[KVcxMyVideosOtherCategoryIndex]++;
                    }
                break;
            }
        }

    // Update videos counts
    UpdateVideosCountL( *categoryArray, videosIncrements, newVideosIncrements );

    CleanupStack::PopAndDestroy( &newVideosIncrements ); // <-1
    CleanupStack::PopAndDestroy( &videosIncrements ); // <-1
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::UpdateVideosCountL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::UpdateVideosCountL( CMPXMediaArray& aCategoryArray,
    RArray<TInt>& aVideosIncrements, RArray<TInt>& aNewVideosIncrements )
    {
    TBool modified = EFalse;
    TInt categoryCount = aCategoryArray.Count();
    CMPXMedia* category;
    
    for ( TInt i = 0; i < categoryCount; i++ )
        {
        if ( aVideosIncrements[i] != 0 )
            {
            category = aCategoryArray.AtL( i );

            //codescanner warning: aVideosIncrements count is same as aCategoryArray count, so the range is checked
            UpdateVideosCountL( *category, aVideosIncrements[i],
                    KVcxMediaMyVideosCategoryItemCount, EVcxMyVideosListNoInfo );

            modified = ETrue;
            }

        //codescanner warning: aNewVideosIncrements count is same as aCategoryArray count, so the range is checked
        if ( aNewVideosIncrements[i] != 0 )
            {
            category = aCategoryArray.AtL( i );

            UpdateVideosCountL( *category, aNewVideosIncrements[i],
                    KVcxMediaMyVideosCategoryNewItemCount, EVcxMyVideosListNoInfo );

            modified = ETrue;
            }

        }
    if ( modified )
        {
        iCollection.iMessageList->SendL();
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::UpdateVideosCountL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::UpdateVideosCountL( CMPXMedia& aCategory,
    TInt aIncrement, TMPXAttributeData aCountAttribute, TInt aEventInfo )
    {
    if ( aIncrement != 0 )
        {
        AddToTUint32AttributeL( aCategory, aIncrement, aCountAttribute );
                
        iCollection.iMessageList->AddEventL( aCategory.ValueTObjectL<TMPXItemId>(
                KMPXMediaGeneralId ), EMPXItemModified, aEventInfo );
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::VideosAddedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::VideosAddedL( RArray<TUint32>& aIds )
    {
    VideosAddedOrRemovedL( aIds, ETrue );
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::VideosRemovedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::VideosRemovedL( RArray<TUint32>& aIds )
    {
    VideosAddedOrRemovedL( aIds, EFalse );
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::VideoAddedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::VideoAddedL( TUint32 aMdsId )
    {
    TInt pos( KErrNotFound );
    CMPXMedia* video = iCollection.iCache->FindVideoByMdsIdL( aMdsId, pos );
    if ( pos != KErrNotFound )
        {
        VideoAddedOrRemovedL( *video, ETrue );
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::VideoAddedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::VideoAddedL( CMPXMedia& aVideo )
    {
    VideoAddedOrRemovedL( aVideo, ETrue );
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::VideoRemovedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::VideoRemovedL( TUint32 aMdsId )
    {
    TInt pos( KErrNotFound );
    CMPXMedia* video = iCollection.iCache->FindVideoByMdsIdL( aMdsId, pos );
    if ( pos != KErrNotFound )
        {
        VideoAddedOrRemovedL( *video, EFalse );
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::VideoRemovedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::VideoRemovedL( CMPXMedia& aVideo )
    {
    VideoAddedOrRemovedL( aVideo, EFalse );
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::VideosAddedOrRemovedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::VideosAddedOrRemovedL(
        RArray<TUint32>& aIds, TBool aAdded )
    {
    TInt count = aIds.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        VideoAddedOrRemovedL( aIds[i], aAdded );
        }
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::VideoAddedOrRemovedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::VideoAddedOrRemovedL( TUint32 aMdsId, TBool aAdded )
    {    
    TInt pos( KErrNotFound );
    CMPXMedia* video = iCollection.iCache->FindVideoByMdsIdL( aMdsId, pos );
    if ( pos != KErrNotFound )
        {
        VideoAddedOrRemovedL( *video, aAdded );
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::VideoAddedOrRemovedL
// All remove&add calls eventually come here.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::VideoAddedOrRemovedL( CMPXMedia& aVideo, TBool aAdded )
    {
    MPX_FUNC("CVcxMyVideosCategories::VideoAddedOrRemovedL()");

    ResetModifiedCategoriesArray();

    TInt categoryIndex  = CategoryIndex( TVcxMyVideosCollectionUtil::OriginL( aVideo ) );
    CMPXMedia* category = iListArray->AtL( categoryIndex );
    // every event updates always 2 categories, all and something else
    UpdateCategoryL( *category, categoryIndex, aVideo, aAdded );

    //iMessageArray will discard duplicates
    AddEventsFromModifiedCategoriesArrayL();
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::UpdateCategoryL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::UpdateCategoryL( CMPXMedia& aCategory, TInt aCategoryIndex,
        CMPXMedia& aVideo, TBool aAdded )
    {
    MPX_FUNC("CVcxMyVideosCategories::UpdateCategoryL");

    TInt addition = 0;
    
    if ( aAdded )
        {
        MPX_DEBUG1("CVcxMyVideosCategories:: video added");
        if ( TVcxMyVideosCollectionUtil::FlagsL( aVideo ) & EVcxMyVideosVideoNew )
            {
            TInt64 newItemsDate = TVcxMyVideosCollectionUtil::CreationDateL( aVideo );
            TInt64 prevNewDate  = TVcxMyVideosCollectionUtil::CreationDateL( aCategory );

            if ( static_cast<TInt64>(newItemsDate) > static_cast<TInt64>(prevNewDate) )
                {                    
                MPX_DEBUG1("CVcxMyVideosCategories:: EMPXItemInserted -> changing new video name");
                aCategory.SetTextValueL( KVcxMediaMyVideosCategoryNewItemName,
                        aVideo.ValueText( KMPXMediaGeneralTitle ) );
                aCategory.SetTObjectValueL( KMPXMediaGeneralDate, newItemsDate ); 
                iListArray->AtL( KVcxMyVideosAllVideosCategoryIndex)->SetTextValueL(
                        KVcxMediaMyVideosCategoryNewItemName,
                        aVideo.ValueText( KMPXMediaGeneralTitle ) );
                iListArray->AtL( KVcxMyVideosAllVideosCategoryIndex)->SetTObjectValueL(
                        KMPXMediaGeneralDate, newItemsDate );
                iModifiedCategories[aCategoryIndex]                     = ETrue;
                iModifiedCategories[KVcxMyVideosAllVideosCategoryIndex] = ETrue;
                }
            }
        addition = 1;
        }
    else
        {
        MPX_DEBUG1("CVcxMyVideosCategories:: video deleted");

        TBool modified = EFalse;
        RArray<TUint32> ignoredIds;
        ignoredIds.Reset();
        CleanupClosePushL( ignoredIds ); // 1->
        if ( !aAdded )
            {
            ignoredIds.AppendL( TVcxMyVideosCollectionUtil::IdL( aVideo ) );
            }
        UpdateCategoryNewVideoNameAndDateL( -1 /* -1 origin resolves to All category */,
                modified, ignoredIds );
        UpdateCategoryNewVideoNameAndDateL( TVcxMyVideosCollectionUtil::OriginL( aVideo ),
                modified, ignoredIds );
        CleanupStack::PopAndDestroy( &ignoredIds ); // <-1

        if ( modified )
            {
            iModifiedCategories[aCategoryIndex]                     = ETrue;
            iModifiedCategories[KVcxMyVideosAllVideosCategoryIndex] = ETrue;
            }
            
        addition = -1;
        }
        
    
    if ( addition != 0 )
        {
        AddToTUint32AttributeL( aCategory, addition, KVcxMediaMyVideosCategoryItemCount );
        AddToTUint32AttributeL( *(iListArray->AtL( KVcxMyVideosAllVideosCategoryIndex) ),
                addition, KVcxMediaMyVideosCategoryItemCount );
        if ( TVcxMyVideosCollectionUtil::FlagsL( aVideo ) & EVcxMyVideosVideoNew )
            {
            AddToTUint32AttributeL( aCategory, addition, KVcxMediaMyVideosCategoryNewItemCount );
            AddToTUint32AttributeL( *(iListArray->AtL( KVcxMyVideosAllVideosCategoryIndex) ),
                    addition, KVcxMediaMyVideosCategoryNewItemCount );
            }
        iModifiedCategories[aCategoryIndex]                     = ETrue;
        iModifiedCategories[KVcxMyVideosAllVideosCategoryIndex] = ETrue;
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::AddToTUint32AttributeL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::AddToTUint32AttributeL( CMPXMedia& aCategory, TInt aAddition,
        TMPXAttributeData aAttribute )
    {
    TUint32 previousCount = 0;
    if ( aCategory.IsSupported( aAttribute ) )
        {
        previousCount = aCategory.ValueTObjectL<TUint32>( aAttribute );
        }
    
    TInt totalCount = previousCount + aAddition;
    if ( totalCount < 0 )
        {
        totalCount = 0;
        }
    aCategory.SetTObjectValueL<TUint32>( aAttribute, totalCount );

    MPX_DEBUG2("CVcxMyVideosCategories::AddToTUint32AttributeL() attribute(%x): ", aAttribute );
    MPX_DEBUG3("CVcxMyVideosCategories::AddToTUint32AttributeL() %d -> %d", previousCount, totalCount);
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::CategoryIndex
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosCategories::CategoryIndex( TInt aOrigin )
    {
    switch ( aOrigin )
        {
        case -1:
            return KVcxMyVideosAllVideosCategoryIndex;
                        
        case EVcxMyVideosOriginCapturedWithCamera:
            return KVcxMyVideosCapturedCategoryIndex;
                            
        case EVcxMyVideosOriginDownloaded:
#ifdef VCX_DOWNLOADS_CATEGORY
            return KVcxMyVideosDownloadsCategoryIndex;
#endif
        case EVcxMyVideosOriginTvRecording:
        case EVcxMyVideosOriginSideLoaded:                
        case EVcxMyVideosOriginOther:
        default:
            return KVcxMyVideosOtherCategoryIndex;
        }
    }     

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::NewVideoFlagChangedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::NewVideoFlagChangedL( TUint32 aOldFlag, TUint32 aNewFlag,
        TUint8 aOrigin, TBool& aModified )
    {
    CMPXMedia* categoryAll;
    CMPXMedia* categoryByOrigin;
    TBool modified = EFalse;
    
    if ( (aOldFlag & EVcxMyVideosVideoNew) && !(aNewFlag & EVcxMyVideosVideoNew) )
        {
        // new flag cleared -> update counters
        categoryByOrigin = iListArray->AtL( CategoryIndex( aOrigin ) );
        AddToTUint32AttributeL( *categoryByOrigin, -1, KVcxMediaMyVideosCategoryNewItemCount );
        categoryAll = iListArray->AtL( KVcxMyVideosAllVideosCategoryIndex );
        AddToTUint32AttributeL( *categoryAll, -1, KVcxMediaMyVideosCategoryNewItemCount );
        aModified = ETrue;
        modified  = ETrue;
        } 
    else if ( !(aOldFlag & EVcxMyVideosVideoNew) && (aNewFlag & EVcxMyVideosVideoNew) )
        {
        // new flag set -> update counters
        categoryByOrigin = iListArray->AtL( CategoryIndex( aOrigin ) );
        AddToTUint32AttributeL( *categoryByOrigin, 1, KVcxMediaMyVideosCategoryNewItemCount );
        categoryAll = iListArray->AtL( KVcxMyVideosAllVideosCategoryIndex );
        AddToTUint32AttributeL( *categoryAll, 1, KVcxMediaMyVideosCategoryNewItemCount );
        aModified = ETrue;
        modified  = ETrue;
        }
        
    if ( modified )
        {
        TMPXItemId mpxId( categoryByOrigin->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
        iCollection.iMessageList->AddEventL( mpxId, EMPXItemModified );

        mpxId = categoryAll->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
        iCollection.iMessageList->AddEventL( mpxId, EMPXItemModified );
        } 
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::ResetVideoCountersL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::ResetVideoCountersL()
    {
    TInt count = iListArray->Count();
    
    for ( TInt i = 0; i < count; i++ )
        {
        iListArray->AtL( i )->SetTObjectValueL<TUint32>(
                KVcxMediaMyVideosCategoryNewItemCount, 0 );
        iListArray->AtL( i )->SetTObjectValueL<TUint32>(
                KVcxMediaMyVideosCategoryItemCount, 0 );
        }
    }

/*

New video name generation:

1. When new video list is created: calculate new video names from scratch at the end of list creation.
2. Item is removed (mds remove event): calculate from scratch to the categories which are affected.
3. Item is added (mds add event): If new item && date is newer than previous -> put it to categories which are affected.
4. Items new flag is modified: calculate from scratch to the categories which are affected.
5. Items title is modified: calculate from scratch to the categories which are affected.
*/

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::UpdateCategoriesNewVideoNamesL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::UpdateCategoriesNewVideoNamesL()
    {
    MPX_FUNC("CVcxMyVideosCategories::UpdateCategoriesNewVideoNamesL");
    
    RArray<TUint32> ignoredIds;
    ignoredIds.Reset();
    CleanupClosePushL( ignoredIds ); // 1->
    TBool modified = EFalse;
    UpdateCategoryNewVideoNameAndDateL( -1 /* all category */, modified, ignoredIds );
    UpdateCategoryNewVideoNameAndDateL( EVcxMyVideosOriginCapturedWithCamera, modified,
            ignoredIds );
    UpdateCategoryNewVideoNameAndDateL( EVcxMyVideosOriginDownloaded, modified, ignoredIds );
    UpdateCategoryNewVideoNameAndDateL( EVcxMyVideosOriginTvRecording, modified, ignoredIds );
    UpdateCategoryNewVideoNameAndDateL( EVcxMyVideosOriginOther, modified, ignoredIds );
    CleanupStack::PopAndDestroy( &ignoredIds ); // <-1
    
    if ( modified )
        {
        // flush event list
        MPX_DEBUG1("CVcxMyVideosCategories:: sending event list");
        iCollection.iMessageList->SendL();
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::UpdateCategoryNewVideoNameAndDateL
// ----------------------------------------------------------------------------
//    
void CVcxMyVideosCategories::UpdateCategoryNewVideoNameAndDateL( TInt aOrigin, TBool& aModified,
        RArray<TUint32>& aIgnoredIds )
    {   
    MPX_FUNC("CVcxMyVideosCategories::UpdateCategoryNewVideoNameAndDateL");

    TPtrC prevNewVideoName;
    TPtrC currentNewVideoName;
    
    CMPXMedia* videoList = iCollection.iCache->iVideoList;
    
    TInt categoryIndex = CategoryIndex( aOrigin );
     
    prevNewVideoName.Set( iListArray->AtL( categoryIndex )->ValueText(
            KVcxMediaMyVideosCategoryNewItemName ) );
    CMPXMedia* newVideo;
    currentNewVideoName.Set( CalculateNewVideoNameL(
            *videoList, aOrigin, aIgnoredIds, newVideo ) );
    if ( currentNewVideoName.CompareF( prevNewVideoName ) != 0 )
        {
        iListArray->AtL( categoryIndex )->SetTextValueL(
                KVcxMediaMyVideosCategoryNewItemName, currentNewVideoName );
        MPX_DEBUG3("CVcxMyVideosCategories:: Category[%d] New Video Name changed to %S",
                categoryIndex, &currentNewVideoName);

        if ( newVideo )
            {
            TInt64 newDate( 0 );
            if ( newVideo->IsSupported( KMPXMediaGeneralDate ) )
                {
                newDate = newVideo->ValueTObjectL<TInt64>( KMPXMediaGeneralDate );
                }
                
            iListArray->AtL( categoryIndex )->SetTObjectValueL( KMPXMediaGeneralDate, newDate );
            }
        else
            {
            iListArray->AtL( categoryIndex )->SetTObjectValueL( KMPXMediaGeneralDate, 0 );
            }
            
        MPX_DEBUG1("CVcxMyVideosCategories:: adding event");
        
        TMPXItemId mpxId = iListArray->AtL( categoryIndex )->ValueTObjectL<TMPXItemId>(
                KMPXMediaGeneralId );
        iCollection.iMessageList->AddEventL( mpxId, EMPXItemModified );

        aModified = ETrue;
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::CalculateNewVideoNameL
// ----------------------------------------------------------------------------
//    
const TDesC& CVcxMyVideosCategories::CalculateNewVideoNameL( CMPXMedia& aVideoList,
        TInt aOrigin, RArray<TUint32>& aIgnoredIds, CMPXMedia*& aNewVideo )
    {
    MPX_FUNC("CVcxMyVideosCategories::CalculateNewVideoNameL");

    if ( aOrigin == EVcxMyVideosOriginSideLoaded )
        {
        aOrigin = EVcxMyVideosOriginOther;
        }
        
    CMPXMediaArray* videoArray = aVideoList.Value<CMPXMediaArray>(
            KMPXMediaArrayContents );
    
    TInt64 latestCreationDate = 0;
    TInt64 currentItemsCreationDate;
    TInt currentLatestItemIndex   = -1;
    TUint8 origin;
    TUint32 flags;
    CMPXMedia* video;
    TInt count = videoArray->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        video = videoArray->AtL( i );

        // Handle only items which are not on ignore list (aIgnoredIds).
        // Items on ignore list are going to be deleted, they just hang around
        // on the list at this point.
        if ( aIgnoredIds.Find( video->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ).iId1 )
                == KErrNotFound )
            {
            // IsSupported check omitted since this value should be set always
            flags = video->ValueTObjectL<TUint32>( KMPXMediaGeneralFlags );
            if ( flags & EVcxMyVideosVideoNew )
                {
                origin = TVcxMyVideosCollectionUtil::OriginL( *video );
                if ( aOrigin == -1 || aOrigin == origin )
                    {
                    currentItemsCreationDate = video->ValueTObjectL<TInt64>(
                            KMPXMediaGeneralDate ); 
                    if ( latestCreationDate < currentItemsCreationDate )
                        {
                        latestCreationDate     = currentItemsCreationDate;
                        currentLatestItemIndex = i;
                        aNewVideo              = video;
                        }
                    }
                }
            }
        else
            {
            MPX_DEBUG1("CVcxMyVideosCategories:: item is on ignore list, skipping");
            }
        }

    if ( currentLatestItemIndex == -1 )
        {
        aNewVideo = NULL;
        return KNullDesC;
        }
    else
        {
        MPX_DEBUG2("CVcxMyVideosCategories:: returning %S",
                &videoArray->AtL( currentLatestItemIndex )->ValueText( KMPXMediaGeneralTitle ) );
        return videoArray->AtL( currentLatestItemIndex )->ValueText( KMPXMediaGeneralTitle );
        }
        
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::UpdateCategoryNewVideoNameAndDateL
// Called when video is updated in a way that it affects category new
// video name.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::UpdateCategoryNewVideoNameAndDateL( CMPXMedia& aVideo,
        TBool& aModified )
    {
    //       mpx items for current category new video names should be saved and used to
    //       speed up this func
    
    TBool modified = EFalse;
    RArray<TUint32> ignoredIds;
    ignoredIds.Reset();
    CleanupClosePushL( ignoredIds ); // 1->
    UpdateCategoryNewVideoNameAndDateL( -1 /* -1 origin resolves to All category */, modified,
            ignoredIds );
    UpdateCategoryNewVideoNameAndDateL( TVcxMyVideosCollectionUtil::OriginL( aVideo ), modified,
            ignoredIds );

    if ( modified )
        {
        aModified = ETrue;
        }
    CleanupStack::PopAndDestroy( &ignoredIds ); // <-1
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::GenerateCategoryModifiedEventL
// Called when video is updated in a way that it modifies categories in a way
// that event is sent.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::GenerateCategoryModifiedEventL( CMPXMedia& aMedia,
        TBool aFlushMessageList, TInt32 aExtraInfo )
    {
    TMPXItemId mpxId = iListArray->AtL( 0 /* All category*/ )->ValueTObjectL<TMPXItemId>(
            KMPXMediaGeneralId );
    
    MPX_DEBUG2("CVcxMyVideosCategories:: adding modify event for category[0], extra info = %d", aExtraInfo );
    iCollection.iMessageList->AddEventL( mpxId, EMPXItemModified, aExtraInfo );
    mpxId = iListArray->AtL( CategoryIndex(
            TVcxMyVideosCollectionUtil::OriginL( aMedia ) ) )->ValueTObjectL<TMPXItemId>(
            KMPXMediaGeneralId );

    MPX_DEBUG3("CVcxMyVideosCategories:: adding modify event for category[%d], extra info = %d", mpxId.iId1, aExtraInfo );
    iCollection.iMessageList->AddEventL( mpxId, EMPXItemModified, aExtraInfo );

    if ( aFlushMessageList )
        {
        iCollection.iMessageList->SendL();
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::OriginChangedL
// Called when videos origin is changed (Video moves from one category to other).
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::OriginChangedL( CMPXMedia& aVideo, TUint8 aPrevOrigin,
        TUint8 aNewOrigin )
    {
    CMPXMediaArray* categoryArray = iList->Value<CMPXMediaArray>( KMPXMediaArrayContents );

    UpdateVideosCountL( *(categoryArray->AtL( CategoryIndex( aPrevOrigin ) )),
            -1, KVcxMediaMyVideosCategoryItemCount, EVcxMyVideosVideoListOrderChanged ); //event added but not sent

    UpdateVideosCountL( *(categoryArray->AtL( CategoryIndex( aNewOrigin ) )),
            1, KVcxMediaMyVideosCategoryItemCount, EVcxMyVideosVideoListOrderChanged ); //event added but not sent

    if ( TVcxMyVideosCollectionUtil::FlagsL( aVideo ) & EVcxMyVideosVideoNew )
        {
        UpdateVideosCountL( *(categoryArray->AtL( CategoryIndex( aPrevOrigin ) )),
                -1, KVcxMediaMyVideosCategoryNewItemCount, EVcxMyVideosVideoListOrderChanged ); //this event will be optimized away at iCollection.iMessageList

        UpdateVideosCountL( *(categoryArray->AtL( CategoryIndex( aNewOrigin ) )),
                1, KVcxMediaMyVideosCategoryNewItemCount, EVcxMyVideosVideoListOrderChanged ); //this event will be optimized away at iCollection.iMessageList
        }
    
    RArray<TUint32> ignoredIds;
    ignoredIds.Reset();
    CleanupClosePushL( ignoredIds ); // 1->
    TBool modified = EFalse;
    UpdateCategoryNewVideoNameAndDateL( aPrevOrigin, modified, ignoredIds );
    UpdateCategoryNewVideoNameAndDateL( aNewOrigin, modified, ignoredIds );

    CleanupStack::PopAndDestroy( &ignoredIds ); // <-1
    iCollection.iMessageList->SendL();
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::ResetModifiedCategoriesArray
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::ResetModifiedCategoriesArray()
    {
    TInt count = iModifiedCategories.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iModifiedCategories[i] = EFalse;
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::AddEventsFromModifiedCategoriesArrayL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::AddEventsFromModifiedCategoriesArrayL()
    {
    TMPXItemId mpxId;
    TInt count = iModifiedCategories.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iModifiedCategories[i] )
            {
            MPX_DEBUG2("CVcxMyVideosCategories:: category[%d] modified, adding event.", i);
            mpxId = iListArray->AtL(i)->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
            iCollection.iMessageList->AddEventL( mpxId, EMPXItemModified );
            }
        }
    }

// End of file

