#ifndef __CleanupPointerArray_H__
#define __CleanupPointerArray_H__/*
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
* Description:   A class for simplifying cleanup of RPointerArrays
*
*/



#include "e32base.h"

template <class T>
inline void CleanupPointerArrayPushL(T& aPtr);

/**
A class for simplifying the cleanup of RPointerArrays using ResetAndDestroy.
@internalTechnology
*/
template <class T>
class TCleanupPointerArray
	{
public: // Interface
	static void PushL(T& aRef);
private: // Methods
	static void ResetAndDestroy(TAny* aPtr);
	};

#include "CleanupPointerArray.inl"

#endif // __CleanupPointerArray_H__

// End of file.