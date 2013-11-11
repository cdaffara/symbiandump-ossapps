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
* Description:   VideoProxyModelContent implementation
*
*/

// Version : 

// INCLUDE FILES
#include "videoproxymodeldata.h"
#include "videoproxymodelcontent.h"
#include "videolistdatamodel.h"
#include "videocollectiontrace.h"

// -----------------------------------------------------------------------------
// VideoProxyModelContent::VideoProxyModelGeneric
// -----------------------------------------------------------------------------
//
VideoProxyModelContent::VideoProxyModelContent(QObject *parent) :
VideoProxyModelGeneric(parent)
{
}

// -----------------------------------------------------------------------------
// VideoProxyModelContent::~VideoProxyModelContent
// -----------------------------------------------------------------------------
//
VideoProxyModelContent::~VideoProxyModelContent()
{

}

// -----------------------------------------------------------------------------
// VideoProxyModelContent::connectSignals
// -----------------------------------------------------------------------------
//
bool VideoProxyModelContent::connectSignals()
{
    return true;
}
   
// -----------------------------------------------------------------------------
// VideoProxyModelContent::disconnectSignals
// -----------------------------------------------------------------------------
//
void VideoProxyModelContent::disconnectSignals()
{

}

// -----------------------------------------------------------------------------
// VideoProxyModelContent::filterAcceptsRow
// -----------------------------------------------------------------------------
//
bool VideoProxyModelContent::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_row);
    Q_UNUSED(source_parent);
    return true;
}

// -----------------------------------------------------------------------------
// VideoProxyModelContent::getOpenItem()
// -----------------------------------------------------------------------------
//
TMPXItemId VideoProxyModelContent::getOpenItem() const
{
    return VideoProxyModelData::mOpenedItemId;
}

// -----------------------------------------------------------------------------
// VideoProxyModelContent::albumChangedSlot()
// -----------------------------------------------------------------------------
//
void VideoProxyModelContent::albumChangedSlot()
{

}

// End of file
