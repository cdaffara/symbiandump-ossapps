/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: HttpHandler is used for HTTP connection components testing
*
*/

// INCLUDE FILES
#include <commdb.h>

#include "HttpHandler.h"
#include "uinotify.h"
#include "SettingData.h"
#include "Utils.h"
#include "ConnTest.pan"
#include "conntest.hrh"

// CONSTANTS

// Size of buffer used when submitting request bodies
//const TInt KMaxSubmitSize = 2048;
const TInt KMaxHeaderNameLen = 32;
const TInt KMaxHeaderValueLen = 128;
const TInt KMaxStatusStrLen = 32;


// ================= MEMBER FUNCTIONS =======================



// Standard headers used by default
_LIT8(KUserAgent, "User-Agent: Nokia6600/1.0");
_LIT8(KAccept, "*/*");


enum THttpExampleClientPanics
{
    EReqBodySumitBufferNotAllocated,
        KBodyWithInvalidSize,
        KCouldntNotifyBodyDataPart
};



//============================================================
//
// Implementation of CHttpClient
//
//============================================================



// ----------------------------------------------------------------------------
// CHttpClient::CHttpClient(MUINotify& aConsole)
// Constructor
// ----------------------------------------------------------------------------
//
CHttpClient::CHttpClient(MUINotify& aConsole)
: iReqBodySubmitBufferPtr(0,0), iConsole(aConsole)
{
}


// ----------------------------------------------------------------------------
// CHttpClient::~CHttpClient()
// Destructor
// ----------------------------------------------------------------------------
//
CHttpClient::~CHttpClient()
{
    delete iReqBodySubmitBuffer;
    delete iTransObs;
    iHttpSession.Close();
}


// ----------------------------------------------------------------------------
// CHttpClient::NewLC(MUINotify& aConsole)
// Two-phase constructor
// ----------------------------------------------------------------------------
//
CHttpClient* CHttpClient::NewLC(MUINotify& aConsole)
{
    CHttpClient* me = new(ELeave) CHttpClient(aConsole);
    CleanupStack::PushL(me);
    me->ConstructL();
    return me;
}

// ----------------------------------------------------------------------------
// CHttpClient::NewL(MUINotify& aConsole)
// Two-phase constructor
// ----------------------------------------------------------------------------
//
CHttpClient* CHttpClient::NewL(MUINotify& aConsole)
{
    CHttpClient* me = NewLC(aConsole);
    CleanupStack::Pop(me);
    return me;
}

// ----------------------------------------------------------------------------
// CHttpClient::ConstructL()
// EPOC two-phased constructor
// ----------------------------------------------------------------------------
//
void CHttpClient::ConstructL()
{
    iHttpSession.OpenL();
    
    // Install this class as the callback for authentication requests
    InstallAuthenticationL(iHttpSession);
    
    iTransObs = CHttpEventHandler::NewL(iConsole);
}


