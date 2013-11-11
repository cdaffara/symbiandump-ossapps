/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Maptile interface source implementation for maptile
*              service.
*
*/

#include <EPos_CPosLandmark.h>
#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLmItemIterator.h>
#include <EPos_CPosLmOperation.h>
#include <f32file.h>
#include <bautils.h>
#include "maptileinterface.h"
#include "mylocationlogger.h"
#include "mylocationsdefines.h"
#include <math.h>

//Invalid latitude & longitude value
const TReal KInvalidLatitudeLongitude =  200.0f;
const TInt KStreetLvelZoom = 16;
const TInt KCityLevelZoom = 11;
const TInt KImagePathSize=36;
const TInt KMapTileWidthHeight = 256 ;
_LIT(KFileExtn, ".png");

// -----------------------------------------------------------------------------
// CMapTileInterface::NewL()
// Creates and returns the instance of CMapTileInterface
// -----------------------------------------------------------------------------
//
CMapTileInterface* CMapTileInterface::NewL()
{
    CMapTileInterface* self = new (ELeave) CMapTileInterface();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CMapTileInterface::~CMapTileInterface()
// Destructor
// -----------------------------------------------------------------------------
//
CMapTileInterface::~CMapTileInterface()
{
    __TRACE_CALLSTACK;
   
    delete iFilePath;
    
    delete iMaptileGeocoder;
    
    delete iLandmark;
}    


// -----------------------------------------------------------------------------
// CMapTileInterface::CMapTileInterface()
// Constructor
// -----------------------------------------------------------------------------
//
CMapTileInterface::CMapTileInterface() :
           iMaptileGeocoder( NULL ), 
           iFilePath( NULL ),
           iObserver(NULL),
           iLandmark(NULL)
{
}

// -----------------------------------------------------------------------------
// CMapTileInterface::ConstructL()
// Two phase constructor
// -----------------------------------------------------------------------------
//
void CMapTileInterface::ConstructL()
{
    __TRACE_CALLSTACK;

    iMaptileGeocoder = CMaptileGeocoderPlugin::NewL();
    
    iMaptileGeocoder->SetMaptileGeocoderObserver( *this );
}
// -----------------------------------------------------------------------------
// CMapTileInterface::GetGeoCodeFromAddressL()
// Interface for requesting GEO fields for the given address
// -----------------------------------------------------------------------------
//
void CMapTileInterface::GetGeoCodeFromAddressL(const TDesC& aAddressDetails,
        const TDesC& aFilePath, MMapTileObserver* aObserver)
{
    __TRACE_CALLSTACK;
    iObserver = aObserver;
    delete iFilePath;
    iFilePath = NULL;    
    iFilePath = HBufC::NewL(aFilePath.Length());
    iFilePath->Des().Copy(aFilePath);
    iStreetAvailable = EFalse;
    iMaptileGeocoder->GetCoordinateByAddressL(aAddressDetails, ESilent);
     
}
// -----------------------------------------------------------------------------
// CMapTileInterface::GetGeoCodeFromAddressL()
// Interface for requesting GEO fields for the given landmark adress
// -----------------------------------------------------------------------------
//
void CMapTileInterface::GetGeoCodeFromAddressL(CPosLandmark* aLandmark,
        const TDesC& aFilePath, MMapTileObserver* aObserver)
{
    __TRACE_CALLSTACK;
    iObserver = aObserver;
    delete iFilePath;
    iFilePath = NULL;    
    iFilePath = HBufC::NewL(aFilePath.Length());
    iFilePath->Des().Copy(aFilePath);

    //Reset the street level available flag
	iStreetAvailable = EFalse;
	
    TPtrC getStreet;
    aLandmark->GetPositionField(EPositionFieldStreet, getStreet);

    if (getStreet.Length() > 0)
   {
        iStreetAvailable = ETrue;
    }
    iMaptileGeocoder->GetCoordinateByAddressL(*aLandmark, ESilent);
}

// -----------------------------------------------------------------------------
// CMapTileInterface::GetMapTileL()
// Function for Retrieving latitude & longitude information
// -----------------------------------------------------------------------------
//
void CMapTileInterface::GetMapTileL( const TReal& aLatitude, const TReal& aLongitude)
{
    __TRACE_CALLSTACK;//Notification to observer , to update contact db,
    TInt zoom = KCityLevelZoom;
    if (iStreetAvailable)
    {
        zoom = KStreetLvelZoom;
    }
    iStreetAvailable = EFalse;
    TMapTileParam mapTileparam(aLatitude, aLongitude, zoom, MapTileWidth,MapTileHeight );
    iMaptileGeocoder->GetMapTileByGeoCodeL( mapTileparam, *iFilePath );
}

// -----------------------------------------------------------------------------
// CMapTileInterface::MapComplete()
// Maptile retrievel notification
// -----------------------------------------------------------------------------
//
void CMapTileInterface::MapTileFetchingCompleted( TInt aErrCode, const TDesC& aMapTilePath )
{
    __TRACE_CALLSTACK;
    MYLOCLOGSTRING1("MapComplete() status-%d ",aErrCode );
    iObserver->MapTilefetchingCompleted( aErrCode, aMapTilePath );
}

// ----------------------------------------------------------------------------
// CMapTileInterface::UpdateFilePathL()
// Converts the geocoordinate to maptile pixel coordinate and updates the
// file path to Maptilefolder\RowCol.png
// ----------------------------------------------------------------------------
//
void CMapTileInterface::UpdateFilePathL( const TReal& aLatitude, const TReal& aLongitude )
{
    TInt iZoomLvl = KCityLevelZoom;
    if ( iStreetAvailable )
        iZoomLvl = KStreetLvelZoom;
    
    TInt totalTilesHorizontal = 1 << iZoomLvl;
    TInt totalTilesVertical = 1 << iZoomLvl;

    TInt totalMapWidth = totalTilesHorizontal * KMapTileWidthHeight;
    TInt totalMapHeight = totalTilesVertical * KMapTileWidthHeight;
    
    TReal pi = 3.14159265;

    TInt convertedx = (( aLongitude + 180.0) * totalMapWidth)
            / 360.0;
    TReal convertedtemp = log(tan(( aLatitude + 90) * pi
            / 360.0));
    int convertedy = (1 - convertedtemp / pi) * totalMapHeight / 2.0;

    //Get the image row,col
    TInt iMapTileImageRow = ( convertedy / 256.0 ) * 1000;
    TInt iMapTileImageCol = ( convertedx / 256.0 ) * 1000;
    
    TBuf<KImagePathSize> mImagePath;

    mImagePath.AppendNum(iMapTileImageRow);
    mImagePath.AppendNum(iMapTileImageCol);
    mImagePath.Append(KFileExtn);   
    
    if( iFilePath )
    {
        iFilePath = iFilePath->ReAllocL(iFilePath->Length() + mImagePath.Length() );
        iFilePath->Des().Append(mImagePath);    
    }
}

// -----------------------------------------------------------------------------
// CMapTileInterface::GeocodeComplete()
// Geocode completion  notification
// -----------------------------------------------------------------------------
//
void CMapTileInterface::GeocodingCompleted(TInt aErrorcode,
        MAddressInfo& aAddressInfo)
{
    __TRACE_CALLSTACK;
    TReal latitude, longitude;
    latitude=aAddressInfo.GetLatitude();
    longitude=aAddressInfo.GetLongitude();
    
    MYLOCLOGSTRING1("GeocodeCompleted() status-%d",aErrorcode);
    MYLOCLOGSTRING1("GeocodeCompleted() latitude-%f",latitude );
    MYLOCLOGSTRING1("GeocodeCompleted() longitude-%f",longitude );
	
    if (aErrorcode == KErrNone)
    {
        if ( latitude != KInvalidLatitudeLongitude
                && longitude != KInvalidLatitudeLongitude)
        {           
            TRAP_IGNORE( SetLandMarkDetailsL(aAddressInfo) );
			      TRAP_IGNORE( UpdateFilePathL( latitude, longitude ) );
	      
            iObserver->GeoCodefetchingCompleted(KErrNone,  latitude, longitude, iFilePath->Des());

        }
        else
        {
            iObserver->GeoCodefetchingCompleted(KErrGeneral,
		      KInvalidLatitudeLongitude, KInvalidLatitudeLongitude, KNullDesC);
        }        
    }
    else
    {
        iObserver->GeoCodefetchingCompleted(aErrorcode,
		       KInvalidLatitudeLongitude, KInvalidLatitudeLongitude, KNullDesC );
    }
}

// -----------------------------------------------------------------------------
// CMapTileInterface::GetLandMarkDetails()
// return pointer to CPosLandmark 
// -----------------------------------------------------------------------------
//
CPosLandmark* CMapTileInterface::GetLandMarkDetails()
{
    return iLandmark;  
               
}
// -----------------------------------------------------------------------------
// CMapTileInterface::SetLandMarkDetails()
// create CPosLandmark details from MAddressInfo 
// -----------------------------------------------------------------------------
//
void CMapTileInterface::SetLandMarkDetailsL(MAddressInfo& aAddressInfo)
{
    if(iLandmark)
    {
        delete iLandmark;
        iLandmark=NULL;
    }
    TReal latitude,longitude;
    latitude=aAddressInfo.GetLatitude();
    longitude=aAddressInfo.GetLongitude();    
    TLocality position(TCoordinate(latitude,longitude),0);
    iLandmark=CPosLandmark::NewL();
    //latitude and longitude
    iLandmark->SetPositionL(position);
    
    //street
    TPtrC tempText=aAddressInfo.GetThoroughfareName();
    if(tempText.Length()>0)
    {
        iStreetAvailable=ETrue;
    }
    iLandmark->SetPositionFieldL(EPositionFieldStreet, tempText);
    //postal code
    tempText.Set(aAddressInfo.GetPincode());
    iLandmark->SetPositionFieldL(EPositionFieldPostalCode, tempText);
    //city
    tempText.Set(aAddressInfo.GetCity());
    iLandmark->SetPositionFieldL(EPositionFieldCity, tempText);
    //state
    tempText.Set(aAddressInfo.GetState());
    iLandmark->SetPositionFieldL(EPositionFieldState, tempText);
    //country
    tempText.Set(aAddressInfo.GetCountryName());
    iLandmark->SetPositionFieldL(EPositionFieldCountry, tempText); 
    
}
//end of line
