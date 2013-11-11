/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message store property container client implementation.
*
*/



#include "MsgStorePropertyContainer.h"
//<cmail>
#include "DebugLogMacros.h"
#include "MsgStorePropertyKeys.h"
//</cmail>
#include "PropertiesSerializer.h"

// ===========
// LOCAL TYPES
// ===========

// --------------------------
// CMsgStorePropertyContainer
// --------------------------

// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
EXPORT_C CMsgStorePropertyContainer* CMsgStorePropertyContainer::NewL()
	{
    CMsgStorePropertyContainer* self = new( ELeave ) CMsgStorePropertyContainer();
    return self;
	} // end NewL

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CMsgStorePropertyContainer::CMsgStorePropertyContainer()
	{
	iId       = KMsgStoreInvalidId;
	iParentId = KMsgStoreInvalidId;
	} // end constructor
	
// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CMsgStorePropertyContainer::~CMsgStorePropertyContainer()
	{
	iProperties.ResetAndDestroy();
	} // end destructor
	
// ==========================================================================
// FUNCTION: Id
// ==========================================================================
EXPORT_C TMsgStoreId CMsgStorePropertyContainer::Id() const
	{
	return iId;
	} // end Id

// ==========================================================================
// FUNCTION: ParentId
// ==========================================================================
EXPORT_C TMsgStoreId CMsgStorePropertyContainer::ParentId() const
	{
	return iParentId;
	} // end ParentId

// ==========================================================================
// FUNCTION: PropertyCount
// ==========================================================================
EXPORT_C TUint CMsgStorePropertyContainer::PropertyCount() const
	{
	return iProperties.Count();
	} // end PropertyCount
	
// ==========================================================================
// FUNCTION: PropertyNameL
// ==========================================================================
EXPORT_C const TDesC8& CMsgStorePropertyContainer::PropertyNameL( TUint aPropertyIndex ) const
	{
	ValidateIndexL( aPropertyIndex );

	return iProperties[aPropertyIndex]->iName;
	} // end PropertyNameL

// ==========================================================================
// FUNCTION: PropertyTypeL
// ==========================================================================
EXPORT_C TMsgStorePropertyValueType CMsgStorePropertyContainer::PropertyTypeL( TUint aPropertyIndex ) const
	{
	ValidateIndexL( aPropertyIndex );
	
	return iProperties[aPropertyIndex]->iType;
	} // end PropertyTypeL

// ==========================================================================
// FUNCTION: DoAddPropertyL
// ==========================================================================
TUint CMsgStorePropertyContainer::DoAddPropertyL( TMsgStorePropertyValueType aType, const TDesC8& aName, const TDesC8& aValue )	
	{
	ValidateLengthsL( aName.Length(), aValue.Length() );
	
	TMsgStoreProperty* newNode = new(ELeave) TMsgStoreProperty;
	CleanupStack::PushL( newNode );
	
	newNode->iName.CreateL( aName );
    newNode->iValue.CreateL( aValue );	
	newNode->iType = aType;
	
	iProperties.AppendL( newNode );
	
	CleanupStack::Pop( newNode );
	
	return iProperties.Count() - 1;
	} // end DoAddPropertyL
	
// ==========================================================================
// FUNCTION: AddPropertyL
// ==========================================================================
EXPORT_C TUint CMsgStorePropertyContainer::AddPropertyL( const TDesC8& aName, TBool aValue )	
    {
	TPckg<TBool> valuePckg( aValue );

    return DoAddPropertyL( EMsgStoreTypeBool, aName, valuePckg );	
	} // end AddPropertyL

// ==========================================================================
// FUNCTION: AddPropertyL
// ==========================================================================
EXPORT_C TUint CMsgStorePropertyContainer::AddPropertyL( const TDesC8& aName, TUint32 aValue )
	{
	TPckg<TUint> valuePckg( aValue );

    return DoAddPropertyL( EMsgStoreTypeUint32, aName, valuePckg );	
	} // end AddPropertyL

// ==========================================================================
// FUNCTION: AddPropertyL
// ==========================================================================
EXPORT_C TUint CMsgStorePropertyContainer::AddPropertyL( const TDesC8& aName, const TTime& aValue )
	{
	TPckg<TInt64> valuePckg( aValue.Int64() );

    return DoAddPropertyL( EMsgStoreTypeTime, aName, valuePckg );	
	} // end AddPropertyL

