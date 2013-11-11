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
* Description:  Synchronization handler
*
*/


#ifndef NSMLDMSYNCHANDLER_H
#define NSMLDMSYNCHANDLER_H

// INCLUDES
#include <e32base.h>
//#include <AknProgressDialog.h>
#include <SyncMLObservers.h>
//#include "NSmlDMProgress.h"
#include "nsmldmsyncutil.h"
#include "nsmldmsyncprofile.h"
#include "nsmldmsyncappengine.h"
#include <hbdialog.h>
#include <hblabel.h>
#include <hbaction.h>
// FORWARD DECLARATIONS
class CNSmlDMSyncState;
class CNSmlDMActiveCaller;
//class CEikonEnv;
//class CNSmlDMSyncDocument;
//class CAknWaitDialog;

// CLASS DECLARATION

/**
* CNSmlDMSyncHandler class
* 
*/
NONSHARABLE_CLASS (CNSmlDMSyncHandler) : public CBase, 
                        /*public MNSmlDMProgressDialogObserver,*/
						public MNSmlDMActiveCallerObserver,
						public MSyncMLEventObserver,
						public MSyncMLProgressObserver/*,
						public MProgressDialogCallback*/
						
	{
    public:
        /**
        * Two-phased constructor.
        */
	    static CNSmlDMSyncHandler* NewL( RSyncMLSession* aSyncSession,
	                                     CNSmlDMSyncAppEngine* aAppEngine/*,
	                                     CNSmlDMSyncDocument* aSyncDocument*/ );

	    /**
        * Destructor.
        */
	    virtual ~CNSmlDMSyncHandler();

    private:
	    /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();

	    /**
        * C++ default constructor.
        */
	    CNSmlDMSyncHandler( RSyncMLSession* iSyncSession,
                            CNSmlDMSyncAppEngine* aAppEngine/*,
	                        CNSmlDMSyncDocument* aSyncDocument*/ );		
    
	private: // from MAspActiveCallerObserver
		/**
        * From MAspActiveCallerObserver
		* Called when CAspActiveCaller completes.
        * @param None
        * @return None.
        */
		void HandleActiveCallL();


    private:    // from MSyncMLEventObserver
        
		/**
        * From MSyncMLEventObserver
		* Called when SyncML session events oocur.
        * @param aEvent Sync event
        * @param aIdentifier Identifier for the sync event
        * @param aError Synce event error
        * @param aAdditionalData.
        * @return None.
        */
        void OnSyncMLSessionEvent( TEvent aEvent,
                                   TInt aIdentifier,
                                   TInt aError,
                                   TInt aAdditionalData );
	
	
    private:  //from MSyncMLProgressObserver
	    
	    /**
	    * Receives notification of a synchronisation error.
        * @param aErrorLevel  The error level.
	    * @param aError		  The type of error. This is one of 
	    *                     the SyncMLError error values.	
	    * @param aTaskId      The ID of the task for which the error occurred.
	    * @param aInfo1       An integer that can contain additional
	    *                     information about the error. Normally 0.
	    * @param aInfo2       An integer that can contain additional
	    *                     information about the error. Normally 0.
	    * @return             None.
	    */
	    void OnSyncMLSyncError( TErrorLevel aErrorLevel,
	                            TInt aError,
	                            TInt aTaskId,
	                            TInt aInfo1,
	                            TInt aInfo2 );
       	
       	/**
	    * Receives notification of synchronisation progress.
	    * @param aStatus	The current status, e.g. 'Connecting'.
	    * @param aInfo1	    An integer that can contain additional
	    *                   information about the progress.
	    * @param aInfo2	    An integer that can contain additional
        *                   information about the progress.
	    */
	    void OnSyncMLSyncProgress( TStatus aStatus, TInt aInfo1, TInt aInfo2);
	    
	    /**
	    * Receives notification of modifications to synchronisation tasks.
        * @param aTaskId               The ID of the task.
	    * @param aClientModifications  Modifications made on 
	    *                              the clients Data Store.
	    * @param aServerModifications  Modifications made on
	    *                              the server Data Store.
	    */
	    void OnSyncMLDataSyncModifications( 
	                TInt /*aTaskId*/,
	                const TSyncMLDataSyncModifications& aClientModifications,
	                const TSyncMLDataSyncModifications& aServerModifications );
        
	private:

        /**
        * Shows error dialog.
		* @param None.
        * @return Error code.
        */
        void HandleSyncErrorL();
        
        /**
        * Called when sync completes
		* @param aError.
        * @return None.
        */        
        void SynchronizeCompletedL(TInt aError);
        
        /**
        * Read sync error from CNSmlSyncLog and show error message.
		* @param aProfileId Profile id.
        * @return Result code.
        */
		TInt HandleSyncErrorL(TInt aProfileId);
	
	public:
        /**
        * Performs synchronization.
        * @param aServerName Name of the server
		* @param aProfileId Profile identifier.
		* @param aConnectionBearer Bearer
		* @param aUseFotaProgressNote Should the simplified progress note used. Only
		*                             used when checking for firmware updates.
        * @return None
        */
		void SynchronizeL( TDesC& aServerName,
		                   const TInt aProfileId,
		                   const TInt aConnectionBearer,
                           const TBool aUseFotaProgressNote );
		
        /**
        * Performs ServerInitiated synchronization.
        * @param aServerName Name of the server
		* @param aProfileId Profile id
		* @param aJobId Job id.
		* @param aConnectionBearer Bearer
		* @param aUseFotaProgressNote Should the simplified progress note used. Only
		*                             used when checking for firmware updates.
        * @return None
        */		
		void SynchronizeL( TDesC& aServerName, 
		                   const TInt aProfileId,
		                   const TInt aJobId,
		                   const TInt aConnectionBearer,
                           const TBool aUseFotaProgressNote );
		
        /**
        * Shows the progress dialog.
		* @param None.
        * @return None.
        */
        void ShowProgressDialogL();

        /**
        * Deletes the progress dialog if it exists.
		* @param None.
        * @return None.
        */
        void HideProgressDialogL();
        
		/**
        * Cancel synchronization.
		* @param None 
        * @return None
        */
		void CancelSynchronizeL();
				
	private:
        
        /**
        * Performs synchronization.
        * @return None
        */	
		void SynchronizeL();
		
		/**
        * Utility function.
        * @return Sync session.
        */
		RSyncMLSession& Session();

    public:

		/**
        * Utility function.
        * @return ETrue if sync is currently running, EFalse otherwise.
        */
		TBool SyncRunning();

    private:
		// session with sync server
		RSyncMLSession*                 iSyncSession;
		// app engine
		CNSmlDMSyncAppEngine*           iSyncAppEngine;       
		// profile id
		TInt                            iProfileId;
		// sync job id
		TInt                            iSyncJobId;
		// sync job
		RSyncMLDevManJob                iSyncJob;		
        
        // CheckUpdate state shown or Not
        TBool iCheckUpdate;

		CNSmlDMActiveCaller*            iActiveCaller;
		// is sync currently running
		TBool                           iSyncRunning;
		// sync error code
		TInt                            iSyncError;
		// long buffer for string handling
		TBuf<KBufSize256>               iBuf;
		//Job id
        TSmlJobId                       iJobId;
        // Bearer
        TInt                            iConnectionBearer;
        // Server name
        TBuf<KNSmlMaxProfileNameLength> iServerName;
        // Server sync
        TBool                           iServerAlertedSync;
        // Should the simpler FOTA progress not be used
        TBool                           iUseFotaProgressNote;
        // Retry sync (authentication error)
        TBool							iRetrySync;
          // Variable to show whether in fotaview or not        
        TInt  iNotinFotaView;
        
        // Identifies Fota progress note launched or not
        TBool iFotaProgressLaunched;
        
        HbDialog * iDialog;
        
        HbAction * iPrimaryAction;
	};

#endif  // NSMLDMSYNCHANDLER_H

// End of file
