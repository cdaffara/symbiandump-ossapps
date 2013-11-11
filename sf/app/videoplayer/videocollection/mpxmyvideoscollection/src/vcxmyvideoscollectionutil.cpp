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
* Description:
*
*/



// INCLUDE FILES
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxlog.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <vcxmyvideosdefs.h>
#include <mpxmessagecontainerdefs.h>
#include <bautils.h>
#include <mpxmediavideodefs.h>
#ifdef _DEBUG
#include <flogger.h>
#include <f32file.h> 
#endif
#include "vcxmyvideoscollectionutil.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtil::CreateEmptyMediaListL
// ----------------------------------------------------------------------------
//
CMPXMedia* TVcxMyVideosCollectionUtil::CreateEmptyMediaListL()
    {
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds ); // 1->
                
    supportedIds.AppendL( KMPXMediaIdContainer );
    supportedIds.AppendL( KMPXMediaIdGeneral );

    //create container
    CMPXMedia* container = CMPXMedia::NewL( supportedIds.Array() );
    CleanupStack::PushL( container ); // 2->

    CMPXMediaArray* mediaArray = CMPXMediaArray::NewL();
    CleanupStack::PushL( mediaArray ); // 3->

    container->SetCObjectValueL( KMPXMediaArrayContents, mediaArray );

    CleanupStack::PopAndDestroy( mediaArray );    // <-3
    CleanupStack::Pop( container );               // <-2    
    CleanupStack::PopAndDestroy( &supportedIds ); // <-1
    return container;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtil::CreateEmptyMessageListL
// ----------------------------------------------------------------------------
//
CMPXMedia* TVcxMyVideosCollectionUtil::CreateEmptyMessageListL()
    {
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds ); // 1->
                
    supportedIds.AppendL( KMPXMediaIdContainer );

    //create container
    CMPXMedia* container = CMPXMedia::NewL( supportedIds.Array() );
    CleanupStack::PushL( container ); // 2->

    CMPXMediaArray* messageArray = CMPXMediaArray::NewL();
    CleanupStack::PushL( messageArray ); // 3->

    container->SetCObjectValueL( KMPXMessageArrayContents, messageArray );

    CleanupStack::PopAndDestroy( messageArray );  // <-3
    CleanupStack::Pop( container );               // <-2
    CleanupStack::PopAndDestroy( &supportedIds ); // <-1
    return container;
    }

// ---------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::CopyFromListToListL
// ---------------------------------------------------------------------------
//
void TVcxMyVideosCollectionUtil::CopyFromListToListL(
        CMPXMedia& aFromList,
        CMPXMedia& aToList,
        RArray<TUint32>& aMdsIds )
    {
    MPX_FUNC("TVcxMyVideosCollectionUtil::CopyFromListToListL ()");
    
    if ( !aFromList.IsSupported( KMPXMediaArrayContents ) ||
            !aToList.IsSupported( KMPXMediaArrayContents ) )
        {
        MPX_DEBUG1("TVcxMyVideosCollectionUtil::CopyFromListToListL left, media array(s) missing. ");
        User::Leave( KErrArgument );
        }

    TInt mdsIdsCount          = aMdsIds.Count();
    CMPXMediaArray* fromArray = aFromList.Value<CMPXMediaArray>( KMPXMediaArrayContents );
    CMPXMediaArray* toArray   = aToList.Value<CMPXMediaArray>( KMPXMediaArrayContents );
    TInt fromArrayCount       = fromArray->Count();
    TMPXItemId fromId;
    TMPXItemId toId;

    for ( TInt i = 0; i < mdsIdsCount; i++ )
        {
        for ( TInt j = 0; j < fromArrayCount; j++ )
            {
            if ( aMdsIds[i] ==
                    fromArray->AtL( j )->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ).iId1 )
                {
                toArray->AppendL( *fromArray->AtL( j ) );
                MPX_DEBUG3("TVcxMyVideosCollectionUtil:: copying %d (from pointer %x)", aMdsIds[i], fromArray->AtL( j ));
                break;
                }                
            }
        }
    }