// ==========================================================================
// FUNCTION: AddPropertyL
// ==========================================================================
EXPORT_C TUint CMsgStorePropertyContainer::AddPropertyL( const TDesC8& aName, const RMsgStoreAddress& aValue )
	{
	CMsgStorePropertyContainer* props = CMsgStorePropertyContainer::NewL();
	CleanupStack::PushL( props );
	
	props->AddPropertyL( KMsgStorePropertyEmailAddress, aValue.iEmailAddress );
	props->AddPropertyL( KMsgStorePropertyDisplayName, aValue.iDisplayName );
	
    TUint index = AddPropertyL( aName, *props, EMsgStoreTypeAddress );	
    
    CleanupStack::PopAndDestroy( props );
    return index;
	} // end AddPropertyL

// ==========================================================================
// FUNCTION: AddPropertyL
// ==========================================================================
EXPORT_C TUint CMsgStorePropertyContainer::AddPropertyL( const TDesC8& aName, const TDesC8& aValue )
	{
    return DoAddPropertyL( EMsgStoreTypeDes8, aName, aValue );	
	} // end AddPropertyL
	
// ==========================================================================
// FUNCTION: AddPropertyL
// ==========================================================================
EXPORT_C TUint CMsgStorePropertyContainer::AddPropertyL( const TDesC8& aName, const TDesC& aValue )
	{
	const TUint8* valuePtr     = reinterpret_cast<const TUint8*>( aValue.Ptr() );
	TUint         valueLength = aValue.Length() * 2;
	
    TPtrC8 valueDes8( valuePtr, valueLength );	
	
    TUint index = DoAddPropertyL( EMsgStoreTypeDes, aName, valueDes8 );	
    
	const TUint16* valuePtr16 = reinterpret_cast<const TUint16*>( iProperties[index]->iValue.Ptr() );
	
	iProperties[index]->iValue16.Set( valuePtr16, aValue.Length() );
	
    return index;
	} // end AddPropertyL

// ==========================================================================
// FUNCTION: AddPropertyL
// ==========================================================================
EXPORT_C TUint CMsgStorePropertyContainer::AddPropertyL( const TDesC8& aName, const CMsgStorePropertyContainer& aContainer )
	{
	return AddPropertyL( aName, aContainer, EMsgStoreTypeContainer );
	} // end AddPropertyL


// ==========================================================================
// FUNCTION: DoUpdatePropertyL
// ==========================================================================
void CMsgStorePropertyContainer::DoUpdatePropertyL( TUint aPropertyIndex, TMsgStorePropertyValueType aType, const TDesC8& aValue )
	{
	// An update cannot change the type of the property.
	ValidateIndexAndTypeL( aPropertyIndex, aType );
	ValidateValueLengthL( aValue.Length() );	

    iProperties[aPropertyIndex]->iValue.Close();
    
    iProperties[aPropertyIndex]->iValue.CreateL( aValue );
    iProperties[aPropertyIndex]->iType = aType;
	} // end DoUpdatePropertyL

// ==========================================================================
// FUNCTION: UpdatePropertyL
// ==========================================================================
EXPORT_C void CMsgStorePropertyContainer::UpdatePropertyL( TUint aPropertyIndex, const TDesC8& aValue )
	{
	return DoUpdatePropertyL( aPropertyIndex, EMsgStoreTypeDes8, aValue );
	}  // end UpdatePropertyL

// ==========================================================================
// FUNCTION: UpdatePropertyL
// ==========================================================================
EXPORT_C void CMsgStorePropertyContainer::UpdatePropertyL( TUint aPropertyIndex, const TDesC& aValue )
	{
	const TUint8* valuePtr     = reinterpret_cast<const TUint8*>( aValue.Ptr() );
	TUint         valueLength = aValue.Length() * 2;
	
    TPtrC8 valueDes8( valuePtr, valueLength );	
	
	DoUpdatePropertyL( aPropertyIndex, EMsgStoreTypeDes, valueDes8 );
	
	const TUint16* valuePtr16 = reinterpret_cast<const TUint16*>( iProperties[aPropertyIndex]->iValue.Ptr() );

	iProperties[aPropertyIndex]->iValue16.Set( valuePtr16, aValue.Length() );
	} // end 

// ==========================================================================
// FUNCTION: UpdatePropertyL
// ==========================================================================
EXPORT_C void CMsgStorePropertyContainer::UpdatePropertyL( TUint aPropertyIndex, TUint32 aValue )
	{
	TPckg<TUint> valuePckg( aValue );

	DoUpdatePropertyL( aPropertyIndex, EMsgStoreTypeUint32, valuePckg );
	} // end 

