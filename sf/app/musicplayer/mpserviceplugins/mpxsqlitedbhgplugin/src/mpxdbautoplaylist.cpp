/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Responsible for the auto playlist functionality.
*
*/


// INCLUDE FILES
#include <mpxlog.h>
#include <mpxcollectiondbhgres.rsg>
#include <sqldb.h>
#include <hbtextresolversymbian.h>

#include "mpxresource.h"
#include "mpxdbcommonutil.h"
#include "mpxdbmanager.h"
#include "mpxdbutil.h"
#include "mpxcollectiondbdef.h"
#include "mpxdbautoplaylist.h"
#include "mpxdbpluginqueries.h"
#include "mpxdbcommondef.h"

const TInt KMPXTableDefaultIndex = 0;
const TInt32 KMPXRecentlyPlayedPlaylistId = 0x20000000;
const TInt32 KMPXMostPlayedPlaylistId = 0x20000001;
const TInt32 KMPXRecentlyAddedPlaylistId = 0x20000002;


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbAutoPlaylist* CMPXDbAutoPlaylist::NewL(
    CMPXDbManager& aDbManager,
    RFs& aFs,
    CMPXResource& aResource)
    {
    MPX_FUNC("CMPXDbAutoPlaylist::NewL");

    CMPXDbAutoPlaylist* self = CMPXDbAutoPlaylist::NewLC(aDbManager, aFs, aResource);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbAutoPlaylist* CMPXDbAutoPlaylist::NewLC(
    CMPXDbManager& aDbManager,
    RFs& aFs,
    CMPXResource& aResource)
    {
    MPX_FUNC("CMPXDbAutoPlaylist::NewLC");

    CMPXDbAutoPlaylist* self = new (ELeave) CMPXDbAutoPlaylist(aDbManager);
    CleanupStack::PushL(self);
    self->ConstructL(aFs, aResource);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXDbAutoPlaylist::~CMPXDbAutoPlaylist()
    {
    MPX_FUNC("CMPXDbAutoPlaylist::~CMPXDbAutoPlaylist");

    delete iRecentlyPlayedPlaylist;
    delete iMostPlayedPlaylist;
    delete iRecentlyAddedPlaylist;
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CMPXDbAutoPlaylist::CMPXDbAutoPlaylist(CMPXDbManager& aDbManager)
    : iDbManager(aDbManager)
    {
    MPX_FUNC("CMPXDbAutoPlaylist::CMPXDbAutoPlaylist");
    }

// ----------------------------------------------------------------------------
// Second phase constructor.
// ----------------------------------------------------------------------------
//
void CMPXDbAutoPlaylist::ConstructL(
    RFs& /*aFs*/,
    CMPXResource& /*aResource*/)
    {
    MPX_FUNC("CMPXDbAutoPlaylist::ConstructL");

    // Localization using QT
    TBool result = HbTextResolverSymbian::Init(KMPXMusicPlayerTsFile, KMPXMusicPlayerTsPath);
    if ( result )
        {
        iMostPlayedPlaylist = HbTextResolverSymbian::LoadL( _L("txt_mus_list_most_played") );
        iRecentlyAddedPlaylist = HbTextResolverSymbian::LoadL( _L("txt_mus_list_recently_added") );
        iRecentlyPlayedPlaylist = HbTextResolverSymbian::LoadL( _L("txt_mus_list_recently_played") );
        }
    else
        {
        // error initializing HbTextResolverSymbian, use logical string.
        MPX_DEBUG1("CMPXDbAutoPlaylist::ConstructL - HbTextResolverSymbian::Init() Failed.");
        TBufC<50> buf( _L("txt_mus_list_most_played") );
        iMostPlayedPlaylist = buf.AllocL();
        buf = _L("txt_mus_list_recently_added");
        iRecentlyAddedPlaylist = buf.AllocL();
        buf = _L("txt_mus_list_recently_played");
        iRecentlyPlayedPlaylist = buf.AllocL();
        }

    iRecentlyPlayedPlaylistId = KMPXRecentlyPlayedPlaylistId;
    iMostPlayedPlaylistId = KMPXMostPlayedPlaylistId;
    iRecentlyAddedPlaylistId = KMPXRecentlyAddedPlaylistId;
    }

// ----------------------------------------------------------------------------
// CMPXDbAutoPlaylist::AutoPlaylistIdL
// ----------------------------------------------------------------------------
//
TUint32 CMPXDbAutoPlaylist::AutoPlaylistIdL(
    TMPXAutoPlaylistType aPlaylistType)
    {
    MPX_FUNC("CMPXDbAutoPlaylist::AutoPlaylistIdL");

    TUint32 id(0);
    switch (aPlaylistType)
        {
        case EMPXRecentlyPlayedPlaylist:
            {
            id = iRecentlyPlayedPlaylistId;
            }
            break;

        case EMPXMostPlayedPlaylist:
            {
            id = iMostPlayedPlaylistId;
            }
            break;

        case EMPXRecentlyAddedPlaylist:
            {
            id = iRecentlyAddedPlaylistId;
            }
            break;

        default:
            User::Leave(KErrNotSupported);
        }

    return id;
    }

// ----------------------------------------------------------------------------
// CMPXDbAutoPlaylist::AutoPlaylistNameL
// ----------------------------------------------------------------------------
//
TPtrC CMPXDbAutoPlaylist::AutoPlaylistNameL(
    TUint32 aPlaylistId)
    {
    MPX_FUNC("CMPXDbAutoPlaylist::AutoPlaylistNameL");

    TPtrC ptr;
    if (aPlaylistId == iRecentlyPlayedPlaylistId)
        {
        ptr.Set(*iRecentlyPlayedPlaylist);
        }
    else if (aPlaylistId == iMostPlayedPlaylistId)
        {
        ptr.Set(*iMostPlayedPlaylist);
        }
    else if (aPlaylistId == iRecentlyAddedPlaylistId)
        {
        ptr.Set(*iRecentlyAddedPlaylist);
        }
    else
        {
        User::Leave(KErrNotSupported);
        }

    return ptr;
    }

// ----------------------------------------------------------------------------
// CMPXDbAutoPlaylist::AutoPlaylistTypeL
// ----------------------------------------------------------------------------
//
TMPXAutoPlaylistType CMPXDbAutoPlaylist::AutoPlaylistTypeL(
    TUint32 aPlaylistId)
    {
    MPX_FUNC("CMPXDbAutoPlaylist::AutoPlaylistTypeL");

    TMPXAutoPlaylistType type(EMPXNoAutoPlaylist);
    if (aPlaylistId == iRecentlyPlayedPlaylistId)
        {
        type = EMPXRecentlyPlayedPlaylist;
        }
    else if (aPlaylistId == iMostPlayedPlaylistId)
        {
        type = EMPXMostPlayedPlaylist;
        }
    else if (aPlaylistId == iRecentlyAddedPlaylistId)
        {
        type = EMPXRecentlyAddedPlaylist;
        }
    else
        {
        // do nothing
        }

    return type;
    }

// ----------------------------------------------------------------------------
// CMPXDbAutoPlaylist::GetPlaylistL
// ----------------------------------------------------------------------------
//
void CMPXDbAutoPlaylist::GetPlaylistL(
    TUint32 aPlaylistId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbAutoPlaylist::GetPlaylistL");

    if (EMPXNoAutoPlaylist == AutoPlaylistTypeL(aPlaylistId))
        {
        User::Leave(KErrNotSupported);
        }

    UpdateMediaL(aPlaylistId, aAttrs, aMedia);
    }

// ----------------------------------------------------------------------------
// CMPXDbAutoPlaylist::GetAllPlaylistsL
// ----------------------------------------------------------------------------
//
void CMPXDbAutoPlaylist::GetAllPlaylistsL(
    CMPXMediaArray& aMediaArray)
    {
    MPX_FUNC("CMPXDbAutoPlaylist::GetAllPlaylistsL");

    MPX_ASSERT(aMediaArray.Count());
    MPX_ASSERT(aMediaArray[0]->IsSupported(KMPXMediaGeneralValue));
    TInt pPath = aMediaArray[0]->ValueTObjectL<TInt>(KMPXMediaGeneralValue);
    MPX_ASSERT(pPath);
    MPXDbCommonUtil::AppendMediaL(aMediaArray, *iMostPlayedPlaylist, EMPXItem,
        EMPXPlaylist, iMostPlayedPlaylistId, EMPXWrite);
    TInt count = aMediaArray.Count();
    TMPXItemId id = aMediaArray[count-1]->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
    if (count==2)
        { // the first playlist
        if (((CMPXCollectionPath*)pPath)->Id()==KMPXInvalidItemId)
            { // Remove top level with invalid id
            ((CMPXCollectionPath*)pPath)->Back();
            }
        ((CMPXCollectionPath*)pPath)->AppendL(id);
        }
    else
        { // append the end
        ((CMPXCollectionPath*)pPath)->InsertL(id,
                                              ((CMPXCollectionPath*)pPath)->Count());
        }
    TInt songCount = 0;
    TInt duration = 0;
    GetPlaylistDetailsL(EMPXMostPlayedPlaylist, songCount, duration);
    aMediaArray[count-1]->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, songCount);
    aMediaArray[count-1]->SetTObjectValueL<TInt>(KMPXMediaGeneralDuration, duration);

    MPXDbCommonUtil::AppendMediaL(aMediaArray, *iRecentlyPlayedPlaylist, EMPXItem,
        EMPXPlaylist, iRecentlyPlayedPlaylistId, EMPXWrite);
    id = aMediaArray[count]->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
    ((CMPXCollectionPath*)pPath)->InsertL(id, ((CMPXCollectionPath*)pPath)->Count());
    songCount = 0;
    duration = 0;
    GetPlaylistDetailsL(EMPXRecentlyPlayedPlaylist, songCount, duration);
	aMediaArray[count]->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, songCount);
   	aMediaArray[count]->SetTObjectValueL<TInt>(KMPXMediaGeneralDuration, duration);

    MPXDbCommonUtil::AppendMediaL(aMediaArray, *iRecentlyAddedPlaylist, EMPXItem,
        EMPXPlaylist, iRecentlyAddedPlaylistId, EMPXWrite);
    id = aMediaArray[count+1]->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
    ((CMPXCollectionPath*)pPath)->InsertL(id, ((CMPXCollectionPath*)pPath)->Count());
    songCount = 0;
	duration = 0;
	GetPlaylistDetailsL(EMPXRecentlyAddedPlaylist, songCount, duration);
	aMediaArray[count+1]->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, songCount);
	aMediaArray[count+1]->SetTObjectValueL<TInt>(KMPXMediaGeneralDuration, duration);
    }

