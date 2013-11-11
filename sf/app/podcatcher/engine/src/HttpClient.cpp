// HttpClient.cpp


#include <e32base.h>
#include <http/rhttpheaders.h>
#include <http.h>
#include <commdb.h>
#include <eikenv.h>
#include <es_sock.h>
#include <bautils.h>
#include <CommDbConnPref.h>
#include "debug.h"
#include "constants.h"
#include "HttpClient.h"
#include "connectionengine.h"
#include "settingsengine.h"
#include "Podcatcher.pan"

const TInt KTempBufferSize = 100;

CHttpClient::~CHttpClient()
  {
  if (iHandler)
  	{
  	iHandler->CloseSaveFile();
	delete iHandler;
  	}
  
  iSession.Close();
  }

CHttpClient* CHttpClient::NewL(CPodcastModel& aPodcastModel, MHttpClientObserver& aObserver)
  {
  CHttpClient* me = NewLC(aPodcastModel, aObserver);
  CleanupStack::Pop(me);
  return me;
  }

CHttpClient::CHttpClient(CPodcastModel& aPodcastModel, MHttpClientObserver& aObserver) : iPodcastModel(aPodcastModel), iObserver(aObserver)
  {
  iResumeEnabled = EFalse;
  }

CHttpClient* CHttpClient::NewLC(CPodcastModel& aPodcastModel, MHttpClientObserver& aObserver)
  {
  CHttpClient* me = new (ELeave) CHttpClient(aPodcastModel, aObserver);
  CleanupStack::PushL(me);
  me->ConstructL();
  return me;
  }

void CHttpClient::ConstructL()
  {
  iPodcastModel.ConnectionEngine().AddObserver(this);
  }

void CHttpClient::SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8& aHdrValue)
	{
	RStringF valStr = iSession.StringPool().OpenFStringL(aHdrValue);
	THTTPHdrVal val(valStr);
	aHeaders.SetFieldL(iSession.StringPool().StringF(aHdrField, RHTTPSession::GetTable()), val);
	valStr.Close();
	}

TBool CHttpClient::IsActive()
	{
	return iIsActive;
	}

void CHttpClient::SetResumeEnabled(TBool aEnabled)
	{
	iResumeEnabled = aEnabled;
	}


void CHttpClient::ConnectHttpSessionL()
{
	DP("ConnectHttpSessionL START");	
	CConnectionEngine::TConnectionState connState = iPodcastModel.ConnectionEngine().ConnectionState();
	if(connState == CConnectionEngine::EConnected)
		{
		DP("ConnectionState == CConnectionEngine::EConnected");
		// Session already connected, call connect complete directly but return status since URLs or so might be faulty
		ConnectCompleteL(KErrNone);		
		return;
		}

	if(connState == CConnectionEngine::ENotConnected)
		{
		DP1("SpecificIAP() == %d",iPodcastModel.SettingsEngine().SpecificIAP());

		if(iPodcastModel.SettingsEngine().SpecificIAP() == -1)
			{
			iPodcastModel.ConnectionEngine().StartL(CConnectionEngine::EUserSelectConnection);	
			}
		else if ( iPodcastModel.SettingsEngine().SpecificIAP() == 0 )
			{
			iPodcastModel.ConnectionEngine().StartL(CConnectionEngine::EDefaultConnection);	
			}
		else if( (iPodcastModel.SettingsEngine().SpecificIAP()&KUseIAPFlag))
			{
			iPodcastModel.ConnectionEngine().StartL(CConnectionEngine::EIAPConnection);
			}
		else
			{
			iPodcastModel.ConnectionEngine().StartL(CConnectionEngine::EMobilityConnection);
			}
		}
	DP("ConnectHttpSessionL END");	
}

void CHttpClient::ConnectCompleteL(TInt aErrorCode)
	{
	if(iWaitingForGet)
		{
		iWaitingForGet = EFalse;
		if( aErrorCode == KErrNone)
			{
			RHTTPConnectionInfo connInfo = iSession.ConnectionInfo();
			RStringPool pool = iSession.StringPool();
			// Attach to socket server
			connInfo.SetPropertyL(pool.StringF(HTTP::EHttpSocketServ, RHTTPSession::GetTable()), THTTPHdrVal(iPodcastModel.ConnectionEngine().SockServ().Handle()));
			// Attach to connection
			TInt connPtr = REINTERPRET_CAST(TInt, &iPodcastModel.ConnectionEngine().Connection());
			connInfo.SetPropertyL(pool.StringF(HTTP::EHttpSocketConnection, RHTTPSession::GetTable()), THTTPHdrVal(connPtr));


			iPodcastModel.SetProxyUsageIfNeededL(iSession);
			DoGetAfterConnectL();		
			}
		else
			{
			ClientRequestCompleteL(KErrCouldNotConnect);
			iSession.Close();			
			}
		}				
	}

