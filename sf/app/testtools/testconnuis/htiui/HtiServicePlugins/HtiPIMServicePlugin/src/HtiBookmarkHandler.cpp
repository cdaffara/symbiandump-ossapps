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
* Description:  Implementation of browser bookmark handling.
*
*/


// INCLUDE FILES
#include "HtiBookmarkHandler.h"
#include "HtiPIMServicePlugin.h"

#include <cmconnectionmethod.h>
#include <cmconnectionmethoddef.h>
#include <cmdestination.h>
#include <cmmanager.h>

#include <favouritesitem.h>
#include <favouritesitemlist.h>
#include <favouritesdb.h>
#include <favouritesfile.h>
#include <favouriteswapap.h>

#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
_LIT8( KErrorUnrecognizedCommand, "Unrecognized command" );
_LIT8( KErrorInvalidParameters, "Invalid command parameters" );
_LIT8( KErrorCreateFailed, "Bookmark creation failed" );
_LIT8( KErrorDeleteFailed, "Bookmark deletion failed" );
_LIT8( KErrorBookmarkNotFound, "Bookmark not found" );
_LIT8( KErrorFolderNotFound, "Folder not found" );
_LIT8( KErrorApNotFound, "Access point not found" );
_LIT8( KErrorFolderSearchFailed, "Error when searching folder" );
_LIT8( KErrorApSearchFailed, "Error when searching access point" );

const TInt KCreateMessageMinLength = 10;
const TInt KDeleteMessageMinLength = 3;

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHtiBookmarkHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CHtiBookmarkHandler* CHtiBookmarkHandler::NewL()
    {
    HTI_LOG_FUNC_IN( "CHtiBookmarkHandler::NewL" );
    CHtiBookmarkHandler* self = new ( ELeave ) CHtiBookmarkHandler();
    CleanupStack::PushL ( self );
    self->ConstructL();
    CleanupStack::Pop();
    HTI_LOG_FUNC_OUT( "CHtiBookmarkHandler::NewL" );
    return self;
    }


// ----------------------------------------------------------------------------
// CHtiBookmarkHandler::CHtiBookmarkHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
CHtiBookmarkHandler::CHtiBookmarkHandler()
    {
    HTI_LOG_FUNC_IN( "CHtiBookmarkHandler::CHtiBookmarkHandler" );
    HTI_LOG_FUNC_OUT( "CHtiBookmarkHandler::CHtiBookmarkHandler" );
    }


// -----------------------------------------------------------------------------
// CHtiBookmarkHandler::~CHtiBookmarkHandler
// Destructor.
// -----------------------------------------------------------------------------
CHtiBookmarkHandler::~CHtiBookmarkHandler()
    {
    HTI_LOG_FUNC_IN( "CHtiBookmarkHandler::~CHtiBookmarkHandler" );
    delete iUrl;
    iFavSession.Close();
    HTI_LOG_FUNC_OUT( "CHtiBookmarkHandler::~CHtiBookmarkHandler" );
    }


// -----------------------------------------------------------------------------
// CHtiBookmarkHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CHtiBookmarkHandler::ConstructL()
    {
    HTI_LOG_FUNC_IN( "CHtiBookmarkHandler::ConstructL" );
    User::LeaveIfError( iFavSession.Connect() );
    HTI_LOG_FUNC_OUT( "CHtiBookmarkHandler::ConstructL" );
    }


// -----------------------------------------------------------------------------
// CHtiBookmarkHandler::SetDispatcher
// Sets the dispatcher pointer.
// -----------------------------------------------------------------------------

void CHtiBookmarkHandler::SetDispatcher( MHtiDispatcher* aDispatcher )
    {
    HTI_LOG_FUNC_IN( "CHtiBookmarkHandler::SetDispatcher" );
    iDispatcher = aDispatcher;
    HTI_LOG_FUNC_OUT( "CHtiBookmarkHandler::SetDispatcher" );
    }


