/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Stub class for testing purposes
*
*/

#ifndef CXESYSUTIL_H
#define CXESYSUTIL_H

#include <e32base.h>
#include <QObject>

class RFs;
class CxeSettings;

class CxeSysUtil
    {

public:

    static bool DiskSpaceBelowCriticalLevel(
        RFs* aFs,
        TInt aBytesToWrite,
        TInt aDrive);

    static qint64 spaceAvailable(RFs &fs, int index, CxeSettings &settings);
    static void setSpaceAvailable(qint64 aSpaceAvailable);

    static int getCameraDrive(RFs &fs);

public:
    static qint64 mSpaceAvailable;
    };

#endif // CXESYSUTIL_H
