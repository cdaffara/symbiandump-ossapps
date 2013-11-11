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


#include <bamdesca.h>
#include <s32strm.h>
#include "presencecachebuddyinfo.h"
#include <utf.h>

#define STRCNST(x)  (const_cast<TDesC*>( &x ))

// -----------------------------------------------------------------------------
CPresenceCacheBuddyInfo::CPresenceCacheBuddyInfo()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
CPresenceCacheBuddyInfo::~CPresenceCacheBuddyInfo()
    {      
    iHashMap.Close();  
    delete iAvailabilityTextBuf;
    delete iStatusTextBuf;
    delete iBuddyId;
    
    iIds.ResetAndDestroy();
    iIds.Close();
    iValues.ResetAndDestroy();
    iValues.Close();    
    }

// ---------------------------------------------------------------------------
// MPresenceBuddyInfo2::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C MPresenceBuddyInfo2* MPresenceBuddyInfo2::NewL( )
    {
    return CPresenceCacheBuddyInfo::NewL( );
    }
    
EXPORT_C MPresenceBuddyInfo2* MPresenceBuddyInfo2::NewLC( )
    {
    CPresenceCacheBuddyInfo* info = CPresenceCacheBuddyInfo::NewLC( );
    CleanupStack::Pop(info);
    MPresenceBuddyInfo2* minfo = info;  
    CleanupDeletePushL(minfo);
    return minfo; 
    }
    
// -----------------------------------------------------------------------------
CPresenceCacheBuddyInfo* CPresenceCacheBuddyInfo::NewL()
    {
    CPresenceCacheBuddyInfo* self = NewLC();
    CleanupStack::Pop(); // self;    
    return self;
    }
    
// -----------------------------------------------------------------------------
CPresenceCacheBuddyInfo* CPresenceCacheBuddyInfo::NewLC()
    {
    CPresenceCacheBuddyInfo* self = new (ELeave)CPresenceCacheBuddyInfo();
    CleanupStack::PushL(self);
    self->ConstructL();  
    return self;
    }    
    
// -----------------------------------------------------------------------------
static TUint32 StringPtrHash( TDesC* const& aValue )
    {
    return DefaultHash::Des16( *aValue );
    }

// -----------------------------------------------------------------------------
static TBool StringIdentity( TDesC* const& aA, TDesC* const& aB )
    {
    return !aA->CompareF( *aB );
    }

// -----------------------------------------------------------------------------
void CPresenceCacheBuddyInfo::ConstructL()
    {
    iHashMap.Close();
    iHashMap = RHashMap<TDesC*, TInt>(    
        THashFunction32<TDesC*>( StringPtrHash ),
        TIdentityRelation<TDesC*>( StringIdentity ) );
    }

// -----------------------------------------------------------------------------
void CPresenceCacheBuddyInfo::SetIdentityL( 
    const TDesC& aBuddyId )
    {
    delete iBuddyId;
    iBuddyId = NULL;
    iBuddyId = aBuddyId.AllocL( );        
    }
    
// -----------------------------------------------------------------------------
TPtrC CPresenceCacheBuddyInfo::BuddyId( ) const
    {
    return iBuddyId ? iBuddyId->Des() : TPtrC();
    }

// -----------------------------------------------------------------------------
void CPresenceCacheBuddyInfo::SetAvailabilityL( 
    TAvailabilityValues aAvailability,
    const TDesC& aAvailabilityText )
    {  
    iAvailability = aAvailability;    
    DoSet16BitValueL( NPresenceCacheFieldName::KAvailability, aAvailabilityText );
    }

// -----------------------------------------------------------------------------
MPresenceBuddyInfo2::TAvailabilityValues CPresenceCacheBuddyInfo::Availability( )
    {
    return iAvailability;    
    }
    
// -----------------------------------------------------------------------------
TPtrC CPresenceCacheBuddyInfo::AvailabilityText( )
    {
    return DoGet16BitValue( NPresenceCacheFieldName::KAvailability );
    }

// -----------------------------------------------------------------------------
void CPresenceCacheBuddyInfo::DoSet16BitValueL( 
    const TDesC& aKey,    
    const TDesC& aValue )
    {   
      
    // Remove old values first
    RemoveField( aKey );
    
    //Convert Unicode to Utf-8
    HBufC8* convertBuffer =  CnvUtfConverter::ConvertFromUnicodeToUtf8L( aValue );
    CleanupStack::PushL( convertBuffer );
    HBufC* keyBuffer = aKey.AllocLC();    
    
    TInt insertPos = iIds.Find(0);
    if ( insertPos < 0 )
        {
        insertPos = iIds.Count();
        iIds.Append( keyBuffer );
        iValues.Append( convertBuffer );
        }
    else
        {
        iIds[insertPos] = keyBuffer;
        iValues[insertPos] = convertBuffer;
        }
    
    iHashMap.InsertL( keyBuffer, insertPos  );
        
    CleanupStack::Pop( keyBuffer );
    CleanupStack::Pop( convertBuffer );       
    }