// -----------------------------------------------------------------------------
// CHtiBookmarkHandler::ProcessMessageL
// Parses the received message and calls handler functions.
// -----------------------------------------------------------------------------
void CHtiBookmarkHandler::ProcessMessageL( const TDesC8& aMessage,
    THtiMessagePriority /*aPriority*/ )
    {
    HTI_LOG_FUNC_IN( "CHtiBookmarkHandler::ProcessMessageL" );
    // Zero legth of aMessage tested already in CHtiPIMServicePlugin.
    // Other sanity checks must be done here.
    if ( aMessage[0] == CHtiPIMServicePlugin::ECreateBookmark )
        {
        TRAPD( err, ParseCreateMessageL( aMessage ) );
        if ( err != KErrNone )
            {
            SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
            }
        else
            {
            TRAP( err, HandleCreateBookmarkL() );
            if ( err != KErrNone )
                {
                SendErrorMessageL( err, KErrorCreateFailed );
                }
            }
        }

    else if ( aMessage[0] == CHtiPIMServicePlugin::EDeleteBookmark )
        {
        TRAPD( err, ParseDeleteMessageL( aMessage ) );
        if ( err != KErrNone )
            {
            SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
            }
        else
            {
            TRAP( err, HandleDeleteBookmarkL() );
            if ( err != KErrNone )
                {
                SendErrorMessageL( err, KErrorDeleteFailed );
                }
            }
        }

    else
        {
        SendErrorMessageL( KErrArgument, KErrorUnrecognizedCommand );
        }
    HTI_LOG_FUNC_OUT( "CHtiBookmarkHandler::ProcessMessageL" );
    }


// ----------------------------------------------------------------------------
// CHtiBookmarkHandler::IsBusy
// Returns the status whether this handler is busy processing a request.
// ----------------------------------------------------------------------------
TBool CHtiBookmarkHandler::IsBusy()
    {
    // Never busy if ProcessMessageL has completed.
    return EFalse;
    }


// ----------------------------------------------------------------------------
// CHtiBookmarkHandler::HandleCreateBookmarkL
// Handles the bookmark and/or folder creation command.
// ----------------------------------------------------------------------------
void CHtiBookmarkHandler::HandleCreateBookmarkL()
    {
    HTI_LOG_FUNC_IN( "CHtiBookmarkHandler::HandleCreateBookmarkL" );
    TInt err = KErrNone;
    // If folder given, get the UID if it exists
    TInt folderUid = KFavouritesRootUid;
    if ( iFolderName.Length() > 0 )
        {
        TRAP( err, folderUid = GetFolderUidL( iFolderName ) );
        if ( err != KErrNone )
            {
            SendErrorMessageL( err, KErrorFolderSearchFailed );
            return;
            }
        }
    HTI_LOG_FORMAT( "Folder UID = %d", folderUid );

    // If access point given, search the UID. It's an error if AP is not found.
    TInt apUid = KErrNotFound;
    if ( iApName.Length() > 0 )
        {
        TRAP( err, apUid = GetApUidL( iApName ) );
        if ( err != KErrNone )
            {
            SendErrorMessageL( err, KErrorApSearchFailed );
            return;
            }
        if ( apUid == KErrNotFound )
            {
            SendErrorMessageL( KErrNotFound, KErrorApNotFound );
            return;
            }
        }

    RFavouritesDb db;
    User::LeaveIfError( db.Open( iFavSession, KBrowserBookmarks ) );
    CleanupClosePushL<RFavouritesDb>( db );

    TInt itemsCreated = 0;
    // If folder was given but was not found, create the folder first.
    if ( iFolderName.Length() > 0 && folderUid == KErrNotFound )
        {
        CFavouritesItem* folder = CFavouritesItem::NewLC();
        folder->SetType( CFavouritesItem::EFolder );
        folder->SetParentFolder( KFavouritesRootUid ) ;
        folder->SetNameL( iFolderName );

        HTI_LOG_TEXT( "Adding the folder item to DB" );
        err = db.Add( *folder, EFalse );
        HTI_LOG_FORMAT( "Add returned %d", err );
        if ( err != KErrNone )
            {
            User::Leave( err );
            }
        folderUid = folder->Uid();
        HTI_LOG_FORMAT( "New folder UID = %d", folderUid );
        CleanupStack::PopAndDestroy(); // folder
        itemsCreated++;
        }

    // Now create the actual bookmark item
    CFavouritesItem* item = CFavouritesItem::NewLC();
    item->SetType( CFavouritesItem::EItem );
    item->SetParentFolder( folderUid ) ;
    item->SetNameL( iBookmarkName );
    item->SetUrlL( *iUrl );
    if ( apUid != KErrNotFound )
        {
        TFavouritesWapAp favAp;
        favAp.SetApId(  ( TUint32 ) apUid );
        item->SetWapAp( favAp );
        }
    if ( iUserName.Length() > 0 )
        {
        item->SetUserNameL( iUserName );
        }
    if ( iPassword.Length() > 0 )
        {
        item->SetPasswordL( iPassword );
        }

    HTI_LOG_TEXT( "Adding the bookmark item to DB" );
    err = db.Add( *item, EFalse );
    HTI_LOG_FORMAT( "Add returned %d", err );
    if ( err )
        {
        User::Leave( err );
        }
    HTI_LOG_FORMAT( "Bookmark UID = %d", item->Uid() );
    CleanupStack::PopAndDestroy(); // item
    itemsCreated++;

    CleanupStack::PopAndDestroy(); // db

    TBuf8<1> response;
    response.Append( itemsCreated );
    SendOkMsgL( response );

    HTI_LOG_FUNC_OUT( "CHtiBookmarkHandler::HandleCreateBookmarkL" );
    }


