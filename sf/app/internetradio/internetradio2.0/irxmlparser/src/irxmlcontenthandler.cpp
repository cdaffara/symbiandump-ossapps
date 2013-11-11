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


#include <f32file.h>
#include <parser.h>
#include <utf.h>

#include "iradvertisement.h"
#include "irbrowsecatagoryitems.h"
#include "irbrowsechannelitems.h"
#include "ircachemgmt.h"
#include "irdebug.h"
#include "irisdspreset.h"
#include "irparserobserver.h"
#include "irotaupdate.h"
#include "irxmlcontenthandler.h"
#include "irxmlelements.h"

const TInt KLargeArraySize = 256;
const TUint8 KAttributeBufferSize = 100;
const TUint8 KBitRateCounterValue0 = 0;
const TUint8 KBitRateCounterValue1 = 1;
const TUint8 KBitRateCounterValue2 = 2;
const TUint8 KCategoriesItemSelect = 0;
const TUint8 KChannelItemSelect = 1;
const TUint8 KPresetItemSelect = 2;
const TUint8 KOTAUpdateItemSelect = 3;
const TUint8 KIRIDSelect = 4;
const TUint8 KTen = 10; 

// ---------------------------------------------------------------------------
// Gets the Filepath of the XML file
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRXMLContentHandler::ParseXmlL( const TDesC& aFilePath,
                           const TDesC& aCachePath,
                         const CIRHttpResponseData& aResponseHeaders )
    { 
    IRLOG_DEBUG( "CIRXMLContentHandler::ParseXmlL" );
    iFileToCache.Zero( );
    iFileToCache.Copy( aCachePath );     
     
    RFs fs;
    CleanupClosePushL( fs );
    User::LeaveIfError( fs.Connect( ) );           
    Xml::ParseL( *iParser, fs, aFilePath );
    CleanupStack::PopAndDestroy( &fs );

   
    //! Error in the Document
    if ( iEOD != KErrNone )
        {
        ResetValues( );
        iParseObserver.ParseError( iEOD ); 
        }
    else
        {    
        if ( iCategories )
            {
            TRAP_IGNORE( iCache->CacheCategoryItemsL( *iPtrCategory, 
            iFileToCache , aResponseHeaders ) )
            ResetValues();    
            iParseObserver.ParsedStructureL( KCategoriesItemSelect );
            }
    
		if ( iChan )            
			{        
			if ( iFileToCache.Length( ) != 0 )
				{
				TRAP_IGNORE( iCache->CacheChannelItemsL( *iPtrChannel, 
				iFileToCache ,aResponseHeaders ) )
				}
			ResetValues( );
			iParseObserver.ParsedStructureL( KChannelItemSelect );
		    }
		
		if ( iPresets )
			 {
		     if ( 0 < iPtrPreset->Count( ) )
                 {
                 TRAP_IGNORE( iCache->CachePresetItemL( *iPtrPreset,
                   iFileToCache, aResponseHeaders ) )
                 ResetValues( );
                 iParseObserver.ParsedStructureL( KPresetItemSelect );     
                 }        
             }  
        if ( iOTAUpdate )
            {
            TRAP_IGNORE( iCache->CacheOtaInfoL( *iOTA,
                iFileToCache, aResponseHeaders ) )
            ResetValues( );
            iParseObserver.ParsedStructureL( KOTAUpdateItemSelect );
            } 
        if ( iIRID )
            {
            ResetValues( );
            iParseObserver.ParsedStructureL( KIRIDSelect );
            }      
        if ( iTimeStamp )
            {
            //session log response is obtained
            ResetValues( );
            iParseObserver.ParsedStructureL( KSessionLogResponse );   
            }
        }
    IRLOG_DEBUG( "CIRXMLContentHandler::ParseXmlL - Exiting." );    
  }  

