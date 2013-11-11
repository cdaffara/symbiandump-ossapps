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
#include "videoproxymodelcontent.h"
#include "videolistdatamodel.h"
#include "videocollectionclient.h"

int VideoProxyModelContent::mContentModelConstructCallCount = 0;

// -----------------------------------------------------------------------------
// VideoProxyModelContent::VideoProxyModelContent
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

// end of file
