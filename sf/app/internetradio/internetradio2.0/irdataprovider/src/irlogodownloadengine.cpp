/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Engine to fetch the logos 
*
*/


#include <aknnotewrappers.h> 

#include "irlogodownloadengine.h"
#include "irnetworkcontroller.h"
#include "ircachemgmt.h"
#include "irhttprequestdata.h"
#include "irdebug.h"
#include "irdataproviderconstants.h"
#include "iractivenetworkobserver.h"


_LIT(KLogoX, "?x=");
_LIT(KLogoY, "&y=");
const TInt KPresetsArrayMaxLength = 50;
const TInt KLogoSize = 59;
const TInt KParameterSize = 12;
const TInt KTen = 10;
const TInt KCacheAvailable = 4;
const TInt KLogo = 4;
const TInt KStatusOK = 200;
const TInt KStatusMultipleChoices = 300;
const TInt KStatusNotModified = 304;
const TInt KStatusGatewayTimeout = 504;


// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::NewL()
// Creates instance of CIRLogoDownloadEngine.
// ---------------------------------------------------------------------------
//
EXPORT_C CIRLogoDownloadEngine* CIRLogoDownloadEngine::NewL()
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::NewL - Entering" );
    CIRLogoDownloadEngine* self = new ( ELeave ) CIRLogoDownloadEngine();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRLogoDownloadEngine::NewL - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::CIRLogoDownloadEngine()
// Standard C++ constructor
// ---------------------------------------------------------------------------
//
CIRLogoDownloadEngine::CIRLogoDownloadEngine()
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::CIRLogoDownloadEngine " );
    //code here, if needed
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::~CIRLogoDownloadEngine()
// Destructs instance of CIRLogoDownloadEngine.
// ---------------------------------------------------------------------------
//
EXPORT_C CIRLogoDownloadEngine::~CIRLogoDownloadEngine()
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::~CIRLogoDownloadEngine - Entering" );
    if ( iReqBody )
        {
        delete iReqBody;
        iReqBody = NULL;
        }
    if ( iRespBody )
        {
        delete iRespBody;
        iRespBody = NULL;
        }
    CancelTransaction();
    // remove E32USER-CBase-66
#ifdef _DEBUG
    TRAP_IGNORE( iLogoSession.Close() );
#else
    iLogoSession.Close();
#endif
    
    if ( iCopyPreset )
        {
        delete iCopyPreset;
        }
    if ( iCacheTempPreset )
        {
        delete iCacheTempPreset;
        }
    if ( iTempPreset )
        {
        delete iTempPreset;
        }
    if ( iPresetArray )
        {
        iPresetArray->ResetAndDestroy();
        }
    delete iPresetArray;
    if ( iObserverArray )
        {
        delete iObserverArray;
        iObserverArray = NULL;
        }
    if ( iReqFromArray )
        {
        delete iReqFromArray;
        iReqFromArray = NULL;
        }
    if ( iCachePresetArray )
        {
        iCachePresetArray->ResetAndDestroy();
        }
    delete iCachePresetArray;
    if ( iCacheObserverArray )
        {
        delete iCacheObserverArray;
        iCacheObserverArray = NULL;
        }
    if ( iCacheReqFromArray )
        {
        delete iCacheReqFromArray;
        iCacheReqFromArray = NULL;
        }
    if ( iIRNetworkControllerHandle )
        {
        iIRNetworkControllerHandle->DeleteActiveNetworkObserver( *this );
        iIRNetworkControllerHandle->Close();
        }
    if ( iCache )
        {
        iCache->RemoveObserver( this );
        iCache->Close();
        }
    if ( iResponseHeaders )
        {
        delete iResponseHeaders;
        }
    IRLOG_DEBUG( "CIRLogoDownloadEngine::~CIRLogoDownloadEngine - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::ConstructL()
// 2nd phase construction
// ---------------------------------------------------------------------------
//
void CIRLogoDownloadEngine::ConstructL()
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::ConstructL - Entering" );
    iPresetArray = new( ELeave ) CArrayPtrFlat<CIRIsdsPreset>(
                                                     KPresetsArrayMaxLength );
    iObserverArray = new( ELeave ) CArrayPtrFlat<MLogoDownloadObserver>(
                                                     KPresetsArrayMaxLength );
    iReqFromArray = new( ELeave ) CArrayFixFlat<TInt>(
                                                     KPresetsArrayMaxLength );
    iCachePresetArray = new( ELeave ) CArrayPtrFlat<CIRIsdsPreset>(
                                                     KPresetsArrayMaxLength );
    iCacheObserverArray = new( ELeave ) CArrayPtrFlat<MLogoDownloadObserver>( 
                                                     KPresetsArrayMaxLength );
    iCacheReqFromArray = new( ELeave ) CArrayFixFlat<TInt>(
                                                     KPresetsArrayMaxLength );
    iPresetArrayPos = 0;
    iReqFromArrayPos = 0;
    iObserverArrayPos = 0;
    iCachePresetArrayPos = 0;
    iCacheReqFromArrayPos = 0;
    iCacheObserverArrayPos = 0;
    iImageIndex = 0; //using temporarily till the isds provides urls

    iPresetModifiedStatus = EFalse;
    iFirstTime = ETrue;
    iIRNetworkControllerHandle = CIRNetworkController::OpenL();
    iIRNetworkControllerHandle->RegisterActiveNetworkObserverL( *this );
    iCache = CIRCacheMgmt::OpenL( *this );
    iCache->AddObserverL( this );
    iResponseHeaders = new ( ELeave ) CIRHttpResponseData;
    //change to use the new session for logo downloading
    TRAPD( LeaveValue, iLogoSession.OpenL() );
    if ( LeaveValue != KErrNone )
        {
        // Most common error; no access point configured, and session creation
        // leaves with KErrNotFound.
        // Load a string from the resource file and add the error code to string
        User::Leave( LeaveValue );
        }
    IRLOG_DEBUG( "CIRLogoDownloadEngine::ConstructL - Exiting" );
    }


// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::IsRunning()
// To know the status of downloading logo
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CIRLogoDownloadEngine::IsRunning() const
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::IsRunning" );
    return iRunning;
    }


// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::SetHeaderL()
// to set the headers of the http transaction
// ---------------------------------------------------------------------------
//
void CIRLogoDownloadEngine::SetHeaderL( RHTTPHeaders aHeaders,
                               TInt aHdrField, const TDesC8& aHdrValue ) const
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::SetHeaderL - Entering" );
    RStringF valStr = iLogoSession.StringPool().OpenFStringL( aHdrValue );
    CleanupClosePushL( valStr );
    THTTPHdrVal val( valStr );
    aHeaders.SetFieldL( iLogoSession.StringPool().StringF( aHdrField,
                                            RHTTPSession::GetTable() ), val );
    CleanupStack::PopAndDestroy( &valStr ); // valStr
    IRLOG_DEBUG( "CIRLogoDownloadEngine::SetHeaderL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::SendRequestL()
// API is called from the SearchResultsView for to download logo 
// while doing Add to Favorites
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRLogoDownloadEngine::SendRequestL( CIRIsdsPreset* aPreset,
                                                  TInt aXValue, TInt aYValue )
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::SendRequestL - Entering" );
    if ( !iCheckingCache )
        {
        ++iImageIndex;
        iCheckingCache = ETrue;
        iCacheReqFrom = 0;
        if ( iCacheTempPreset )
            {
            delete iCacheTempPreset;
            }
        iCacheTempPreset = NULL;
        iCacheTempPreset = CIRIsdsPreset::NewL();
        *iCacheTempPreset = *aPreset;

        if ( iCacheTempPreset->GetImgUrl().Length() != 0 )
            {
            if ( ( aXValue > 0 ) && ( aYValue > 0 ) )
                {
                //Append the parameters(width,height) of the logo size to imgUrl
                TInt size = iCacheTempPreset->GetImgUrl().Length()
                                + KParameterSize;
                RBuf urlWithSize;
                CleanupClosePushL(urlWithSize);
                urlWithSize.CreateL( size );
                urlWithSize.Copy( iCacheTempPreset->GetImgUrl() );
                urlWithSize.Append( KLogoX );
                urlWithSize.AppendNum( aXValue );
                urlWithSize.Append( KLogoY );
                urlWithSize.AppendNum( aYValue );

                //set the imgUrl with parameters to the preset
                iCacheTempPreset->SetImgUrlL( urlWithSize );
                CleanupStack::PopAndDestroy();
                }
            //This method checks whether the logo is available
            // for the request made
            iForceGet = EFalse;
            CheckCacheForLogoL( 0 );
            }
        else
            {
            //imgUrl doesnt exist in the Preset
            iCheckingCache = EFalse;
            return;
            }

        iCheckingCache = EFalse;
        if ( !iLogoCached )
            {
            FetchLogoDataL( iCacheTempPreset );
            }
        }
    else
        {
        CIRIsdsPreset* tempPreset;
        tempPreset = CIRIsdsPreset::NewL();
        CleanupStack::PushL(tempPreset);
        *tempPreset = *aPreset;
        if ( tempPreset->GetImgUrl().Length() != 0 )
            {
            if ( ( aXValue > 0 ) && ( aYValue > 0 ) )
                {
                //Append the parameters(width,height) of the logo size to imgUrl
                TInt size = tempPreset->GetImgUrl().Length() + KParameterSize;
                RBuf urlWithSize;
                CleanupClosePushL(urlWithSize);
                urlWithSize.CreateL( size );
                urlWithSize.Copy( tempPreset->GetImgUrl() );
                urlWithSize.Append( KLogoX );
                urlWithSize.AppendNum( aXValue );
                urlWithSize.Append( KLogoY );
                urlWithSize.AppendNum( aYValue );

                //set the imgUrl with parameters to the preset
                tempPreset->SetImgUrlL( urlWithSize );
                CleanupStack::PopAndDestroy();
                }
            }
        iCachePresetArray->AppendL( tempPreset );
        iCacheReqFromArray->AppendL( 0 );
        CleanupStack::PopAndDestroy();
        }
    IRLOG_DEBUG( "CIRLogoDownloadEngine::SendRequestL - Exiting" );
    }


