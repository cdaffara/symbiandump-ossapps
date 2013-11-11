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

// HttpEventHandler.cpp
#include <e32debug.h>
#include <httperr.h>
#include "HttpEventHandler.h"
#include "bautils.h"
#include "Httpclient.h"

void CHttpEventHandler::ConstructL()
	{
	//iVerbose = ETrue;	
	}


CHttpEventHandler::CHttpEventHandler(CHttpClient* aClient, MHttpClientObserver &aCallbacks, RFs& aFs): 
	iFileServ(aFs), iHttpClient(aClient), iCallbacks(aCallbacks)
	{
	}


CHttpEventHandler::~CHttpEventHandler()
	{	
	}


CHttpEventHandler* CHttpEventHandler::NewLC(CHttpClient* aClient, MHttpClientObserver &aCallbacks, RFs& aFs)
	{
	CHttpEventHandler* me = new(ELeave)CHttpEventHandler(aClient, aCallbacks, aFs);
	CleanupStack::PushL(me);
	me->ConstructL();
	return me;
	}


CHttpEventHandler* CHttpEventHandler::NewL(CHttpClient* aClient, MHttpClientObserver &aCallbacks, RFs& aFs)
	{
	CHttpEventHandler* me = NewLC(aClient, aCallbacks, aFs);
	CleanupStack::Pop(me);
	return me;
	}

void CHttpEventHandler::MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent)
	{
	switch (aEvent.iStatus)
		{
		case THTTPEvent::EGotResponseHeaders:
			{
			// HTTP response headers have been received. We can determine now if there is
			// going to be a response body to save.
			RHTTPResponse resp = aTransaction.Response();
			iLastStatusCode = resp.StatusCode();
			DP1("Status: %d", iLastStatusCode);

			// Dump the headers if we're being verbose
			//DumpRespHeadersL(aTransaction);

			if (resp.HasBody() && (iLastStatusCode >= 200) && (iLastStatusCode < 300) && (iLastStatusCode != 204))
				{
				TInt dataSize = resp.Body()->OverallDataSize();
				if (dataSize >= 0) {
					DP1("Response body size is %d", dataSize);
					iBytesTotal = dataSize;	
				} else {
					DP("Response body size is unknown");
					iBytesTotal = -1;
				}
				iCallbacks.DownloadInfo(iHttpClient, dataSize);

				}

			DP1("iFileOpen=%d", iFileOpen);
			if (!iFileOpen)
				{
				iFileServ.Parse(iFileName, iParsedFileName);
				TInt valid = iFileServ.IsValidName(iFileName);
				
				if (!valid)
					{
					DP("The specified filename is not valid!.");
					iHttpClient->ClientRequestCompleteL(KErrBadName);
					}
				else
					{
					if (iContinue) {
						TInt err = iRespBodyFile.Open(iFileServ, iParsedFileName.FullName(),EFileWrite);
						if (err)
							{
							DP2("There was an error=%d opening file '%S'", err, &iParsedFileName.FullName());
							iHttpClient->ClientRequestCompleteL(KErrInUse);
							User::Leave(err);
							} 
						else
							{
							iFileOpen = ETrue;
							int pos = -KByteOverlap;
							if((err=iRespBodyFile.Seek(ESeekEnd, pos)) != KErrNone)
								{
								DP("Failed to set position!");
								iHttpClient->ClientRequestCompleteL(KErrWrite);
								User::Leave(err);
								}
							iBytesDownloaded = (pos > 0) ? pos : 0;
							iBytesTotal += iBytesDownloaded;
							DP1("Total bytes is now %u", iBytesTotal);
							DP1("Seeking end: %d", pos);
							}
						}
					else 
						{
						TInt err = iRespBodyFile.Replace(iFileServ,
														 iParsedFileName.FullName(),
														 EFileWrite);
						if (err)
							{
							DP("There was an error replacing file");
							User::Leave(err);
							}
						else
							{
							iFileOpen = ETrue;
							}
						}
					}
				}
			} break;
		case THTTPEvent::EGotResponseBodyData:
			{
			// Get the body data supplier
			iRespBody = aTransaction.Response().Body();

			// Some (more) body data has been received (in the HTTP response)
			//DumpRespBody(aTransaction);
			//DP1("Saving: %d", iSavingResponseBody);
			// Append to the output file if we're saving responses
			if (iFileOpen)
				{
				TPtrC8 bodyData;
				iRespBody->GetNextDataPart(bodyData);
				iBytesDownloaded += bodyData.Length();
				TInt error = iRespBodyFile.Write(bodyData);

				// on writing error we close connection 
				if (error != KErrNone) {
					iFileOpen = EFalse;
					iRespBodyFile.Close();
					iCallbacks.FileError(error);
					iHttpClient->ClientRequestCompleteL(error);
					return;
				}

				if (!iSilent) {
					iCallbacks.Progress(iHttpClient, iBytesDownloaded, iBytesTotal);
				}
				}

			// Done with that bit of body data
			iRespBody->ReleaseData();
			} break;
		case THTTPEvent::EResponseComplete:
			{
			// The transaction's response is complete
			DP("Transaction Complete");
			DP("Closing file");
			iFileOpen = EFalse;
			iRespBodyFile.Close();
			} break;
		case THTTPEvent::ESucceeded:
			{
			DP("Transaction Successful");
			iFileOpen = EFalse;
			iRespBodyFile.Close();
			aTransaction.Close();
			iHttpClient->ClientRequestCompleteL(KErrNone);
			} break;
		case THTTPEvent::EFailed:
			{
			DP("Transaction Failed");
			iFileOpen = EFalse;
			iRespBodyFile.Close();
			aTransaction.Close();
			
			if(iLastStatusCode == HTTPStatus::EOk || iLastStatusCode == HTTPStatus::ECreated || iLastStatusCode == HTTPStatus::EAccepted)
				{
				iLastStatusCode = KErrNone;
				}
			
			iHttpClient->ClientRequestCompleteL(iLastStatusCode);
			} break;
		case THTTPEvent::ERedirectedPermanently:
			{
			DP("Permanent Redirection");
			} break;
		case THTTPEvent::ERedirectedTemporarily:
			{
			DP("Temporary Redirection");
			} break;
		default:
			{
			DP1("<unrecognised event: %d>", aEvent.iStatus);
			// close off the transaction if it's an error
			if (aEvent.iStatus < 0)
				{
				iFileOpen = EFalse;
				iRespBodyFile.Close();
				aTransaction.Close();
				iHttpClient->ClientRequestCompleteL(aEvent.iStatus);
				}
			} break;
		}
	}

