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



#ifndef T_GLXID_H
#define T_GLXID_H

#include "e32std.h"
#include "glxpanic.h"

/** Invalid or undefined id value */
// Use "id == KGlxIdNone" and "id != KGlxIdNone" to check for
// id validity.
class TGlxIdNone {};
const TGlxIdNone KGlxIdNone = TGlxIdNone();

/**
 *  TGlxId
 *
 *  Type-safe template for ids
 *  Id value of KMaxTUint is consided to be undefined.
 *  @usage Define like this:
 *            class TMyIdBase {}; 
 *            typedef TGlxId<TMyIdBase> TMyId;
 *            Now use can use TMyId in other code
 */
template <class T> 
class TGlxId : public T {
public:
    inline TGlxId(); 
    inline TGlxId(const TGlxId<T>& aId);
    inline TGlxId(TUint aIdValue); 
    inline TGlxId(const TGlxIdNone&);
    inline TGlxId<T>& operator=(const TGlxId<T>& aId);
    inline TGlxId<T>& operator=(TUint aIdValue);
        
    inline TGlxId<T>& operator=(const TGlxIdNone&);
    inline TGlxId<T> operator++(int); // Postfix, keep dummy parameter as int
    inline TBool operator!=(const TGlxId<T>& aId) const;
    inline TBool operator==(const TGlxId<T>& aId) const;
    inline TBool operator!=(const TGlxIdNone&) const;
    inline TBool operator==(const TGlxIdNone&) const;
    inline TBool operator>(const TGlxId<T>& aId) const;
    inline TBool operator<(const TGlxId<T>& aId) const;

    inline TUint Value() const;
    inline void SetValue(TUint aIdValue); // Allows setting undefined as value (KMaxTUint)

private:
    TUint iIdValue;
};

/**
 * MGlxIdProvider
 * Abstract interface class to provide ids 
 * Define as MGlxIdProvider<TMyId>
 */ 
template <class T>
class MGlxIdProvider {
public:
    /**
     * getNextId
     * Returns the next unique id.
     * Returns value in the argument, since otherwise cannot have the same
     * deriving class provide multiple different types of ids
     */
    virtual void NextId(T& aId) = 0;
};

/**
 * TGlxDefaultIdProvider
 * Simple implementation of id provider
 */ 
template <class T>
class TGlxDefaultIdProvider : public MGlxIdProvider<T> {
private:
    TUint iNextIdValue;
public:
    inline TGlxDefaultIdProvider();
    virtual void NextId(T& aId);
};

#include "glxid.inl"

#endif // T_GLXID_H