EXPORT_C TBool CIRLogoDownloadEngine::IsLogoCachedL( 
                          CIRIsdsPreset* aPreset, TInt aXValue, TInt aYValue )
{
    TInt cached = 0;
    
    if(iCacheTempPreset)
    {
        delete iCacheTempPreset;
        iCacheTempPreset = NULL;
    }
    
    iCacheTempPreset = CIRIsdsPreset::NewL();
    *iCacheTempPreset = *aPreset;
    iCacheReqMade = EFalse;
    
    if (iCacheTempPreset->GetImgUrl().Length() != 0)
    {
        if ((aXValue > 0) && (aYValue > 0))
        {
            //Append the parameters(width,height) of the logo size to imgUrl
            TInt size = iCacheTempPreset->GetImgUrl().Length() + KParameterSize;
            RBuf urlWithSize;
            CleanupClosePushL(urlWithSize);
            urlWithSize.CreateL(size);
            urlWithSize.Copy(iCacheTempPreset->GetImgUrl());
            urlWithSize.Append(KLogoX);
            urlWithSize.AppendNum(aXValue);
            urlWithSize.Append(KLogoY);
            urlWithSize.AppendNum(aYValue);

            //set the imgUrl with parameters to the preset
            iCacheTempPreset->SetImgUrlL(urlWithSize);
            CleanupStack::PopAndDestroy();
        }
    }
    
    iCache->CheckCacheL(KLogo,iCacheTempPreset->GetImgUrl(),EFalse,cached);
    
    return (CIRCacheMgmt::ECacheUseable == cached)?ETrue:EFalse;     
}
// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::SendRequestL()
// API is called from several views for to download logo 
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRLogoDownloadEngine::SendRequestL( CIRIsdsPreset* aPreset,
                                    MLogoDownloadObserver* aLogoHandle, 
                                    TInt aNPVReq, TInt aXValue, TInt aYValue )
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::SendRequestL - Entering" );
    if ( aPreset->GetChannelType() == 0 )
        {
        aLogoHandle->PresetLogoDownloadedL( aPreset );
        }
    else if ( !iCheckingCache )
        {
        ++iImageIndex;
        iCheckingCache = ETrue;
        if ( aNPVReq == 2 )
            {
            iCacheReqFrom = 2;
            }
        else
            {
            iCacheReqFrom = 1;
            }

        if ( iCacheTempPreset )
            {
            delete iCacheTempPreset;
            iCacheTempPreset = NULL;
            }
        iCacheTempPreset = CIRIsdsPreset::NewL();
        *iCacheTempPreset = *aPreset;
        iCacheTempLogoHandle = aLogoHandle;

        if ( iCacheTempPreset->GetImgUrl().Length() != 0 )
            {
            if ( ( aXValue > 0 ) && ( aYValue > 0 ) )
                {
                //Append the parameters(width,height) of the logo size to imgUrl
                TInt size = iCacheTempPreset->GetImgUrl().Length()
                                + KParameterSize;
                RBuf urlWithSize;
                CleanupClosePushL(urlWithSize);
                urlWithSize.CreateL( size );
                urlWithSize.Copy( iCacheTempPreset->GetImgUrl() );
                urlWithSize.Append( KLogoX );
                urlWithSize.AppendNum( aXValue );
                urlWithSize.Append( KLogoY );
                urlWithSize.AppendNum( aYValue );

                //set the imgUrl with parameters to the preset
                iCacheTempPreset->SetImgUrlL( urlWithSize );
                CleanupStack::PopAndDestroy();
                }
            if ( iCacheReqFrom == 1 || iCacheReqFrom == 2 )
                {
                //This method checks whether the logo is available
                // for the request made
                iForceGet = EFalse;
                CheckCacheForLogoL( 0 );
                }
/*            else if(iCacheReqFrom==2)
                {
                //the request has come from NowPlayingView(NPV)
                //dont check for logo availability in cache
                //download the logo freshly
                iLogoCached=EFalse;                                
                }
*/            }
        else
            {
            //imgUrl doesnt exist in the Preset
            iCheckingCache = EFalse;
            return;
            }

        iCheckingCache = EFalse;
        if ( !iLogoCached )
            {
            FetchLogoDataL( iCacheTempPreset, aLogoHandle, iCacheReqFrom );
            }
        }
    else
        {
        CIRIsdsPreset* tempPreset;
        tempPreset = CIRIsdsPreset::NewL();
        CleanupStack::PushL(tempPreset);
        *tempPreset = *aPreset;
        if ( tempPreset->GetImgUrl().Length() != 0 )
            {
            if ( ( aXValue > 0 ) && ( aYValue > 0 ) )
                {
                //Append the parameters(width,height) of the logo size to imgUrl
                TInt size = tempPreset->GetImgUrl().Length() + KParameterSize;
                RBuf urlWithSize;
                CleanupClosePushL(urlWithSize);
                urlWithSize.CreateL( size );
                urlWithSize.Copy( tempPreset->GetImgUrl() );
                urlWithSize.Append( KLogoX );
                urlWithSize.AppendNum( aXValue );
                urlWithSize.Append( KLogoY );
                urlWithSize.AppendNum( aYValue );

                //set the imgUrl with parameters to the preset
                tempPreset->SetImgUrlL( urlWithSize );
                CleanupStack::PopAndDestroy();
                }
            }
        iCachePresetArray->AppendL( tempPreset );
        iCacheObserverArray->AppendL( aLogoHandle );
        if ( aNPVReq == 2 )
            {
            iCacheReqFromArray->AppendL( 2 );
            }
        else
            {
            iCacheReqFromArray->AppendL( 1 );
            }
        CleanupStack::PopAndDestroy();
        }
    IRLOG_DEBUG( "CIRLogoDownloadEngine::SendRequestL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::GetCacheLogoL()
// takes the url as a parameter and returns the logo data which is in cache
// this API is called form the search results for to display logo on the view
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRLogoDownloadEngine::GetCacheLogoL( const TDesC& aUrl,
                                                               TInt& aStatus )
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::GetCacheLogoL - Entering" );
    iSendingLogo = 2;
    iResult = 0;
    iForceGet = EFalse;
    //2 means, this API is called for the search results view
    iCacheReqMade = ETrue;
    iCache->CheckCacheL( KLogo, aUrl, iForceGet, iResult );
    aStatus = iResult;
    IRLOG_DEBUG( "CIRLogoDownloadEngine::GetCacheLogoL - Exiting" );

    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::SendCacheLogo()
// sends the logo which is cached
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC8& CIRLogoDownloadEngine::SendCacheLogo()
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::SendCacheLogo" );
    return iCache->iLogoData;
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::FetchLogoDataL()
// API is used to download logo from isds/internet
// called in the API SendRequestL(CIRIsdsPreset* aPreset)
// ---------------------------------------------------------------------------
//
void CIRLogoDownloadEngine::FetchLogoDataL( CIRIsdsPreset* aPreset )
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::FetchLogoDataL - Entering" );
    if ( !iRunning )
        {
        //which means the request came from SearchView 
        // while doing Add to Favorite
        iTempReqFrom = 0;
        if ( iReqBody )
            {
            delete iReqBody;
            iReqBody = NULL;
            }
        if ( iRespBody )
            {
            delete iRespBody;
            iRespBody = NULL;
            }
        if ( iTempPreset )
            {
            delete iTempPreset;
            iTempPreset = NULL;
            }
        iTempPreset = CIRIsdsPreset::NewL();
        *iTempPreset = *aPreset;
        if ( iTempPreset->GetImgUrl().Length() != 0 )
            {
            IssueLogoDownloadRequestL();
            }
        else /*ImgUrl doesnt exist in the iTempPreset*/
            {
            //simply ignore it
            }
        }
    else
        {
        CIRIsdsPreset* tempPreset;
        tempPreset = CIRIsdsPreset::NewL();
        CleanupStack::PushL( tempPreset );
        *tempPreset = *aPreset;
        iPresetArray->AppendL( tempPreset );
        CleanupStack::Pop( tempPreset );
        iReqFromArray->AppendL( 0 );
        }
    IRLOG_DEBUG( "CIRLogoDownloadEngine::FetchLogoDataL - Exiting" );
    }


// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::FetchLogoDataL()
// API is used to download logo from isds/internet
// called in the API SendRequestL())
// ---------------------------------------------------------------------------
//
void CIRLogoDownloadEngine::FetchLogoDataL( CIRIsdsPreset* aPreset,
                            MLogoDownloadObserver* aLogoHandle, TInt aNPVReq )
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::FetchLogoDataL - Entering" );
    if ( !iRunning )
        {
        if ( aNPVReq == 2 )
            {
            //which means the request came from NowPlayingView
            iTempReqFrom = 2;
            }
        else
            {
            //which means the request came from other than NowPlayingView
            iTempReqFrom = 1;
            }

        iTempLogoHandle = aLogoHandle;
        if ( iReqBody )
            {
            delete iReqBody;
            iReqBody = NULL;
            }
        if ( iRespBody )
            {
            delete iRespBody;
            iRespBody = NULL;
            }
        if ( iTempPreset )
            {
            delete iTempPreset;
            iTempPreset = NULL;
            }
        iTempPreset = CIRIsdsPreset::NewL();
        *iTempPreset = *aPreset;
        if ( iTempPreset->GetImgUrl().Length() != 0 )
            {
            IssueLogoDownloadRequestL();
            }
        else /*ImgUrl doesnt exist in the iTempPreset*/
            {
            //simply ignore it
            }
        }
    else
        {
        CIRIsdsPreset* tempPreset;
        tempPreset = CIRIsdsPreset::NewL();
        CleanupStack::PushL( tempPreset );
        *tempPreset = *aPreset;
        iPresetArray->AppendL( tempPreset );
        CleanupStack::Pop( tempPreset );
        iObserverArray->AppendL( aLogoHandle );
        if ( aNPVReq == 2 )
            {
            iReqFromArray->AppendL( 2 );
            }
        else
            {
            iReqFromArray->AppendL( 1 );
            }
        }
    IRLOG_DEBUG( "CIRLogoDownloadEngine::FetchLogoDataL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::CheckCacheForLogoL()
// Checks the cache whether the logo is available or not
// ---------------------------------------------------------------------------
//
void CIRLogoDownloadEngine::CheckCacheForLogoL( TInt aValue )
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::CheckCacheForLogoL - Entering" );
    iSendingLogo = aValue;
    iResult = 0;
    //0 means, this API is called for the usecase of checking cache
    if ( iSendingLogo == 0 )
        {
        iCacheReqMade = ETrue;
        iCache->CheckCacheL( KLogo, iCacheTempPreset->GetImgUrl(), iForceGet,
                        iResult );
        }
    //1 means, this API is called for the usecase of downloading logo
    else if ( iSendingLogo == 1 )
        {
        iCacheReqMade = ETrue;
        iCache->CheckCacheL( KLogo, iTempPreset->GetImgUrl(), iForceGet,
                        iResult );
        }
    if ( ( iResult == 0 ) )//not available in the cache
        {
        iLogoCached = EFalse;
        iIsConditionalGET = EFalse;
        }
    else if ( iResult == -1 )//cache is stale/not useable
        {
        iLogoCached = EFalse;
        iIsConditionalGET = ETrue;
        }
    else if ( iResult == 1 )//cache is useable
        {
        iLogoCached = ETrue;
        }
    IRLOG_DEBUG( "CIRLogoDownloadEngine::CheckCacheForLogoL - Exiting" );
    }


// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::IssueLogoDownloadRequest()
// submits the transaction for to download the logo
// ---------------------------------------------------------------------------
//
void CIRLogoDownloadEngine::IssueLogoDownloadRequestL()
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::IssueLogoDownloadRequestL - Entering" );
    //This piece of code is for selecting the access point which is already
    //set at the begining of the application.  To download the logo the same
    //accesse point will be used.
    if ( !iIRNetworkControllerHandle->GetNetworkStatus() )
        {
        iIRNetworkControllerHandle->ChooseAccessPointL();
        iLogoRequestPending = ETrue;
        }
    else
        {
        if ( iFirstTime )
            {
            iLogoSession.Close();
            TRAPD( LeaveValue, iLogoSession.OpenL() );
            if ( LeaveValue != KErrNone )
                {
                // Most common error; no access point configured, and session 
                // creation leaves with KErrNotFound. Load a string from the 
                // resource file and add the error code to string
                User::Leave( LeaveValue );
                }
            // Set the HTTP connection properties
            RStringPool strP = iLogoSession.StringPool();
            RHTTPConnectionInfo connInfo = iLogoSession.ConnectionInfo();
            // RSocketServ Handle
            connInfo.SetPropertyL( strP.StringF( HTTP::EHttpSocketServ, RHTTPSession::GetTable() ),
                                   THTTPHdrVal( iIRNetworkControllerHandle->GetIRSocketServer().Handle() ) );
            
            // RConnection Handle
            TInt connPtr = REINTERPRET_CAST( TInt, &( iIRNetworkControllerHandle->GetIRConnection() ) );
            connInfo.SetPropertyL( strP.StringF( HTTP::EHttpSocketConnection, RHTTPSession::GetTable() ),
                                   THTTPHdrVal( connPtr ) );
            
            // HTTP Version ( Specify that i am using HTTP/1.1
            connInfo.SetPropertyL( strP.StringF( HTTP::EVersion, RHTTPSession::GetTable() ), 
                                   THTTPHdrVal( strP.StringF( HTTP::EHttp11, RHTTPSession::GetTable() ) ) );

            // Set RHttpSession into NetworkController for logo byte counter
            iIRNetworkControllerHandle->RegisterLogoDataTransferTrackerL(
                                                               iLogoSession );

            iFirstTime = EFalse;
            }

        HBufC8* buffertmp = HBufC8::NewLC( iTempPreset->GetImgUrl().Length()
                        + 1 );
        TPtr8 buffertmpPtr( buffertmp->Des() );
        buffertmpPtr.Copy( iTempPreset->GetImgUrl() );

        TUriParser8 uri;
        uri.Parse( buffertmpPtr );
        RStringF method = iLogoSession.StringPool().StringF( HTTP::EGET,
                                                   RHTTPSession::GetTable() );
        iTransaction = iLogoSession.OpenTransactionL( uri, *this, method );
        RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
        // Set the User-Agent header to UAProf string
        SetHeaderL( hdr, HTTP::EUserAgent,
                    iIRNetworkControllerHandle->GetUAProfString()->Des() );
        // If KAccept include "image/jpeg", we can't get correct content of 
		// png file from server 
        _LIT8(KAccept,"image/png,image/gif");
        SetHeaderL( hdr, HTTP::EAccept, KAccept );
        SetHeaderL( hdr, HTTP::EAcceptLanguage, KAcceptLanguage );

        if ( iIsConditionalGET )
            {
            RStringF valStr = iLogoSession.StringPool().OpenFStringL( iETag );
            CleanupClosePushL( valStr );
            THTTPHdrVal val( valStr );
            hdr.SetFieldL( iLogoSession.StringPool().StringF(HTTP::EIfNoneMatch, RHTTPSession::GetTable() ),
                           val );
            CleanupStack::PopAndDestroy( &valStr ); // valStr
            }
        CleanupStack::PopAndDestroy( buffertmp ); // buffertmp

        //setting the member variable to zero
        iRespBodySize = 0;
        iTransaction.SubmitL();

        iRunning = ETrue;
        iLogoRequestPending = ETrue;
        iState = ESendingEvent;
        }
    IRLOG_DEBUG( "CIRLogoDownloadEngine::IssueLogoDownloadRequestL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::CancelTransaction()
// cancels the current transaction
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRLogoDownloadEngine::CancelTransaction()
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::CancelTransaction - Entering" );
    if ( !iRunning )
        {
        return;
        }
    iTransaction.Close();
    iRunning = EFalse;
    IRLOG_DEBUG( "CIRLogoDownloadEngine::CancelTransaction - Exiting" );
    }


