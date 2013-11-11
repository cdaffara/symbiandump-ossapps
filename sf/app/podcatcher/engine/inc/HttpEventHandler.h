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

#ifndef HTTPEVENTHANDLER_H
#define HTTPEVENTHANDLER_H

#include <e32base.h>
#include <http/mhttpdatasupplier.h>
#include <http/rhttpsession.h>
#include <http/rhttpheaders.h>
#include <http/mhttptransactioncallback.h>
#include <httpstringconstants.h>
#include "HttpClientObserver.h"

const TInt KMaxSubmitSize = 1024;
const TInt KMaxHeaderNameLen = 32;
const TInt KMaxHeaderValueLen = 128;
const TInt KByteOverlap = 4096;

class CHttpEventHandler : public CBase,
                          public MHTTPTransactionCallback
  {
  public:
	virtual ~CHttpEventHandler();
	static CHttpEventHandler* NewLC(CHttpClient* aClient, MHttpClientObserver &aCallbacks, RFs& aFs);
	static CHttpEventHandler* NewL(CHttpClient* aClient, MHttpClientObserver &aCallbacks, RFs& aFs);
	void SetSaveFileName(const TDesC &fName,TBool aContinue=EFalse);
	void CloseSaveFile();	
	// from MHTTPTransactionCallback
	virtual void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent);
	virtual TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent& aEvent);
	void SetSilent(TBool aSilent);

	
protected:
	CHttpEventHandler(CHttpClient* aClient, MHttpClientObserver &aCallbacks, RFs& aFs);
	void ConstructL();
	
private:
	void DumpRespHeadersL(RHTTPTransaction& aTrans);
	void DumpRespBody(RHTTPTransaction& aTrans);
	void DumpIt(const TDesC8& aData);

private:
	TBool iVerbose;
	TBool iFileOpen;
	RFs& iFileServ;
	RFile iRespBodyFile;
	TFileName iFileName;
	TParse iParsedFileName;
	MHTTPDataSupplier* iRespBody;	
	TInt iBytesDownloaded;
	TInt iBytesTotal;
	CHttpClient* iHttpClient;
	MHttpClientObserver& iCallbacks;
	TBool iSilent;
	TBool iContinue;
	TInt iLastStatusCode;
};

#endif
