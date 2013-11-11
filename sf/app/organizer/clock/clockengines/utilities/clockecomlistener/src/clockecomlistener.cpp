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
* Description:   The source file of the CClockEcomListener class.
*
*/

// System includes
#include <ecom/ecom.h>

// User includes
#include "clockecomlistener.h"
#include "clockecomobserver.h"
#include "clock_debug.h"

// Constants
const TUid KTimeSourceInterfaceUID = { 0x200159A7 };

// ---------------------------------------------------------
// ReleaseOnCleanup
// Used for cleanup of objects on stack if a function leaves.
// ---------------------------------------------------------
//
LOCAL_C void ReleaseOnCleanup( TAny* aObject )
    {
    __PRINTS( "ReleaseOnCleanup - Entry" );
    
    REINTERPRET_CAST( RImplInfoPtrArray*, ( aObject ) )->ResetAndDestroy();
    REINTERPRET_CAST( RImplInfoPtrArray*, ( aObject ) )->Close();
    
    __PRINTS( "ReleaseOnCleanup - Exit" );
    }

// ---------------------------------------------------------
// CClockEComListener::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClockEComListener* CClockEComListener::NewL( MClockEComObserver& aClockEComObserver )
    {
    __PRINTS( "CClockEComListener::NewL - Entry" );
    
    CClockEComListener* self = new ( ELeave ) CClockEComListener( aClockEComObserver );
    CleanupStack::PushL( self );
    
    self->ConstructL();
    
    CleanupStack::Pop();
    
    __PRINTS( "CClockEComListener::NewL - Exit" );
    
    return self;
    }
    
// ---------------------------------------------------------
// CClockEComListener::~CClockEComListener
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClockEComListener::~CClockEComListener()
    {
    __PRINTS( "CClockEComListener::~CClockEComListener - Entry" );
    
    // Cancel any outstanding requests.
    Stop();
    if( iEComSession )
        {
        iEComSession->Close();
        }
    
    iImplArray.ResetAndDestroy();
    iImplArray.Close();
    
    __PRINTS( "CClockEComListener::~CClockEComListener - Exit" );
    }
 
// ---------------------------------------------------------
// CClockEComListener::NotifyEComObserverL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockEComListener::NotifyEComObserverL( CImplementationInformation* aImplInfo, TBool aInstalled )
    {
    __PRINTS( "CClockEComListener::NotifyEComObserverL - Entry" );
    
    // Notify the observer about changes in the registry.
    iClockEComObserver.NotifyPluginInstallUninstallL( *aImplInfo, aInstalled );
    
    __PRINTS( "CClockEComListener::NotifyEComObserverL - Exit" );
    }

// ---------------------------------------------------------
// CClockEComListener::RunL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockEComListener::RunL()
    {
    __PRINTS( "CClockEComListener::RunL - Entry" );
    
    // Check whether a plugin was installed or removed.
    CheckForEcomPluginInstallUninstallL();
    
    //Restart the active Object
    Start();
    
    __PRINTS( "CClockEComListener::RunL - Exit" );
    }

// ---------------------------------------------------------
// CClockEComListener::DoCancel
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockEComListener::DoCancel()
    {
    __PRINTS( "CClockEComListener::DoCancel - Entry" );
    
    // Cancel any outstanding requests.
    iEComSession->CancelNotifyOnChange( iStatus );
    
    __PRINTS( "CClockEComListener::DoCancel - Exit" );
    }

// ---------------------------------------------------------
// CClockEComListener::CClockEComListener
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockEComListener::CClockEComListener( MClockEComObserver& aClockEComObserver ) : CActive( CActive::EPriorityStandard ), iClockEComObserver( aClockEComObserver )
    {
    __PRINTS( "CClockEComListener::CClockEComListener - Entry" );
        
    // No implementation yet.
    
    __PRINTS( "CClockEComListener::CClockEComListener - Exit" );
    }

// ---------------------------------------------------------
// CClockEComListener::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockEComListener::ConstructL()
    {
    __PRINTS( "CClockEComListener::ConstructL - Entry" );
    
    // Connect to the ECOM session server.
    iEComSession = &REComSession::OpenL();
    
    // Get the list of all plugins which are present.
    TRAP_IGNORE( REComSession::ListImplementationsL( KTimeSourceInterfaceUID, iImplArray ) );
    
    // Queue the active object in the scheduler
    CActiveScheduler::Add( this );
        
    // Start the listener.
    Start();
    
    __PRINTS( "CClockEComListener::ConstructL - Exit" );
    }

// ---------------------------------------------------------
// CClockEComListener::Start
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockEComListener::Start()
    {
    __PRINTS( "CClockEComListener::Start - Entry" );
    
    // Start the active object and listen for changes in the ECOM registry.
    if( iEComSession )
        {
        iEComSession->NotifyOnChange( iStatus );
        }
    SetActive();
    
    __PRINTS( "CClockEComListener::Start - Exit" );
    }

// ---------------------------------------------------------
// CClockEComListener::Stop
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockEComListener::Stop()
    {
    __PRINTS( "CClockEComListener::Stop - Entry" );
    
    Cancel();
    
    __PRINTS( "CClockEComListener::Stop - Exit" );
    }

