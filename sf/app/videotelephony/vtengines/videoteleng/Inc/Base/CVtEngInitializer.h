/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for initializing the engine gradually.
*                Active object initializes one "subsystem" on each
*                round. In case of failure timer is started and
*                retrial is done after time-out. Uninitialization
*                is done is one shot.
*
*/



#ifndef CVTENGINITIALIZER_H
#define CVTENGINITIALIZER_H

//  INCLUDES
#include    "MVtEngSettingObserver.h"
#include    <e32base.h>
#include    <mvtprotocolhandler.h>

// FORWARD DECLARATIONS
class CVtEngHandlerContainer;
class CVtEngOperation;


/**
*  Engine initializer.
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtEngInitializer ) :
    public CActive,
	public MVtEngSettingObserver
    {
    public: // Constructors and destructor

        static CVtEngInitializer* NewL( CVtEngHandlerContainer& aHandlers );

        /**
        * Destructor.
        */
        virtual ~CVtEngInitializer();

    public: // new functions

        /**
        * Starts initialization process.
        */
        void InitializeL( CVtEngOperation& aCallback );

        /**
        * Cancels initialiation
        * @return ETrue if CActive::Cancel was called
        */
        TBool CancelInit();

        /**
        * Uninitializes engine.
        */
        void Uninitialize( CVtEngOperation& aCallback );

        /**
        * Gets GetSessionCommand ownership after initialization.
        * @return CSessionCommand interface.
        */
        MVtSessionCommand* GetSessionCommand();

        /**
        * Returns if engine is initialized.
        * @return ETrue if initialized
        */
        TBool Initialized() const;

		/**
        * Creates handler for DTMF sending.
		* @param aH324Config H324 configure interface
        */
        void CreateDtmfHandlerL( MVtH324ConfigCommand* aH324Config );

		/**
		* Gets pointer to comms. Still owns the comms.
		* @return Pointer to comms.
		*/		
        MCommServer* GetVtComms();

    private: // from CActive

        /**
        * performs one initialization step.
        */
        void RunL();

        /**
        * Cancels initialization process.
        */
        void DoCancel();

        /**
        * Handles error in the process.
        * @return always returns KErrNone
        */
        TInt RunError( TInt aError );

    public: // from MVtEngSettingObserver
        virtual void HandleSettingChangedL( 
            CVtEngSettings::TSettingId aId, 
            const TDesC& aValue );
            

    private: // Constructors

        /**
        * c++ constructor
        */
        CVtEngInitializer( CVtEngHandlerContainer& aHandlers );

        /**
        *
        */
        void ConstructL();

    private:

        enum TProcess
            {
            // Go to next step
            EContinue,
            // Reset engine
            EReset,
            // Retry current step when failed.
            ERetry
            };

    private: // new functions

        /**
        * Reads bit reversal information from locally variated flags.
        * @return ETrue if reversed.
        */
        TBool BitReversalUsed();

        /**
        * Proceeds to next step or starts retrial of
        * previously failed step.
        */
        void ContinueOrRetry( const TProcess aOperation );

        /**
        * Notifies observer on initialization completion.
        */
        void Notify( const TInt aResult );

        /**
        * Callback for retrial timer.
        */
        static TInt HandleTimer( TAny* aAny );

        /**
        * Handles retrial timer expiration.
        */
        void DoHandleTimer();
                  
        TBool ActivateAndSignal( TInt aSignalValue );
        
    private:


        /**
        * Initialization steps
        */
        enum TVtEngInitializationSteps
            {
            EInitNone,
            EInitSession,
            EInitMedia,
            EInitProto,
			EInitDataportCompleted,
            EInitComplete,
            EResetMedia,
            EResetMediaPhase2
            };

        /**
        * Dataport information handling flags.
        */
        enum TVtEngDataportInformationFlags
            {
            EDataportListened = 0x01,
            EDataportReceived = 0x02
            };

    private:

        // handler container
        CVtEngHandlerContainer& iHandlers;

        // Current (un)initialization step
        TInt                    iStep;

        // Operation to be notified on completion
        CVtEngOperation*        iCallback;

        // Retrial count
        TInt                    iRetrials;

        // Retrial timer
        CPeriodic*              iTimer;

        // Protocol Session Engine
		MVtSessionCommand* iSessionCommand;

        // Protocol Comms Server
        MCommServer*      iComms;

        // Has subscription been made for dataport name.
        TInt iDataportHandled;

		// If MediaHandler has taken ownership of 2-way, this is true,
		// otherwise false.
		TBool iOwnershipInMH;

    };


#endif      // CVTENGINITIALIZER_H

// End of File