void CHttpClient::Disconnected()
	{
	iIsActive = EFalse;
	iSession.Close();
	}

void  CHttpClient::DoGetAfterConnectL()
	{	
	// since nothing should be downloading now. Delete the handler
	if (iHandler)
		{
		delete iHandler;
		iHandler = NULL;
		}
		
	iHandler = CHttpEventHandler::NewL(this, iObserver, iPodcastModel.FsSession());
	iHandler->SetSilent(iSilentGet);

	TEntry entry;
	TBuf8<KTempBufferSize> rangeText;

	if (iResumeEnabled && iPodcastModel.FsSession().Entry(iCurrentFileName, entry) == KErrNone) {
		DP1("Found file, with size=%d", entry.iSize);
		// file exists, so we should probably resume
		rangeText.Format(_L8("bytes=%d-"), (entry.iSize-KByteOverlap > 0 ? entry.iSize-KByteOverlap : 0));
		iHandler->SetSaveFileName(iCurrentFileName, ETrue);
	} else {
		// otherwise just make sure the directory exists
		BaflUtils::EnsurePathExistsL(iPodcastModel.FsSession(),iCurrentFileName);
		iHandler->SetSaveFileName(iCurrentFileName);
	}
	
	RStringPool strP = iSession.StringPool();
	RStringF method;
	method = strP.StringF(HTTP::EGET, RHTTPSession::GetTable());
	
	iTrans = iSession.OpenTransactionL(iUriParser, *iHandler, method);
	RHTTPHeaders hdr = iTrans.Request().GetHeaderCollection();
	// Add headers appropriate to all methods
	SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	SetHeaderL(hdr, HTTP::EAccept, KAccept);
	TBuf<KTempBufferSize> range16;
	range16.Copy(rangeText);
	DP1("range text: %S", &range16);
	if (rangeText.Length() > 0) {
		SetHeaderL(hdr, HTTP::ERange, rangeText);
	}
	iTransactionCount++;
	// submit the transaction
	iTrans.SubmitL();
	iIsActive = ETrue;	
	DP("CHttpClient::Get END");		
	}

TBool CHttpClient::GetL(const TDesC& aUrl, const TDesC& aFileName,  TBool aSilent) {
	DP("CHttpClient::Get START");
	DP2("Getting '%S' to '%S'", &aUrl, &aFileName);
	
	if (iIsActive)
		{
		return EFalse;
		}
	iCurrentURL.Copy(aUrl);	
		
	TInt urlError = iUriParser.Parse(iCurrentURL);

	if(urlError != KErrNone ||!iUriParser.IsSchemeValid())
		{		
		iCurrentURL = KNullDesC8;
		iSession.Close();		
		iObserver.CompleteL(this, KErrHttpInvalidUri);
		return EFalse;		
		}	
	
	iSilentGet = aSilent;
	iCurrentFileName.Copy(aFileName);
	iWaitingForGet = ETrue;
	
	if (iTransactionCount == 0) 
		{
		DP("CHttpClient::GetL\t*** Opening HTTP session ***");
		iSession.Close();
		iSession.OpenL();
		ConnectHttpSessionL();			
		}
	else
		{
		DoGetAfterConnectL();		
		}
	return ETrue;
}

void CHttpClient::Stop()
	{
	iIsActive = EFalse;
	if(iHandler != NULL)
		{
		// cancel the ongoing transaction
		iTrans.Cancel();
		iTransactionCount = 0;
		
		// make sure that we save the file
		iHandler->CloseSaveFile();
		
		// we could now delete the handler since a new will be created
		delete iHandler;
		iHandler = NULL;
		
		// close the session
		DP("CHttpClient::Stop\t*** Closing HTTP session ***");
		iSession.Close();
		}

	TRAP_IGNORE(iObserver.CompleteL(this, KErrDisconnected));

	}

void CHttpClient::ClientRequestCompleteL(TInt aErrorCode) {
	iIsActive = EFalse;
	iObserver.CompleteL(this, aErrorCode);
	DP("CHttpClient::ClientRequestCompleteL");
	if(iTransactionCount>0)
		{
		iTransactionCount--;
	
		if(iTransactionCount == 0) 
			{
			DP("CHttpClient::ClientRequestCompleteL\t*** Closing HTTP session ***");
			delete iHandler;
			iHandler = NULL;
			iSession.Close();
			}
		}
}
