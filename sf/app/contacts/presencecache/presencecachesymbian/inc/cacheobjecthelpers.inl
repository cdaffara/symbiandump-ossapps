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
* Description:  Templated algorithms to simplify object handling
*
*/


#include <e32base.h>


// -----------------------------------------------------------------------------
// TCacheObjectPacker::PackL() implementation.
// -----------------------------------------------------------------------------
//
template< class T >
HBufC8* TCacheObjectPacker<T>::PackL( const T& aObj )
	{
    // we don't know how large the returned descriptor is going to be
    CBufFlat* packBuf = CBufFlat::NewL( KObjectBufGranurality );
    CleanupStack::PushL( packBuf );

    RBufWriteStream ws;
    ws.Open( *packBuf ); // CSI: 65 #
    CleanupClosePushL( ws );

    aObj.ExternalizeL( ws );

    ws.CommitL();
    CleanupStack::PopAndDestroy(); //ws

    // now CBufFlat contains the descriptor
    HBufC8* packBufDesc = packBuf->Ptr(0).AllocL();

    CleanupStack::PopAndDestroy( packBuf );
    return packBufDesc;

	}

// -----------------------------------------------------------------------------
// TCacheObjectPacker::UnPackL() implementation.
// -----------------------------------------------------------------------------
//
template< class T >
void TCacheObjectPacker<T>::UnPackL( T& aObj, const TDesC8& aPack )
	{
    if ( ! aPack.Length() )
        {
        // empty, don't attempt to unpack.
        return;
        }

    RDesReadStream rs;
    CleanupClosePushL( rs );
    rs.Open( aPack ); // CSI: 65 #
    aObj.InternalizeL( rs );
    CleanupStack::PopAndDestroy(); // rs

	}

