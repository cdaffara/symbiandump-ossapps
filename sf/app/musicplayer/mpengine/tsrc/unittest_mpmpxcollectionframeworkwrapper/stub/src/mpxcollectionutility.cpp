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
* Description: mpxcollectionutility stub for testing mpmpxframeworkwrapper
*
*/


#include <mpxmedia.h>
#include <mpxmessage.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionmessage.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxcollectionplugin.hrh>
#include <mpxcollectionpath.h>
#include <mpxcollectionobserver.h>
#include "mptrace.h"
#include <e32debug.h>

#include "stub/inc/mpxcollectionutility.h"
const TInt KMPXPlaylistViewIndex = 1;

/*!
 Stub function.
*/
MMPXCollectionUtility::MMPXCollectionUtility():iBack(EFalse),
                                               iOpen(EFalse),
                                               iMedia(EFalse),
                                               iOpenCount(0),
                                               iCountPath(0),
                                               iIndex(0),
                                               iPlaylists(0),
                                               iAlbumSongs(0),
                                               iAsynchFindResult(0)
{
}

/*!
 Stub function.
*/
MMPXCollectionUtility::~MMPXCollectionUtility()
{
    if (iAlbumSongs)
    {
        delete iAlbumSongs;
    }
    if (iPlaylists)
    {
        delete iPlaylists;
    }
    if (iAsynchFindResult)
    {
        delete iAsynchFindResult;
    }
}

/*!
 Stub function.
*/
MMPXCollectionUtility* MMPXCollectionUtility::NewL(MMPXCollectionObserver* aObs, const TUid& aModeId)
{
    Q_UNUSED(aObs);
    Q_UNUSED(aModeId);
    MMPXCollectionUtility* temp = new MMPXCollectionUtility();
    return temp;
}

/*!
 Stub function.
*/
MMPXCollection& MMPXCollectionUtility::Collection()
{ 
    return *this;
}

/*!
 Stub function.
*/
void MMPXCollectionUtility::Close()
{
    delete this;
}

/*!
 Stub function.
*/
CMPXMedia* MMPXCollectionUtility::FindAllL(const CMPXSearchCriteria& aCriteria,
                                    const TArray<TMPXAttribute>& aAttrs)
{
    Q_UNUSED(aAttrs);
    
    // Find Playlists
    if(aCriteria.IsSupported(KMPXMediaGeneralType) && EMPXGroup == aCriteria.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType) &&
       aCriteria.IsSupported(KMPXMediaGeneralCategory) && EMPXPlaylist == aCriteria.ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory))
        {
        return CMPXMedia::NewL(*iPlaylists);
        }
    // Find Album with ID
    else if (aCriteria.IsSupported(KMPXMediaGeneralType) && EMPXGroup == aCriteria.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType) &&
       aCriteria.IsSupported(KMPXMediaGeneralCategory) && EMPXSong == aCriteria.ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory) &&
       aCriteria.IsSupported(KMPXMediaGeneralId) && iAlbumSongs &&
       //it is assumed that album ID matches index on the media array, just to make test logic more simple.
       aCriteria.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId).iId2 &&
       (aCriteria.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId).iId2 < iAlbumSongs->Count()) )
        {
        return CMPXMedia::NewL(*iAlbumSongs->AtL(aCriteria.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)));
        }
    else
        {
        return NULL;
        }
}
/*!
 Stub function.
*/
void MMPXCollectionUtility::FindAllL(const CMPXSearchCriteria& aCriteria,
                                    const TArray<TMPXAttribute>& aAttrs,
                                    MMPXCollectionFindObserver& aObs)
{
    Q_UNUSED(aObs);
    iAsynchFindResult = FindAllL(aCriteria, aAttrs);
}

/*!
 Stub function.
*/
void MMPXCollectionUtility::MediaL(const CMPXCollectionPath& aPath,
                                   const TArray<TMPXAttribute>& aAttrs,
                                   CMPXAttributeSpecs* aSpecs,
                                   CMPXFilter* aFilter)
{
    Q_UNUSED(aPath);
    Q_UNUSED(aAttrs);
    Q_UNUSED(aSpecs);
    Q_UNUSED(aFilter);
    iMedia = ETrue;
}

/*!
 Stub function.
*/
void MMPXCollectionUtility::CancelRequest()
{   
}

/*!
 Stub function.
*/
void MMPXCollectionUtility::BackL()
{
    iBack = ETrue;
}

/*!
 Stub function.
*/
CMPXCollectionPath* MMPXCollectionUtility::PathL()
{
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    path->AppendL( TUid::Uid(EMPXCollectionPluginMusic).iUid );
    path->AppendL( KMPXPlaylistViewIndex );
    //Fake some elements for the path
    for(TInt i = 0; i < 10; i++){
        path->InsertL(TUid::Uid(EMPXCollectionPluginMusic).iUid,0);
    }
    CleanupStack::Pop( path );
        
    return path;
}

/*!
 Stub function.
*/
TUid MMPXCollectionUtility::CollectionIDL(const TArray<TUid>& aUids)
{ 
    Q_UNUSED(aUids);
    return TUid::Uid(EMPXCollectionPluginMusic);
}

/*!
 Stub function.
*/
void MMPXCollectionUtility::OpenL(TMPXOpenMode aMode)
{
    Q_UNUSED(aMode);
    iOpen = ETrue;
}

/*!
 Stub function.
*/
void MMPXCollectionUtility::OpenL(TInt aIndex,TMPXOpenMode aMode)
{
    Q_UNUSED(aMode);
    iIndex = aIndex;  
    iOpen = ETrue;
}

/*!
 Stub function.
*/
void MMPXCollectionUtility::OpenL(const CMPXCollectionPath& aPath, TMPXOpenMode aMode)
{
    Q_UNUSED(aMode);
    iCountPath = aPath.Count();
    iOpen = ETrue;
    iOpenCount++;
}

/*!
 Stub function.
*/
void MMPXCollectionUtility::setPlaylists( const CMPXMedia& entries )
{
    delete iPlaylists;
    iPlaylists = 0;
    iPlaylists = CMPXMedia::NewL(entries);
}

//end of file
