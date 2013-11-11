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
* Description:  DTMF state classes.
*
*/



#ifndef CVTENGDTMFSTATE_H
#define CVTENGDTMFSTATE_H

//  INCLUDES
#include    <e32base.h>
#include    <mvtprotocolhandler.h>
#include    "TVtEngDtmfTone.h"
#include    "MVtEngDtmfHandler.h"



// CLASS DECLARATION

/**
*  Base class for DTMF states.
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtEngDtmfState ) : public CBase
    {
    public:
        /**
        * C++ constructor.
        * @param aDtmfHandler 
        * @param aH324Config H324 configure interface 
        */
        CVtEngDtmfState( 
            MVtEngDtmfHandler& aDtmfHandler,
            MVtH324ConfigCommand& aH324Config );

    public: // new methods

        /**
        * Sends a DTMF tone. Derived classes implement this
        * and each vary in behavior.
        * @param aTone tone to send
        */
        virtual void SendDtmfL( const TVtEngDtmfTone& aTone ) = 0;

        /**
        * Handles callback about DTMF sending. Releases pv2way dtmf instance.
        * @param aError Symbian OS error code
        */
        virtual void DtmfSendCompleteL( TInt aError );

    protected: // new methods
        /**
        * Sends tone and changes state
        * @param aDtmf tone to send
        * @param aNewState new DTMF state
        */
        void DoSendAndActivateStateL( 
            const TVtEngDtmfTone& aDtmf,
            MVtEngDtmfHandler::TVtEngDtmfState aNewState );

    protected: // member data

        // DTMF handler
        MVtEngDtmfHandler& iDtmfHandler;

        // H324 configure interface
        MVtH324ConfigCommand& iH324Config;
    };

/**
*  Idle DTMF state class
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtEngDtmfIdle ) : public CVtEngDtmfState
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aDtmfHandler 
        * @param aH324Config H324 configure interface
        */
        static CVtEngDtmfState* NewL( 
            MVtEngDtmfHandler& aDtmfHandler,
            MVtH324ConfigCommand& aH324Config );

        /**
        * Destructor.
        */
        virtual ~CVtEngDtmfIdle();

    public: // From CVtEngStateBase
        
        /**
        * Sends DTMF tone and changes state to "sending".
        * @param aTone tone to send
        */
        virtual void SendDtmfL( const TVtEngDtmfTone& aTone );

        /**
        * @see CVtEngDtmfState
        */
        virtual void DtmfSendCompleteL( TInt aError );

    private: // constructors

        /**
        * C++ constructor.
        * @param aDtmfHandler 
        * @param aH324Config H324 configure interface
        */
        CVtEngDtmfIdle( 
            MVtEngDtmfHandler& aDtmfHandler,
            MVtH324ConfigCommand& aH324Config );
    };

/**
*  Sending DTMF state class
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtEngDtmfSending ) : public CVtEngDtmfState
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aDtmfHandler 
        * @param aH324Config H324 configure interface
        */
        static CVtEngDtmfState* NewL( 
            MVtEngDtmfHandler& aDtmfHandler,
            MVtH324ConfigCommand& aH324Config );

        /**
        * Destructor.
        */
        virtual ~CVtEngDtmfSending();

    public: // From CVtEngStateBase
        
        /**
        * Changes state to "buffered".
        * @param aTone tone to send
        */
        virtual void SendDtmfL( const TVtEngDtmfTone& aTone );

        /**
        * @see CVtEngDtmfState
        */
        virtual void DtmfSendCompleteL( TInt aError );

    private: // constructor

        /**
        * C++ constructor.
        * @param aDtmfHandler 
        * @param aH324Config 
        */
        CVtEngDtmfSending( 
            MVtEngDtmfHandler& aDtmfHandler,
            MVtH324ConfigCommand& aH324Config );

    };

/**
*  Buffered DTMF state class
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtEngDtmfBuffered ) : public CVtEngDtmfState
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aDtmfHandler 
        * @param aH324Config H324 configure interface 
        */
        static CVtEngDtmfState* NewL( 
            MVtEngDtmfHandler& aDtmfHandler,
            MVtH324ConfigCommand& aH324Config );

        /**
        * Destructor.
        */
        virtual ~CVtEngDtmfBuffered();

    public: // From CVtEngStateBase
        
        /**
        * Buffers DTMF tone that is sent when previous
        * tones in buffer are sent.
        * @param aTone tone that is added to the buffer
        */
        virtual void SendDtmfL( const TVtEngDtmfTone& aTone );

        /**
        * @see CVtEngDtmfState
        */
        virtual void DtmfSendCompleteL( TInt aError );

    private: // constructors

        /**
        * C++ constructor.
        * @param aDtmfHandler 
        * @param aH324Config 
        */
        CVtEngDtmfBuffered( 
            MVtEngDtmfHandler& aDtmfHandler,
            MVtH324ConfigCommand& aH324Config );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // Member data

        // Buffered DTMF tones.
        CArrayFixFlat<TVtEngDtmfTone>* iTones;
    };

#endif      // CVTENGDTMFSTATE_H
            
// End of File