// -----------------------------------------------------------------------------
void CPresenceCacheBuddyInfo::RemoveField(const TDesC& aKey )
    {
    TInt* pos = iHashMap.Find( STRCNST( aKey ) );    
    if ( pos )
        {
        TInt mypos = *pos;
        iHashMap.Remove( iIds[mypos ]);
        
        delete iIds[mypos];
        iIds[mypos] = NULL;
        
        delete iValues[mypos];
        iValues[mypos] = NULL;               
        }    
    }

// -----------------------------------------------------------------------------
TPtrC CPresenceCacheBuddyInfo::DoGet16BitValue(const TDesC& aKey )
    {
    HBufC8* val = NULL;        
    TInt* pos = iHashMap.Find( STRCNST( aKey ) );
    TPtrC result( KNullDesC() );
    
    if ( pos )
        {       
        val = iValues[*pos];
        if ( val )
            {
            HBufC* temp = NULL;
            // Convert  Utf-8 to Unicode
            TRAPD( errx,  temp = CnvUtfConverter::ConvertToUnicodeFromUtf8L( *val ) );
            if ( !errx && temp )
                { 
                // Avalibility text handled here
                if ( !aKey.CompareF( NPresenceCacheFieldName::KAvailability ) )
                	{
                    delete iAvailabilityTextBuf;
                    iAvailabilityTextBuf = NULL;                 	
                    iAvailabilityTextBuf = temp; 
                    result.Set( iAvailabilityTextBuf->Des() );
                	}
                // Status message handled here
                else if( !aKey.CompareF( NPresenceCacheFieldName::KStatusMessage ) )
                	{
                    delete iStatusTextBuf;
                    iStatusTextBuf = NULL;                 	
                    iStatusTextBuf = temp;  
                    result.Set( iStatusTextBuf->Des() );
                	}
                }
            }
        }
    return result;
    }
    
// -----------------------------------------------------------------------------
void CPresenceCacheBuddyInfo::SetAvatarL( 
    const TDesC8& aAvatar )
    {
    SetAnyFieldL( NPresenceCacheFieldName::KAvatar, aAvatar);
    }
    
// -----------------------------------------------------------------------------
TPtrC8 CPresenceCacheBuddyInfo::Avatar( )
    {
    return GetAnyField( NPresenceCacheFieldName::KAvatar ); 
    }
    
// -----------------------------------------------------------------------------
void CPresenceCacheBuddyInfo::SetStatusMessageL( 
    const TDesC& aStatusMessage )
    {
    DoSet16BitValueL( NPresenceCacheFieldName::KStatusMessage, aStatusMessage );
    }


// -----------------------------------------------------------------------------
TPtrC CPresenceCacheBuddyInfo::StatusMessage(  )
    {
    return DoGet16BitValue( NPresenceCacheFieldName::KStatusMessage );
    }
    
// -----------------------------------------------------------------------------
void CPresenceCacheBuddyInfo::SetAnyFieldL( 
    const TDesC16& aKey,
    const TDesC8& aValue )
    {
    if( NPresenceCacheFieldName::KExpiry().CompareF( aKey ) == 0 )
        {
        TPckg<TInt64> expiryPck( iExpiryTime );
        expiryPck.Copy( aValue );
        // write also to key-value map
        }
    
    // Remove old values first
    RemoveField( aKey );    
    
    HBufC8* valueBuffer = aValue.AllocLC();      
    HBufC* keyBuffer = aKey.AllocLC();  
    
    TInt insertPos = iIds.Find(0);
    if ( insertPos < 0 )
        {
        insertPos = iIds.Count();
        iIds.Append( keyBuffer );
        iValues.Append( valueBuffer );
        }
    else
        {
        iIds[insertPos] = keyBuffer;
        iValues[insertPos] = valueBuffer;
        }
    
    iHashMap.InsertL( keyBuffer, insertPos  ); 
    
    CleanupStack::Pop( keyBuffer );
    CleanupStack::Pop( valueBuffer );    
    }

// -----------------------------------------------------------------------------
void CPresenceCacheBuddyInfo::SetAnyFieldPtrL( 
    HBufC16* aKey,
    HBufC8* aValue )
    {  
    if ( !aKey )
        {
        User::Leave( KErrArgument );
        }
    // Remove old values first
    RemoveField( aKey->Des() );
         
    TInt insertPos = iIds.Find(0);
    if ( insertPos < 0 )
        {
        insertPos = iIds.Count();
        iIds.Append( aKey );
        iValues.Append( aValue );
        }
    else
        {
        iIds[insertPos] = aKey;
        iValues[insertPos] = aValue;
        }
    iHashMap.InsertL( aKey, insertPos  );       
    }
    