// ----------------------------------------------------------------------------
// CHtiBookmarkHandler::HandleDeleteBookmarkL
// Handles the bookmark and/or folder deletion.
// ----------------------------------------------------------------------------
void CHtiBookmarkHandler::HandleDeleteBookmarkL()
    {
    HTI_LOG_FUNC_IN( "CHtiBookmarkHandler::HandleDeleteBookmarkL" );
    TInt err = KErrNone;
    // If folder given, get the UID if it exists.
    // It is an error if the given folder does not exist.
    TInt folderUid = KFavouritesRootUid;
    if ( iFolderName.Length() > 0 )
        {
        TRAP( err, folderUid = GetFolderUidL( iFolderName ) );
        if ( err != KErrNone )
            {
            SendErrorMessageL( err, KErrorFolderSearchFailed );
            return;
            }
        if ( folderUid == KErrNotFound )
            {
            SendErrorMessageL( KErrNotFound, KErrorFolderNotFound );
            return;
            }
        }
    HTI_LOG_FORMAT( "Folder UID = %d", folderUid );

    RFavouritesDb db;
    User::LeaveIfError( db.Open( iFavSession, KBrowserBookmarks ) );
    CleanupClosePushL<RFavouritesDb>( db );

    CArrayFixFlat<TInt>* deleteList = new ( ELeave ) CArrayFixFlat<TInt>( 10 );
    CleanupStack::PushL( deleteList );
    if ( iBookmarkName.Length() > 0 )
        {
        // Get the UID of bookmark item matching to iBookmarkName
        // It is an error if nothing found.
        CFavouritesItemList* items = new ( ELeave ) CFavouritesItemList();
        CleanupStack::PushL( items );
        User::LeaveIfError( db.GetAll( *items, folderUid,
            CFavouritesItem::EItem ) );
        TInt uid = KErrNotFound;
        TInt itemCount = items->Count();
        HTI_LOG_FORMAT( "Found %d items", itemCount );
        TInt i = 0;
        while ( uid == KErrNotFound && i < itemCount )
            {
            if ( ( *items )[i]->Name().CompareF( iBookmarkName ) == 0 )
                {
                uid = ( *items )[i]->Uid();
                }
            i++;
            }
        CleanupStack::PopAndDestroy(); // items
        HTI_LOG_FORMAT( "Matching item UID: %d", uid );
        if ( uid == KErrNotFound )
            {
            SendErrorMessageL( KErrNotFound, KErrorBookmarkNotFound );
            CleanupStack::PopAndDestroy( 2 ); // deleteList, db
            return;
            }
        else
            {
            deleteList->AppendL( uid );
            }
        }
    else
        {
        // We are deleting a folder (whose UID we already have) or deleting all.
        if ( folderUid != KFavouritesRootUid )
            {
            deleteList->AppendL( folderUid );
            }
        else
            {
            // We don't have folder or bookmark name - deleting all
            User::LeaveIfError( db.GetUids( *deleteList, KFavouritesRootUid ) );
            HTI_LOG_FORMAT( "Found %d matching items", deleteList->Count() );
            }
        }

    TInt beginningCount = KErrNotFound;
    User::LeaveIfError( db.Count( beginningCount ) );
    HTI_LOG_FORMAT( "Items in DB before delete: %d", beginningCount );

    // Start deleting
    TInt deleteCount = 0;
    for ( TInt i = 0; i < deleteList->Count(); i++ )
        {
        HTI_LOG_FORMAT( "Deleting with UID %d", ( *deleteList )[i] );
        err = db.Delete( ( *deleteList )[i] );
        HTI_LOG_FORMAT( "Delete returned %d", err );
        if ( err == KErrNone )
            {
            deleteCount++;
            }
        if ( err != KErrNone && err != KErrAccessDenied )
            {
            HTI_LOG_TEXT( "Unexpected error when deleting - leaving" );
            User::Leave( err );
            }
        }

    TInt endingCount = KErrNotFound;
    User::LeaveIfError( db.Count( endingCount ) );
    HTI_LOG_FORMAT( "Items in DB after delete: %d", endingCount );

    CleanupStack::PopAndDestroy( 2 ); // deleteList, db

    TBuf8<1> response;
    response.Append( beginningCount - endingCount );
    SendOkMsgL( response );

    HTI_LOG_FUNC_OUT( "CHtiBookmarkHandler::HandleDeleteBookmarkL" );
    }


