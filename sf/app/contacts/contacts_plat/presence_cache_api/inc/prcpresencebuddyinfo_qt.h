/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for presence buddy presence info - Qt.
*
*/

#ifndef _M_PRESENCEBUDDYINFO_QT_H_
#define _M_PRESENCEBUDDYINFO_QT_H_

#include <e32std.h>
#include <badesca.h>
#include <QList>
#include <QString>


/**
* Common field names. 
* If you make your own extension it is recommended to use "x-" prefix in your fields, 
* e.g. "x-myapp-location".
*/ 
namespace PrcPresenceCacheQtFieldNames
{
    
    /**
     * Field type for "status message" field.
     *
     * Status message field contains user written
     * free text, describing for example his/hers current
     * presence status.
     */
     const QString  StatusMessage = "statustext";
    
    /**
     * Field type for "avatar" field.
     *
     * Avatar field contains user selected image. Image may be
     * an avatar describing the user or it may be image showing
     * user current presence status.
     */
    const QString Avatar = "avatar";
    
    
    /**
     * Field type for "availability-text" field.
     *
     * Availability text field describes presentity's communication
     * availability as text. This is useful in case of brand
     * presence availability, since any text value is possible.
     *
     */
    const QString Availability = "availabilitytext";
    
    /**
     * Field type for "serviceiconbrandid-text" field.
     *
     * Service icon brand ID text field identifies the service brand which
     * the current presence information is from. Brand information can
     * be used with BrandingServer to fetch branded content to UI.  
     *
     */
    const QString ServiceIconBrand =  "ServiceBrandingID" ;
    
    /**
     * Field type for "serviceiconentryid-text" field.
     *
     * Service icon entry ID text field identifies the icon which represents
     * the current presence service. Icon id can be used with branding
     * information to fetch the actual image from BrandingServer.  
     *
     */
    const QString ServiceIconId = "ServiceIconEntryID" ;
    
    /**
     * Field type for "timestamp" field.
     *
     * TimeStamp field represents the time when the presence information
     * was last updated. Client can show the TimeStamp on the UI or omit
     * old presence information completely.
     *
     */
    const QString TimeStamp = "TimeStamp";        
    /**
     * Field type for "expiry" field.
     *
     * Expiry field represents the time (microseconds) how long buddy 
     * information is valid with out new buddy info write events. 
     * 
     * If Buddy information is not updated in expiry time then buddy 
     * information is removed automatically from presence cache.
     *
     */
    const QString Expiry = "expiry" ;

}

/**
 * Interface for buddy presence info. 
 *
 */       
class PrcPresenceBuddyInfoQt
{
public:

    /**
     * Common enumerated availability values for "availability"
     * presence information field.
     *
     */
    enum AvailabilityValues
        {            
        /**
         * Presentity status is unknown.
         */
        PrcUnknownAvailability = 0,                

        /**
         * Presentity is not available for communication.
         */
        PrcNotAvailable = 1,

        /**
         * Presentity is currently busy.
         * Presentity may or may not be available
         * for communication.
         */
        PrcBusy = 2,

        /**
         * Presentity is available for communication.
         */
        PrcAvailable = 3
        };


    /**
     * Factory method to instantiate MPresenceBuddyInfoQt
     *
     * @return The new MPresenceBuddyInfoQt object. Object
     *          ownership is returned to caller.
     */
IMPORT_C static PrcPresenceBuddyInfoQt* createInstance( );    

    /**
     * Public destructor.
     * Objects can be deleted through this interface.
     */
      virtual ~PrcPresenceBuddyInfoQt() {}
public:

    /**
     * Sets buddy's identity
     *
     * @param buddyId buddy id 
     */
    virtual void setIdentity( const QString& buddyId ) = 0;        
    /**
     * Gets buddy's identity
     *
     * @return identity
     */
    virtual QString buddyId( ) const = 0;        

    /**
     * Sets buddy's availability
     *
     * @param aAvailability avalability value
     * @param aAvailabilityText avalability text, same as Availability field
     */
    virtual void setAvailability( 
        AvailabilityValues availability,
        const QString& availabilityText ) = 0;
        
    /**
     * Gets buddy's availability
     *
     * @return availability
     */
    virtual AvailabilityValues availability( ) const = 0;         
        
    /**
     * Gets buddy's availability text
     *
     * @return availability text
     */
    virtual QString availabilityText( ) const = 0;                   
        
    /**
     * Sets buddy's avatar
     *
     * @param aAvatar avatar, same as KAvatar field
     */
    virtual void setAvatar( const QString& avatar ) = 0;
    
    /**
     * Gets buddy's avatar
     *
     * @return avatar
     */
    virtual QString avatar( ) const = 0;
    /**
     * Sets buddy's status text
     *
     * @param statusMessage status text, same as StatusMessage field
     */
    virtual void setStatusMessage( 
        const QString& statusMessage ) = 0; 
    
    /**
     * Gets buddy's status text.
     *
     * @return status text, same as StatusMessage field
     */
    virtual QString statusMessage( ) const = 0;     
        
    /**
     * Sets any key value parameter for the buddy
     *
     * @param key field name
     * @param value field value
     */
    virtual void setAnyField( 
        const QString& key,
        const QString& value ) = 0;
      
          
    /**
     * Gets any key value parameter for the buddy
     *
     * @param key field name
     * @return field value, zero length if not found.     
     */
    virtual QString getAnyField( 
        const QString& key ) const = 0; 
        
    /**
     * Gets all the keys that are in use
     *
     * @param qtKeyList array where the keys are written to.   
     */
    virtual void getFieldKeys(QList<QString>& qtKeyList ) const = 0;
    
    /**
     * Removes the field
     * @param key field name     
     */
          
    virtual void removeField(const QString& key ) = 0;      
                                                    
    /**
     * Return true if identity is same with other instance.
     *
     * @param otherInstance other instance of this object.
     * @return ETrue if same.
     */
    
    virtual bool equalsIdentity(
        const PrcPresenceBuddyInfoQt& otherInstance ) const = 0;

    };

#endif // _M_PRESENCEBUDDYINFO_QT_H_



