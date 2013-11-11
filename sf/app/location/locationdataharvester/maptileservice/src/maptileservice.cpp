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
* Description: 
*     Maptile service implementation
*
*/


#include <centralrepository.h> 
#include <locationservicedefines.h>
#include <qvaluespacepublisher.h>
#include <qvaluespacesubscriber.h>
#include <qfile.h>
#include "mylocationlogger.h"

#include "maptileservice.h"
#include "mylocationsdefines.h"
#include "maptiledblookuptable.h"
#include <QVariant>
// CONSTANTS
// Maptile interface uid
const TUid KUidMapTileInterface = { 0x2002E6E8 };

// Central Repository Key IDs
const TInt KEnableLocationFeature  = 0x1;
const TInt KMaptileDbSyncState  = 0x2;

const char *MAPTILE_STATUS_RECEIVER = "/maptilestatuspublisher/name";
const char *MAPTILE_STATUS_PUBLISHER = "/maptilecontactpublisher";
const char *MAPTILE_STATUS_CALENDAR_PUBLISHER = "/maptilecalpublisher";


// -----------------------------------------------------------------------------
// MapTileService::MapTileService()
// Default constructor
// -----------------------------------------------------------------------------
//
MapTileService::MapTileService():mCalPublisher(0)
{
   /* Constructs a QValueSpacePublisher that publishes values under path /mypublisher*/
    mPublisher = NULL;    
    mSubscriber = new QValueSpaceSubscriber( MAPTILE_STATUS_RECEIVER );
    //Connect for maptile status change key
    QObject::connect(mSubscriber, SIGNAL(contentsChanged()), this, SLOT(setMaptileStatus()));
    
}

// -----------------------------------------------------------------------------
// MapTileService::~MapTileService()
// Destructor
// -----------------------------------------------------------------------------
//
MapTileService::~MapTileService()
{
    if( mSubscriber )
    {
        delete mSubscriber;
        mSubscriber = NULL;
    }
    
    if( mPublisher )
    {
        delete mPublisher;
        mPublisher = NULL;
    }
	if( mCalPublisher )
	{
	    delete mCalPublisher;
		mCalPublisher = NULL;
	}
}
// -----------------------------------------------------------------------------
// MapTileService::isLocationFeatureEnabled()
// Checks whether location feature is enabled or disabled
// -----------------------------------------------------------------------------
//
bool MapTileService::isLocationFeatureEnabled(AppType appType)
{    
    //Create the centrep with uid 0x2002C3A8
    bool enableLocationFeature = false;
    
    CRepository* centralRepository = NULL;  
     
    TRAPD( err, centralRepository = CRepository::NewL( KUidMapTileInterface ) );
    if ( KErrNone == err )
    {
        TInt repValue;
      
        //Get the Location feature flag
        int ret=0;
        if ( appType == AppTypeContacts )
        {
            ret = centralRepository->Get( KEnableLocationFeature , repValue );
        }
        else if ( appType == AppTypeCalendar )
        {
             ret = centralRepository->Get( KEnableLocationFeature , repValue );
        }
      
        if ( ret == KErrNone && repValue == 1 )
        {
            enableLocationFeature  = true;
        }
           
         delete centralRepository;
    }
           
    return   enableLocationFeature;
    
}


