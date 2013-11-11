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
* Description:  Interface for presence buddy presence info.
*
*/

#ifndef MPRESENCEBUDDYINFO2_H
#define MPRESENCEBUDDYINFO2_H

#include <e32std.h>
#include <badesca.h>

#include <s32strm.h>

class RWriteStream;
class RReadStream;

    /**
     * Common field names. 
     * If you make your own extension it is recommended to use "x-" prefix in your fields, 
     * e.g. "x-myapp-location".
     */ 
     namespace NPresenceCacheFieldName
        {

        /**
         * Field type for "status message" field.
         *
         * Status message field contains user written
         * free text, describing for example his/hers current
         * presence status. Status message is in unicode format.
         */
        _LIT( KStatusMessage, "statustext");

        /**
         * Field type for "avatar" field.
         *
         * Avatar field contains user selected image. Image may be
         * an avatar describing the user or it may be image showing
         * user current presence status.
         *
         * Image is in JPEG or similar format, for what there exists
         * image codes in the platform. Field contains image binary
         * content and MIME string describing the content format.
         */
        _LIT( KAvatar,             "avatar");


        /**
         * Field type for "availability-text" field.
         *
         * Availability text field describes presentity's communication
         * availability as text. This is useful in case of brande
         * presence availability, since any text value is possible.
         *
         * Field storage format: MPresenceInfoFieldValueText
         */
        _LIT( KAvailability,       "availabilitytext");
        
        /**
         * Field type for "serviceiconbrandid-text" field.
         *
         * Service icon brand ID text field identifies the service brand which
         * the current presence information is from. Brand information can
         * be used with BrandingServer to fetch branded content to UI.  
         *
         * Field storage format: 8bit descriptor
         */
        _LIT( KServiceIconBrand,   "ServiceBrandingID" );
        
        /**
         * Field type for "serviceiconentryid-text" field.
         *
         * Service icon entry ID text field identifies the icon which represents
         * the current presence service. Icon id can be used with branding
         * information to fetch the actual image from BrandingServer.  
         *
         * Field storage format: 8bit descriptor
         */
        _LIT( KServiceIconId,      "ServiceIconEntryID" );
        
        /**
         * Field type for "timestamp" field.
         *
         * TimeStamp field represents the time when the presence information
         * was last updated. Client can show the TimeStamp on the UI or omit
         * old presence information completely.
         *
         * Field storage format: TPckg<TTime>
         * 
         * Write example:
         *     TTime time;
         *     time.HomeTime();
         *     TPckg<TTime> timePck( time );
         *     aBuddyInfo.SetAnyFieldL( KTimeStamp, timePck );
         *
         * Read example:
         *     TTime time;
         *     TPckg<TTime> timePck( time );
         *     timePck.Copy( aBuddyInfo.GetAnyField( KTimeStamp ) );
         */
        _LIT( KTimeStamp,          "TimeStamp" );        

        /**
         * Field type for "expiry" field.
         *
         * Expiry field represents the time (microseconds) how long buddy 
         * information is valid with out new buddy info write events. 
         * See @MPresenceCacheWriter2
         * If Buddy information is not updated in expiry time then buddy 
         * information is removed automatilly from presence cache.
         *
         * Field storage format: TPckg<TInt64>
         * 
         * Write example:
         *     TInt64 expiry([EXPIRY_TIME]);
         *     TPckg<TInt64> expiryPck( expiry );
         *     aBuddyInfo.SetAnyFieldL( 
         *                      NPresenceCacheFieldName::KExpiry, expiryPck );
         *
         * Read example:
         *     TInt64 expiry;
         *     TPckg<TInt64> expiryPck( expiry );
         *     expiryPck.Copy( aBuddyInfo.GetAnyField( 
         *                      NPresenceCacheFieldName::KExpiry ) );
         */
        _LIT( KExpiry,          "expiry" );

        }

/**
 * Interface buddy presence info. 
 *
 * @since S60 v5.1
 */
 
        
class MPresenceBuddyInfo2 
{
public:

    /**
     * Common enumerated availability values for "availability"
     * presence information field.
     *
     * @since S60 v5.1
     */
    enum TAvailabilityValues
        {            
        /**
         * Presentity status is unknown.
         */
        EUnknownAvailability = 0,                

