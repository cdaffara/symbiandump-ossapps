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
* Description:  Operation base class.
*
*/



#ifndef CVTENGOPERATION_H
#define CVTENGOPERATION_H

//  INCLUDES
#include <e32base.h>
#include "vtengcommands.h"

// FORWARD DECLARATIONS
class CVtEngCommandHandler;
class CVtEngHandlerContainer;
class MVtEngOperationObserver;

/**
*  Operation base class.
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtEngOperation ) : public CBase
    {
    public: // constructor and destructors

        /**
        * two-phase constructor
        * @param aCommandId command id
        * @param aHandlers handler container
        * @param aObserver observer called back when operation is completed
        * @param aDelete if ETrue command deletes itself after completion
        */
        static CVtEngOperation* NewL(
            TVtEngCommandId aCommandId,
            CVtEngHandlerContainer& aHandlers,
            MVtEngOperationObserver& aObserver,
            TBool aDelete = ETrue );

        /**
        * Destructor
        */
        ~CVtEngOperation( );

    public: // new functions

        /**
        * Performs operations.
        * @return ETrue is operation is complete (i.e. sync).
        */
        virtual TBool ExecuteL( TDesC8* aParams );

        /**
        * Cancels pending command.
        * @return ETrue if cancelled was succesfull or there
        * is nothing to cancel.
        */
        TBool Cancel();

        /**
        * Returns command identifier.
        * @return command identifier
        */
        inline TVtEngCommandId Command() const;

        /**
        * Returns command parameter buffer.
        * @return parameter buffer
        */
        inline const TDesC8* Parameters() const;

        /**
        * Notifies observer on completion.
        * @param aResult Symbian OS error code
        */
        virtual void HandleOpComplete( const TInt aResult );

    protected: // new functions

        /**
        *
        */
        TBool OfferExecuteSyncL( TDesC8* aParams );

    protected:
        /**
        * c++ constructor
        */
        CVtEngOperation(
            TVtEngCommandId aCommandId,
            CVtEngHandlerContainer& aHandlers,
            MVtEngOperationObserver& aCommandHandler,
            TBool aDelete );
	
		/**
        * Handles asynchronous operation complete
        */
    	static TInt ASyncHandleOpComplete( TAny* aPtr );

    protected:

        // Command identifier
        const TVtEngCommandId   iCommand;

        // Handler container
        CVtEngHandlerContainer& iHandlers;

        // observer for command response
        MVtEngOperationObserver&   iObserver;

        // Parameter
        TDesC8*                 iParams;

        // Delete command when complete
        TBool                   iDelete;
		
		// Pointer to asynchronous callback
        CAsyncCallBack*         iAsyncCallback;

		// Asynchronous callback error identifier
        TInt                    iAsyncErr;
    };

/**
* Utility class for unpacking data type with descriptor data.
* @lib videoteleng
* @since 2.6
*/
template <class T>
class TVtEngOpParamUtil
    {
    public:
        /**
        * Sets parameter in aDes to aRef.
        */
        inline void static Set( T& aRef , const CVtEngOperation& aOp );
    };

#include    "CVtEngOperation.inl"

#endif      // CVTENGOPERATION_H

// End of File