// ----------------------------------------------------------------------------------
// CIRLogoDownloadEngine::SendPresetWithLogoL()
// sends the preset with logo to the requestors
// ----------------------------------------------------------------------------------
//
EXPORT_C void CIRLogoDownloadEngine::SendPresetWithLogoL()
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::SendPresetWithLogoL - Entering" );
    //Logo has downloaded, so update the status of the variable iLogoRequestPending
    iLogoRequestPending = EFalse;
    //Remove the imgUrl with parameters from the Preset
    //now set imgUrl without parameters in to the Preset
    TInt sizeImgUrlWithPar = iTempPreset->GetImgUrl().Length();
    TBool hasParameters = EFalse;
    RBuf urlWithPar;
    CleanupClosePushL(urlWithPar);
    urlWithPar.CreateL( sizeImgUrlWithPar );
    urlWithPar.Copy( iTempPreset->GetImgUrl() );
    RBuf tempBuf;
    CleanupClosePushL(tempBuf);
    tempBuf.CreateL( sizeImgUrlWithPar );

    TLex urlLex( urlWithPar );
    for ( ; !urlLex.Eos(); )
        {
        tempBuf.Zero();
        while ( urlLex.Peek() != '?' && !urlLex.Eos() )
            {
            tempBuf.Append( urlLex.Get() );
            }
        if ( urlLex.Peek() == '?' )
            {
            hasParameters = ETrue;
            }
        urlLex.Inc();
        }

    RBuf imgUrl;
    CleanupClosePushL(imgUrl);
    if ( hasParameters )
        {
        //let say urlWithPar has the value http://www.logos.com/images/test.pngx=100&y=100
        //now the tempBuf contains the value  x=100&y=100
        TInt sizeUrlParameters = tempBuf.Length();
        sizeUrlParameters++; // now this integer contains the size of x=100&y=100 
        TInt sizeImgUrl = sizeImgUrlWithPar - sizeUrlParameters;

        //Create a buffer to store the imgUrl without parameters
        imgUrl.CreateL( sizeImgUrl );
        imgUrl.Copy( urlWithPar.Left( sizeImgUrl ) );
        iTempPreset->SetImgUrlL( imgUrl );
        }
    else
        {
        //nothing to be done
        //the imgUrl present in the preset is correct, no need to change anything
        }

    //set the downloaded logo to the preset with the API SetLogoData()
    iTempPreset->SetLogoDataL( *iRespBody );
    //which means the request came from SearchView while doing Add to Favorite
    if ( iTempReqFrom == 0 )
        {
        iPresetModifiedStatus = ETrue;
        }
    //which menas the request came from a view other than NowPlayingView
    else if ( iTempReqFrom == 1 )
        {
        iTempLogoHandle->PresetLogoDownloadedL( iTempPreset );
        }
    //which menas the request came from NowPlayingView
    else if ( iTempReqFrom == 2 )
        {
        //(1)logo has downloaded freshly
        //(2)send the logo to the NPV through PresetLogoDownloadedL
        //(3)check whether the preset exists in favdb; if so replace that with new logo
        iTempLogoHandle->PresetLogoDownloadedL( iTempPreset );
        //iFavDb->ReplacePresetL(*iTempPreset)
        }

    iTempPreset->SetImgUrlL( urlWithPar );
    //Cache the newly downloaded logo
    iCache->CacheLogoL( *iRespBody, iTempPreset->GetImgUrl(),
                    *iResponseHeaders );

    CleanupStack::PopAndDestroy(3);

    iRunning = EFalse;
    iState = EIdle;
    IRLOG_DEBUG( "CIRLogoDownloadEngine::SendPresetWithLogoL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::MHFRunL())
// callback API from MHTTPTransactionCallback
// ---------------------------------------------------------------------------
//
void CIRLogoDownloadEngine::MHFRunL( RHTTPTransaction aTransaction,
                                     const THTTPEvent& aEvent )
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::MHFRunL - Entering" );
    switch ( aEvent.iStatus )
        {
        case THTTPEvent::EGotResponseHeaders:
            {
            RHTTPResponse resp = aTransaction.Response();
            TInt status = resp.StatusCode();
            iRespCode = status;
            ExtractResponseHeadersL( aTransaction );
            if ( status >= KStatusOK && status < KStatusMultipleChoices )
                {
                THTTPHdrVal headerValue( 0 );
                RHTTPHeaders headers = resp.GetHeaderCollection();
                TInt error = headers.GetField( iLogoSession.StringPool().StringF(
                                HTTP::EContentLength,
                                RHTTPSession::GetTable() ), 0, headerValue );
                iRespBodySize = 0;
                if ( KErrNone == error )
                    {
                    iRespBodySize = headerValue.Int();
                    }
                iRespBody = HBufC8::NewL( iRespBodySize + 1 );
                }
            //304 means, the content has not modified
            else if ( status == KStatusNotModified )
                {
                if ( iIsConditionalGET )
                    {
                    iIsConditionalGET = EFalse;
                    iRunning = EFalse;
                    //update the trust period 
                    //no problem if it leaves
                    TRAP_IGNORE( iCache->UpdateTrustPeriodL( KLogo, iTempPreset->GetImgUrl(), *iResponseHeaders))
                    //do a forced get because the data is notmodifed in isds and 
                    //trustable
                    iForceGet = ETrue;
                    CheckCacheForLogoL( 1 );
                    }
                }
            else if ( status == KStatusGatewayTimeout )
                {
                //implementation, if needed
                CancelTransaction();
                if ( iTempReqFrom == 0 )
                    {
                    //do nothing
                    }
                else
                    {
                    iTempLogoHandle->PresetLogoDownloadError( iTempPreset );
                    }
                ManagePresetArrayIndexL();
                }
            }
            break;
        case THTTPEvent::EGotResponseBodyData:
            {
            MHTTPDataSupplier* body = aTransaction.Response().Body();
            TPtrC8 dataChunk;
            body->GetNextDataPart( dataChunk );
            if ( iRespBodySize > 0 )
                {
                iRespBody->Des().Append( (const TDesC8&)dataChunk );
                }
            else if ( iRespBody )
                {
                //which means the body of the response has come
                //but in the response headers Content-Length is missing
                //for this usecase realloc the iRespBody with the data received
                HBufC8* buffertmp = HBufC8::NewLC( dataChunk.Length() + 1 );
                TPtr8 buffertmpPtr( buffertmp->Des() );
                buffertmpPtr.Copy( (const TDesC8&)dataChunk );

                TInt tempSize;
                tempSize = iRespBody->Length() + dataChunk.Length();

                iRespBody = iRespBody->ReAllocL( tempSize );
                TPtr8 ibufferPtr( iRespBody->Des() );
                ibufferPtr.Append( buffertmpPtr );

                CleanupStack::PopAndDestroy( buffertmp ); // buffertmp        
                }
            body->ReleaseData();
            }
            break;
        case THTTPEvent::EResponseComplete:
            {
            //No implementation
            }
            break;
        case THTTPEvent::ESucceeded:
            {
            iTransaction.Close();
            if ( iIsConditionalGET )
                {
                iIsConditionalGET = EFalse;
                }

            iRunning = EFalse;
            //set the downloaded logo in the preset and send it back to the requestor
            SendPresetWithLogoL();
            ManagePresetArrayIndexL();
            }
            break;

        case THTTPEvent::EFailed:
            {
            aTransaction.Close();
            iRunning = EFalse;
            iState = EIdle;
            //304 means, the content has not modified
            if ( iRespCode == KStatusNotModified )
                {
                //do nothing;
                }
            else
                {
                if ( iTempReqFrom == 0 )
                    {
                    //do nothing
                    }
                else
                    {
                    iTempLogoHandle->PresetLogoDownloadError( iTempPreset );
                    }
                }
            ManagePresetArrayIndexL();
            }
            break;
        default:

            if ( aEvent.iStatus < 0 )
                {
                aTransaction.Close();
                iRunning = EFalse;
                iState = EIdle;
                if ( iTempReqFrom == 0 )
                    {
                    //do nothing
                    }
                else
                    {
                    iTempLogoHandle->PresetLogoDownloadError( iTempPreset );
                    }
                ManagePresetArrayIndexL();
                }
            break;
        }
    IRLOG_DEBUG( "CIRLogoDownloadEngine::MHFRunL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::MHFRunError()
// callback API from MHTTPTransactionCallback
// ---------------------------------------------------------------------------
//
TInt CIRLogoDownloadEngine::MHFRunError( TInt /*aError*/,
                RHTTPTransaction aTransaction, const THTTPEvent& /*aEvent*/)
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::MHFRunError - Entering" );
    aTransaction.Close();
    iRunning = EFalse;
    iState = EIdle;
    if ( iIsConditionalGET )
        {
        iIsConditionalGET = EFalse;
        }
    if ( iTempReqFrom == 0 )
        {
        //do nothing
        }
    else
        {
        iTempLogoHandle->PresetLogoDownloadError( iTempPreset );
        }
    TRAP_IGNORE(ManagePresetArrayIndexL())
    IRLOG_DEBUG( "CIRLogoDownloadEngine::MHFRunError - Exiting" );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::GetNextDataPart(TPtrC8& aDataPart)
// callback API from MHTTPDataSupplier
// ---------------------------------------------------------------------------
TBool CIRLogoDownloadEngine::GetNextDataPart( TPtrC8& aDataPart )
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::GetNextDataPart - Entering" );
    aDataPart.Length();
    iReqBody->Length();
    aDataPart.Set( *iReqBody );
    IRLOG_DEBUG( "CIRLogoDownloadEngine::GetNextDataPart - Exiting" );
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::ReleaseData()
// callback API from MHTTPDataSupplier
// ---------------------------------------------------------------------------
void CIRLogoDownloadEngine::ReleaseData()
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::ReleaseData" );
    //No implementaion
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::Reset()
// callback API from MHTTPDataSupplier
// ---------------------------------------------------------------------------
TInt CIRLogoDownloadEngine::Reset()
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::Reset" );
    return KErrNone;
    }

// --------------------------------------------------------------------
// CIRLogoDownloadEngine::OverallDataSize()
// callback API from MHTTPDataSupplier
// --------------------------------------------------------------------
TInt CIRLogoDownloadEngine::OverallDataSize()
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::OverallDataSize" );
    return iReqBody->Length();
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::ExtractResponseHeadersL()
// Used to Extract the response headers.
// ---------------------------------------------------------------------------
//
void CIRLogoDownloadEngine::ExtractResponseHeadersL(
                                        const RHTTPTransaction& aTransaction )
    {
    IRLOG_DEBUG( "CIRHttpDataProvider::ExtractResponseHeadersL - Entering" );
    RHTTPResponse response = aTransaction.Response();
    RHTTPHeaders respHeader = response.GetHeaderCollection();
    THTTPHdrFieldIter iterator = respHeader.Fields();
    RStringPool httpStringPool = aTransaction.Session().StringPool();
    iterator.First();
    HBufC8 *headerField = HBufC8::NewLC( KMaxHeaderNameLength
                    + KMaxHeaderValueLength );
    HBufC8 *fieldValBuf = HBufC8::NewLC( KMaxHeaderValueLength );
    while ( iterator.AtEnd() == EFalse )
        {
        RStringTokenF fieldName = iterator();
        RStringF fieldNameStr = httpStringPool.StringF( fieldName );
        THTTPHdrVal fieldVal;
        if ( respHeader.GetField( fieldNameStr, 0, fieldVal ) == KErrNone )
            {
            const TDesC8 &fieldNameDesC = fieldNameStr.DesC();
            headerField->Des().Copy(
                            fieldNameDesC.Left( KMaxHeaderNameLength ) );
            fieldValBuf->Des().Zero();
            switch ( fieldVal.Type() )
                {
                // the value is an integer
                //lint restore -e747:Significant prototype
                //coercion (arg. no. 1) int to long long  :
                case THTTPHdrVal::KTIntVal:
                    fieldValBuf->Des().Num( fieldVal.Int() );
                    break;
                    // the value is a case-insensitive string
                case THTTPHdrVal::KStrFVal:
                    {
                    RStringF fieldValStr = httpStringPool.StringF(
                                    fieldVal.StrF() );
                    const TDesC8 &fieldValDesC = fieldValStr.DesC();
                    fieldValBuf->Des().Copy( fieldValDesC.Left(
                                    KMaxHeaderValueLength ) );
                    }
                    break;
                    // the value is a case-sensitive string
                case THTTPHdrVal::KStrVal:
                    {
                    RString fieldValStr = httpStringPool.String(
                                    fieldVal.Str() );
                    const TDesC8 &fieldValDesC = fieldValStr.DesC();
                    fieldValBuf->Des().Copy( fieldValDesC.Left(
                                    KMaxHeaderValueLength ) );
                    }
                    break;
                    // the value is a date/time
                case THTTPHdrVal::KDateVal:
                    {
                    TDateTime date = fieldVal.DateTime();
                    TTime t( date );
                    // if ( iSetNonUAProfUserAgent )
                    //     {
                    HttpDateHeaderReceived( *headerField, t );
                    //     }
                    }
                    break;
                    // the value is type is unknown
                default:
                    break;
                }
            // Display HTTP header field name and value
            headerField->Des().Append( KDPColon );
            headerField->Des().Append( *fieldValBuf );
            HttpHeaderReceived( *headerField );
            }
        ++iterator;
        }

    CleanupStack::PopAndDestroy( fieldValBuf );
    CleanupStack::PopAndDestroy( headerField );
    IRLOG_DEBUG( "CIRLogoDownloadEngine::ExtractResponseHeadersL - Exiting." );
    }

// ---------------------------------------------------------------------------
//  CIRLogoDownloadEngine::HttpHeaderReceived()
//  indicate that an HTTP header is received.
// ---------------------------------------------------------------------------
//
void CIRLogoDownloadEngine::HttpHeaderReceived( const TDesC8& aHeaderData )
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::HttpHeaderReceived - Entering." );
    _LIT8(KDelim,":");
    _LIT8(KContentType,"Content-Type");
    ExtractHeaderValue( aHeaderData, KContentType, KDelim,
                        iResponseHeaders-> iContentType );
    _LIT8(KMaxAge,"max-age");
    _LIT8(KDelimEqual,"=");
    ExtractHeaderValue( aHeaderData, KMaxAge, KDelimEqual,
                        iResponseHeaders-> iMaxAge );
    _LIT8(KContentLength,"Content-Length");
    ExtractHeaderValue( aHeaderData, KContentLength, KDelim,
                        iResponseHeaders-> iContentLength );
    _LIT8(KExpires,"Expires");
    ExtractHeaderValue( aHeaderData, KExpires, KDelim,
                        iResponseHeaders->iExpires );
    _LIT8(KETag,"ETag");
    ExtractHeaderValue( aHeaderData, KETag, KDelim, iResponseHeaders->iETag );
    IRLOG_DEBUG( "CIRLogoDownloadEngine::HttpHeaderReceived - Exiting." );
    }

