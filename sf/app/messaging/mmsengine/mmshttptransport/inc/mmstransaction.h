/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*     Class implementing transaction within WAP session
*
*/



#ifndef MMSTRANSACTION_H
#define MMSTRANSACTION_H

//  INCLUDES
#include    "mmsoperationtimer.h"
#include    "mmstransportobserver.h"
#include    "mmsbearerstatus.h"

#include    <e32base.h>
#include    <mentact.h>     // CMsgActive
#include    <http/thttpevent.h>
#include    <http/mhttptransactioncallback.h>
#include    <http/mhttpdatasupplier.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MMmsCodecDataSupplier;
class MMmsCodecDataSink;

// CLASS DECLARATION

/**
*  Class implementing transaction within WAP session
*/
NONSHARABLE_CLASS( CMmsTransaction ):public CMsgActive,
    public MMmsTransportObserver,
    public MHTTPTransactionCallback,
    public MHTTPDataSupplier
    {
    public:  // Constructors and destructor
            
        /**
        * Two-phased constructor.
        */
        static CMmsTransaction* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CMmsTransaction();

    public: // New functions
           
        /**
        * Start the state machine.
        * @param aHTTPSession Reference to HTTP session running in client
        * @param aTransferControl Object used to detect if transfer is cancelled
        * @param aUri Address to MMSC with message id 
        * @param aMethod The type of transaction (POST/GET)
        * @param aMessageBuffer An IN/OUT parameter. Includes the data to be sent to
        *        to WAP GW. If there should be data to be returned, this includes it.
        * @param aTransactionTimer Timer for transaction
        * @param aMaxReceiveSize upper limit for accepted message. 0 = anything goes.
        * @param aExpectedReceiveSize tells how large buffer will be allocated for the data
        * @param aDataSupplier The class that will serve as data supplier
        *        for chunked transfer.
        * @param aDataSink The class that will serve as data sink for chunked receive
        * @param aStatus Status of calling active object
        */
        void ExecuteL( 
            RHTTPSession&           aSession,    
            CMmsBearerStatus&       aTransferControl,
            const TDesC&            aUri,
            const TInt              aMethod,
            CBufFlat&               aMessageBuffer,
            const TInt              aTransactionTimer,
            TInt32                  aMaxReceiveSize,
            TInt32                  aExpectedReceiveSize,
            MMmsCodecDataSupplier&  aDataSupplier,
            MMmsCodecDataSink&      aDataSink,
            TRequestStatus&         aStatus );
        

    public: // Functions from base classes

        // from MHTTPTransactionCallback
        /** Called when the filter's registration conditions are satisfied for
        * events that occur on a transaction.
        * Note that this function is not allowed to leave if called with
        * certain events. @see THTTPEvent
        * @param aTransaction The transaction that the event has occurred on.
        * @param aEvent The event that has occurred.
        * @exception Any Leaves must be handled by the appropriate MHFRunError.
        */
        void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent);

        /** Called when RunL leaves from a transaction event. This works in the same
        * way as CActve::RunError; return KErrNone if you have handled the error.
        * If you don't completely handle the error, a panic will occur.
        * @param aError The leave code that RunL left with.
        * @param aTransaction The transaction that was being processed.
        * @param aEvent The Event that was being processed.
        * @return KErrNone if the error has been cancelled or the code
        * of the continuing error otherwise.
        */
        TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent& aEvent);

        // from MHTTPDataSupplier
        /** Obtain a data part from the supplier.  The data is guaranteed
        * to survive until a call is made to ReleaseData().
        * @param aDataPart - the data part
        * @return ETrue if this is the last part. EFalse otherwise
        */
        TBool GetNextDataPart(TPtrC8& aDataPart);

        /** Release the current data part being held at the data
        * supplier.  This call indicates to the supplier that the part
        * is no longer needed, and another one can be supplied, if
        * appropriate.
        */
        void ReleaseData();

        /** Obtain the overall size of the data being supplied, if known
        * to the supplier.  Where a body of data is supplied in several
        * parts this size will be the sum of all the part sizes. If
        * the size is not known, KErrNotFound is returned; in this case
        * the client must use the return code of GetNextDataPart to find
        * out when the data is complete.
        * @return A size in bytes, or KErrNotFound if the size is not known.
        */
        TInt OverallDataSize();

        /** Reset the data supplier.  This indicates to the data supplier that it should
        * return to the first part of the data.  This could be used in a situation where
        * the data consumer has encountered an error and needs the data to be supplied
        * afresh.  Even if the last part has been supplied (i.e. GetNextDataPart has
        * returned ETrue), the data supplier should reset to the first part.
        * If the supplier cannot reset it should return an error code; otherwise it should
        * return KErrNone, where the reset will be assumed to have succeeded
        */
        TInt Reset();

        // from MMmsTransportObserver
        /**
        * Callback function for CMmsOperationtimer to notify
        * about timer expiration
        */
        void TimerExpired();

        /**
        * Callback function for CMmsBearerStatus to notify
        * about gprs suspend
        */
        void GprsSuspended();

        /**
        * Callback function for CMmsBearerStatus to notify
        * about gprs resume
        */
        void GprsResumed();
       
        /**
        * Callback function for CMmsBearerStatus to notify
        * about gprs resume
        */
        void TransferCancelled();

        
    protected:  // New functions
        
    protected:  // Functions from base classes
        
        /**
        * From CActive 
        */
        void DoCancel();

        /**
        * From CMsgActive: Active object completion
        * We must override this because the CMmsOperationTimer
        * and GPRS suspend information is delivered as an error.
        * This class DoRunL is not called if iStatus is error.
        */
        void RunL();

        /**
        * From CMsgActive 
        */
        void DoRunL();

    private:
                     
        /**
        * C++ default constructor.
        */
        CMmsTransaction();
 
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * From CMsgActive: Complete current operation.
        * Cancel all active operations (timer, bearer status)
        * @param aError Error code received by RunL
        */
        void DoComplete( TInt& aError );

        /**
        * Select next state for state machine
        */
        void SelectNextState();

        /**
        * Switch states
        */
        void ChangeStateL();

        /**
        * Send transaction request (POST/GET) to WAP GW
        */
        void MakeTransactionL();

        /**
        * Finalizing procedures
        */
        void FinishL();

        /**
        * Sets a transaction header
        * @param aHeaders the header collection where the new header is inserted
        * @param aHdrField enumeration of the header field
        * @param aHdsValue the string value of the header to be set
        */
        void SetHeaderL( RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8& aHdrValue );

        /**
        * Sets the 'Host' transaction header
        * @param aHeaders the header collection where the new header is inserted
        */
        void SetHostHeaderL( RHTTPHeaders aHeaders );

    public:     // Data
    
    protected:  // Data

    private:    // Data
        
        enum TMmsTransactionStates
            {
            EMmsIdle,
            EMmsSendingTransactionRequest,
            EMmsFinished
            };

        TInt                iState;
        TInt                iError;        
        HBufC*              iUri;                  // URI
        CBufFlat*           iBuffer; 
        TInt                iMethod;
        RHTTPSession*       iHTTPSession;          // transport session
        THTTPEvent          iEvent;
        RHTTPTransaction    iTransaction;          // transaction
        // This is owned by caller
        CMmsOperationTimer* iTimer;
        CMmsBearerStatus*   iTransferControl;
        TInt                iTransactionTimeout;
        TBool               iRetryCount;
        TBool               iRequestOngoing;
        TBool               iSuspendOccurred;
        TBool               iGotBodyData;
        TBool               iDataChunkNumber;
        TInt                iMaxReceiveSize;       // used in case we don't allow big messages
        TInt                iExpectedReceiveSize;  // used for reserving correct size receive buffer
        TBool               iTransactionOpened;    // tells if transaction created successfully
        TBool               iGotResponseHeaders;   // did we get response headers
        // pointer to the class that will serve as data supplier for chunked transfer.
        // pointer is owned by caller - we don't delete it
        MMmsCodecDataSupplier*  iDataSupplier;
        // pointer to the class that will serve as data sink for chunked receive.
        // pointer is owned by caller - we don't delete it
        MMmsCodecDataSink*      iDataSink;
        TBool                iLastChunk; // keep track if we already got last chunk
        TInt                 iCumulativeSize; // the amound of data received or sent so far
        TInt                 iChunkSize; // remember last chunk
        // The size the buffer can be expanded to in chunked mode.
        // When buffer reaches this size the contents must be sent for decoding.
        TInt                 iBufferSize;
        // Position where the data will be written into buffer. There may be some old
        // unhandled data left.
        TInt                 iPosition;


    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };

#endif      // MMSTRANSACTION_H   
            
// End of File 
