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
* Description: Mylocation engine header implementation.
*
*/


#ifndef __MYLOCATIONSENGINE_H__
#define __MYLOCATIONSENGINE_H__

// INCLUDES
// calendar related headers
#include <calsession.h>
#include <calentryview.h>

// Header for contact DB
#include <cntdb.h>

//Qt contacts mobility apis
#include <qtcontacts.h>
#include <qcontactmanager.h>

// Header for landmarks db
#include<EPos_CPosLandmarkDatabase.h>

// mylocations database manager
#include "maptiledblookuptable.h"
#include "maptileinterface.h"
#include "mylocationsdatabasemanager.h"
#include "calendernotification.h"
#include "mylocationgeotagtimerao.h"
class GeocodeUpdate;
class CContactSubscriber;
class CCalendarSubscriber;
using namespace QTM_NAMESPACE;

/**  
 * This class contains the required data  for maptile image  processing.
 * Each  address added/modified in contact database will be added as queue
 * using this class for maptile processing.
 */
class CMapTileRequest: public CBase
{
public:
    //Consturctor
    CMapTileRequest():iLandmarkInfo(NULL),iAddressDetails(NULL){};
    //Destrucotr
    ~CMapTileRequest(){delete iLandmarkInfo;delete iAddressDetails;};
public:
    //Landmark contains address details
    CPosLandmark* iLandmarkInfo;
    //single row address.
    HBufC* iAddressDetails;
    //entry id
    TInt32 iUId;
    //Address type 
    TInt32 iAddressType;
    //Image path 
    TFileName iImagePath;
    //Event type    
    TInt iEventType;
};

/**  
 * This class stores the information required for  maptile
 * processing when it is triggered from the application(contact)
 * viewer.
 */
class TAppAddressInfo
{
public:    
  TInt iUid;
  TInt iAddressType;
};

// CLASS DECLARATION
/**  Class used to listen on various database change events from applications like calendar, 
 * contacts, landmarks
 * 
 * This class listens on database change events from applications like contacts, calendar, landmarks
 * and triggers updating the Mylocations database accordingly. 
 * This process starts on phone bootup and always runs in the background.
 */
class CMyLocationsEngine : public CActive, public MCalChangeCallBack2,
				 public MContactDbObserver, public MMapTileObserver,public MNotifyChange,
				 public MyLocationTimerObserver
			