// ----------------------------------------------------------------------------
// CHtiBookmarkHandler::ParseCreateMessageL
// Extracts the parameters from a create bookmark message.
// ----------------------------------------------------------------------------
void CHtiBookmarkHandler::ParseCreateMessageL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiBookmarkHandler::ParseCreateMessageL" );
    TInt length = aMessage.Length();
    HTI_LOG_FORMAT( "CreateBookmark message length = %d", length );

    if ( length < KCreateMessageMinLength )
        {
        User::Leave( KErrArgument );
        }

    // Extract folder name
    TInt offset = 1; // skip the command code in position 0
    offset = ExtractStringL( aMessage, offset, 1, 0, KFavouritesMaxName,
                             iFolderName );
    HTI_LOG_FORMAT( "Folder name: %S", &iFolderName );

    // Extract bookmark name
    offset = ExtractStringL( aMessage, offset, 1, 1, KFavouritesMaxName,
                             iBookmarkName );
    HTI_LOG_FORMAT( "Bookmark name: %S", &iBookmarkName );

    // Extract URL
    delete iUrl;
    iUrl = NULL;
    iUrl = HBufC::NewL( KFavouritesMaxUrl );
    TPtr urlPtr = iUrl->Des();
    offset = ExtractStringL( aMessage, offset, 2, 1, KFavouritesMaxUrl,
                             urlPtr );
    HTI_LOG_FORMAT( "URL: %S", iUrl );

    // Extract AP name
    offset = ExtractStringL( aMessage, offset, 1, 0, KApMaxConnNameLength,
                             iApName );
    HTI_LOG_FORMAT( "AP name: %S", &iApName );

    // Extract user name
    offset = ExtractStringL( aMessage, offset, 1, 0, KFavouritesMaxUserName,
                             iUserName );
    HTI_LOG_FORMAT( "User name: %S", &iUserName );

    // Extract password
    offset = ExtractStringL( aMessage, offset, 1, 0, KFavouritesMaxPassword,
                             iPassword );
    HTI_LOG_FORMAT( "Password: %S", &iPassword );

    // Check that there's no extra data
    if ( offset != length  )
        {
        User::Leave( KErrArgument );
        }

    HTI_LOG_FUNC_OUT( "CHtiBookmarkHandler::ParseCreateMessageL" );
    }


// ----------------------------------------------------------------------------
// CHtiBookmarkHandler::ParseDeleteMessageL
// Extracts the parameters from a delete bookmark message.
// ----------------------------------------------------------------------------
void CHtiBookmarkHandler::ParseDeleteMessageL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiBookmarkHandler::ParseDeleteMessageL" );
    TInt length = aMessage.Length();
    HTI_LOG_FORMAT( "DeleteBookmark message length = %d", length );

    if ( length < KDeleteMessageMinLength )
        {
        User::Leave( KErrArgument );
        }

    // Extract folder name
    TInt offset = 1; // skip the command code in position 0
    offset = ExtractStringL( aMessage, offset, 1, 0, KFavouritesMaxName,
                             iFolderName );
    HTI_LOG_FORMAT( "Folder name: %S", &iFolderName );

    // Extract bookmark name
    offset = ExtractStringL( aMessage, offset, 1, 0, KFavouritesMaxName,
                             iBookmarkName );
    HTI_LOG_FORMAT( "Bookmark name: %S", &iBookmarkName );

    HTI_LOG_FUNC_OUT( "CHtiBookmarkHandler::ParseDeleteMessageL" );
    }


