/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef CXEFAKESYSUTIL_H
#define CXEFAKESYSUTIL_H

#include <e32base.h>

class RFs;

class CxeFakeSysUtil
    {

public:

    /**
     * Checks if free disk drive storage space is or will fall below critical
     * level. Static configuration values stored in Central Repository are
     * used to determine a critical level for each drive.
     * 
     * If aBytesToWrite is more than zero, function returns false otherwise true.
     * By defining aBytesToWrite to zero it is possible to get fake fulldisk error.
     *  
     */
    static TBool DiskSpaceBelowCriticalLevel(
        RFs* aFs,
        TInt aBytesToWrite,
        TInt aDrive);
    };

#endif // CXEFAKESYSUTIL_H
