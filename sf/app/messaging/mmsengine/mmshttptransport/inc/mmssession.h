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
*     MMS transport session
*
*/



#ifndef MMSSESSION_H
#define MMSSESSION_H

//  INCLUDES
#include <e32base.h>
#include <http/rhttpsession.h>
#include <http/mhttpsessioneventcallback.h>
#include <http/rhttpheaders.h>

// CONSTANTS
_LIT8( KProtocolHTTP, "HTTP/TCP" );
_LIT8( KXProfile, "x-wap-profile");

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CMmsTransaction;
class CMmsBearerStatus;
class RSocketServ;
class RConnection;
class MMmsCodecDataSupplier;
class MMmsCodecDataSink;

// CLASS DECLARATION

/**
* Class for MMS server MTM to send and fetch messages
* @since 2.0
* @lib mmstransport.lib
*/
class CMmsSession : public CActive, public MHTTPSessionEventCallback
    {
    public:  // Constructors and destructor
     
        /**
        * Two-phased constructor.
        * @param aPriority Priority for this active object
        * @param aSocketServ opened socket server session
        * @param aConnection opened connection
        */
        IMPORT_C static CMmsSession* NewL(
            TInt aPriority,
            RSocketServ& aSocketServ,
            RConnection& aConnection );
        
        /**
        * Destructor.
        */
        virtual ~CMmsSession();

    public: // New functions
       
        /**
        * Initializes session headers for HTTP protocol.
        * @param aProxyAddress Proxy address: ip address + port
        * @param aUseProxy is a proxy to be used with this access point
        * @param aMaxReceiveSize maximum size of incoming message that can be accepted
        * @param aMaxSendSize maximum size of message to be sent ( needed for WAP only )
        * @param aStatus Status of the calling active object
        */
        IMPORT_C void OpenL(
            const TDesC8& aProxyAddress,
            const TBool aUseProxy,
            const TInt32 aMaxReceiveSize,
            const TInt32 aMaxSendSize,
            TRequestStatus& aStatus );

        /**
        * Delivers the message to MMSC.
        * @param aUri Uri of the MMSC
        * @param aMessageBuffer In/Out parameter. In: message to posted
        *    Out: response from MMSC
        * @param aDataSupplier The class that will serve as data supplier
        *        for chunked transfer.
        * @param aDataSink The class that will serve as data sink for chunked receive
        * @param aStatus Status of the calling active object
        */
        IMPORT_C void SendMessageL( 
            const TDesC& aUri,    
            CBufFlat& aMessageBuffer, 
            MMmsCodecDataSupplier&  aDataSupplier,
            MMmsCodecDataSink&      aDataSink,
            TRequestStatus& aStatus );

        /**
        * Get message from MMSC.
        * @param aUri URI of the message to fetch
        * @param aMessageBuffer Buffer for the message 
        * @param aDataSupplier The class that will serve as data supplier
        *        for chunked transfer.
        * @param aDataSink The class that will serve as data sink for chunked receive
        * @param aStatus Status of the calling active object
        */
        IMPORT_C void FetchMessageL( 
            const TDesC& aUri,
            CBufFlat& aMessageBuffer,
            TInt32 aExpectedReceiveSize,
            MMmsCodecDataSupplier&  aDataSupplier,
            MMmsCodecDataSink&      aDataSink,
            TRequestStatus& aStatus );

    public: // Functions from base classes

        /**
        * Session event Callback
        * from MHTTPSessionEventCallback
        * Called when the filters registration conditions are satisfied for events that occur
        * on the session.
        * @ param aEvent The session event that has occurred.
        * @ exception Any leaves must be handled by the appropriate MHFRunError.
        */
        void MHFSessionRunL( const THTTPSessionEvent& aEvent );

        /**
        * Session event error Callback
        * from MHTTPSessionEventCallback
        * Called when MHFRunL leaves from a session event. This works in the same
        * way as CActve::RunError
        * @param aError The leave code that RunL left with.
        * @param aEvent The Event that was being processed.
        * @return KErrNone if the error has been cancelled or the code
        * of the continuing error otherwise.    
        */
        TInt MHFSessionRunError( TInt aError, const THTTPSessionEvent& aEvent );

    protected:  // New functions
 
    protected:  // Functions from base classes
        
        /**
        * From CActive 
        */
        virtual void DoCancel();

        /**
        * From CActive 
        */
        virtual void RunL();

    private:

        /**
        * C++ default constructor.
        */
        CMmsSession( TInt aPriority );

        /**
        * By default Symbian OS constructor is private.
        * @param aSocketServ opened socket server session
        * @param aConnection opened connection
        */
        void ConstructL( RSocketServ& aSocketServ, RConnection& aConnection );
        
        // By default, prohibit copy constructor
        CMmsSession( const CMmsSession& );
        // Prohibit assignment operator
        CMmsSession& operator= ( const CMmsSession& );

        /**
        * Set UserAgent string to a fixed value.
        * FOR TESTING PURPOSES ONLY.
        */
        void SetFixedUserAgentL();

        /**
        * Sets a header to be used throughout all transactions
        * @param aHeaders the header collection where the new header is inserted
        * @param aHdrField enumeration of the header field
        * @param aHdsValue the string value of the header to be set
        */
        void SetHeaderL(
            RHTTPHeaders aHeaders,
            TInt aHdrField,
            const TDesC8& aHdrValue );

    public:     // Data
    protected:  // Data
    private:    // Data
        
        enum TSessionStatus
            {
            ESessionIdle = 0,
            ESessionSending,
            ESessionFetching
            };
        
        TBool               iConnected;         // Do we have a connection to WAP GW       
        CMmsTransaction*    iTransaction;       // state machine for making transactions
        TRequestStatus*     iRequestStatus;     // status of the calling program        
        TSessionStatus      iSessionStatus;     // Status of the session object                                  
        RHTTPSession        iHTTPSession;       // transport session
        THTTPSessionEvent   iHTTPEvent;         // event from HTTP
        RHTTPHeaders        iSessionHeaders;    // headers common to all transactions
        TBool               iHeadersCreated;    // if headers have not been created, they cannot be deleted
        TBool               iSessionOpened;     // if HTTP session has not been opened, it cannot be closed
        CMmsBearerStatus*   iTransferControl;
        TInt                iTransactionTimeout;
        TInt                iMaxReceiveSize;
        TInt                iMaxSendSize;
        TInt                iError;
        RSocketServ*        iSocketServ;
        RConnection*        iConnection;
        HBufC8*             iUserAgent;
        HBufC8*             iUaProf;
        // pointer to the class that will serve as data supplier for chunked transfer.
        // pointer is owned by caller - we don't delete it
        MMmsCodecDataSupplier*  iDataSupplier;
        // pointer to the class that will serve as data sink for chunked receive.
        // pointer is owned by caller - we don't delete it
        MMmsCodecDataSink*      iDataSink;

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes
    };

#endif      // MMSSESSION_H   
            
// End of File
