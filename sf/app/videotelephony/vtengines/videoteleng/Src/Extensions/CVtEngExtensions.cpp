/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Videoteleng API extension configuration
*
*/


// INCLUDE FILES
#include    "CVtEngExtensions.h"
#include    "VtEngPanic.h"
#include    "VtEngUtils.h"

#include    "CVtEngOperation.h"
#include 	"CVtEngMediaHandler.h"
#include    "CVtEngCameraPreferences.h"
#include 	"cvtengrecord2file.h"

#include    <cvtlogger.h>

// LOCAL CONSTANTS AND MACROS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngExtensions::CVtEngExtensions
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngExtensions::CVtEngExtensions(): 
                  iExtensionArray( KExtensionGranularity )
    {
    }

// -----------------------------------------------------------------------------
// CVtEngExtensions::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtEngExtensions::ConstructL()
    {
    __VTPRINTENTER( "Extensions.ConstructL" )
    __VTPRINTEXIT( "Extensions.ConstructL" )
    }

// -----------------------------------------------------------------------------
// CVtEngExtensions::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngExtensions* CVtEngExtensions::NewL()
    {
    CVtEngExtensions* self = new( ELeave ) CVtEngExtensions;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngExtensions::~CVtEngExtensions
// Destructor. Cannot leave.
// -----------------------------------------------------------------------------
//
CVtEngExtensions::~CVtEngExtensions()
    {
    __VTPRINTENTER( "Extensions.~" )
    // Clean and destroy pointer array 
    iExtensionArray.ResetAndDestroy();
    __VTPRINTEXIT( "Extensions.~" )
    }
    
// -----------------------------------------------------------------------------
// CVtEngExtensions::CreateExtensions
// 
// -----------------------------------------------------------------------------
//
CVtEngExtensionBase* CVtEngExtensions::CreateExtensionsL( const TUid& aUid,
	CVtEngMediaHandler& aHandler)
    {
    __VTPRINTENTER( "Extensions.CreateExtensions" )
    
    // init local variables
    TInt error = KErrNotSupported;
    CVtEngExtensionBase* extension = NULL;
    
    // Check if supported UID
    if ( aUid == KVtEngExtensionCameraPreferences  || 
    	aUid == KVtEngExtensionRecord2File )
   		{  		
   		// Create extension	
        extension = aHandler.CreateExtensionsL( aUid );        
        
        // Push it to clean up stack because 
        // it needs to be append in to
        // array and it can be failed
        CleanupStack::PushL( extension );
            
        // append extension in to array             
        error = iExtensionArray.Append(extension);
        if ( error != KErrNone )
            {
            // if append fails
            // clear clean up stack and leave 
            __VTPRINTEXITR( 
            	"Extensions.CreateExtensions iExtension.Append failed %d",
            	 error )
            CleanupStack::PopAndDestroy();
            User::Leave( error );
            }
        // append succesful
        // pop extension from clena up stack and return     
        __VTPRINTEXITR( 
        	"Extensions.CreateExtensions iExtension UID[%d] succesfully Appended",
        	 aUid.iUid)     
        CleanupStack::Pop();
        return extension;    
   		}
   	// not supported UID	
   	// leave with error
   	else
   		{
   		__VTPRINTEXITR( 
   			"Extensions.CreateExtensions Extension NOTSUPPORTED UID%d",
   			 aUid.iUid )
        User::Leave( error );
        return extension;
   		}
   		
    }
       
// -----------------------------------------------------------------------------
// CVtEngExtensions::GetClientInterface
// 
// -----------------------------------------------------------------------------
//
TInt CVtEngExtensions::GetClientInterface( TAny** aInterfacePtr, 
	const TUid& aUid , CVtEngMediaHandler& aHandler)
    {
    __VTPRINTENTER( "Extensions.GetClientInterface" )
   	// if aInterfacePtr pointer is NULL -> panic
   	__ASSERT_ALWAYS( aInterfacePtr,Panic( EVtEngPanicInvalidPointer ) );

	// init local variables
    CVtEngExtensionBase* extension = NULL;
    TInt error = KErrGeneral;
    
    // initialize IF pointer to be NULL
    // if correct extension API IF is found
    // then changed to valid pointer
    *aInterfacePtr = NULL;
    
    // Check if extension is allready in pointer array
    TInt index = LookupExtensions( aUid );
    if ( index != KErrNotFound )
        {
        *aInterfacePtr = iExtensionArray[index]->GetInterface();
        __VTPRINTEXITR( "Extensions.GetClientInterface error [%d]", KErrNone ) 
        return KErrNone;
        }
    // Create extension   
    else
        {
        TRAP( error, extension = CreateExtensionsL( aUid, aHandler ) );
        if ( extension && error == KErrNone )
            {
            *aInterfacePtr = extension->GetInterface();
            error = KErrNone;   
            }  
        __VTPRINTEXITR( "Extensions.GetClientInterface error [%d]", error )
        return error;       
        }
    }
// -----------------------------------------------------------------------------
// CVtEngExtensions::LookupExtensions
// 
// -----------------------------------------------------------------------------
//
TInt CVtEngExtensions::LookupExtensions( const TUid& aUid) const
    {
    __VTPRINTENTER( "Extensions.LookupExtensions" )
    TInt index( iExtensionArray.Count() );
    
    // Loop whole extension array
    while( index-- )
        {
        // If matches in one of arrays UIDs        
        if( aUid == iExtensionArray[index]->Uid() )
            {
            __VTPRINTEXITR( "Extensions.LookupExtensions index [%d]", index) 
            return index;
            }
        }   
    __VTPRINTEXITR("Extensions.LookupExtensions Not Found index [%d]",
    	KErrNotFound)   
    return KErrNotFound; 
    }
    
// -----------------------------------------------------------------------------
// CVtEngExtensions::OfferOperationL
// 
// -----------------------------------------------------------------------------
//
TBool CVtEngExtensions::OfferOperationL( CVtEngOperation* aOp )
    {   
    __VTPRINTENTER( "Extensions.OfferOperationL" )
    
    TInt index( iExtensionArray.Count() );
    	
    // Format cmdComleted as false
    TBool cmdCompleted = EFalse;
    // Offer operation to all extension in array
    while( index-- )
        {
        // If valid extension found
        if ( iExtensionArray[index]->HandleL( aOp ) )
            {
            cmdCompleted = ETrue;
            break;
            }
        }
    __VTPRINTEXITR( "Extensions.OfferOperationL cmdCompleted [%d]",
    	cmdCompleted)  
    return cmdCompleted;
    }    

// -----------------------------------------------------------------------------
// CVtEngExtensions::ValidateCommand
// 
// -----------------------------------------------------------------------------
//        
TBool CVtEngExtensions::ValidateCommand( const TVtEngCommandId aCommandId )
    {
    __VTPRINTENTER( "Extensions.ValidateCommand" )
     TInt index( iExtensionArray.Count() );

    // Format oKToPerform as false
    TBool oKToPerform = EFalse;
    // Try validate command in all extensions in array
    while( index-- )
        {
        // If valid extension found
        if ( iExtensionArray[index]->ValidateCommand( aCommandId ) )
            {
            oKToPerform = ETrue;
            break;
            }
        }
    __VTPRINTEXITR( "Extensions.ValidateCommand oKToPerform [%d]",
    	oKToPerform)
    return oKToPerform;
    }    
    
//  End of File

