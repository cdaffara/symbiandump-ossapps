/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message store property collection.
*
*/



#ifndef __MSG_STORE_PROPERTY_CONTAINER_H__
#define __MSG_STORE_PROPERTY_CONTAINER_H__

#include <e32base.h>
//<cmail>
#include "MsgStoreTypes.h"
#include "MsgStoreAddress.h"
//</cmail>

/**
 * Internal property value storage
 */
struct TMsgStoreProperty
{
	RBuf8                      iName;
	RBuf8                      iValue;
	TPtrC16                    iValue16;
	TMsgStorePropertyValueType iType;
	
	inline ~TMsgStoreProperty() {iName.Close(); iValue.Close();}

}; // end TMsgStoreProperty

/** This class defines a collection of name/value pairs.

	This class represents a collection of properties (i.e. name/value pairs) and provides functions
	to add/update/remove/find the properties.
	
	The maximum length for property names is 255 characters.  Functions will leave with KErrUnderflow if
	a zero length property name is given, and will leave with KErrOverflow if the property name is too
	long.
	
	The maximum length for property values is 65535 bytes.  Functions will leave with KErrOverflow if the
	property value is too long.	
*/
class CMsgStorePropertyContainer : public CBase
	{
	public:

		/** Creates a property container. */
		IMPORT_C static CMsgStorePropertyContainer* NewL();
		
		virtual ~CMsgStorePropertyContainer();
	
		/** Returns the ID of the container in storage (KMsgStoreInvalid if not yet stored). */
		IMPORT_C TMsgStoreId Id() const;
	
		/** Returns the parent ID of the container in storage (KMsgStoreInvalid if not yet stored). */
		IMPORT_C TMsgStoreId ParentId() const;

		/** Returns the total number of properties. */
		IMPORT_C TUint PropertyCount() const;
		
		/** Returns the name of the given property.
		
			\note: This function will leave with KErrNotFound if an invalid property index is provided.
		*/
		IMPORT_C const TDesC8& PropertyNameL( TUint aPropertyIndex ) const;
		
		/** Returns the type of value of the given property.
		
			\note: This function will leave with KErrNotFound if an invalid property index is provided.
		*/		
		IMPORT_C TMsgStorePropertyValueType PropertyTypeL( TUint aPropertyIndex ) const;
		
		/** Returns the value of the given property as a boolean.
		
			\note: This function will leave with KErrNotFound if an invalid property index is provided.			
			\note: This function will leave with KErrArgument if the property value type doesn't match.
		*/
		IMPORT_C TBool PropertyValueBoolL( TUint aPropertyIndex ) const;
		
		/** Returns the value of the given property as an 8-bit descriptor.
		
			\note: This function will leave with KErrNotFound if an invalid property index is provided.			
			\note: This function will leave with KErrArgument if the property value type doesn't match.
		*/
		IMPORT_C const TDesC8& PropertyValueDes8L( TUint aPropertyIndex ) const;
		
		/** Returns the value of the given property as a 16-bit descriptor.

			\note: This function will leave with KErrNotFound if an invalid property index is provided.			
			\note: This function will leave with KErrArgument if the property value type doesn't match.
		*/		
		IMPORT_C const TDesC& PropertyValueDesL( TUint aPropertyIndex ) const;
		
		/** Returns the value of the given property as an integer. 
		
			\note: This function will leave with KErrNotFound if an invalid property index is provided.			
			\note: This function will leave with KErrArgument if the property value type doesn't match.
		*/
		IMPORT_C TUint32 PropertyValueUint32L( TUint aPropertyIndex ) const;
		
		/** Returns the value of the given property as an TTime. 
		
			\note: This function will leave with KErrNotFound if an invalid property index is provided.			
			\note: This function will leave with KErrArgument if the property value type doesn't match.
		*/
		IMPORT_C void PropertyValueTimeL( TUint aPropertyIndex, TTime& aTime ) const;
		
		/** Returns the value of the given property as an Address. 
		
			\note: This function will leave with KErrNotFound if an invalid property index is provided.			
			\note: This function will leave with KErrArgument if the property value type doesn't match.
		*/
		IMPORT_C void PropertyValueAddressL( TUint aPropertyIndex, RMsgStoreAddress& aAddress ) const;
		
		/** Returns the value of the given property as an property container. 
		
			The caller is responsible for deallocating the returned object.
		
			\note: This function will leave with KErrNotFound if an invalid property index is provided.			
			\note: This function will leave with KErrArgument if the property value type doesn't match.
		*/
		IMPORT_C CMsgStorePropertyContainer* PropertyValueContainerL( TUint aPropertyIndex ) const;
		
		/** Adds a boolean property. 

			\retval Returns the index of the added property.
		*/
		IMPORT_C TUint AddPropertyL( const TDesC8& aName, TBool aValue );
		
		/** Adds a integer property. 

			\retval Returns the index of the added property.
		*/
		IMPORT_C TUint AddPropertyL( const TDesC8& aName, TUint32 aValue );
		
		/** Adds an 8-bit descriptor property.
		
			\retval Returns the index of the added property.
		*/	
		IMPORT_C TUint AddPropertyL( const TDesC8& aName, const TDesC8& aValue );
	
		/** Adds a 16-bit descriptor property.
		
			\retval Returns the index of the added property.
		*/	
		IMPORT_C TUint AddPropertyL( const TDesC8& aName, const TDesC& aValue );
		
		/** Adds a TTime property.
		
			\retval Returns the index of the added property.
		*/	
		IMPORT_C TUint AddPropertyL( const TDesC8& aName, const TTime& aValue );
		
		/** Adds a Address property.
		
			\retval Returns the index of the added property.
		*/	
		IMPORT_C TUint AddPropertyL( const TDesC8& aName, const RMsgStoreAddress& aValue );
	
		/** Adds a container property.
		
			\retval Returns the index of the added property.
		*/	
		IMPORT_C TUint AddPropertyL( const TDesC8& aName, const CMsgStorePropertyContainer& aContainer );
	
		/** Updates an 8-bit descriptor property.
		
			\note: This function will leave with KErrNotFound if an invalid property index is provided.						
			\note: This function will leave with KErrArgument if the property value type doesn't match.
		*/
		IMPORT_C void UpdatePropertyL( TUint aPropertyIndex, const TDesC8& aValue );
	
		/** Updates a 16-bit descriptor property.
		
			\note: This function will leave with KErrNotFound if an invalid property index is provided.						
			\note: This function will leave with KErrArgument if the property value type doesn't match.
		*/
		IMPORT_C void UpdatePropertyL( TUint aPropertyIndex, const TDesC& aValue );
	
		/** Updates a integer property.
		
			\note: This function will leave with KErrNotFound if an invalid property index is provided.						
			\note: This function will leave with KErrArgument if the property value type doesn't match.
		*/
		IMPORT_C void UpdatePropertyL( TUint aPropertyIndex, TUint32 aValue );
	
		/** Updates a boolean property.
		
			\note: This function will leave with KErrNotFound if an invalid property index is provided.						
			\note: This function will leave with KErrArgument if the property value type doesn't match.
		*/
		IMPORT_C void UpdatePropertyL( TUint aPropertyIndex, TBool aValue );
		
		/** Updates a TTime property.
		
			\note: This function will leave with KErrNotFound if an invalid property index is provided.						
			\note: This function will leave with KErrArgument if the property value type doesn't match.
		*/
		IMPORT_C void UpdatePropertyL( TUint aPropertyIndex, const TTime& aValue );
		
		/** Updates a Address property.
		
			\note: This function will leave with KErrNotFound if an invalid property index is provided.						
			\note: This function will leave with KErrArgument if the property value type doesn't match.
		*/
		IMPORT_C void UpdatePropertyL( TUint aPropertyIndex, const RMsgStoreAddress& aValue );
	
		/** Updates a container property.
		
			\note: This function will leave with KErrNotFound if an invalid property index is provided.						
			\note: This function will leave with KErrArgument if the property value type doesn't match.
		*/
		IMPORT_C void UpdatePropertyL( TUint aPropertyIndex, const CMsgStorePropertyContainer& aContainer );
	
		/** Adds or updates a boolean property. 

			If a property with the given name already exists then the first instance of the property will
			be updated.  If not, the property will be added.
		
			\note: This function will leave with KErrArgument if the property already exists and the
			       value type doesn't match.
			
			\retval Returns the index of the added or updated property.
		*/
		IMPORT_C TUint AddOrUpdatePropertyL( const TDesC8& aName, TBool aValue );
		
		/** Adds or updates a integer property. 

			If a property with the given name already exists then the first instance of the property will
			be updated.  If not, the property will be added.
		
			\note: This function will leave with KErrArgument if the property already exists and the
			       value type doesn't match.
			
			\retval Returns the index of the added or updated property.
		*/
		IMPORT_C TUint AddOrUpdatePropertyL( const TDesC8& aName, TUint32 aValue );
		
		/** Adds or updates an 8-bit descriptor property.
		
			If a property with the given name already exists then the first instance of the property will
			be updated.  If not, the property will be added.
		
			\note: This function will leave with KErrArgument if the property already exists and the
			       value type doesn't match.
			
			\retval Returns the index of the added or updated property.
		*/	
		IMPORT_C TUint AddOrUpdatePropertyL( const TDesC8& aName, const TDesC8& aValue );
	
		/** Adds or updates a 16-bit descriptor property.
		
			If a property with the given name already exists then the first instance of the property will
			be updated.  If not, the property will be added.
		
			\note: This function will leave with KErrArgument if the property already exists and the
			       value type doesn't match.
			
			\retval Returns the index of the added or updated property.
		*/	
		IMPORT_C TUint AddOrUpdatePropertyL( const TDesC8& aName, const TDesC& aValue );
		
		/** Adds or updates a TTime property.
		
			If a property with the given name already exists then the first instance of the property will
			be updated.  If not, the property will be added.
		
			\note: This function will leave with KErrArgument if the property already exists and the
			       value type doesn't match.
			
			\retval Returns the index of the added or updated property.
		*/	
		IMPORT_C TUint AddOrUpdatePropertyL( const TDesC8& aName, const TTime& aValue );
	
		/** Adds or updates an Address property.
		
			If a property with the given name already exists then the first instance of the property will
			be updated.  If not, the property will be added.
		
			\note: This function will leave with KErrArgument if the property already exists and the
			       value type doesn't match.
			
			\retval Returns the index of the added or updated property.
		*/	
		IMPORT_C TUint AddOrUpdatePropertyL( const TDesC8& aName, const RMsgStoreAddress& aValue );
		
		/** Adds or updates a container property.
		
			If a property with the given name already exists then the first instance of the property will
			be updated.  If not, the property will be added.
		
			\note: This function will leave with KErrArgument if the property already exists and the
			       value type doesn't match.
			
			\retval Returns the index of the added or updated property.
		*/	
		IMPORT_C TUint AddOrUpdatePropertyL( const TDesC8& aName, const CMsgStorePropertyContainer& aContainer );
	
		/** Removes a property.
		
			\note: This function will leave with KErrNotFound if an invalid property index is provided.			
		*/
		IMPORT_C void RemovePropertyL( TUint aPropertyIndex );

		/** Finds a property based on name, starting at the given index.
		
			\retval Returns ETrue if the property was found, EFalse otherwise.
		*/		
		IMPORT_C TBool FindProperty( const TDesC8& aName, TUint& aFoundIndex, TUint aStartIndex = 0 ) const;

		// ---------------------------
		// INTERNAL USE (NOT EXPORTED)
		// ---------------------------
		
		void SerializeL( RBuf8& aBuffer ) const;
		
		void DeserializeL( const TDesC8& aBuf );
		
		void SetIds( TMsgStoreId aId, TMsgStoreId aParentId );

	protected:
		
		CMsgStorePropertyContainer();

        void ValidateIndexL( TUint aIndex ) const;

		TMsgStoreId                      iId;
		TMsgStoreId                      iParentId;		
        RPointerArray<TMsgStoreProperty> iProperties;
        
    private:
    	TUint AddPropertyL( const TDesC8& aName, const CMsgStorePropertyContainer& aContainer, TMsgStorePropertyValueType aType );
    	void UpdatePropertyL( TUint aPropertyIndex, const CMsgStorePropertyContainer& aContainer, TMsgStorePropertyValueType aType );
    	CMsgStorePropertyContainer* CMsgStorePropertyContainer::PropertyValueContainerL( TUint aPropertyIndex, TMsgStorePropertyValueType aType ) const;   
    	
        void ValidateIndexAndTypeL( TUint aIndex, TMsgStorePropertyValueType aType ) const;
        void ValidateLengthsL( TUint aNameLength, TUint aValueLength ) const;
        void ValidateValueLengthL( TUint aValueLength ) const;
		
		TUint DoAddPropertyL( TMsgStorePropertyValueType aType, const TDesC8& aName, const TDesC8& aValue );
        void DoUpdatePropertyL( TUint aPropertyIndex, TMsgStorePropertyValueType aType, const TDesC8& aValue );
        
	}; // end class CMsgStorePropertyContainer
		

#endif  //__MSG_STORE_PROPERTY_CONTAINER_H__