/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Common utility methods.
*
*/


#ifndef PSUTILS_H
#define PSUTILS_H

#include <f32file.h>
#include <pathinfo.h>

#include "psdebug.h"

/**
 * Static class that provides common utility methods.
 */
class PSUtils
    {

public:

    /**
     * Gets the full file name and path to the preset database file.
     *
     * @param   aFullName       On return, contains the full file name and path
     *                          to the database file.
     */
    inline static void GetDatabaseFullNameL( TFileName& aFullName );

    };
    
#include "psutils.inl"

#endif // PSUTILS_H
