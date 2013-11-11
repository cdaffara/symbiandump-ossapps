/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The implementation for presentation elements.
*
*/


#include <escapeutils.h>
#include <tinternetdate.h>
#include <utf.h>

#include "irbrowsecatagoryitems.h"
#include "irbrowsechannelitems.h"
#include "ircachemgmt.h"
#include "irdataprovider.h"
#include "irdataproviderconstants.h"
#include "irdebug.h"
#include "irhttprequestdata.h"
#include "irotaupdate.h"
#include "irsessionlogger.h"
#include "irsettings.h"
#include "irxmlcontenthandler.h"
#include "isdsclientdll.h"
#include "misdsresponseobserver.h"
#include "irisdspreset.h"

const TInt KChannelidMaxLength = 25;
const TInt KTwo = 2;
/*const TInt KDays = 7;
const TInt KHour = 24;
const TInt KMinute = 60;
const TInt KSecond = 60;*/
const TInt KUrlPassMaxLength = 124;
const TInt KChidMaxLength = 6;
const TInt KCopyOfTimeMaxLength = 32;
const TInt KMaxLength = 256;
const TInt KFour = 4;

_LIT(KBrowseUrl,"/do/directory/browse?type=");
_LIT(KChannelnSearchUrl,"/do/directory/browse?type=channels&searchText=");
_LIT(KGetIRIDUrl,"/do/service/getIrid");
_LIT(KGenres,"genres");
_LIT(KLanguages,"languages");
_LIT(KCountries,"countries");
_LIT(KHotpicks,"hotpicks");
_LIT(KOtaUpdateUrl,"/do/service/otaUpdate?irid=");
_LIT(KLogUsageUrl,"/do/service/logUsage");
_LIT(KSearchUrl,"&start=1&size=50");
_LIT(KMultiSearchUrl, "/do/directory/browse?type=channels&genreID=%S&countryID=%S&languageID=%S&searchText=");