TInt CHttpEventHandler::MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent& /*aEvent*/)
	{
	DP1("MHFRunError fired with error code %d", aError);
	aTransaction.Close();
	TRAP_IGNORE(iHttpClient->ClientRequestCompleteL(aError));
	return KErrNone;
	}

void CHttpEventHandler::SetSaveFileName(const TDesC &fName, TBool aContinue)
	{
	DP1("CHttpEventHandler::SetSaveFileName, aContinue=%d", aContinue);
	iFileName.Copy(fName);
	iContinue = aContinue;
	}

void CHttpEventHandler::DumpRespHeadersL(RHTTPTransaction& aTrans)
	{
	RHTTPResponse resp = aTrans.Response();
	RStringPool strP = aTrans.Session().StringPool();
	RHTTPHeaders hdr = resp.GetHeaderCollection();
	THTTPHdrFieldIter it = hdr.Fields();

	TBuf<KMaxHeaderNameLen>  fieldName16;
	TBuf<KMaxHeaderValueLen> fieldVal16;

	while (it.AtEnd() == EFalse)
		{
		RStringTokenF fieldName = it();
		RStringF fieldNameStr = strP.StringF(fieldName);
		THTTPHdrVal fieldVal;
		if (hdr.GetField(fieldNameStr,0,fieldVal) == KErrNone)
			{
			const TDesC8& fieldNameDesC = fieldNameStr.DesC();
			fieldName16.Copy(fieldNameDesC.Left(KMaxHeaderNameLen));
			switch (fieldVal.Type())
				{
			case THTTPHdrVal::KTIntVal:
				DP2("%S: %d", &fieldName16, fieldVal.Int());
				break;
			case THTTPHdrVal::KStrFVal:
				{
				RStringF fieldValStr = strP.StringF(fieldVal.StrF());
				const TDesC8& fieldValDesC = fieldValStr.DesC();
				fieldVal16.Copy(fieldValDesC.Left(KMaxHeaderValueLen));
				DP2("%S: %S", &fieldName16, &fieldVal16);
				}
				break;
			case THTTPHdrVal::KStrVal:
				{
				RString fieldValStr = strP.String(fieldVal.Str());
				const TDesC8& fieldValDesC = fieldValStr.DesC();
				fieldVal16.Copy(fieldValDesC.Left(KMaxHeaderValueLen));
				DP2("%S: %S", &fieldName16, &fieldVal16);
				}
				break;
			case THTTPHdrVal::KDateVal:
				{
				TDateTime date = fieldVal.DateTime();
				} 
				break;
			default:
				DP1("%S: <unrecognised value type>", &fieldName16);
				break;
				}

			// Display realm for WWW-Authenticate header
			RStringF wwwAuth = strP.StringF(HTTP::EWWWAuthenticate,RHTTPSession::GetTable());
			if (fieldNameStr == wwwAuth)
				{
				// check the auth scheme is 'basic'
				RStringF basic = strP.StringF(HTTP::EBasic,RHTTPSession::GetTable());
				RStringF realm = strP.StringF(HTTP::ERealm,RHTTPSession::GetTable());
				THTTPHdrVal realmVal;
				if ((fieldVal.StrF() == basic) && 
					(!hdr.GetParam(wwwAuth, realm, realmVal)))
					{
					RStringF realmValStr = strP.StringF(realmVal.StrF());
					fieldVal16.Copy(realmValStr.DesC());
					DP1("Realm is: %S", &fieldVal16);
					}
				}
			}
		++it;
		}
	}

