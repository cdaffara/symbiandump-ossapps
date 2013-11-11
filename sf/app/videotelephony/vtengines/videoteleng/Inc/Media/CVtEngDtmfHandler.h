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
* Description:  DTMF handler
*
*/



#ifndef CVTENGDTMFHANDLER_H
#define CVTENGDTMFHANDLER_H

//  INCLUDES
#include <e32std.h>
#include <mvtprotocolhandler.h>
#include "CVtEngDtmfState.h"
#include "CVtEngOperation.h"
#include "MVtEngDtmfHandler.h"

// FORWARD DECLARATIONS
class MVTUserInput;

// CLASS DECLARATION

/**
*  Base class for data types
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtEngDtmfHandler ) : public CBase, public MVtEngDtmfHandler
    {
    public: // Data structures
        enum TUIISupport
            {
            EBasicString    = 2,
            EIA5String      = 4,
            EGeneralString  = 8,
            EDTMFString     = 16
            };

    public: // constructor and destructor

        /**
        * Two-phased constructor.
        * @param aH324Config H324 configure interface
        * @return dtmf handler
        */
        static CVtEngDtmfHandler* NewL( 
            MVtH324ConfigCommand& aH324Config );

        /**
        * Destructor
        */
        ~CVtEngDtmfHandler();

    public: // from MVtEngDtmfHandler

        /**
        * @see MVtEngDtmfHandler
        */
        virtual void HandleL( CVtEngOperation& aOp );

        /**
        * @see MVtEngDtmfHandler
        */
        virtual CVtEngDtmfState& ActivateState( 
            MVtEngDtmfHandler::TVtEngDtmfState aNewState );

        /**
        * @see MVtEngDtmfHandler
        */
        virtual void SendComplete( TInt aError );

        /**
        * @see MVtEngDtmfHandler
        */
        virtual MVTUserInput& ReserveProtoDtmfL(
            const TVtEngDtmfTone& aTone  );

        /**
        * @see MVtEngDtmfHandler
        */
        virtual void ReleaseProtoDtmf();

		/**
        * @see MVtEngDtmfHandler
        */
		virtual TBool CheckCommandId( TInt aCheckCommandId ) const;

		/**
        * @see MVtEngDtmfHandler       
        */
		virtual void StoreCommandId( TInt aStoreCommandId );


    public: // Protocol DTMF Support

        /**
        * Specifies the UII support
        * @param aValue the support to be used
        */
        void SetUIISupport( TUIISupport aValue );

        /**
        * Gets the current UII support
        * @return UII support
        */
        TUIISupport GetUIISupport() const;

    private: // constructors

        /**
        * C++ constructor
        */
        CVtEngDtmfHandler( MVtH324ConfigCommand& aH324Config );

        /**
        * 2nd phase constructor
        */
        void ConstructL();

    private: // new functions

        /**
        * Handles completion of DTMF sending, leavable method.
        */
        void HandleSendCompleteL( TInt aError );

        /**
        * Creates a state instance
        * @param EVtDtmfStateIdle, EVtDtmfStateSending or EVtDtmfStateBuffered
        */
        void CreateStateL( TInt aState );

    private: // member data

        // DTMF states
        CArrayPtrFlat<CVtEngDtmfState>* iDtmfStates;

        // current state index
        TVtEngDtmfState                 iCurrentState;

        // protocol tone
		MVTUserInput*        iProtoDtmf;

        // H324 interface
        MVtH324ConfigCommand&        iH324Config;

        // Indicates supported UII type: 
        // 16 DTMF
        // 8 General String
        // 4 IA5 String
        // 2 Basic string
        TUIISupport iUIISupport;

		// DTMF command ID returned by H324 interface for last tone		
		TInt iIssuedDtmfCommandId;
    };


#endif      CVTENGDTMFHANDLER_H

// End of File
