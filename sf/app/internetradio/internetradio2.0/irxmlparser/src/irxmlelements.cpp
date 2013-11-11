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


#include <stringpool.h>

#include "irdebug.h"
#include "irxmlelements.h"

//Ignores PC Lint Warning 618.

//! Literals for the StringPool

//! Literals for List of genres,languages,countries
 _STLIT8( KName, "name" );
 _STLIT8( KID, "id" );
 _STLIT8( KSize, "size" );
 
//!Literals for the List of Channels
 _STLIT8( KShortDesC, "shortDesc" );

 _STLIT8( KBitrate, "bitrate" );
 _STLIT8( KValue, "value" );
//! Literals for the channel 

_STLIT8( KSdsid, "sdsid" );
_STLIT8( KLangCode, "langCode" );
_STLIT8( KLang, "lang" );
_STLIT8( KCountryCode, "countryCode" );
_STLIT8( KLastUpdated, "lastUpdated" );
_STLIT8( KDescription, "description" );
_STLIT8( KImgURL, "imgUrl" );
_STLIT8( KAdvUrl, "advertiseImgUrl" );
_STLIT8( KAdvUse, "advertiseInUse" );
_STLIT8( KGenreId, "genreid" );
_STLIT8( KURL, "url" );
_STLIT8( KMusicStoreEnabled, "musicStoreEnabled" );
//!Literals for the log Usage

_STLIT8( KIRId, "irid" ); 
//! Literals for the OTA Update

_STLIT8( KVersion, "version" );
_STLIT8( KUpdateInterval, "updateInterval" );
_STLIT8( KUpdateMethod, "updateMethod" );

 //! Literals for Advertisement
_STLIT8( KInUse, "inUse" );
_STLIT8( KUsageVisibleTimes, "usageVisibleTimes" );
_STLIT8( KVisibleTime, "visibleTime" );
 
const TInt KNoOfElementInTable = 27;
 
 //! PoolTable
 const TAny* const IRXMLPoolContent[] =
    { 
  (const TAny*)         &KGetOp,    
  (const TAny*)         &KName,       
  (const TAny*)         &KID,    
  (const TAny*)         &KSize,
  (const TAny*)         &KShortDesC,
  (const TAny*)         &KValue,        
  (const TAny*)         &KSdsid,
  (const TAny*)         &KLangCode,
  (const TAny*)         &KLang,
  (const TAny*)         &KCountryCode,
  (const TAny*)         &KCountry,
  (const TAny*)         &KLastUpdated,
  (const TAny*)         &KDescription,
  (const TAny*)         &KImgURL,
  (const TAny*)         &KAdvUrl,    
  (const TAny*)         &KAdvUse,
  (const TAny*)         &KGenreId,
  (const TAny*)         &KURL,    
  (const TAny*)         &KMusicStoreEnabled,    
  (const TAny*)         &KBitrate,
  (const TAny*)         &KIRId,
  (const TAny*)         &KVersion,
  (const TAny*)         &KUpdateInterval,
  (const TAny*)         &KUpdateMethod,
  (const TAny*)          &KInUse,
  (const TAny*)         &KUsageVisibleTimes,
  (const TAny*)          &KVisibleTime
    };
    
//! Pointer to a StringTable
const static TStringTable IRPoolTable = 
    {
    KNoOfElementInTable,      
    &IRXMLPoolContent[0],
    EFalse
    };     


// ---------------------------------------------------------------------------
// Creates an initialised string pool with no pre-loaded string tables.
// ---------------------------------------------------------------------------
//
void TIRXMLElementFormat::OpenStringPoolL( RStringPool&  aStringPool )
    {
    IRLOG_DEBUG( "CIRXMLContentHandler::OpenStringPoolL" );
    aStringPool.OpenL( IRPoolTable );
    IRLOG_DEBUG( "CIRXMLContentHandler::OpenStringPoolL - Exiting." );
    }


// ---------------------------------------------------------------------------
// TIRXMLElementFormat :: GetElementIdL
// The ElementId of the element from the StringPool
// ---------------------------------------------------------------------------
//
TIRXMLElement TIRXMLElementFormat::GetElementIdL( const TDesC8& aElement, 
                                  const RStringPool& aStringPool )
    {
    IRLOG_DEBUG( "TIRXMLElementFormat::GetElementIdL" );
    TBool validElement = EFalse;
    TInt  tableEntries;
    TInt  pos(0);
    tableEntries = IRPoolTable.iCount;
    while( pos < tableEntries && validElement == EFalse )
        {
        RStringF tableEntry;
        tableEntry = aStringPool.StringF( pos, IRPoolTable );
        HBufC8* ElementName = HBufC8::NewLC( tableEntry.DesC().Length() );
        ElementName->Des().Copy( tableEntry.DesC() );
        TLex8 string( *ElementName );
        TPtrC8 token = string.NextToken();
        TInt CompareResult = aElement.Compare( token );
        if ( CompareResult == 0 )
            {
             validElement = ETrue;
            }
        if ( validElement == EFalse )
            {
             pos++;
            }
        tableEntry.Close();
        CleanupStack::PopAndDestroy( ElementName );        
        }
    if ( validElement )
        {
        IRLOG_DEBUG( "CIRXMLContentHandler::GetElementIdL - Exiting (1)." );
        return static_cast<TIRXMLElement>( pos );
        }        
    IRLOG_DEBUG( "CIRXMLContentHandler::GetElementIdL - Exiting (2)." );
    return(EElementIDUnknown);
    } 
    
// ---------------------------------------------------------------------------
// TIRXMLElementFormat::CloseStringPoolL
// Closes the StringPool
// ---------------------------------------------------------------------------
//
void TIRXMLElementFormat::CloseStringPoolL( RStringPool&  aStringPool )
    {
    IRLOG_DEBUG( "TIRXMLElementFormat::CloseStringPoolL" );
    aStringPool.Close();
    IRLOG_DEBUG( "TIRXMLElementFormat::CloseStringPoolL - Exiting." );
    return;
    }    
   

