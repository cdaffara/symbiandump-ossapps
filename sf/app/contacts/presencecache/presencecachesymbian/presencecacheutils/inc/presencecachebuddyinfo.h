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
* Description:  
*
*/


#ifndef CPRESENCECACHEBUDDYINFO_H
#define CPRESENCECACHEBUDDYINFO_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32hashtab.h>

#include <mpresencebuddyinfo2.h>

class RWriteStream;
class RReadStream;

// CLASS DECLARATION

/**
 *  CPresenceCacheBuddyInfo
 * 
 */
class CPresenceCacheBuddyInfo : public CBase, public MPresenceBuddyInfo2
    {
        
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CPresenceCacheBuddyInfo();


    /**
     * Two-phased constructor.
     */
    static CPresenceCacheBuddyInfo* NewL();
    
    /**
     * Two-phased constructor.
     */
    static CPresenceCacheBuddyInfo* NewLC();    
    
    /**
     * @return ETrue if buddy info is expired 
     */
    inline TBool IsExpired() const;
    
    /**
     * Reset buddy expiry time stamp
     */
    inline void ResetTimeStamp();
    
public: //from MPresenceBuddyInfo2
    
    /**
     * Defined in a base class
     */
    void SetIdentityL( const TDesC& aBuddyId );
        
    /**
     * Defined in a base class
     */
    TPtrC BuddyId( ) const;        

    /**
     * Defined in a base class
     */
    void SetAvailabilityL( 
        TAvailabilityValues aAvailability,
        const TDesC& aAvailabilityText );
        
    /**
     * Defined in a base class
     */
    TAvailabilityValues Availability( );         
        
    /**
     * Defined in a base class
     */
    TPtrC AvailabilityText( );           
        
        
    /**
     * Defined in a base class
     */
    void SetAvatarL( const TDesC8& aAvatar );
        
    /**
     * Defined in a base class
     */
    TPtrC8 Avatar( );             
        
    /**
     * Defined in a base class
     */
    void SetStatusMessageL( const TDesC& aStatusMessage ); 
    
    /**
     * Defined in a base class
     */
    TPtrC StatusMessage( );      
        
    /**
     * Defined in a base class
     */
    void SetAnyFieldL( 
        const TDesC16& aKey,
        const TDesC8& aValue );
        
    /**
     * Defined in a base class
     */
    TPtrC8 GetAnyField( 
        const TDesC16& aKey ); 
        
    /**
     * Defined in a base class
     */
    void GetFieldKeysL( 
            CDesCArrayFlat& aKeys );
    
    /**
     * Defined in a base class
     */    
    void RemoveField(const TDesC& aKey );     

    /**
     * Defined in a base class
     */
    TBool EqualsIdentity(
        const MPresenceBuddyInfo2& aOtherInstance ) const;   
    
    /**
     * Defined in a base class
     */
    void ExternalizeL( RWriteStream& aStream ) const;

    /**
     * Defined in a base class
     */
    void InternalizeL( RReadStream& aStream );       
        
    
    

private:

    /**
     * Constructor for performing 1st stage construction
     */
    CPresenceCacheBuddyInfo();

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();
    
    void DoSet16BitValueL( 
            const TDesC& aKey,    
            const TDesC& aValue );
    
    TPtrC DoGet16BitValue(const TDesC& aKey );
        
    void ExternalizeFieldL( const TDesC& aData, RWriteStream& aStream ) const;
    
    void ExternalizeFieldL( const TDesC8& aData, RWriteStream& aStream ) const;    
            
    HBufC* InternalizeFieldL( RReadStream& aStream );
    
    HBufC8* InternalizeField8L( RReadStream& aStream );  
    
    /**
     * Set key and value, takes ownership of parameters
     */
    void SetAnyFieldPtrL( 
        HBufC16* aKey,
        HBufC8* aValue );    
          
    
private: //data
    /**
     * Buddy id
     * OWN
     */
    MPresenceBuddyInfo2::TAvailabilityValues iAvailability;      
    HBufC* iBuddyId;    
    RPointerArray<HBufC> iIds;
    RPointerArray<HBufC8> iValues;     
    RHashMap<TDesC*, TInt> iHashMap;
    
    /**
     * Heap buffer for converted avavilablity text
     * OWN
     */  
    HBufC* iAvailabilityTextBuf;
    
    /**
     * Heap buffer for converted status text
     * OWN
     */ 
    HBufC* iStatusTextBuf;

    TInt64 iExpiryTime;
    TTime iExpiryTimeStamp;
    };

inline TBool CPresenceCacheBuddyInfo::IsExpired() const
    {
    if(iExpiryTime)
        {
        // expiry is in use
        TTime now;
        now.UniversalTime();
        if( now > iExpiryTimeStamp )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

inline void CPresenceCacheBuddyInfo::ResetTimeStamp()
    {
    TTime now;
    now.UniversalTime();
    iExpiryTimeStamp = now.Int64() +  iExpiryTime;
    }


#endif // CPRESENCECACHEBUDDYINFO_H