// ----------------------------------------------------------------------------
// CHttpClient::SetHttpConnectionInfo()
// Set RConnection and RSocketServ as session properties, if
// http fw does not use it's own connection.
// ----------------------------------------------------------------------------
//
void CHttpClient::SetHttpConnectionInfoL( TBool aUseOwnConnection, 
                                          RConnection& aConnection, 
                                          RSocketServ& aSocketServ )
{
    TInt result;
    TBuf<16> serviceType;
    TUint32 serviceId;
    TBuf<100> query;
    TBuf<100> proxyAddr;
    TBuf8<100> proxyAddr2;
    TUint32 proxyPort;
    TBool useProxy;
    TUint connCount;
    CCommsDatabase* TheDb;
    RStringF proxyName;
    
    // Trick to get new values into use    
    iHttpSession.Close();
    iHttpSession.OpenL();
    
    RStringPool strPool = iHttpSession.StringPool();
    
    // Remove first session properties just in case.
    RHTTPConnectionInfo connInfo = iHttpSession.ConnectionInfo();
    
    // Clear RConnection and Socket Server instances
    connInfo.RemoveProperty(strPool.StringF(HTTP::EHttpSocketServ,RHTTPSession::GetTable()));
    connInfo.RemoveProperty(strPool.StringF(HTTP::EHttpSocketConnection,RHTTPSession::GetTable()));
    
    // Clear the proxy settings
    THTTPHdrVal proxyUsage(strPool.StringF(HTTP::EUseProxy,RHTTPSession::GetTable()));
    connInfo.RemoveProperty(strPool.StringF(HTTP::EProxyUsage,RHTTPSession::GetTable()));
    connInfo.RemoveProperty(strPool.StringF(HTTP::EProxyAddress,RHTTPSession::GetTable()));
    
    if(!aUseOwnConnection)
    {
        // RConnection has been started, set proxy (if defined) and RConnection and
        // Socket Server session properties.
        
        // Proxy
        result = aConnection.EnumerateConnections(connCount);
        User::LeaveIfError(result);
        
        //
        // Get service and service type for this connection
        //
        query.Format(_L("%s\\%s"), IAP, IAP_SERVICE);
        result = aConnection.GetIntSetting(query, serviceId);
        
        query.Format(_L("%s\\%s"), IAP, IAP_SERVICE_TYPE);
        result = aConnection.GetDesSetting(query, serviceType);
        User::LeaveIfError(result);
        
        TheDb = CCommsDatabase::NewL();
        CleanupStack::PushL(TheDb);
        
        CCommsDbTableView* view = TheDb->OpenViewOnProxyRecordLC(serviceId, serviceType);
        result = view->GotoFirstRecord();
        
        if(result == KErrNone)
        {
            // Check whether proxy should be used for this IAP
            TRAPD(proxyErr, view->ReadBoolL(TPtrC(PROXY_USE_PROXY_SERVER), useProxy));
            if((proxyErr == KErrNone) && useProxy)
            {
                // This IAP uses proxy, set it to http session
                view->ReadUintL(TPtrC(PROXY_PORT_NUMBER), proxyPort);
                HBufC* k = view->ReadLongTextLC(TPtrC(PROXY_SERVER_NAME));
                proxyAddr.Copy(k->Des());
                proxyAddr.AppendFormat(_L(":%d"), proxyPort);
                
                proxyAddr2.Copy(proxyAddr);
                
                CleanupClosePushL(proxyName);
                proxyName = iHttpSession.StringPool().OpenFStringL(proxyAddr2);
                connInfo.SetPropertyL( strPool.StringF(HTTP::EProxyUsage,RHTTPSession::GetTable()), 
                                       proxyUsage );
                connInfo.SetPropertyL( strPool.StringF(HTTP::EProxyAddress,RHTTPSession::GetTable()), 
                                       proxyName );
                CleanupStack::PopAndDestroy(&proxyName); // proxyName
                CleanupStack::PopAndDestroy(k); //k
                
                RDebug::Print(_L("ConnTest: Proxy address: %S"), &proxyAddr);
            }
        }
        CleanupStack::PopAndDestroy(view); // view
        CleanupStack::PopAndDestroy(TheDb); // TheDb
        
        // RConnection and Socket Server
        connInfo.SetPropertyL ( 
            strPool.StringF(HTTP::EHttpSocketServ, RHTTPSession::GetTable()), 
            THTTPHdrVal (aSocketServ.Handle()) );
        
        TInt connPtr1 = REINTERPRET_CAST(TInt, &aConnection);
        connInfo.SetPropertyL ( 
            strPool.StringF(HTTP::EHttpSocketConnection, 
            RHTTPSession::GetTable() ), THTTPHdrVal (connPtr1) );    
        
    }
}


