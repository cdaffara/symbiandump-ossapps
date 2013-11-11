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
#include "cxeimagedataitemunit.h"
#include "cxeimagedataitemsymbian.h"
#include "cxutils.h"
#include "cxefakesysutil.h"

CxeImageDataItemUnit::CxeImageDataItemUnit(int index, QByteArray data, QString filename, bool addLocation)
    : CxeImageDataItemSymbian(data, filename, index, addLocation)
{
    CX_DEBUG_IN_FUNCTION();
}

CxeImageDataItemUnit::~CxeImageDataItemUnit()
{
    CX_DEBUG_IN_FUNCTION();
}

bool CxeImageDataItemUnit::checkDiskSpace(RFs* aFs,
                                          TInt aBytesToWrite,
                                          TInt aDrive)
{
    CX_DEBUG_IN_FUNCTION();
    return CxeFakeSysUtil::DiskSpaceBelowCriticalLevel(aFs, aBytesToWrite, aDrive);
}

// end of file
