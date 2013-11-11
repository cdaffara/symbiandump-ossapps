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
* Description:   Type-safe unique id template
*
*/



#include "glxpanic.h"

const TUint KGlxInvalidIdValue = KMaxTUint;

// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
//
template <class T> 
inline TGlxId<T>::TGlxId() 
    { 
    iIdValue = KGlxInvalidIdValue; 
    }
    
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
template <class T> 
inline TGlxId<T>::TGlxId(const TGlxId<T>& aId) 
    { 
    iIdValue = aId.iIdValue; 
    }
    
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
template <class T> 
inline TGlxId<T>::TGlxId(TUint aIdValue) 
    { 
    __ASSERT_DEBUG(aIdValue != KGlxInvalidIdValue, Panic(EGlxPanicIllegalArgument)); 
    iIdValue = aIdValue; 
    }
        
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
template <class T> 
inline TGlxId<T>::TGlxId(const TGlxIdNone&) 
    { 
    iIdValue = KGlxInvalidIdValue; 
    }
    
// -----------------------------------------------------------------------------
// Assignment, checks for invalid value
// -----------------------------------------------------------------------------
//
template <class T> 
inline TGlxId<T>& TGlxId<T>::operator=(const TGlxId<T>& aId) 
    { 
    __ASSERT_DEBUG(aId.iIdValue != KGlxInvalidIdValue, Panic(EGlxPanicIllegalArgument)); // Use SetValue to set undefined value
    iIdValue = aId.iIdValue; 
    return *this; 
    }
        
// -----------------------------------------------------------------------------
// Assignment, checks for invalid value
// -----------------------------------------------------------------------------
//
template <class T> 
inline TGlxId<T>& TGlxId<T>::operator=(TUint aIdValue) 
    { 
    __ASSERT_DEBUG(aIdValue != KGlxInvalidIdValue, Panic(EGlxPanicIllegalArgument)); 
    iIdValue = aIdValue; 
    return *this; 
    }
        
// -----------------------------------------------------------------------------
// Assignment 
// -----------------------------------------------------------------------------
//
template <class T> 
inline TGlxId<T>& TGlxId<T>::operator=(const TGlxIdNone&) 
    { 
    iIdValue = KGlxInvalidIdValue;  
    return *this; 
    }

// -----------------------------------------------------------------------------
// Post-increment
// -----------------------------------------------------------------------------
//
template <class T> 
inline TGlxId<T> TGlxId<T>::operator++(int) 
    { 
    TUint value = iIdValue;
    iIdValue++; 
    return TGlxId<T>(value); 
    }

// -----------------------------------------------------------------------------
// Comparison
// -----------------------------------------------------------------------------
//
template <class T> 
inline TBool TGlxId<T>::operator!=(const TGlxId<T>& aId) const 
    { 
    return iIdValue != aId.iIdValue; 
    }
    
// -----------------------------------------------------------------------------
// Comparison
// -----------------------------------------------------------------------------
//
template <class T> 
inline TBool TGlxId<T>::operator==(const TGlxId<T>& aId) const 
    { 
    return iIdValue == aId.iIdValue; 
    }
    
// -----------------------------------------------------------------------------
// Comparison
// -----------------------------------------------------------------------------
//
template <class T> 
inline TBool TGlxId<T>::operator!=(const TGlxIdNone&) const 
    { 
    return iIdValue != KGlxInvalidIdValue; 
    }
    
// -----------------------------------------------------------------------------
// Comparison
// -----------------------------------------------------------------------------
//
template <class T> 
inline TBool TGlxId<T>::operator==(const TGlxIdNone&) const 
    { 
    return iIdValue == KGlxInvalidIdValue; 
    }
    
// -----------------------------------------------------------------------------
// Comparison
// -----------------------------------------------------------------------------
//
template <class T> 
inline TBool TGlxId<T>::operator>(const TGlxId<T>& aId) const 
    { 
    return iIdValue > aId.iIdValue; 
    }
    
// -----------------------------------------------------------------------------
// Comparison
// -----------------------------------------------------------------------------
//
template <class T> 
inline TBool TGlxId<T>::operator<(const TGlxId<T>& aId) const 
    { 
    return iIdValue < aId.iIdValue; 
    }
    
// -----------------------------------------------------------------------------
// Returns current value 
// -----------------------------------------------------------------------------
//
template <class T> 
inline TUint TGlxId<T>::Value() const 
    { 
    return iIdValue; 
    }
    
// -----------------------------------------------------------------------------
// Allows setting undefined as value
// -----------------------------------------------------------------------------
//
template <class T> 
inline void TGlxId<T>::SetValue(TUint aIdValue) 
    { 
    iIdValue = aIdValue; 
    }

/**
 * TGlxDefaultIdProvider
 * Simple implementation of id provider
 */ 
// -----------------------------------------------------------------------------
// Allows setting undefined as value
// -----------------------------------------------------------------------------
//
template <class T>
TGlxDefaultIdProvider<T>::TGlxDefaultIdProvider() 
    { 
    iNextIdValue = 0; 
    }
    
// -----------------------------------------------------------------------------
// Allows setting undefined as value
// -----------------------------------------------------------------------------
//
template <class T>
void TGlxDefaultIdProvider<T>::NextId(T& aId) 
    { 
    aId = iNextIdValue++; 
    }

