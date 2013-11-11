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
* Description:  Command handler of the engine.
*
*/



#ifndef CVTENGCOMMANDHANDLER_H
#define CVTENGCOMMANDHANDLER_H

//  INCLUDES
#include <e32base.h>
#include "MVtEngCommandHandler.h"
#include "mvtengoperationobserver.h"

// FORWARD DECLARATIONS
class CVtEngHandlerContainer;
class CVtEngOperation;

// CLASS DECLARATION

/**
*  Handler for commands from client.
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtEngCommandHandler ) : public CBase,
    public MVtEngCommandHandler,
    public MVtEngOperationObserver
    {
    public: // constructor and destructors

        static CVtEngCommandHandler* NewL( 
            CVtEngHandlerContainer& aHandlers );

        virtual ~CVtEngCommandHandler();

    public: // from MVtEngCommandHandler

        /**
        * @see MVtEngCommandHandler
        */        
        virtual void ExecuteL( 
            const TVtEngCommandId aCommand,
            TDesC8* aParams );

        /**
        * @see MVtEngCommandHandler        
        */
        virtual TInt CancelCommand( const TVtEngCommandId aCommandId );

        /**
        * @see MVtEngCommandHandler        
        */
        virtual TInt GetCommandCaps( 
            const TVtEngCommandId aCommand );

        /**
        * @see MVtEngCommandHandler        
        */
        virtual TVtEngCommandId PendingCommand();

        /**
        * @see MVtEngCommandHandler        
        */
        virtual TVtEngCommandId InvalidCommand();
        
        /**
        * @see MVtEngCommandHandler        
        */
        virtual void AddObserverL( MVtEngCommandObserver& aObserver );

        /**
        * @see MVtEngCommandHandler        
        */
        virtual void RemoveObserver( MVtEngCommandObserver& aObserver );

    public: // from MVtEngOperationObserver

        /**
        * Handles command completion.
        * @param aCommand command id
        * @param aResult Symbian OS error code.
        */
        virtual void CommandCompleteL( 
            const TVtEngCommandId aCommand, 
            const TInt aResult );

        /**
        * Sets pending asynch operation
        * @param aActiveOp pending operation or NULL.
        */
        virtual void SetOperation( CVtEngOperation* aActiveOp );

    private: // constructors

        /**
        * Second phase constructor
        */
        void ConstructL();

        /**
        * c++ constructor
        */
        CVtEngCommandHandler( CVtEngHandlerContainer& aHandlers );

    private:

        void AddToPoolL( TVtEngCommandId aCommand );

        /**
        * Validates command
        */
        void ValidateL( 
            const TVtEngCommandId aCommandId,
            TDesC8* aParams );

        /**
        * Check if command is async.        
        */
        void CheckAsynchronity( 
            const TVtEngCommandId aCommandId, 
            TInt& aCaps );

        /**
        * Check if command is supported.
        */
        void CheckSupport( 
            const TVtEngCommandId aCommandId, 
            TInt& aCaps );
            
        /**
        * Check if command is applicable in current state.
        */
        void CheckState( 
            const TVtEngCommandId aCommandId, 
            TInt& aCaps );

    private: // command pool

        struct TVtOpItem
            {
            TVtEngCommandId  iId;
            CVtEngOperation* iOperation;
            };
        /**
        * Command pool for commands instantiated at construction
        * time to guarantee certain operations.
        */
        NONSHARABLE_CLASS( CVtEngCmdPool ): public CArrayPtrFlat<CVtEngOperation>
            {
            public: // constructor and destructor

                // Constructor
                CVtEngCmdPool( TInt aGranularity );

                // Destructor
                ~CVtEngCmdPool();

            public: // data structures
                
                
            public: // new methods

                /**
                * Returns operation stored in the pool.
                * @param aId command id
                * @return operation or NULL if not found.
                */
                CVtEngOperation* Get( const TVtEngCommandId aId );
                
            };

    private: // Data members

        RPointerArray<MVtEngCommandObserver> iObservers;
        CVtEngHandlerContainer&              iHandlers;
        CVtEngOperation*                     iActiveOp;
        CVtEngCmdPool*                       iCommandPool;
        TVtEngCommandId                      iInvalidCommandId;

    };

#endif //CVTENGCOMMANDHANDLER_H

// End of File