// ---------------------------------------------------------
// CClockEComListener::CheckForEcomPluginInstallUninstallL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockEComListener::CheckForEcomPluginInstallUninstallL()
    {
    __PRINTS( "CClockEComListener::CheckForEcomPluginInstallUninstallL - Entry" );
    
    // Array to return all implementations in an interface
    RImplInfoPtrArray plugInArray;

    // Get the list of all implementations.
    TRAPD( errorVal, REComSession::ListImplementationsL( KTimeSourceInterfaceUID, plugInArray ) );
    CleanupStack::PushL( TCleanupItem( ReleaseOnCleanup, &plugInArray ) );

    TBool implAdded( EFalse );
    TBool implRemoved( EFalse );

    // If an implementation is not present in the iPluginUidList then its added. 
    for( TInt index( plugInArray.Count() - 1 ); index >= 0; --index )  
        {
        TUid uId( plugInArray[ index ]->ImplementationUid() );
        if( IsPluginInstalled( uId ) )
            {
            // Add the implementation which is not present in the plugin array.
            // Notify the observer about the plugin addition.
            __PRINT( "UID: %x", uId );
            __PRINT( "Version: %d", plugInArray[ index ]->Version() );
            
            NotifyEComObserverL( plugInArray[ index ], ETrue );
            
            implAdded = ETrue;
			
			break;
            }
        }

    if( !implAdded )
        {
        // If an implementation is not present in present in the plugInArray then its removed. 
        for( TInt index( iImplArray.Count() - 1 ); index >= 0; --index )
            {
            TUid uId( iImplArray[ index ]->ImplementationUid() );
            if( IsPluginUninstalled( uId, plugInArray ) )
                {
                // Remove the implementation in the plugin array.
                // Notify the observer about the plugin removal.
                __PRINT( "UID: %x", uId );
                __PRINT( "Version: %d", iImplArray[ index ]->Version() );
                
                NotifyEComObserverL( iImplArray[ index ], EFalse );
                
                implRemoved = ETrue;
				
				break;
                }
            }
        }
    
    if( !implAdded && !implRemoved )
        {
        // Check if a plugin was upgraded by installation OR downgraded by uninstallation.
        CheckForPluginUpgradeDowngradeL( plugInArray );
        }
    
    // Reset the array and refresh the plugin list.
    iImplArray.ResetAndDestroy();
    iImplArray.Close();
    
    TRAP( errorVal, REComSession::ListImplementationsL( KTimeSourceInterfaceUID, iImplArray ) );

    // Cleanup.
    CleanupStack::Pop( &plugInArray );
    plugInArray.ResetAndDestroy();
    plugInArray.Close();
    
    __PRINTS( "CClockEComListener::CheckForEcomPluginInstallUninstallL - Exit" );
    }

// ---------------------------------------------------------
// CClockEComListener::IsPluginInstalled
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClockEComListener::IsPluginInstalled( const TUid aUid )
    {
    __PRINTS( "CClockEComListener::IsPluginInstalled - Entry" );
    
    for( TInt index( iImplArray.Count() - 1 ); index >= 0; --index )
        {
        if( aUid == ( iImplArray[ index ] )->ImplementationUid() )
            {
            __PRINTS( "CClockEComListener::IsPluginInstalled - Exit" );
            
            return EFalse;
            }
        }
    __PRINT( "Plugin added: %x", aUid );
    
    __PRINTS( "CClockEComListener::IsPluginInstalled - Exit" );
    
    return ETrue;
    }

// ---------------------------------------------------------
// CClockEComListener::IsPluginUninstalled
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClockEComListener::IsPluginUninstalled( const TUid aUid, RImplInfoPtrArray aPlugInArray )
    {
    __PRINTS( "CClockEComListener::IsPluginUninstalled - Entry" );
    
    for( TInt index( aPlugInArray.Count() - 1 ); index >= 0; --index )
        {
        if( aUid == aPlugInArray[ index ]->ImplementationUid() )
            {
            __PRINTS( "CClockEComListener::IsPluginUninstalled - Exit" );
            
            return EFalse;
            }
        }
    __PRINT( "Plugin removed: %x", aUid );
    
    __PRINTS( "CClockEComListener::IsPluginUninstalled - Exit" );
    
    return ETrue;
    }

// ---------------------------------------------------------
// CClockEComListener::CheckForPluginUpgradeDowngradeL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockEComListener::CheckForPluginUpgradeDowngradeL( const RImplInfoPtrArray& aPlugInArray )
    {
    __PRINTS( "CClockEComListener::CheckForPluginUpgradeDowngradeL - Entry" );
    
    // Check for each plugin in the array if the version matches with the plugin we have
    for( TInt outerIndex( aPlugInArray.Count() - 1 ); outerIndex >= 0; --outerIndex )  
            {
            TUid uId( aPlugInArray[ outerIndex ]->ImplementationUid() );
            for( TInt innerIndex( iImplArray.Count() - 1 ); innerIndex >= 0; --innerIndex )
                {
				if( uId == iImplArray[ innerIndex ]->ImplementationUid() )
					{
	                if( aPlugInArray[ outerIndex ]->Version() != iImplArray[ innerIndex ]->Version() )
	                    {
	                    __PRINTS( "Plugin upgraded / degraded" );
	                    __PRINT( "UID: %x", uId );
	                    __PRINT( "Version: %d", aPlugInArray[ outerIndex ]->Version() );
	                    
	                    // If control reaches here, it means we either have an upgrade or downgrade.
	                    NotifyEComObserverL( aPlugInArray[ outerIndex ], ETrue );
						
						break;
	                    }
					}
                }
            }
    
    __PRINTS( "CClockEComListener::CheckForPluginUpgradeDowngradeL - Exit" );
    }

// End of file
