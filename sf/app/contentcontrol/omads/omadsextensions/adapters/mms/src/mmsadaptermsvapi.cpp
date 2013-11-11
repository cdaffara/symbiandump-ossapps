/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Part of SyncML Data Synchronization Plug In Adapter
*
*/



// INCLUDE FILES
#include <msvapi.h>
#include <msvuids.h>
#include <msvstd.h>
#include <msvids.h>
#include "mmsadaptermsvapi.h" 
#include "logger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmsAdapterMsvApi::NewL
// -----------------------------------------------------------------------------
//
 CMmsAdapterMsvApi* CMmsAdapterMsvApi::NewL( CMsvSession& aMsvSession )
    {
    CMmsAdapterMsvApi* self = new( ELeave ) CMmsAdapterMsvApi( aMsvSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CMmsAdapterMsvApi::~CMmsAdapterMsvApi()
// -----------------------------------------------------------------------------
//
CMmsAdapterMsvApi::~CMmsAdapterMsvApi()
    {
    TRACE_FUNC;
    }
       
// -----------------------------------------------------------------------------
// CMmsAdapterMsvApi::CMmsAdapterMsvApi
// -----------------------------------------------------------------------------
//
CMmsAdapterMsvApi::CMmsAdapterMsvApi( CMsvSession& aMsvSession ):
    iSession( aMsvSession )
    { 
    }

// -----------------------------------------------------------------------------
// CMmsAdapterMsvApi::ConstructL
// -----------------------------------------------------------------------------
//
void CMmsAdapterMsvApi::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CMmsAdapterMsvApi::DeleteUserFolderL
// -----------------------------------------------------------------------------
//
 TInt CMmsAdapterMsvApi::DeleteUserFolderL( TMsvId aUid )
    {
    TRACE_FUNC_ENTRY;
    
    CMsvEntry* entry = iSession.GetEntryL(aUid);
    CleanupStack::PushL(entry);
    
    TMsvEntry tEntry = entry->Entry();
    TMsvId parent = tEntry.Parent();        
    
    if ( tEntry.iType != KUidMsvFolderEntry || parent != KMsvMyFoldersEntryIdValue )
        {
        LOGGER_WRITE( "Not correct folder" );
        CleanupStack::PopAndDestroy(entry);
        LOGGER_LEAVEFN( "MsvApi::DeleteUserFolderL" );
        return KErrNotSupported;
        }
        
    CMsvEntrySelection* children = entry->ChildrenL();
    TInt count = children->Count();
    delete children;
    
    if ( count > 0 )
        {
        LOGGER_WRITE( "Folder not empty" );
        CleanupStack::PopAndDestroy( entry );
        LOGGER_LEAVEFN( "MsvApi::DeleteUserFolderL" );
        return KErrInUse;
        }
        
    tEntry.SetReadOnly( EFalse );
    entry->ChangeL( tEntry );
    
    entry->SetEntryL( parent );
    entry->DeleteL( aUid );
    
    CleanupStack::PopAndDestroy( entry );
    TRACE_FUNC_EXIT;
    return KErrNone;
    }   
    
// -----------------------------------------------------------------------------
// CMmsAdapterMsvApi::ValidUserFolder
// -----------------------------------------------------------------------------
//
TBool CMmsAdapterMsvApi::ValidFolderL( TMsvId aFolder, TBool aOutboxValid )
    {
    TBool valid(EFalse);
    
    switch ( aFolder )
        {
        case KMsvGlobalInBoxIndexEntryId:
            valid = ETrue;  
            break;
        case KMsvDraftEntryId:
           valid = ETrue;  
           break;        
        case KMsvSentEntryId:
           valid = ETrue;  
           break;
        case KMsvGlobalOutBoxIndexEntryId:
            if ( aOutboxValid )
                {
                valid = ETrue;
                }
            break;
        case KMsvMyFoldersEntryIdValue:
            valid = ETrue;
            break;
        default:
            valid = FindUserFolderL( aFolder );
        }
    
    return valid;          
    }

// -----------------------------------------------------------------------------
// CMmsAdapterMsvApi::FindUserFolderL
// -----------------------------------------------------------------------------
//    
TBool CMmsAdapterMsvApi::FindUserFolderL( TMsvId aFolder, TPtrC& aName, TTime& aDate )
    {
    TBool found( EFalse );
    
    CMsvEntry* entry = iSession.GetEntryL( KMsvMyFoldersEntryIdValue );
    CleanupStack::PushL( entry );
     
    CMsvEntrySelection* selection = entry->ChildrenL();
    CleanupStack::PushL( selection );
    
    TMsvId serviceId;
    TMsvEntry entryT;

    for ( TInt i = 0; i < selection->Count(); i++ )
        {
        User::LeaveIfError( iSession.GetEntry( selection->At( i ), serviceId, entryT ) );
        
        if ( !entryT.Deleted() && entryT.iType == KUidMsvFolderEntry && entryT.Id() == aFolder )
            {
            found = ETrue;
            aDate = entryT.iDate;
            aName.Set( entryT.iDetails );
            break;
            }
        }
    
    CleanupStack::PopAndDestroy( selection );
    CleanupStack::PopAndDestroy( entry ); 
    
    return found;        
    }
    
// -----------------------------------------------------------------------------
// CMmsAdapterMsvApi::FindUserFolderL
// -----------------------------------------------------------------------------
//    
TBool CMmsAdapterMsvApi::FindUserFolderL( TMsvId aFolder )
    {
    TPtrC name;
    TTime time;
    
    return FindUserFolderL( aFolder, name, time ); 
    }    
    
// -----------------------------------------------------------------------------
// CMmsAdapterMsvApi::FindUserFolderL
// -----------------------------------------------------------------------------
//    
TBool CMmsAdapterMsvApi::FindUserFolderL( const TDesC& aName, TMsvId& aFolder )
    {
    CMsvEntry* entry = iSession.GetEntryL( KMsvMyFoldersEntryIdValue );
    CleanupStack::PushL( entry );
     
    CMsvEntrySelection* selection = entry->ChildrenL();
    CleanupStack::PushL( selection );
    
    TBool found( EFalse );
    TMsvId serviceId;
    TMsvEntry entryT;

    for ( TInt i = 0; i < selection->Count(); i++ )
        {
        User::LeaveIfError( iSession.GetEntry( selection->At( i ), serviceId, entryT ) );
        
        if ( !entryT.Deleted() && entryT.iType == KUidMsvFolderEntry && 
            aName.Compare(entryT.iDescription) == 0 )
            {
            found = ETrue;
            aFolder = entryT.Id();
            break;
            }
        }
    
    CleanupStack::PopAndDestroy( selection );
    CleanupStack::PopAndDestroy( entry );
    
    return found;           
    }

// -----------------------------------------------------------------------------
// CMmsAdapterMsvApi::AddUserFolderL
// Creates new user folder
// -----------------------------------------------------------------------------        
TInt CMmsAdapterMsvApi::AddUserFolderL( TMsvId& aFolder, const TDesC& aName )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "aName: %S", &aName );
        
    // Make sure that we are not going to add same folder twise
    TBool found( EFalse );
    found = FindUserFolderL( aName, aFolder );
    if ( found )
        {
        LOGGER_WRITE( "Folder already exists" );
        LOGGER_LEAVEFN( "CMmsAdapterMsvApi::AddUserFolderL" );
        return KErrNone;
        } 
        
    CMsvEntry* entry = iSession.GetEntryL(KMsvMyFoldersEntryIdValue);
    CleanupStack::PushL( entry );
    
    TTime date;
    date.UniversalTime();    
    
    TMsvEntry folderEntry;
    folderEntry.iType = KUidMsvFolderEntry;
    folderEntry.iMtm = KUidMsvLocalServiceMtm;
    folderEntry.iDetails.Set( aName );   
    folderEntry.iServiceId = KMsvLocalServiceIndexEntryIdValue;
    folderEntry.iSize = sizeof( folderEntry );
    folderEntry.iDate = date;
    folderEntry.SetStandardFolder( EFalse );
    folderEntry.SetVisible( ETrue );
    folderEntry.SetComplete( ETrue );
    folderEntry.SetInPreparation( EFalse ); 
    folderEntry.SetReadOnly( EFalse );
    
    entry->CreateL( folderEntry );
    CleanupStack::PopAndDestroy( entry );
    
    aFolder = folderEntry.Id();
    
    TRACE_FUNC_EXIT;
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CMmsAdapterMsvApi::UpdateUserFolderL
// Updates user folder (changes name)
// -----------------------------------------------------------------------------    
TInt CMmsAdapterMsvApi::UpdateUserFolderL( TMsvId aFolder, const TDesC& aName )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "aName: %S", &aName );
    
    CMsvEntry* entry = iSession.GetEntryL( aFolder );
    CleanupStack::PushL( entry );
    
    TMsvEntry tEntry = entry->Entry();
    
    if ( tEntry.iType != KUidMsvFolderEntry )
        {
        CleanupStack::PopAndDestroy( entry );
        LOGGER_WRITE( "No message folder" );
        TRACE_FUNC_EXIT;
        return KErrNotSupported;
        }
       
    tEntry.iDetails.Set( aName );   
    tEntry.iDescription.Set( aName );
    
    entry->ChangeL( tEntry );
    
    CleanupStack::PopAndDestroy( entry );
    
    TRACE_FUNC_EXIT;
    return KErrNone;
    } 
    
// End of file