// ----------------------------------------------------------------------------
// CHttpClient::InvokeHttpMethodL()
// Create the transaction, set the headers and body and start the transaction
// ----------------------------------------------------------------------------
//
void CHttpClient::InvokeHttpMethodL(const CSettingData* aData, TBool aHasBody, TBool aIsSecure)
{
    
    iSettingData = (CSettingData*)aData;
    
    RStringPool strPool = iHttpSession.StringPool();
    RStringF method = strPool.StringF(HTTP::EGET,RHTTPSession::GetTable());
    
    if(aHasBody)
    {
        method = strPool.StringF(HTTP::EPOST,RHTTPSession::GetTable());
        
        delete iReqBodySubmitBuffer;
        iReqBodySubmitBuffer = NULL;
        iReqBodySubmitBuffer = HBufC8::NewMaxL(KSendDataSize);
        iReqBodySubmitBufferPtr.Set(iReqBodySubmitBuffer->Des());
                
        // Create body chunk
        Utils::CreateDataChunk(iReqBodySubmitBufferPtr, aData->iPacketSize);
        iDataChunkCount = 0;
    }
    else
    {
        method = strPool.StringF(HTTP::EGET,RHTTPSession::GetTable());
    }
        
    TBuf8<256> aUri;
    
    if(aIsSecure)
        aUri.Copy(_L8("https://"));
    else
        aUri.Copy(_L8("http://"));
    
    aUri.Append(aData->iServerName);
    
    // Don't add the port for https
    if(!aIsSecure)
        aUri.AppendFormat(_L8(":%d"), aData->iPort);
    
    // Add '/' if it is not included in the given page name
    if(!((TChar)aData->iHttpPage[0] == '/'))
    {
        aUri.Append(_L8("/"));
    }
    aUri.Append(aData->iHttpPage);
    
    TUriParser8 uri; 
    uri.Parse(aUri);
    iTrans = iHttpSession.OpenTransactionL(uri, *iTransObs, method);
    RHTTPHeaders hdr = iTrans.Request().GetHeaderCollection();
    
    // Add headers appropriate to all methods
    SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
    SetHeaderL(hdr, HTTP::EAccept, KAccept);
    
    if (aHasBody)
    {
        // Content type header
        TBuf8<KMaxContentTypeSize> contTypeBuf;
        contTypeBuf.Copy(iReqBodyContentType);
        RStringF contTypeStr = iHttpSession.StringPool().OpenFStringL(contTypeBuf);
        THTTPHdrVal contType(contTypeStr);
        hdr.SetFieldL( iHttpSession.StringPool().StringF( HTTP::EContentType,
                                                          RHTTPSession::GetTable() ),
                       contType );
        contTypeStr.Close();
        
        MHTTPDataSupplier* dataSupplier = this;
        iTrans.Request().SetBody(*dataSupplier);
    }
    
    iTrans.SubmitL();
}


// ----------------------------------------------------------------------------
// CHttpClient::GetNextDataPart
// Return next data chunk to be posted.
// ----------------------------------------------------------------------------
//
TBool CHttpClient::GetNextDataPart(TPtrC8& aDataPart)
{
    
    aDataPart.Set(iReqBodySubmitBufferPtr);

    if(iDataChunkCount == 0)
    {
        iConsole.PrintNotify(_L("Sending body...\n"));
        iLastTimeStamp.UniversalTime();
    }

    ++iDataChunkCount;
    iNoMoreDate = iDataChunkCount < iSettingData->iPackets ? EFalse : ETrue;
    
    return iNoMoreDate;
}


// ----------------------------------------------------------------------------
// CHttpClient::ReleaseData
// Data has been posted, release the data chunk.
// ----------------------------------------------------------------------------
//
void CHttpClient::ReleaseData()
{
    if (iNoMoreDate==EFalse)
    {
        TRAPD(err, iTrans.NotifyNewRequestBodyPartL());
        if (err != KErrNone)
            User::Panic(KPanicConnTest, KCouldntNotifyBodyDataPart);
    }
    else
    {
        DisplayTimeElapsed();
    }
    return;
}


// ----------------------------------------------------------------------------
// CHttpClient::OverallDataSize
// Return size of the data to be posted.
// ----------------------------------------------------------------------------
//
TInt CHttpClient::OverallDataSize()
{
    TInt size = (iSettingData->iPackets)*(iSettingData->iPacketSize);
    return size;
}


// ----------------------------------------------------------------------------
// CHttpClient::Reset()
// Method from MHTTPDataSupplier
// ----------------------------------------------------------------------------
//
TInt CHttpClient::Reset()
{
    return KErrNotSupported;
}


