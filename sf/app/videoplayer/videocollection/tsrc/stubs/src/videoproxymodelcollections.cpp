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
* Description: VideoProxyModelCollections implementation
*
*/

// Version : 

// INCLUDE FILES
#include <vcxmyvideosdefs.h>
#include "videoproxymodeldata.h"
#include "videoproxymodelcollections.h"
#include "videolistdatamodel.h"
#include "videocollectiontrace.h"

// -----------------------------------------------------------------------------
// VideoProxyModelCollections::VideoProxyModelCollections
// -----------------------------------------------------------------------------
//
VideoProxyModelCollections::VideoProxyModelCollections(QObject *parent) :
VideoProxyModelGeneric(parent)
{
	mDefaultSortRole = VideoCollectionCommon::KeyTitle;
}

// -----------------------------------------------------------------------------
// VideoProxyModelCollections::~VideoProxyModelCollections
// -----------------------------------------------------------------------------
//
VideoProxyModelCollections::~VideoProxyModelCollections()
{
	
}

// -----------------------------------------------------------------------------
// VideoProxyModelCollections::lessThan
// -----------------------------------------------------------------------------
//
bool VideoProxyModelCollections::lessThan(const QModelIndex &left,
    const QModelIndex &right) const
{
    Q_UNUSED(left);
    Q_UNUSED(right);
    return true;
}
// -----------------------------------------------------------------------------
// VideoProxyModelCollections::filterAcceptsRow
// -----------------------------------------------------------------------------
//
bool VideoProxyModelCollections::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent);
    Q_UNUSED(source_row);
    return true;
}

// End of file