// -----------------------------------------------------------------------------
// MapTileService::getMapTileImage()
// Gets the maptile image path associated with a contact.
// -----------------------------------------------------------------------------
//
int MapTileService::getMapTileImage( int id, AddressType sourceType, 
             QString& imagePath, Qt::Orientations orientation )    
{
    __TRACE_CALLSTACK;
    
     if ( sourceType == AddressPlain ){
         CRepository* centralRepository = NULL; 
         TRAP_IGNORE( centralRepository = CRepository::NewL( KUidMapTileInterface ) );
         int repValue=0;
         if(centralRepository){
            centralRepository->Get( KMaptileDbSyncState , repValue );            
            delete centralRepository;   
            if(repValue == 1){ 
                publishCalEntry( id );
                return MapTileFetchingInProgress;    
            }
        }
    }    
    
    MaptileLookupItem lookupItem;    
    int addressCount = 0;  
    int maptileStatus = MapTileFetchingUnknownError;
    int error;
    error = readEntryFromMaptileDataBase( id, sourceType, lookupItem, addressCount );
        
    //if entry available returns the file path otherwise NULL. 
    if ( KErrNone == error  )
    {
     
        maptileStatus = lookupItem.iFetchingStatus;
        MYLOCLOGSTRING1("getMapTileImage maptileStatus  - %d ", maptileStatus );
        
        if( maptileStatus == MapTileFetchingCompleted )
        {
            //Get the image path
	          QString imageFile(lookupItem.iFilePath);
	          imagePath = imageFile;
	        
	          if( orientation == Qt::Vertical )
	          {
	              imagePath.append( MAPTILE_IMAGE_PORTRAIT ); 
	          }
	          else
	          {
	              if( sourceType == AddressPlain )
	              {
	                  imagePath.append( MAPTILE_IMAGE_CALENDAR );
	              }
	              else if ( sourceType == AddressHome || 
	                   sourceType == AddressPreference || sourceType == AddressWork )
	              {
	                  imagePath.append( MAPTILE_IMAGE_CONTACT );
	              }
	              imagePath.append( MAPTILE_IMAGE_LANDSCAPE );
	          }
	          
	          //Check if file exist
	          if( !QFile::exists( imagePath ) )
	          {
	              imagePath.clear();
	           		maptileStatus = MapTileFetchingUnknownError;
	          }
	      }
        else if( maptileStatus == MapTileFetchingNetworkError ||
                    maptileStatus == MapTileFetchingInProgress )
        {
            switch(sourceType)
            {           
                case AddressHome:            
                case AddressPreference:           
                case AddressWork:
                {
                    publishValue( id, sourceType, addressCount );
                    break;
                }
                
                case AddressPlain:
                {
                    publishCalEntry( id );
                    break;
                }
                default:
                    break;
            };
           		  
        }
                    

    }
    else if ( KErrNotFound == error )
    {
        MYLOCLOGSTRING("getMapTileImage entry not found, return MapTileFetchingInProgress" );
        //If entry is not found , it will be a newly added entry.       
        switch (sourceType) {
            case AddressHome:
            case AddressPreference:
            case AddressWork:
            {
                publishValue(id, sourceType, addressCount);
                break;
            }
    
            case AddressPlain:
            {
                publishCalEntry(id);
                break;
            }
            default:
                break;
        };
        maptileStatus = MapTileFetchingInProgress;
    }
 
    //Return the maptile status
    MYLOCLOGSTRING("getMapTileImage  returns MapTileFetchingUnknownError" );
    return maptileStatus;

}

// -----------------------------------------------------------------------------
// MapTileService::setMaptileStatus()
// Gets call back from location engine , after maptile fetching done ,
// to set status and emit signal . 
// -----------------------------------------------------------------------------
//
void MapTileService::setMaptileStatus()
{
    __TRACE_CALLSTACK;
    QString imagePath;
    AddressType addressType = AddressUnknown;   
    QStringList subPath = mSubscriber->subPaths();
    QVariant value = mSubscriber->value(subPath.at(0));
      
    //Subscriber Protocol : [appid-addresstype-maptilestatus]
    QStringList text = value.toString().split("-");
    
    int id = text.at(0).toInt();
    int status = text.at(2).toInt();
    
    switch( text.at(1).toInt() )
    {
        case ESourceContactsPref:
            addressType = AddressPreference;
            break;
        case ESourceContactsWork:
            addressType = AddressWork;
            break;        
        case ESourceContactsHome:
            addressType = AddressHome;
            break;
        case ESourceCalendar:           
            addressType = AddressPlain;
            break;
        default:
            break;
    }
            
    //Emit the maptile status signal 
    int type = addressType;
    MYLOCLOGSTRING1("setMaptileStatus() status  - %d ", addressType );
    if( mLastViewedEntryId == id )
    {
        emit maptileFetchingStatusUpdate( mLastViewedEntryId, type , status );
	}
  
}

