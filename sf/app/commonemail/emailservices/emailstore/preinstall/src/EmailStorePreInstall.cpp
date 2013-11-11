/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: PreInstall helper class implementation.
*
*/



#include <e32const.h>
#include "EmailStorePreInstall.h"
#include "emailstoreuids.hrh"
#include "emailstorepskeys.h"

// Timeout for process exit.
const TInt KProcessTimeout              = 5; //5 seconds

/**
 * Information about an executable file.
 */
class TImsServerInfo
    {
public:
    /** Server name. */
    TFileName iServerName;
    
    /** Server SecureID, usually UID3. */
    TUint32 iServerSid;
    };

/**
 * List of servers to be stopped, in order.
 * Usually, the one with least dependencies is the last.
 */
const TImsServerInfo serverList[] =
    {
    { _L("MESSAGESTOREEXE*"), KUidMessageStoreExe },
    { _L(""), 0 }  // end of array, DO NOT remove.
    };


/**********************************
 * Method: NewL
 **********************************/
CEmailStorePreInstall* CEmailStorePreInstall::NewL()
    {
    CEmailStorePreInstall* self = CEmailStorePreInstall::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

/**********************************
 * Method: NewL
 **********************************/
CEmailStorePreInstall* CEmailStorePreInstall::NewLC()
    {
    CEmailStorePreInstall* self = new(ELeave) CEmailStorePreInstall();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

/**********************************
 * Method: Constructor
 **********************************/
CEmailStorePreInstall::CEmailStorePreInstall()
  : CTimer(EPriorityHigh), iServerIndex(0), iState( EInit )
    {
    __LOG_CONSTRUCT( "IMS", "CEmailStorePreInstall" )
    }


/**********************************
 * Method: ConstructL
 **********************************/
void CEmailStorePreInstall::ConstructL()
    {
    __LOG_ENTER("ConstructL")
    
    // Construct base
    CTimer::ConstructL();
    
    // Define upgrade property.        
    // Define security rights
    TSecurityPolicy readPolicy( ECapabilityReadDeviceData );
    TSecurityPolicy writePolicy( ECapabilityWriteDeviceData );
    TInt err = RProperty::Define( KEmailStoreUpgradePSCategory, 
                                  KProperty_EmailStore_Upgrade, 
                                  RProperty::EInt, 
                                  readPolicy, 
                                  writePolicy );
    
    if ( KErrNone != err && KErrAlreadyExists != err )
        {
        User::Leave( err );
        }

    // We are an AO
    CActiveScheduler::Add(this);

    // Need timeout for closing a process
    iTimeoutTimer = CProcessTimer::NewL(*this);
    
    // Start right away
    CTimer::After( 0 );
    __LOG_EXIT
    }

/**********************************
 * Method: Destructor
 **********************************/
CEmailStorePreInstall::~CEmailStorePreInstall()
    {
    __LOG_ENTER( "Destructor" )
    __LOG_WRITE_INFO( "EmailStore PreInstall.exe is exiting..." )
    
    // Stop AO activity
    Cancel();
    
    // If any logon on process
    delete iTimeoutTimer;
    iProcess.Close();
    
    // Delete upgrade property.
    RProperty::Delete( KEmailStoreUpgradePSCategory, KProperty_EmailStore_Upgrade );
    
    __LOG_EXIT
    __LOG_DESTRUCT
    }
    	
/**********************************
 * Method: RunL
 **********************************/
void CEmailStorePreInstall::RunL()
    {
    __LOG_ENTER("RunL")
    __LOG_WRITE_FORMAT1_INFO( "iStatus=%d", iStatus.Int() )
    __LOG_WRITE_FORMAT1_INFO( "iState=%d", iState )

    if ( KErrNone == iStatus.Int() )
    {
        switch( iState )
        {
            case EInit:
                {    
                iServerIndex = 0;
                iState = EStopServers;
                After( 0 );
                }
                break;
                
            case EStopServers:
                {
                StopNextServerL();
                }
                break;
            
            case EWaitForProcess:
                {                
                ++iServerIndex;
                iState = EStopServers;
                __LOG_WRITE_INFO( "Stoping Timeout Timer" );
                iTimeoutTimer->Cancel();
                StopNextServerL();
                }
                break;
                
            case EDone:
                {
                // Clear upgrade property.
                RProperty::Set( KEmailStoreUpgradePSCategory, 
                                KProperty_EmailStore_Upgrade, 
                                0 );
                            
                __LOG_WRITE_INFO( "PreInstall done, Calling CActiveScheduler::Stop()" )
                CActiveScheduler::Stop();
                }
                break;
        }
    }
    else
        {
        if ( EWaitForProcess == iState && KErrCancel != iStatus.Int() )
                {
                ++iServerIndex;
                iState = EStopServers;
                __LOG_WRITE_INFO( "Stoping Timeout Timer" );
                iTimeoutTimer->Cancel();                          
                StopNextServerL();
                }
        else
            {
            // Clear upgrade property.
            RProperty::Set( KEmailStoreUpgradePSCategory, 
                            KProperty_EmailStore_Upgrade, 
                            0 );

            __LOG_WRITE_INFO( "PreInstall done due to errors, Calling CActiveScheduler::Stop()" )
            CActiveScheduler::Stop();
            }
        }
    
    __LOG_EXIT;
    }

/**********************************
 * Method: DoCancel
 **********************************/
void CEmailStorePreInstall::DoCancel()
    {
    // If we are waiting for a process to close?
    if ( EWaitForProcess == iState )
        {
        iTimeoutTimer->Cancel();
        iProcess.LogonCancel( iStatus );
        }
    else
        {
        CTimer::Cancel();
        }
    
    // Clear upgrade property.
    RProperty::Set( KEmailStoreUpgradePSCategory, 
                    KProperty_EmailStore_Upgrade, 
                    0 );

    iState = EDone;
    // Should we do what EDone state is supposed to do?
    //CActiveScheduler::Stop();
    }

/**********************************
 * Method: RunError
 **********************************/
TInt CEmailStorePreInstall::RunError( TInt __LOG_BLOCK(aError) )
    {
    __LOG_ENTER( "RunError" )
    __LOG_WRITE_FORMAT1_INFO( "err=%d", aError )
    __LOG_EXIT
    return KErrNone;
    }

void CEmailStorePreInstall::StopNextServerL()
    {
    __LOG_ENTER( "StopNextServerL" )
    
    while ( serverList[iServerIndex].iServerSid != 0 )
        {
        // Notify upgrade regardless of the server running state.
        RProperty::Set( KEmailStoreUpgradePSCategory, 
                        KProperty_EmailStore_Upgrade, 
                        static_cast<TInt>( serverList[iServerIndex].iServerSid ) );

        // Verify if the process is running
        TFindProcess findProcess;
        findProcess.Find( serverList[iServerIndex].iServerName );
        TFullName fullProcessName;
        if ( KErrNone == findProcess.Next( fullProcessName ) )
            {
            // Process is running
            __LOG_WRITE_FORMAT1_INFO( "Running Process - %S", &fullProcessName )
            iProcess.Close();

            if ( KErrNone == iProcess.Open( findProcess ) )
                {                
                iProcess.Logon( iStatus );
                __LOG_BLOCK( TFileName processName = iProcess.FullName(); )
                __LOG_WRITE_FORMAT1_INFO( "Starting Timeout Timer for: %S", &processName )
                iTimeoutTimer->StartTimeoutTimer( KProcessTimeout );
                SetActive();
                iState = EWaitForProcess;
                break;
                }
            else
                {
                // Move to next one.
                ++iServerIndex;
                }
            }
        else
            {
            // Move to next one.
            ++iServerIndex;
            }
        }
    
    if ( serverList[iServerIndex].iServerSid == 0 )
        {
        iState = EDone;
        After( 0 );
        }
    
    __LOG_EXIT
    }

void CEmailStorePreInstall::Timeout()
    {
    // Cancel notifications
    iProcess.LogonCancel( iStatus );
    
    // Kill this process
    iProcess.Kill( KErrNone );
    
    // Stop next server
    ++iServerIndex;
    iState = EStopServers;    
    }

LOCAL_C void DoPreInstallL() 
    {   
    // Construct and install the active scheduler
    CActiveScheduler* scheduler = new ( ELeave ) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler ); 
        
    CEmailStorePreInstall::NewLC();
    
    CActiveScheduler::Start();
    
    CleanupStack::PopAndDestroy( 2 );  // CEmailStorePreInstall, scheduler
    } // DoPostInstallL


GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
    
    CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
    
    TRAP_IGNORE( DoPreInstallL() ); 
    
    delete cleanup; // destroy clean-up stack
    
    __UHEAP_MARKEND;
    
    return 0; 
    } // end E32Main