// ----------------------------------------------------------------------------
// CMPXDbAutoPlaylist::GetPlaylistDetails
// ----------------------------------------------------------------------------
//
void CMPXDbAutoPlaylist::GetPlaylistDetailsL(TMPXAutoPlaylistType aType, TInt& aCount, TInt& aDuration)
    {
    MPX_FUNC("CMPXDbAutoPlaylist::GetPlaylistDetails");

    HBufC* query = NULL;

    switch ( aType )
    	{
		case EMPXMostPlayedPlaylist:
		    query = HBufC::NewLC(KQueryMusicGetDurationMostPlayed().Length() + KMCIntegerLen);
		    query->Des().Format(KQueryMusicGetDurationMostPlayed(), KMPXMaxMostPlayedSongs);
		    break;
		case EMPXRecentlyPlayedPlaylist:
		    query = HBufC::NewLC(KQueryMusicGetDurationRecentlyPlayed().Length() + KMCIntegerLen);
		    query->Des().Format(KQueryMusicGetDurationRecentlyPlayed(), KMPXMaxRecentlyPlayedSongs);
			break;
		case EMPXRecentlyAddedPlaylist:
		    query = HBufC::NewLC(KQueryMusicGetDurationRecentlyAdded().Length());
		    query->Des().Append(KQueryMusicGetDurationRecentlyAdded());
			break;
		default:
			User::Leave(KErrArgument);
			break;
		}

    RSqlStatement recordset(iDbManager.ExecuteSelectQueryL(*query));
    CleanupClosePushL(recordset);

    TInt sum(0);
    TInt err(KErrNone);
    while ((err = recordset.Next()) == KSqlAtRow)
        {
        aCount++;
        sum += recordset.ColumnInt64(KMPXTableDefaultIndex);
        }

    CleanupStack::PopAndDestroy(&recordset);
    CleanupStack::PopAndDestroy(query);

    if (err != KSqlAtEnd)
        {
        User::Leave(KErrCorrupt);
        }

    aDuration = sum;
    MPX_DEBUG3("    Count[%d], Duration[%d]", aCount, aDuration);
    }

