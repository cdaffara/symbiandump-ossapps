/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <FavouritesWapAp.h> 

#include "engine.h"
#include "enginewrapper.h"
#include "creator_browser.h" 
#include "creator_traces.h"


static const TInt KBrowserFieldLength = 128;

_LIT(KCreatorBrowserPrefixName, "CR_");
_LIT(KCreatorBrowserPrefixFolderName, "CR_FLDR_");

//----------------------------------------------------------------------------

CBrowserParameters::CBrowserParameters()
    {
    LOGSTRING("Creator: CBrowserParameters::CBrowserParameters");

    iBookmarkName = HBufC::New(KBrowserFieldLength);
    iBookmarkAddress = HBufC::New(KBrowserFieldLength);
    iBookmarkUsername = HBufC::New(KBrowserFieldLength);
    iBookmarkPassword = HBufC::New(KBrowserFieldLength);    
    iBookmarkFolderName = HBufC::New(KBrowserFieldLength);
    iSavedDeckLinkName = HBufC::New(KBrowserFieldLength);
    iSavedDeckLocalAddress = HBufC::New(KBrowserFieldLength);
    iSavedDeckFolderName = HBufC::New(KBrowserFieldLength);
    }

CBrowserParameters::~CBrowserParameters()
    {
    LOGSTRING("Creator: CBrowserParameters::~CBrowserParameters");

    delete iSavedDeckFolderName;
    delete iSavedDeckLocalAddress;
    delete iSavedDeckLinkName;
    delete iBookmarkFolderName;
    delete iBookmarkPassword;    
    delete iBookmarkUsername;
    delete iBookmarkAddress;
    delete iBookmarkName;

    }

//----------------------------------------------------------------------------

CCreatorBrowser* CCreatorBrowser::NewL(CCreatorEngine* aEngine)
    {
    CCreatorBrowser* self = CCreatorBrowser::NewLC(aEngine);
    CleanupStack::Pop(self);
    return self;
    }

CCreatorBrowser* CCreatorBrowser::NewLC(CCreatorEngine* aEngine)
    {
    CCreatorBrowser* self = new (ELeave) CCreatorBrowser;
    CleanupStack::PushL(self);
    self->ConstructL(aEngine);
    return self;
    }

CCreatorBrowser::CCreatorBrowser()
    {
    }

void CCreatorBrowser::ConstructL(CCreatorEngine* aEngine)
    {
    LOGSTRING("Creator: CCreatorBrowser::ConstructL");

    iEngine = aEngine;
    }

CCreatorBrowser::~CCreatorBrowser()
    {
    LOGSTRING("Creator: CCreatorBrowser::~CCreatorBrowser");
    if ( iEngine && iBmEntryIds.Count() )
        {
        TRAP_IGNORE( iEngine->WriteEntryIdsToStoreL( iBmEntryIds, KUidDictionaryUidBrowserBookmarks ) );
        }
    iBmEntryIds.Reset();
    iBmEntryIds.Close();

    if ( iEngine && iBmFEntryIds.Count() )
        {
        TRAP_IGNORE( iEngine->WriteEntryIdsToStoreL( iBmFEntryIds, KUidDictionaryUidBrowserBookmarkF ) );
        }
    iBmFEntryIds.Reset();
    iBmFEntryIds.Close();

    if ( iEngine && iSpEntryIds.Count() )
        {
        TRAP_IGNORE( iEngine->WriteEntryIdsToStoreL( iSpEntryIds, KUidDictionaryUidBrowserSavedPg ) );
        }
    iSpEntryIds.Reset();
    iSpEntryIds.Close();

    if ( iEngine && iSpFEntryIds.Count() )
        {
        TRAP_IGNORE( iEngine->WriteEntryIdsToStoreL( iSpFEntryIds, KUidDictionaryUidBrowserSavedPgF ) );
        }
    iSpFEntryIds.Reset();
    iSpFEntryIds.Close();

    delete iParameters;
    }

//----------------------------------------------------------------------------

