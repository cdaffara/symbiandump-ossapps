/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  mmsmessagestoreutils implementation
*
*/


   
// INCLUDE FILES   
#include <msvids.h>
#include <msvapi.h>

// USERINCLUDE FILES
#include "mmsmessagestoreutils.h"
#include "mmsdebuglogging.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ============================== LOCAL FUNCTIONS ==============================

// ============================== MEMBER FUNCTIONS =============================

// -----------------------------------------------------------------------------
// CMmsMessageStoreUtils::CMmsMessageStoreUtils
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmsMessageStoreUtils::CMmsMessageStoreUtils()   
    {
    }

// -----------------------------------------------------------------------------
// CMmsMessageStoreUtils::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmsMessageStoreUtils::ConstructL( CMsvSession& aSession )
    {
    LOG(_L("CMmsMessageStoreUtils::ConstructL"));
    iSession = &aSession;
    iParentEntry = iSession->GetEntryL( KMsvRootIndexEntryId );
    iMmsEntry = iSession->GetEntryL( KMsvRootIndexEntryId );
    }

// -----------------------------------------------------------------------------
// CMmsMessageStoreUtils::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMmsMessageStoreUtils* CMmsMessageStoreUtils::NewL( CMsvSession& aSession )
    {
    CMmsMessageStoreUtils* self = new ( ELeave ) CMmsMessageStoreUtils;
    CleanupStack::PushL( self );
    self->ConstructL( aSession );
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CMmsMessageStoreUtils::~CMmsMessageStoreUtils()
    {
    LOG(_L("~CMmsMessageStoreUtils"));
    // iSession is not owned by CMmsMessageStoreUtils
    delete iParentEntry;
    delete iMmsEntry;
    }

// -----------------------------------------------------------------------------
// CreateServiceEntryL
// -----------------------------------------------------------------------------
//
TMsvId CMmsMessageStoreUtils::CreateServiceEntryL()
    {
    LOG(_L("CMmsMessageStoreUtils::CreateServiceEntryL"));

    //
    // Find old MMS entries
    //
    iParentEntry->SetEntryL( KMsvRootIndexEntryId );
    // Find also invisible entries
    iParentEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
    CMsvEntrySelection* selection = iParentEntry->ChildrenWithMtmL( KUidMsgTypeMultimedia );
    CleanupStack::PushL( selection ); // ***
    TInt count = selection->Count();
    LOG2(_L("- found %d MMS entries"), count );

    //
    // Concentrate on service entries
    //
    TMsvEntry tEntry;
    for( TInt index = count-1; index >= 0; --index )
        {
        iMmsEntry->SetEntryL( selection->At( index ) );
        tEntry = iMmsEntry->Entry();
        if( tEntry.iType != KUidMsvServiceEntry )
            {
            // Take out from selection
            selection->Delete( index );
            }
        }
    count = selection->Count();
    LOG2(_L("- found %d MMS service entries"), count );
    
    //
    // Handle entry depending on number of already existing entries
    //
    TMsvId id = KMsvNullIndexEntryId;
    if( count == 0 ) // no service before
        {
        tEntry.iType = KUidMsvServiceEntry;
        tEntry.iMtm = KUidMsgTypeMultimedia;
        tEntry.iDetails.Set( KMmsDefaultServiceName ); 
        tEntry.SetVisible( EFalse );
        tEntry.SetInPreparation( EFalse );
        iParentEntry->CreateL( tEntry );
        id = tEntry.Id();        
        } 
    else // At least one service found
        {
        for( TInt index = 1; index < count; ++index )
            {
            // More than 1 service found -> delete them
            // The error must be ignored because there is nothing we can do
            // if we can't delete the extra services
#ifdef _DEBUG
            TRAPD( error, iParentEntry->DeleteL( selection->At( index ) ) );
            LOG3(_L("- deletion of service %d returned error %d "), selection->At( index ), error );
#else
            TRAP_IGNORE( iParentEntry->DeleteL( selection->At( index ) ) );
#endif            
            }
        id = selection->At( 0 );
        }
        
    //
    // Return new service entry id
    //
    CleanupStack::PopAndDestroy( selection );
    return id;
    }

// -----------------------------------------------------------------------------
// CreateFolderEntryL
// -----------------------------------------------------------------------------
//
TMsvId CMmsMessageStoreUtils::CreateFolderEntryL( const TDesC& aFolderName )
    {
    LOG(_L("CMmsMessageStoreUtils::CreateFolderEntryL"));

    //
    // Find old MMS entries
    //
    iParentEntry->SetEntryL( KMsvLocalServiceIndexEntryId );
    // Find also invisible entries
    iParentEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
    CMsvEntrySelection* selection = iParentEntry->ChildrenWithTypeL( KUidMsvFolderEntry );
    CleanupStack::PushL( selection ); // ***
    TInt count = selection->Count();
    LOG2(_L("- found %d folder entries under localservice"), count );

    //
    // Concentrate on folders that have name 'aFolderName'
    //
    TMsvEntry tEntry;
    for( TInt index = count-1; index >= 0; --index )
        {
        iMmsEntry->SetEntryL( selection->At( index ) );
        tEntry = iMmsEntry->Entry();
        if( tEntry.iDetails.Compare( aFolderName ) != 0 )
            {
            // Take out from selection
            selection->Delete( index );            
            }
        }
    count = selection->Count();
    LOG2(_L("- found %d folders with matching iDetails"), count );
    
    //
    // Handle entry depending on number of already existing entries
    //
    TMsvId id = KMsvNullIndexEntryId;
    if( count == 0 ) // no folder before
        {
        tEntry.iType = KUidMsvFolderEntry;
        tEntry.iMtm = KUidMsvLocalServiceMtm;
        tEntry.iDetails.Set( aFolderName );
        tEntry.SetVisible( EFalse );
        tEntry.SetInPreparation( EFalse );
        tEntry.iServiceId = KMsvLocalServiceIndexEntryId;
        iParentEntry->CreateL( tEntry );
        id = tEntry.Id();        
        } 
    else // At least one folder found
        {
        for( TInt index = 1; index < count; index++ )
            {
            // More than 1 folder found -> delete them
            // The error must be ignored because there is nothing we can do
            // if we can't delete the extra folders
#ifdef _DEBUG
            TRAPD( error, iParentEntry->DeleteL( selection->At( index ) ) );
            LOG3(_L("- deletion of folder %d returned error %d "), selection->At( index ), error );
#else
            TRAP_IGNORE( iParentEntry->DeleteL( selection->At( index ) ) );
#endif            
            }
        id = selection->At( 0 );
        }
        
    //
    // Return new service entry id
    //
    CleanupStack::PopAndDestroy( selection );
    return id;
    }

// -----------------------------------------------------------------------------
// CreateFolderEntryL
//
// -----------------------------------------------------------------------------
//
TInt CMmsMessageStoreUtils::CreateFolderEntryL(
    TMsvId aParentFolder, const TDesC& aFolderName, TMsvId& aFolderId )
    {
    
    // Check if the folder already exists under parent folder.
    aFolderId = FolderEntryL( aParentFolder, aFolderName );    
    if ( aFolderId != KMsvNullIndexEntryId )
        {
         return KErrAlreadyExists;   
        }
      
    // Create a new folder.
    iMmsEntry->SetEntryL( aParentFolder );

    TMsvEntry entry;
    entry.iType = KUidMsvFolderEntry;
    entry.iMtm = KUidMsvLocalServiceMtm;
    entry.iDetails.Set( aFolderName );
    entry.SetVisible( EFalse );
    entry.SetInPreparation( EFalse );
    entry.iServiceId = KMsvLocalServiceIndexEntryId;
    iMmsEntry->CreateL( entry );    
    aFolderId = entry.Id();    
    return KErrNone;
       
    }

// -----------------------------------------------------------------------------
// FolderEntryL
//
// -----------------------------------------------------------------------------
//
TMsvId CMmsMessageStoreUtils::FolderEntryL( TMsvId aParentFolder, const TDesC& aFolderName )
    {
    
    TMsvId folderId = KMsvNullIndexEntryId;
     
    iMmsEntry->SetEntryL( aParentFolder );

    // Show invisible entries
    iMmsEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByIdReverse, ETrue ) );
    CMsvEntrySelection* selection = iMmsEntry->ChildrenWithTypeL( KUidMsvFolderEntry );
    CleanupStack::PushL( selection );

    // If selection contains folders, check if the folder is found.
    TInt count = selection->Count();
    for ( TInt i = 0; i < count && folderId == KMsvNullIndexEntryId; ++i )
        {
        iMmsEntry->SetEntryL( selection->At( i ) );
        // must be exact match
        if ( iMmsEntry->Entry().iDetails.Compare( aFolderName ) == 0 )
            {
            folderId = selection->At( i );
            }
        }
    CleanupStack::PopAndDestroy( selection );
    
    return folderId;
       
    }
        

// =========================== OTHER EXPORTED FUNCTIONS ========================

//  End of File  
