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
* Description:   stub VideoProxyModelGeneric for VideoCollectionWrapperPrivate unit tests   
* 
*/

#include <qstringlist.h>
#include <qtimer.h>
#include <qdatetime.h>

#include "videocollectioncommon.h"
#include "videoproxymodelcollections.h"
#include "videolistdatamodel.h"
#include "videocollectionclient.h"

int VideoProxyModelCollections::mCollectionsModelConstructCallCount = 0;

// -----------------------------------------------------------------------------
// VideoProxyModelCollections::VideoProxyModelCollections
// -----------------------------------------------------------------------------
//
VideoProxyModelCollections::VideoProxyModelCollections(QObject *parent) : 
VideoProxyModelGeneric(parent)
{
}

// -----------------------------------------------------------------------------
// VideoProxyModelCollections::~VideoProxyModelCollections
// -----------------------------------------------------------------------------
//
VideoProxyModelCollections::~VideoProxyModelCollections() 
{
}

// end of file
