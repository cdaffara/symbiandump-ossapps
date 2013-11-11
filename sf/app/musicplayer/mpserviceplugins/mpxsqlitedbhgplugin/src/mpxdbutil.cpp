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
* Description:  Contains utility functions for the DB plugin
*
*/


// INCLUDE FILES
#include <mpxlog.h>

#include "mpxcollectiondbdef.h"
#include "mpxdbutil.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Filter out duplicated Ids
// ----------------------------------------------------------------------------
//
void MPXDbUtil::FilterDuplicatedIdsL(
    const TArray<TUint32>& aIds,
    RArray<TUint32>& aFilteredIds)
    {
    MPX_FUNC("MPXDbUtil::FilterDuplicatedIdsL");

    TInt count(aIds.Count());
    for (TInt i = 0; i < count; ++i)
        {
        MPX_TRAPD(error, aFilteredIds.InsertInUnsignedKeyOrderL(aIds[i]));
        if ((error != KErrNone) && (error != KErrAlreadyExists))
            {
            User::Leave(error);
            }
        }
    }

// ----------------------------------------------------------------------------
// MPXDbUtil::TableNameForCategoryL
// ----------------------------------------------------------------------------
//
TPtrC MPXDbUtil::TableNameForCategoryL(
    TMPXGeneralCategory aCategory)
    {
    MPX_FUNC("MPXDbUtil::TableNameForCategoryL");

    TPtrC ptr;
    switch (aCategory)
        {
        case EMPXPlaylist:
            ptr.Set(KMCPlaylistTable);
            break;
        case EMPXArtist:
            ptr.Set(KMCArtistTable);
            break;
        case EMPXAlbum:
            ptr.Set(KMCAlbumTable);
            break;
        case EMPXGenre:
            ptr.Set(KMCGenreTable);
            break;
        case EMPXComposer:
            ptr.Set(KMCComposerTable);
            break;
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        case EMPXAbstractAlbum:
            ptr.Set(KMCAbstractAlbumTable);
            break;
#endif // ABSTRACTAUDIOALBUM_INCLUDED
        default:
            User::Leave(KErrNotSupported);
        }

    return ptr;
    }

// ----------------------------------------------------------------------------
// MPXDbUtil::MusicFieldNameForCategoryL
// ----------------------------------------------------------------------------
//
TPtrC MPXDbUtil::MusicFieldNameForCategoryL(
    TMPXGeneralCategory aCategory)
    {
    MPX_FUNC("MPXDbUtil::MusicFieldNameForCategoryL");

    TPtrC ptr;
    switch (aCategory)
        {
        case EMPXArtist:
            ptr.Set(KMCMusicArtist);
            break;
        case EMPXAlbum:
            ptr.Set(KMCMusicAlbum);
            break;
        case EMPXGenre:
            ptr.Set(KMCMusicGenre);
            break;
        case EMPXComposer:
            ptr.Set(KMCMusicComposer);
            break;
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        case EMPXAbstractAlbum:
            ptr.Set(KMCMusicAbstractAlbum);
            break;
#endif // ABSTRACTAUDIOALBUM_INCLUDED
        default:
            User::Leave(KErrNotSupported);
        }

    return ptr;
    }

// End of File
