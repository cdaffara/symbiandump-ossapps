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
* Description: Maptile interface header implementation.
*
*/

#ifndef __MAPTILEINTERFACE_H__
#define __MAPTIlEINTERFACE_H__

#include <e32std.h> 
#include <e32base.h>
// Header for landmarks db
#include<EPos_CPosLandmarkDatabase.h>
#include <maptilegeocoderplugin.h>


 
/**
 * MMapTileObserver class.
 * Observer class for notifying the maptile related events 
 *
 */
class MMapTileObserver
    {
public:   
    
   /** Informs once maptile is retrived from the REST server
    * 
    * @param[in] aErrCode Error code
    * @param[in] aMapTilePath Path where the maptile image staroed
    * 
    */    
    virtual void MapTilefetchingCompleted( TInt aErrCode,const TDesC& aMapTilePath ) = 0;
    
    /** Informs once geo coordinates  retrived from the REST server
    * 
    * @param[in] aErrCode Error code
    * @param[in] aMapTilePath Path where the maptile image staroed
    * 
    */    
    virtual void GeoCodefetchingCompleted( TInt aErrCode, const TReal& aLatitude,
	                const TReal& aLongitude, const TDesC& aMapTilePath ) = 0;
    
  
    
    };



/** 
 * Class used for interacting with maptile and geocoder components .
 * This class raises the request for geocoding service and maptile
 * fetching service.
 */
class CMapTileInterface : public CBase, MMaptileGeocoderObserver
{
public:

   /**
    * Factory method
    */
    static CMapTileInterface* NewL(); 
    
    /**
     * Interface for requesting GEO fields a landmark object containing address
     * 
     * @param[in] aLandmark Landmark containing address details
     * @param[in] aFilePath Path where the maptile image to be stored
     * @param[in] aObserver Observer for callback notification
     * 
     */
    void GetGeoCodeFromAddressL(  CPosLandmark*  aLandmark , 
            const TDesC& aFilePath, MMapTileObserver* aObserver  );
    
    /**
     * Interface for requesting GEO fields for the address
     * 
     * @param[in] aAddressDetails containing address details
     * @param[in] aFilePath Path where the maptile image to be stored
     * @param[in] aObserver Observer for callback notification
     */
    void GetGeoCodeFromAddressL(const TDesC& aAddressDetails,
            const TDesC& aFilePath, MMapTileObserver* aObserver);
    
    /**
     * Interface for requesting  for a landmark object.
     * @return CPosLandmark object.
     */
    CPosLandmark* GetLandMarkDetails();
        
    
    /**
     * Constructor
     */
    CMapTileInterface();
    
    /**
     * Destructor
     */
    ~CMapTileInterface();
    
    /**
     * Function for Retrieving maptile     
     */
    void GetMapTileL( const TReal& aLatitude, const TReal& aLongitude );    

#ifdef MYLOCATIONENGINE_UNIT_TEST
public:
#else
private:
#endif
	
	/**
	 * Converts geocoordinates to maptile pixel coordinate and update the 
	 * image saving file path to MaptileFolder\RowCol.png
	 */	
	void UpdateFilePathL( const TReal& aLatitude, const TReal& aLongitude );
	
    /**
     * Interface for requesting  for a landmark object.
     * @param[in] aAddressInfo , geo-code address details
     */
    void SetLandMarkDetailsL(MAddressInfo& aAddressInfo);
    
#ifdef MYLOCATIONENGINE_UNIT_TEST
public:
#else
protected:
#endif
	
    /**
     * Derived from MMaptileGeocoderObserver
     */
    void GeocodingCompleted( TInt aErrorcode, MAddressInfo& aAddressInfo );
    
    /**
     * Derived from MMaptileGeocoderObserver
     */
    void MapTileFetchingCompleted( TInt aErrorcode, const TDesC& aMapImagePath );
    
#ifdef MYLOCATIONENGINE_UNIT_TEST
public:
#else
private:
#endif
    
    /**
     * Two phase construction
     */
    void ConstructL();
    
#ifdef MYLOCATIONENGINE_UNIT_TEST
public:
#else
private:
#endif
	
    //MaptileGeocode plugin interface instance
    CMaptileGeocoderPlugin* iMaptileGeocoder;
    //Flag to check street availability
    TBool iStreetAvailable;
    //Maptile image path
	HBufC* iFilePath;
	//Maptile observer
	MMapTileObserver* iObserver ;
	//pointer to landmark  from address information.
	CPosLandmark  *iLandmark;

    };
#endif // MAPTILEINTERFACE_H

