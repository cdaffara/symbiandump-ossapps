/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides cleanup item to push ResetAndDestroy calls to 
 *                cleanup stack. It is heavily used for RPointerArrays,
 *                which are used all over Symbian's CalenInterimAPI.
 *                Implementation is more or less copy-paste of
 *                cleanup item of Close method and CleanupClosePushL 
 *
*/



#ifndef __CLEANUPRESETANDDESTROY_H__
#define __CLEANUPRESETANDDESTROY_H__

/**
 *  A utility class used by the templated function CleanupResetAndDestroyPushL() 
 *  to create a TCleanupItem item that will perform a close type operation on
 *  the class T type object.
 **/
template <class T>
class CleanupResetAndDestroy
{
 public:
    inline static void PushL(T& aRef);

 private:
    static void ResetAndDestroy(TAny *aPtr);

};

/**
 *  Constructs and pushes a TCleanupItem object onto the cleanup stack.
 *  
 *  The TCleanupItem encapsulates:
 *
 *  1. a reference aRef to the object of type class T which is to be cleaned up
 *  2. an associated cleanup operation.
 *  
 *  The cleanup operation is the private static function ResetAndDestroy() of 
 *  the templated class CleanupResetAndDestroy and is invoked as a result of 
 *  a subsequent call to CleanupStack::PopAndDestroy().
 *  
 *  CleanupResetAndDestroy::ResetAndDestroy() is passed a pointer to the 
 *  class T object to be cleaned up, and the function implements cleanup by 
 *  calling ResetAndDestroy() on the passed object.
 *  The class T object must, therefore, define and implement (or inherit) a 
 *  ResetAndDestroy() member function.
 *
 *  RPointerArray<HBufC> buffers;
 *  CleanupResetAndDestroyPushL( buffers );
 *  buffers.Append( HBufC::NewL(100) );
 *  ...
 *  CleanupStack::PopAndDestroy(); // <--- results in ResetAndDestroy() being called on "buffers".
 *  ...
 *  @endcode
 *  
 *  @param aRef A reference to a class T type object for which the cleanup item is being created.
 *  @see TCleanupItem
 *  @see CleanupResetAndDestroy
 *  @see CleanupStack::PopAndDestroy()
 **/
template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef);

#include "CleanupResetAndDestroy.inl"


#endif //__CLEANUPRESETANDDESTROY_H__

//  End of File