// -----------------------------------------------------------------------------
// MapTileService::publishValue()
// publish the contact entry 
// -----------------------------------------------------------------------------
//
void MapTileService::publishValue( int id, AddressType sourceType, int addressCount )
{
    __TRACE_CALLSTACK;
    mLastViewedEntryId = id;
    
	int addressType = ESourceInvalid;    
    switch( sourceType )
    {
        case AddressPreference:
            addressType = ESourceContactsPref;
            break;
        case AddressWork:
            addressType = ESourceContactsWork;
            break;
        case AddressHome:
            addressType = ESourceContactsHome;
            break;
    }
    
    if ( !mPublisher )
    {
        /* Constructs a QValueSpacePublisher that publishes values under path /mypublisher*/
        mPublisher = new QValueSpacePublisher( MAPTILE_STATUS_PUBLISHER, this);
    }
   
    
    /* Publisher protocol [appid-addresstype-count] */
    QString addressId;
    addressId.append( QVariant(id).toString());
    addressId.append( QChar('-') );
    addressId.append( QVariant(addressType).toString());
    addressId.append( QChar('-') );
    addressId.append( QVariant(addressCount).toString() );
    mPublisher->setValue("name", addressId.toAscii() );
    mPublisher->sync();
    
}

// -----------------------------------------------------------------------------
// MapTileService::publishValue()
// publish the calendar entry 
// -----------------------------------------------------------------------------
//
void MapTileService::publishCalEntry( int id)
{
    __TRACE_CALLSTACK;
    mLastViewedEntryId = id;       
    if ( NULL ==mCalPublisher )
    {
        /* Constructs a QValueSpacePublisher that publishes values under path /mypublisher*/
        mCalPublisher = new QValueSpacePublisher( MAPTILE_STATUS_CALENDAR_PUBLISHER, this);
    }    
    mCalPublisher->setValue("name", id);
    mCalPublisher->sync();
}

// -----------------------------------------------------------------------------
// MapTileService::readEntryFromMaptileDataBase()
// Read all entries from database
// -----------------------------------------------------------------------------
//
int MapTileService::readEntryFromMaptileDataBase( 
            int id, AddressType sourceType, MaptileLookupItem& aLookupItem, int& aNoOfAddress )
{
    
    TInt err = KErrUnknown;

    //Maptile database  instance
    LookupMapTileDatabase* mapTileDatabase = new LookupMapTileDatabase();
    
    if ( mapTileDatabase )
    {     
       if( mapTileDatabase->open() )
       {
           err = KErrNone;
           aNoOfAddress = mapTileDatabase->findNumberOfAddress(id);
           aLookupItem.iUid = id;
           switch (sourceType)
           {
               case AddressPlain:
                   aLookupItem.iSource = ESourceCalendar;
                   break;
               case AddressPreference:
                   aLookupItem.iSource = ESourceContactsPref;
                   break;
               case AddressWork:
                   aLookupItem.iSource = ESourceContactsWork;
                   break;
               case AddressHome:
                   aLookupItem.iSource = ESourceContactsHome;
                   break;
               default:
                   break;
           }
           
           if( !mapTileDatabase->getEntry( aLookupItem ) )
           {
              err = KErrNotFound;
           }
     
           MYLOCLOGSTRING1("getMapTileImage getEntry returns - %d ", err );
        }
        //delet the database instance
        delete mapTileDatabase;
           
    }
    
    return err;
}

// -----------------------------------------------------------------------------
// MapTileService::keepExistingLocation()
// update the map tile table user setting status according to user setting status
// -----------------------------------------------------------------------------
//
void MapTileService::keepExistingLocation(int id ,AddressType sourceType, bool value)
{
    //Maptile database  instance
    LookupMapTileDatabase mapTileDatabase ;
    switch(sourceType)
    {
        case AddressPlain:
        {
            if( mapTileDatabase.open() )
            {
                MaptileLookupItem lookupItem;                
                lookupItem.iUid=id;
                lookupItem.iSource=sourceType;
                mapTileDatabase.getEntry(lookupItem);                
                lookupItem.iUserSetting=value;
                if(!value)
                {
                    lookupItem.iFetchingStatus=MapTileFetchingInProgress;
                }
                mapTileDatabase.updateUserSetting(lookupItem);               
            }
            break;
        }
        default:
            break;
    }

}
// End of file