// ---------------------------------------------------------------------------
// CIRIsdsClient::NewL(MIsdsResponseObserver& aobserver)
// Used by UI to create instance of CIRIsdsClient.
// ---------------------------------------------------------------------------
//
EXPORT_C CIRIsdsClient* CIRIsdsClient::NewL( MIsdsResponseObserver& aobserver,
    const TDesC& aISDSBaseUrl )
    {
    IRLOG_DEBUG( "CIRIsdsClient::NewL - Entering" );
    CIRIsdsClient* self = CIRIsdsClient::NewLC( aobserver, aISDSBaseUrl );

    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRIsdsClient::NewL - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRIsdsClient::NewLC(MIsdsResponseObserver& aobserver)
// Creates instance of CIRIsdsClient.
// ---------------------------------------------------------------------------
//
CIRIsdsClient* CIRIsdsClient::NewLC( MIsdsResponseObserver& aobserver,
                                     const TDesC& aISDSBaseUrl )
    {
    IRLOG_DEBUG( "CIRIsdsClient::NewLC - Entering" );
    CIRIsdsClient* self = new ( ELeave ) CIRIsdsClient( aobserver );
    CleanupStack::PushL( self );
    self->ConstructL( aISDSBaseUrl );
    IRLOG_DEBUG( "CIRIsdsClient::NewLC - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRIsdsClient::ConstructL()
// Symbian two phased constructor
// ---------------------------------------------------------------------------
//
void CIRIsdsClient::ConstructL( const TDesC& aISDSBaseUrl )
    {
    //!Construsting the DataproviderDLL instance
    IRLOG_DEBUG( "IRIsdsClient::ConstructL - Entering" );
    iSettings = CIRSettings::OpenL();
    iISDSBaseUrl = aISDSBaseUrl.AllocL();
    //Base URL required to browse through the category
    iCatgoryURL.Zero();
    iCatgoryURL.Copy( *iISDSBaseUrl );
    iCatgoryURL.Append( KBrowseUrl );
    //Base URL required to search isds 
    iSearchURL.Zero();
    iSearchURL.Copy( *iISDSBaseUrl );
    iSearchURL.Append( KChannelnSearchUrl );
    //Base URL required to get IRID
    iGetIridUrl.Zero();
    iGetIridUrl.Copy( *iISDSBaseUrl );
    iGetIridUrl.Append( KGetIRIDUrl );
    //creates DataProvider
    iDataProvider = CIRDataProvider::NewL( *this );
    //creates XML Parser
    iXmlReader = CIRXMLContentHandler::NewL( *this, *this );
    //creates Cache manager
    iCache = CIRCacheMgmt::OpenL( *this );
    iCache->AddObserverL( this );
    //Session Log
    iBrowseReport = CIRReportGenerator::OpenL();
    // Retrive the current language and set it as an Accept Language
    TLanguage currentLanguage = User::Language();
    iShortLanguageCode = IRLanguageMapper::MapLanguage( currentLanguage );

    IRLOG_DEBUG( "IRIsdsClient::ConstructL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRIsdsClient::CIRIsdsClient(MIsdsResponseObserver& aobserver)
// Default constructor
// ---------------------------------------------------------------------------
//
CIRIsdsClient::CIRIsdsClient( MIsdsResponseObserver& aobserver )
 : iIsdsResponseObserver( aobserver )
    {
    IRLOG_DEBUG( "CIRIsdsClient::CIRIsdsClient" );
    //No implementation
    }

// ---------------------------------------------------------------------------
// CIRIsdsClient::~CIRIsdsClient()
// Destructor
// ---------------------------------------------------------------------------
//
CIRIsdsClient::~CIRIsdsClient()
    {
    IRLOG_DEBUG( "CIRIsdsClient::~CIRIsdsClient - Entering" );
    delete iDataProvider;
    delete iXmlReader;
    if ( iSendPreset )
        {
        iSendPreset->ResetAndDestroy();
        }
    delete iSendPreset;

    if ( iCache )
        {
        iCache->RemoveObserver( this );
        iCache->Close();
        }

    if ( iSettings )
        {
        iSettings->Close();
        }
    delete iISDSBaseUrl;
    if ( iBrowseReport )
        {
        iBrowseReport->Close();
        }
    if ( iTempRequestObject )
        {
        delete iTempRequestObject;
        iTempRequestObject = NULL;
        }

    IRLOG_DEBUG( "CIRIsdsClient::~CIRIsdsClient - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRIsdsClient::IRIsdsIsCategoryCachedL
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CIRIsdsClient::IRIsdsIsCategoryCachedL(
             TIRIsdsclientInterfaceIDs aISDSClientRequest)
    { 
    iForceGet=EFalse;  
    TInt res = 0;
    iCacheReqMade = EFalse;
    switch( aISDSClientRequest )
        {
            //When genre data is requested by user
        case EGenre:
            {
            //set the request type for subsiquent Use           
             
            iCachePath.Zero();
            iCachePath.Copy( KGenres );            
            iCache->CheckCacheL( 0, KGenres, iForceGet, res );       
            }
          
        break;
        //When Language data is requested by user                       
        case ELanguages:
            {
            
            iCachePath.Zero();
            iCachePath.Copy( KLanguages );            
            iCache->CheckCacheL( 0, KLanguages, iForceGet, res );          
          
            }
        break;
        //When Country data is requested by user                                                    
        case  ECountries:
            {
         
            iCachePath.Zero();
            iCachePath.Copy( KCountries );          
            iCache->CheckCacheL( 0, KCountries, iForceGet, res );          
            }
        break;
                    
            //When hotpics data is requested by user                                                        
        case  Ehotpicks:
            {
            //set the request type for subsiquent Use            
            iCachePath.Zero();
            iCachePath.Copy( KHotpicks );
            iCache->CheckCacheL( 1, KHotpicks, iForceGet, res );
            
            }
            break;   
        default:
            break;    
    }
    
    if ( res == CIRCacheMgmt::ECacheUseable )
        {
        return ETrue;
        }
    
    return EFalse; 
    }
// ---------------------------------------------------------------------------
// void CIRIsdsClientInteface(TIRIsdsclientInterfaceIDs aISDSClientRequest)
// Called by UI to make request to ISDS for catogories
// @param :ENUM value depending on catogory which user wants to explore
// ---------------------------------------------------------------------------
//

EXPORT_C TBool CIRIsdsClient::IRIsdsClientIntefaceL(
                                TIRIsdsclientInterfaceIDs aISDSClientRequest )
    {
    //cancels any pending request
    IRLOG_DEBUG( "CIRIsdsClient::IRIsdsClientIntefaceL - Entering" );
    CIRHttpRequestData * requestData = new ( ELeave ) CIRHttpRequestData;
    CleanupStack::PushL( requestData );
    requestData->iAcceptLanguage.Copy( iShortLanguageCode );
    //Sets the Htpp method to GET
    requestData->iMethod = EIRHttpGET;
    //check for valid cache
    iForceGet = EFalse;

    TInt res = 0;

    switch ( aISDSClientRequest )
        {
        //When genre data is requested by user
        case EGenre:
            {
            //set the request type for subsiquent Use
            iRequestType = 0;
            iDataFrom = EFalse;
            iCachePath.Zero();
            iCachePath.Copy( KGenres );
            iCacheReqMade = ETrue;
            iCache->CheckCacheL( 0, KGenres, iForceGet, res );
            if ( res == 0 )//if cache is not available
                {
                requestData->iUri.Copy( iCatgoryURL );
                requestData->iUri.Append( KGenres );
                iConditonalGet = EFalse;
                //issues request for Genre data
                IRHttpIssueRequestL( *requestData );
                iBrowseUrl.Zero();
                iBrowseUrl.Copy( requestData->iUri );
                iBrowseReport->BrowseUrl( iBrowseUrl, 0 );
                iDataFrom = ETrue;
                }
            if ( res == -1 )//i cache is stale
                {
                iConditonalGet = ETrue;
                requestData->iIfModifiedSince = iLastModifiedTime.DateTime();
                requestData->isIfModifiedSet = ETrue;
                requestData->iUri.Copy( iCatgoryURL );
                requestData->iUri.Append( KGenres );
                IRHttpIssueRequestL( *requestData );
                iBrowseUrl.Zero();
                iBrowseUrl.Copy( requestData->iUri );
                iBrowseReport->BrowseUrl( iBrowseUrl, 0 );
                iDataFrom = ETrue;
                }
            if ( res == 1 )
                {
                requestData->iUri.Copy( iCatgoryURL );
                requestData->iUri.Append( KGenres );
                iBrowseUrl.Zero();
                iBrowseUrl.Copy( requestData->iUri );
                iBrowseReport->BrowseUrl( iBrowseUrl, 0 );
                }
            }
            break;
            //When Language data is requested by user
        case ELanguages:
            {
            //set the request type for subsiquent Use
            iRequestType = 0;
            iDataFrom = EFalse;

            iCachePath.Zero();
            iCachePath.Copy( KLanguages );
            iCacheReqMade = ETrue;
            iCache->CheckCacheL( 0, KLanguages, iForceGet, res );

            if ( res == 0 )//if cache is not available
                {
                requestData->iUri.Copy( iCatgoryURL );
                requestData->iUri.Append( KLanguages );
                //Isuues request for language data
                IRHttpIssueRequestL( *requestData );
                iConditonalGet = EFalse;
                iBrowseUrl.Zero();
                iBrowseUrl.Copy( requestData->iUri );
                iBrowseReport->BrowseUrl( iBrowseUrl, 0 );
                iDataFrom = ETrue;
                }
            if ( res == -1 )//i cache is stale
                {
                iConditonalGet = ETrue;
                requestData->iIfModifiedSince = iLastModifiedTime.DateTime();
                requestData->isIfModifiedSet = ETrue;
                requestData->iUri.Copy( iCatgoryURL );
                requestData->iUri.Append( KLanguages );
                //Isuues request for language data if cache isnt valid
                IRHttpIssueRequestL( *requestData );
                iBrowseUrl.Zero();
                iBrowseUrl.Copy( requestData->iUri );
                iBrowseReport->BrowseUrl( iBrowseUrl, 0 );
                iDataFrom = ETrue;
                }
            if ( res == 1 )
                {
                requestData->iUri.Copy( iCatgoryURL );
                requestData->iUri.Append( KLanguages );
                iBrowseUrl.Zero();
                iBrowseUrl.Copy( requestData->iUri );
                iBrowseReport->BrowseUrl( iBrowseUrl, 0 );
                }
            }
            break;
            //When Country data is requested by user
        case ECountries:
            {
            //set the request type for subsiquent Use
            iRequestType = 0;
            iDataFrom = EFalse;

            iCachePath.Zero();
            iCachePath.Copy( KCountries );
            iCacheReqMade = ETrue;
            iCache->CheckCacheL( 0, KCountries, iForceGet, res );
            if ( res == 0 )//if cache is not available
                {
                requestData->iUri.Copy( iCatgoryURL );
                requestData->iUri.Append( KCountries );
                //Isuues request for country data
                IRHttpIssueRequestL( *requestData );
                iConditonalGet = EFalse;
                iBrowseUrl.Zero();
                iBrowseUrl.Copy( requestData->iUri );
                iBrowseReport->BrowseUrl( iBrowseUrl, 0 );
                iDataFrom = ETrue;
                }
            if ( res == -1 )//i cache is stale
                {
                iConditonalGet = ETrue;
                requestData->iIfModifiedSince = iLastModifiedTime.DateTime();
                requestData->isIfModifiedSet = ETrue;
                requestData->iUri.Copy( iCatgoryURL );
                requestData->iUri.Append( KCountries );
                //Isuues request for country data if cache isnt valid
                IRHttpIssueRequestL( *requestData );
                iBrowseUrl.Zero();
                iBrowseUrl.Copy( requestData->iUri );
                iBrowseReport->BrowseUrl( iBrowseUrl, 0 );
                iDataFrom = ETrue;
                }
            if ( res == 1 )
                {
                requestData->iUri.Copy( iCatgoryURL );
                requestData->iUri.Append( KCountries );
                iBrowseUrl.Zero();
                iBrowseUrl.Copy( requestData->iUri );
                iBrowseReport->BrowseUrl( iBrowseUrl, 0 );
                }

            }
            break;

            //When hotpics data is requested by user
        case Ehotpicks:
            {
            //set the request type for subsiquent Use
            iRequestType = 1;
            iCacheReqMade = ETrue;
            iCache->CheckCacheL( 1, KHotpicks, iForceGet, res );
            iCachePath.Zero();
            iCachePath.Copy( KHotpicks );
            iDataFrom = EFalse;

            if ( res == 0 )//if cache is not available
                {
                requestData->iUri.Copy( iCatgoryURL );
                requestData->iUri.Append( KHotpicks );
                //Isuues request for hotpics data
                IRHttpIssueRequestL( *requestData );
                iConditonalGet = EFalse;
                iBrowseUrl.Zero();
                iBrowseUrl.Copy( requestData->iUri );
                iBrowseReport->BrowseUrl( iBrowseUrl, 0 );
                iDataFrom = ETrue;
                }
            if ( res == -1 )//i cache is stale
                {
                iConditonalGet = ETrue;
                requestData->iIfModifiedSince = iLastModifiedTime.DateTime();
                requestData->isIfModifiedSet = ETrue;
                requestData->iUri.Copy( iCatgoryURL );
                requestData->iUri.Append( KHotpicks );
                //Isuues request for hotpics data if cache isnt valid
                IRHttpIssueRequestL( *requestData );
                iBrowseUrl.Zero();
                iBrowseUrl.Copy( requestData->iUri );
                iBrowseReport->BrowseUrl( iBrowseUrl, 0 );
                iDataFrom = ETrue;
                }
            if ( res == 1 )
                {
                requestData->iUri.Copy( iCatgoryURL );
                requestData->iUri.Append( KHotpicks );
                iBrowseUrl.Zero();
                iBrowseUrl.Copy( requestData->iUri );
                iBrowseReport->BrowseUrl( iBrowseUrl, 0 );
                }
            }
            break;

        }
    CleanupStack::PopAndDestroy( requestData );
    IRLOG_DEBUG( "CIRIsdsClient::IRIsdsClientIntefaceL - Exiting." );
    return iDataFrom;
    }//end of function

// ---------------------------------------------------------------------------
// CIRIsdsClient::GetDataProvider
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CIRDataProvider* CIRIsdsClient::GetDataProvider()
    {
    IRLOG_DEBUG( "CIRIsdsClient::GetDataProvider" );
    return iDataProvider;
    }

// ---------------------------------------------------------------------------
// void IRHttpContentNotChanged()
// this is call back funtion called by Dataprovider in case 
// HTTP content isnt changed by ISDS 
// ---------------------------------------------------------------------------
//
void CIRIsdsClient::IRHttpResponseCodeReceived( TInt aResponseCode,
                CIRHttpResponseData& aResponseHeaders )
    {
    IRLOG_INFO2( "CIRIsdsClient::IRHttpResponseCodeReceived (%d)", aResponseCode );
    TInt res = KErrNotFound;
    switch ( aResponseCode )
        {
        case KNotFound://404
            {
            //make channel user defined.
            if ( iSyncRequest )
                {
                iSyncRequest = EFalse;
                //make the preset user-defined
                TRAP_IGNORE(iIsdsResponseObserver.IsdsPresetRemovedL(
                                                    iPresetToBeSynced))
            }

            /*if(iOtaReqFrom)
             {
             TRAP_IGNORE(PurgeOtaInfoL())
             }*/

            }
            break;
        case KNotModified://304
            {
            if ( iConditonalGet )
                {
                //update the trust period 
                //no problem if it leaves
                TRAP_IGNORE( iCache->UpdateTrustPeriodL( iRequestType,iCachePath,
                                                    aResponseHeaders ) )
                //do a forced get because the data is notmodifed in isds and 
                //trustable
                iForceGet = ETrue;
                iCacheReqMade = ETrue;
                TRAP_IGNORE( iCache->CheckCacheL( iRequestType,iCachePath,
                                                    iForceGet,res ) )
                }
            /*if(iOtaReqFrom)
             {
             TRAP_IGNORE(PurgeOtaInfoL())
             }*/
            }
            break;
        default:
            {

            }
            break;
        }
    IRLOG_DEBUG( "CIRIsdsClient::IRHttpResponseCodeReceived - Exiting." );
    }

// ---------------------------------------------------------------------------
// void RHttpGeneralError(TInt aErrorCode)
// It is a call back function called by the data provider 
// to return error code in case of some error.
// ---------------------------------------------------------------------------
//
void CIRIsdsClient::IRHttpGeneralError( TInt aErrorCode )
    {
    IRLOG_DEBUG( "CIRIsdsClient::IRHttpGeneralError - Entering" );
    IRLOG_ERROR2( "CIRIsdsClient::IRHttpGeneralError (%d)", aErrorCode );
    if ( iSyncRequest )
        {
        IRLOG_DEBUG( "CIRISDSCLIENT::IRHTTPGENERALERROR--IF ISYNC" );
        iSyncRequest = EFalse;
        if ( aErrorCode == KNotModified || aErrorCode == KServiceUnavailable )
            {
            TRAP_IGNORE( iIsdsResponseObserver.IsdsPresetNoChangeL() )
            return;
            }
        IRLOG_DEBUG( "CIRIsdsClient::IRHttpGeneralError - Exiting (1)." );
        }

    /*if(iOtaReqFrom)
     {
     //if internal service request
     //reset the flag
     iOtaReqFrom = EFalse
     IRLOG_DEBUG( "CIRIsdsClient::IRHttpGeneralError - Exiting (2)." )
     return
     }*/
    if ( iConditonalGet )
        {
        IRLOG_DEBUG( "CIRIsdsClient::IRHttpGeneralError - Exiting (3)." );
        iConditonalGet = EFalse;
        if ( aErrorCode == KNotModified )
            {
            return;
            }
        }
    if ( iPostResponsePending )
        {
        IRLOG_DEBUG( "CIRIsdsClient::IRHttpGeneralError - Exiting (4)." );
        IRLOG_DEBUG( "CIRISDSCLIENT::ISDSGENERALERROR--POSTPENDING" );
        iPostResponsePending = EFalse;
        //no action required
        //session log is backed up before sending;
        return;
        }
    if ( iIRIDPending )
        {
        IRRDEBUG2("CIRISDSCLIENT::IRHTTPGENERALERROR--IF IRID",KNullDesC );
        IRLOG_DEBUG( "CIRIsdsClient::IRHttpGeneralError - Exiting (7)." );
        iIRIDPending = EFalse;
        //dTRAP_IGNORE( IRGetIRIDL() )
        }
    IRLOG_DEBUG("CIRISDSCLIENT::IRHTTPGENERALERROR--BEFORE TRAPD" );
    TRAPD( errd, iIsdsResponseObserver.IsdsErrorL( aErrorCode ) );
    if ( errd )
        {
        IRLOG_DEBUG("CIRISDSCLIENT::IRHTTPGENERALERROR--IF ERRD" );
        IRLOG_DEBUG( "CIRIsdsClient::IRHttpGeneralError - Exiting (5)." );
        return;
        }
    IRLOG_DEBUG( "CIRIsdsClient::IRHttpGeneralError - Exiting (6)." );
    }

// ---------------------------------------------------------------------------
// void IRHttpDataReceived(TDesC& aXmlPath)
// It is a call back function called by the data provider 
// once it recieves the XML data
// ---------------------------------------------------------------------------
//
void CIRIsdsClient::IRHttpDataReceived( const TDesC& aXmlPath,
                const CIRHttpResponseData& aResponseHeaders )
    {
    IRLOG_DEBUG( "CIRIsdsClient::IRHttpDataReceived - Entering" );
    TBuf<KMaxLength> xmlPath;
    xmlPath.Copy( aXmlPath );
    //!Calls XML parser with file path to XML file recieved
    TRAPD( xmlErr,iXmlReader->ParseXmlL( xmlPath, iCachePath, aResponseHeaders ) )
    if ( xmlErr != KErrNone )
        {
        IRLOG_DEBUG( "CIRIsdsClient::IRHttpDataReceived - Exiting (1)." );
        return;
        }

    if ( iIRIDPending )
        {
        iIRIDPending = EFalse;
        //if any request is pending by the time IRID response comes
        if ( iReqPending )
            {
                //issue the pending request
                TRAP_IGNORE( IRHttpIssueRequestL( *iTempRequestObject ) )
            }
        }

    IRLOG_DEBUG( "CIRIsdsClient::IRHttpDataReceived - Exiting (2)." );
    }

// ---------------------------------------------------------------------------
// CIRIsdsClient::IRIsdsIsChannelCachedL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CIRIsdsClient::IRIsdsIsChannelCachedL( TUint aSelectedOption )
    {   
    TInt res = 0;
    iCacheReqMade = EFalse;
    iForceGet=EFalse;
     
     //previous data from cache
    if (iCatDataFrom)
    {
        if (aSelectedOption >= iCache->iPtrCategory->Count())
        {
            User::Leave(KErrArgument);
        }

        if (iCache->iPtrCategory->Count())
        {
            iCachePath.Zero();
            iCachePath.Copy(
                    (*(iCache->iPtrCategory))[aSelectedOption]->iCatId->Des());

            iCache->CheckCacheL(1,
                    iCachePath,
                    iForceGet, res);
        }
    }
    //previous data from xml
    else
    {
        if (aSelectedOption >= iXmlReader->iPtrCategory->Count())
        {
            User::Leave( KErrArgument );
        }

        if (iXmlReader->iPtrCategory->Count())
        {
            iCachePath.Zero();
            iCachePath.Copy(
                    (*(iXmlReader-> iPtrCategory))[aSelectedOption]->iCatId->Des());
            iCache->CheckCacheL(
                    1,
                    iCachePath,
                    iForceGet, res );
        }
    }
     
    if( CIRCacheMgmt::ECacheUseable == res )
        return ETrue;
    
    return EFalse;  
    }

// ---------------------------------------------------------------------------
// void CIRIsdsClientInteface(TUint aSelectedOption,TIRIsdsClientInterfaces aChoice)
// This API is used by the UI to make request for isds listed channels
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CIRIsdsClient::IRIsdsClientIntefaceL( TUint aSelectedOption,
                TIRIsdsClientInterfaces aChoice, TInt aHistoryBool )
    {
    IRLOG_DEBUG( "CIRIsdsClient::IRIsdsClientIntefaceL - Entering" );
    iForceGet = EFalse;
    CIRHttpRequestData * requestData = new ( ELeave ) CIRHttpRequestData;
    CleanupStack::PushL( requestData );
    requestData->iMethod = EIRHttpGET;
    requestData->iAcceptLanguage.Copy( iShortLanguageCode );
    requestData->iUri.Copy( *iISDSBaseUrl );

    TBool isAdvPresent = EFalse;

    TInt res( 0 );
    if ( aChoice == ECatagory )
        {
        //set the request type for subsiquent Use
        iRequestType = 1;
        //previous data from cache
        if ( iCatDataFrom )
            {
            TInt categoryCacheIndex;

            if ( ( *( iCache->iPtrCategory ) )[0]->iCatGetOperation )
                {
                isAdvPresent = EFalse;
                }
            else
                {
                isAdvPresent = ETrue;
                }
            if ( !isAdvPresent )
                {
                categoryCacheIndex = 0;
                }
            else
                {
                categoryCacheIndex = 1;
                }

            if ( aSelectedOption >= iCache->iPtrCategory->Count() )
                {
                User::Leave( KErrArgument );
                }

            if ( iCache->iPtrCategory->Count() )
                {
                iCachePath.Zero();
                iCachePath.Copy(
                                ( *( iCache->iPtrCategory ) )[aSelectedOption]->iCatId->Des() );
                iCacheReqMade = ETrue;
                iCache->CheckCacheL(
                                1,
                                ( *( iCache->iPtrCategory ) )[aSelectedOption]-> iCatId->Des(),
                                iForceGet, res );
                iDataFrom = EFalse;
                if ( res == 0 )
                    {
                    requestData->iUri.Append(
                                    ( *( iCache-> iPtrCategory ) )[categoryCacheIndex]->iCatGetOperation->Des() );
                    requestData->iUri.Append(
                                    ( *( iCache-> iPtrCategory ) )[aSelectedOption]->iCatId->Des() );
                    IRHttpIssueRequestL( *requestData );
                    iConditonalGet = EFalse;
                    iBrowseUrl.Copy( requestData->iUri );
                    iBrowseReport->BrowseUrl( iBrowseUrl, 0 );
                    iDataFrom = ETrue;
                    }
                if ( res == -1 )
                    {
                    iConditonalGet = ETrue;
                    requestData->iIfModifiedSince
                                    = iLastModifiedTime.DateTime();
                    requestData->isIfModifiedSet = ETrue;
                    requestData->iUri.Append(
                                    ( *( iCache-> iPtrCategory ) )[categoryCacheIndex]->iCatGetOperation->Des() ); //0
                    requestData->iUri.Append(
                                    ( *( iCache-> iPtrCategory ) )[aSelectedOption]->iCatId->Des() );
                    IRHttpIssueRequestL( *requestData );
                    iBrowseUrl.Copy( requestData->iUri );
                    iBrowseReport->BrowseUrl( iBrowseUrl, 0 );
                    iDataFrom = ETrue;
                    }
                }
            }
        //previous data from xml
        else
            {
            TInt categoryXmlIndex;
            if ( ( *( iXmlReader->iPtrCategory ) )[0]->iCatGetOperation )
                {
                isAdvPresent = EFalse;
                }
            else
                {
                isAdvPresent = ETrue;
                }
            if ( !isAdvPresent )
                {
                categoryXmlIndex = 0;
                }
            else
                {
                categoryXmlIndex = 1;
                }

            if ( aSelectedOption >= iXmlReader->iPtrCategory->Count() )
                {
                User::Leave( KErrArgument );
                }

            if ( iXmlReader->iPtrCategory->Count() )
                {
                iCachePath.Zero();
                iCachePath.Copy(
                                ( *( iXmlReader-> iPtrCategory ) )[aSelectedOption]->iCatId->Des() );
                iCacheReqMade = ETrue;
                iCache->CheckCacheL(
                                1,
                                ( *( iXmlReader-> iPtrCategory ) )[aSelectedOption]->iCatId->Des(),
                                iForceGet, res );
                iDataFrom = EFalse;
                if ( res == 0 )
                    {
                    requestData->iUri.Append(
                                    ( *( iXmlReader-> iPtrCategory ) )[categoryXmlIndex]->iCatGetOperation->Des() );
                    requestData->iUri.Append(
                                    ( *( iXmlReader-> iPtrCategory ) )[aSelectedOption]->iCatId->Des() );
                    IRHttpIssueRequestL( *requestData );
                    iConditonalGet = EFalse;
                    iBrowseUrl.Copy( requestData->iUri );
                    iBrowseReport->BrowseUrl( iBrowseUrl, 0 );
                    iDataFrom = ETrue;
                    }
                if ( res == -1 )
                    {
                    iConditonalGet = ETrue;
                    requestData->iIfModifiedSince
                                    = iLastModifiedTime.DateTime();
                    requestData->isIfModifiedSet = ETrue;
                    requestData->iUri.Append(
                                    ( *( iXmlReader-> iPtrCategory ) )[categoryXmlIndex]->iCatGetOperation->Des() );
                    requestData->iUri.Append(
                                    ( *( iXmlReader-> iPtrCategory ) )[aSelectedOption]->iCatId->Des() );
                    IRHttpIssueRequestL( *requestData );
                    iBrowseUrl.Copy( requestData->iUri );
                    iBrowseReport->BrowseUrl( iBrowseUrl, 0 );
                    iDataFrom = ETrue;
                    }
                }
            }
        }
    else if ( aChoice == EChannels )
        {

        TBuf<KChannelidMaxLength> channelid;
        //set the request type for subsiquent Use
        iRequestType = KTwo;
        _LIT(KFormat,"%d");
        if ( aHistoryBool )
            {
            _LIT(KPresetFetchUrl,"/do/directory/getPreset?channelid=");
            requestData->iUri.Append( KPresetFetchUrl );
            TBuf<KChannelidMaxLength> channelid;
            _LIT(KFormat,"%d");
            channelid.Format( KFormat, aSelectedOption );
            requestData->iUri.Append( channelid );
            IRHttpIssueRequestL( *requestData );
            iConditonalGet = EFalse;
            //Session Log
            iBrowseUrl.Zero();
            iBrowseUrl.Copy( requestData->iUri );
            iBrowseReport->BrowseUrl( iBrowseUrl, 1 );
            iDataFrom = ETrue;
            CleanupStack::PopAndDestroy( requestData );
            return iDataFrom;
            }

        //previous data from cache
        if ( iChannelDataFrom )
            {
            TInt channelCacheIndex;
            if ( ( *( iCache->iPtrChannel ) )[0]->iChannelGetOperation )
                {
                isAdvPresent = EFalse;
                }
            else
                {
                isAdvPresent = ETrue;
                }
            if ( !isAdvPresent )
                {
                channelCacheIndex = 0;
                }
            else
                {
                channelCacheIndex = 1;
                }

            if( aSelectedOption >= iCache->iPtrChannel->Count())
                {
                User::Leave( KErrArgument );
                }

            if ( iCache->iPtrChannel->Count() )
                {
                channelid.Format(
                                KFormat,
                                ( *( iCache-> iPtrChannel ) )[aSelectedOption]->iChannelID );
                iCachePath.Zero();
                iCachePath.Copy( channelid );
                iCacheReqMade = ETrue;
                iCache->CheckCacheL( KTwo, channelid, iForceGet, res );
                iDataFrom = EFalse;
                if ( res == 0 || res == -1 )
                    {
                    requestData->iUri.Append(
                                    ( *( iCache-> iPtrChannel ) )[channelCacheIndex]->iChannelGetOperation->Des() );
                    requestData->iUri.Append( channelid );
                    IRHttpIssueRequestL( *requestData );
                    iConditonalGet = EFalse;
                    //Session Log
                    iBrowseUrl.Copy( requestData->iUri );
                    iBrowseReport->BrowseUrl( iBrowseUrl, 1 );
                    iDataFrom = ETrue;
                    }
                }
            }
        //previous data from xml
        else
            {
            TInt channelXmlIndex;
            if ( 0 == ( *( iXmlReader->iPtrChannel ) ).Count() )
                {
                CleanupStack::PopAndDestroy( requestData );
                IRLOG_DEBUG( "CIRIsdsClient::IRIsdsClientIntefaceL - Error!" );
                return iDataFrom;
                }

            if ( ( *( iXmlReader->iPtrChannel ) )[0]->iChannelGetOperation )
                {
                isAdvPresent = EFalse;
                }
            else
                {
                isAdvPresent = ETrue;
                }
            if ( !isAdvPresent )
                {
                channelXmlIndex = 0;
                }
            else
                {
                channelXmlIndex = 1;
                }

            if( aSelectedOption >= iXmlReader->iPtrChannel->Count())
                {
                User::Leave( KErrArgument );
                }

            if ( iXmlReader->iPtrChannel->Count() )
                {
                channelid.Format(
                                KFormat,
                                ( *( iXmlReader-> iPtrChannel ) )[aSelectedOption]->iChannelID );
                iCachePath.Zero();
                iCachePath.Copy( channelid );
                iCacheReqMade = ETrue;
                iCache->CheckCacheL( KTwo, channelid, iForceGet, res );
                iDataFrom = EFalse;
                if ( res == 0 || res == -1 )
                    {
                    requestData->iUri.Append(
                                    ( *( iXmlReader-> iPtrChannel ) )[channelXmlIndex]->iChannelGetOperation->Des() );
                    requestData->iUri.Append( channelid );
                    IRHttpIssueRequestL( *requestData );
                    iConditonalGet = EFalse;
                    //Session Log
                    iBrowseUrl.Copy( requestData->iUri );
                    iBrowseReport->BrowseUrl( iBrowseUrl, 1 );
                    iDataFrom = ETrue;
                    }
                }
            }
        }
    else
        {

        }
    CleanupStack::PopAndDestroy( requestData );
    IRLOG_DEBUG( "CIRIsdsClient::IRIsdsClientIntefaceL - Exiting" );
    return iDataFrom;
    }

// ---------------------------------------------------------------------------
// void CIRGetIRID()
// This API is used by the UI to get IR ID from 
// isds if it doesnt have one
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsClient::IRGetIRIDL()
    {
    IRLOG_DEBUG( "CIRIsdsClient::IRGetIRIDL - Entering" );
    //if irid is not available fetch it
    if ( iSettings->GetIRIDL().Length() == 0 )
        {
        iReqFromGetIRIDFunc = ETrue;
        iIRIDPending = EFalse;
        CIRHttpRequestData *requestData = new ( ELeave ) CIRHttpRequestData();
        CleanupStack::PushL( requestData );
        requestData->iMethod = EIRHttpGET;
        requestData->iAcceptLanguage.Copy( iShortLanguageCode );
        requestData->iUri.Copy( iGetIridUrl );
        IRHttpIssueRequestL( *requestData );
        CleanupStack::PopAndDestroy( requestData );
        iIRIDPending = ETrue;
        IRLOG_DEBUG( "CIRIsdsClient::IRGetIRIDL - Exiting (1)." );
        return;
        }
    IRLOG_DEBUG( "CIRIsdsClient::IRGetIRIDL - Exiting (2)." );
    }

// ---------------------------------------------------------------------------
// void GetOtaStatus()
// This API is used by the UI to query for ota update 
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsClient::GetOtaStatusL(TBool& aDataFrom)
    {
    IRLOG_DEBUG( "CIRIsdsClient::GetOtaStatusL - Entering" );

    CIRHttpRequestData *requestData = new ( ELeave ) CIRHttpRequestData();

    CleanupStack::PushL( requestData );

    requestData->iMethod = EIRHttpGET;
    requestData->iAcceptLanguage.Copy( iShortLanguageCode );
    iGetOtaStatus.Zero();
    iGetOtaStatus.Copy( *iISDSBaseUrl );
    iGetOtaStatus.Append( KOtaUpdateUrl );
    iGetOtaStatus.Append( iSettings->GetIRIDL() );

    //set the request type for subsiquent Use
    iRequestType = EOtaInfo;
    iDataFrom = EFalse;
    iCachePath.Zero();
    iCachePath.Copy( KOtaUpdateUrl );
    TInt res = 0;
    //check cache if it is a genral service request
    //not initiated by the user.
    if ( iOtaReqFrom )
        {
        iCacheReqMade = ETrue;
        iCache->CheckCacheL( EOtaInfo, KOtaUpdateUrl, iForceGet, res );
        }
    if ( res == 1 )
        {
        aDataFrom = EFalse;
        }
    //if cache is not available or stale(dont use the trust period val)
    if ( res == 0 || res == -1 )
        {
        requestData->iUri.Copy( iGetOtaStatus );
        iConditonalGet = EFalse;
        //issues request for ota data
        iDataProvider->IRHttpIssueRequestL( *requestData );
        iDataFrom = ETrue;
        aDataFrom = ETrue;
        }
    CleanupStack::PopAndDestroy( requestData );

    IRLOG_DEBUG( "CIRIsdsClient::GetOtaStatusL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRIsdsClient::IRISDSPostL()
// This API is used to Post Irapp usage report to isds
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsClient::IRISDSPostL(TFileName &/*aFileName*/)
    {
    IRLOG_DEBUG( "CIRIsdsClient::IRISDSPostL - Entering" );
    IRRDEBUG2("CIRISDSCLIENT::ISDSPOSTL",KNullDesC);
    //Cancels any pending requests
    IRISDSCancelRequest();
    CIRHttpRequestData * requestData = new ( ELeave ) CIRHttpRequestData;
    CleanupStack::PushL( requestData );
    //Sets the Http Method to POST
    requestData->iMethod = EIRHttpPOST;
    requestData->iAcceptLanguage.Copy( iShortLanguageCode );
    requestData->iUri.Copy( *iISDSBaseUrl );
    requestData->iUri.Append( KLogUsageUrl );
    //issues request to post the log usage report to isds
    iPostResponsePending = ETrue;
    IRHttpIssueRequestL( *requestData );
    CleanupStack::PopAndDestroy( requestData );
    IRLOG_DEBUG( "CIRIsdsClient::IRISDSPostL - Exiting" );
    }

// ---------------------------------------------------------------------------
// void CIRISDSCancelRequest()
// This API is used by the UI to cancel a request made.
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsClient::IRISDSCancelRequest()
    {
    IRLOG_DEBUG( "CIRIsdsClient::IRISDSCancelRequest - Entering" );
    iSyncRequest = EFalse;
    iReqFromSync = EFalse;
    iOtaReqFrom = EFalse;
    iConditonalGet = EFalse;
    iPostResponsePending = EFalse;
    iIRIDPending = EFalse;
    iDataProvider->IRHttpCancelRequest();
    IRLOG_DEBUG( "CIRIsdsClient::IRISDSCancelRequest - Exiting" );
    }

// ---------------------------------------------------------------------------
// void IRISDSMultiSearchL()
// This API is used by the UI to search a channel.
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsClient::IRISDSMultiSearchL(const TDesC& aGenreID, 
                             const TDesC& aCountryID, 
                             const TDesC& aLanguageID, 
                             const TDesC& aSearchText)
    {
    IRLOG_DEBUG( "CIRIsdsClient::IRISDSMultiSearch - Entering" );
    CIRHttpRequestData * requestData= new(ELeave) CIRHttpRequestData();
    iCachePath.Zero();  
    CleanupStack::PushL(requestData);
    
    requestData->iMethod = EIRHttpGET;
    requestData->iAcceptLanguage.Copy( iShortLanguageCode );
    
    HBufC8* utfEncodedSearchText = CnvUtfConverter::ConvertFromUnicodeToUtf8L( aSearchText );     
    CleanupStack::PushL( utfEncodedSearchText );    
    
     
    HBufC8 *encodedQuery = NULL; 
    encodedQuery = EscapeUtils::EscapeEncodeL(*utfEncodedSearchText, 
                                EscapeUtils::EEscapeQuery);       
       
    HBufC* searchURL = HBufC::NewLC(iISDSBaseUrl->Length() 
                                    + KMultiSearchUrl().Length() 
                                    + aGenreID.Length() 
                                    + aLanguageID.Length() 
                                    + aCountryID.Length() 
                                    + KSearchUrl().Length() 
                                    + utfEncodedSearchText->Size()/2+1 );
    
    HBufC* searchPartURL = HBufC::NewLC( KMultiSearchUrl().Length() 
                                       + aGenreID.Length() 
                                       + aLanguageID.Length() 
                                       + aCountryID.Length() );
    
    searchPartURL->Des().Format( KMultiSearchUrl, &aGenreID, &aCountryID, 
                                &aLanguageID );
    
    HBufC* searchText = HBufC::NewLC( utfEncodedSearchText->Size()/2+1 );
    TPtr searchTextPtr( searchText->Des() );
    searchTextPtr.Copy( *utfEncodedSearchText );
     
    TPtr uriPtr( searchURL->Des() );
    uriPtr.Append( *iISDSBaseUrl );
    uriPtr.Append( *searchPartURL );
    uriPtr.Append( searchTextPtr );
    uriPtr.Append( KSearchUrl );    
    
    if ( encodedQuery )
    {
        delete encodedQuery;
    }   
    
    requestData->iUri.Copy( searchURL->Left( requestData->iUri.MaxLength() ) );
    IRHttpIssueRequestL( *requestData );
    CleanupStack::PopAndDestroy( searchText );
    CleanupStack::PopAndDestroy( searchPartURL );    
    CleanupStack::PopAndDestroy( searchURL );
    CleanupStack::PopAndDestroy( utfEncodedSearchText );
    
    
    iBrowseUrl.Zero();
    iBrowseUrl.Copy( requestData->iUri );
    EncodeUrlL( iBrowseUrl );
    iBrowseReport->BrowseUrl( iBrowseUrl, 1 );
    CleanupStack::PopAndDestroy( requestData );   
    
    IRLOG_DEBUG( "CIRIsdsClient::IRISDSMultiSearch - Exiting." );
    }

// ---------------------------------------------------------------------------
// void CIRISDSSearch(TDesC& aSearchString)
// This API is used by the UI to search channels from isds
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsClient::IRISDSSearchL(const TDesC& aSearchString)
    {
    IRLOG_DEBUG( "CIRIsdsClient::IRISDSSearchL - Entering" );
    CIRHttpRequestData * requestData = new ( ELeave ) CIRHttpRequestData();
    iCachePath.Zero();
    CleanupStack::PushL( requestData );
    //Sets the Http method to GET
    requestData->iMethod = EIRHttpGET;
    requestData->iAcceptLanguage.Copy( iShortLanguageCode );

    // This is a blatant hack to circumvent the fact that the application has hard-coded
    // 256 to be the maximum length of the URI everywhere. Due to time-restraints it is
    // not feasible to fix the actual issue here, so every search query that would exceed
    // the said limit is truncated to fit it.
    //
    // The practical impacts of this hack is that should the maximum length be exceeded,
    // a connection timed out query is most often displayed. 
    //
    // The scope of this quick hack is to enable users to perform search with UTF-8 characters
    // with no possibility of application panics due to descriptor overflows, not to fix
    // the actual problem with the hard-coded TBufs in the application.

    HBufC8* utfEncoded = CnvUtfConverter::ConvertFromUnicodeToUtf8L(
                    aSearchString );
    CleanupStack::PushL( utfEncoded );
    HBufC8* uri = HBufC8::NewL( iSearchURL.Size() + utfEncoded->Size()
                    + KSearchUrl().Size() );
    TPtr8 uriPtr( uri->Des() );
    uriPtr.Copy( iSearchURL );

    HBufC8 *encodedQuery = NULL;
    encodedQuery = EscapeUtils::EscapeEncodeL( *utfEncoded,
                    EscapeUtils::EEscapeQuery );
    uriPtr.Append( *encodedQuery );
    uriPtr.Append( KSearchUrl );
    if ( encodedQuery )
        {
        delete encodedQuery;
        }
    CleanupStack::PopAndDestroy( utfEncoded );
    requestData->iUri.Copy( uri->Left( requestData->iUri.MaxLength() ) );
    delete uri;

    IRHttpIssueRequestL( *requestData );
    iBrowseUrl.Zero();
    iBrowseUrl.Copy( requestData->iUri );
    EncodeUrlL( iBrowseUrl );
    iBrowseReport->BrowseUrl( iBrowseUrl, 1 );
    CleanupStack::PopAndDestroy( requestData );
    IRLOG_DEBUG( "CIRIsdsClient::IRISDSSearchL - Exiting." );
    }

// ---------------------------------------------------------------------------
//void ParsedStructure(TInt aChoice)
//Call back funtion called by XML parser after parsing is over
// ---------------------------------------------------------------------------
//
void  CIRIsdsClient::ParsedStructureL(TInt aChoice)
    {
    IRLOG_DEBUG( "CIRIsdsClient::ParsedStructureL - Entering" );
    //if catogory info is recieved
    if ( aChoice == ECategoryInfoRecieved )
        {
        iCatDataFrom = EFalse;
        iIsdsResponseObserver.IsdsCatogoryDataReceivedL(
                        *( iXmlReader-> iPtrCategory ) );
        }
    // if channels list is recieved
    else if ( aChoice == EChannelListRecieved )
        {
        iChannelDataFrom = EFalse;
        iIsdsResponseObserver.IsdsChannelDataReceivedL(
                        *( iXmlReader->iPtrChannel ) );
        }

    //if preset data is recieved
    else if ( aChoice == EPresetDataRecieved )
        {
        if ( iSyncRequest )
            {
            iSyncRequest = EFalse;
            //update the preset(replace it)
            iIsdsResponseObserver.IsdsPresetChangedL(
                            *iXmlReader->iPtrPreset->At( 0 ) );
            }
        else
            {
            iIsdsResponseObserver.IsdsPresetDataReceivedL(
                            *( iXmlReader-> iPtrPreset ) );
            }
        }
    /*else if(aChoice==EOtaInfoRecieved)
     {
     if(!iOtaReqFrom)
     {
     iIsdsResponseObserver.IsdsOtaInfoRecieved(*iXmlReader->iOTA)
     }
     else
     {
     iOtaReqFrom=EFalse;
     //update settings
     if(iXmlReader->iOTA->iUpdateInterval!=0)
     {
     iSettings->SetSilencePeriodL(iXmlReader->iOTA->iUpdateInterval)
     }
     else
     {
     iSettings->SetSilencePeriodL(KDays*KHour*KMinute*KSecond)
     }
     }
     }*/
    else if ( aChoice == EIridRecieved )
        {
        iSettings->SetIRIDL( iXmlReader->iRID );
        }
    else if ( aChoice == ESessionLogRecieved )
        {
        //session log is recieved in ISDS
        iPostResponsePending = EFalse;
        iBrowseReport->ReponseFromISDSObtained();
        }

    else
        {
        TRAPD(errd,iIsdsResponseObserver.IsdsErrorL( KDataProviderTimeout) );
        if ( errd )
            {
            IRLOG_DEBUG( "CIRIsdsClient::ParsedStructureL - Exiting ." );
            return;
            }
        }
    IRLOG_DEBUG( "CIRIsdsClient::ParsedStructureL - Exiting." );
    }
    
// ---------------------------------------------------------------------------
// void ParseError()
// Call back funtion called by XML parser in case of parsing error
// ---------------------------------------------------------------------------
//
void CIRIsdsClient::ParseError( TInt aErrorCode )
    {
    IRLOG_ERROR( "CIRIsdsClient::ParseError - Entering" );

    IRHttpGeneralError( aErrorCode );
    IRLOG_DEBUG( "CIRIsdsClient::ParseError - Exiting." );
    }

// ---------------------------------------------------------------------------
// void CacheError()
// 
// ---------------------------------------------------------------------------
//
void CIRIsdsClient::CacheError()
    {
    IRLOG_ERROR( "CIRIsdsClient::CacheError" );
    }

// ---------------------------------------------------------------------------
// void CacheFailed()
// 
// ---------------------------------------------------------------------------
//
void CIRIsdsClient::CacheFailed()
    {
    IRLOG_WARNING( "CIRIsdsClient::CacheFailed" );
    }

// ---------------------------------------------------------------------------
// cache is present but Stale.
// called by cache mgmt as a callback.
// after copying the lastmodified time into its variable
// here it is copied into the variable ilastModifiedTime 
// ---------------------------------------------------------------------------
//
void CIRIsdsClient::CacheInvalid()
    {
    IRLOG_DEBUG( "CIRIsdsClient::CacheInvalid - Entering" );
    iLastModifiedTime = iCache->iLastModified;
    IRLOG_DEBUG( "CIRIsdsClient::CacheInvalid - Exiting" );
    }

// ---------------------------------------------------------------------------
// void CachedStructureL()
// 
// ---------------------------------------------------------------------------
//
void CIRIsdsClient::CachedStructureL( TInt aChoice )
    {
    IRLOG_DEBUG( "CIRIsdsClient::CachedStructureL - Entering" );
    if ( iCacheReqMade )
        {
        iCacheReqMade = EFalse;
        //if catogory info is recieved
        if ( aChoice == ECategoryInfoRecieved )
            {
            iCatDataFrom = ETrue;
            iIsdsResponseObserver.IsdsCatogoryDataReceivedL(
                            *( iCache->iPtrCategory ) );
            }

        // if channels list is recieved
        else if ( aChoice == EChannelListRecieved )
            {
            iChannelDataFrom = ETrue;
            iIsdsResponseObserver.IsdsChannelDataReceivedL(
                            *( iCache->iPtrChannel ) );
            }

        //if preset data is recieved
        else if ( aChoice == EPresetDataRecieved )
            {
            //multiple presets can be presented to the UI.
            iIsdsResponseObserver.IsdsPresetDataReceivedL(
                            *( iCache->iPtrPreset ) );
            }

        //if ota data is recieved
        /*else if(aChoice==EOtaInfoRecieved)
         {
         if(!iOtaReqFrom)
         {
         iIsdsResponseObserver.IsdsOtaInfoRecieved(*iCache->iOTA)
         }
         else
         {
         iOtaReqFrom=EFalse;
         //update settings
         if(iCache->iOTA->iUpdateInterval!=0)
         {
         iSettings->SetSilencePeriodL(iCache->iOTA->iUpdateInterval)
         }
         else
         {
         iSettings->SetSilencePeriodL(KDays*KHour*KMinute*KSecond)
         }
         }
         }*/
        //if logo data is recieved
        else if ( aChoice == KFour )
            {
            //No implementation
            }
        else
            {
            TRAPD( errd, iIsdsResponseObserver.IsdsErrorL( KDataProviderTimeout ) );
            if ( errd )
                {
                IRLOG_DEBUG( "CIRIsdsClient::CachedStructureL- Exiting ." );
                return;
                }
            }
        }
    IRLOG_DEBUG( "CIRIsdsClient::CachedStructureL - Exiting." );
    }

// ---------------------------------------------------------------------------
// void CIRIsdsClient::SyncPresetL()
// Used to syncronize presets
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRIsdsClient::SyncPresetL(TInt aPresetId,const TDesC& 
    aIfModifiedSince )
    {
    IRLOG_DEBUG( "CIRIsdsClient::SyncPresetL - Entering" );
    IRRDEBUG2("CIRISDCLIENT::SYNCPRESETL",KNullDesC);
    iReqFromSync = ETrue;
    iPresetToBeSynced = aPresetId;
    CIRHttpRequestData * requestData = new ( ELeave ) CIRHttpRequestData;
    CleanupStack::PushL( requestData );
    requestData->iMethod = EIRHttpGET;

    requestData->iAcceptLanguage.Copy( iShortLanguageCode );
    TDateTime lastModified;
    ConvertToTimeDate( aIfModifiedSince, lastModified );
    requestData->isIfModifiedSet = ETrue;
    requestData->iIfModifiedSince.Set( lastModified.Year(),
                    lastModified.Month(), lastModified.Day(),
                    lastModified.Hour(), lastModified.Minute(),
                    lastModified.Second(), lastModified.MicroSecond() );
    _LIT(KPresetFetchUrl,"/do/directory/getPreset?channelid=");

    TBuf<KUrlPassMaxLength> urlPass( iSettings->GetISDSBaseUrlL() );
    urlPass.Append( KPresetFetchUrl );

    TBuf<KChidMaxLength> chid;
    _LIT(KFormat,"%d");
    chid.Format( KFormat, aPresetId );
    urlPass.Append( chid );
    requestData->iUri.Copy( urlPass );

    IRHttpIssueRequestL( *requestData );
    CleanupStack::PopAndDestroy( requestData );
    IRLOG_DEBUG( "CIRIsdsClient::SyncPresetL - Exiting." );
    return 1;
    }

// ---------------------------------------------------------------------------
// void ReleaseResources()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsClient::ReleaseResources()
    {
    IRLOG_DEBUG( "CIRIsdsClient::ReleaseResources - Entering" );
    iDataProvider->ReleaseResources();
    IRLOG_DEBUG( "CIRIsdsClient::ReleaseResources - Exiting" );
    }

//---------------------------------------------------------------------------
//CIRIsdsClient::GetMultiplePresetsL
//for multiple selected presets to be saved 
//
//---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsClient::GetMultiplePresetsL(
                const RArray<TInt>& aIndexArray, TBool& aDataFrom )
    {
    IRLOG_DEBUG( "CIRIsdsClient::GetMultiplePresetsL - Entering" );
    _LIT(KSeparator,",");
    //get the count of the no of requested presets
    TInt multiSelectedCount = aIndexArray.Count();
    //allocate space for the request url
    HBufC8* channelIds = HBufC8::NewLC( multiSelectedCount
                    * KDefaultRealWidth + multiSelectedCount );
    //http request data
    CIRHttpRequestData* requestData = new ( ELeave ) CIRHttpRequestData;
    requestData->iUri.Copy( *iISDSBaseUrl );
    CleanupStack::PushL( requestData );
    requestData->iAcceptLanguage.Copy( iShortLanguageCode );
    //create a tptr to modify it
    TPtr8 channelIdPtr = channelIds->Des();
    //set the request type 
    iRequestType = EPresets;
    //result
    TInt res;
    _LIT8(KFormat,"%d");
    //if from cache
    if ( iChannelDataFrom )
        {
        for ( TInt i = 0; i < multiSelectedCount; i++ )
            {
            ASSERT( aIndexArray[i] >= 0 && aIndexArray[i] < iCache->iPtrChannel->Count() );
            TInt channelID = iCache->iPtrChannel->At( aIndexArray[i] )->iChannelID;
            if ( i != 0 )
                {
                channelIdPtr.Append( KSeparator );
                }
            TBuf8<KChannelidMaxLength> channelIdBuf;
            channelIdBuf.Format( KFormat, channelID );
            channelIdPtr.Append( channelIdBuf );
            }
        iCachePath.Zero();
        iCachePath.Copy( *channelIds );
        //check if cache is available
        iCacheReqMade = ETrue;
        iCache->CheckCacheL( EPresets, iCachePath, iForceGet, res );
        aDataFrom = EFalse;
        if ( res == KErrNone )
            {
            requestData->iUri.Append(
                            ( *( iCache-> iPtrChannel ) )[0]->iChannelGetOperation->Des() );
            requestData->iUri.Append( *channelIds );
            IRHttpIssueRequestL( *requestData );
            iConditonalGet = EFalse;
            //Session Log
            iBrowseUrl.Copy( requestData->iUri );
            iBrowseReport->BrowseUrl( iBrowseUrl, 1 );
            aDataFrom = ETrue;
            }
        if ( res == KErrNotFound )
            {
            iConditonalGet = ETrue;
            requestData->iIfModifiedSince = iLastModifiedTime.DateTime();
            requestData->isIfModifiedSet = ETrue;
            requestData->iUri.Append(
                            ( *( iCache-> iPtrChannel ) )[0]->iChannelGetOperation->Des() );
            requestData->iUri.Append( *channelIds );
            IRHttpIssueRequestL( *requestData );
            iBrowseUrl.Copy( requestData->iUri );
            iBrowseReport->BrowseUrl( iBrowseUrl, 1 );
            aDataFrom = ETrue;
            }
        }
    //if from xml file
    else
        {
        for ( TInt i = 0; i < multiSelectedCount; i++ )
            {
            ASSERT( aIndexArray[i] >= 0 && aIndexArray[i] < iXmlReader->iPtrChannel->Count() );
            TInt channelID = iXmlReader->iPtrChannel->At( aIndexArray[i] )->iChannelID;
            if ( i != 0 )
                {
                channelIdPtr.Append( KSeparator );
                }
            TBuf8<KChannelidMaxLength> channelIdBuf;
            channelIdBuf.Format( KFormat, channelID );
            channelIdPtr.Append( channelIdBuf );
            }
        iCachePath.Zero();
        iCachePath.Copy( *channelIds );
        iCacheReqMade = ETrue;
        iCache->CheckCacheL( EPresets, iCachePath, iForceGet, res );
        aDataFrom = EFalse;
        if ( res == KErrNone )
            {
            requestData->iUri.Append(
                  ( *( iXmlReader-> iPtrChannel ) )[0]->iChannelGetOperation->Des() );
            requestData->iUri.Append( *channelIds );
            IRHttpIssueRequestL( *requestData );
            iConditonalGet = EFalse;
            //Session Log
            iBrowseUrl.Copy( requestData->iUri );
            iBrowseReport->BrowseUrl( iBrowseUrl, 1 );
            aDataFrom = ETrue;
            }
        if ( res == KErrNotFound )
            {
            iConditonalGet = ETrue;
            requestData->iIfModifiedSince = iLastModifiedTime.DateTime();
            requestData->isIfModifiedSet = ETrue;
            requestData->iUri.Append(
                  ( *( iXmlReader-> iPtrChannel ) )[0]->iChannelGetOperation->Des() );
            requestData->iUri.Append( *channelIds );
            IRHttpIssueRequestL( *requestData );
            //Session Log
            iBrowseUrl.Copy( requestData->iUri );
            iBrowseReport->BrowseUrl( iBrowseUrl, 1 );
            aDataFrom = ETrue;
            }
        }
    CleanupStack::PopAndDestroy( requestData );
    CleanupStack::PopAndDestroy( channelIds );
    IRLOG_DEBUG( "CIRIsdsClient::GetMultiplePresetsL - Exiting" );
    }

//---------------------------------------------------------------------------
//CIRIsdsClient::ConvertToTimeDate
//converts the date-time stamp (last modified) to Symbian DateTime format
//---------------------------------------------------------------------------
void CIRIsdsClient::ConvertToTimeDate( const TDesC& aTime,
                TDateTime& aDateTime ) const
    {
    IRLOG_DEBUG( "CIRIsdsClient::ConvertToTimeDate - Entering" );
    TBuf8<KCopyOfTimeMaxLength> copyOfTime;
    copyOfTime.Copy( aTime );
    TInternetDate time;
    TRAPD( error,time.SetDateL( copyOfTime ) );
    if ( !error )
        {
        aDateTime = time.DateTime();
        }

    IRLOG_DEBUG( "CIRIsdsClient::ConvertToTimeDate - Exiting." );
    }

//---------------------------------------------------------------------------
//CIRIsdsClient::IRHttpIssueRequestL
//Issues Http Request to DataProvider to get data from URL in Request object
//The Request URL is also encoded here
//---------------------------------------------------------------------------
//
void CIRIsdsClient::IRHttpIssueRequestL( CIRHttpRequestData &aRequestObject )
    {
    IRLOG_DEBUG( "CIRIsdsClient::IRHttpIssueRequestL - Entering" );
    IRRDEBUG2("CIRISDSCLIENT::IRHTTPISSUEREQUESTL",KNullDesC);
    if ( iIRIDPending )
        {
        //if IRID reqeust is pending then queue the reuest 
        //till IRID response comes
        iReqPending = ETrue;
        //temporary request object
        iTempRequestObject = new ( ELeave ) CIRHttpRequestData;
        iTempRequestObject->iMethod = aRequestObject.iMethod;
        iTempRequestObject->iIfModifiedSince
                        = aRequestObject.iIfModifiedSince;
        iTempRequestObject->isIfModifiedSet = aRequestObject.isIfModifiedSet;
        iTempRequestObject->iAcceptLanguage = aRequestObject.iAcceptLanguage;
        iTempRequestObject->iContentType = aRequestObject.iContentType;
        iTempRequestObject->iUri = aRequestObject.iUri;
        }
    else
        {
        if ( iSettings->GetIRIDL().Length() == 0 )
            {
            if ( iReqFromGetIRIDFunc )
                {
                iReqFromGetIRIDFunc = EFalse;
                iDataProvider->IRHttpIssueRequestL( aRequestObject );
                }
            else
                {
                //When the user cancels the http req,irid is not present.
                //On next request,automatically first irid req is made.
                //save the actual request
                //get irid from isds
                //once irid is received send the saved request to isds
                iReqPending = ETrue;
                //temporary request object
                iTempRequestObject = new ( ELeave ) CIRHttpRequestData;
                iTempRequestObject->iMethod = aRequestObject.iMethod;
                iTempRequestObject->iIfModifiedSince
                                = aRequestObject.iIfModifiedSince;
                iTempRequestObject->isIfModifiedSet
                                = aRequestObject.isIfModifiedSet;
                iTempRequestObject->iAcceptLanguage
                                = aRequestObject.iAcceptLanguage;
                iTempRequestObject->iContentType
                                = aRequestObject.iContentType;
                iTempRequestObject->iUri = aRequestObject.iUri;
                IRGetIRIDL();
                }
            }
        else
            {
            iReqPending = EFalse;
            if ( iReqFromSync )
                {
                iReqFromSync = EFalse;
                iSyncRequest = ETrue;
                }
            iDataProvider->IRHttpIssueRequestL( aRequestObject );
            }
        }
    IRLOG_DEBUG( "CIRIsdsClient::IRHttpIssueRequestL - Exiting." );
    }

//---------------------------------------------------------------------------
//CIRIsdsClient::EncodeUrlL
//This url encoding is for session log
//---------------------------------------------------------------------------
//
void CIRIsdsClient::EncodeUrlL( TDes &aQuery )
    {
    IRLOG_DEBUG( "CIRIsdsClient::EncodeUrlL - Entering" );
    //Encodes the URL inside the request object
    HBufC *encodedurl = NULL;
    encodedurl = EscapeUtils::EscapeEncodeL( aQuery,
                    EscapeUtils::EEscapeUrlEncoded );
    aQuery.Copy( encodedurl->Left( aQuery.MaxLength() ) );
    //encoded url is released
    if ( encodedurl )
        {
        delete encodedurl;
        }
    IRLOG_DEBUG( "CIRIsdsClient::EncodeUrlL - Exiting." );
    }

//---------------------------------------------------------------------------
//CIRIsdsClient::PurgeOtaInfoL
//delete all the data cached wrt OTA
//---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsClient::PurgeOtaInfoL()
    {
    IRLOG_DEBUG( "CIRIsdsClient::PurgeOtaInfoL - Entering" );
    //reset update available flag to 0
    iSettings->ReSetUpdateAvailableL();
    //reset silence period to 0
    iSettings->SetSilencePeriodL( 0 );
    //reset silence period start time to a zero string
    iSettings->SetSilencePeriodStartL( KNullDesC );
    //clear the cache info if any
    TRAP_IGNORE( iCache->RemoveOtaInfoL() )
    IRLOG_DEBUG( "CIRIsdsClient::PurgeOtaInfoL - Exiting" );
    }