// ---------------------------------------------------------------------------
// CIRXMLContentHandler ::NewL
// Constructor
// ---------------------------------------------------------------------------
// 
EXPORT_C CIRXMLContentHandler* CIRXMLContentHandler ::NewL( MIRParserObserver 
    &aObserver, MIRCacheObserver &aCacheObserver )
    {    
    IRLOG_DEBUG( "CIRXMLContentHandler::NewL" );
    CIRXMLContentHandler* self = new (ELeave) CIRXMLContentHandler( aObserver,
      aCacheObserver );    
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRXMLContentHandler::NewL - Exiting." );
    return( self ); 
    }
// ---------------------------------------------------------------------------
// CIRXMLContentHandler ::CIRXMLContentHandler
// Default Constructor
// ---------------------------------------------------------------------------
//
CIRXMLContentHandler ::CIRXMLContentHandler( MIRParserObserver &aObserver,
    MIRCacheObserver &aCacheObserver ) : iParseObserver( aObserver ),
    iCacheObserver ( aCacheObserver )
    {
    IRLOG_DEBUG( "CIRXMLContentHandler::CIRXMLContentHandler" );
    }
    
// ---------------------------------------------------------------------------
// CIRXMLContentHandler ::ConstructL
// Two-Phase Constructor
// ---------------------------------------------------------------------------
//
void CIRXMLContentHandler ::ConstructL()
    {
    IRLOG_DEBUG( "CIRXMLContentHandler::ConstructL" );
    ResetValues( );    
    TIRXMLElementFormat::OpenStringPoolL( iStringPool );
    
    iPtrCategory = new (ELeave) CArrayPtrFlat<CIRBrowseCatagoryItems>(
      KLargeArraySize );
    iPtrChannel = new (ELeave) CArrayPtrFlat<CIRBrowseChannelItems>(
      KLargeArraySize );
    iPtrPreset = new (ELeave) CArrayPtrFlat<CIRIsdsPreset>( KLargeArraySize );
    
    iParser = CParser::NewL( KFile, *this );
    iCache = CIRCacheMgmt::OpenL( iCacheObserver );

    iBufGetOperation = HBufC::NewL( 0 );
    iSize = HBufC::NewL( 0 );
    IRLOG_DEBUG( "CIRXMLContentHandler::ConstructL - Exiting." );
    }
// ---------------------------------------------------------------------------
// CIRXMLContentHandler::~CIRXMLContentHandler()
// Destructing the object
// ---------------------------------------------------------------------------
//
CIRXMLContentHandler::~CIRXMLContentHandler()
    {    
    IRLOG_DEBUG( "CIRXMLContentHandler::~CIRXMLContentHandler." );           
    TRAP_IGNORE( TIRXMLElementFormat::CloseStringPoolL( iStringPool ) )
    delete iParser;
    if ( iUrlContentForAds )
        {
        delete iUrlContentForAds;
        iUrlContentForAds = NULL;
        }
    if ( iPtrPreset )
        {
        iPtrPreset->ResetAndDestroy();    
        }
        delete iPtrPreset;
    if ( iPtrCategory )
        {
        iPtrCategory->ResetAndDestroy();
        }       
        delete iPtrCategory;
    if ( iPtrChannel )
        {
        iPtrChannel->ResetAndDestroy();
        }
    delete iPtrChannel;
    
    delete iOTA;
    delete iAdv;
    
    if ( iCache )
         {
         iCache->Close();
         }
    
    delete iPreset;
    delete iCategory;
    delete iChannel;
    delete iBufGetOperation;
    delete iSize;
      
    IRLOG_DEBUG( "CIRXMLContentHandler::~CIRXMLContentHandler - Exiting." );       
    }
// ---------------------------------------------------------------------------
// CIRXMLContentHandler::ResetValues
// Reseting the Values
// ---------------------------------------------------------------------------
//