void CHttpEventHandler::DumpRespBody(RHTTPTransaction& aTrans)
	{
	MHTTPDataSupplier* body = aTrans.Response().Body();
	TPtrC8 dataChunk;
	TBool isLast = body->GetNextDataPart(dataChunk);
	DumpIt(dataChunk);
	if (isLast)
		DP("Got last data chunk.");
	}


void CHttpEventHandler::DumpIt(const TDesC8& aData)
//Do a formatted dump of binary data
	{
	// Iterate the supplied block of data in blocks of cols=80 bytes
	const TInt cols=16;
	TInt pos = 0;
	TBuf<KMaxFileName - 2> logLine;
	TBuf<KMaxFileName - 2> anEntry;
	const TInt dataLength = aData.Length();

	while (pos < dataLength)
		{
		//start-line hexadecimal( a 4 digit number)
		anEntry.Format(TRefByValue<const TDesC>_L("%04x : "), pos);
		logLine.Append(anEntry);

		// Hex output
		TInt offset;
		for (offset = 0; offset < cols; ++offset)
			{
			if (pos + offset < aData.Length())
				{
				TInt nextByte = aData[pos + offset];
				anEntry.Format(TRefByValue<const TDesC>_L("%02x "), nextByte);
				logLine.Append(anEntry);
				}
			else
				{
				//fill the remaining spaces with blanks untill the cols-th Hex number 
				anEntry.Format(TRefByValue<const TDesC>_L("   "));
				logLine.Append(anEntry);
				}
			}
			anEntry.Format(TRefByValue<const TDesC>_L(": "));
			logLine.Append(anEntry);

		// Char output
		for (offset = 0; offset < cols; ++offset)
			{
			if (pos + offset < aData.Length())
				{
				TInt nextByte = aData[pos + offset];
				if ((nextByte >= ' ') && (nextByte <= '~'))
					{
					anEntry.Format(TRefByValue<const TDesC>_L("%c"), nextByte);
					logLine.Append(anEntry);
					}
				else
					{
					anEntry.Format(TRefByValue<const TDesC>_L("."));
					logLine.Append(anEntry);
					}
				}
			else
				{
				anEntry.Format(TRefByValue<const TDesC>_L(" "));
				logLine.Append(anEntry);
				}
			}
			logLine.Zero();

		// Advance to next  byte segment (1 seg= cols)
		pos += cols;
		}
	}

void CHttpEventHandler::SetSilent(TBool aSilent)
	{
	iSilent = aSilent;
	}

void CHttpEventHandler::CloseSaveFile()
{
	if(iRespBody != NULL)
	{		
		if(iRespBodyFile.SubSessionHandle() != 0)
			{
			TInt size;
			iRespBodyFile.Size(size);
			DP2("Closing file at size %d, bytes downloaded %d", size, iBytesDownloaded);
			iRespBodyFile.Close();
			}
	}
}