// ----------------------------------------------------------------------------
// CHttpClient::DisplayTimeElapsed()
// Calculate and display throughput for POST.
// ----------------------------------------------------------------------------
//
void CHttpClient::DisplayTimeElapsed()
{
    // Throughput calculation
    TInt size = OverallDataSize();
    TBuf8<128> b(_L8("Body sent\n"));
    Utils::CalculateThroughput(b, iLastTimeStamp, size);
    
    b.Append(_L("\n\n"));
    iConsole.PrintNotify(b);    
}


// ----------------------------------------------------------------------------
// CHttpClient::SetHeaderL()
// Set HTTP request header for http fw.
// ----------------------------------------------------------------------------
//
void CHttpClient::SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8& aHdrValue)
{
    RStringF valStr = iHttpSession.StringPool().OpenFStringL(aHdrValue);
    THTTPHdrVal val(valStr);
    aHeaders.SetFieldL(iHttpSession.StringPool().StringF(aHdrField,RHTTPSession::GetTable()), val);
    valStr.Close();
}


// ----------------------------------------------------------------------------
// CHttpClient::SetPerformance
// Turn on/off performance measurement
// ----------------------------------------------------------------------------
//
void CHttpClient::SetPerformance(const TBool aValue)
{
    iDoPerformance = aValue;
    iTransObs->SetPerformance(aValue);
}


// ----------------------------------------------------------------------------
// CHttpClient::GetCredentialsL()
// Called when a authenticated page is requested. Asks the user for a username
// and password that would be appropriate for the url that was supplied.
// ----------------------------------------------------------------------------
//
TBool CHttpClient::GetCredentialsL(const TUriC8& aURI, RString aRealm, 
                                   RStringF aAuthenticationType,
                                   RString& aUsername, 
                                   RString& aPassword)
                                   
{
    TBuf<80> scratch;
    TBuf8<80> scratch8;
    scratch8.Format(_L8("Enter credentials for URL %S, realm %S"), &aURI.UriDes(), &aRealm.DesC());
    scratch.Copy(scratch8);
    //	iUtils->Test().Printf(_L("%S\n"), &scratch);
    scratch.Copy(aAuthenticationType.DesC());
    //	iUtils->Test().Printf(_L("Using %S authentication\n"), &scratch);
    //	iUtils->GetAnEntry(_L("Username (or QUIT to give up): "), scratch);
    scratch8.Copy(scratch);
    if (scratch8.CompareF(_L8("quit")))
    {
        TRAPD(err, aUsername = aRealm.Pool().OpenStringL(scratch8));
        if (!err)
        {
            //			iUtils->GetAnEntry(_L("Password: "), scratch);
            scratch8.Copy(scratch);
            TRAP(err, aPassword = aRealm.Pool().OpenStringL(scratch8));
            if (!err)
                return ETrue;
        }
    }
    return EFalse;
}



//=============================================================================
//
// Implementation of class CHttpEventHandler
//
//=============================================================================



// ----------------------------------------------------------------------------
// CHttpEventHandler::NewLC(MUINotify& aConsole)
// Two-phase constructor
// ----------------------------------------------------------------------------
//
CHttpEventHandler* CHttpEventHandler::NewLC(MUINotify& aConsole)
{
    CHttpEventHandler* me = new(ELeave)CHttpEventHandler(aConsole);
    CleanupStack::PushL(me);
    me->ConstructL();
    return me;
}

// ----------------------------------------------------------------------------
// CHttpEventHandler::NewL(MUINotify& aConsole)
// Two-phase constructor
// ----------------------------------------------------------------------------
//
CHttpEventHandler* CHttpEventHandler::NewL(MUINotify& aConsole)
{
    CHttpEventHandler* me = NewLC(aConsole);
    CleanupStack::Pop(me);
    return me;
}

// ----------------------------------------------------------------------------
// CHttpEventHandler::ConstructL()
// EPOC two-phased constructor
// ----------------------------------------------------------------------------
//
void CHttpEventHandler::ConstructL()
{
}

