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
* Description:  Class for command execution for Video Telephone application.
*
*/

#ifndef CLCVTCMDEXECUTOR_H_
#define CLCVTCMDEXECUTOR_H_

//  INCLUDES
#include    <e32base.h>
#include    <mvtengcommandobserver.h>

// FORWARD DECLARATIONS
class MVtEngCommandHandler;
class MLcVtEngineCommandManager;

// CLASS DECLARATION

/**
*  Class for command execution for Video Telephone application.
*
*  @since Series 60 2.6
*/
class CLcVtCmdExecutor
    : public CBase,
      private MVtEngCommandObserver
    {
    public: // Constructors and destructor

        /**
        * Constructor.
        *
        * @param aSelfPtr optional pointer to itself.
        * @param aCommandHandler command handler.
        * @param aCommandManager command manager.
        */
        CLcVtCmdExecutor(
            CLcVtCmdExecutor** aSelfPtr,
            MVtEngCommandHandler& aCommandHandler,
            MLcVtEngineCommandManager& aCommandManager );

        /**
        * Destructor.
        */
        ~CLcVtCmdExecutor();

    public: // New functions

        /**
        * Executes command.
        * @param aCommandId command identifier.
        * @param aParams parameters.
        * @param aRequest request status, optional.
        */
        void ExecuteCmdL(
            const TVtEngCommandId aCommandId,
            TDesC8* aParams,
            TRequestStatus* aRequest );

        /**
        * @see MVtEngCommandObserver::HandleVTCommandPerformedL.
        */
        virtual void HandleVTCommandPerformedL(
            TVtEngCommandId aCommand,
            const TInt aError );
        
    protected: // New functions

        /**
        * Completes dialog with error.
        * Note: this method will delete the dialog.
        * @param aError error code.
        */
        void Complete( const TInt aError );
        
    private:
        /**
        * Handles execution failure.
        * @param aResult error code.
        */
        void HandleExecuteFailed( TInt aResult );

    private:

        // Inner class.
        class CActiveCmd;

        // Friend.
        friend class CActiveCmd;
        
        //pointer to itself 
        CLcVtCmdExecutor** iSelfPtr;
        
        // Error code.
        TInt* iError;

        // Request status.
        TRequestStatus* iRequest;        

        // Ref to command handler.
        MVtEngCommandHandler& iCommandHandler;

        // Ref to command manager.
        MLcVtEngineCommandManager& iCommandManager;

        // Command to be executed.
        TVtEngCommandId iCommand;

        // Owned parameters.
        HBufC8* iCommandParams;

        // ETrue if command has been completed.
        TBool iCommandCompleted;

        // Owned active object to perform the command.
        CActiveCmd* iActiveCmd;        
        
        // ActiveSheduler Waiter inside CLcVtCmdExecutor
        CActiveSchedulerWait iWait;
        
    };



#endif /* CLCVTCMDEXECUTOR_H_ */
