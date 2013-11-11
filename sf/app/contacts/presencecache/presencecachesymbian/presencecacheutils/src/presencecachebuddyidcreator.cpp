/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include "presencecachebuddyidcreator.h"
#include "presencecachebuddyidcreatorprivate.h"


EXPORT_C CPresenceCacheBuddyIdCreator* CPresenceCacheBuddyIdCreator::NewL(
        CVPbkContactManager* aContactManager )
    {
    CPresenceCacheBuddyIdCreator* self = 
            new (ELeave) CPresenceCacheBuddyIdCreator();
    CleanupStack::PushL( self );
    self->ConstructL( aContactManager );
    CleanupStack::Pop( self );
    return self;
    }

inline CPresenceCacheBuddyIdCreator::CPresenceCacheBuddyIdCreator( )
    {
    }

inline void CPresenceCacheBuddyIdCreator::ConstructL( 
        CVPbkContactManager* aContactManager )
    {
    iImpl = CPresenceCacheBuddyIdCreatorPrivate::NewL( aContactManager );
    }

CPresenceCacheBuddyIdCreator::~CPresenceCacheBuddyIdCreator()
    {
    delete iImpl;
    }

EXPORT_C HBufC* CPresenceCacheBuddyIdCreator::CreateBuddyIdLC( 
        const TDesC8& aContactLink )
    {
    return iImpl->CreateBuddyIdLC( aContactLink );
    }
