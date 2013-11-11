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
* Description:	The header file for MaptileGeocoderPlugin that provides the Geocoding
*               and Maptile  interfaces.
*
*/


#ifndef __MAPTILEGEOCODERPLUGIN_H__
#define __MAPTILEGEOCODERPLUGIN_H__

// INCLUDES
#include <e32base.h>
#include <lbsposition.h> 
#include <EPos_CPosLandmark.h>

//Maptile geocoder plugin uid
const TUid KMaptileGeoCoderPluginUid = { 0x2002E6BE };


/**
 * Data class to get the address details. An handle to this type will be given to the
 * user through MMaptileGeocoderObserver::GeocodingCompleted callback, through which
 * user can retrieve the address details using the following interfaces.
 *
 */
class MAddressInfo
{
public:
	
    /**
     * Gets the latitude information.
     * 
     * @return aLatitude latitude value. 
     */
    virtual const TReal& GetLatitude( ) = 0;
    
    
    /**
	 * Gets the longitude information.
	 * 
	 * @return aLongitude longitude value. 
	 */
    virtual const TReal& GetLongitude( ) = 0;
    
    /*
     * Gets the reference to the country name. Scope of this returned reference is limited to
     * this perticular call. User has to store it for their further processing.
     * 
     *  @return reference to the String which holds the Country Name. 
     */
    virtual const TDesC& GetCountryName()= 0;
    
    /*
     * Gets the reference to the State. Scope of this returned reference is limited to
     * this perticular call. User has to store it for their further processing.
     * 
     *  @return reference to the String which holds the State name. 
     */
    virtual const TDesC& GetState()= 0;
    
    /*
     * Gets the reference to the City. Scope of this returned reference is limited to
     * this perticular call. User has to store it for their further processing.
     * 
     *  @return reference to the String which holds the City Name. 
     */
    virtual const TDesC& GetCity()= 0;
    
    /*
     * Gets the reference to the District name. Scope of this returned reference is limited to
     * this perticular call. User has to store it for their further processing.
     * 
     *  @return reference to the String which holds the District Name. 
     */
    virtual const TDesC& GetDistrict()= 0;
    
    /*
     * Gets the reference to the postal code. Scope of this returned reference is limited to
     * this perticular call. User has to store it for their further processing.
     * 
     *  @return reference to the String which holds the postal code. 
     */
    virtual  const TDesC& GetPincode()= 0;
    
    /*
     * Gets the reference to the thoroughfare name. Scope of this returned reference is limited to
     * this perticular call. User has to store it for their further processing.
     * 
     *  @return reference to the String which holds the thoroughfare name. 
     */
    virtual const TDesC& GetThoroughfareName()= 0;
    
    /*
     * Gets the reference to the thoroughfare number. Scope of this returned reference is limited to
     * this perticular call. User has to store it for their further processing.
     * 
     *  @return reference to the String which holds the thoroughfare number. 
     */
    virtual const TDesC& GetThoroughfareNumber() = 0;
         
};



/**
 * TMapTileParam
 * Data class containing the information required for  fetching the map tile. 
 *
 */
class TMapTileParam
{

public:

    /**
     * Parameterized constructor.
     *
     * @param aLatitude latitude value
     * @param aLongitude longitude value
     * @param aZoom  maptile zoom level
     * @param aSize requested maptile size
     *
     */
    TMapTileParam( const TReal aLatitude, const TReal aLongitude, 
            const TInt aZoom, const TInt aWidth, const TInt aHeight ): iLattitude( aLatitude ),
            iLongitude( aLongitude ), iZoomLevel( aZoom ), iWidth( aWidth ), iHeight( aHeight )
    {
    }
		
    
    /**
     * Default constructor
     */
    TMapTileParam()
    {
    }
	

public:

    // Latitude
    TReal iLattitude;
    //Longitude
    TReal iLongitude;
    //Zoom level
    TInt iZoomLevel;
    //Required image width
    TInt iWidth;
    //Required image height
    TInt iHeight;
};


/**
 * TConnectionOption
 * Data class containing the connection options.
 *
 */
