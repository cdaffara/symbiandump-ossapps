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
* Description:  Inline finction definitions for class TSearchParams.
*
*/


#include <StringLoader.h>

const TInt KIpsPlgArrayGranularity = 3;
// <cmail> mirrored
//_LIT( KIpsPlgPattern, "*%0U*" );
_LIT( KIpsPlgPattern, "*" );
// </cmail> 


// ---------------------------------------------------------------------------
// CIpsPlgSearch::TSearchParams::TSearchParams()
// ---------------------------------------------------------------------------
//
CIpsPlgSearch::TSearchParams::TSearchParams(
    const RPointerArray<TDesC>& aSearchStrings,
    const TFSMailMsgId& aMailbox,
    const RArray<TFSMailMsgId> aFolderIds,
    const TFSMailSortCriteria& aSortCriteria )
    :
    iSearchStrings( NULL ),
    iMailbox( aMailbox ),
    iSortCriteria( aSortCriteria )
    {
    TRAP_IGNORE( SetSearchStringsL( aSearchStrings ) );
    TRAP_IGNORE( SetFolderIdsL( aFolderIds ) );
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::TSearchParams::~TSearchParams()
// ---------------------------------------------------------------------------
//
CIpsPlgSearch::TSearchParams::~TSearchParams()
    {
    iFolderIds.Reset();
    RemoveArray();
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::TSearchParams::operator=()
// ---------------------------------------------------------------------------
//
CIpsPlgSearch::TSearchParams& CIpsPlgSearch::TSearchParams::operator=(
    const TSearchParams& aParams )
    {
    // Make sure the lists are empty.
    iFolderIds.Reset();
    RemoveArray();

    TRAP_IGNORE( SetSearchStringsL( *aParams.iSearchStrings ) );
    iMailbox = aParams.iMailbox;
    TRAP_IGNORE( SetFolderIdsL( aParams.iFolderIds ) );
    iSortCriteria = aParams.iSortCriteria;

    return *this;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::TSearchParams::operator=()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::TSearchParams::operator+=(
    const TSearchParams& aParams )
    {
    // Update folder list and sort criteria.
    TRAP_IGNORE( AddFolderIdsL( aParams.iFolderIds ) );
    iSortCriteria = aParams.iSortCriteria;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::TSearchParams::RemoveArray()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::TSearchParams::RemoveArray()
    {
    if ( iSearchStrings )
        {
        iSearchStrings->Reset();
        }

    delete iSearchStrings;
    iSearchStrings = NULL;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::TSearchParams::SetSearchStringsL()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::TSearchParams::SetSearchStringsL(
    const RPointerArray<TDesC>& aSearchStrings )
    {
    RemoveArray();

    iSearchStrings =
        new( ELeave ) CDesC16ArrayFlat( KIpsPlgArrayGranularity );

    for ( TInt item = 0; item < aSearchStrings.Count(); item++ )
        {
        // <cmail> mirrored

        // Add the wildcards here.
        //TBuf<5> pattern( KIpsPlgPattern );
        RBuf destroyThisHackStringNow;
        destroyThisHackStringNow.CreateL(
            aSearchStrings[item]->Length() + 2 );
        CleanupClosePushL( destroyThisHackStringNow );

        destroyThisHackStringNow.Insert(0, KIpsPlgPattern);
        destroyThisHackStringNow.Append(*aSearchStrings[item]);
        destroyThisHackStringNow.Append(KIpsPlgPattern);
        /*
        StringLoader::Format(
            destroyThisHackStringNow, pattern, 0, *aSearchStrings[item] );
        */

        // </cmail> 

        
        iSearchStrings->AppendL( destroyThisHackStringNow ); //<cmail>

        CleanupStack::PopAndDestroy( &destroyThisHackStringNow );
        }
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::TSearchParams::SetSearchStringsL()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::TSearchParams::SetSearchStringsL(
    const CDesC16Array& aSearchStrings )
    {
    RemoveArray();

    iSearchStrings =
        new( ELeave ) CDesC16ArrayFlat( KIpsPlgArrayGranularity );

    for ( TInt item = 0; item < aSearchStrings.Count(); item++ )
        {
        iSearchStrings->AppendL( aSearchStrings[item] ); //<cmail>
        }
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::TSearchParams::SetFolderIdsL()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::TSearchParams::SetFolderIdsL(
    const RArray<TFSMailMsgId>& aFolderIds )
    {
    for ( TInt item = 0; item < aFolderIds.Count(); item++ )
        {
        iFolderIds.AppendL( aFolderIds[item] );
        }
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::TSearchParams::FolderExists()
// ---------------------------------------------------------------------------
//
TBool CIpsPlgSearch::TSearchParams::FolderExists( const TFSMailMsgId& aFolderId )
    {
    for ( TInt existing = 0; existing < iFolderIds.Count(); existing++ )
        {
        if ( aFolderId == iFolderIds[existing] )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::TSearchParams::AddFolderIdsL()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::TSearchParams::AddFolderIdsL(
    const RArray<TFSMailMsgId>& aFolderIds )
    {
    for ( TInt item = 0; item < aFolderIds.Count(); item++ )
        {
        if ( !FolderExists( aFolderIds[item] ) )
            {
            iFolderIds.AppendL( aFolderIds[item] );
            }
        }
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::TMsgContainer::SetFolderIdsL()
// ---------------------------------------------------------------------------
//
TMsvId CIpsPlgSearch::TMsgContainer::Id() const
    {
    return iEntry.Id();
    }

// End of File
