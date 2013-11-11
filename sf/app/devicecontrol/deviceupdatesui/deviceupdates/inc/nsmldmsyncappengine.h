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
* Description:  Application engine for the sync events
*
*/



#ifndef CNSMLDMSYNCAPPENGINE_H
#define CNSMLDMSYNCAPPENGINE_H

//  INCLUDES
#include <e32base.h>
#include <SyncMLClient.h>
#include "nsmldmdef.h"

// FORWARD DECLARATIONS
class CNSmlDMSyncHandler;
class CNSmlDMSyncProfile;
class DmInfo;
//class CNSmlDMSyncDocument;

// CLASS DECLARATION

/**
* MNSmlDMSyncObserver
*
* MNSmlDMSyncObserver is for observing Sync.
*/
class MNSmlDMSyncObserver
    {
    public:
	    virtual void SyncComplete( TNSmlStatus aStatus ) = 0;
    };
   
/**
* CNSmlDMSyncAppEngine
* Sync engine for handling synchronization and profiles.
*/
NONSHARABLE_CLASS (CNSmlDMSyncAppEngine) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CNSmlDMSyncAppEngine* NewL(DmInfo* dmObj);
        
        /**
        * Destructor.
        */
        virtual ~CNSmlDMSyncAppEngine();

    public: // New functions
        
        /**
        * Delete profile
        * @param aProfileId Profile to be deleted.
        * @return None
        */
        void DeleteProfileL( TInt aProfileId );

        /**
        * Create profile
        * @param None.
        * @return None
        */
        TInt CreateProfileL();

        /**
        * Create copy profile
        * @param aProfileId Profile that is used for copying
        * @return None
        */
        CNSmlDMSyncProfile* CreateCopyProfileL( TInt aProfileId );

        /**
        * Return current profile
        * @param None.
        * @return CNSmlDMSyncProfile*
        */
        CNSmlDMSyncProfile* Profile( );

        /**
        * Cancel synchronization
        * @param None.
        * @return None
        */
        void CancelSyncL();
        
        /**
        * Return RSyncMLSession
        * @param None.
        * @return RSyncMLSession*
        */
        RSyncMLSession* Session();
        
        /**
        * Open profile
        * @param aProfileId Profile id to open
        * @param aOpenMode opening mode
        * @return None
        */
        CNSmlDMSyncProfile* OpenProfileL( TInt aProfileId,
                                          TInt aOpenMode = ESmlOpenReadWrite );

        /**
        * Close profile
        * @param None.
        * @return None
        */        
        void CloseProfile();
        
        /**
        * Start synchronization
        * @param aProfileId Profile id to sync
        * @return None
        */               
        void SynchronizeL( TDesC& aServerName,
                           const TInt aProfileId,
                           const TInt aConnectionBearer,
                           const TBool aUseFotaProgressNote );
        
        /**
        * Start synchronization
        * @param aProfileId Profile id to sync
        * @param aJobId Jod id to sync
        * @return None
        */         
        void SynchronizeL( TDesC& aServerName,
                           const TInt aProfileId,
                           const TInt aJobId,
                           const TInt aConnectionBearer,
                           const TBool aUseFotaProgressNote );
        
        /**
        * Check if duplicate server id is found
        * @param  aServerId Server id
        * @param  aProfileId current profile id
        * @return ETrue if server id found
        */                 
        TBool ServerIdFoundL( const TDesC& aServerId, const TInt aProfileId );
        
		/**
		* Compare two descriptors
        * @param aLeft  
		* @param aRight  
		* @return - Positive, if this descriptor is 
        *                     greater than the specified descriptor.
		*           Negative, if this descriptor is
		*                     less than the specified descriptor.
		*           Zero, if the content of both descriptors match
        */
        TInt Compare( const TDesC& aLeft, const TDesC& aRight );
        
        /**
        * Sync state
        * @param None
        * @return ETrue if sync is running
        */          
        TBool SyncRunning();
        
        /**
        * Utility function.
        * @param aSyncObserver
        * @return None
        */
		void RequestSyncStatus( MNSmlDMSyncObserver* aSyncObserver );
		
		void SyncCompleted( TNSmlStatus aStatus );

        /**
        * Hides or shows the device management progress dialog.
        * @since Series 60 3.1
        * @param aShow ETrue -> The progress dialog is shown; EFalse -> The dialog is hidden.
        * @return None
        */
        void ShowProgressDialogL( TBool aShow );
        
    private:

        /**
        * C++ default constructor.
        */
        CNSmlDMSyncAppEngine();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( DmInfo* dmcallback );


    private:    // Data
        // SyncML Session
        RSyncMLSession          iSyncMLSession;
        // Sync profile
        CNSmlDMSyncProfile*     iProfile;
        // Sync handler
        CNSmlDMSyncHandler*     iSyncHandler;
        TBool               iProgressShown;
        // Sync Observer
        MNSmlDMSyncObserver*    iSyncObserver;
        DmInfo* iDmcallback;
    };

#endif      // CNSMLDMSYNCAPPENGINE_H

// End of File