// ----------------------------------------------------------------------------
// CHtiBookmarkHandler::ExtractStringL
// Extract one string from the given message. Leaves on syntax errors.
// ----------------------------------------------------------------------------
TInt CHtiBookmarkHandler::ExtractStringL( const TDesC8& aMessage, TInt aOffset,
        TInt aLengthBytes, TInt aMinLength, TInt aMaxLength, TDes& aResult )
    {
    HTI_LOG_FUNC_IN( "CHtiBookmarkHandler::ExtractStringL" );
    __ASSERT_ALWAYS( aResult.MaxLength() >= aMaxLength,
                     User::Panic( _L( "HtiPimAssert" ), 1 ) );
    __ASSERT_ALWAYS( aLengthBytes > 0, User::Panic( _L( "HtiPimAssert" ), 2 ) );

    TInt length = aMessage.Length();
    if ( length < aOffset + 1 )
        {
        User::Leave( KErrArgument );
        }
    TInt parseLength = aMessage[aOffset];
    aOffset++;
    if ( aLengthBytes > 1 )
        {
        for ( TInt i = 1; i < aLengthBytes; i++ )
            {
            parseLength += aMessage[aOffset] << ( i * 8 );
            aOffset++;
            }
        }
    HTI_LOG_FORMAT( "Parse length = %d", parseLength );
    if ( parseLength < aMinLength  || parseLength > aMaxLength ||
         parseLength + aOffset > length )
        {
        User::Leave( KErrArgument );
        }
    else
        {
        aResult.Copy( aMessage.Mid( aOffset, parseLength ) );
        aOffset += parseLength;
        }
    HTI_LOG_FUNC_OUT( "CHtiBookmarkHandler::ExtractStringL" );
    return aOffset;
    }


// ----------------------------------------------------------------------------
// CHtiBookmarkHandler::GetFolderUidL
// Finds the UID of the named folder if it exists.
// ----------------------------------------------------------------------------
TInt CHtiBookmarkHandler::GetFolderUidL( const TDesC& aFolderName )
    {
    HTI_LOG_FUNC_IN( "CHtiBookmarkHandler::GetFolderUidL" );
    HTI_LOG_FORMAT( "Searching folder %S", &aFolderName );
    RFavouritesDb db;
    User::LeaveIfError( db.Open( iFavSession, KBrowserBookmarks ) );
    CleanupClosePushL<RFavouritesDb>( db );

    /* Doing the name matching ourselves 'cause giving the name match pattern
     * parameter to GetAll or GetUids methods crashes the favourites server.
     */
    CFavouritesItemList* folders = new ( ELeave ) CFavouritesItemList();
    CleanupStack::PushL( folders );
    User::LeaveIfError( db.GetAll( *folders, KFavouritesNullUid,
        CFavouritesItem::EFolder ) );
    TInt uid = KErrNotFound;
    TInt folderCount = folders->Count();
    HTI_LOG_FORMAT( "Found %d folders", folderCount );
    TInt i = 0;
    while ( uid == KErrNotFound && i < folderCount )
        {
        if ( ( *folders )[i]->Name().CompareF( aFolderName ) == 0 )
            {
            uid = ( *folders )[i]->Uid();
            }
        i++;
        }

    CleanupStack::PopAndDestroy( 2 ); // folders, db
    HTI_LOG_FORMAT( "Matching UID: %d", uid );
    HTI_LOG_FUNC_OUT( "CHtiBookmarkHandler::GetFolderUidL" );
    return uid;
    }


