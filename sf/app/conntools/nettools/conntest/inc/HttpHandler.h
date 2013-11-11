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
* Description: HttpHandler is used for HTTP connection components testing
*
*/

#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H

// INCLUDES

#include <in_sock.h>
#include <http.h>
#include <http/mhttpauthenticationcallback.h>

// CONSTANTS

const TInt KMaxContentTypeSize = 64;

// FORWARD DECLARATIONS
class MUINotify;
class CHttpEventHandler;
class CSettingData;

// CLASS DECLARATION


/**
*  CHttpClient
*  
*  Class for initiating HTTP transaction.
*/
class CHttpClient : public CBase, public MHTTPDataSupplier, 
public MHTTPAuthenticationCallback
{
public:
    virtual ~CHttpClient();
    static CHttpClient* NewLC(MUINotify& aConsole);
    static CHttpClient* NewL(MUINotify& aConsole);
    
    
    /**
    * Start http request
    */
    void InvokeHttpMethodL(const CSettingData* aData, TBool aHasBody, TBool aIsSecure);

    /**
    * Set connection info for HTTP FW session
    */
    void SetHttpConnectionInfoL( TBool aUseOwnConnection, 
                                 RConnection& aConnection, 
                                 RSocketServ& aSocketServ );
        
    /**
    * Set throughput calculation on or off.
    */
    void SetPerformance(const TBool aValue);
    
    /**
    * Methods inherited from MHTTPDataSupplier
    */
    virtual TBool GetNextDataPart(TPtrC8& aDataPart);
    virtual void ReleaseData();
    virtual TInt OverallDataSize();
    virtual TInt Reset();
    
    /**
    * Methods inherited from MHTTPAuthenticationCallback
    */
    virtual TBool GetCredentialsL(const TUriC8& aURI, RString aRealm, 
        RStringF aAuthenticationType,
        RString& aUsername, 
        RString& aPassword);
    
protected:
    CHttpClient(MUINotify& aConsole);
    void ConstructL();
private:
    
    enum TMenuItems
    {
        EGet,
            EPost,
            EHead,
            ETrace,
            EToggleVerbosity,
            EQuit
    };
    
    /**
    * Display performance information when sending POST.
    */
    void DisplayTimeElapsed();
    
    /**
    * Set HTTP request header for http fw.
    */
    void SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8& aHdrValue);
    
private:
    RHTTPSession         iHttpSession;
    TTime                iLastTimeStamp;
    TInt                 iDataChunkCount;
    RHTTPTransaction     iTrans;
    CHttpEventHandler*   iTransObs;
    HBufC8*              iReqBodySubmitBuffer;
    TPtr8                iReqBodySubmitBufferPtr;
    TBool                iNoMoreDate;
    MUINotify&           iConsole;
    CSettingData*        iSettingData;
    TBool                iDoPerformance;
    TBuf<KMaxContentTypeSize> iReqBodyContentType; // not used now
};




/**
*  CHttpEventHandler
*  
*  Handles all events for the active HTTP transaction.
*/
class CHttpEventHandler : public CBase, public MHTTPTransactionCallback
{
public:
    virtual ~CHttpEventHandler();
    static CHttpEventHandler* NewLC(MUINotify& aConsole);
    static CHttpEventHandler* NewL(MUINotify& aConsole);
    
    /**
    * Turn performance measurement on/off
    */
    void SetPerformance(const TBool aValue){iDoPerformance = aValue;};
    
    /**
    * Methods from MHTTPTransactionCallback
    */
    virtual void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent);
    virtual TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent& aEvent);
    
protected:
    CHttpEventHandler(MUINotify& aConsole);
    void ConstructL();
private:
    void DumpRespHeadersL(RHTTPTransaction& aTrans);
    void DumpRespBody(RHTTPTransaction& aTrans);

    MHTTPDataSupplier*   iRespBody;
    MUINotify&           iConsole; // console for displaying text etc
    TTime                iStartTime;
    TInt                 iBodySize;
    TBool                iDoPerformance;
};


#endif // HTTPHANDLER_H

