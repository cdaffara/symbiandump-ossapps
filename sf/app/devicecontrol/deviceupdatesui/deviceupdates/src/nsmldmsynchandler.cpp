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
* Description: Implementation of DM UI component
* 	This is part of omadmappui.
*
*/


// INCLUDES

#include <textresolver.h>   // for CTextResolver
#include <e32property.h>
#include <data_caging_path_literals.hrh>  // for resource and bitmap directories
#include <SyncMLErr.h>      // sync error codes
#include <DevManInternalCRKeys.h>
#include <centralrepository.h>
#include <featmgr.h>
#include "nsmldmsynchandler.h"
#include "nsmldmsyncdebug.h"
#include "nsmldmsyncinternalpskeys.h"
#include "nsmldmsyncprivatecrkeys.h"

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlDMSyncHandler* CNSmlDMSyncHandler::NewL( RSyncMLSession* aSyncSession,
                                              CNSmlDMSyncAppEngine* aAppEngine )
    {
	FLOG( "CNSmlDMSyncHandler::NewL()" );

    CNSmlDMSyncHandler* self =
        new (ELeave) CNSmlDMSyncHandler( aSyncSession,
                                         aAppEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlDMSyncHandler::~CNSmlDMSyncHandler()
    {
    FLOG( "CNSmlDMSyncHandler::~CNSmlDMSyncHandler()" );     
	delete iActiveCaller;

	if ( iSyncRunning )
	    {
	    TRAP_IGNORE( iSyncJob.StopL() );
        iSyncJob.Close();
	    }
	if(iPrimaryAction)
	    iPrimaryAction->deleteLater();
	
	if(iDialog)
	    iDialog->deleteLater();
	
	  FeatureManager::UnInitializeLib();
	FLOG( "CNSmlDMSyncHandler::~CNSmlDMSyncHandler() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::ConstructL( void )
    {
    FLOG( "CNSmlDMSyncHandler::ConstructL" );
   FeatureManager::InitializeLibL();   	
	iActiveCaller = CNSmlDMActiveCaller::NewL(this);
	iServerAlertedSync = EFalse;
	iSyncRunning = EFalse;
	iSyncError = KErrNone;	
	iCheckUpdate = EFalse;	
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::CNSmlDMSyncHandler
// -----------------------------------------------------------------------------
//
CNSmlDMSyncHandler::CNSmlDMSyncHandler( RSyncMLSession* aSyncSession,
                                        CNSmlDMSyncAppEngine* aAppEngine) 
    : iSyncSession( aSyncSession ),
    iSyncAppEngine( aAppEngine )   
	{
	}



// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::SynchronizeL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::SynchronizeL( TDesC& aServerName,
                                       const TInt aProfileId,
                                       const TInt aJobId,
                                       const TInt aConnectionBearer,
                                       const TBool aUseFotaProgressNote )
	{
	FLOG( "CNSmlDMSyncHandler::SynchronizeL Server alert" );
	
	iServerName = aServerName;
	iConnectionBearer = aConnectionBearer;
    iProfileId = aProfileId;
    iJobId = aJobId;
    iUseFotaProgressNote = aUseFotaProgressNote;
    
    iSyncJob.OpenL( Session(), iJobId );
    iSyncJobId = iSyncJob.Identifier();
    FTRACE( FPrint( _L(
	    "[OMADM]\tCNSmlDMSyncHandler::SynchronizeL--server alert, iSyncJobId = %d" ),
	     iSyncJobId ) ); 
	  //SAN Support
	  iServerAlertedSync = ETrue;  
	                      
	  //SAN Support
    SynchronizeL();
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::SynchronizeL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::SynchronizeL( TDesC& aServerName,
                                       const TInt aProfileId,
                                       const TInt aConnectionBearer,
                                       const TBool aUseFotaProgressNote )
    {
    FLOG( "CNSmlDMSyncHandler::SynchronizeL" );

    iServerName = aServerName;
    iConnectionBearer = aConnectionBearer;
    iProfileId = aProfileId;
    iUseFotaProgressNote = aUseFotaProgressNote;

    iSyncJob.CreateL( Session(), iProfileId );
    iSyncJobId = iSyncJob.Identifier();
    SynchronizeL();
    
    
    FTRACE( FPrint( _L(
	    "[OMADM]\tCNSmlDMSyncHandler::SynchronizeL--UI initiated, iSyncJobId = %d" ),
	     iSyncJobId ) );       
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::SynchronizeL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::SynchronizeL()
	{
	FLOG( "CNSmlDMSyncHandler::SynchronizeL" );
	
	TInt err = KErrNone;
	
	// for MSyncMLEventObserver events
	TRAP( err, Session().RequestEventL(*this) );
	if ( err != KErrNone )
		{
		iSyncJob.StopL();
		iSyncJob.Close();
		User::Leave(err);
		}
    
    // for MSyncMLProgressObserver events
    TRAP( err, Session().RequestProgressL(*this) );
	if ( err != KErrNone )
		{
		Session().CancelEvent();
		iSyncJob.StopL();
		iSyncJob.Close();
		User::Leave( err );
		}
	
	   
	
	TRAP( err, ShowProgressDialogL() );
	if ( err != KErrNone )
		{
		Session().CancelEvent();
		Session().CancelProgress();
		iSyncJob.StopL();
		iSyncJob.Close();
		User::Leave( err );
		}
	FLOG( "CNSmlDMSyncHandler::SynchronizeL Sync is running" );

    //iSyncDocument->MarkFwUpdChangesStartL();
	
	iSyncRunning = ETrue;
	iSyncError = KErrNone;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::SynchronizeCompletedL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::SynchronizeCompletedL( TInt aError )
	{
    FTRACE( FPrint(
        _L("[OMADM]\t CNSmlDMSyncHandler::SynchronizeCompletedL(), aError = %d"),
        aError ) );
	
	if ( !SyncRunning() )
		{
		FLOG( "CNSmlDMSyncHandler::SynchronizeCompletedL already completed" );
		return;  // sync has already completed
		}
		
    iSyncJob.Close();
    
     FTRACE( FPrint( _L(
	    "[OMADM]\tCNSmlDMSyncHandler::SynchronizecompletedL, iSyncJobId = %d" ),
	     KNSmlDMNoUserInitiatedJob ) );
   	
	iSyncRunning = EFalse;
	iSyncError = aError;    
    iCheckUpdate = EFalse;        	

#ifndef NO_UPDATE 
        CRepository* centrep = NULL;
        TRAPD( err, centrep = CRepository::NewL( KCRUidNSmlDMSyncApp ) );
	TInt profileId;
	centrep->Get( KNSmlDMDefaultFotaProfileKey, profileId );
	//FLOG( "CNSmlDMSyncHandler::SynchronizeCompletedL profileId = %d", profileId );
	delete centrep;
	if ( profileId ==  iProfileId )
        {
        
	//FLOG( "CNSmlDMSyncHandler::SynchronizeCompletedL - connected witrh default profile ID" );
        TInt configFlags( 0 );
        //TInt SetGenValue(0);
        TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys ) );
        if ( centrep )
            {

	  //  FLOG( "CNSmlDMSyncHandler::SynchronizeCompletedL Centrep is defined" );
	       if( err == KErrNone )
        	{
	        centrep->Get( KDevManSessionType, configFlags );
	    //	FLOG( "CNSmlDMSyncHandler::SynchronizeCompletedL DevManSessiontype = %d", configFlags );
             	}
	       centrep->Set( KDevManSessionType, 0 );
	    	//FLOG( "CNSmlDMSyncHandler::SynchronizeCompletedL KDevManSessionType getting reset" );
	        delete centrep;
            } 
        // configFlags=2 for FOTA Package not downloaded case ,1- successful download
        if ( (aError == KErrNone) 
        			&& configFlags == 2 )
            {
            //FLOG( "[OMADM]\t CNSmlDMSyncHandler::SynchronizeCompletedL(): No changes in FW Update states." );
        iDialog = new HbDialog();
        iDialog->setHeadingWidget(new HbLabel(hbTrId("txt_device_update_title_phone_uptodate")));
        iDialog->setContentWidget(new HbLabel(hbTrId("txt_device_update_dpopinfo_your_phone_is_already_u")));
        iPrimaryAction = new HbAction();
        iPrimaryAction->setText(hbTrId("OK"));
	    iDialog->setPrimaryAction(iPrimaryAction);
	    iDialog->setTimeout(HbPopup::NoTimeout);
	    iDialog->setDismissPolicy(HbPopup::NoDismiss);
	    iDialog->show();
            // show the dialog
            }

        }  
#endif

	iUseFotaProgressNote = EFalse;
    iSyncJob.Close();
    
	// handle error in HandleActiveCallL (when active caller completes)
	iActiveCaller->Start( aError );

	FLOG( "CNSmlDMSyncHandler::SynchronizeCompletedL() completed" );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::OnSyncMLSessionEvent
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::OnSyncMLSessionEvent( TEvent aEvent,
                                               TInt aIdentifier,
                                               TInt aError,
                                               TInt /*aAdditionalData*/ )
	{
	FTRACE( FPrint( _L(
	    "[OMADM]\tCNSmlDMSyncHandler::OnSyncMLSessionEvent(), aEvent = %d, aIdentifier = %d, aError = %d" ),
	    aEvent, aIdentifier, aError ) );
	FTRACE( RDebug::Print( _L(
	    "[OMADM]\tCNSmlDMSyncHandler::OnSyncMLSessionEvent(), aEvent = %d, aIdentifier = %d, aError = %d" ),
	    aEvent, aIdentifier, aError ) );

	if ( aEvent == EJobStop
	    || aEvent == EJobStartFailed
	    || aEvent == EJobRejected )
		{
    	FTRACE( RDebug::Print( _L(
    	    "[OMADM]\tCNSmlDMSyncHandler::OnSyncMLSessionEvent(), iSyncJobId = %d" ),
    	    iSyncJobId ) );
    	  FTRACE( FPrint( _L(
    	    "[OMADM]\tCNSmlDMSyncHandler::OnSyncMLSessionEvent(), iSyncJobId = %d" ),
    	    iSyncJobId ) );  
			
		if ( iSyncJobId == aIdentifier )
			{
			FLOG( "CNSmlDMSyncHandler::OnSyncMLSessionEvent EJobStop close" );
			
            TRAP_IGNORE( SynchronizeCompletedL( aError ) );
			}
		}
    FLOG( "[OMADM]\tCNSmlDMSyncHandler::OnSyncMLSessionEvent() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::OnSyncMLSyncError
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::OnSyncMLSyncError( TErrorLevel aErrorLevel,
                                            TInt aError,
                                            TInt /*aTaskId*/,
                                            TInt /*aInfo1*/,
                                            TInt /*aInfo2*/)
	{
	FTRACE( FPrint( _L(
	    "[OMADM]\tCNSmlDMSyncHandler::OnSyncMLSyncError(), aErrorLevel = %d, aError = %d"),
	    aErrorLevel, aError ) );
	    FTRACE( RDebug::Print(
	          _L("CNSmlDMSyncHandler::OnSyncMLSyncProgress  aErrorLevel =(%d) aError =(%d)"), aErrorLevel, aError  ) );
	
	if ( aErrorLevel == ESmlFatalError )
		{
        TRAP_IGNORE( SynchronizeCompletedL( aError ) );
		}
    FLOG( "[OMADM]\tCNSmlDMSyncHandler::OnSyncMLSyncError() completed" );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::OnSyncMLSyncProgress
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::OnSyncMLSyncProgress( TStatus aStatus,
                                               TInt /*aInfo1*/,
                                               TInt /*aInfo2*/ )
	{
	FTRACE( RDebug::Print(
	          _L("CNSmlDMSyncHandler::OnSyncMLSyncProgress (%d)"), aStatus ) );
	
	switch (aStatus)
		{
		case ESmlConnecting:
		    //State()->SetSyncPhase( CNSmlDMSyncState::EPhaseConnecting );
		    break;
		case ESmlConnected:
		    break;
		case ESmlLoggingOn:
		    break;
		case ESmlLoggedOn:
		    
		    break;
		case ESmlDisconnected:
		    
		    break;
		case ESmlCompleted:
		   
		    break;
		case ESmlProcessingServerCommands:
		   
		    break;
		case ESmlReceivingServerCommands:
		   
		    break;
		
		case ESmlSendingModificationsToServer:
		    {		   		    
		    break;
		    }
		
		default:
		    break;
		}    
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::OnSyncMLDataSyncModifications
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::OnSyncMLDataSyncModifications( TInt /*aTaskId*/,
                   const TSyncMLDataSyncModifications& /*aClientModifications*/,
                   const TSyncMLDataSyncModifications& /*aServerModifications*/ )
    {
    FLOG( "CNSmlDMSyncHandler::OnSyncMLDataSyncModifications" );

    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::HandleActiveCallL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::HandleActiveCallL()
	{
    FLOG( "CNSmlDMSyncHandler::HandleActiveCallL" );
    
    // HandleSyncErrorL will set this to true if
    // sync will be retried.
    iRetrySync = EFalse;

	if ( (iSyncError != KErrNone) && (iSyncError != KErrCancel) )
		{
		TRAP_IGNORE( HandleSyncErrorL() );
		}
	if ( iRetrySync == EFalse )
		{
	    // Inform parent that sync is done.
	    iSyncAppEngine->SyncCompleted( ENSmlSyncComplete );		
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::HandleSyncErrorL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::HandleSyncErrorL()
	{
	FLOG( "CNSmlDMSyncHandler::HandleSyncErrorL" );
	
	if ( iSyncError != KDMErr )
		{
		}
	if ( ( iSyncError == SyncMLError::KErrAuthenticationFailure ) ||
	    ( iSyncError == SyncMLError::KErrTransportAuthenticationFailure ) )
        {
	    FLOG( "CNSmlDMSyncHandler::HandleSyncErrorL ask username" );
        }
    }
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::ShowProgressDialogL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::ShowProgressDialogL( )
	{
	FLOG( "CNSmlDMSyncHandler::ShowProgressDialogL" );
	
	FLOG( "CNSmlDMSyncHandler::ShowProgressDialogL() completed" );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::HideProgressDialogL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::HideProgressDialogL()
    {
    FLOG( "CNSmlDMSyncHandler::HideProgressDialogL" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::Session
// -----------------------------------------------------------------------------
//
RSyncMLSession& CNSmlDMSyncHandler::Session()
	{
	__ASSERT_DEBUG(iSyncSession, TUtil::Panic(KErrGeneral));
	
	return *iSyncSession;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::SyncRunning
// -----------------------------------------------------------------------------
//
TBool CNSmlDMSyncHandler::SyncRunning()
	{
	FTRACE( RDebug::Print(
	          _L("CNSmlDMSyncHandler::SyncRunning status (%d)"), iSyncRunning ) );
	          
	return iSyncRunning;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::CancelSynchronizeL
// -----------------------------------------------------------------------------
//		
void CNSmlDMSyncHandler::CancelSynchronizeL()
    {
    FLOG( "CNSmlDMSyncHandler::CancelSynchronizeL" );
    
    if ( iSyncRunning )
        {
        iSyncJob.StopL();
        SynchronizeCompletedL( KErrCancel );
        }
    }

// End of File
