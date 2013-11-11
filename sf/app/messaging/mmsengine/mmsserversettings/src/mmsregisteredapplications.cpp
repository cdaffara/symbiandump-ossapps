/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  mmsregisteredapplications implementation
*
*/


   
// INCLUDE FILES   
#include <centralrepository.h>
#include <msvids.h>

// USERINCLUDE FILES
#include "mmsregisteredapplications.h"
#include "mmssettings.h"
#include "mmsdebuglogging.h"
#include "MmsEngineDomainCRKeys.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TInt KMmsApplicationIdArrayGranularity = 4;
const TInt KMmsApplicationNameDefaultLength = 64; // a reasonable guess for the length

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ============================== LOCAL FUNCTIONS ==============================

// ============================== MEMBER FUNCTIONS =============================

// -----------------------------------------------------------------------------
// CMmsRegisteredApplications::NewL
//
// -----------------------------------------------------------------------------
//
EXPORT_C CMmsRegisteredApplications* CMmsRegisteredApplications::NewL()
    {
    CMmsRegisteredApplications* self = new(ELeave) CMmsRegisteredApplications();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMmsRegisteredApplications::CMmsRegisteredApplications
//
// -----------------------------------------------------------------------------
//
CMmsRegisteredApplications::CMmsRegisteredApplications()
    {
    }

// -----------------------------------------------------------------------------
// CMmsRegisteredApplications::~CMmsRegisteredApplications
//
// -----------------------------------------------------------------------------
//
CMmsRegisteredApplications::~CMmsRegisteredApplications()
    {
    LOG(_L("~CMmsRegisteredApplications"));
    delete iUserFriendlyName;
    delete iRepository;
    delete iAppIdArray;  
    }

// -----------------------------------------------------------------------------
// CMmsRegisteredApplications::ConstructL
//
// -----------------------------------------------------------------------------
//
void CMmsRegisteredApplications::ConstructL()
    {
   
    iAppIdArray = new ( ELeave )CDesCArrayFlat( KMmsApplicationIdArrayGranularity );

    iRepository = CRepository::NewL( KCRUidMMSApplicationRegistrations ); 
    // The length may change later if the names are long.
    iUserFriendlyName = HBufC::NewL( KMmsApplicationNameDefaultLength );
    }

// -----------------------------------------------------------------------------
// CMmsRegisteredApplications::LoadRegisteredApplicationsL
//
// -----------------------------------------------------------------------------
//

EXPORT_C void CMmsRegisteredApplications::LoadRegisteredApplicationsL()
    {
    LOG(_L("CMmsRegisteredApplications::LoadRegisteredApplicationsL"));
    iAppIdArray->Reset();
    TInt count = 0;
    TInt error = iRepository->Get( KMmsEngineNbrOfRegisteredApplications, count );
    if ( error == KErrNotFound )
        {
        iRepository->Set( KMmsEngineNbrOfRegisteredApplications, 0 );   
        }
    
    // KMaxUnicodeStringLength is the maximum length of string in repository -
    // The strings will always fit into the bugger.
    HBufC* buffer = HBufC::NewL( NCentralRepositoryConstants::KMaxUnicodeStringLength );
    CleanupStack::PushL( buffer );
    TPtr ptr = buffer->Des();
    // We use the same buffer for reading everything - AppendL will always allocate
    // a new buffer anyway. No need to keep allocating a new read buffer every time
    for( TInt i = 0; i < count; i++ )
        {
        error = iRepository->Get( KMmsEngineRegisteredApplicationsArrayBase + i, ptr ) ;
        if ( error == KErrNone )
            { 
            iAppIdArray->AppendL( ptr );
            }
        else
            {
            LOG3(_L("- Loading registered appid array item %d returned error %d"), i, error );
            }
        }   
    CleanupStack::PopAndDestroy( buffer );
    buffer = NULL;
    
    }


// -----------------------------------------------------------------------------
// CMmsRegisteredApplications::NumberOfRegisteredApplicationsL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMmsRegisteredApplications::NumberOfRegisteredApplicationsL() const
    {
    return iAppIdArray->MdcaCount();
    }
    
// -----------------------------------------------------------------------------
// CMmsRegisteredApplications::RegisteredL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMmsRegisteredApplications::RegisteredL( const TDesC& aApplicationId )
    {
    TInt index = -1;
    return Registered( aApplicationId, index );
    }  


    
// -----------------------------------------------------------------------------
// CMmsRegisteredApplications::RegisterL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMmsRegisteredApplications::RegisterL( const TDesC& aApplicationId )
    {
    return RegisterL( aApplicationId, TPtrC() );
    }

// -----------------------------------------------------------------------------
// CMmsRegisteredApplications::UnregisterL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMmsRegisteredApplications::UnregisterL( const TDesC& aApplicationId )
    {
    TInt index = 0;
    if ( !Registered( aApplicationId, index ) )
        {
        return KErrNone;            
        }
    LOG2(_L("CMmsRegisteredApplications::Unregister: array[%d]"), index );
    
    TInt error = KErrNone;
    
    // if item is not last one in the array. move the last item here.
    TInt lastIndex = iAppIdArray->MdcaCount() - 1;
    if ( index != lastIndex )
        {
        HBufC* buffer = HBufC::NewL( NCentralRepositoryConstants::KMaxUnicodeStringLength );
        CleanupStack::PushL( buffer );
        TPtr ptr = buffer->Des();
    
        error = iRepository->Get(
            KMmsEngineRegisteredApplicationsArrayBase + lastIndex, ptr );
        if ( error == KErrNone )
            {
            error = iRepository->Set(
                KMmsEngineRegisteredApplicationsArrayBase + index, ptr );
            }
        // local array has to follow the same order
        if ( error == KErrNone )
            {
            // delete the old user friendly name.
            // The new one will replace it in a moment, but we make sure 
            // there is no incorrect name left in place in case InsertL
            // leaves because of lack of memory.
            iRepository->Delete( KMmsEngineApplicationNameArrayBase + index );
            // array index should be safe
            iAppIdArray->InsertL( index, ptr );
            iAppIdArray->Delete( index + 1 ); 
            }
        if ( error == KErrNone )
            {
            error = iRepository->Get( KMmsEngineApplicationNameArrayBase + lastIndex, ptr );
            if ( error == KErrNone && ptr.Length() > 0 )
                {
                error = iRepository->Set(
                    KMmsEngineApplicationNameArrayBase + index, ptr );
                }
            // The setting of the name must not prevent final cleanup    
            error = KErrNone;    
            }
            
        CleanupStack::PopAndDestroy( buffer );
        buffer = NULL;
        }
    // delete the last item from the arrays
    if ( error == KErrNone )
        {
        error = iRepository->Delete( KMmsEngineRegisteredApplicationsArrayBase + lastIndex );
        if ( error == KErrNone )
            {
            iAppIdArray->Delete( lastIndex );
            iAppIdArray->Compress();
            SaveCount();        
            }
        // The user friendly name of the last item is deleted in any case
        // We are trying to make sure there are no friendly names pointing
        // to incorrect application ids left over.
        // The name can be fixed by trying to register the application again
        // Even if the application is already registered, the user friedly name
        // can be changed by registering the same application id again. 
        iRepository->Delete( KMmsEngineApplicationNameArrayBase + lastIndex );
        }
    return error;       
  
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
EXPORT_C const CDesCArray& CMmsRegisteredApplications::RegisteredApplicationList() const
    {
    return *iAppIdArray; 
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMmsRegisteredApplications::RegisterL(
    const TDesC& aApplicationId,
    const TDesC& aUserFriendlyName )
    {
    TInt error = KErrNone;
    TInt index = -1;
    if ( aApplicationId.Length() > NCentralRepositoryConstants::KMaxUnicodeStringLength )
        {
        // The application id is too long, it cannot be registerd
        return KErrArgument;
        }
    if ( aUserFriendlyName.Length() > NCentralRepositoryConstants::KMaxUnicodeStringLength )
        {
        return KErrArgument;
        }
        
    if ( !Registered( aApplicationId, index ) )
        {
        // add new app id to last item in the array.
        index = iAppIdArray->MdcaCount();
        error = iRepository->Create(
            KMmsEngineRegisteredApplicationsArrayBase + index, aApplicationId );
        if ( error == KErrNone || error == KErrAlreadyExists )
            {
            // We should not get "already exists" unless someone has 
            // edited the registry behind our back...
            error = KErrNone;
            // The index now points to the newly created application id
            iAppIdArray->AppendL( aApplicationId );
            // update count in cenrep.
            SaveCount();
            }
        }
        
    // If index == -1 at this point, the registeration has failed.    
    if ( aUserFriendlyName.Length() > 0 && error == KErrNone && index != -1 )
        {
        error = iRepository->Set( KMmsEngineApplicationNameArrayBase + index,
            aUserFriendlyName );
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
EXPORT_C TPtrC CMmsRegisteredApplications::UserFriendlyName( const TDesC& aApplicationId )
    {
    // This is a "best effort" function. 
    // If memory runs out or some other error occurs, an empty string is returned
    TInt index = -1;
    if ( !Registered( aApplicationId, index ) )
        {
        return TPtrC();
        }
    
    TInt error = KErrNone;
    if ( !iUserFriendlyName )
        {
        // iUserFriendlyName may be NULL only if we have ran out of memory earlier
        TRAP( error, iUserFriendlyName = HBufC::NewL( KMmsApplicationNameDefaultLength ) );
        }
    if ( error != KErrNone )
        {
        // If no memory, return empty string
        return TPtrC();
        }
        
    TPtr ptr = iUserFriendlyName->Des();
    TInt actualLength = 0;

    error = iRepository->Get( KMmsEngineApplicationNameArrayBase + index, ptr, actualLength );
    if ( error == KErrOverflow )
        {
        delete iUserFriendlyName;
        iUserFriendlyName = NULL;
        TRAP( error,
            {
            iUserFriendlyName = HBufC::NewL( actualLength );
            ptr.Set( iUserFriendlyName->Des() ); 
            error = iRepository->Get( KMmsEngineApplicationNameArrayBase + index, ptr );
            });
        }
    if ( error == KErrNone )
        {
        return iUserFriendlyName->Des();
        }
    return TPtrC();
        
    }

    
// -----------------------------------------------------------------------------
// CMmsRegisteredApplications::Registered
// -----------------------------------------------------------------------------
//
TBool CMmsRegisteredApplications::Registered( const TDesC& aApplicationId, TInt& aIndex )
    {
    aIndex = -1; 
    TInt count = iAppIdArray->MdcaCount();
    for( TInt i = 0; i < count && aIndex == -1; ++i )
        {
        TPtrC16 ptr = iAppIdArray->MdcaPoint( i );
        if ( ptr.Compare( aApplicationId ) == 0 )
            {
            aIndex = i; 
            }
        }
    return ( aIndex != -1 );        
    } 

// -----------------------------------------------------------------------------
// CMmsRegisteredApplications::SaveCount
// -----------------------------------------------------------------------------
//
TInt CMmsRegisteredApplications::SaveCount()
    {
    TInt count = iAppIdArray->MdcaCount();
    // Set also creates a new key if it does not exist
    TInt error = iRepository->Set( KMmsEngineNbrOfRegisteredApplications, count );
    if( error != KErrNone )
        {
        LOG3( _L("-ERROR saving appid count %d: %d"), KMmsEngineNbrOfRegisteredApplications, error );
        }
    return error;   
    }
    
    
// =========================== OTHER EXPORTED FUNCTIONS ========================

//  End of File  