TBool CCreatorBrowser::AskDataFromUserL(TInt aCommand)
    {
    LOGSTRING("Creator: CCreatorBrowser::AskDataFromUserL");

    CCreatorModuleBase::AskDataFromUserL( aCommand );//ignore retval
    
    TBool result( EFalse );
    switch ( aCommand )
        {
        case ECmdDeleteBrowserBookmarks:
            {
            result = iEngine->GetEngineWrapper()->YesNoQueryDialog( _L("Delete all Browser bookmarks?"), this, ECreatorModuleDelete );
            break;
            }
        case ECmdDeleteCreatorBrowserBookmarks:
            {
            result = iEngine->GetEngineWrapper()->YesNoQueryDialog( _L("Delete all Browser bookmarks created with Creator?"), this, ECreatorModuleDelete );
            break;
            }
        case ECmdDeleteBrowserBookmarkFolders:
            {
            result = iEngine->GetEngineWrapper()->YesNoQueryDialog( _L("Delete all Browser bookmark folders?"), this, ECreatorModuleDelete );
            break;
            }
        case ECmdDeleteCreatorBrowserBookmarkFolders:
            {
            result = iEngine->GetEngineWrapper()->YesNoQueryDialog( _L("Delete all Browser bookmark folders created with Creator?"), this, ECreatorModuleDelete );
            break;
            }
        case ECmdDeleteBrowserSavedPages:
            {
            result = iEngine->GetEngineWrapper()->YesNoQueryDialog( _L("Delete all Browser saved pages?"), this, ECreatorModuleDelete );
            break;
            }
        case ECmdDeleteCreatorBrowserSavedPages:
            {
            result = iEngine->GetEngineWrapper()->YesNoQueryDialog( _L("Delete all Browser saved pages created with Creator?"), this, ECreatorModuleDelete );
            break;
            }
        case ECmdDeleteBrowserSavedPageFolders:
            {
            result = iEngine->GetEngineWrapper()->YesNoQueryDialog( _L("Delete all Browser saved pages folders?"), this, ECreatorModuleDelete );
            break;
            }
        case ECmdDeleteCreatorBrowserSavedPageFolders:
            {
            result = iEngine->GetEngineWrapper()->YesNoQueryDialog( _L("Delete all Browser saved pages folders created with Creator?"), this, ECreatorModuleDelete );
            break;
            }
        default:
            {
            result = iEngine->GetEngineWrapper()->EntriesQueryDialog(&iEntriesToBeCreated, _L("How many entries to create?"), EFalse, this, ECreatorModuleStart );
            break;
            }
        }
    return result;
    }


//----------------------------------------------------------------------------

TInt CCreatorBrowser::CreateBookmarkEntryL(CBrowserParameters *aParameters)
    {
    LOGSTRING("Creator: CCreatorBrowser::CreateBookmarkEntryL");

    // clear any existing parameter definations
    delete iParameters;
    iParameters = NULL;
    
    CBrowserParameters* parameters = aParameters;
        
    // random data needed if no predefined data available
    if( !parameters )
        {
        iParameters = new(ELeave) CBrowserParameters;
        parameters = iParameters; 

        TFileName userAndPassword = iEngine->RandomString(CCreatorEngine::EFirstName);
        TFileName name = iEngine->RandomString(CCreatorEngine::ECompany);
        TFileName url = name;
        url.Insert(0, _L("http://www."));
        url.Append(_L(".com/"));
        name.Insert(0, KCreatorBrowserPrefixName);

        parameters->iBookmarkName->Des() = name;
        parameters->iBookmarkAddress->Des() = url;
        parameters->iBookmarkUsername->Des() = userAndPassword;
        parameters->iBookmarkPassword->Des() = userAndPassword;
        }
   
    TInt err = KErrNone;

    RFavouritesSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL<RFavouritesSession>(session);
    RFavouritesDb db;
    User::LeaveIfError( db.Open(session, KBrowserBookmarks) );
    CleanupClosePushL<RFavouritesDb>(db);

    CFavouritesItem* item = CFavouritesItem::NewLC();
    item->SetType( CFavouritesItem::EItem );
    item->SetParentFolder( KFavouritesRootUid ) ;
    item->SetNameL( parameters->iBookmarkName->Des() );
    item->SetUrlL( parameters->iBookmarkAddress->Des() );
    item->SetUserNameL( parameters->iBookmarkUsername->Des() );
    item->SetPasswordL( parameters->iBookmarkPassword->Des() );
    err = db.Add(*item, /*aAutoRename=*/ETrue);
    if (err)
        db.Commit();
    
    // id has been generated, store it for being able to delete
    // only entries created with Creator
    iBmEntryIds.AppendL( item->Uid() );
    
    CleanupStack::PopAndDestroy(3); //db, session, item

    return err;
    }

