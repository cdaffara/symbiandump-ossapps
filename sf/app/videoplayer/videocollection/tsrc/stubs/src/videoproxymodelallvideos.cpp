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
* Description:   VideoProxyModelAllVideos implementation
*
*/

// Version : 

// INCLUDE FILES
#include <vcxmyvideosdefs.h>
#include "videoproxymodeldata.h"
#include "videoproxymodelallvideos.h"
#include "videolistdatamodel.h"
#include "videocollectiontrace.h"

// -----------------------------------------------------------------------------
// VideoProxyModelAllVideos::VideoProxyModelGeneric
// -----------------------------------------------------------------------------
//
VideoProxyModelAllVideos::VideoProxyModelAllVideos(QObject *parent) :
VideoProxyModelGeneric(parent)
{

}

// -----------------------------------------------------------------------------
// VideoProxyModelAllVideos::~VideoProxyModelAllVideos
// -----------------------------------------------------------------------------
//
VideoProxyModelAllVideos::~VideoProxyModelAllVideos()
{

}

// -----------------------------------------------------------------------------
// VideoProxyModelAllVideos::filterAcceptsRow
// -----------------------------------------------------------------------------
//
bool VideoProxyModelAllVideos::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_row);
    Q_UNUSED(source_parent);
    return true;
}

// -----------------------------------------------------------------------------
// VideoProxyModelAllVideos::getOpenItem()
// -----------------------------------------------------------------------------
//
TMPXItemId VideoProxyModelAllVideos::getOpenItem() const
{
    return VideoProxyModelData::mOpenedItemId;
}

// End of file
