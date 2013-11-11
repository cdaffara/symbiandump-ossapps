

#ifndef CHTTPReceiver_H
#define CHTTPReceiver_H

#include <http.h>
#include <mhttpdatasupplier.h>
#include <mhttptransactioncallback.h>
#include <Es_sock.h>
#include <chttpformencoder.h> 

// CONSTANTS
// None.

// MACROS
#ifdef _DEBUG
#define TRACE( text )         RDebug::Printf( text );
#define TRACE2( text, arg )   RDebug::Printf( text, arg );
#else
#define TRACE( text )
#define TRACE2( text, arg )
#endif

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
// None.

class MHTTPRecvObserver
    {
    public:
       
        /**
        * Callback for HTTP response received.
        * @param aStatus Status code.
        * @return None.
        */
        virtual void HTTPFileReceived( TInt aStatus ) = 0;        
    };

// CLASS DECLARATION

class CHTTPReceiver
        : public CBase,
          public MHTTPTransactionCallback
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CHTTPReceiver* NewL( MHTTPRecvObserver& aObserver );
        
        /**
        * Two-phased constructor.
        */
        static CHTTPReceiver* NewLC( MHTTPRecvObserver& aObserver );        
        
        /**
        * Destructor.
        */        
        virtual ~CHTTPReceiver();
      
        /**
        * Cancels the current HTTP transaction.
        */   
        void CancelTransaction();

        /**
        * Opens a connection to destination aUri and sends data using HTTP GET. 
        * Callbacks via MHTTPRecvObserver::HTTPFileReceived
        * Leaves with KErrNotReady If network is not available
        * @param aUri Destination URL, if http scheme not specified, prepends scheme.       
        */                                 
        void SendHTTPGetL( const TDesC8& aUri );
        
    protected: // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CHTTPReceiver( MHTTPRecvObserver& aObserver );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();
      
        /**
        * Sets HTTP header fields.
        * @param aHeaders HTTP headers.
        * @param aHdrField Header field to set.
        * @param aHdrValue Header field value.
        */   
        void SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField,
						const TDesC8& aHdrValue);

		//From MHTTPTransactionCallback
        /**
        * Called by HTTP framework when HTTP events occur.
        * @param aTransaction The transaction that the event has occurred on.
        * @param aEvent The event that has occurred.
        */           
		void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent);
		
        /**
        * Called when RunL leaves from a transaction event.
        * @param aError The leave code that RunL left with.
        * @param aTransaction The transaction that was being processed.
        * @param aEvent The event that was being processed.
        * @return KErrNone.
        */   
		TInt MHFRunError(	TInt aError,
						RHTTPTransaction aTransaction,
						const THTTPEvent& aEvent);
						
		/**
        * Starts HTTP connection.
        */   
       	void SetupConnectionL();
        
        /**
        * Utility:Checks for http uri scheme and prepends if not exists to the url
        * @param aUri UriParser.
        * @param aURL URL to check for.
        * @return None 
        */
        void CheckForHTTPSchemeL(TUriParser8& aUri, const TDesC8& aURL);

        /**
        * Utility: finish receiving, call calback HTTPFileReceived()
        * @param aError to be signaled
        * @return None 
        */
        void Finalize();
        
    private:
      	
        // Request uri, might need to prepend http scheme.
        HBufC8*					iUrl;
        
        // RSocketServ object.
	    RSocketServ 			iSocketServ;
        
        // RConnection object.
	    RConnection 			iConnection;
	    
	    // HTTP session.
		RHTTPSession			iSession;
		
		// HTTP transaction used for request.
		RHTTPTransaction		iTransaction;
	
		// Observer to notify.
		MHTTPRecvObserver&	    iObserver;	

		// ETrue if transaction in progress.
		TBool					iRunning;	
		
		// ETrue if connection set up done.
		TBool 					iConnectionSetupDone;                        
		
		// Response Status 
		TInt                    iResponseStatus;
		
#ifdef _DEBUG
		// Http response body written to file
		RFile 					iResponseFile;
		
		// RFs to be used for writing to iResponseFile
		RFs                     iFs;
#endif
		
};
    
#endif

// CHTTPReceiver_H
// End of File	
