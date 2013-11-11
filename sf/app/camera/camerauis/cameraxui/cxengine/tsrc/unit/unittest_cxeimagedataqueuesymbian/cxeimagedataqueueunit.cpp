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

#include "cxutils.h"
#include "cxefakeimagedataitem.h"
#include "cxeimagedataqueueunit.h"

CxeImageDataQueueUnit::CxeImageDataQueueUnit()
{
    CX_DEBUG_IN_FUNCTION();
}

CxeImageDataQueueUnit::~CxeImageDataQueueUnit()
{
    CX_DEBUG_IN_FUNCTION();
}

CxeImageDataItem *CxeImageDataQueueUnit::createDataItem( int index, const QByteArray& data, const QString& path ) const
{
    static int id = 0;
    return new CxeFakeImageDataItem(index, data, path, id++, false);
}
