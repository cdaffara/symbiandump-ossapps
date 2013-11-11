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
#include <QImage>
#include <QList>

#include "cxeimagedataqueuedesktop.h"
#include "cxeimagedataitemdesktop.h"
#include "cxutils.h"


CxeImageDataQueueDesktop::CxeImageDataQueueDesktop()
{
    CX_DEBUG_IN_FUNCTION();
}

CxeImageDataQueueDesktop::~CxeImageDataQueueDesktop()
{
    CX_DEBUG_ENTER_FUNCTION();

    // This will delete all remaining items!
    clear();

    CX_DEBUG_EXIT_FUNCTION();
}

int CxeImageDataQueueDesktop::size() const
{
    return mList.size();
}

void CxeImageDataQueueDesktop::clear()
{
    CX_DEBUG_ENTER_FUNCTION();

    // This will delete all remaining items!
    mList.clear();

    CX_DEBUG_EXIT_FUNCTION();
}

CxeImageDataItem &CxeImageDataQueueDesktop::operator[](int index)
{
    CX_DEBUG_ASSERT( index >= 0 && index < mList.count() );
    return *mList[index];
}

CxeImageDataItem *CxeImageDataQueueDesktop::startSave(QByteArray data, QString path, int id)
{
    CX_DEBUG_ENTER_FUNCTION();
    
    int index = mList.size(); 
    CxeImageDataItem *dataItem = new CxeImageDataItemDesktop(index, data, path, id);
    mList.append(dataItem);
    
    CX_DEBUG_EXIT_FUNCTION();
    return dataItem;
}