// ==========================================================================
// FUNCTION: UpdatePropertyL
// ==========================================================================
EXPORT_C void CMsgStorePropertyContainer::UpdatePropertyL( TUint aPropertyIndex, const TTime& aValue )
	{
	TPckg<TInt64> valuePckg( aValue.Int64() );

	DoUpdatePropertyL( aPropertyIndex, EMsgStoreTypeTime, valuePckg );
	} // end 

// ==========================================================================
// FUNCTION: UpdatePropertyL
// ==========================================================================
EXPORT_C void CMsgStorePropertyContainer::UpdatePropertyL( TUint aPropertyIndex, const RMsgStoreAddress& aValue )
	{
	CMsgStorePropertyContainer* props = CMsgStorePropertyContainer::NewL();
	CleanupStack::PushL( props );
	
	props->AddPropertyL( KMsgStorePropertyEmailAddress, aValue.iEmailAddress );
	props->AddPropertyL( KMsgStorePropertyDisplayName, aValue.iDisplayName );
	
    UpdatePropertyL( aPropertyIndex, *props, EMsgStoreTypeAddress );	
    
    CleanupStack::PopAndDestroy( props );
	} // end 

// ==========================================================================
// FUNCTION: UpdatePropertyL
// ==========================================================================
EXPORT_C void CMsgStorePropertyContainer::UpdatePropertyL( TUint aPropertyIndex, TBool aValue )
	{
	TPckg<TBool> valuePckg( aValue );

	DoUpdatePropertyL( aPropertyIndex, EMsgStoreTypeBool, valuePckg );
	} // end UpdatePropertyL

// ==========================================================================
// FUNCTION: UpdatePropertyL
// ==========================================================================
EXPORT_C void CMsgStorePropertyContainer::UpdatePropertyL( TUint aPropertyIndex, const CMsgStorePropertyContainer& aContainer )
	{
	UpdatePropertyL( aPropertyIndex, aContainer, EMsgStoreTypeContainer );
	} // end UpdatePropertyL

// ==========================================================================
// FUNCTION: AddOrUpdatePropertyL
// ==========================================================================
EXPORT_C TUint CMsgStorePropertyContainer::AddOrUpdatePropertyL( const TDesC8& aName, TBool aValue )
	{
	TUint index;
	if( FindProperty( aName, index ) )
		{
		UpdatePropertyL( index, aValue );
		}
	else
		{
		index = AddPropertyL( aName, aValue );
		} // end if
		
	return index;
	} // end AddOrUpdatePropertyL

// ==========================================================================
// FUNCTION: AddOrUpdatePropertyL
// ==========================================================================
EXPORT_C TUint CMsgStorePropertyContainer::AddOrUpdatePropertyL( const TDesC8& aName, TUint32 aValue )
	{
	TUint index;
	if( FindProperty( aName, index ) )
		{
		UpdatePropertyL( index, aValue );
		}
	else
		{
		index = AddPropertyL( aName, aValue );
		} // end if
		
	return index;
	} // end AddOrUpdatePropertyL

// ==========================================================================
// FUNCTION: AddOrUpdatePropertyL
// ==========================================================================
EXPORT_C TUint CMsgStorePropertyContainer::AddOrUpdatePropertyL( const TDesC8& aName, const TTime& aValue )
	{
	TUint index;
	if( FindProperty( aName, index ) )
		{
		UpdatePropertyL( index, aValue );
		}
	else
		{
		index = AddPropertyL( aName, aValue );
		} // end if
		
	return index;
	} // end AddOrUpdatePropertyL

// ==========================================================================
// FUNCTION: AddOrUpdatePropertyL
// ==========================================================================
EXPORT_C TUint CMsgStorePropertyContainer::AddOrUpdatePropertyL( const TDesC8& aName, const RMsgStoreAddress& aValue )
	{
	TUint index;
	if( FindProperty( aName, index ) )
		{
		UpdatePropertyL( index, aValue );
		}
	else
		{
		index = AddPropertyL( aName, aValue );
		} // end if
		
	return index;
	} // end AddOrUpdatePropertyL

// ==========================================================================
// FUNCTION: AddOrUpdatePropertyL
// ==========================================================================
EXPORT_C TUint CMsgStorePropertyContainer::AddOrUpdatePropertyL( const TDesC8& aName, const TDesC8& aValue )
	{
	TUint index;
	if( FindProperty( aName, index ) )
		{
		UpdatePropertyL( index, aValue );
		}
	else
		{
		index = AddPropertyL( aName, aValue );
		} // end if
		
	return index;
	} // end AddOrUpdatePropertyL

