/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CXESYSUTIL_H
#define CXESYSUTIL_H

#include <e32base.h>

class RFs;
class CxeSettings;


/**
 *  Helper class for file system queries.
 */
class CxeSysUtil
    {

public:

    static bool DiskSpaceBelowCriticalLevel(
        RFs* aFs,
        TInt aBytesToWrite,
        TInt aDrive );

    static int getCameraDrive(RFs &fs);

    static qint64 spaceAvailable(RFs &fs, int index, CxeSettings &settings);
    };

#endif // CXESYSUTIL_H