// ---------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::CopyFromListToListL
// ---------------------------------------------------------------------------
//
void TVcxMyVideosCollectionUtil::CopyFromListToListL(
        RArray<CMPXMedia*>& aFromArray,
        CMPXMedia& aToList,
        RArray<TUint32>& aMdsIds )
    {
    MPX_FUNC("TVcxMyVideosCollectionUtil::CopyFromListToListL( RArray version )");
    
    if ( !aToList.IsSupported( KMPXMediaArrayContents ) )
        {
        MPX_DEBUG1("TVcxMyVideosCollectionUtil::CopyFromListToListL left, media array missing. ");
        User::Leave( KErrArgument );
        }

    TInt mdsIdsCount          = aMdsIds.Count();
    CMPXMediaArray* toArray   = aToList.Value<CMPXMediaArray>( KMPXMediaArrayContents );
    TInt fromArrayCount       = aFromArray.Count();
    TMPXItemId fromId;
    TMPXItemId toId;

    for ( TInt i = 0; i < mdsIdsCount; i++ )
        {
        for ( TInt j = 0; j < fromArrayCount; j++ )
            {
            if ( aMdsIds[i] ==
                    aFromArray[j]->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ).iId1 )
                {
                toArray->AppendL( *aFromArray[j] );
                MPX_DEBUG3("TVcxMyVideosCollectionUtil:: copying %d (from pointer %x)", aMdsIds[i], aFromArray[j]);
                break;
                }                
            }
        }
    }

// ---------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::AppendToListL
// New items in aToList will point to same shared memory as items in aFromList.
// ---------------------------------------------------------------------------
//
void TVcxMyVideosCollectionUtil::AppendToListL( CMPXMedia& aToList, CMPXMedia& aFromList )
    {
    CMPXMedia* media;
    CMPXMediaArray* toArray   = aToList.Value<CMPXMediaArray>( KMPXMediaArrayContents );
    CMPXMediaArray* fromArray = aFromList.Value<CMPXMediaArray>( KMPXMediaArrayContents );
    TInt count = fromArray->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        media = CMPXMedia::NewL( *(fromArray->AtL( i )) ); // points to same shared memory
        CleanupStack::PushL( media );
        toArray->AppendL( media ); //ownership moves
        CleanupStack::Pop( media );
        }
    }