// ==========================================================================
// FUNCTION: AddOrUpdatePropertyL
// ==========================================================================
EXPORT_C TUint CMsgStorePropertyContainer::AddOrUpdatePropertyL( const TDesC8& aName, const TDesC& aValue )
	{
	TUint index;
	if( FindProperty( aName, index ) )
		{
		UpdatePropertyL( index, aValue );
		}
	else
		{
		index = AddPropertyL( aName, aValue );
		} // end if
		
	return index;
	} // end AddOrUpdatePropertyL

// ==========================================================================
// FUNCTION: AddOrUpdatePropertyL
// ==========================================================================
EXPORT_C TUint CMsgStorePropertyContainer::AddOrUpdatePropertyL( const TDesC8& aName, const CMsgStorePropertyContainer& aContainer )
	{
	TUint index;
	if( FindProperty( aName, index ) )
		{
		UpdatePropertyL( index, aContainer );
		}
	else
		{
		index = AddPropertyL( aName, aContainer );
		} // end if
		
	return index;
	} // end AddOrUpdatePropertyL
	
// ==========================================================================
// FUNCTION: PropertyValueDes8L
// ==========================================================================
EXPORT_C const TDesC8& CMsgStorePropertyContainer::PropertyValueDes8L( TUint aPropertyIndex ) const
	{
	ValidateIndexAndTypeL( aPropertyIndex, EMsgStoreTypeDes8 );

	return iProperties[aPropertyIndex]->iValue;
	} // end PropertyValueDes8L

// ==========================================================================
// FUNCTION: PropertyValueDesL
// ==========================================================================
EXPORT_C const TDesC& CMsgStorePropertyContainer::PropertyValueDesL( TUint aPropertyIndex ) const
	{
	ValidateIndexAndTypeL( aPropertyIndex, EMsgStoreTypeDes );
	
	return iProperties[aPropertyIndex]->iValue16;
	} // end PropertyValueDesL

// ==========================================================================
// FUNCTION: PropertyValueUint32L
// ==========================================================================
EXPORT_C TUint32 CMsgStorePropertyContainer::PropertyValueUint32L( TUint aPropertyIndex ) const
	{
	ValidateIndexAndTypeL( aPropertyIndex, EMsgStoreTypeUint32 );

	TPckgBuf<TUint> intValue;

	TPtrC8 des8( iProperties[aPropertyIndex]->iValue.Ptr(), iProperties[aPropertyIndex]->iValue.Length() );
	
	intValue.Copy( des8 );
	
	return intValue();
	} // end PropertyValueUint32L

// ==========================================================================
// FUNCTION: PropertyValueTimeL
// ==========================================================================
EXPORT_C void CMsgStorePropertyContainer::PropertyValueTimeL( TUint aPropertyIndex, TTime& aTime ) const
	{
	ValidateIndexAndTypeL( aPropertyIndex, EMsgStoreTypeTime );

	TPckgBuf<TInt64> int64Value;

	TPtrC8 des8( iProperties[aPropertyIndex]->iValue.Ptr(), iProperties[aPropertyIndex]->iValue.Length() );
	
	int64Value.Copy( des8 );
	
	aTime = int64Value();
	} // end PropertyValueTimeL

// ==========================================================================
// FUNCTION: PropertyValueAddressL
// ==========================================================================
EXPORT_C void CMsgStorePropertyContainer::PropertyValueAddressL( TUint aPropertyIndex, RMsgStoreAddress& aAddress ) const
	{
	CMsgStorePropertyContainer* address = PropertyValueContainerL( aPropertyIndex, EMsgStoreTypeAddress );
	CleanupStack::PushL( address );
	
	TUint index = 0;
	if ( address->FindProperty(KMsgStorePropertyEmailAddress, index) )
		{
		aAddress.iEmailAddress.Create( address->PropertyValueDesL(index) );
		}
	
	if ( address->FindProperty(KMsgStorePropertyDisplayName, index) )
		{
		aAddress.iDisplayName.Create( address->PropertyValueDesL(index) );
		}
	
	CleanupStack::PopAndDestroy( address );
	
	} // end PropertyValueAddressL

