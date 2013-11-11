/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Helpers for handling objects
*
*/


#include <mpresencebuddyinfo2.h>
#include "cacheobjecthelpers.h"
#include "presenceobjecthelpers.h"
#include "presencecachebuddyinfo.h"

// ==================== TPresenceArrayPacker MEMBER FUNCTIONS ====================

// -----------------------------------------------------------------------------
// TPresenceArrayPacker::PackArrayL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* TPresenceArrayPacker::PackArrayL( const RPointerArray< MPresenceBuddyInfo2 >& aBufferArray )
    {
    CBufFlat* packBuf = CBufFlat::NewL( KObjectBufGranurality );
    CleanupStack::PushL( packBuf );

    RBufWriteStream ws;
    ws.Open( *packBuf ); // CSI: 65 #
    CleanupClosePushL( ws );

    // Get count of objects
    TInt objCount( aBufferArray.Count() );
    // write the count
    ws.WriteInt32L( objCount );
    // objects
    MPresenceBuddyInfo2* info = NULL;    
    for ( TInt count(0); count < objCount; count++ )
        {
        info = aBufferArray[ count ];
        info->ExternalizeL( ws );
        }

    ws.CommitL();
    CleanupStack::PopAndDestroy(); //ws

    HBufC8* packBufDesc = packBuf->Ptr(0).AllocL();
    CleanupStack::PopAndDestroy( packBuf );

    return packBufDesc;
    }


// -----------------------------------------------------------------------------
// TPresenceArrayPacker::UnPackArrayL()
// -----------------------------------------------------------------------------
//
EXPORT_C void TPresenceArrayPacker::UnPackArrayL( RPointerArray< MPresenceBuddyInfo2 >& aBufferArray, const TDesC8& aPack  )
    {
    CleanupResetAndDestroyPushL( aBufferArray );
	// No reset
    if ( ! aPack.Length() )
        {
        // empty, don't attempt to unpack
        return;
        }

    RDesReadStream rs;
    CleanupClosePushL( rs );
    rs.Open( aPack ); // CSI: 65 #

    // read the object count
    TInt objCount( rs.ReadInt32L() );
    
    MPresenceBuddyInfo2* info = NULL;      

	for ( TInt count = 0; count < objCount; count++ )
        {
        info = MPresenceBuddyInfo2::NewLC();
        info->InternalizeL( rs );
        aBufferArray.AppendL( info );
        CleanupStack::Pop();
		}
    CleanupStack::PopAndDestroy(); // rs    
    CleanupStack::Pop( &aBufferArray );
    }


// -----------------------------------------------------------------------------
// PresenceCacheBuddyUtils::ResetBuddyInfoTimeStamp()
// -----------------------------------------------------------------------------
//
EXPORT_C void PresenceCacheBuddyUtils::ResetBuddyInfoTimeStamp(
        MPresenceBuddyInfo2& aBuddy)
    {
    static_cast<CPresenceCacheBuddyInfo*>(&aBuddy)->ResetTimeStamp();
    }

// -----------------------------------------------------------------------------
// PresenceCacheBuddyUtils::IsBuddyInfoExpired()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool PresenceCacheBuddyUtils::IsBuddyInfoExpired(
        const MPresenceBuddyInfo2& aBuddy)
    {
    return static_cast<const CPresenceCacheBuddyInfo*>(&aBuddy)->IsExpired();
    }


// End of file