// ----------------------------------------------------------------------------
// CHttpEventHandler::CHttpEventHandler(MUINotify& aConsole)
// Constructor
// ----------------------------------------------------------------------------
//
CHttpEventHandler::CHttpEventHandler(MUINotify& aConsole)
: iConsole(aConsole)
{
}

// ----------------------------------------------------------------------------
// CHttpEventHandler::~CHttpEventHandler()
// Destructor
// ----------------------------------------------------------------------------
//
CHttpEventHandler::~CHttpEventHandler()
{
}


// ----------------------------------------------------------------------------
// CHttpEventHandler::MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent)
// HTTP event receiver.
// ----------------------------------------------------------------------------
//
void CHttpEventHandler::MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent)
{
    switch (aEvent.iStatus)
    {
    case THTTPEvent::EGotResponseHeaders:
        {
            // HTTP response headers have been received. We can determine now if there is
            // going to be a response body to save.
            RHTTPResponse resp = aTransaction.Response();
            TInt status = resp.StatusCode();
            RStringF statusStr = resp.StatusText();
            const TDesC8& statusStrDesC = statusStr.DesC();
            TBuf< KMaxStatusStrLen > statusStr16; 
            statusStr16.Copy( statusStrDesC.Left( KMaxStatusStrLen ) );
            
            TBuf<64> st;
            st.Format(_L("Status: %d (%S)\n"), status, &statusStr16);
            iConsole.PrintNotify(st);
            
            
            // Dump the headers
            if(!iDoPerformance)
                DumpRespHeadersL(aTransaction);
            
            // Note! For some reason resp.HasBody() returns False although
            // there is body (Tomcat, index.jsp), so this doesn't work.
            // Maybe it checks the Content-Length header?
            if (resp.HasBody() && (status >= 200) && (status < 300) && (status != 204))
            {
                TInt dataSize = resp.Body()->OverallDataSize();
                if (dataSize >= 0)
                {
                    TBuf<64> st;
                    st.Format(_L("Response body size is %d\n"), dataSize);
                    iConsole.PrintNotify(st);
                }
                else
                {
                    iConsole.PrintNotify(_L("Response body size is unknown\n"));
                }
            }
            
            if(iDoPerformance)
            {
                iConsole.PrintNotify(_L("Getting body...\n"));
                iStartTime.UniversalTime();
            }
            iBodySize = 0;
            
        } break;
    case THTTPEvent::EGotResponseBodyData:
        {
            // Get the body data supplier
            iRespBody = aTransaction.Response().Body();
            
            TPtrC8 dataChunk;
            iRespBody->GetNextDataPart(dataChunk);
            iBodySize += dataChunk.Length();
            
            if(!iDoPerformance)
                DumpRespBody(aTransaction);
            
            iRespBody->ReleaseData();
        } break;
    case THTTPEvent::EResponseComplete:
        {
            // The transaction's response is complete
            
            TBuf8<128> b(_L8("Got body\n"));
            
            if(iDoPerformance)
                Utils::CalculateThroughput(b, iStartTime, iBodySize);
            
            iConsole.PrintNotify(b);
            
        } break;
    case THTTPEvent::ESucceeded:
        {
            iConsole.PrintNotify(_L("Transaction Successful\n"));
            aTransaction.Close();
            //CActiveScheduler::Stop();
        } break;
    case THTTPEvent::EFailed:
        {
            iConsole.PrintNotify(_L("Transaction Failed\n"));
            aTransaction.Close();
            //CActiveScheduler::Stop();
        } break;
    case THTTPEvent::ERedirectedPermanently:
        {
            iConsole.PrintNotify(_L("Permanent Redirection\n"));
        } break;
    case THTTPEvent::ERedirectedTemporarily:
        {
            iConsole.PrintNotify(_L("Temporary Redirection\n"));
        } break;
    default:
        {
            TBuf<32> text;
            text.Format(_L("<unrecognised event: %d>\n"), aEvent.iStatus);
            iConsole.PrintNotify(text);
            // close off the transaction if it's an error
            if (aEvent.iStatus < 0)
            {
                aTransaction.Close();
                //CActiveScheduler::Stop();
            }
        } break;
    }
}


