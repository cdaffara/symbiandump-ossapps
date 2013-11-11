/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class representing album in My Videos collection.
*/

#include <mpxlog.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include "vcxmyvideosalbum.h"
#include "vcxmyvideoscollectionplugin.h"
#include "vcxmyvideosvideocache.h"
#include "vcxmyvideoscollectionutil.h"


// ---------------------------------------------------------------------------
// TVcxMyVideosAlbumVideo::TVcxMyVideosAlbumVideo
// ---------------------------------------------------------------------------
//
TVcxMyVideosAlbumVideo::TVcxMyVideosAlbumVideo()
    {
    Set( 0, 0, 0 );
    }

// ---------------------------------------------------------------------------
// TVcxMyVideosAlbumVideo::operator=
// ---------------------------------------------------------------------------
//                
TVcxMyVideosAlbumVideo& TVcxMyVideosAlbumVideo::operator=( const TVcxMyVideosAlbumVideo& aVideo )
    {
    Set( aVideo.iMdsId, aVideo.iRelationMdsId, aVideo.iMedia );
    return *this;
    }

// ---------------------------------------------------------------------------
// TVcxMyVideosAlbumVideo::Set
// ---------------------------------------------------------------------------
//            
void TVcxMyVideosAlbumVideo::Set( TUint32 aMdsId, TUint32 aRelationMdsId, CMPXMedia* aVideo )
    {
    iMdsId         = aMdsId;
    iRelationMdsId = aRelationMdsId;
    iMedia         = aVideo;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::CVcxMyVideosAlbum
// ---------------------------------------------------------------------------
//
CVcxMyVideosAlbum::CVcxMyVideosAlbum( CVcxMyVideosCollectionPlugin& aCollectionPlugin )
: iCollection( aCollectionPlugin )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::~CVcxMyVideosAlbum
// ---------------------------------------------------------------------------
//
CVcxMyVideosAlbum::~CVcxMyVideosAlbum()
    {
    iVideoList.Close();
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::NewLC
// ---------------------------------------------------------------------------
//
CVcxMyVideosAlbum* CVcxMyVideosAlbum::NewLC( CVcxMyVideosCollectionPlugin& aCollectionPlugin )
    {
    CVcxMyVideosAlbum* self = new (ELeave) CVcxMyVideosAlbum( aCollectionPlugin );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::NewL
// ---------------------------------------------------------------------------
//
CVcxMyVideosAlbum* CVcxMyVideosAlbum::NewL( CVcxMyVideosCollectionPlugin& aCollectionPlugin )
    {
    CVcxMyVideosAlbum* self = CVcxMyVideosAlbum::NewLC( aCollectionPlugin );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::ConstructL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosAlbum::ConstructL()
    {
    iVideoList.Reset();
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::Sort
// ---------------------------------------------------------------------------
//
void CVcxMyVideosAlbum::Sort()
    {
    const TLinearOrder<TVcxMyVideosAlbumVideo> KOrderByMdsId(
            CVcxMyVideosAlbum::CompareVideosByMdsId );

    iVideoList.Sort( KOrderByMdsId );
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::SetL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosAlbum::SetL( /*CMPXMedia& aAlbum*/ )
    {
    //TODO:
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::BelongsToAlbum
// ---------------------------------------------------------------------------
//
TBool CVcxMyVideosAlbum::BelongsToAlbum( TUint32 aMdsId )
    {
    const TLinearOrder<TVcxMyVideosAlbumVideo> KOrderByMdsId(
            CVcxMyVideosAlbum::CompareVideosByMdsId );
    
    TInt index;
    TVcxMyVideosAlbumVideo video;
    video.iMdsId = aMdsId;
    TInt err = iVideoList.FindInOrder( video, index, KOrderByMdsId );
    
    if ( err != KErrNone )
        {
        return EFalse;
        }
    
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::Video
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosAlbum::Video( TUint32 aMdsId, TVcxMyVideosAlbumVideo& aVideo, TInt& aIndex )
    {
    const TLinearOrder<TVcxMyVideosAlbumVideo> KOrderByMdsId(
            CVcxMyVideosAlbum::CompareVideosByMdsId );
    
    TVcxMyVideosAlbumVideo videoToFind;
    videoToFind.iMdsId = aMdsId;
    TInt err = iVideoList.FindInOrder( videoToFind, aIndex, KOrderByMdsId );

    if ( err == KErrNone )
        {
        aVideo = iVideoList[aIndex];
        }
   
    return err;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::AddL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosAlbum::AddL( TVcxMyVideosAlbumVideo aVideo )
    {
    const TLinearOrder<TVcxMyVideosAlbumVideo> KOrderByMdsId(
            CVcxMyVideosAlbum::CompareVideosByMdsId );

    iVideoList.InsertInOrderAllowRepeatsL( aVideo, KOrderByMdsId );
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::Remove
// ---------------------------------------------------------------------------
//
void CVcxMyVideosAlbum::Remove( RArray<TUint32>& aMdsIds )
    {
    TInt count = aMdsIds.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        Remove( aMdsIds[i], EFalse /* don't compress */ );
        }
    iVideoList.Compress();
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::Remove
// ---------------------------------------------------------------------------
//
void CVcxMyVideosAlbum::Remove( TUint32 aMdsId, TBool aCompress )
    {
    const TLinearOrder<TVcxMyVideosAlbumVideo> KOrderByMdsId(
            CVcxMyVideosAlbum::CompareVideosByMdsId );

    TVcxMyVideosAlbumVideo video;
    video.iMdsId = aMdsId;
    TInt pos;
    TInt err = iVideoList.FindInOrder( video, pos, KOrderByMdsId );
    if ( err == KErrNone )
        {
        iVideoList.Remove( pos );
        }
    if ( aCompress )
        {
        iVideoList.Compress();
        }
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::CompareVideosByMdsId
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosAlbum::CompareVideosByMdsId( const TVcxMyVideosAlbumVideo& aVideo1,
        const TVcxMyVideosAlbumVideo& aVideo2 )
    {
    if ( aVideo1.iMdsId == aVideo2.iMdsId )
        {
        return 0;
        }

    if ( aVideo1.iMdsId < aVideo2.iMdsId )
        {
        return -1;
        }
    return 1;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::CreateVideoListL
// Creates album video list from iCollection.iCache->iVideoList.
// ---------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosAlbum::CreateVideoListL()
    {
    CMPXMedia* toVideoList = TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
    CleanupStack::PushL( toVideoList );

    AppendToVideoListL( *iCollection.iCache->iVideoList, *toVideoList, 0 );
    
    CleanupStack::Pop( toVideoList );

    return toVideoList;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::CreateVideoListL
// Appends to video list the items which belong to this album.
// ---------------------------------------------------------------------------
//
void CVcxMyVideosAlbum::AppendToVideoListL( CMPXMedia& aFromVideoList,
        CMPXMedia& aToVideoList, TInt aNewItemStartIndex )
    {
    if ( aNewItemStartIndex < 0 )
        {
        return;
        }
    
    CMPXMediaArray* fromArray = aFromVideoList.Value<CMPXMediaArray>( KMPXMediaArrayContents ); 
    CMPXMediaArray* toArray   = aToVideoList.Value<CMPXMediaArray>( KMPXMediaArrayContents ); 
    TUint32 mdsId;
    CMPXMedia* fromVideo;
    TInt count = fromArray->Count();
    for ( TInt i = aNewItemStartIndex; i < count; i++ )
        {
        fromVideo = fromArray->AtL( i );
        mdsId = TVcxMyVideosCollectionUtil::IdL( *fromVideo ).iId1;
        if ( BelongsToAlbum( mdsId ) )
            {
            CMPXMedia* toVideo = CMPXMedia::NewL( *fromVideo ); //share underlying memory
            CleanupStack::PushL( toVideo );
            toArray->AppendL( toVideo );
            CleanupStack::Pop( toVideo );
            }
        }
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::CalculateAttributesL
// ---------------------------------------------------------------------------
//
TBool CVcxMyVideosAlbum::CalculateAttributesL()
    {
    TInt videoCount = 0;
    TUint32 count = iVideoList.Count();
    TInt newCount = 0;
    CMPXMedia* video;
    CMPXMedia* latestNewVideo = NULL;
    TInt pos;
    TUint32 flags;
    TInt64 currentItemsCreationDate = 0;
    TInt64 latestCreationDate = TVcxMyVideosCollectionUtil::CreationDateL( *iMedia );
    for ( TInt i = 0; i < count; i++ )
        {
        video = iCollection.iCache->FindVideoByMdsIdL( iVideoList[i].iMdsId, pos );
        if ( video )
            {
            videoCount++;
            flags = TVcxMyVideosCollectionUtil::FlagsL( *video );
            if ( flags & EVcxMyVideosVideoNew )
                {
                newCount++;
                currentItemsCreationDate = TVcxMyVideosCollectionUtil::CreationDateL( *video );
                if ( latestCreationDate < currentItemsCreationDate )
                    {
                    latestCreationDate = currentItemsCreationDate;
                    latestNewVideo     = video;
                    }
                }
            }
        }
    
    TBool modified = EFalse;
    
    TUint32 prevValue = TVcxMyVideosCollectionUtil::CategoryItemCountL( *iMedia );
    if ( prevValue != videoCount )
        {
        iMedia->SetTObjectValueL<TUint32>( KVcxMediaMyVideosCategoryItemCount, videoCount );
        modified = ETrue;
        }

    prevValue = TVcxMyVideosCollectionUtil::CategoryNewItemCountL( *iMedia );
    if ( prevValue != newCount )
        {
        iMedia->SetTObjectValueL<TUint32>( KVcxMediaMyVideosCategoryNewItemCount, newCount );
        modified = ETrue;
        }
    
    if ( latestNewVideo )
        {
        TPtrC prevNewVideoName( TVcxMyVideosCollectionUtil::CategoryNewVideoName( *iMedia ) );
        TPtrC latestNewVideoName( TVcxMyVideosCollectionUtil::Title( *latestNewVideo ) );
        if ( prevNewVideoName != latestNewVideoName )
            {
            iMedia->SetTextValueL( KVcxMediaMyVideosCategoryNewItemName,
                    TVcxMyVideosCollectionUtil::Title( *latestNewVideo ) );
            iMedia->SetTObjectValueL<TInt64>( KMPXMediaGeneralDate, latestCreationDate );    
            modified = ETrue;
            }
        }
    
    return modified;
    }