{
    
public:
    
    /** 
     * Contact address types
     */
    enum TContactAddressType
        {
        /** Address Type Pref */
        EAddressPref,
        /** Address type Work */
        EAddressWork,
        /** Address type Home */
        EAddressHome,
        };

public:

    /** 
     * CMyLocationsEngine::NewL()
     */
    static CMyLocationsEngine* CMyLocationsEngine::NewL();
    
    /**
     * ~CMyLocationsEngine.
     * Virtual Destructor.
     */
    virtual ~CMyLocationsEngine();
    
    /** Starts calender change notification method to get callbacks when entries are added/modified/deleted in calender
     *
     */      
    void StartCalenderChangeNotifierL();

    /** Starts contacts change notification method to get callbacks when entries are added/modified/deleted in contacts
     *
     */      
    void StartContactsChangeNotifierL();

    /** Starts landmarks change notification method to get callbacks when entries are added/modified/deleted in landmarks
     *
     */      
    void StartLandmarksChangeNotifier();


    
    /** Maps the source type change type to Mylocations entry change type
    * @param[in] aSrcType Source type of the change
    * @param[in] aChangeType Type of change ( added/modified/deleted ).
    * @return The mylocations entry change type value.
     */      
    TEntryChangeType MapChangeType( TUidSourceType aSrcType, TUint32 aChangeType );    
 
#ifdef MYLOCATIONENGINE_UNIT_TEST
public:
#else
private:
#endif
    
    //cenrep setting for db in sync state.
    enum TCenrepSetting
    {
        //get the value
        EMapTileDbStatusGet=0,
        //set the value
        EMapTileDbStatusSet
    };
    
    /**
     * ConstructL.
     * 2nd phase constructor.
     */
    void ConstructL();
    
    /**
     * CMyLocationsEngine.
     * C++ default constructor. 
     */
    CMyLocationsEngine();
    
    /** Creates a landmark object from contact item's location details and checks whether address available.
     * @param[in] aContactAddress A contact address whose location details needs to be used to create landmark object.
     * @returns A valid landmark object if location in the contact item is validated, NULL otherwise.
     */      
    CPosLandmark* GetContactAddressDetailsLC( QContactAddress& aContactAddress );
            
    /**
     *  A call back function for calendar change notification
	 *	@param aChangeItems  List of changes since the last notification.	
	 */      
    virtual void CalChangeNotification( RArray<TCalChangeEntry>& aChangeItems );
    
    /** Tests the contact database observer event type and handles it. The ID of a 
    * contact affected by the change event, if relevant, can be retrieved via TContactDbObserverEvent::iContactId.
    * @param aEvent Provides information about the change event. 
    */
    virtual void HandleDatabaseEventL( TContactDbObserverEvent aEvent );
    
    /** 
     * Triggers the maptile processing for complete address
     * @param aEvent  Provides information about the change event.   
     */      
	void TriggerMaptileRequestL(  TContactDbObserverEvent& aEvent  );
			
    /** 
    * Process the maptile database event and updates the maptile database
    * @param aEventType  Provides information about the type of request address.
    * @param aLookupItem Provides information about single address of contact.  
    */  
	void UpdateMaptileDatabaseL(TInt aEventType ,
	                             MaptileLookupItem& aLookupItem );
	
    /**
     *  Handles active object's request completion event. 
     */
    void RunL();

    /** 
     * Implements cancellation of an outstanding request. 
     */
    void DoCancel();
 
    
    /** 
     * Requests for map tile image , structure serch.
     * @param aLandmark  Provides information about the address.
     * @param aAddressType Provides information about address type .  
     * @param aUId, uid of the event.
     */
    void RequestMapTileImageL( CPosLandmark&  aLandmark, const TUidSourceType aAddressType,
                               const TInt32 aUId, const TInt aEventType  );
    
    /** 
     * Requests for map tile image , one box serch.
     * @param aAddressDetails  Provides information about the address.
     * @param aAddressType Provides information about address type .  
     * @param aUId, uid of the event.
     */
    void RequestMapTileImageL(const TDesC& aAddressDetails,
            const TUidSourceType aAddressType, const TInt32 aUId,const TInt aEventType);
 
    /** 
     * Checks whether contact has a valid geocoordinates.
     * @param aContact  Provides all the required information about the contact.
     * @param aAddressType Provides information about address type .  
     * @param aLatitude, Latitude information.
     * @param aLongitude, Longitude information.
     * @return If geocoordinate available returns TRUE otherwise FALSE
     */    
    TBool IsGeoCoordinateAvailable( QContact& aContact, 
            QString aAddressType, double& aLatitude , double& aLongitude );
    
    /** 
     * Add to maptile request queue.
     * @param mapTileRequest, request queue.
     * @return If successful, retrun KErrNone, or return some systemwide error.
     */
    TInt RequestExecute( CMapTileRequest* aMapTileRequest);
	/**
    * SetFolderPath.
    * Sets the maptile storing path.
    */
    void SetFolderPathL();

    /**
    * Refreshes calendar entries in mylocation databases. This is typically called when there is an undefined 
    * change in calendar db.
    */
    void RefreshCalendarEntryListL();    

    /**
    * Deletes invalid calendar entries from mylocation databases.
    */
    void DeleteInvalidCalendarEntriesL();    
    
    /**
    * Returns True if a calendar entry is available for a given id.
    */
    TBool IsCalendarEntryValidL( TUint32 aId );    

    /**
    * Calender entry added.
    */
    void CalenderEntryAddedL(TCalChangeEntry aCalChangeEntry);    

    /**
    * Calender entry modifyied.
    */
    void CalenderEntryModifyL(TCalChangeEntry aCalChangeEntry);
    /**
    * Update the mylocations database.
    */
    void UpdateDatabaseL( CPosLandmark* aLandmark, const TUint32 aUid, 
            const TUint32 aSourceType, const TEntryChangeType aChangeType );
			
    /** 
     * Informs the geocode completion
     */
    void UpdateGeoCodeToAppDataBase( TReal latitude,TReal longitude );		
    
    /** 
     * Process the pending maptile requests
     */
    void ProcessNextMaptileRequest();
    
    /**
     * maptile database manipulation.
     */
    void ManipulateMapTileDataBaseL(MaptileLookupItem& aLookupItem);
    
    /**
     * Crop and create multiple maptile images for different applications requirements.
     */
    void CreateMultipleMaptiles( const TDesC& aMaptilePath );
    
    /**
     * Crop the maptile image and save the different files.
     */
    void CropAndSaveImage( QString filePath, int width, 
                               int height, QString appType, QString orientationType );

	/**Get the contact name for this perticular id.
     */
	void GetContactName( TInt32 iUId , TDes& aName );
	
    /**
    * Publishing the entry after maptile db modification.
    * @param aUid, entry id
    * @param aLookupItem, all entry details
    */
    void PublishEntry(TInt32 aUId,  MaptileLookupItem& aLookupItem);
    
    /**
    * Get/Set the maptile db status 
    * @param aSettingType, Type of setting
    * @param aValue, Value for the key
    */
    void MaptileDbSyncStatusL(const TCenrepSetting& aSettingType, TInt& aValue);
     
public:  //From MMapTileObserver
    
    /**
     *  Informs the maptile fetching completion  
     */ 
    void MapTilefetchingCompleted( TInt aErrCode,const TDesC& aMapTilePath );
    
    /**
     *  Informs the geo codes fetching completion  
     */
    void GeoCodefetchingCompleted( TInt aErrCode, const TReal& aLatitude,
	                                   const TReal& aLongitude, const TDesC& aMapTilePath );
public://from MNotifychange
    void NotifyChangeL(TInt &aStatus);
    void GetChangeNotificationL(TInt &aId, TInt &addressType, TInt &addressCount );
    void SubscribeFromCalendarL(TInt aId);
	
public: //from MyLocationTimerObserver
    /** 
     * Notifies the timer expiry when 3AM timer was started to notify. 
     */
    void MyLocationThreeAMTimerExpiredL();

    
#ifdef MYLOCATIONENGINE_UNIT_TEST
public:
#else
private:
#endif

    // Data member
    
    //handle to timer class
    //calendar session handle
    CCalSession *iCalSession;
    
    // calendar view
    CCalEntryView *iCalView;   

    // Handle to the contacts database
    CContactDatabase* iContactsDb;
    
    // Receives events reporting changes to a contact database.
    CContactChangeNotifier *iContactChangeNotifier;
    
    // Handle to landmarks db
    CPosLandmarkDatabase* iLandmarkDb;
    
    // landmark change event type.
    TPosLmEvent iLmEvent;
   
    //Handle to maptile interface
    CMapTileInterface* iMapTileInterface;
    
    CMyLocationsDatabaseManager*  iMyLocationsDatabaseManager;
    
    //maptiledb 
    LookupMapTileDatabase* iMaptileDatabase;

    //Current event type
    TInt iEventType;
    
    //Maptile image request queue
    RArray<CMapTileRequest*> iMapTileRequestQueue;
    
    // 3am Timer
    CLocationGeoTagTimerAO *iMyLocationThreeAMTimer;
    
    //Flag to check maptilegeocoder plugin availability
    TBool iMaptileGeocoderPluginAvailable;
	
	//Maptile image path
    TFileName imageFilePath;
    
    //Object to listen calender db create notification.    
    CCalenderNotification  *iCalenderNotification;
    
    //Contact manager instance for retrieving contact info.
    QContactManager* iContactManager;
    
    //Subscribe from contact
    CContactSubscriber *iContactSubscriber;
    
    //Subscribe from calendar
    CCalendarSubscriber *iCalendarSubscriber;
    
    //Geo-code class instance
    GeocodeUpdate *iGeocodeUpdate;
       
    //Last viewed contact id
    TInt iLastContactId;
    
    //Last viewed calendar id
    TInt iLastCalendarId;
    
    //Address information for viewver maptile processing.
    RArray<TAppAddressInfo*> iAddressInfo;
   
};

#endif // __MYLOCATIONSENGINE_H__
// End of File
