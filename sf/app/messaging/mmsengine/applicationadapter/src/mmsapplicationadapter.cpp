/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: mmsapplicationadapter implementation
*
*/




// INCLUDE FILES
#include    "mmsapplicationadapter.h"
#include    "mmsheaders.h"
#include    "mmsmsventry.h" // for the TMmsMsvEntry
#include    "mmssettings.h"
#include	"mmsconninit.h"
#include 	"mmsregisteredapplications.h"
#include	<msvids.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  


// CONSTANTS


// MACROS

// LOCAL CONSTANTS AND MACROS


// MODULE DATA STRUCTURES


// LOCAL FUNCTION PROTOTYPES


// FORWARD DECLARATIONS



// ============================= LOCAL FUNCTIONS ===============================



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmsApplicationAdapter::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CMmsApplicationAdapter* CMmsApplicationAdapter::NewL()
    {
    CMmsApplicationAdapter* self = new ( ELeave ) CMmsApplicationAdapter;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CMmsApplicationAdapter::~CMmsApplicationAdapter
// -----------------------------------------------------------------------------
//    
// Destructor
CMmsApplicationAdapter::~CMmsApplicationAdapter()
    {
    delete iMmsHeaders;
    delete iMmsSettings;
    delete iRegAppIds;
    delete iClientEntry;
    delete iSession;
    }
    
// -----------------------------------------------------------------------------
// CMmsApplicationAdapter::RegisterL
// -----------------------------------------------------------------------------
//

EXPORT_C TInt CMmsApplicationAdapter::RegisterL(
    const TDesC& aApplicationId, TMsvId& aFolderId )
    {
    return RegisterWithNameL( aApplicationId, TPtrC(), aFolderId );
    }

// -----------------------------------------------------------------------------
// CMmsApplicationAdapter::UnregisterL
// -----------------------------------------------------------------------------
//

EXPORT_C TInt CMmsApplicationAdapter::UnregisterL(
    const TDesC& aApplicationId )
    {
        
    TInt error = KErrNone;     
    if ( RegisteredL( aApplicationId ) )
	    {
	    error = iRegAppIds->UnregisterL( aApplicationId );
	    }
	    
	TMsvId applicationFolder = iMmsSettings->ApplicationFolder(); 
    TMsvId folderId = iMmsSettings->FolderEntryL( *iSession, applicationFolder,
        aApplicationId );
            
	// Despite of the error, delete the folder anyway.    
	if (  folderId != KMsvNullIndexEntryId )
	    {
	    error = DeleteL( folderId );	
	    }		    
		
	return error;	
    }

// -----------------------------------------------------------------------------
// CMmsApplicationAdapter::RegisteredL
// -----------------------------------------------------------------------------
//    
    
EXPORT_C TBool CMmsApplicationAdapter::RegisteredL( const TDesC& aApplicationId )
	{
	iRegAppIds->LoadRegisteredApplicationsL();
	return iRegAppIds->RegisteredL( aApplicationId );	
    }

// -----------------------------------------------------------------------------
// CMmsApplicationAdapter::FolderIdL
// -----------------------------------------------------------------------------
//
EXPORT_C TMsvId CMmsApplicationAdapter::FolderIdL( const TDesC& aApplicationId )
    {
    // check if the appid is registered
    TBool registered = RegisteredL( aApplicationId );
    
    // check if folder for the appid exists
    TMsvId applicationFolder = iMmsSettings->ApplicationFolder(); 
    TMsvId folderId = iMmsSettings->FolderEntryL( *iSession, applicationFolder,
        aApplicationId );
 
    if ( registered )
	    {
	    if ( folderId != KMsvNullIndexEntryId )
		    {
		    return folderId;		    	
		    }
		 else
			 {
			 User::LeaveIfError( iMmsSettings->CreateFolderEntryL( *iSession, applicationFolder, 
        	 aApplicationId, folderId) );
        	 return folderId;	
			 }   
	    }
	else // application id is not registered
		{
		if ( folderId != KMsvNullIndexEntryId )
			{
		    User::LeaveIfError( DeleteL( folderId ) ); 
			}
		return KMsvNullIndexEntryId;			
		}    
               
    }
    
// -----------------------------------------------------------------------------
// CMmsApplicationAdapter::ApplicationIdL
// -----------------------------------------------------------------------------
//

EXPORT_C TPtrC16 CMmsApplicationAdapter::ApplicationIdL(
    TMsvId aMmId )
    {
    iClientEntry->SetEntryL( aMmId );
    CMsvStore* store = iClientEntry->ReadStoreL();
    CleanupStack::PushL( store );
    iMmsHeaders->RestoreL( *store );
    
    TPtrC16 applicId = iMmsHeaders->ApplicId();
    CleanupStack::PopAndDestroy( store );
    return applicId;
    } 
    
// -----------------------------------------------------------------------------
// CMmsApplicationAdapter::SetApplicationIdL
// -----------------------------------------------------------------------------
//

EXPORT_C void CMmsApplicationAdapter::SetApplicationIdL(
    TMsvId aMmId,
    const TDesC& aApplicationId )
    {
    iClientEntry->SetEntryL( aMmId );
    
    CMsvStore* store = iClientEntry->EditStoreL();
    CleanupStack::PushL( store );
    iMmsHeaders->RestoreL( *store );

    iMmsHeaders->SetApplicIdL( aApplicationId );

    // The headers changed, save them. 
    iMmsHeaders->StoreL( *store );
    store->CommitL();
    CleanupStack::PopAndDestroy( store );
    
    }
    
// -----------------------------------------------------------------------------
// CMmsApplicationAdapter::ReplyToApplicationIdL
// -----------------------------------------------------------------------------
//

EXPORT_C TPtrC16 CMmsApplicationAdapter::ReplyToApplicationIdL(
    TMsvId aMmId )
    {
    iClientEntry->SetEntryL( aMmId );
    
    CMsvStore* store = iClientEntry->ReadStoreL();
    CleanupStack::PushL( store );
    iMmsHeaders->RestoreL( *store );
    TPtrC16 replyToApplicId = iMmsHeaders->ReplyApplicId();
    CleanupStack::PopAndDestroy( store );
    
    return replyToApplicId;
    } 
    
// -----------------------------------------------------------------------------
// CMmsApplicationAdapter::SetReplyToApplicationIdL
// -----------------------------------------------------------------------------
//

EXPORT_C void CMmsApplicationAdapter::SetReplyToApplicationIdL(
    TMsvId aMmId,
    const TDesC& aApplicationId )
    {
    iClientEntry->SetEntryL( aMmId );
    
    CMsvStore* store = iClientEntry->EditStoreL();
    CleanupStack::PushL( store );
    iMmsHeaders->RestoreL( *store );

    iMmsHeaders->SetReplyApplicIdL( aApplicationId );

    // The headers changed, save them. 
    iMmsHeaders->StoreL( *store );
    store->CommitL();
    CleanupStack::PopAndDestroy( store );
    
    }
    
// -----------------------------------------------------------------------------
// CMmsApplicationAdapter::DeleteL
// -----------------------------------------------------------------------------
//

EXPORT_C TInt CMmsApplicationAdapter::DeleteL(
    TMsvId aMmId )
    {    
    TInt error = KErrNone;
    TRAP ( error,
    {
    iClientEntry->SetEntryL( aMmId );
    TMsvEntry tEntry = iClientEntry->Entry();
    iClientEntry->SetEntryL( tEntry.Parent() );
    iClientEntry->DeleteL( aMmId );
    } )
    
    if ( error == KErrNotFound )
        {
        error = KErrNone;
        }
    return error;    
    } 
        
// -----------------------------------------------------------------------------
// CMmsApplicationAdapter::MmscUrlL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMmsApplicationAdapter::MmscUrlL( HBufC*& aServerUri )
	{
	TUint32 accessPointLink = 0;
    TUint32 accessPoint = 0;
    TBool proxyUsed = EFalse;
    HBufC8* proxyAddress = NULL;
    
    iMmsSettings->LoadSettingsL();
    
    TInt accessPointCount = iMmsSettings->AccessPointCount();
   
    if ( accessPointCount > 0 )
        {
        accessPointLink = iMmsSettings->AccessPoint( 0 );
        }
    else
	    {
	    return KErrNotFound;	
	    } 
	    
    CMmsConnectionInitiator::GetParametersL( accessPointLink, accessPoint, 
	aServerUri, proxyUsed, proxyAddress); 
	
	delete proxyAddress;
	proxyAddress = NULL;
	
	if ( accessPoint == 0 )
		{
		// something is gone wrong.
		return KErrGeneral;	
		}	           
	return KErrNone;	
	}
	
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
EXPORT_C const CDesCArray& CMmsApplicationAdapter::RegisteredApplicationList() const
    {
    // Leaves only if runs out of memory
    TRAP_IGNORE( iRegAppIds->LoadRegisteredApplicationsL() ); 
    // Return what we got.
    // The list always exists, but may not be up to date if we run out of memory
	return iRegAppIds->RegisteredApplicationList();
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMmsApplicationAdapter::RegisterWithNameL(
    const TDesC& aApplicationId,
    const TDesC& aUserFriendlyName,
    TMsvId& aFolderId )
    {
    aFolderId = KMsvNullIndexEntryId;
    TInt error = KErrNone;
	iRegAppIds->LoadRegisteredApplicationsL();
	
	// iRegAppIds checks if the application id is already registered.
	// User friendly name can be added afterwards even if the id
	// is already registered, so we let CMmsRegisteredApplications class
	// do the checking.
	// CMmsRegisteredApplications also checks if the length of the
	// strings is too long to fit into registry. 
    error = iRegAppIds->RegisterL( aApplicationId, aUserFriendlyName );	    
	    
	if ( error == KErrNone || error == KErrAlreadyExists )
		{
		TMsvId applicationFolder = iMmsSettings->ApplicationFolder();
		error =  iMmsSettings->CreateFolderEntryL( *iSession, applicationFolder, 
    	aApplicationId, aFolderId);	
    	if ( error == KErrAlreadyExists )
	    	{
	    	error = KErrNone;	    		
	    	}
		}
		   
	return error;	    
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CMmsApplicationAdapter::UserFriendlyName( const TDesC& aApplicationId )
    {
    // Leaves only if out of memory
    TRAPD( error, iRegAppIds->LoadRegisteredApplicationsL() );
    if ( error == KErrNone )
        {
    	return iRegAppIds->UserFriendlyName( aApplicationId );	
        }
    else
        {
        // if out of memory, no user friendly name
        return TPtrC();
        }
    }
    
// -----------------------------------------------------------------------------
// CMmsApplicationAdapter::HandleSessionEventL
// -----------------------------------------------------------------------------
//
void CMmsApplicationAdapter::HandleSessionEventL( TMsvSessionEvent  /* aEvent */, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
     
    }  
    
// -----------------------------------------------------------------------------
// CMmsApplicationAdapter::CMmsApplicationAdapter
// -----------------------------------------------------------------------------
//
CMmsApplicationAdapter::CMmsApplicationAdapter()
    {     
    }
    
    
// -----------------------------------------------------------------------------
// CMmsApplicationAdapter::ConstructL
// -----------------------------------------------------------------------------
//
void CMmsApplicationAdapter::ConstructL()
    {
          
    iSession = CMsvSession::OpenSyncL( *this );
    
    iClientEntry = iSession->GetEntryL( KMsvRootIndexEntryId );
   
    // First loading settings
    iMmsSettings = CMmsSettings::NewL(); 
    iMmsSettings->LoadSettingsL();   
    
    iRegAppIds = CMmsRegisteredApplications::NewL();
        
    iMmsHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
    
    }
    


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
