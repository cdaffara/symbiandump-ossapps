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

#include "presencecachebuddyidcreatorprivate.h"
#include <CVPbkContactIdConverter.h>
#include <CVPbkContactStoreUriArray.h>
#include <VPbkContactStoreUris.h>
#include <MVPbkContactStore.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactLinkArray.h>
#include <cntdef.h>

_LIT( KServiceIdentifier, "ContactStatusService:" );

CPresenceCacheBuddyIdCreatorPrivate* CPresenceCacheBuddyIdCreatorPrivate::NewL( 
        CVPbkContactManager* aContactManager )
    {
    CPresenceCacheBuddyIdCreatorPrivate* self =
            new (ELeave) CPresenceCacheBuddyIdCreatorPrivate( aContactManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

inline CPresenceCacheBuddyIdCreatorPrivate::CPresenceCacheBuddyIdCreatorPrivate( 
        CVPbkContactManager* aContactManager )
: iContactManager( aContactManager )
    {
    }

inline void CPresenceCacheBuddyIdCreatorPrivate::ConstructL()
    {
    if( !iContactManager )
        {
        CVPbkContactStoreUriArray* uriList = CVPbkContactStoreUriArray::NewLC();
        uriList->AppendL( TVPbkContactStoreUriPtr( 
                VPbkContactStoreUris::DefaultCntDbUri() ) );

        iContactManager = CVPbkContactManager::NewL( *uriList );
        CleanupStack::PopAndDestroy( uriList );
        iOwnContactManager = ETrue;
        }
    MVPbkContactStore* store = iContactManager->ContactStoresL().
            Find( VPbkContactStoreUris::DefaultCntDbUri() );
    
    iConverter = CVPbkContactIdConverter::NewL( *store );
    }

CPresenceCacheBuddyIdCreatorPrivate::~CPresenceCacheBuddyIdCreatorPrivate()
    {
    delete iConverter;
    if( iOwnContactManager )
        {
        delete iContactManager;
        }
    }

HBufC* CPresenceCacheBuddyIdCreatorPrivate::CreateBuddyIdLC( 
        const TDesC8& aContactLink )
    {
    MVPbkContactLinkArray* linkList = 
            iContactManager->CreateLinksLC( aContactLink );
    TInt32 id = KNullContactId;
    if( linkList->Count() > 0 )
        {
        const MVPbkContactLink& link = linkList->At( 0 );
        id = iConverter->LinkToIdentifier( link );
        }
    CleanupStack::PopAndDestroy();  // linkList
    
    HBufC* buddyId = NULL;
    if( KNullContactId == id )
        {
        // incorrect aContactLink
        //  -> can't create identifier 
        User::Leave( KErrArgument );
        }
    else
        {
        // generate presence identity from contact id in format 
        // "[ServiceName]:[contact id]"
        //
        TBuf<11> num; // enough space to hold 32bit number
        num.Num( id );
        buddyId = HBufC::NewLC( num.Length() + KServiceIdentifier().Length() );
        TPtr retPtr( buddyId->Des() );
        retPtr.Copy( KServiceIdentifier );
        retPtr.Append( num );
        }
    
    return buddyId;
    }