// ==========================================================================
// FUNCTION: PropertyValueBoolL
// ==========================================================================
EXPORT_C TBool CMsgStorePropertyContainer::PropertyValueBoolL( TUint aPropertyIndex ) const
	{
	ValidateIndexAndTypeL( aPropertyIndex, EMsgStoreTypeBool );

	TPckgBuf<TBool> boolValue;

	TPtrC8 des8( iProperties[aPropertyIndex]->iValue.Ptr(), iProperties[aPropertyIndex]->iValue.Length() );
	
	boolValue.Copy( des8 );
	
	return boolValue();
	} // end PropertyValueBoolL

// ==========================================================================
// FUNCTION: PropertyValueContainerL
// ==========================================================================
EXPORT_C CMsgStorePropertyContainer* CMsgStorePropertyContainer::PropertyValueContainerL( TUint aPropertyIndex ) const
	{
	return PropertyValueContainerL( aPropertyIndex, EMsgStoreTypeContainer );
	} // end PropertyValueContainerL

// ==========================================================================
// FUNCTION: RemovePropertyL
// ==========================================================================
EXPORT_C void CMsgStorePropertyContainer::RemovePropertyL( TUint aPropertyIndex )
	{
	ValidateIndexL( aPropertyIndex );

    delete iProperties[aPropertyIndex];
	iProperties.Remove( aPropertyIndex );
	} // end RemovePropertyL

// ==========================================================================
// FUNCTION: FindProperty
// ==========================================================================
EXPORT_C TBool CMsgStorePropertyContainer::FindProperty( const TDesC8& aName, TUint& aFoundIndex, TUint aStartIndex ) const
	{
	TInt  index = aStartIndex;
	TBool found = EFalse;
	while( (index < iProperties.Count()) && !found )
		{
		if( iProperties[index]->iName.Compare( aName ) == 0 )
			{
			aFoundIndex = index;
			found       = ETrue;
			}
		else
			{
			index++;
			}
		} // end while
		
	return found;
	} // end FindProperty
	

// ==========================================================================
// FUNCTION: AddPropertyL
// ==========================================================================
TUint CMsgStorePropertyContainer::AddPropertyL( const TDesC8& aName, const CMsgStorePropertyContainer& aContainer, TMsgStorePropertyValueType aType )
	{
	TMsgStoreProperty* newNode = new(ELeave) TMsgStoreProperty;
	CleanupStack::PushL( newNode );
	
	newNode->iName.CreateL( aName );
    aContainer.SerializeL( newNode->iValue );	
	newNode->iType = aType;

	ValidateLengthsL( aName.Length(), newNode->iValue.Length() );
		
	iProperties.AppendL( newNode );
	
	CleanupStack::Pop( newNode );
	
	return iProperties.Count() - 1;
	} // end AddPropertyL
	
// ==========================================================================
// FUNCTION: UpdatePropertyL
// ==========================================================================
void CMsgStorePropertyContainer::UpdatePropertyL( TUint aPropertyIndex, const CMsgStorePropertyContainer& aContainer, TMsgStorePropertyValueType aType )
	{
	ValidateIndexAndTypeL( aPropertyIndex, aType );

    iProperties[aPropertyIndex]->iValue.Close();
    
    RBuf8 serializedBuf;
    CleanupClosePushL( serializedBuf );
    
    aContainer.SerializeL( serializedBuf );
    
    ValidateValueLengthL( serializedBuf.Length() );
    
    CleanupStack::Pop( &serializedBuf );
    
    iProperties[aPropertyIndex]->iValue.Swap( serializedBuf );
    iProperties[aPropertyIndex]->iType = aType;
	} // end UpdatePropertyL

// ==========================================================================
// FUNCTION: PropertyValueContainerL
// ==========================================================================
CMsgStorePropertyContainer* CMsgStorePropertyContainer::PropertyValueContainerL( TUint aPropertyIndex, TMsgStorePropertyValueType aType ) const
	{
	ValidateIndexAndTypeL( aPropertyIndex, aType );
	
	CMsgStorePropertyContainer* container = CMsgStorePropertyContainer::NewL();
	CleanupStack::PushL( container );
	
	container->DeserializeL( iProperties[aPropertyIndex]->iValue );

	CleanupStack::Pop( container );

	return container;	
	} // end PropertyValueContainerL

// ==========================================================================
// FUNCTION: SetIds
// ==========================================================================
void CMsgStorePropertyContainer::SetIds( TMsgStoreId aId, TMsgStoreId aParentId )
	{
	iId       = aId;
	iParentId = aParentId;
	} // end SetIds
	