// ---------------------------------------------------------------------------
//  CIRLogoDownloadEngine::ExtractHeaderValue()
//  Used to build the CIRHttpResponseData
// ---------------------------------------------------------------------------
//
void CIRLogoDownloadEngine::ExtractHeaderValue( const TDesC8& aHeaderData,
                const TDesC8& aHeaderName, const TDesC8& aDelimeter,
                TDes8& aHolder ) const
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::ExtractHeaderValue - Entering" );
    TInt position = aHeaderData.Find( aHeaderName );
    if ( position >= 0 )
        {
        TPtrC8 headerValue = aHeaderData.Mid( position );
        TInt delimeterPosition = headerValue.Find( aDelimeter );
        if ( delimeterPosition != KErrNotFound )
            {
            delimeterPosition++;
            TPtrC8 value = headerValue.Mid( delimeterPosition );
            aHolder.Copy( value );
            aHolder.TrimAll();
            }
        }
    IRLOG_DEBUG( "CIRLogoDownloadEngine::ExtractHeaderValue - Exiting." );
    }

// ---------------------------------------------------------------------------
//  CIRLogoDownloadEngine::HttpDateHeaderReceived()
//  Used to build the CIRHttpResponseData members
// ---------------------------------------------------------------------------
//
void CIRLogoDownloadEngine::HttpDateHeaderReceived( const TDesC8 &aHeader,
                                                    const TTime& aTime )
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::HttpDateHeaderReceived - Entering." );
    _LIT8(KDate,"Date");
    _LIT8(KLastModified,"Last-Modified");
    TInt position = aHeader.Find( KDate );
    if ( position != KErrNotFound )
        {
        iResponseHeaders->iDate = aTime;
        return;
        }
    position = aHeader.Find( KLastModified );
    if ( position != KErrNotFound )
        {
        iResponseHeaders->iLastModified = aTime;
        }
    IRLOG_DEBUG( "CIRLogoDownloadEngine::HttpDateHeaderReceived - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::ManagePresetArrayIndexL()
// To make another download request if exists in the queue
// To update the iPresetArray current index
// ---------------------------------------------------------------------------
void CIRLogoDownloadEngine::ManagePresetArrayIndexL()
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::ManagePresetArrayIndexL - Entering" );
    TInt PresetArrayLength = iPresetArray->Count();
    if ( ( PresetArrayLength > 0 ) && ( iPresetArrayPos < PresetArrayLength ) )
        {
        CIRIsdsPreset* tempPreset = CIRIsdsPreset::NewL();
        CleanupStack::PushL( tempPreset );
        *tempPreset = *( iPresetArray->At( iPresetArrayPos ) );
        if ( iReqFromArrayPos < iReqFromArray->Count() && 
             iReqFromArray->At(iReqFromArrayPos ) == 0 )
            {
            FetchLogoDataL( tempPreset );
            }
        else if ( iObserverArrayPos < iObserverArray->Count() )
            {
            FetchLogoDataL( tempPreset,
                            iObserverArray->At( iObserverArrayPos ),
                            iReqFromArray->At( iReqFromArrayPos ) );
            ++iObserverArrayPos;
            }
        ++iPresetArrayPos;
        ++iReqFromArrayPos;
        // delete tempPreset;
        CleanupStack::PopAndDestroy( tempPreset );
        }
    IRLOG_DEBUG( "CIRLogoDownloadEngine::ManagePresetArrayIndexL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::ManageCachePresetArrayIndexL()
// To make another download request if exists in the queue
// To update the iCachePresetArray current index
// ---------------------------------------------------------------------------
void CIRLogoDownloadEngine::ManageCachePresetArrayIndexL()
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::ManageCachePresetArrayIndexL - Exiting" );
    TInt CachePresetArrayLength = iCachePresetArray->Count();
    if ( ( CachePresetArrayLength > 0 ) && 
         ( iCachePresetArrayPos < CachePresetArrayLength ) )
        {
        CIRIsdsPreset* tempPreset;
        tempPreset = CIRIsdsPreset::NewL();
        CleanupStack::PushL( tempPreset );
        *tempPreset = *( iCachePresetArray->At( iCachePresetArrayPos ) );

        if ( iCacheReqFromArrayPos < iCacheReqFromArray->Count() &&
             iCacheReqFromArray->At( iCacheReqFromArrayPos ) == 0 )
            {
            SendRequestL( tempPreset, KLogoSize, KLogoSize );
            }
        else if ( iCacheObserverArrayPos < iCacheObserverArray->Count() )
            {
            SendRequestL( tempPreset, 
                          iCacheObserverArray->At( iCacheObserverArrayPos ), 
                          iCacheReqFromArray->At( iCacheReqFromArrayPos ) );
            ++iCacheObserverArrayPos;
            }
        ++iCachePresetArrayPos;
        ++iCacheReqFromArrayPos;
        // delete tempPreset;
        CleanupStack::PopAndDestroy( tempPreset );
        }
    IRLOG_DEBUG( "CIRLogoDownloadEngine::ManageCachePresetArrayIndexL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::CacheError()
// Called from cachemgmt in case of an error
// callback API from MIRCacheObserver
// ---------------------------------------------------------------------------
void CIRLogoDownloadEngine::CacheError()
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::CacheError" );

    //code here, if needed
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::CacheFailed()
// Called from cachemgmt in case of cache failure
// callback API from MIRCacheObserver
// ---------------------------------------------------------------------------
void CIRLogoDownloadEngine::CacheFailed()
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::CacheFailed" );
    //code here, if needed
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::CacheInvalid()
// Called from cachemgmt in case cache is invalid
// callback API from MIRCacheObserver
// ---------------------------------------------------------------------------
void CIRLogoDownloadEngine::CacheInvalid()
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::CacheInvalid - Entering" );
    iETag = iCache->iETag;
    IRLOG_DEBUG( "CIRLogoDownloadEngine::CacheInvalid - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::CachedStructureL(TInt aChoice)
// Called from cachemgmt data retrival is successful
// callback API from MIRCacheObserver
// ---------------------------------------------------------------------------
void CIRLogoDownloadEngine::CachedStructureL( TInt aChoice )
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::CachedStructureL - Entering" );
    if ( iCacheReqMade )
        {
        iCacheReqMade = EFalse;
        if ( aChoice == KCacheAvailable )
            {

            TInt sizeImgUrlWithPmts = 0;
            //Remove the imgUrl with parameters from the Preset
            //now set imgUrl without parameters in to the Preset
            if ( iSendingLogo == 0 )
                {
                sizeImgUrlWithPmts = iCacheTempPreset->GetImgUrl().Length();
                }
            else if ( iSendingLogo == 1 )
                {
                sizeImgUrlWithPmts = iTempPreset->GetImgUrl().Length();
                }
            else if ( iSendingLogo == 2 )
                {
                sizeImgUrlWithPmts = KTen;
                }
            TBool hasParameters = EFalse;
            RBuf urlWithPar;
            CleanupClosePushL(urlWithPar);
            urlWithPar.CreateL( sizeImgUrlWithPmts );
            if ( iSendingLogo == 0 )
                {
                urlWithPar.Copy( iCacheTempPreset->GetImgUrl() );
                }
            else if ( iSendingLogo == 1 )
                {
                urlWithPar.Copy( iTempPreset->GetImgUrl() );
                }
            else if ( iSendingLogo == 2 )
                {
                _LIT(KNone,"");
                urlWithPar.Copy( KNone );
                }

            RBuf tempBuf;
            CleanupClosePushL(tempBuf);
            tempBuf.CreateL( sizeImgUrlWithPmts );

            TLex urlLex( urlWithPar );
            for ( ; !urlLex.Eos(); )
                {
                tempBuf.Zero();
                while ( urlLex.Peek() != '?' && !urlLex.Eos() )
                    {
                    tempBuf.Append( urlLex.Get() );
                    }
                if ( urlLex.Peek() == '?' )
                    {
                    hasParameters = ETrue;
                    }
                urlLex.Inc();
                }

            RBuf imgUrl;
            CleanupClosePushL(imgUrl);
            if ( hasParameters )
                {
                //let say urlWithPar has the value http://www.logos.com/images/test.pngx=100&y=100
                //now the tempBuf contains the value  x=100&y=100
                TInt sizeUrlParameters = tempBuf.Length();
                sizeUrlParameters++; // now this integer contains the size of x=100&y=100 
                TInt sizeImgUrl = sizeImgUrlWithPmts - sizeUrlParameters;

                //Create a buffer to store the imgUrl without parameters
                imgUrl.CreateL( sizeImgUrl );
                imgUrl.Copy( urlWithPar.Left( sizeImgUrl ) );
                if ( iSendingLogo == 0 )
                    {
                    iCacheTempPreset->SetImgUrlL( imgUrl );
                    }
                else if ( iSendingLogo == 1 )
                    {
                    iTempPreset->SetImgUrlL( imgUrl );
                    }
                }
            else
                {
                //nothing to be done
                //the imgUrl present in the preset is correct, no need to change anything
                }

            if ( iSendingLogo == 0 )
                {
                //logo is available in the cache
                if ( aChoice == KCacheAvailable )
                    {
                    iCacheTempPreset->SetLogoDataL( iCache->iLogoData );
                    }
                if ( iCacheReqFrom == 0 )
                    {
                    iPresetModifiedStatus = ETrue;
                    }
                else
                    {
                    iCacheTempLogoHandle->PresetLogoDownloadedL(
                                    iCacheTempPreset );
                    }

                ManageCachePresetArrayIndexL();
                }
            else if ( iSendingLogo == 1 )
                {
                //logo is available in the cache
                if ( aChoice == KCacheAvailable )
                    {
                    iTempPreset->SetLogoDataL( iCache->iLogoData );
                    }
                if ( iTempReqFrom == 0 )
                    {
                    iPresetModifiedStatus = ETrue;
                    }
                else
                    {
                    iTempLogoHandle->PresetLogoDownloadedL( iTempPreset );
                    }

                ManagePresetArrayIndexL();
                }
            //logo request from the search results (while displaying logo)
            else if ( iSendingLogo == 2 )
                {
                //code here, if needed
                }
            //close all the buffers
            CleanupStack::PopAndDestroy(3);
            }
        }
    IRLOG_DEBUG( "CIRLogoDownloadEngine::CachedStructureL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRLogoDownloadEngine::ReleaseResources()
// Releases the resources held by logo download engine
// Used to close the Http Session if already open
// ---------------------------------------------------------------------------
void CIRLogoDownloadEngine::ReleaseResources()
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::ReleaseResources - Entering" );
    CancelTransaction();
    iLogoSession.Close();
    iFirstTime = ETrue;
    IRLOG_DEBUG( "CIRLogoDownloadEngine::ReleaseResources - Exiting" );
    }

// -----------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// Notified by network controller when network is active, to reissue 
// the request NotifyActiveNetworkObserversL()
// ---------------------------------------------------------------------------

void CIRLogoDownloadEngine::NotifyActiveNetworkObserversL(
                                                      TIRNetworkEvent aEvent )
    {
    IRLOG_DEBUG( "CIRLogoDownloadEngine::NotifyActiveNetworkObserversL - Entering" );
    switch ( aEvent )
        {
        case ENetworkConnectionDisconnected:
            {
            ReleaseResources();
            }
            break;
        case ENetworkConnectionEstablished:
            {
            if ( iLogoRequestPending )
                {
                IssueLogoDownloadRequestL();
                } 
            }
        }
    IRLOG_DEBUG( "CIRLogoDownloadEngine::NotifyActiveNetworkObserversL - Exiting" );
    }

// ---------------------------------------------------------------------------
// Notified by network controller when user cancels network connection, 
// to reset the pending requests  
// ResetPendingRequests()
// ---------------------------------------------------------------------------
void CIRLogoDownloadEngine::ResetPendingRequests( TBool /*aValue*/)
    {
    // nothing to do
    }
