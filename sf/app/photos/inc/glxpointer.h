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
* Description:   Smart pointer
*
*/



#ifndef T_GLXPOINTER_H
#define T_GLXPOINTER_H

#include "glxpanic.h"

/**
 *  TGlxPointer
 *
 *  Smart pointer class. Template that can house any object
 *
 *  @lib None
 */
template <class T>
class TGlxPointer {
private:
	T*			pointer;
 	const void*	iOwns;		// Defined as void* instead of TBool so that can use <pointer> directly as boolean value

public:
	/** 
	 * Default constructor
	 */
	TGlxPointer() {
		pointer = NULL;
		iOwns = 0;
	}

	/** 
	 * Constructor T*
	 */
	explicit TGlxPointer(T* pointer) {
		this->pointer = pointer;
		iOwns = pointer;
	}

	/** 
	 * Constructor
	 */
	TGlxPointer(const TGlxPointer<T>& ptrObj) { // Should not be const, but without this, ARM compiler refuses
		iOwns = ptrObj.iOwns;
		pointer = const_cast<TGlxPointer<T>&>(ptrObj).Release(); 
	}

	/** 
	 * Destructor
	 */
	~TGlxPointer() {
		if (iOwns) {
			__ASSERT_DEBUG(pointer != NULL, Panic(EGlxPanicIllegalState));
			delete pointer; 
		}
	}

	/** 
	 * Operator =
	 */
	TGlxPointer<T>& operator=(TGlxPointer<T>& ptrObj) {
		if (this != &ptrObj) {
			if (iOwns && pointer != ptrObj.pointer) {
				delete this->pointer;
			}
			iOwns = ptrObj.iOwns; 
			pointer = ptrObj.Release(); 
		}
		return *this; 
	}

	/** 
	 * Operator =
	 */
	TGlxPointer<T>& operator=(T* pointer) {
		if (iOwns && pointer != this->pointer) {
			delete this->pointer;
		}
		iOwns = pointer;
		this->pointer = pointer;
		return *this; 
	}
 
	/** 
	 * operator*
	 */
	inline T& operator*() const {
		__ASSERT_DEBUG(pointer != NULL, Panic(EGlxPanicNullPointer));
		return *pointer; 
	}

	/** 
	 * operator ->
	 */
	inline T* operator->() const {
		__ASSERT_DEBUG(pointer != NULL, Panic(EGlxPanicNullPointer));
		return pointer; 
	}

	/** 
	 * Ptr
	 */
	inline T* Ptr() const {
		return pointer; 
	}

	/** 
	 * Release
	 * Return the pointer and release ownership
	 */
	T* Release() {
		iOwns = 0;
		return pointer; 
	}

	/** 
	 * Owns
	 * Returns ETrue if the object owns the pointer
	 */
	inline TBool Owns() const {
		return (TBool)iOwns;
	}

	/** 
	 * PtrRef
	 * Return a reference to the pointer 
	 */
	inline T*& PtrRef() {
		__ASSERT_DEBUG(pointer != NULL, Panic(EGlxPanicNullPointer));
		return pointer; 
	}
};

#endif // T_GLXPOINTER_H