enum TConnectionOption
{
    //Interactive connection
    EInteractive = 0,
    //Silent connection
    ESilent
};


/**
 * Observer class which exposes callbacks to notify the completion of geocoding 
 * and maptile fetching events.
 *
 */
class MMaptileGeocoderObserver
{
public:

    /**
     * Callback function which notifys the completion of geocoding service. 
     * This signals the completion of the asynchronous function 
     * CMaptileGeocoderPlugin::GetCoordinateByAddressL.
     * 
     *  @param aErrorcode Error status KErrNone in case of success or other system 
     *                    specific errorcodes in case of failures.
     *                       
     *  @param aAddressInfo refrence to the address stucture, through which user can 
     *                      access the coordinate information. 
     */
    virtual void GeocodingCompleted( TInt aErrorcode, MAddressInfo& aAddressInfo ) = 0;


	
    /**
     * Callback function which notifys the completion of Maptile fetching event. 
     * This signals the completion of the asynchronous function 
     * CMaptileGeocoderPlugin::GetMapTileByGeoCode.
     * 
     * @param aErrorcode Error status KErrNone in case of success or other system 
     *                   specific errorcodes in case of failures.
     *						
     * @param aMapImagePath Maptile image path
     *					   
     */
    virtual void MapTileFetchingCompleted( TInt aErrorcode, const TDesC& aMapImagePath ) = 0;
	
};





/**
 * CMaptileGeocoderPlugin
 *
 * An interface of the CMaptileGeocoderPlugin encapsulates the goecoding  and 
 * maptile fetching functionality. It acts as a base, for a concrete implementation 
 * class to provide all the functionality that a client requires from a 
 * CMaptileGeocoderPlugin implementation.
 *
 */
 
class CMaptileGeocoderPlugin : public CBase
{
public:
	
    /**
	 * Create instance of CMaptileGeocoderPlugin concrete implementation. 
	 * @return: Instance of this class.
	 */
    static CMaptileGeocoderPlugin* NewL();

    /**
	 * Destructor.
  	 */
    virtual ~CMaptileGeocoderPlugin();


public:
	
    /**
	 * Initializes the ReverseGeoCodeObserver
	 * @param: aObserver The observer class instance that is to be notified when 
	 *         geocoding and maptile fetching service completes.
	 * 
	 */
    virtual void SetMaptileGeocoderObserver( MMaptileGeocoderObserver& aObserver ) = 0;
	
    /**
     * Gets the geo-coordinate information for the given address. This is an 
     * asynchronous function whose completion will be notified by the 
     * MMaptileGeocoderObserver::GeocodingCompleted callback.
     * 
     * @param aLandmark Landmark containing the address informations.
     * @param aOption  OCC setting option
     *
     */
	virtual void GetCoordinateByAddressL( const CPosLandmark& aLandmark,  
                       const TConnectionOption aOption = EInteractive ) = 0;
         
    /**
     * Gets the geo-coordinate information for the given address. This is an 
     * asynchronous function whose completion will be notified by the 
     * MMaptileGeocoderObserver::GeocodingCompleted callback.
     * 
     * @param aAddressDetails , containing the address informations.
     * @param aOption  OCC setting option
     *
     */                   
    virtual void GetCoordinateByAddressL( const TDesC& aAddressDetails,  
	                       const TConnectionOption aOption = EInteractive ) = 0;
	
    /**
     * Gets the maptile image path for the given geocordinates. This is an 
     * asynchronous function whose completion will be notified by the 
     * MMaptileGeocoderObserver::MapTileFetchingCompleted callback.
     * 
     * @param aMapTileParam Object containing the geocoordinate, zoom level 
     *        informations.
     * @param aFilePath  Path where the maptile image should  be stored.
     *
     */
    virtual void GetMapTileByGeoCodeL( TMapTileParam& aMapTileParam,
                                       const TDesC& aFilePath ) = 0;

private:
	
    /**
	 * iDtorKey
	 * Identification of the plugin on cleanup
 	 */
    TUid iDtorKey;
};

#include "maptilegeocoderplugin.inl"

#endif //__MAPTILEGEOCODERPLUGIN_H__

//End of file

