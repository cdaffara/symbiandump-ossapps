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
*     maptile service interface
*
*/

#ifndef _MAPTILESERVICE_H_
#define _MAPTILESERVICE_H_


#include <QString>
#include <QtGlobal>
#include <QObject>
#include <qmobilityglobal.h>
#include <locationservicedefines.h>

QTM_BEGIN_NAMESPACE
class QValueSpacePublisher;
class QValueSpaceSubscriber;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

#ifdef  MAPTILESERVICEDLL
#define MAPTILESERVICE_EXPORT Q_DECL_EXPORT
#else
#define MAPTILESERVICE_EXPORT Q_DECL_IMPORT
#endif


// CLASS DECLARATION

/**
*  Maptile service class,  provides interface to get map tile image associated with 
*  contact. Also provides interface to check whether location feature is enabled or disabled.
*
*  Note: Location feature can be enabled or disabled by modifying conf\cntmaptileservice.confml file.
*/
class MAPTILESERVICE_EXPORT MapTileService : public QObject
{
    Q_OBJECT
public:

    /** 
     * Maptile fetching status
     */
    enum MapTileStatus
    {
        /** Map tile fetching completed */
        MapTileFetchingCompleted = 0,
        /** Map tile fetching in progress */
        MapTileFetchingInProgress,
        /** Map  tile fetching n/w error */
        MapTileFetchingNetworkError,
        /** Map tile fetching invalid address */
        MapTileFetchingInvalidAddress,
        /** Map tile fetching unknown erro */
        MapTileFetchingUnknownError
    };

    /**
     * Application types      
     */
    enum AppType
    {
        /** Contacts application */
        AppTypeContacts,
        /** Calendar application */
        AppTypeCalendar
    };
	
    /** 
     * Address types
     */
    enum AddressType      
    {
        /** Plain Address Type */
        AddressPlain,
        /** Address Type Pref */
        AddressPreference,
        /** Address type Work */
        AddressWork,
        /** Address type Home */
        AddressHome,
        /** Address type Unknown */
        AddressUnknown
    };
        
    MapTileService();
    
    ~MapTileService();
    
   /**
    * Checks whether location feature enabled or disabled for specific application.
    * 
    * @return Returns true or false based on location feature setting.
    */
    bool isLocationFeatureEnabled(AppType appType);
            
   /**
    * Gets a maptile image associated with a id(contact id/calendar id). 
    * Returns a maptile image path if it is available otherwise returns NULL.
    * 
    * @param id,  entry id     
    * @param sourceType , Source address type( Preferred, Home , Work address )
    * @param imagePath  Maptile image path associated with the app id
    * @param orientation Application current orientation.   
    *      
    * @return Returns the maptile fetching status.
    */
    int getMapTileImage( int id, AddressType sourceType, 
             QString& imagePath, Qt::Orientations orientation = Qt::Vertical  );  
    
    /**
    * keep existing location as per user confirmation . 
    * @param id ,entry id     
    * @param sourceType Source address type( Preferred, Home , Work address )
    * @param value ,true if user confirm as yes otherwise false.
    */
    void keepExistingLocation(int id ,AddressType sourceType, bool value);
    
public slots: 
    /**
     * Receives maptile status information and emits the maptilFetchingStatusUpdate
     * signal.
     */
    void setMaptileStatus();

signals:
    /**
     * Signal to update the maptile fetchings status to contact/calendar application.
     * @param id  app id     
     * @param addressType Source address type( Preferred, Home , Work address )
     * @param status Maptile status for the associated address
     */
     void maptileFetchingStatusUpdate( int id, int addressType, int status );
    

private:

    /**
     * Publish the contact entry id , AddressType and number of address
     * @param id, Contact id .
     * @param sourceType, address type( Preferred, Home , Work address )
     * @param addressCount ,number of address to be published. 
     */
    void publishValue(int id, AddressType sourceType, int addressCount );
    /**
    * Publish the calendar entry id , AddressType and number of address
    * @param id, calendar entry id .
    * @param sourceType, address type( plain) 
    * @param addressCount ,number of address to be published. 
    */
    void publishCalEntry(int id);
    
    /**
    * Read entry from database
    * @param id, Contact/calendar id .
    * @param sourceType, address type( plain ,Preferred, Home , Work address )
    * @param aLookupItem ,entry information
    * @param aNoOfAddress ,number of address read from databse 
    */
    int readEntryFromMaptileDataBase( int id, AddressType sourceType,
                MaptileLookupItem& aLookupItem, int& aNoOfAddress  );

private:
    
    //The contact/calendar id for which maptile requested
    int mLastViewedEntryId;
	
    //Maptile request publisher
    QValueSpacePublisher *mPublisher;
    //Maptile status request subscriber
    QValueSpaceSubscriber *mSubscriber;
	//Calendar Maptile request publisher
    QValueSpacePublisher *mCalPublisher;
};

#endif //MAPTILESERVICE_H_