// -----------------------------------------------------------------------------
TPtrC8 CPresenceCacheBuddyInfo::GetAnyField( 
    const TDesC16& aKey )
    {
    HBufC8* val = NULL;    
    TInt* pos = iHashMap.Find( STRCNST( aKey ) );
    if ( pos )
        {       
        val = iValues[*pos];        
        } 
    return val ? val->Des() : TPtrC8();    
    }
    
// -----------------------------------------------------------------------------
void CPresenceCacheBuddyInfo::GetFieldKeysL( 
    CDesCArrayFlat& aKeys )
    {
    aKeys.Reset();
    THashMapIter<TDesC*, TInt> iter(iHashMap);    
    TDesC* const* nextKey = iter.NextKey();
    while (nextKey)
        {
        aKeys.AppendL( **nextKey );
        nextKey = iter.NextKey();
        }    
    }
                                                                          
// -----------------------------------------------------------------------------
TBool CPresenceCacheBuddyInfo::EqualsIdentity(
    const MPresenceBuddyInfo2& aOtherInstance ) const
    {
    TBool same = ETrue;

    TBool x;
    x = 0 == BuddyId().CompareF( aOtherInstance.BuddyId() );
    same &= x;

    return same;
    }  

// -----------------------------------------------------------------------------
void CPresenceCacheBuddyInfo::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iAvailability ); 
    ExternalizeFieldL( BuddyId(), aStream );
           
    // externalize expiry time
    TUint32 time_high = I64HIGH( iExpiryTime );
    TUint32 time_low = I64LOW( iExpiryTime );
    aStream.WriteUint32L(time_high);
    aStream.WriteUint32L(time_low);
    
    TInt count = iIds.Count();
    aStream.WriteInt32L( count );     
    for ( TInt i=0; i < count; i++ )
        {
        if ( iIds[i] )
            {
            TPtrC key = iIds[i]->Des();
            TPtrC8 value = iValues[i] ? iValues[i]->Des() : TPtrC8();        
            ExternalizeFieldL( key, aStream );        
            ExternalizeFieldL( value, aStream );             
            }
        else
            {
            // nothing to insert, special case if replacement has been failed earlier
            }
        }
    } 

// -----------------------------------------------------------------------------
void CPresenceCacheBuddyInfo::InternalizeL( RReadStream& aStream )
    {
    iAvailability = (MPresenceBuddyInfo2::TAvailabilityValues)aStream.ReadInt32L( ); 
    
    HBufC* temp  = InternalizeFieldL( aStream );
    iBuddyId = temp;
       
    iIds.ResetAndDestroy();
    iValues.ResetAndDestroy();
    
    HBufC* key = NULL;
    HBufC8* value = NULL;

    // internalize expiry time
    TUint32 time_high = aStream.ReadUint32L();
    TUint32 time_low = aStream.ReadUint32L();
    iExpiryTime = MAKE_TINT64( time_high, time_low );
    
    TInt count = aStream.ReadInt32L( );    
    for ( TInt i=0; i < count; i++ )
        {
        key = InternalizeFieldL( aStream ); 
        
        CleanupStack::PushL( key );
        value = InternalizeField8L( aStream );
        CleanupStack::Pop( key );          
        // This takes ownership anyway.
        SetAnyFieldPtrL( key, value );                       
        }      
    } 

// -----------------------------------------------------------------------------
void CPresenceCacheBuddyInfo::ExternalizeFieldL( const TDesC& aData, RWriteStream& aStream ) const
    {
    TInt32 length = aData.Length();    
    aStream.WriteInt32L( length  );
    aStream.WriteL( aData, length );
    }

// -----------------------------------------------------------------------------
void CPresenceCacheBuddyInfo::ExternalizeFieldL( const TDesC8& aData, RWriteStream& aStream ) const
    {
    TInt32 length = aData.Length();    
    aStream.WriteInt32L( length  );
    aStream.WriteL( aData, length );
    }

// -----------------------------------------------------------------------------
HBufC* CPresenceCacheBuddyInfo::InternalizeFieldL( RReadStream& aStream )
    {
    HBufC16* temp = NULL;    
    TInt32 length = aStream.ReadInt32L();
    if ( length > 0 )
        { 
        temp = HBufC::NewLC( length);
        TPtr stringPtr = temp->Des(); 
        aStream.ReadL( stringPtr, length );
        CleanupStack::Pop( temp );
        } 
    else
        {
        }
    return temp;
    }

// -----------------------------------------------------------------------------
HBufC8* CPresenceCacheBuddyInfo::InternalizeField8L( RReadStream& aStream )
    {
    HBufC8* temp = NULL;    
    TInt32 length = aStream.ReadInt32L();
    if ( length > 0 )
        { 
        temp = HBufC8::NewLC( length);
        TPtr8 stringPtr = temp->Des(); 
        aStream.ReadL( stringPtr, length );
        CleanupStack::Pop( temp );
        } 
    else
        {
        }
    return temp;
    }