// ----------------------------------------------------------------------------
// CHttpEventHandler::MHFRunError(TInt aError, RHTTPTransaction /*aTransaction*/, const THTTPEvent& /*aEvent*/)
// Error handler
// ----------------------------------------------------------------------------
//
TInt CHttpEventHandler::MHFRunError(TInt aError, RHTTPTransaction /*aTransaction*/, const THTTPEvent& /*aEvent*/)
{
    TBuf<64> text;
    text.Format(_L("MHFRunError fired with error code %d\n"), aError);
    iConsole.PrintNotify(text);
    return KErrNone;
}


// ----------------------------------------------------------------------------
// CHttpEventHandler::DumpRespHeadersL(RHTTPTransaction& aTrans)
// Print HTTP headers on console.
// ----------------------------------------------------------------------------
//
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
        
        //TPtrC8 rawField;
        //if (hdr.GetRawField(fieldNameStr,rawField) == KErrNone)
        
        if (hdr.GetField(fieldNameStr,0,fieldVal) == KErrNone)
        {
            const TDesC8& fieldNameDesC = fieldNameStr.DesC();
            fieldName16.Copy(fieldNameDesC.Left(KMaxHeaderNameLen));
            switch (fieldVal.Type())
            {
            case THTTPHdrVal::KTIntVal:
                {
                    TBuf<200> a;
                    a.Format(_L("%S: %d\n"), &fieldName16, fieldVal.Int());
                    iConsole.PrintNotify(a);
                }
                break;
            case THTTPHdrVal::KStrFVal:
                {
                    RStringF fieldValStr = strP.StringF(fieldVal.StrF());
                    const TDesC8& fieldValDesC = fieldValStr.DesC();
                    fieldVal16.Copy(fieldValDesC.Left(KMaxHeaderValueLen));
                    TBuf<200> a;
                    a.Format(_L("%S: %S\n"), &fieldName16, &fieldVal16);
                    iConsole.PrintNotify(a);
                }
                break;
            case THTTPHdrVal::KStrVal:
                {
                    RString fieldValStr = strP.String(fieldVal.Str());
                    const TDesC8& fieldValDesC = fieldValStr.DesC();
                    fieldVal16.Copy(fieldValDesC.Left(KMaxHeaderValueLen));
                    TBuf<200> a;
                    a.Format(_L("%S: %S\n"), &fieldName16, &fieldVal16);
                    iConsole.PrintNotify(a);
                }
                break;
            case THTTPHdrVal::KDateVal:
                {
                    _LIT(KDateString,"%D%M%Y%/0%1%/1%2%/2%3%/3");
                    TDateTime date = fieldVal.DateTime();
                    TBuf<40> dateTimeString;
                    TTime t(date);
                    t.FormatL(dateTimeString,KDateString);
                    TBuf<200> a;
                    a.Format(_L("%S: %S\n"), &fieldName16, &dateTimeString);
                    iConsole.PrintNotify(a);
                } 
                break;
            default:
                {
                    TBuf<200> a;
                    a.Format(_L("%S: <unrecognised value type>\n"), &fieldName16);
                    iConsole.PrintNotify(a);
                }
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
                    TBuf<200> a;
                    a.Format(_L("Realm is: %S\n"), &fieldVal16);
                    iConsole.PrintNotify(a);
                }
            }
        }
        ++it;
    }
}

// ----------------------------------------------------------------------------
// CHttpEventHandler::DumpRespBody(RHTTPTransaction& aTrans)
// Print body size on console.
// ----------------------------------------------------------------------------
//
void CHttpEventHandler::DumpRespBody(RHTTPTransaction& aTrans)
{
    MHTTPDataSupplier* body = aTrans.Response().Body();
    TPtrC8 dataChunk;
    TBool isLast = body->GetNextDataPart(dataChunk);
    
    TBuf<32> b;
    b.Format(_L("-body size: %d\n"), dataChunk.Length());
    iConsole.PrintNotify(b);
    //DumpIt(dataChunk);
    if (isLast)
        iConsole.PrintNotify(_L("Got last data chunk.\n"));
}



