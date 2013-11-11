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
* Description:  Templated algorithms to simplify object handling
*
*/

#ifndef PRESENCECACHEOBJECTHELPERS_H
#define PRESENCECACHEOBJECTHELPERS_H

#include <s32mem.h>


const TInt KObjectBufGranurality = 100;




/**
 * Templated object packer.
 *
 * Object type must support: NewLC(),
 * ExternalizeL() and InternalizeL().
 *
 * @since S60 v3.2
 */
template< class T >
class TCacheObjectPacker
    {
 public:

    /**
     * Templated method to pack object.
     *
     * Packs given object data to flat buffer and
     * returns buffer to caller.
     *
     * @param aObj Templated object to pack.
     *             Object type must support ExternalizeL() method.
     *
     * @return Buffer containing externalized object data.
     *         Buffer ownership is returned to caller.
     */
    static inline HBufC8* PackL( const T& aObj );



    /**
     * Templated method to unpack object.
     *
     * Extracts object data from given flat buffer and
     * returns objects ownership to caller.
     *
     * @param aObj Object which data to internalize from package.
     *             Concrete object type must support InternalizeL() method.
     *
     * @Param aPack Buffer containing object data.
     */
    static void UnPackL( T& aObj,
                         const TDesC8& aPack );

private:   //Prohibited constructor
    TCacheObjectPacker();

    };


#include "cacheobjecthelpers.inl"


#endif // PRESENCECACHEOBJECTHELPERS_H
