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
* Description:
*
*/

#include "cxesysutil.h"
#include "cxutils.h"

qint64 CxeSysUtil::mSpaceAvailable = 0;

 /**
     * Checks if free disk drive storage space is or will fall below critical
     * level. Static configuration values stored in Central Repository are
     * used to determine a critical level for each drive.
     *
     * If aBytesToWrite is more than zero, function returns false otherwise true.
     * By defining aBytesToWrite to zero it is possible to get fake fulldisk error.
     *
     */
bool CxeSysUtil::DiskSpaceBelowCriticalLevel(
        RFs* /*aFs*/,
        TInt aBytesToWrite,
        TInt /*aDrive*/ )
{
    CX_DEBUG_IN_FUNCTION();

    return (aBytesToWrite <= 0);
}

qint64 CxeSysUtil::spaceAvailable(
                     RFs & /*fs*/,
                     int /*index*/,
                     CxeSettings & /*settings*/)
{
    CX_DEBUG_IN_FUNCTION();
    return CxeSysUtil::mSpaceAvailable;
}

void CxeSysUtil::setSpaceAvailable(qint64 aSpaceAvailable)
{
    CX_DEBUG_IN_FUNCTION();
    mSpaceAvailable = aSpaceAvailable;
}

int CxeSysUtil::getCameraDrive(RFs & /*fs*/)
{
    CX_DEBUG_IN_FUNCTION();
    return 1;
}

// End of file
