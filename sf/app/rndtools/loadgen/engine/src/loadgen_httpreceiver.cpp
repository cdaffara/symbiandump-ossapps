
#include <commdb.h>
#include <commdbconnpref.h>
#include "loadgen_httpreceiver.h"

// CONSTANTS
_LIT8( KHttpScheme, "http" );
_LIT8( KSchemeAddon, "://" );
// This client accepts all content types.
_LIT8(KAccept, "*/*");

CHTTPReceiver* CHTTPReceiver::NewL( MHTTPRecvObserver& aObserver )
	{
	CHTTPReceiver* self = CHTTPReceiver::NewLC( aObserver );
	CleanupStack::Pop( self );
	return self;
	}

CHTTPReceiver* CHTTPReceiver::NewLC( MHTTPRecvObserver& aObserver )
	{
	CHTTPReceiver* self = new (ELeave)CHTTPReceiver( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

CHTTPReceiver::CHTTPReceiver( MHTTPRecvObserver& aObserver ) :
	iObserver(aObserver), iRunning(EFalse), iConnectionSetupDone(EFalse)
	{
	}

void CHTTPReceiver::ConstructL()
	{
#ifdef _DEBUG
    User::LeaveIfError( iFs.Connect() );
#endif
	}

CHTTPReceiver::~CHTTPReceiver()
	{
	TRACE( "Entry: CHTTPReceiver::~CHTTPReceiver" );

	CancelTransaction();

	TRAP_IGNORE( iSession.DisconnectL() );
	iSession.Close();
	iConnection.Stop();
	iConnection.Close();
	iSocketServ.Close();
    
	TRACE( "Entry: CHTTPReceiver::~CHTTPReceiver - delete iUrl" );
	delete iUrl;

#ifdef _DEBUG
	iFs.Close();
#endif
	TRACE( "Exit: CHTTPReceiver::~CHTTPReceiver" );
	}

void CHTTPReceiver::CancelTransaction()
	{
	if ( !iRunning)
		return;

	TRACE( "Entry: CHTTPReceiver::CancelTransaction" );

	if ( iRunning)
		{
		iTransaction.Close();
		iRunning = EFalse;
		}

	TRACE( "Exit: CHTTPReceiver::CancelTransaction" );
	}

// ----------------------------------------------------------------------------
// CHTTPReceiver::SendHTTPGetL()
//
// Start a new HTTP GET transaction.
// ----------------------------------------------------------------------------
void CHTTPReceiver::SendHTTPGetL( const TDesC8& aURL )
	{
	TRACE( "Entry: CHTTPReceiver::SendHTTPGetL" );
	
	iResponseStatus = KErrGeneral;
	
#ifdef _DEBUG
    iResponseFile.Replace( iFs, _L("C:\\Data\\loadgen.htm"), EFileWrite );
#endif
    
	SetupConnectionL();	
	
	// Parse string to URI (as defined in RFC2396)
	TUriParser8 uri;
	
	CheckForHTTPSchemeL(uri, aURL);

	RStringF method = iSession.StringPool().StringF(HTTP::EGET, RHTTPSession::GetTable());
	CleanupClosePushL( method );

	iTransaction = iSession.OpenTransactionL(uri, *this, method);

	// Set headers for request: accepted content type
	RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
	SetHeaderL(hdr, HTTP::EAccept, KAccept);

	iTransaction.SubmitL();

	iRunning = ETrue;
	CleanupStack::PopAndDestroy();
	TRACE( "Exit: CHTTPReceiver::SendHTTPGetL" );
	return;
	}

void CHTTPReceiver::CheckForHTTPSchemeL(TUriParser8& aUri, const TDesC8& aURL)
	{
	if( iUrl )
	    {
	    delete iUrl;
	    iUrl = NULL;
	    }
	
	iUrl = aURL.AllocL();
	aUri.Parse( *iUrl );

	TPtrC8 scheme( aUri.Extract (EUriScheme) );

	// unsupported or no scheme in url.
	// Insert 'http://' to the beginning of it.
	if ( scheme != KHttpScheme )
		{
		HBufC8* tempBuf = HBufC8::NewL( KHttpScheme().Length() + KSchemeAddon().Length() + aURL.Length() );
		CleanupStack::PushL( tempBuf );
		tempBuf->Des().Append( KHttpScheme );
		tempBuf->Des().Append( KSchemeAddon );
		tempBuf->Des().Append( aURL );

		if( iUrl )
		    {
            delete iUrl;
            iUrl = NULL;
		    }
		iUrl = tempBuf;
		aUri.Parse( *iUrl );
		CleanupStack::PopAndDestroy( tempBuf );
		}
	}

// Used to set header value to HTTP request
void CHTTPReceiver::SetHeaderL (RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8& aHdrValue)
	{
	RStringF valStr = iSession.StringPool().OpenFStringL (aHdrValue);
	CleanupClosePushL (valStr);
	THTTPHdrVal val(valStr);
	aHeaders.SetFieldL (iSession.StringPool().StringF (aHdrField, RHTTPSession::GetTable ()), val);
	CleanupStack::PopAndDestroy (); // valStr
	}

// Inherited from MHTTPTransactionCallback
// Called by framework to pass transaction events.
// ----------------------------------------------------------------------------
void CHTTPReceiver::MHFRunL( RHTTPTransaction aTransaction,	const THTTPEvent& aEvent )
	{
	TRACE( "Entry: CHTTPReceiver::MHFRunL" );
	TRACE2( "Entry: CHTTPReceiver::MHFRunL <%d>", aEvent.iStatus );

	// state check
	if ( !iRunning )
		{
		TRACE( "Exit1: CHTTPReceiver::MHFRunL: recv in wrong state" );
		return;
		}

	switch ( aEvent.iStatus )
		{
	case THTTPEvent::EGotResponseHeaders:
		{
		TRACE( "Entry: CHTTPReceiver::MHFRunL EGotResponseHeaders" );
		// Get HTTP status code from header (e.g. 200)
		RHTTPResponse resp = aTransaction.Response();
		iResponseStatus = resp.StatusCode();
		}
		break;

	case THTTPEvent::EGotResponseBodyData:
		{
		// Get the body data supplier
		TRACE( "Entry: CHTTPReceiver::MHFRunL EGotResponseBodyData" );
		MHTTPDataSupplier* body = aTransaction.Response().Body ();
		TPtrC8 dataChunk;
		body->GetNextDataPart ( dataChunk );
#ifdef _DEBUG
		if ( dataChunk.Length() )
		    {
			iResponseFile.Write( dataChunk );
		    }
#endif
		body->ReleaseData ();
		}
		break;

	case THTTPEvent::EResponseComplete:
		{
		// Indicates that header & body of response is completely received.
		// Notify Observer
		TRACE( "Entry: CHTTPReceiver::MHFRunL EResponseComplete" );
		}
		break;

	case THTTPEvent::ESucceeded:
		{
		// Transaction can be closed now. It's not needed anymore.
		TRACE( "Entry: CHTTPReceiver::MHFRunL ESucceeded" );
		Finalize();
		}
		break;

	case THTTPEvent::EFailed:
		{
		TRACE( "Entry: CHTTPReceiver::MHFRunL EFailed" );
		iResponseStatus = KErrGeneral;
		Finalize();
		}
		break;

	default:
		TRACE( "Entry: CHTTPReceiver::MHFRunL EDefault" );
		break;
		}
	}

// Called by framework when *leave* occurs in handling of transaction event.
// These errors must be handled, or otherwise HTTP-CORE 6 panic is thrown.
// ----------------------------------------------------------------------------
TInt CHTTPReceiver::MHFRunError ( TInt aError, RHTTPTransaction /*aTransaction*/, const THTTPEvent& /*aEvent*/ )
	{
	TRACE( "Entry: CHTTPReceiver::MHFRunError" );
	iResponseStatus = aError;
	Finalize();

	TRACE( "Exit: CHTTPReceiver::MHFRunError" );
	return KErrNone;
	}

void CHTTPReceiver::SetupConnectionL ()
	{
	TRACE( "Entry: CHTTPReceiver::StartConnectionL" );

	// check if conncetion is already open
	if ( iRunning )
		{
		return;
		}

	if ( iConnectionSetupDone )
	    {
		return;
	    }

	iConnectionSetupDone = ETrue;
	
	// open HTTP session
	iSession.OpenL();

	// open socket server
	TInt result = iSocketServ.Connect();
	if ( result == KErrNone)
		{
		// open connection
		result = iConnection.Open(iSocketServ);
		if ( result == KErrNone)
			{
			// set overrides
			TCommDbConnPref pref;
			pref.SetDialogPreference (ECommDbDialogPrefPrompt);
			//pref.SetDirection (ECommDbConnectionDirectionOutgoing);
			//pref.SetIapId (accessPoint);

			// start with overrides
			result = iConnection.Start(pref);

			if ( result == KErrNone)
				{
				// get connection info from iSession
				RHTTPConnectionInfo connInfo = iSession.ConnectionInfo ();
				RStringPool pool = iSession.StringPool ();

				// set socket server
				connInfo.SetPropertyL (pool.StringF (HTTP::EHttpSocketServ,
						RHTTPSession::GetTable () ),
						THTTPHdrVal( iSocketServ.Handle () ) );

				// attach to connection
				TInt connectionPtr= REINTERPRET_CAST( TInt,
						&iConnection );

				connInfo.SetPropertyL (pool.StringF (
						HTTP::EHttpSocketConnection,
						RHTTPSession::GetTable () ),
						THTTPHdrVal (connectionPtr) );

#if defined(__WINSCW__) || defined(__WINS__) // if Emulator
				_LIT8(KProxyAddr, "192.168.0.252:4040");
				TBufC8<30> proxyAddr(KProxyAddr);

				RStringF prxAddr = iSession.StringPool().OpenFStringL (proxyAddr);
				CleanupClosePushL (prxAddr);
				THTTPHdrVal prxUsage(iSession.StringPool().StringF (HTTP::EUseProxy,
						RHTTPSession::GetTable ()));
				iSession.ConnectionInfo().SetPropertyL (iSession.StringPool().StringF (HTTP::EProxyUsage,
						RHTTPSession::GetTable ()), prxUsage);
				iSession.ConnectionInfo().SetPropertyL (iSession.StringPool().StringF (HTTP::EProxyAddress,
						RHTTPSession::GetTable ()), prxAddr);
				CleanupStack::PopAndDestroy (); // prxAddr	
#endif 

				}
			else
				{
				TRACE2( "CHTTPReceiver:: connection start: <%d>", result );
				}
			}
		else
			{
			TRACE2( "CHTTPReceiver:: connection open: <%d>", result );
			}
		}
	else
		{
		TRACE2( "CHTTPReceiver:: connection to socket server: <%d>", result );		
		}

	User::LeaveIfError( result );
	
	TRACE( "Exit: CHTTPReceiver::StartConnectionL" );
	}

// ----------------------------------------------------------------------------
// CHTTPReceiver::Finalize
// 
// ----------------------------------------------------------------------------
//
void CHTTPReceiver::Finalize()
    {
    TRACE( "Exit: CHTTPReceiver::Finalize" );
#ifdef _DEBUG
    iResponseFile.Close();
#endif
    iTransaction.Close();
    iRunning = EFalse;
    iObserver.HTTPFileReceived( iResponseStatus );
    TRACE( "Exit: CHTTPReceiver::Finalize" );
    }