        /**
         * Presentity is not available for communication.
         */
        ENotAvailable = 1,

        /**
         * Presentity is currently busy.
         * Presentity may or may not be available
         * for for communication.
         */
        EBusy = 2,

        /**
         * Presentity is available for communication.
         */
        EAvailable = 3
        };


    /**
     * Factory method to instantiate MPresenceBuddyInfo2
     *
     * @return The new MPresenceFeeder object. Object
     *         ownership is returned to caller.
     */
    IMPORT_C static MPresenceBuddyInfo2* NewL( );
    
    /**
     * Factory method to instantiate MPresenceBuddyInfo2
     *
     * @return The new MPresenceFeeder object. Object
     *         ownership is returned to caller.
     */
    IMPORT_C static MPresenceBuddyInfo2* NewLC( );    
    


    /**
     * Public destructor.
     * Objects can be deleted through this interface.
     */
    virtual ~MPresenceBuddyInfo2() {}
    
public:

    /**
     * Sets buddy's identity
     *
     * @param aBuddyId buddy id 
     */
    virtual void SetIdentityL( const TDesC& aBuddyId ) = 0;
        
    /**
     * Gets buddy's identity
     *
     * @return identity
     */
    virtual TPtrC BuddyId( ) const = 0;        

    /**
     * Sets buddy's availability
     *
     * @param aAvailability avalability value
     * @param aAvailabilityText avalability text, same as KAvailability field
     */
    virtual void SetAvailabilityL( 
        TAvailabilityValues aAvailability,
        const TDesC& aAvailabilityText ) = 0;
        
    /**
     * Gets buddy's availability
     *
     * @return availability
     */
    virtual TAvailabilityValues Availability( ) = 0;         
        
    /**
     * Gets buddy's availability text
     *
     * @return availability text
     */
    virtual TPtrC AvailabilityText( ) = 0;           
        
        
    /**
     * Sets buddy's avatar
     *
     * @param aAvatar avatar, same as KAvatar field
     */
    virtual void SetAvatarL( const TDesC8& aAvatar ) = 0;
        
    /**
     * Gets buddy's avatar
     *
     * @return avatar
     */
    virtual TPtrC8 Avatar( ) = 0;             
        
    /**
     * Sets buddy's status text
     *
     * @param aStatusMessage status text, same as KStatusMessage field
     */
    virtual void SetStatusMessageL( 
        const TDesC& aStatusMessage ) = 0; 
    
    /**
     * Gets buddy's status text.
     *
     * @return status text, same as KStatusMessage field
     */
    virtual TPtrC StatusMessage( ) = 0;     
        
    /**
     * Sets any key value parameter for the buddy
     *
     * @param aKey field name
     * @param aValue field value
     */
    virtual void SetAnyFieldL( 
        const TDesC16& aKey,
        const TDesC8& aValue ) = 0;
        
    /**
     * Gets any key value parameter for the buddy
     *
     * @param aKey field name
     * @return field value, zero length if not found.     
     */
    virtual TPtrC8 GetAnyField( 
        const TDesC16& aKey ) = 0; 
        
    /**
     * Gets all the keys that are in use
     *
     * @param aKeys array where the keys are written to.   
     */
    virtual void GetFieldKeysL( 
            CDesCArrayFlat& aKeys ) = 0;
    
    /**
     * Removes the field
     * @param aKey field name     
     */
    virtual void RemoveField(const TDesC& aKey ) = 0;      
                                
                                                    
    /**
     * Return true if identity is same with other instance.
     *
     * @param aOtherInstance other instance of this object.
     * @return ETrue if same.
     */
    virtual TBool EqualsIdentity(
        const MPresenceBuddyInfo2& aOtherInstance ) const = 0;
    
    /**
     * Externalize
     * @param aStream write stream
     */
    virtual void ExternalizeL( RWriteStream& aStream ) const = 0;


    /**
     * Internalize
     * @param aStream read stream
     */
    virtual void InternalizeL( RReadStream& aStream ) = 0;     
    
    };

#endif // MPRESENCEBUDDYINFO2_H



