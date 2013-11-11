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
* Description:  Implementation of the CLcVtCmdExecutor class.
*
*/


// INCLUDE FILES
#include    "clcvtcmdexecutor.h"
#include    "mlcvtenginecommandmanager.h"
#include    <mvtengcommandhandler.h>
#include    <cvtlogger.h>

// MODULE DATA STRUCTURES

/**
* Active object to perform asynchronous commands.
* @since Series 60 2.6
*/
class CLcVtCmdExecutor::CActiveCmd
    : public CActive
    {
    public: // Constructors and destructor

        /**
        * Constructor.
        */
        CActiveCmd(
            CLcVtCmdExecutor& aDialog,
            MVtEngCommandHandler& aCommandHandler,
            TVtEngCommandId aCommand,
            TDesC8* aParams );

        /**
        * Destructor.
        */
        ~CActiveCmd();

    public: // New functions

        /**
        * Starts active object. Command will be performed in RunL.
        */
        void Start();

        /**
        * Checks if command has been performed.
        * @return ETrue if command has been performed.
        */
        TBool CommandPerformed() const;

    private:

        /**
        * @see CActive::RunL
        */
        void RunL();

        /**
        * @see CActive::DoCancel.
        */
        void DoCancel();

        /**
        * @see CActive::RunError.
        */
        TInt RunError( TInt aResult );

    private:

        // Ref to dialog.
        CLcVtCmdExecutor& iExecutor;

        // Ref to command handler.
        MVtEngCommandHandler& iCommandHandler;

        // Command to be executed.
        TVtEngCommandId iCommand;

        // Owned parameters.
        TDesC8* iCommandParams;

        // ETrue if command has been performed.
        TBool iCommandPerformed;

    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLcVtCmdExecutor::CLcVtCmdExecutor
// -----------------------------------------------------------------------------
//
CLcVtCmdExecutor::CLcVtCmdExecutor(
        CLcVtCmdExecutor** aSelfPtr,
        MVtEngCommandHandler& aCommandHandler,
        MLcVtEngineCommandManager& aCommandManager )
    : iSelfPtr( aSelfPtr ),
      iCommandHandler( aCommandHandler ),
      iCommandManager( aCommandManager )
    {
    iRequest = NULL;
    __VTPRINT2( DEBUG_GEN, "CLcVtCmdExecutor.Ctor this=%d", (TInt)this )
    }

// -----------------------------------------------------------------------------
// CLcVtCmdExecutor::~CLcVtCmdExecutor
// -----------------------------------------------------------------------------
//
CLcVtCmdExecutor::~CLcVtCmdExecutor()
    {
    __VTPRINTENTER( "CLcVtCmdExecutor.~" )
    __VTPRINT2( DEBUG_GEN, "CLcVtCmdExecutor.~ this=%d", (TInt)this )
    if ( iActiveCmd )
        {
        if ( !iCommandCompleted && iActiveCmd->CommandPerformed() )
            {
            iCommandHandler.CancelCommand( iCommand ); // ignore error
            }
        }
    delete iCommandParams;
    delete iActiveCmd;

    iCommandManager.RemoveObserver( *this );
    
    if ( iSelfPtr )
        {
        *iSelfPtr = NULL;
        iSelfPtr = NULL;
        }

    if ( iRequest )
        {
        __VTPRINT(DEBUG_GEN, "CLcVtCmdExecutor::~CLcVtCmdExecutor RequestComplete")
        User::RequestComplete( iRequest, KErrCancel );
        iRequest = NULL;
        }
    else
        {
        if ( iWait.IsStarted())
            {
            __VTPRINT(DEBUG_GEN, "CLcVtCmdExecutor::~CLcVtCmdExecutor AsyncStop")
            iWait.AsyncStop();
            }           
        }
    
    
    __VTPRINTEXIT( "CLcVtCmdExecutor.~" )
    }

// -----------------------------------------------------------------------------
// CLcVtCmdExecutor::ExecuteCmdL
// -----------------------------------------------------------------------------
//
void CLcVtCmdExecutor::ExecuteCmdL(
        const TVtEngCommandId aCommandId,
        TDesC8* aParams,
        TRequestStatus* aRequest )
    {
    __VTPRINTENTER( "CLcVtCmdExecutor.ExecuteCmdL" )
    __VTPRINT2( DEBUG_GEN, "CLcVtCmdExecutor.ExecuteCmdL this=%d", (TInt)this )
    iCommand = aCommandId;
    CleanupStack::PushL( this );
    if ( aParams )
        {
        iCommandParams = aParams->AllocL();
        }

    iCommandManager.AddObserverL( *this );
    CleanupStack::Pop( this );

    iActiveCmd =
        new ( ELeave ) CActiveCmd(
            *this, iCommandHandler, iCommand, iCommandParams );
    iActiveCmd->Start();    
    
    if ( !aRequest )
        {
        TInt error = KErrNone;
        iError = &error;
        //Start Active Schedule
        iWait.Start();
        User::LeaveIfError( error );
        }
    else
        {
        *aRequest = KRequestPending;        
        iRequest = aRequest; 
        } 
       
    __VTPRINTEXIT( "CLcVtCmdExecutor.ExecuteCmdL" )
    }


// -----------------------------------------------------------------------------
// CLcVtCmdExecutor::Complete
// -----------------------------------------------------------------------------
//
void CLcVtCmdExecutor::Complete( const TInt aError )
    {
    if ( iError )
        {
        *iError = aError;
        }

    if ( iRequest )
        {
        User::RequestComplete( iRequest, aError );
        iRequest = NULL;
        }

    delete this;
    }

// -----------------------------------------------------------------------------
// CLcVtCmdExecutor::HandleVTCommandPerformedL
// -----------------------------------------------------------------------------
//
void CLcVtCmdExecutor::HandleVTCommandPerformedL(
        TVtEngCommandId aCommand,
        const TInt aError )
    {
    __VTPRINTENTER( "CLcVtCmdExecutor.HandleVTCommandPerformed" )
    __VTPRINT2( DEBUG_GEN, "CLcVtCmdExecutor.HandleVTCommandPerformed.cmd.%d", aCommand )
    __VTPRINT2( DEBUG_GEN, "CLcVtCmdExecutor.HandleVTCommandPerformed.err.%d", aError )
    if ( iActiveCmd && ( aCommand == iCommand ) )
        {
        if ( iActiveCmd->CommandPerformed() && !iCommandCompleted )
            {
            __VTPRINT( DEBUG_GEN, "CLcVtCmdExecutor.match" )
            iCommandCompleted = ETrue;
            // Corrupted images may leave during initialization, thus we have
            // to mask out errors when they happen during share initialize.
            // Error code is handled correctly in CVtUiAppUi::CEventObserver::
            // HandleVTCommandPerformedL() method, thus it will not be ignored.
            if ( aCommand == KVtEngInitializeShareImage )
                {
                Complete( KErrNone );
                }
            else
                {
                Complete( aError );
                }
            }
        }
    __VTPRINTEXIT( "CLcVtCmdExecutor.HandleVTCommandPerformed" )
    }

// -----------------------------------------------------------------------------
// CLcVtCmdExecutor::HandleExecuteFailed
// -----------------------------------------------------------------------------
//
void CLcVtCmdExecutor::HandleExecuteFailed( TInt aResult )
    {
    __VTPRINT3( DEBUG_GEN, "CLcVtCmdExecutor.Fail this=%d res=%d",
        (TInt)this, aResult )
    Complete( aResult );
    }

// Implementation of CLcVtCmdExecutor::CActiveCmd:

// -----------------------------------------------------------------------------
// CLcVtCmdExecutor::CActiveCmd::CActiveCmd
// -----------------------------------------------------------------------------
//
CLcVtCmdExecutor::CActiveCmd::CActiveCmd(
        CLcVtCmdExecutor& aExecutor,
        MVtEngCommandHandler& aCommandHandler,
        TVtEngCommandId aCommand,
        TDesC8* aParams)
    : CActive( CActive::EPriorityHigh ),
      iExecutor( aExecutor ),
      iCommandHandler( aCommandHandler ),
      iCommand( aCommand ),
      iCommandParams( aParams )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CLcVtCmdExecutor::CActiveCmd::~CActiveCmd
// -----------------------------------------------------------------------------
//
CLcVtCmdExecutor::CActiveCmd::~CActiveCmd()
    {
    __VTPRINT2( DEBUG_GEN, "CLcVtCmdExecutor.CActiveCmd.Dtor this=%d", (TInt)this )
    Cancel();
    }

// -----------------------------------------------------------------------------
// CLcVtCmdExecutor::CActiveCmd::Start
// -----------------------------------------------------------------------------
//
void CLcVtCmdExecutor::CActiveCmd::Start()
    {
    __VTPRINTENTER( "CLcVtCmdExecutor.CActiveCmd.Start" )
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    __VTPRINTEXITR( "CLcVtCmdExecutor.CActiveCmd.Start this=%d", (TInt)this )
    }

// -----------------------------------------------------------------------------
// CLcVtCmdExecutor::CActiveCmd::CommandPerformed
// -----------------------------------------------------------------------------
//
TBool CLcVtCmdExecutor::CActiveCmd::CommandPerformed() const
    {
    __VTPRINT3( DEBUG_GEN, "CLcVtCmdExecutor.CActiveCmd.Perf this=%d,cmd=%d",
        (TInt)this, iCommandPerformed )
    return iCommandPerformed;
    }

// -----------------------------------------------------------------------------
// CLcVtCmdExecutor::CActiveCmd::RunL
// -----------------------------------------------------------------------------
//
void CLcVtCmdExecutor::CActiveCmd::RunL()
    {
    __VTPRINTENTER( "CLcVtCmdExecutor.CActiveCmd.RunL" )
    iCommandHandler.ExecuteL( iCommand, iCommandParams );
    iCommandPerformed = ETrue;
    __VTPRINTEXITR( "CLcVtCmdExecutor.CActiveCmd.RunL this=%d", (TInt)this )
    }

// -----------------------------------------------------------------------------
// CLcVtCmdExecutor::CActiveCmd::DoCancel
// -----------------------------------------------------------------------------
//
void CLcVtCmdExecutor::CActiveCmd::DoCancel()
    {
    __VTPRINT2( DEBUG_GEN, "CLcVtCmdExecutor.CActiveCmd.DoCnl this=%d", (TInt)this )
    // Request is completed immediately.
    }

// -----------------------------------------------------------------------------
// CVtUiExecuteCmdDialog::CActiveCmd::RunError
// -----------------------------------------------------------------------------
//
TInt CLcVtCmdExecutor::CActiveCmd::RunError( TInt aResult )
    {
    __VTPRINTENTER( "CLcVtCmdExecutor.CActiveCmd.RunError" )
    // Exception was raised in RunL. Inform the dialog to close itself.
    iExecutor.HandleExecuteFailed( aResult );
    __VTPRINTEXITR( "CLcVtCmdExecutor.CActiveCmd.RunError this=%d", (TInt)this )
    return KErrNone;
    }

//  End of File