void CIRXMLContentHandler::ResetValues()
    {
    IRLOG_DEBUG( "CIRXMLContentHandler::ResetValues" );
    iFlag = 1;
    iPresets = EFalse;
    iCategories = EFalse;
    iCat = EFalse;
    iChannels = EFalse;
    iChan = EFalse;
    iAdvertisement = EFalse;
    iOTAUpdate = EFalse;
    iIRID = EFalse;
    iTimeStamp = EFalse;
    // if XML Parsing error occurred, maybe iChannel wasn't deleted 
    if ( iChannel )
        {
        delete iChannel;
        }
    iChannel = NULL;
    // if XML Parsing error occurred, maybe iCategory wasn't deleted     
    if ( iCategory )
        {
        delete iCategory;
        } 
    iCategory = NULL;
    iAdvts = EFalse;
    iAdvertisementTagFlag = EFalse;   
    IRLOG_DEBUG( "CIRXMLContentHandler::ResetValues - Exiting." );
  }    
 
// ---------------------------------------------------------------------------
// CIRXMLContentHandler:: OnStartDocumentL
// This method is a callback to indicate the start of the document
// ---------------------------------------------------------------------------
//   
void CIRXMLContentHandler:: OnStartDocumentL( const Xml::RDocumentParameters& 
  /*aDocParam*/ , TInt /*aErrorCode*/ )
    {                
    IRLOG_DEBUG( "CIRXMLContentHandler::OnStartDocumentL" );
    iEOD = KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CIRXMLContentHandler::OnEndDocumentL
// This method is a callback to indicate the end of the document
// ---------------------------------------------------------------------------
// 
void CIRXMLContentHandler::OnEndDocumentL( TInt /*aErrorCode*/ )
    {
    //No Implementation
    IRLOG_DEBUG( "CIRXMLContentHandler::OnEndDocumentL" );
    return;
    }
 
// ---------------------------------------------------------------------------
// CIRXMLContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed. 
// ---------------------------------------------------------------------------
//
void CIRXMLContentHandler::OnStartElementL( const Xml::RTagInfo &aElement, 
      const Xml::RAttributeArray &aAttributes, TInt /*aErrorCode*/ )
    {     
    IRLOG_DEBUG( "CIRXMLContentHandler::OnStartElementL" );     
    //Compare the element name to identify the type - //TO DO String Pool        
    TInt genre = ( aElement.LocalName().DesC() ).Compare( KGenre );
    TInt lang = ( aElement.LocalName().DesC() ).Compare( KLanguage );
    TInt country = ( aElement.LocalName().DesC() ).Compare( KCountry );
    TInt channel = ( aElement.LocalName().DesC() ).Compare( KChannel );
    TInt preset = ( aElement.LocalName().DesC() ).Compare( KPreset );
    TInt advertisements = ( aElement.LocalName().DesC() ).Compare( KAdvertisements );
    TInt ad = ( aElement.LocalName().DesC() ).Compare( KAd );

    TInt genres = ( aElement.LocalName().DesC() ).Compare( KCatGenre );
    TInt countries = ( aElement.LocalName().DesC() ).Compare( KCatCountry );
    TInt languages = ( aElement.LocalName().DesC() ).Compare( KCatLang );
    TInt channels = ( aElement.LocalName().DesC() ).Compare( KChannels );
    TInt presets = ( aElement.LocalName().DesC() ).Compare( KPresets );

    TInt IRID = ( aElement.LocalName().DesC() ).Compare( KIRID );
    TInt timestamp = ( aElement.LocalName().DesC() ).Compare( KTimeStamp );
    TInt irappversion = ( aElement.LocalName().DesC() ).Compare( KIRVersion );
    TInt adv = ( aElement.LocalName().DesC() ).Compare( KAdv );
    TInt banner = ( aElement.LocalName().DesC() ).Compare( KBanner );

   
    if ( iUrlContentForAds )
        {
        delete( iUrlContentForAds );
        iUrlContentForAds = NULL;
        }

    TInt bufSize( KTen );
    iUrlContentForAds = HBufC::NewL( bufSize );//this is because, the OnContentL method gives chunck by chunck

    if ( ad == 0 )
         {   
         //We want to take the first Ad only so if an Ad has already been created
         //iChannel and iCategory will not be NULL hence the new Ad should not be added
         if ( !iChannel && !iCategory )
             {
             iChannel = CIRBrowseChannelItems::NewL();  
             iCategory = CIRBrowseCatagoryItems::NewL();  
             
             }
         }
    if ( advertisements == 0 )
        { 
        iAdvertisementTagFlag = ETrue;   
        }
    if ( iAdvertisementTagFlag && banner == 0 )
        {
        iAdvts = ETrue;
        }
    // Creates the instance of the class Browse by Genre/Language/Country
    if ( iFlag )
        {
        if ( genre == 0 || lang == 0 || country == 0 )
        {
        //If there are ads then destroy the list and put the ads
        //as ads will the first in the xml
        if ( ( iAdvts )  && ( iCategory ) )
            { 
            iPtrCategory->ResetAndDestroy();
            iPtrCategory->AppendL( iCategory );
            iCategory = NULL;
            delete iChannel; 
            iChannel = NULL;
            iAdvts = EFalse;
            }
            iCategory = CIRBrowseCatagoryItems::NewL();
            iCat = ETrue;                
            }
        }
    if ( iFlag )
        {
        // In case of browse by genres, country and lanaguages
         if ( genres == 0 || countries == 0 || languages == 0 )
            { 
            if ( iAdvts == EFalse )
                {
                iPtrCategory->ResetAndDestroy();
                }
            if ( genres == 0 )
                {
                iFileToCache.Zero();
                iFileToCache.Copy( KCatGenre );
                }
            if ( countries == 0 )                        
                {
                iFileToCache.Zero();
                iFileToCache.Copy( KCatCountry );
                }
            if ( languages == 0 )                        
                {      
                iFileToCache.Zero();
                iFileToCache.Copy( KCatLang );
                }

            iCategories = ETrue;                           
            }
        }
    // Creates the instance of the class Channel
    if ( channel == 0 )
        {
        //If there are ads then destroy the list and put the ads
        //as ads will the first in the xml
        if ( ( iAdvts )   && ( iChannel ) ) 
            {
            iPtrChannel->AppendL( iChannel );
            iChannel = NULL;
            delete iCategory; 
            iCategory = NULL;
            iAdvts = EFalse;
            }
        iChannel = CIRBrowseChannelItems::NewL();    
        iChannels = ETrue;
        iBitrateCounter = 0;
        }
    if ( channels == 0 )
        { 
        iPtrChannel->ResetAndDestroy();   
        iChan = ETrue;
        iChannelFlag = 0;    
        }
    // Creates the instance of the class for Preset Data
    if ( preset == 0 )
        {    
         iPreset = CIRIsdsPreset::NewL();    
         iPresets = ETrue;           
        }
    if ( presets == 0 )
        {     
        iPtrPreset->ResetAndDestroy();  
        iFlag = 0;
        }
    if ( IRID == 0 )
        {
        iIRID = ETrue;    
        }
    if ( timestamp == 0 )
        {
        //time stamp is obtained when session log is obtained in isds 
        iTimeStamp = ETrue;
        }
    if ( irappversion == 0 )
        {
        iOTA = CIROTAUpdate::NewL();
        iOTAUpdate = ETrue;
        }
    if ( adv == 0 )
        {
        iAdv = CIRAdvertisement::NewL();
        iAdvertisement = ETrue;
        }
    TBuf<KAttributeBufferSize> BufParam1;
    TBuf<KAttributeBufferSize> BufParam2; 
    
    // Parses the attributes and the value of the element
    TInt nAttributes = aAttributes.Count();
    for ( TInt indexofattribute = 0; indexofattribute < nAttributes; 
                          ++indexofattribute )
        {
        const RAttribute& attribute = aAttributes[indexofattribute];
        const RTagInfo& nameInfo = attribute.Attribute();
         
        //!Gets the elementId from the Pool for the specified attributename
        TIRXMLElement ATTelementId = TIRXMLElementFormat::GetElementIdL(
                    ( nameInfo.LocalName().DesC() ),iStringPool ); 
         
        HBufC* converted = CnvUtfConverter::
            ConvertToUnicodeFromUtf8L( attribute.Value().DesC() );
        CleanupStack::PushL( converted );
        
        switch( ATTelementId ) 
            {
            case EGetOperation: 
                {
                delete iBufGetOperation;
                iBufGetOperation = NULL;
                iBufGetOperation = converted->Des().AllocL();
                }
                break;
                               
            case EName:
                {
                //if name is parsed
                if ( iCat )
                    {                                                                    
                    //if category
                    iCategory->SetGetOperation( *iBufGetOperation );                                          
                    BufParam1.Copy( *converted );                
                    }
                if ( iChannels )
                    {
                     //if channel
                    iChannel->SetGetOperationL( *iBufGetOperation );
                    iChannel->SetSize( *iSize );
                    BufParam1.Copy( *converted );
                    } 
                if ( iPresets )
                    {
                    if ( ( aElement.LocalName().DesC()) == KPreset )
                      {
                      //for preset
                      iPreset->SetNameL( *converted );
                      }                                
                    BufParam1.Copy( *converted );
                    }                        
                }
                break;    
                                     
            case EID:
                {
                //getting id
                if ( iCat )
                    {
                    //for category
                    iCategory->SetTextL( BufParam1, *converted );    
                    }
                   
                if ( iChannels )
                    {
                    //for channel
                    BufParam2.Copy( *converted );          
                    }     
                if ( iPresets )
                    {
                      //for preset
                    iPreset->SetGenreInfoL( *converted, BufParam1 );
                    }  
                if( iIRID)
                    {
                      //ir id
                    iRID.Copy( *converted );
                    }
                }
                break;
                             
            case ESize: 
                {
                delete iSize;
                iSize = NULL;
                iSize = converted->Des().AllocL();
                if( iCat )
                    {
                    iCategory->SetSize( *iSize );
                    }                       
                }
                break;
                         
            case EShortDesC:
                {
                //getting short description
                if( iChannels )              
                    {
                    iChannel->SetTextL( BufParam1,BufParam2,*converted );
                    }
                if( iPresets )
                     {
                     iPreset->SetShortDescL( *converted );
                     }             
                }
                break;
                          
            case EBitrateValue:
                {
                //parsing bit rate
                if( iChannels )
                    {
                        if( iBitrateCounter == KBitRateCounterValue0 )
                            {
                             iChannel->SetBitrate( *converted );
                             BufParam1.Copy( *converted );
                            }
                                            
                        if( iBitrateCounter == KBitRateCounterValue1 )
                            {
                             iChannel->SetBitrate( BufParam1,*converted );     
                             BufParam2.Copy( *converted );
                            }
                                           
                        if( iBitrateCounter == KBitRateCounterValue2 )
                            {
                            iChannel->SetBitrate( BufParam1,BufParam2,*converted );
                            }
                                           
                        iBitrateCounter++;
                    }
                }                       
                break;
                         
            case ESdsID:
                {                   
                if( iPresets )
                    {
                    iPreset->SetId( *converted );
                    }                                         
                }
                break;
                           
            case ELangCode:
                {
                //language code
                if( iPresets )
                    {
                    iPreset->SetLangCodeL( *converted );
                    }                
                }
                break;
                           
            case ELang:
                {                       
                //language
                if( iPresets )
                    {
                    iPreset->SetLangL( *converted );    
                    }
                }
                break;
                           
            case ECountryCode:
                {
                //country code
                if( iPresets )
                    {
                    iPreset->SetCountryCodeL( *converted );    
                    }
                }
                break;
                    
            case ECountry:
                {
                //country
                if( iPresets )
                    {
                    iPreset->SetCountryNameL( *converted );    
                    }
                }
                break;
                           
            case ELastUpdated:
                {
                //last modified time
                if( iPresets )
                    {
                    iPreset->SetLastModifiedTimeL( *converted );    
                    }
                }
                break;
                           
            case EDesc:
                {
                //description
                if( iPresets )
                    {
                    iPreset->SetDescriptionL( *converted );
                    }
                }
                break;
                           
            case EImgURL:
                {
                //image url
                if( iChannels )                  
                    {
                    iChannel->SetImageUrlL( *converted );
                    }
                if( iPresets )
                    {
                    iPreset->SetImgUrlL( *converted );    
                    }
                }
                break;
                           
            case EAdvImgURL:
                {
                //advertisement url
                if( iPresets )
                    {
                    iPreset->SetAdvertisementUrlL( *converted );    
                    }
                }
                break;
                            
            case EAdvInUse:
                {
                 //advertisement in use
                 if( iPresets )
                     {
                     iPreset->SetAdvertisementInUseL( *converted );    
                     }
                 }
                 break;
                           
            case EGenreID:
                {
                //genre id
                if( iPresets )
                    {
                    iPreset->SetGenreInfoL( BufParam1,*converted );    
                    }
                }
                break;
                          
            case EURL:
                {
                //url
                if( iPresets )
                   {
                    BufParam2.Copy( *converted );    
                    }
                if( iAdvertisement )
                    {
                    iAdv->SetURL( *converted );
                    } 
                }
                break;
                        
            case EMusicStoreEnabled:
                {                   
                if( iPresets )
                    {
                    iPreset->SetMusicStoreStatusL( *converted );
                    }                                         
                }
                break;     
                        
            case EBitrate:
                {
                //set url
                if( iPresets )
                    {
                    iPreset->SetUrlL( BufParam1,BufParam2,*converted );
                    }
                }
                break;
                           
            case EIRID:
                {
                //irid      
                }
                break;
                           
            case EVersion:
                {
                //for ota update
                if( iOTAUpdate )
                   {
                    iOTA->SetVersion( *converted );
                    } 
                }
                break;
                           
            case EUpdateInterval:
                {
                //update interval
                if( iOTAUpdate )
                    {
                    iOTA->SetUpdateInterval( *converted );    
                    }
                }
                break;
                           
            case EUpdateMethod:
                {
                //update method
                if( iOTAUpdate )
                    {
                    iOTA->SetUpdateMethod( *converted );    
                    }
                }
                break;
                           
            case EInUse:
                 {
                 //advertisement in use
                 if( iAdvertisement )
                     {
                     iAdv->SetAdvInUse( *converted );
                     }
                 }
                 break;
                           
            case EUsageVisibleTimes:
                {
                if( iAdvertisement )
                    {
                    iAdv->SetUsage( *converted );
                    }
                }
                break;
                           
            case EVisibleTime:
                {
                //advertisement visible time
                if( iAdvertisement )
                    {
                    iAdv->SetVisibleTime( *converted );
                    }
                }
                break;
                           
            case EElementIDUnknown:
                {
                //unknown
                }
                break;
                          
            default:
                {
                break;
                } 
            }//End of Switch
        CleanupStack::PopAndDestroy( converted );
        } //End of for                    
    IRLOG_DEBUG( "CIRXMLContentHandler::OnStartElementL - Exiting." );                         
    }
 
// ---------------------------------------------------------------------------
// CIRXMLContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached. 
// ---------------------------------------------------------------------------
//
void CIRXMLContentHandler::OnEndElementL( const RTagInfo &aElement, TInt /*aErrorCode*/ )
    {    
    IRLOG_DEBUG( "CIRXMLContentHandler::OnEndElementL" );        
    if( ( aElement.LocalName( ).DesC( ) ) == KPreset )
        {
        if ( iPreset->GetUrlCount() > 0 )
            {
            // The ownership is transferred to the array.
            iPtrPreset->AppendL( iPreset ); 
            }
        else
            {
            // If there is not a single URL, we don't let it in.
            iEOD = KErrCorrupt;
            delete iPreset;
            }
        // Ownership transferred. Remove the pointer.
        iPreset = NULL; 
        }
    if( ( aElement.LocalName( ).DesC( ) ) == KBanner )
        {
        //get the content for the tag "banner"
        //assign it to "iBanner" of CIRBrowseChannelItems
        if( iUrlContentForAds )
            {
            iChannel->SetBannerUrl( *iUrlContentForAds );         
            iCategory->SetBannerUrl( *iUrlContentForAds );          
            }
        }
    if( ( aElement.LocalName(  ).DesC(  ) ) == KClickthroughUrl )
        {
        //get the content for the tag "clickthrough-url"
        //assign it to "iClickThroughUrl" of CIRBrowseChannelItems 
        if( iUrlContentForAds )
            {
            iChannel->SetClickThroughUrl( *iUrlContentForAds );         
            iCategory->SetClickThroughUrl( *iUrlContentForAds );
            }
        }
      
    if( iUrlContentForAds )
        {
        delete iUrlContentForAds;
        iUrlContentForAds = NULL;
         
        }
    if( iFlag )
        {
        if( ( aElement.LocalName( ).DesC( ) ) == KGenre ||
          ( aElement.LocalName( ).DesC( ) ) == KLanguage || 
          ( aElement.LocalName( ).DesC( ) ) == KCountry )
            {         
            iPtrCategory->AppendL( iCategory );
            iCategory = NULL;
            iCat = EFalse;
            }
        }
    
    if( ( aElement.LocalName( ).DesC( ) ) == KChannel )
        {
        iPtrChannel->AppendL( iChannel );
        iChannel = NULL;
        iChannels = EFalse;         
        } 
    //appends the created channel(ad) to the array
    /*
    if((aElement.LocalName().DesC()) == KAd)
        {
    iPtrChannel->AppendL(iChannel);
        iChannel = NULL;

        } */
   
    if( ( aElement.LocalName( ).DesC( ) ) == KServers )
    {
        if ( iPreset )
        {
            iPreset->SetUrlCount( );
        }
        }               
    IRLOG_DEBUG( "CIRXMLContentHandler::OnEndElementL - Exiting." );                   
    }
 
// ---------------------------------------------------------------------------
// CIRXMLContentHandler ::OnContentL
// This method is a callback that sends the content of the element.The data 
// may be sent in chunks
// ---------------------------------------------------------------------------
//
void CIRXMLContentHandler ::OnContentL( const TDesC8 &aBytes, TInt 
    aErrorCode )
    { 
    IRLOG_DEBUG(  "CIRXMLContentHandler::OnContentL" );
    
    if ( KErrNone == aErrorCode )
        {
        
        
        HBufC* buffertmp = HBufC::NewLC( aBytes.Length( ) + 1 );
        TPtr buffertmpPtr( buffertmp->Des( ) );
        buffertmpPtr.Copy( aBytes );
    
    
        TInt tempSize;
        if( iUrlContentForAds )
            {
              tempSize = iUrlContentForAds->Length( ) + aBytes.Length( );
              iUrlContentForAds = iUrlContentForAds->ReAllocL( tempSize );
              TPtr bufferPtr( iUrlContentForAds->Des( ) );
              bufferPtr.Append( buffertmpPtr );
            }
        
        CleanupStack::PopAndDestroy( buffertmp ); // buffertmp
        } 
        
        if ( iOTAUpdate )
            {
            HBufC* converted = CnvUtfConverter::ConvertToUnicodeFromUtf8L( aBytes );
            CleanupStack::PushL( converted );
            converted->Des( ).TrimAll( );
            if ( converted->Des( ).Length( ) )
                {                    
                iOTA->SetOTAUrl( *converted );
                }
            CleanupStack::PopAndDestroy( converted );
            }
    
    IRLOG_DEBUG( "CIRXMLContentHandler::OnContentL - Exiting." );        
    }


// ---------------------------------------------------------------------------
// CIRXMLContentHandler::OnStartPrefixMappingL
// This method is a notification of the beginning of the scope of a 
// prefix-URI Namespace mapping.
// ---------------------------------------------------------------------------
// 
void CIRXMLContentHandler::OnStartPrefixMappingL( const RString& /*aPrefix*/, 
    const RString& /*aUri*/, TInt /*aErrorCode*/ )
    {
    //No Implementaion    
    IRLOG_DEBUG( "CIRXMLContentHandler::OnStartPrefixMappingL" );      
    }  
      
// ---------------------------------------------------------------------------
// CIRXMLContentHandler :: OnEndPrefixMappingL
// This method is a notification of the end of the scope of a prefix-URI mapping
// ---------------------------------------------------------------------------
//  
void CIRXMLContentHandler :: OnEndPrefixMappingL( const RString& /*aPrefix*/, 
    TInt /*aErrorCode*/)
    {
    //No Implementaion    
    IRLOG_DEBUG( "CIRXMLContentHandler::OnEndPrefixMappingL" );    
    }
     
// ---------------------------------------------------------------------------
// CIRXMLContentHandler :: OnIgnorableWhiteSpaceL
// This method is a notification of ignorable whitespace in element content. 
// ---------------------------------------------------------------------------
//
void CIRXMLContentHandler :: OnIgnorableWhiteSpaceL( const TDesC8&  /*aBytes*/, 
    TInt /*aErrorCode*/ )
    {
    //No Implementaion        
    IRLOG_DEBUG( "CIRXMLContentHandler::OnIgnorableWhiteSpaceL" );        
    }
     
// ---------------------------------------------------------------------------
// CIRXMLContentHandler :: OnSkippedEntityL
// This method is a notification of a skipped entity
// ---------------------------------------------------------------------------
//
void CIRXMLContentHandler :: OnSkippedEntityL( const RString& /*aName*/, 
    TInt /*aErrorCode*/ )
    {
    //No Implementaion    
    IRLOG_DEBUG(  "CIRXMLContentHandler::OnSkippedEntityL" );
    }

// ---------------------------------------------------------------------------
// CIRXMLContentHandler :: OnProcessingInstructionL
// This method is a receive notification of a processing instruction.
// ---------------------------------------------------------------------------
//
void CIRXMLContentHandler :: OnProcessingInstructionL( const TDesC8& 
  /*aTarget*/, const TDesC8& /*aData*/, TInt /*aErrorCode*/ )
     {
      //No Implementaion    
     IRLOG_DEBUG( "CIRXMLContentHandler::OnProcessingInstructionL" );
     }
 
// ---------------------------------------------------------------------------
// CIRXMLContentHandler :: OnError
// This method indicates an error has occurred.
// ---------------------------------------------------------------------------
//
void CIRXMLContentHandler :: OnError( TInt aErrorCode )
     {
     iEOD = aErrorCode;         
     IRLOG_ERROR2( "CIRXMLContentHandler::OnError (%d)", aErrorCode );
     }
  
  
// ---------------------------------------------------------------------------
// CIRXMLContentHandler :: GetExtendedInterface
// This method obtains the interface matching the specified uid. 
// ---------------------------------------------------------------------------
//
TAny* CIRXMLContentHandler :: GetExtendedInterface( const TInt32 /*aUid*/ )
     {
      //No Implementation
      IRLOG_DEBUG( "CIRXMLContentHandler::GetExtendedInterface" );
      return NULL;
     }
 


GLDEF_C TInt E32Dll( ) 
// DLL entry point
    {
    return( KErrNone );
    }


