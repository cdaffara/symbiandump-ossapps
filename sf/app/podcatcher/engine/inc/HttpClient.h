/*
* Copyright (c) 2007-2010 Sebastian Brannstrom, Lars Persson, EmbedDev AB
*
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* EmbedDev AB - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#ifndef __HTTPCLIENT_H__
#define __HTTPCLIENT_H__

#include <http/rhttpsession.h>
#include "HttpClientObserver.h"
#include "HttpEventHandler.h"
#include "PodcastModel.h"
#include "connectionengine.h"
#include "es_sock.h"
#include "constants.h"

_LIT8(KUserAgent, "Symbian/Podcatcher");
_LIT8(KAccept, "*/*");

class CHttpClient : public CBase, public MConnectionObserver
{
public:
	virtual ~CHttpClient();
	static CHttpClient* NewL(CPodcastModel& aPodcastModel, MHttpClientObserver& aResObs);
	TBool GetL(const TDesC& aUrl, const TDesC& aFileName, TBool aSilent = EFalse);
	void Stop();
  	TBool IsActive();
	void ClientRequestCompleteL(TInt aErrorCode);
	void SetResumeEnabled(TBool aEnabled);

private:
	CHttpClient(CPodcastModel& aPodcastModel, MHttpClientObserver& aResObs);
	static CHttpClient* NewLC(CPodcastModel& aPodcastModel, MHttpClientObserver& aResObs);
	void ConstructL();
	void SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8& aHdrValue);
	void ConnectHttpSessionL();
	void ConnectCompleteL(TInt aErrorCode);
	void Disconnected();
	void ConnectionSelectionStart() {}
	void ConnectionSelectionEnd() {}
	void DoGetAfterConnectL();
private:
	RHTTPSession iSession;	
	TBool iWaitingForGet;
	TBool iIsActive;
	RHTTPTransaction iTrans;
	CHttpEventHandler* iHandler;
	TBool iResumeEnabled;
	CPodcastModel& iPodcastModel;
	MHttpClientObserver& iObserver;
	TInt iTransactionCount;
	TBuf8<KDefaultURLBufferLength> iCurrentURL;
	TUriParser8 iUriParser;
	TBool iSilentGet;
	TFileName iCurrentFileName;
};
#endif

