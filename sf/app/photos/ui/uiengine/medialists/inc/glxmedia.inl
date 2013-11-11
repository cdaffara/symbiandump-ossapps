/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Media item
*
*/




#ifndef C_GLXMEDIA_INL
#define C_GLXMEDIA_INL

// -----------------------------------------------------------------------------
// The attributes provided in this media object
// -----------------------------------------------------------------------------
//
inline const TArray<TMPXAttribute> CGlxMedia::Attributes() const
    {
    return iAttributes.Array();
    }

// -----------------------------------------------------------------------------
// Does this object contain the value for a given attribute
// -----------------------------------------------------------------------------
//
inline TBool CGlxMedia::IsSupported(const TMPXAttribute& aAttribute) const
    {
    return KErrNotFound != Index(aAttribute);
    }
    
// -----------------------------------------------------------------------------
// The number of attribute values provided in this media object 
// -----------------------------------------------------------------------------
//
inline TInt CGlxMedia::Count() const
    {
    return iAttributes.Count();
    }

// -----------------------------------------------------------------------------
// The attribute for a specific index
// -----------------------------------------------------------------------------
//
inline const TMPXAttribute& CGlxMedia::Attribute(TInt aIndex) const
    {
    return iAttributes[aIndex];
    }

// -----------------------------------------------------------------------------
// The index of a given attribute
// -----------------------------------------------------------------------------
//
inline TInt CGlxMedia::Index(const TMPXAttribute& aAttribute) const
    {
    return iAttributes.Find(aAttribute, TMPXAttribute::Match);
    }
  
// -----------------------------------------------------------------------------
// The value for a specific attribute
// -----------------------------------------------------------------------------
//
template <typename T>
inline T CGlxMedia::ValueTObject(const TMPXAttribute& aAttribute) const
    {
    TInt i = Index(aAttribute);

    __ASSERT_DEBUG( i != KErrNotFound, Panic( EGlxPanicIllegalArgument ) );

    __ASSERT_DEBUG(EMPXTypeTObject == iValues[i].iType,
                                        Panic(EGlxPanicWrongAttributeType));
    const TValue& value = iValues[i];
    const T* ptr = reinterpret_cast<const T*>(value.iValue);
    __ASSERT_DEBUG(ptr != NULL, Panic(EGlxPanicNullPointer));
    return *ptr;
    }   
  
template <typename T>
inline TBool CGlxMedia::GetValueTObject(T& aValue, const TMPXAttribute& aAttribute) const
    {
    TInt i = Index(aAttribute);
    if (KErrNotFound == i)
        {
        return EFalse;
        }

    __ASSERT_DEBUG(EMPXTypeTObject == iValues[i].iType,
                                        Panic(EGlxPanicWrongAttributeType));
    const TValue& value = iValues[i];
    const T* ptr = reinterpret_cast<const T*>(value.iValue);
    __ASSERT_DEBUG(ptr != NULL, Panic(EGlxPanicNullPointer));
    aValue = *ptr;
    return ETrue;
    }   

  
// -----------------------------------------------------------------------------
// Set the value for a specific attribute
// -----------------------------------------------------------------------------
//
template<typename T> 
inline void CGlxMedia::SetTObjectValueL(const TMPXAttribute& aAttribute, T aValue) 
	{
    TAny* ptr = User::AllocL(sizeof(T));
    memcpy(ptr, &aValue, sizeof(T));

    SetValueL(aAttribute, ptr, EMPXTypeTObject);
   	}

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxMedia::TValue::TValue(TAny* aValue, TMPXAttributeType aType)
	{
	iValue = aValue;
	iType = aType;
	}
	
#endif // C_GLXMEDIA_INL