// ==========================================================================
// FUNCTION: ValidateIndexL
// ==========================================================================
void CMsgStorePropertyContainer::ValidateIndexL( TUint aIndex ) const
    {
    if( aIndex >= iProperties.Count() )
        {
    	__LOG_STATIC_ENTER( "MsgClient", "ValidateIndexL" )
    	__LOG_WRITE_ERROR( "invalid index" )
	    __LOG_STATIC_EXIT
        User::Leave( KErrNotFound );
        } // end if
      
    } // end ValidateIndexL
    
// ==========================================================================
// FUNCTION: ValidateIndexAndTypeL
// ==========================================================================
void CMsgStorePropertyContainer::ValidateIndexAndTypeL( TUint aIndex, TMsgStorePropertyValueType aType ) const
    {
    ValidateIndexL( aIndex );
    
    if( aType != iProperties[aIndex]->iType )
        {
    	__LOG_STATIC_ENTER( "MsgClient", "ValidateIndexAndTypeL" )
        __LOG_WRITE8_FORMAT2_ERROR( "Invalid type (expected=%i actual=%i)", iProperties[aIndex]->iType, aType  )
	    __LOG_STATIC_EXIT
        User::Leave( KErrArgument );
        } // end if
        
    }  // end ValidateIndexAndTypeL   

// ==========================================================================
// FUNCTION: ValidateLengthsL
// ==========================================================================
void CMsgStorePropertyContainer::ValidateLengthsL( TUint aNameLength, TUint aValueLength ) const
    {
    if( aNameLength == 0 )
        {
        __LOG_STATIC_ENTER( "MsgClient", "ValidateLengthsL" )    
        __LOG_WRITE_ERROR( "underflow" )
        __LOG_STATIC_EXIT
        User::Leave( KErrUnderflow );
        }
    else if( aNameLength > KMaxTUint8 )
        {
        __LOG_STATIC_ENTER( "MsgClient", "ValidateLengthsL" )    
        __LOG_WRITE_ERROR( "overflow" )
        __LOG_STATIC_EXIT
        User::Leave( KErrOverflow );        
        }
    else
        {
        ValidateValueLengthL( aValueLength );
        } // end if
        
    } // end ValidateLengthsL
    
// ==========================================================================
// FUNCTION: ValidateValueLengthL
// ==========================================================================
void CMsgStorePropertyContainer::ValidateValueLengthL( TUint /*aValueLength*/ ) const
    {    
    // There is no longer a limit on value lengths.
        
    } // ValidateValueLengthL 

// ==========================================================================
// FUNCTION: SerializeL
// ==========================================================================
void CMsgStorePropertyContainer::SerializeL( RBuf8& aBuffer ) const
	{
	TInt totalLength = TPropertiesSerializer::EFixedOverhead;
	
	TInt index;
	for( index = 0; index < iProperties.Count(); index++ )
		{		
		const TMsgStoreProperty* property = iProperties[index];
		
		totalLength += (property->iName.Length() + property->iValue.Length() + TPropertiesSerializer::EPerNodeOverhead);
		} // end for

	aBuffer.CreateL( totalLength );

	TPropertiesSerializer serializer( aBuffer );
	
	for( index = 0; index < iProperties.Count(); index++ )
		{	
		const TMsgStoreProperty* property = iProperties[index];
		
		serializer.AddPropertyL( property->iName, property->iType, property->iValue );
		} // end for
	} // end SerializeL

// ==========================================================================
// FUNCTION: DeserializeL
// ==========================================================================
void CMsgStorePropertyContainer::DeserializeL( const TDesC8& aSerializedBuffer )
	{
	TPropertiesDeserializer deserializer( aSerializedBuffer );
	
    TBool moreProperties = deserializer.First();
    
	while( moreProperties )
		{
        TMsgStoreProperty* property = new(ELeave) TMsgStoreProperty;
        CleanupStack::PushL( property );
        
        property->iName.CreateL( deserializer.Name() );
        property->iValue.CreateL( deserializer.Value() );
        property->iType = static_cast<TMsgStorePropertyValueType>(deserializer.Type());
        
		if( property->iType == EMsgStoreTypeDes )
		    {
        	const TUint16* valuePtr16 = reinterpret_cast<const TUint16*>( property->iValue.Ptr() );

        	property->iValue16.Set( valuePtr16, property->iValue.Length() / 2 );
		    } // end if
		
		iProperties.AppendL( property );
		
		CleanupStack::Pop( property );
        
        moreProperties = deserializer.Next();
		
		} // end while				
	} // end DeserializeL