//----------------------------------------------------------------------------

TInt CCreatorBrowser::CreateBookmarkFolderEntryL(CBrowserParameters *aParameters)
    {
    LOGSTRING("Creator: CCreatorBrowser::CreateBookmarkFolderEntryL");

    // clear any existing parameter definations
    delete iParameters;
    iParameters = NULL;
    
    CBrowserParameters* parameters = aParameters;
    
    // random data needed if no predefined data available
    if (!parameters)
        {
        iParameters = new(ELeave) CBrowserParameters;
        parameters = iParameters;

        TFileName name = iEngine->RandomString(CCreatorEngine::ECompany);
        name.Insert(0, KCreatorBrowserPrefixFolderName);

        parameters->iBookmarkFolderName->Des() = name;
        }
    
    TInt err = KErrNone;

    RFavouritesSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL<RFavouritesSession>(session);
    RFavouritesDb db;
    User::LeaveIfError( db.Open(session, KBrowserBookmarks) );
    CleanupClosePushL<RFavouritesDb>(db);

    CFavouritesItem* item = CFavouritesItem::NewLC();
    item->SetType( CFavouritesItem::EFolder );
    item->SetParentFolder( KFavouritesRootUid ) ;
    item->SetNameL( parameters->iBookmarkFolderName->Des() );
    
    err = db.Add(*item, /*aAutoRename=*/ETrue);
    if (err)
        db.Commit();
    
    // id has been generated, store it for being able to delete
    // only entries created with Creator
    iBmFEntryIds.AppendL( item->Uid() );
    
    CleanupStack::PopAndDestroy(3); //db, session, item
            

    return err;
    
    }

//----------------------------------------------------------------------------

TInt CCreatorBrowser::CreateSavedDeckEntryL(CBrowserParameters *aParameters)
    {
    LOGSTRING("Creator: CCreatorBrowser::CreateSavedDeckEntryL");

    // clear any existing parameter definations
    delete iParameters;
    iParameters = NULL;
    
    CBrowserParameters* parameters = aParameters;
    
    // random data needed if no predefined data available
    if (!parameters)
        {
        iParameters = new(ELeave) CBrowserParameters;
        parameters = iParameters;

        TFileName name = iEngine->RandomString(CCreatorEngine::ECompany);
        name.Insert(0, KCreatorBrowserPrefixName);

        parameters->iSavedDeckLinkName->Des() = name;
        parameters->iSavedDeckLocalAddress->Des() = iEngine->TestDataPathL(CCreatorEngine::ESavedDeck_1kB);
        }
   
    TInt err = KErrNone;

    // check the deck address is correct
    if ( parameters->iSavedDeckLocalAddress->Des().Length() == 0 ||
    		!BaflUtils::FileExists( CEikonEnv::Static()->FsSession(), parameters->iSavedDeckLocalAddress->Des() ))
        {
        // not found, use a default deck instead
        delete parameters->iSavedDeckLocalAddress;
        parameters->iSavedDeckLocalAddress = 0;
        TPtrC tmp = iEngine->TestDataPathL(CCreatorEngine::ESavedDeck_1kB);
        parameters->iSavedDeckLocalAddress = HBufC::NewL(tmp.Length());
        parameters->iSavedDeckLocalAddress->Des() = tmp;
        }


    RFavouritesSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL<RFavouritesSession>(session);
    RFavouritesDb db;
    User::LeaveIfError( db.Open(session, KBrowserSavedPages) );
    CleanupClosePushL<RFavouritesDb>(db);

    CFavouritesItem* item = CFavouritesItem::NewLC();
    item->SetType( CFavouritesItem::EItem );
    item->SetParentFolder( KFavouritesRootUid ) ;
    item->SetNameL( parameters->iSavedDeckLinkName->Des() );
    item->SetUrlL( _L(" ") );

    TFavouritesWapAp favouritesAp;
    favouritesAp.SetDefault();
    item->SetWapAp( favouritesAp );

    
    err = db.Add(*item, /*aAutoRename=*/ETrue);
    if (err)
        db.Commit();
    
    // read the source deck to buffer
    RFile sourceFile;
    User::LeaveIfError( sourceFile.Open(CEikonEnv::Static()->FsSession(), parameters->iSavedDeckLocalAddress->Des(), EFileRead) );
    CleanupClosePushL<RFile>(sourceFile);
    TInt sourceFileSize(0);
    err = sourceFile.Size(sourceFileSize);
    HBufC8* sourceBuf = HBufC8::NewLC(sourceFileSize);
    TPtr8 sourcePtr = sourceBuf->Des();
    sourceFile.Read(sourcePtr);
    
    // write the deck
    RFavouritesFile destinationFile;
    User::LeaveIfError( destinationFile.Replace(db, item->Uid() ));
    CleanupClosePushL<RFavouritesFile>(destinationFile);
    err = destinationFile.Write(sourceBuf->Des());

    // id has been generated, store it for being able to delete
    // only entries created with Creator
    iSpEntryIds.AppendL( item->Uid() );

 
    CleanupStack::PopAndDestroy(6); //db, session, item, sourceFile, destinationFile, sourceBuf    
    
    return err;
    }

