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
#include <QImage>
#include <QList>

#include "cxeimagedataqueuesymbian.h"
#include "cxutils.h"


CxeImageDataQueueSymbian::CxeImageDataQueueSymbian()
{
    CX_DEBUG_IN_FUNCTION();
}

CxeImageDataQueueSymbian::~CxeImageDataQueueSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();

    // This will delete all remaining items!
    // qDeleteAll( mList ); // not owned. Ownership is with filesavethread
    mList.clear();

    CX_DEBUG_EXIT_FUNCTION();
}

int CxeImageDataQueueSymbian::size() const
{
    return mList.size();
}

void CxeImageDataQueueSymbian::clear()
{
    CX_DEBUG_ENTER_FUNCTION();

    // This will delete all remaining items!
    // qDeleteAll( mList );
    mList.clear();

    CX_DEBUG_EXIT_FUNCTION();
}

/*
void CxeImageDataQueueSymbian::deleteImage( int index )
    {
    CX_DEBUG_ENTER_FUNCTION();

    mList[index]->deleteImage();

    CX_DEBUG_EXIT_FUNCTION();
    }

void CxeImageDataQueueSymbian::renameImage( int index, const QString& path )
    {
    CX_DEBUG_ENTER_FUNCTION();

    TPtrC16 newPath (reinterpret_cast<const TUint16*>(path.utf16()));
    mList[index]->renameImage( newPath );

    CX_DEBUG_EXIT_FUNCTION();
    }
*/

CxeImageDataItem &CxeImageDataQueueSymbian::operator[]( int index )
{
    CX_DEBUG_ASSERT( index >= 0 && index < mList.count() );
    return *mList[index];
}

CxeImageDataItemSymbian *CxeImageDataQueueSymbian::startSave(QByteArray data, QString path, int id, bool addLocation)
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeImageDataItemSymbian* dataItem = new CxeImageDataItemSymbian(data, path, id, addLocation);
    mList.append(dataItem);

    CX_DEBUG_EXIT_FUNCTION();
    return dataItem;
}

