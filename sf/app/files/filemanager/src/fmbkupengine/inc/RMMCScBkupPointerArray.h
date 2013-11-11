/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Declaration for RMMCScBkupPointerArray  
*     
*
*/

#ifndef __RMMCSCBKUPPOINTERARRAY_H__
#define __RMMCSCBKUPPOINTERARRAY_H__

// User includes
#include <e32cmn.h>


/**
*
*
* @since 3.0
*/
template <class T>
class RMMCScBkupPointerArray : public RPointerArray<T>
    {
public:
    inline RMMCScBkupPointerArray();
    inline explicit RMMCScBkupPointerArray(TInt aGranularity);
    inline void Close();
    };





// Inline methods

template <class T>
inline RMMCScBkupPointerArray<T>::RMMCScBkupPointerArray()
    : RPointerArray<T>()
    {}

template <class T>
inline RMMCScBkupPointerArray<T>::RMMCScBkupPointerArray(TInt aGranularity)
    : RPointerArray<T>( aGranularity )
    {}

template <class T>
inline void RMMCScBkupPointerArray<T>::Close()
    {
    RPointerArray<T>::ResetAndDestroy();
    RPointerArray<T>::Close();
    }






#endif // __RMMCSCBKUPPOINTERARRAY_H__

//End of File