//----------------------------------------------------------------------------

TInt CCreatorBrowser::CreateSavedDeckFolderEntryL(CBrowserParameters *aParameters)
    {
    LOGSTRING("Creator: CCreatorBrowser::CreateSavedDeckFolderEntryL");

    // clear any existing parameter definations
    delete iParameters;
    iParameters = NULL;
            
    CBrowserParameters* parameters = aParameters;
    // random data needed if no predefined data available
    if (!parameters)
        {
        iParameters = new(ELeave) CBrowserParameters;
        parameters = iParameters;
        
        TFileName name = iEngine->RandomString(CCreatorEngine::ECompany);
        name.Insert(0, KCreatorBrowserPrefixFolderName);

        parameters->iSavedDeckFolderName->Des() = name;
        }
   
    TInt err = KErrNone;

    RFavouritesSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL<RFavouritesSession>(session);
    RFavouritesDb db;
    User::LeaveIfError( db.Open(session, KBrowserSavedPages) );
    CleanupClosePushL<RFavouritesDb>(db);

    CFavouritesItem* item = CFavouritesItem::NewLC();
    item->SetType( CFavouritesItem::EFolder );
    item->SetParentFolder( KFavouritesRootUid ) ;
    item->SetNameL( parameters->iSavedDeckFolderName->Des() );
    
    err = db.Add(*item, /*aAutoRename=*/ETrue);
    if (err)
        db.Commit();

    // id has been generated, store it for being able to delete
    // only entries created with Creator
    iSpFEntryIds.AppendL( item->Uid() );

    CleanupStack::PopAndDestroy(3); //db, session, item
                
    return err;
    
    }


//----------------------------------------------------------------------------
void CCreatorBrowser::DeleteAllL()
    {
    LOGSTRING("Creator: CCreatorBrowser::DeleteAllL");
    User::Leave( KErrNotSupported );
    // will not be supported, see separate implementations below
    }

//----------------------------------------------------------------------------
void CCreatorBrowser::DeleteAllCreatedByCreatorL()
    {
    LOGSTRING("Creator: CCreatorBrowser::DeleteAllCreatedByCreatorL");
    User::Leave( KErrNotSupported );
    // will not be supported, see separate implementations below
    }

//----------------------------------------------------------------------------
void CCreatorBrowser::DeleteAllBookmarksL()
    {
    LOGSTRING("Creator: CCreatorBrowser::DeleteAllBookmarksL");
    DeleteAllItemsL( KBrowserBookmarks,
                     CFavouritesItem::EItem,
                     EFalse,
                     iBmEntryIds, // ignored, because deleting all
                     KUidDictionaryUidBrowserBookmarks );
    }

//----------------------------------------------------------------------------
void CCreatorBrowser::DeleteAllBookmarksCreatedByCreatorL()
    {
    LOGSTRING("Creator: CCreatorBrowser::DeleteAllBookmarksCreatedByCreatorL");
    DeleteAllItemsL( KBrowserBookmarks,
                     CFavouritesItem::EItem,
                     ETrue,
                     iBmEntryIds,
                     KUidDictionaryUidBrowserBookmarks );
    }

//----------------------------------------------------------------------------
void CCreatorBrowser::DeleteAllBookmarkFoldersL()
    {
    LOGSTRING("Creator: CCreatorBrowser::DeleteAllBookmarkFoldersL");
    DeleteAllItemsL( KBrowserBookmarks,
                     CFavouritesItem::EFolder,
                     EFalse,
                     iBmFEntryIds, // ignored, because deleting all
                     KUidDictionaryUidBrowserBookmarkF );
    }