// ----------------------------------------------------------------------------
// CMPXDbAutoPlaylist::UpdateMediaL
// ----------------------------------------------------------------------------
//
void CMPXDbAutoPlaylist::UpdateMediaL(
    TUint32 aPlaylistId,
    const TArray<TMPXAttribute>& aAttrs,
    CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXDbAutoPlaylist::UpdateMediaL");

    TInt count(aAttrs.Count());
    for (TInt i = 0; i < count; ++i)
        {
        TInt contentId(aAttrs[i].ContentId());

        if (contentId == KMPXMediaIdGeneral)
            {
            TUint attributeId(aAttrs[i].AttributeId());

            if (attributeId & EMPXMediaGeneralId)
                {
                aMedia.SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, aPlaylistId);
                }
            if (attributeId & EMPXMediaGeneralCollectionId )
                {
                aMedia.SetTObjectValueL<TUid>(KMPXMediaGeneralCollectionId,
                    TUid::Uid(KDBPluginUid));
                }
            if (attributeId & EMPXMediaGeneralTitle)
                {
                if (aPlaylistId == iRecentlyPlayedPlaylistId)
                    {
                    aMedia.SetTextValueL(KMPXMediaGeneralTitle, *iRecentlyPlayedPlaylist);
                    }
                 else if (aPlaylistId == iMostPlayedPlaylistId)
                    {
                    aMedia.SetTextValueL(KMPXMediaGeneralTitle,*iMostPlayedPlaylist);
                    }
                 else if (aPlaylistId == iRecentlyAddedPlaylistId)
                    {
                    aMedia.SetTextValueL(KMPXMediaGeneralTitle, *iRecentlyAddedPlaylist);
                    }
                 else
                    {
                    User::Leave(KErrArgument);
                    }
                }
            if (attributeId & EMPXMediaGeneralSynchronized)
                {
                aMedia.SetTObjectValueL<TBool>(KMPXMediaGeneralSynchronized, EFalse);
                }
            if (attributeId & EMPXMediaGeneralNonPermissibleActions)
                {
                aMedia.SetTObjectValueL<TMPXGeneralNonPermissibleActions>(
                    KMPXMediaGeneralNonPermissibleActions, EMPXWrite);
                }
            // EMPXMediaGeneralCount, EMPXMediaGeneralDate, EMPXMediaGeneralDuration,
            // EMPXMediaGeneralUri, EMPXMediaGeneralDrive, and EMPXMediaGeneralFlags
            // attributes are not applicable for auto-playlists
            } // end if contentId == KMPXMediaIdGeneral
        } // end for

    aMedia.SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    aMedia.SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXPlaylist);
    }

// End of File
