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
* Description: defines api for lookup database.
*
*/


#ifndef LOCATIONDATA_LOOKUPDB_H
#define LOCATIONDATA_LOOKUPDB_H

// DLL_EXPORT macro definition
#ifdef BUILD_DLL
#define DLL_EXPORT Q_DECL_EXPORT
#else
#define DLL_EXPORT Q_DECL_IMPORT
#endif



// INCLUDES
#include <QObject>
#include <locationservicedefines.h>

class QSqlDatabase;
class QSqlQuery;

class QLookupItem
{
    
public:
    
    enum IconType
    {
        /** Icon type default */
        EIconTypeDefault,
        /** Icon type from gallery */
        EIconTypeGallery,
        /** Icon type from flicker */
        EIconTypeFlicker,
        /** Icon type from contact thumbnail */
        EIconTypeContactThumbnail,
        /** Icon type maptile */
        EIconTypeMapTile
    };
    
public:
    // unique id of the entry
    quint32 mId;

    // flag to identify if the entry is a duplicate
    quint32 mIsDuplicate;

    // Uid of the source entry
    quint32 mSourceUid;

    // Source type
    quint32 mSourceType;

    // Dest uid in the landmarks database
    quint32 mDestId;
   
    // name
    QString mName;

    // Street
    QString mStreet;

    // Postal code
    QString mPostalCode;

    // City
    QString mCity;

    // State
    QString mState;

    // Country
    QString mCountry;
    
    // latitude
    double mLatitude;
    
    // longitude
    double mLongitude;
    
    // icon type
    quint32 mIconType;
    
    // icon path
    QString mIconPath;

    // map tile path
    QString mMapTilePath;
    
    //One line address
    QString mSingleLineAddress;
};

/**
* Location data for calendar specific.
*/
class QCalendarLocationDetails
{
public:
    // Uid of the source entry
    quint32 mSourceUid;
    //One line address
    QString mOnelineLocation;
};

/**
* Location data lookup database class.
*/
class DLL_EXPORT LocationDataLookupDb : public QObject
{
	
public: // Constructor

    /**
    * Constructor
    */
    LocationDataLookupDb( QObject *parent = 0 );	

    /**
    * Destructor
    */
    ~LocationDataLookupDb();  

    /**
    * Opens the lookup database.
    */
    bool open();

    /**
    * Closes the lookup database.
    */
    void close();

    /**
    * Creates an entry in the lookup table.
    * @param[in] aLookupItem The lookup item to be created in the database.
    */
    void createEntry( QLookupItem& aLookupItem );

    /**
    * Updates an entry in the lookup table.
    * The source id and type is used to find the entry in db
    * @param[in] aLookupItem The lookup item to be updated in the database.
    */
    void updateEntryBySourceIdAndType( const QLookupItem& aLookupItem );

    /**
    * Updates an entry in the lookup table.
    * The source id and type is used to find the entry in db
    * @param[in] aLookupItem The lookup item to be updated in the database.
    */
    void updateMaptileBySourceIdAndType( quint32 aSourceId, quint32 aSourceType, QString aImagePath );

    /**
    * Updates an entry in the lookup table.
    * The id is used to find the entry in db
    * @param[in] aLookupItem The lookup item to be updated in the database.
    */
    void updateEntryById( const QLookupItem& aLookupItem );

    /**
    * Deletes an entry from the lookup table.
    * The source id and type is used to find the entry in db
    * @param[in] aLookupItem The lookup item to be deleted from the database.
    */
    void deleteEntryBySourceIdAndType( const QLookupItem& aLookupItem );

    /**
    * Finds an entry in the lookup table.
    * @param[in/out] aLookupItem The lookup item to be found in the database. The source id and source type 
    * is passed in the lookup item. If the entry is found, all other fields are updated in the lookup item.
    * @return true if found, else false
    */
    bool findEntryBySourceIdAndType( QLookupItem& aLookupItem );

    /**
    * Finds an entry in the lookup table.
    * @param[in/out] aLookupItem The lookup item to be found in the database. The id is passed 
    * in the lookup item. If the entry is found, all other fields are updated in the lookup item.
    * @return true if found, else false
    */
    bool findEntryById( QLookupItem& aLookupItem );

    /**
    * Finds list of lookup items given a landmark id.
    * @param[in] aLandmarkId The landmark id to be found in the lookup database.  
    * @param[out] aLookupItemArray List of lookup entries found.  
    */
    void findEntriesByLandmarkId( const quint32 aLandmarkId, 
            QList<QLookupItem>& aLookupItemArray );

   /**
    * Gets list of lookup items.
    * @param[in] aCollectionId The collection id, whose whose corresponding entries needs to be fetched.
    *            By default all the entries in the lookup db are fetched.  
    * @param[out] aLookupItemArray List of lookup entries found.  
    */
    void getEntries( QList<QLookupItem>& aLookupItemArray, const quint32 aCollectionId = ESourceInvalid );
    
   /**
    * Gets count  of lookup items.
	* @param[in/out] aCount  The number of items in lookup table for the collection id
    * @param[in] aCollectionId The collection id, whose whose corresponding entries needs to be fetched.
    *            By default all the entries in the lookup db are fetched.  
    */
    void getCount( QList<int>& aCount,const quint32 aCollectionId = ESourceInvalid );

   /**
    * Gets single line address
    * @param mId , id of the entry
    * @param mSourceType , type of entry
    * @return QString , address associated with cuurent id and type  
    */
    QString getAddressDetails( quint32 mId , quint32 mSourceType );
    
    /**
    * Updates the entry name 
    * @param id , id of the entry
    * @param sourceType , type of entry
    * @param name , name of entry
    */
    void updateEntryNameByIdAndType(quint32 id, quint32 sourceType , QString name);
    
    /**
    * Updates the calendar entry  
    * @param id , id of the calendar entry
    * @param location , location  
    */
   void updateCalendarLocationById(quint32 id,QString location);    
   
    /**
    * Delete the calendar entry  
    * @param id , id of the calendar entry
    */
   void deleteCalendarEntry(quint32 id); 
   /**
    * gets all the calendar entry  
    * @param lookupItemArray , refference to all entry
    */
   void getAllCalendarEntry(QList<QCalendarLocationDetails>& lookupItemArray);
    
private:
	
    // fills the lookup entry
    void fillLookupEntry( QSqlQuery &aRecord, QLookupItem &aLookupItem );    
    
    // Handle to the items database
    QSqlDatabase *mDb;

    // Flag to indicate if db is open
    bool mDbOpen;
};
#endif  // LOCATIONDATA_LOOKUPDB_H

// End of file
    