//----------------------------------------------------------------------------
void CCreatorBrowser::DeleteAllBookmarkFoldersCreatedByCreatorL()
    {
    LOGSTRING("Creator: CCreatorBrowser::DeleteAllBookmarkFoldersCreatedByCreatorL");
    DeleteAllItemsL( KBrowserBookmarks,
                     CFavouritesItem::EFolder,
                     ETrue,
                     iBmFEntryIds,
                     KUidDictionaryUidBrowserBookmarkF );
    }

//----------------------------------------------------------------------------
void CCreatorBrowser::DeleteAllSavedPagesL()
    {
    LOGSTRING("Creator: CCreatorBrowser::DeleteAllSavedPagesL");
    DeleteAllItemsL( KBrowserSavedPages,
                     CFavouritesItem::EItem,
                     EFalse,
                     iSpEntryIds, // ignored, because deleting all
                     KUidDictionaryUidBrowserSavedPg );
    }

//----------------------------------------------------------------------------
void CCreatorBrowser::DeleteAllSavedPagesCreatedByCreatorL()
    {
    LOGSTRING("Creator: CCreatorBrowser::DeleteAllSavedPagesCreatedByCreatorL");
    DeleteAllItemsL( KBrowserSavedPages,
                     CFavouritesItem::EItem,
                     ETrue,
                     iSpEntryIds,
                     KUidDictionaryUidBrowserSavedPg );
    }

//----------------------------------------------------------------------------
void CCreatorBrowser::DeleteAllSavedPageFoldersL()
    {
    LOGSTRING("Creator: CCreatorBrowser::DeleteAllSavedPageFoldersL");
    DeleteAllItemsL( KBrowserSavedPages,
                     CFavouritesItem::EFolder,
                     EFalse,
                     iSpFEntryIds, // ignored, because deleting all
                     KUidDictionaryUidBrowserSavedPgF );
    }

//----------------------------------------------------------------------------
void CCreatorBrowser::DeleteAllSavedPageFoldersCreatedByCreatorL()
    {
    LOGSTRING("Creator: CCreatorBrowser::DeleteAllSavedPageFolderssCreatedByCreatorL");
    DeleteAllItemsL( KBrowserSavedPages,
                     CFavouritesItem::EFolder,
                     ETrue,
                     iSpFEntryIds,
                     KUidDictionaryUidBrowserSavedPgF );
    }

//----------------------------------------------------------------------------
void CCreatorBrowser::DeleteAllItemsL( const TDesC& aDbName, 
                                       CFavouritesItem::TType aTypeFilter,
                                       TBool aOnlyCreatedWithCreator,
                                       RArray<TInt>& aEntryIds,
                                       const TUid aDictUid )
    {
    LOGSTRING("Creator: CCreatorBrowser::DeleteItemL");
    RFavouritesSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL( session );
    RFavouritesDb db;
    User::LeaveIfError( db.Open( session, aDbName ) );
    CleanupClosePushL( db );
       
    if ( aOnlyCreatedWithCreator )
        {
        // read uids from Creator internal store
        aEntryIds.Reset();
        iEngine->ReadEntryIdsFromStoreL( aEntryIds, aDictUid );
        
        // delete entries from db
        for ( TInt i = 0; i < aEntryIds.Count(); i++ )
            {
            db.Delete( aEntryIds[i] );
            }
        }
    else
        {
        CArrayFixFlat<TInt>* uids = new (ELeave) CArrayFixFlat<TInt>(64);
        CleanupStack::PushL( uids );
        
        // read uids from browser db
        User::LeaveIfError( db.GetUids( *uids, KFavouritesRootUid, aTypeFilter ) );

        // delete entries from db
        for ( TInt i = 0; i < uids->Count(); i++ )
            {
            db.Delete( uids->At(i) );
            }
        
        // db.Commit() seems to panic with EFavouritesNoTransaction
        // deletion is successful even without commit, so not calling it
        CleanupStack::PopAndDestroy( uids );
        }
       
    // entries deleted, remove the Browser related registry
    aEntryIds.Reset();
    iEngine->RemoveStoreL( aDictUid );

    CleanupStack::PopAndDestroy( &db );
    CleanupStack::PopAndDestroy( &session );
    }