// ----------------------------------------------------------------------------
// CHtiBookmarkHandler::GetApUidL
// Gets the WAP Access Point ID of access point named by iConnName.
// Favourites item needs the WAP ID for identifying access point.
// Returns KErrNotFound if AP not found.
// ----------------------------------------------------------------------------
TInt CHtiBookmarkHandler::GetApUidL( const TDesC& aApName )
    {
    HTI_LOG_FUNC_IN( "CHtiBookmarkHandler::GetApUidL" );
    TInt uid = KErrNotFound;

    RCmManager cmManager;
    cmManager.OpenL();
    CleanupClosePushL( cmManager );

    // Search from uncategorised first
    RArray<TUint32> array = RArray<TUint32>();
    cmManager.ConnectionMethodL( array );
    CleanupClosePushL( array );
    TInt i = 0;
    while ( i < array.Count() && uid == KErrNotFound )
        {
        RCmConnectionMethod cm = cmManager.ConnectionMethodL( array[i] );
        CleanupClosePushL( cm );
        HBufC* name = cm.GetStringAttributeL( CMManager::ECmName );
        HTI_LOG_FORMAT( "Found name: %S", name );
        CleanupStack::PushL( name );
        if ( aApName.Compare( *name ) == 0 )
            {
            uid = cm.GetIntAttributeL( CMManager::ECmWapId );
            HTI_LOG_FORMAT( "Match: UID = %d", uid );
            }
        CleanupStack::PopAndDestroy(); // name
        CleanupStack::PopAndDestroy(); // cm
        i++;
        }
    CleanupStack::PopAndDestroy(); // array

    // If not found from uncategorised, search from all destinations
    if ( uid == KErrNotFound )
        {
        HTI_LOG_TEXT( "Not found from uncategorised" );
        RArray<TUint32> destIdArray = RArray<TUint32>();
        cmManager.AllDestinationsL( destIdArray );
        CleanupClosePushL( destIdArray );
        i = 0;
        while ( i < destIdArray.Count() && uid == KErrNotFound )
            {
            RCmDestination dest = cmManager.DestinationL( destIdArray[i] );
            CleanupClosePushL( dest );
            TInt j = 0;
            while ( j < dest.ConnectionMethodCount() && uid == KErrNotFound )
                {
                HBufC* name = dest.ConnectionMethodL( j ).GetStringAttributeL(
                        CMManager::ECmName );
                CleanupStack::PushL( name );
                HTI_LOG_FORMAT( "Found name: %S", name );
                if ( aApName.Compare( *name ) == 0 )
                    {
                    uid = dest.ConnectionMethodL( j ).GetIntAttributeL(
                            CMManager::ECmWapId );
                    HTI_LOG_FORMAT( "Match: UID = %d", uid );
                    }
                CleanupStack::PopAndDestroy(); // name
                j++;
                }
            CleanupStack::PopAndDestroy(); // dest
            i++;
            }
        CleanupStack::PopAndDestroy(); // destIdArray
        }

    CleanupStack::PopAndDestroy(); // cmManager
    HTI_LOG_FUNC_OUT( "CHtiBookmarkHandler::GetApUidL" );
    return uid;
    }


// ----------------------------------------------------------------------------
// CHtiBookmarkHandler::SendOkMsgL
// Helper function for sending response messages.
// ----------------------------------------------------------------------------
void CHtiBookmarkHandler::SendOkMsgL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiBookmarkHandler::SendOkMsgL" );
    User::LeaveIfNull( iDispatcher );
    HBufC8* temp = HBufC8::NewL( aData.Length() + 1 );
    TPtr8 response = temp->Des();
    response.Append( ( TChar ) CHtiPIMServicePlugin::EResultOk );
    response.Append( aData );
    User::LeaveIfError( iDispatcher->DispatchOutgoingMessage(
        temp, KPIMServiceUid ) );
    HTI_LOG_FUNC_OUT( "CHtiBookmarkHandler::SendOkMsgL" );
    }


// ----------------------------------------------------------------------------
// CHtiBookmarkHandler::SendErrorMessageL
// Helper function for sending error response messages.
// ----------------------------------------------------------------------------
void CHtiBookmarkHandler::SendErrorMessageL( TInt aError,
        const TDesC8& aDescription )
    {
    HTI_LOG_FUNC_IN( "CHtiBookmarkHandler::SendErrorMessageL" );
    User::LeaveIfNull( iDispatcher );
    User::LeaveIfError( iDispatcher->DispatchOutgoingErrorMessage(
        aError, aDescription, KPIMServiceUid ) );
    HTI_LOG_FUNC_OUT( "CHtiBookmarkHandler::SendErrorMessageL" );
    }


// End of file