// ---------------------------------------------------------------------------
// This helper function converts drive letter to drive number
// ---------------------------------------------------------------------------
//
TInt TVcxMyVideosCollectionUtil::DriveLetter2DriveNumber( const TDesC &aDrive )
    {
    TInt    retVal = KErrArgument;

    if (aDrive.Length() > 0)
        {
        TChar letter = aDrive[0];
    	letter.UpperCase();
    	if (letter >= 'A' && letter <= 'Z')
    		{
    		retVal = (TInt)letter - 'A';
    		}
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::MakeUniqueFileNameL
// ---------------------------------------------------------------------------
//
void TVcxMyVideosCollectionUtil::MakeUniqueFileNameL( RFs& aFs, const TDesC& aPath,
        TDes& aUniquePath )
    {
    const TInt KMaxDigits     = 7;
    TBuf<KMaxDigits> counterDigits;
    
    aUniquePath = aPath;
    TInt counter = 1;
    //TInt zeros   = 0;
    TInt pos     = 0;
    while( BaflUtils::FileExists( aFs, aUniquePath ) )
        {
        pos   = aPath.Locate( '.' );
        if ( pos == KErrNotFound )
            {
            aUniquePath  = aPath;
            }
        else
            {
            aUniquePath  = aPath.Mid( 0, pos );
            }
        _LIT(KPercentD, "%d");
        counterDigits.Format( KPercentD, counter++ );
        aUniquePath.Append( '_' );
        aUniquePath.Append( '(' );
        aUniquePath.AppendJustify( counterDigits, KMaxDigits, TAlign( ERight ), '0' );
        aUniquePath.Append( ')' );
        if ( pos != KErrNotFound )
            {
            aUniquePath.Append( aPath.Mid( pos ) );
            }
        }
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::OriginL
// ----------------------------------------------------------------------------
//
TUint8 TVcxMyVideosCollectionUtil::OriginL( CMPXMedia& aVideo )
    {
    TUint8 origin = EVcxMyVideosOriginOther;
                            
    if ( aVideo.IsSupported( KVcxMediaMyVideosOrigin ) )
        {
        origin = aVideo.ValueTObjectL<TUint8>( KVcxMediaMyVideosOrigin );                
#ifndef VCX_DOWNLOADS_CATEGORY
        if ( origin != EVcxMyVideosOriginCapturedWithCamera )
            {
            origin = EVcxMyVideosOriginOther;
            }
#else
        if ( origin != EVcxMyVideosOriginCapturedWithCamera &&
				 origin != EVcxMyVideosOriginDownloaded
				)
            {
            origin = EVcxMyVideosOriginOther;
            }
#endif
        }
    return origin;
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::FlagsL
// ----------------------------------------------------------------------------
//
TUint32 TVcxMyVideosCollectionUtil::FlagsL( CMPXMedia& aVideo )
    {
    TUint32 flags = 0;
                            
    if ( aVideo.IsSupported( KMPXMediaGeneralFlags ) )
        {
        flags = aVideo.ValueTObjectL<TUint32>( KMPXMediaGeneralFlags );                
        }
    return flags;
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::CreationDateL
// ----------------------------------------------------------------------------
//
TInt64 TVcxMyVideosCollectionUtil::CreationDateL( CMPXMedia& aVideo )
    {
    TInt64 creationDate = 0;
                            
    if ( aVideo.IsSupported( KMPXMediaGeneralDate ) )
        {
        creationDate = aVideo.ValueTObjectL<TInt64>( KMPXMediaGeneralDate );                
        }
    return creationDate;
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::DownloadIdL
// ----------------------------------------------------------------------------
//
TUint32 TVcxMyVideosCollectionUtil::DownloadIdL( CMPXMedia& aVideo )
    {
    TUint32 downloadId = 0;
                            
    if ( aVideo.IsSupported( KVcxMediaMyVideosDownloadId ) )
        {
        downloadId = aVideo.ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId );                
        }
    return downloadId;
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::DownloadStateL
// ----------------------------------------------------------------------------
//
TVcxMyVideosDownloadState TVcxMyVideosCollectionUtil::DownloadStateL( CMPXMedia& aVideo )
    {
    TVcxMyVideosDownloadState downloadState = EVcxMyVideosDlStateNone;
                            
    if ( aVideo.IsSupported( KVcxMediaMyVideosDownloadState ) )
        {
        downloadState = static_cast<TVcxMyVideosDownloadState>(
                aVideo.ValueTObjectL<TUint8>( KVcxMediaMyVideosDownloadState ) );
        }
    return downloadState;
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::IdL
// ----------------------------------------------------------------------------
//
TMPXItemId TVcxMyVideosCollectionUtil::IdL( const CMPXMedia& aVideo )
    {
    TMPXItemId id( 0, 0 );
                            
    if ( aVideo.IsSupported( KMPXMediaGeneralId ) )
        {
        id = aVideo.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
        }
    return id;
    }



// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::DurationL
// ----------------------------------------------------------------------------
//
TReal32 TVcxMyVideosCollectionUtil::DurationL( CMPXMedia& aVideo )
    {
    TReal32 duration = -1;
                            
    if ( aVideo.IsSupported( KVcxMediaMyVideosDuration ) )
        {
        duration = aVideo.ValueTObjectL<TReal32>( KVcxMediaMyVideosDuration );
        }
    return duration;
    }


// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::Title
// ----------------------------------------------------------------------------
//
const TDesC& TVcxMyVideosCollectionUtil::Title( const CMPXMedia& aVideo )
    {                            
    if ( aVideo.IsSupported( KMPXMediaGeneralTitle ) )
        {
        return aVideo.ValueText( KMPXMediaGeneralTitle );
        }
    else
        {
        return KNullDesC;
        }
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::RatingL
// ----------------------------------------------------------------------------
//
TUint8 TVcxMyVideosCollectionUtil::RatingL( CMPXMedia& aVideo )
    {
    TUint8 rating = 0;
                            
    if ( aVideo.IsSupported( KVcxMediaMyVideosRating ) )
        {
        rating = aVideo.ValueTObjectL<TUint8>( KVcxMediaMyVideosRating );                
        }
    return rating;
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::AudioFourCcL
// ----------------------------------------------------------------------------
//
TUint32 TVcxMyVideosCollectionUtil::AudioFourCcL( CMPXMedia& aVideo )
    {
    TUint32 audioFourCc = 0;
                            
    if ( aVideo.IsSupported( KVcxMediaMyVideosAudioFourCc ) )
        {
        audioFourCc = aVideo.ValueTObjectL<TUint32>( KVcxMediaMyVideosAudioFourCc );                
        }
    return audioFourCc;
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::AreSupported
// ----------------------------------------------------------------------------
//
TBool TVcxMyVideosCollectionUtil::AreSupported( CMPXMedia& aVideo,
        const TArray<TMPXAttribute>& aAttrs,
        TBool& aNonSupportedAttrCanBeFoundFromMds )
    {
    MPX_FUNC("TVcxMyVideosCollectionUtil::AreSupported()");

    TBool areSupported( ETrue );
    aNonSupportedAttrCanBeFoundFromMds = EFalse;
    
    for ( TInt i = 0; i < aAttrs.Count(); i++ )
        {
        if ( !aVideo.IsSupported( aAttrs[i] ) )
            {
            MPX_DEBUG3("  attr (%x)(%x) not supported in media object", aAttrs[i].ContentId(),
                    aAttrs[i].AttributeId() );

            areSupported = EFalse;
            
            if ( AttrBelongsToFullSet( aAttrs[i] ) )
                {
                MPX_DEBUG1("  attr can be found from mds");
                aNonSupportedAttrCanBeFoundFromMds = ETrue;
                return areSupported; // We can return since areSupported and aNonSupportedAttrCanBeFoundFromMds
                                     // won't change anymore, no matter what rest of the attributes are.
                                     // Fetching from MDS is justified.
                }
            else
                {
                MPX_DEBUG1("  attr can NOT be found from mds");
                }
            }
        }

    return areSupported;
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::AttrBelongsToFullSet
// ----------------------------------------------------------------------------
//
TBool TVcxMyVideosCollectionUtil::AttrBelongsToFullSet( const TMPXAttribute& aAttr )
    {
    // these attributes are saved to MDS but not to cache.
    if ( aAttr == KMPXMediaGeneralComment ||
        aAttr == KMPXMediaGeneralCopyright ||
        aAttr == KMPXMediaGeneralMimeType ||
        aAttr == KMPXMediaGeneralLastPlaybackPosition ||
        aAttr == KVcxMediaMyVideosModifiedDate ||
        aAttr == KVcxMediaMyVideosAudioLanguage ||
        aAttr == KVcxMediaMyVideosAuthor ||
        aAttr == KMPXMediaVideoBitRate ||
        aAttr == KMPXMediaVideoHeight ||
        aAttr == KMPXMediaVideoWidth ||
        aAttr == KMPXMediaVideoArtist )
        {
        return ETrue;
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::Origin
// ----------------------------------------------------------------------------
//
TInt TVcxMyVideosCollectionUtil::Origin( TInt aCategoryId )
    {
    switch ( aCategoryId )
        {
        case KVcxMvcCategoryIdAll:
            return KErrNotFound;

        case KVcxMvcCategoryIdDownloads:
            return EVcxMyVideosOriginDownloaded;

        case KVcxMvcCategoryIdTvRecordings:
            return EVcxMyVideosOriginTvRecording;
            
        case KVcxMvcCategoryIdCaptured:
            return EVcxMyVideosOriginCapturedWithCamera;
            
        case KVcxMvcCategoryIdOther:
            return EVcxMyVideosOriginOther;
        }
    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::MediaArrayL
// ----------------------------------------------------------------------------
//
CMPXMediaArray* TVcxMyVideosCollectionUtil::MediaArrayL( const CMPXMedia& aMedia )
    {
    if ( !aMedia.IsSupported( KMPXMediaArrayContents ) )
        {
        User::Leave( KErrArgument );
        }

    return aMedia.Value<CMPXMediaArray>( KMPXMediaArrayContents );
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::Int32ValueL
// ----------------------------------------------------------------------------
//
TInt32 TVcxMyVideosCollectionUtil::Int32ValueL( CMPXMedia& aMedia )
    {
    if ( !aMedia.IsSupported( KVcxMediaMyVideosInt32Value ) )
        {
        User::Leave( KErrArgument );
        }
    return aMedia.ValueTObjectL<TInt32>( KVcxMediaMyVideosInt32Value );
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::Uint32ValueL
// ----------------------------------------------------------------------------
//
TUint32 TVcxMyVideosCollectionUtil::Uint32ValueL( CMPXMedia& aMedia )
    {
    if ( !aMedia.IsSupported( KVcxMediaMyVideosUint32Value ) )
        {
        User::Leave( KErrArgument );
        }
    return aMedia.ValueTObjectL<TUint32>( KVcxMediaMyVideosUint32Value );
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::CategoryItemCountL
// ----------------------------------------------------------------------------
//
TUint32 TVcxMyVideosCollectionUtil::CategoryItemCountL( CMPXMedia& aVideo )
    {
    TUint32 categoryItemCount = 0;
                            
    if ( aVideo.IsSupported( KVcxMediaMyVideosCategoryItemCount ) )
        {
        categoryItemCount = aVideo.ValueTObjectL<TUint32>( KVcxMediaMyVideosCategoryItemCount );                
        }
    return categoryItemCount;
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::CategoryNewItemCountL
// ----------------------------------------------------------------------------
//
TUint32 TVcxMyVideosCollectionUtil::CategoryNewItemCountL( CMPXMedia& aVideo )
    {
    TUint32 categoryNewItemCount = 0;
                            
    if ( aVideo.IsSupported( KVcxMediaMyVideosCategoryNewItemCount ) )
        {
        categoryNewItemCount = aVideo.ValueTObjectL<TUint32>( KVcxMediaMyVideosCategoryNewItemCount );                
        }
    return categoryNewItemCount;
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::CategoryNewVideoName
// ----------------------------------------------------------------------------
//
const TDesC& TVcxMyVideosCollectionUtil::CategoryNewVideoName( const CMPXMedia& aVideo )
    {                            
    if ( aVideo.IsSupported( KVcxMediaMyVideosCategoryNewItemName ) )
        {
        return aVideo.ValueText( KVcxMediaMyVideosCategoryNewItemName );
        }
    else
        {
        return KNullDesC;
        }
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::GetIdsFromMediaArrayL
// ----------------------------------------------------------------------------
//
void TVcxMyVideosCollectionUtil::GetIdsFromMediaArrayL( CMPXMediaArray& aMediaArray,
        RArray<TUint32>& aIdArray )
    {
    TInt count = aMediaArray.Count();
    aIdArray.Reset();
    aIdArray.ReserveL( count );
    for ( TInt i = 0; i < count; i++ )
        {
        aIdArray.AppendL( IdL( *aMediaArray.AtL( i ) ).iId1 );
        }
    }

#ifdef _DEBUG
// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::PrintOpenFileHandlesL
// ----------------------------------------------------------------------------
//
void TVcxMyVideosCollectionUtil::PrintOpenFileHandlesL( const TDesC& aFileName, RFs& aFs )
    {

#ifdef _DEBUG
    TPtrC fileName( aFileName.Mid(2) );
    MPX_DEBUG2("TVcxMyVideosCollectionUtil :: Checking open file handles to %S", &fileName);
#endif

    CFileList* fileList = 0;

    TOpenFileScan fileScan( aFs );

    fileScan.NextL( fileList );   
  
    while ( fileList )   
        {
        for (TInt i = 0; i < fileList->Count(); i++ )   
            {   
            if ( (*fileList)[i].iName == aFileName.Mid(2) )
                {
                TFullName processName;
                GetProcessName( fileScan.ThreadId(), processName );
                MPX_DEBUG3("TVcxMyVideosCollectionUtil:: %S has a file handle open to %S.", &processName, &(*fileList)[i].iName);
                }
            }

        fileScan.NextL( fileList );   
        }
    }   

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::GetProcessName
// ----------------------------------------------------------------------------
//
void TVcxMyVideosCollectionUtil::GetProcessName( TInt aThreadId, TFullName& aProcessName )
    {
    _LIT(KAsterixDes, "*");
  	TFindThread find( KAsterixDes );
  	while( find.Next( aProcessName ) == KErrNone )
        {
      	RThread thread;
      	TInt err = thread.Open( aProcessName );
      	
      	if ( err == KErrNone )
      	    {
      	    if ( thread.Id().Id() ==  aThreadId )
      	        {
      	        aProcessName = thread.Name();
                thread.Close();
                return;
      	        }
      	         	
      	    thread.Close();
      	    }
        }
    aProcessName = KNullDesC;
    }   
#endif
   
// End of file
