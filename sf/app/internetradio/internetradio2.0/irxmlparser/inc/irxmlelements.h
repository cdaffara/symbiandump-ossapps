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


#ifndef IRXMLELEMENTS_H
#define IRXMLELEMENTS_H

#include <e32cmn.h>
#include <stringtablesupport.h>

//Literals to instantiate the class
 _STLIT8( KGetOp, "getOperation" );
 _LIT8( KCatGenre, "genres" );
 _STLIT8( KCatLang, "languages" );
 _STLIT8( KCatCountry, "countries" );
 _STLIT8( KChannels, "channels" );
 _STLIT8( KPresets, "presets" );
 _STLIT8( KIRID, "irid" );
 _STLIT8( KTimeStamp, "timestamp" ); 
 _STLIT8( KIRVersion, "iRAPPVersion" ); 
 _STLIT8( KAdv,"advertisement" );
 _STLIT8( KPreset, "preset" );
 _STLIT8( KServers, "servers" ); 
 _STLIT8( KGenre, "genre" );
 _STLIT8( KLanguage, "language" );
 _STLIT8( KCountry, "country" );
 _STLIT8( KChannel, "channel" );
 _STLIT8( KAd, "ad" );
 _STLIT8( KAdvertisements, "advertisements" );
 _STLIT8( KBanner, "banner" );
 _STLIT8( KClickthroughUrl, "clickthrough-url" );


//! Enum for the attributes
enum TIRXMLElement
    {
     EGetOperation,
     
    // List of Genres/Language/Contry
    EName,
    EID,
    ESize,
    
    //! Channels 
    EShortDesC,
    EBitrateValue,
    
    // Preset
    ESdsID,
    ELangCode,
    ELang,
    ECountryCode,
    ECountry,
    ELastUpdated,
    EDesc,
    EImgURL,
    EAdvImgURL,
    EAdvInUse,
    EGenreID,
    EURL,
    EMusicStoreEnabled,
    EBitrate,
    
    // Log Usage
    EIRID,
    
    // OTA Update
    EVersion,
    EUpdateInterval,
    EUpdateMethod,
    
    //Advertisement
    EInUse,
    EUsageVisibleTimes,
    EVisibleTime,
    /**
    Unknown Element
    */    
    EElementIDUnknown,
     }; 
 
class RStringPool;

/**
* This class handles the XML string pool
* not accessible outside the dll
*/
class TIRXMLElementFormat
    {
public : 
    /**
    * Creates an initialised string pool with no pre-loaded string tables.
    * @param aStringPool Stringpool
    */
    static void OpenStringPoolL( RStringPool& aStringPool );
    /**
    * The ElementId of the element from the StringPool
    * @param aElement the element name to be searched in the pool
    * @param aStringPool stringpool reference
    * @return the enum for the element
    */
    static TIRXMLElement GetElementIdL( const TDesC8& aElement, 
                                const RStringPool& aStringPool );
    /**
    * Closes the StringPool
    * @param aStringPool Stringpool
    */                                     
    static void CloseStringPoolL( RStringPool& aStringPool );                                    
    }; 
 
 
#endif //IRXMLELEMENTS_H
